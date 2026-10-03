/*
 * hle_d3d8.c -- render the title's Direct3D 8 calls through the PC's d3d8.dll.
 *
 * Clone Wars links Microsoft's Xbox Direct3D 8 into its D3D section and drives
 * the NV2A through it. Every Direct3D entry point the title calls is wrapped
 * here. The wrapper always runs the title's own implementation first (the
 * generated body, renamed sub_X_gen by tools/apply_manual.py), so the Xbox
 * side keeps doing its own bookkeeping -- resource headers, lock pointers,
 * push-buffer fences -- exactly as before. When the DX8 renderer is enabled,
 * the wrapper then mirrors the call onto a real IDirect3DDevice8.
 *
 * Mirroring rather than replacing means the renderer can grow one feature at a
 * time: whatever it does not handle yet is simply not drawn, and the title
 * never sees a difference in behaviour.
 *
 * The renderer exists only in the 32-bit build (Windows ships d3d8.dll as
 * 32-bit only) and only when CW_D3D8=1. The x64 build compiles every wrapper
 * as a plain pass-through.
 *
 * Addresses and argument counts come from XbSymbolDatabase's scan of this XBE
 * (XDK 5233), checked against each function's `ret n`.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "recomp/gen/recomp_types.h"
#include "recomp/gen/recomp_funcs.h"
#include "xbox_memory_layout.h"

#if defined(_M_IX86) && defined(CW_HAVE_D3D8)
#define HLE_D3D8_AVAILABLE 1
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d8.h>
#include <mmsystem.h>
#else
#define HLE_D3D8_AVAILABLE 0
#endif

/* ---- switch and logging ----------------------------------------------- */

static int hle_on(void)
{
#if HLE_D3D8_AVAILABLE
    static int state = -1;
    if (state < 0) {
        /* On by default; CW_D3D8=0 falls back to the NV2A push-buffer path. */
        const char *v = getenv("CW_D3D8");
        state = (v && *v == '0') ? 0 : 1;
    }
    return state;
#else
    return 0;
#endif
}

static void d3d_log(const char *fmt, ...)
{
    static int budget = 600;
    va_list ap;
    if (budget <= 0)
        return;
    budget--;
    va_start(ap, fmt);
    fprintf(stderr, "[D3D8] ");
    vfprintf(stderr, fmt, ap);
    fputc('\n', stderr);
    va_end(ap);
    fflush(stderr);
}

/* stdcall arguments as the wrapper sees them on entry: esp -> return address. */
#define ARG(n) MEM32(g_esp + 4 + 4 * (n))

/*
 * Wrapper body. Each wrapper is written out as
 *
 *     void sub_X_gen(void);
 *     void sub_X(void) { D3D_WRAP_BODY(sub_X_gen, NARGS, MIRROR) }
 *
 * so tools/apply_manual.py can see both names in the text. Arguments are
 * captured before the title's implementation runs (it pops them); the mirror
 * step then runs with those copies.
 */
#define D3D_WRAP_BODY(GEN, NARGS, MIRROR)                    \
    uint32_t a[8] = {0};                                     \
    uint32_t in_ecx = g_ecx, in_edx = g_edx;                 \
    int _i;                                                  \
    (void)in_ecx; (void)in_edx;                              \
    for (_i = 0; _i < (NARGS) && _i < 8; _i++)               \
        a[_i] = ARG(_i);                                     \
    GEN();                                                   \
    if (hle_on()) { MIRROR; }

#if HLE_D3D8_AVAILABLE

/* ---- device ------------------------------------------------------------ */

static IDirect3D8       *s_d3d;
static IDirect3DDevice8 *s_dev;
static HWND              s_hwnd;
static int               s_failed;
static unsigned          s_frames;

static LRESULT CALLBACK hle_wndproc(HWND h, UINT m, WPARAM w, LPARAM l)
{
    if (m == WM_CLOSE) {
        /* Not ExitProcess: that waits on locks the guest's other threads
         * (sound, loading) may hold, and the game then never closed. The
         * title saves at its own save points; nothing is pending here. */
        fflush(stdout);
        fflush(stderr);
        TerminateProcess(GetCurrentProcess(), 0);
        return 0;
    }
    return DefWindowProcA(h, m, w, l);
}

/* ---- display settings -------------------------------------------------- */

/* CloneWars.ini beside the exe, [Video]:
 *   Width, Height  render and window size (0 = the monitor's)
 *   Fullscreen     1 = borderless window covering the monitor
 *   Widescreen     1 = the 3D view widens to fill a wide screen (more is seen
 *                  at the sides, nothing is stretched); 0 = 4:3 with bars
 *   FXAA           1 = smooth jagged edges after each frame
 * The game draws for 640x480. 3D goes through the projection and viewport and
 * simply renders at the new size; screen-space 2D (HUD, menus, text) is scaled
 * up and kept 4:3, centred -- except full-width pieces (fades, cutscene bars),
 * which are stretched across. Environment overrides for testing: CW_RES=WxH,
 * CW_FULLSCREEN, CW_WIDESCREEN, CW_FXAA. */
static int   s_bbw = 640, s_bbh = 480, s_fullscreen, s_widescreen = 1, s_fxaa = 1;
static float s_hs = 1.0f, s_hx0, s_hy0;      /* 2D: x' = s_hx0 + x * s_hs */
static float s_k = 1.0f;                      /* full-screen 3D: clip x *= s_k */

static void load_settings(void)
{
    char ini[MAX_PATH], *slash;
    const char *e;
    GetModuleFileNameA(NULL, ini, MAX_PATH);
    slash = strrchr(ini, '\\');
    if (slash) slash[1] = 0;
    strncat(ini, "CloneWars.ini", MAX_PATH - strlen(ini) - 1);
    if (GetFileAttributesA(ini) == INVALID_FILE_ATTRIBUTES) {
        FILE *f = fopen(ini, "w");
        if (f) {
            fputs("; Star Wars: The Clone Wars -- display settings\n"
                  "[Video]\n"
                  "; Render and window size. 0 = the monitor's resolution.\n"
                  "Width=1280\n"
                  "Height=720\n"
                  "; 1 = borderless window covering the whole monitor.\n"
                  "Fullscreen=0\n"
                  "; 1 = the 3D view widens to fill a wide screen; 0 = original 4:3 picture with side bars.\n"
                  "Widescreen=1\n"
                  "; 1 = smooth jagged edges (FXAA).\n"
                  "FXAA=1\n", f);
            fclose(f);
        }
    }
    s_bbw = (int)GetPrivateProfileIntA("Video", "Width", 1280, ini);
    s_bbh = (int)GetPrivateProfileIntA("Video", "Height", 720, ini);
    s_fullscreen = (int)GetPrivateProfileIntA("Video", "Fullscreen", 0, ini);
    s_widescreen = (int)GetPrivateProfileIntA("Video", "Widescreen", 1, ini);
    s_fxaa = (int)GetPrivateProfileIntA("Video", "FXAA", 1, ini);
    if ((e = getenv("CW_RES")) && *e) sscanf(e, "%dx%d", &s_bbw, &s_bbh);
    if ((e = getenv("CW_FULLSCREEN")) && *e) s_fullscreen = atoi(e);
    if ((e = getenv("CW_WIDESCREEN")) && *e) s_widescreen = atoi(e);
    if ((e = getenv("CW_FXAA")) && *e) s_fxaa = atoi(e);
    if (s_bbw <= 0 || s_bbh <= 0) {
        s_bbw = GetSystemMetrics(SM_CXSCREEN);
        s_bbh = GetSystemMetrics(SM_CYSCREEN);
    }
    if (s_bbw < 320) s_bbw = 320;
    if (s_bbh < 240) s_bbh = 240;
    if (s_bbw > 7680) s_bbw = 7680;
    if (s_bbh > 4320) s_bbh = 4320;
    s_hs = (float)s_bbw / 640.0f < (float)s_bbh / 480.0f ? (float)s_bbw / 640.0f : (float)s_bbh / 480.0f;
    s_hx0 = ((float)s_bbw - 640.0f * s_hs) * 0.5f;
    s_hy0 = ((float)s_bbh - 480.0f * s_hs) * 0.5f;
    s_k = 1.0f;
    if (s_widescreen && (float)s_bbw * 3.0f > (float)s_bbh * 4.0f)
        s_k = ((float)s_bbh * 4.0f) / ((float)s_bbw * 3.0f);
}

static void pump_messages(void)
{
    MSG msg;
    while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

/* The frame is drawn into s_scene (the size in CloneWars.ini) and copied to
 * the back buffer at Present, through FXAA when it is on. */
static IDirect3DTexture8 *s_scene_tex, *s_luma_tex;
static IDirect3DSurface8 *s_scene_surf, *s_scene_ds, *s_luma_surf;
static DWORD s_ps_luma, s_ps_fxdir, s_ps_fxaa;
static int   s_hor_c = -1;                    /* PC vertex constant holding the widescreen factor */
static float s_k_cur;                         /* factor for the current viewport (set in vp_apply) */
static void  vp_apply(void);
static void  post_init(void);

/* Created on first use, on the thread that renders. */
static int ensure_device(void)
{
    D3DPRESENT_PARAMETERS pp;
    HRESULT hr;
    RECT r;
    DWORD style;
    int x = CW_USEDEFAULT, y = CW_USEDEFAULT;

    if (s_dev)
        return 1;
    if (s_failed)
        return 0;

    load_settings();
    {
        WNDCLASSA wc;
        memset(&wc, 0, sizeof(wc));
        wc.lpfnWndProc   = hle_wndproc;
        wc.hInstance     = GetModuleHandleA(NULL);
        wc.hCursor       = LoadCursorA(NULL, (LPCSTR)IDC_ARROW);
        wc.lpszClassName = "CloneWarsDX8";
        RegisterClassA(&wc);
    }
    if (s_fullscreen) {
        /* Borderless, covering the monitor; the picture is scaled to it. */
        style = WS_POPUP | WS_VISIBLE;
        r.left = 0; r.top = 0;
        r.right = GetSystemMetrics(SM_CXSCREEN); r.bottom = GetSystemMetrics(SM_CYSCREEN);
        x = 0; y = 0;
    } else {
        RECT wa;
        style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
        r.left = 0; r.top = 0; r.right = s_bbw; r.bottom = s_bbh;
        AdjustWindowRect(&r, style, FALSE);
        if (SystemParametersInfoA(SPI_GETWORKAREA, 0, &wa, 0)) {
            int ww = r.right - r.left, wh = r.bottom - r.top;
            if (ww > wa.right - wa.left || wh > wa.bottom - wa.top) {
                /* Larger than the desktop: fit the window, keep the render size. */
                float f = (float)(wa.right - wa.left) / ww;
                if ((float)(wa.bottom - wa.top) / wh < f) f = (float)(wa.bottom - wa.top) / wh;
                r.right = r.left + (int)(ww * f); r.bottom = r.top + (int)(wh * f);
            }
            x = wa.left + ((wa.right - wa.left) - (r.right - r.left)) / 2;
            y = wa.top + ((wa.bottom - wa.top) - (r.bottom - r.top)) / 2;
        }
    }
    s_hwnd = CreateWindowExA(0, "CloneWarsDX8", "Star Wars: The Clone Wars",
                             style, x, y, r.right - r.left, r.bottom - r.top, NULL, NULL,
                             GetModuleHandleA(NULL), NULL);
    if (!s_hwnd) {
        d3d_log("CreateWindow failed (%lu)", GetLastError());
        s_failed = 1;
        return 0;
    }

    s_d3d = Direct3DCreate8(D3D_SDK_VERSION);
    if (!s_d3d) {
        d3d_log("Direct3DCreate8 failed");
        s_failed = 1;
        return 0;
    }

    memset(&pp, 0, sizeof(pp));
    pp.Windowed               = TRUE;
    pp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
    pp.BackBufferWidth        = (UINT)s_bbw;
    pp.BackBufferHeight       = (UINT)s_bbh;
    pp.BackBufferFormat       = D3DFMT_X8R8G8B8;
    pp.BackBufferCount        = 1;
    pp.EnableAutoDepthStencil = TRUE;
    pp.AutoDepthStencilFormat = D3DFMT_D24S8;
    pp.hDeviceWindow          = s_hwnd;

    hr = IDirect3D8_CreateDevice(s_d3d, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, s_hwnd,
                                 D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
                                 &pp, &s_dev);
    if (FAILED(hr))
        hr = IDirect3D8_CreateDevice(s_d3d, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, s_hwnd,
                                     D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
                                     &pp, &s_dev);
    if (FAILED(hr)) {
        d3d_log("CreateDevice failed hr=0x%08lX", (unsigned long)hr);
        s_failed = 1;
        return 0;
    }
    d3d_log("device created (%dx%d %s, widescreen %s, FXAA %s)", s_bbw, s_bbh,
            s_fullscreen ? "borderless fullscreen" : "windowed", s_k < 1.0f ? "on" : "off", s_fxaa ? "on" : "off");
    {
        D3DCAPS8 caps;
        if (SUCCEEDED(IDirect3DDevice8_GetDeviceCaps(s_dev, &caps))) {
            extern int hle_vs_consts_set(int);
            hle_vs_consts_set((int)caps.MaxVertexShaderConst);
            if (caps.MaxVertexShaderConst >= 193)
                s_hor_c = (int)caps.MaxVertexShaderConst - 1;
            d3d_log("vertex shader version %lX, %lu constants; pixel shader version %lX",
                    (unsigned long)caps.VertexShaderVersion, (unsigned long)caps.MaxVertexShaderConst,
                    (unsigned long)caps.PixelShaderVersion);
            if (s_fxaa && (caps.PixelShaderVersion & 0xFFFF) < 0x0104) {
                d3d_log("FXAA needs pixel shader 1.4 -- off");
                s_fxaa = 0;
            }
        }
    }
    post_init();
    vp_apply();
    IDirect3DDevice8_BeginScene(s_dev);
    return 1;
}

/* ---- guest memory helpers ---------------------------------------------- */

extern uint32_t xbox_ContiguousAllocatedBytes(void);

/* A resource's Data field holds a physical address. Physical memory the
 * runtime handed out as contiguous lives in the window at 0x80000000; the
 * rest is ordinary RAM at the same number. Same rule as nv2a_pb_exec.c. */
static uint32_t phys_to_va(uint32_t p)
{
    if (p >= 0x80000000u)
        return p;
    if (p < xbox_ContiguousAllocatedBytes())
        return 0x80000000u + p;
    return p;
}

static const void *gptr(uint32_t va) { return (const void *)XBOX_PTR(va); }

/* ---- captured pipeline state ------------------------------------------- */

static uint32_t s_tex[4];               /* guest D3DBaseTexture* per stage   */
static uint32_t s_stream_vb[16], s_stream_stride[16];
static uint32_t s_ib_base;
static uint32_t s_vshader;              /* FVF code, or handle with bit 0 set */
static uint32_t s_pshader;
static unsigned s_draws, s_draws_done, s_draws_skipped_vs;

/* Render targets. The title renders shadow maps and other off-screen passes by
 * pointing D3D at another surface (D3D_CommonSetRenderTarget); the mirror has
 * one back buffer, so draws and clears aimed elsewhere are skipped. Targets
 * that were current at a Swap are the ones the screen shows. */
static uint32_t s_cur_rt, s_cur_z, s_shown_rt[4];
static int      s_nshown;
static unsigned s_offscreen_draws;

static int rt_offscreen(void)
{
    int i;
    if (!s_cur_rt || !s_nshown)
        return 0;
    for (i = 0; i < s_nshown; i++)
        if (s_shown_rt[i] == s_cur_rt)
            return 0;
    return 1;
}

static void rt_note_shown(void)
{
    int i;
    if (!s_cur_rt)
        return;
    for (i = 0; i < s_nshown; i++)
        if (s_shown_rt[i] == s_cur_rt)
            return;
    if (s_nshown < 4)
        s_shown_rt[s_nshown++] = s_cur_rt;
}

/* ---- textures ---------------------------------------------------------- */

#include "../../xboxrecomp/src/d3d/d3d8_swizzle.h"

typedef struct {
    uint32_t data, format, size, sample;
    IDirect3DTexture8 *tex;
} tex_entry;

#define TEX_CACHE 512
static tex_entry s_texcache[TEX_CACHE];
static int       s_texnext;

/* A cheap content fingerprint, so a texture the title rewrote in place is
 * uploaded again. */
static uint32_t tex_sample(uint32_t va, uint32_t bytes)
{
    uint32_t h = 2166136261u, i, step = bytes / 64u;
    const uint8_t *p = (const uint8_t *)gptr(va);
    if (!step) step = 4;
    for (i = 0; i + 4 <= bytes; i += step)
        h = (h ^ *(const uint32_t *)(p + (i & ~3u))) * 16777619u;
    return h;
}

/* Expand one texel of an Xbox colour format to A8R8G8B8. */
static uint32_t texel_argb(uint32_t fmt, const uint8_t *s)
{
    uint32_t v, r, g, b, a;
    switch (fmt) {
    case 0x06: case 0x12:                             /* A8R8G8B8 */
        return *(const uint32_t *)s;
    case 0x07: case 0x1E:                             /* X8R8G8B8 */
        return *(const uint32_t *)s | 0xFF000000u;
    case 0x3A: case 0x3F:                             /* A8B8G8R8 */
        v = *(const uint32_t *)s;
        return (v & 0xFF00FF00u) | ((v >> 16) & 0xFFu) | ((v & 0xFFu) << 16);
    case 0x05: case 0x11:                             /* R5G6B5 */
        v = *(const uint16_t *)s;
        r = (v >> 11) & 31; g = (v >> 5) & 63; b = v & 31;
        return 0xFF000000u | ((r << 3 | r >> 2) << 16) | ((g << 2 | g >> 4) << 8) | (b << 3 | b >> 2);
    case 0x02: case 0x10: case 0x03: case 0x1C:       /* A1R5G5B5 / X1R5G5B5 */
        v = *(const uint16_t *)s;
        r = (v >> 10) & 31; g = (v >> 5) & 31; b = v & 31;
        a = (fmt == 0x02 || fmt == 0x10) ? ((v & 0x8000) ? 0xFF : 0) : 0xFF;
        return (a << 24) | ((r << 3 | r >> 2) << 16) | ((g << 3 | g >> 2) << 8) | (b << 3 | b >> 2);
    case 0x04: case 0x1D:                             /* A4R4G4B4 */
        v = *(const uint16_t *)s;
        a = (v >> 12) & 15; r = (v >> 8) & 15; g = (v >> 4) & 15; b = v & 15;
        return (a * 17u) << 24 | (r * 17u) << 16 | (g * 17u) << 8 | (b * 17u);
    case 0x00: case 0x13:                             /* L8 */
        v = s[0]; return 0xFF000000u | v << 16 | v << 8 | v;
    case 0x19: case 0x1F:                             /* A8 */
        return (uint32_t)s[0] << 24 | 0x00FFFFFFu;
    case 0x01: case 0x1B:                             /* AL8 */
        v = s[0]; return v << 24 | v << 16 | v << 8 | v;
    case 0x1A: case 0x20:                             /* A8L8 */
        v = s[0]; return (uint32_t)s[1] << 24 | v << 16 | v << 8 | v;
    default:
        return 0xFFFF00FFu;
    }
}

static uint32_t fmt_bpp(uint32_t fmt)
{
    switch (fmt) {
    case 0x06: case 0x12: case 0x07: case 0x1E: case 0x3A: case 0x3F: return 4;
    case 0x05: case 0x11: case 0x02: case 0x10: case 0x03: case 0x1C:
    case 0x04: case 0x1D: case 0x1A: case 0x20: return 2;
    case 0x00: case 0x13: case 0x19: case 0x1F: case 0x01: case 0x1B: return 1;
    default: return 0;
    }
}

/* Linear (unswizzled) colour formats: the LIN_ variants. */
static int fmt_is_linear(uint32_t fmt)
{
    switch (fmt) {
    case 0x10: case 0x11: case 0x12: case 0x13: case 0x1B: case 0x1C:
    case 0x1D: case 0x1E: case 0x1F: case 0x20: case 0x3F:
        return 1;
    default:
        return 0;
    }
}

/* Host texture for a guest D3DPixelContainer: Common, Data, Lock, Format, Size.
 * Top mip level only for now. */
static IDirect3DTexture8 *get_texture(uint32_t res)
{
    uint32_t data, format, size, fmt, w, h, pitch, va, bytes, sample, levels;
    D3DFORMAT hostfmt = D3DFMT_A8R8G8B8;
    IDirect3DTexture8 *tex = NULL;
    D3DLOCKED_RECT lr;
    int i, dxt = 0;

    if (!res)
        return NULL;
    data   = MEM32(res + 4);
    format = MEM32(res + 0xC);
    size   = MEM32(res + 0x10);
    fmt    = (format >> 8) & 0xFF;
    if (size) {
        w = (size & 0xFFF) + 1;
        h = ((size >> 12) & 0xFFF) + 1;
        pitch = (((size >> 24) & 0xFF) + 1) * 64;
    } else {
        w = 1u << ((format >> 20) & 0xF);
        h = 1u << ((format >> 24) & 0xF);
        pitch = 0;
    }
    if (!data || w > 4096 || h > 4096)
        return NULL;
    va = phys_to_va(data);

    if (fmt >= 0x2A && fmt <= 0x31) {
        {
            static uint32_t seen[16]; static int n; int k;
            for (k = 0; k < n; k++) if (seen[k] == res) break;
            if (k == n && n < 16 && getenv("CW_RT_LOG")) {
                seen[n++] = res;
                {
                    /* dump as 8-bit grayscale PGM (high byte of each 16-bit texel) */
                    char fn[64]; FILE *pf; uint32_t tw = 1u << ((format >> 20) & 0xF), th = 1u << ((format >> 24) & 0xF);
                    uint16_t *lin16 = (uint16_t *)malloc(tw * th * 2);
                    snprintf(fn, sizeof fn, "depthtex_%08X.pgm", res);
                    if (lin16 && tw <= 1024 && th <= 1024 && (pf = fopen(fn, "wb")) != NULL) {
                        uint32_t i;
                        xbox_unswizzle_rect((uint8_t *)lin16, (const uint8_t *)gptr(va), tw, th, 2);
                        fprintf(pf, "P5 %u %u 255\n", tw, th);
                        for (i = 0; i < tw * th; i++) fputc(lin16[i] >> 8, pf);
                        fclose(pf);
                    }
                    free(lin16);
                }
                fprintf(stderr, "[DEPTHTEX] res %08X data %08X format %08X size %08X | rt data %08X z data %08X\n",
                        res, data, format, size, s_cur_rt ? MEM32(s_cur_rt + 4) : 0, s_cur_z ? MEM32(s_cur_z + 4) : 0);
            }
        }
        /* Depth formats (D24S8, F24S8, D16, F16 and linear forms): shadow
         * maps, rendered off-screen and not mirrored. Sample as white with
         * full alpha, i.e. "not in shadow". */
        static IDirect3DTexture8 *white;
        if (!white && SUCCEEDED(IDirect3DDevice8_CreateTexture(s_dev, 1, 1, 1, 0, D3DFMT_A8R8G8B8,
                                                               D3DPOOL_MANAGED, &white))) {
            if (SUCCEEDED(IDirect3DTexture8_LockRect(white, 0, &lr, NULL, 0))) {
                *(uint32_t *)lr.pBits = 0xFFFFFFFFu;
                IDirect3DTexture8_UnlockRect(white, 0);
            }
        }
        return white;
    }
    if (fmt == 0x0C) { hostfmt = D3DFMT_DXT1; dxt = 8; }
    else if (fmt == 0x0E) { hostfmt = D3DFMT_DXT3; dxt = 16; }
    else if (fmt == 0x0F) { hostfmt = D3DFMT_DXT5; dxt = 16; }
    else if (!fmt_bpp(fmt)) {
        static int warned;
        if (warned++ < 20)
            d3d_log("texture format 0x%02X not handled (%ux%u)", fmt, w, h);
        return NULL;
    }
    /* Mip levels (format bits 16-19); linear (pitched) surfaces have one. */
    levels = (format >> 16) & 0xF;
    if (!levels || size || fmt_is_linear(fmt)) levels = 1;
    bytes = dxt ? ((w + 3) / 4) * ((h + 3) / 4) * dxt
                : (pitch ? pitch * h : w * h * fmt_bpp(fmt));
    sample = tex_sample(va, bytes);

    for (i = 0; i < TEX_CACHE; i++) {
        tex_entry *e = &s_texcache[i];
        if (e->tex && e->data == data && e->format == format && e->size == size) {
            if (e->sample == sample)
                return e->tex;
            tex = e->tex;                 /* same texture, new contents */
            e->sample = sample;
            break;
        }
    }
    if (!tex) {
        tex_entry *e = &s_texcache[s_texnext];
        s_texnext = (s_texnext + 1) % TEX_CACHE;
        if (e->tex)
            IDirect3DTexture8_Release(e->tex);
        memset(e, 0, sizeof(*e));
        if (FAILED(IDirect3DDevice8_CreateTexture(s_dev, w, h, levels, 0, hostfmt,
                                                  D3DPOOL_MANAGED, &tex)))
            return NULL;
        e->data = data; e->format = format; e->size = size; e->sample = sample; e->tex = tex;
    }

    /* Swizzled and DXT textures carry their mip chain right after level 0,
     * each level half the size (DXT: whole 4x4 blocks). */
    {
        uint32_t lvl, lva = va;
        for (lvl = 0; lvl < levels; lvl++) {
            uint32_t lw = w >> lvl ? w >> lvl : 1, lh = h >> lvl ? h >> lvl : 1;
            if (FAILED(IDirect3DTexture8_LockRect(tex, lvl, &lr, NULL, 0)))
                break;
            if (dxt) {
                uint32_t rows = (lh + 3) / 4, rowbytes = ((lw + 3) / 4) * dxt, y;
                for (y = 0; y < rows; y++)
                    memcpy((uint8_t *)lr.pBits + y * lr.Pitch,
                           (const uint8_t *)gptr(lva) + y * rowbytes, rowbytes);
                lva += rows * rowbytes;
            } else {
                uint32_t bpp = fmt_bpp(fmt), x, y, lpitch = pitch;
                const uint8_t *src = (const uint8_t *)gptr(lva);
                uint8_t *lin = NULL;
                if (!fmt_is_linear(fmt)) {
                    lin = (uint8_t *)malloc(lw * lh * bpp);
                    if (lin) {
                        xbox_unswizzle_rect(lin, src, lw, lh, bpp);
                        src = lin;
                    }
                    lpitch = lw * bpp;
                } else if (!lpitch) {
                    lpitch = lw * bpp;
                }
                for (y = 0; y < lh; y++) {
                    uint32_t *drow = (uint32_t *)((uint8_t *)lr.pBits + y * lr.Pitch);
                    for (x = 0; x < lw; x++)
                        drow[x] = texel_argb(fmt, src + y * lpitch + x * bpp);
                }
                free(lin);
                lva += lw * lh * bpp;
            }
            IDirect3DTexture8_UnlockRect(tex, lvl);
        }
    }
    return tex;
}

/* ---- draws ------------------------------------------------------------- */

/* Xbox primitive types: 1 points, 2 lines, 3 line loop, 4 line strip,
 * 5 triangles, 6 strip, 7 fan, 8 quads, 9 quad strip, 10 polygon. */
static int prim_convert(uint32_t xprim, uint32_t nverts, D3DPRIMITIVETYPE *pt, UINT *count)
{
    switch (xprim) {
    case 1:  *pt = D3DPT_POINTLIST;     *count = nverts;         break;
    case 2:  *pt = D3DPT_LINELIST;      *count = nverts / 2;     break;
    case 3:
    case 4:  *pt = D3DPT_LINESTRIP;     *count = nverts ? nverts - 1 : 0; break;
    case 5:  *pt = D3DPT_TRIANGLELIST;  *count = nverts / 3;     break;
    case 6:
    case 9:  *pt = D3DPT_TRIANGLESTRIP; *count = nverts >= 3 ? nverts - 2 : 0; break;
    case 7:
    case 10: *pt = D3DPT_TRIANGLEFAN;   *count = nverts >= 3 ? nverts - 2 : 0; break;
    case 8:  *pt = D3DPT_TRIANGLELIST;  *count = (nverts / 4) * 2; break;
    default: return 0;
    }
    return *count > 0;
}

/*
 * Render and texture-stage state, read from the Xbox D3D's own state arrays at
 * draw time. D3D_g_RenderState (0x002E4250) holds every render state by the
 * XDK 5233 enum; the indices below were pinned against a dump of the array
 * (defaults such as ZFUNC 0x203 = LESSEQUAL, SRCBLEND 0x302 = SRCALPHA,
 * POINTSIZE_MAX 64.0 land where the enum puts them; the complex block starts
 * with PSTEXTUREMODES at 136, so ZENABLE is 143). D3D_g_DeferredTextureState
 * (0x002E4050) holds 4 stages x 32 texture-stage states.
 */
/* Simple render states (57..91) also arrive through
 * D3DDevice_SetRenderState_Simple(method, value) called straight from game
 * code, which leaves D3D_g_RenderState untouched -- the title turns colour
 * writes off that way for depth-only passes. Those writes are recorded here
 * (sub_002D3690 below) with the array value they shadowed, and win until the
 * array itself changes again. */
static uint32_t s_simple_val[35], s_simple_arr[35];
static uint8_t  s_simple_set[35];

static uint32_t xrs_read(int i)
{
    uint32_t arr = MEM32(0x002E4250u + 4u * (uint32_t)i);
    if (i >= 57 && i < 57 + 35 && s_simple_set[i - 57]) {
        if (arr == s_simple_arr[i - 57])
            return s_simple_val[i - 57];
        s_simple_set[i - 57] = 0;
    }
    return arr;
}
#define XRS(i) xrs_read(i)
#define XTSS(st, i) MEM32(0x002E4050u + 128u * (st) + 4u * (i))

enum {
    XRS_ZFUNC = 57, XRS_ALPHAFUNC, XRS_ALPHABLENDENABLE, XRS_ALPHATESTENABLE, XRS_ALPHAREF,
    XRS_SRCBLEND, XRS_DESTBLEND, XRS_ZWRITEENABLE, XRS_DITHERENABLE, XRS_SHADEMODE,
    XRS_COLORWRITEENABLE, XRS_STENCILZFAIL, XRS_STENCILPASS, XRS_STENCILFUNC, XRS_STENCILREF,
    XRS_STENCILMASK, XRS_STENCILWRITEMASK, XRS_BLENDOP, XRS_BLENDCOLOR,
    XRS_FOGENABLE = 92, XRS_FOGTABLEMODE, XRS_FOGSTART, XRS_FOGEND, XRS_FOGDENSITY,
    XRS_FOGCOLOR = 138, XRS_FILLMODE, XRS_BACKFILLMODE,
    XRS_ZENABLE = 143, XRS_STENCILENABLE, XRS_STENCILFAIL, XRS_FRONTFACE, XRS_CULLMODE,
    XRS_TEXTUREFACTOR, XRS_ZBIAS
};

enum {
    XTSS_ADDRESSU = 0, XTSS_ADDRESSV, XTSS_ADDRESSW, XTSS_MAGFILTER, XTSS_MINFILTER, XTSS_MIPFILTER,
    XTSS_MIPMAPLODBIAS, XTSS_MAXMIPLEVEL, XTSS_MAXANISOTROPY, XTSS_COLORKEYOP, XTSS_COLORSIGN,
    XTSS_ALPHAKILL, XTSS_COLOROP, XTSS_COLORARG0, XTSS_COLORARG1, XTSS_COLORARG2, XTSS_ALPHAOP,
    XTSS_ALPHAARG0, XTSS_ALPHAARG1, XTSS_ALPHAARG2, XTSS_RESULTARG, XTSS_TEXTURETRANSFORMFLAGS,
    XTSS_BUMPENVMAT00, XTSS_BUMPENVMAT01, XTSS_BUMPENVMAT11, XTSS_BUMPENVMAT10,
    XTSS_BUMPENVLSCALE, XTSS_BUMPENVLOFFSET, XTSS_TEXCOORDINDEX, XTSS_BORDERCOLOR
};

static DWORD x_cmp(uint32_t v) { return (v >= 0x200 && v <= 0x207) ? (DWORD)(v - 0x200 + 1) : D3DCMP_ALWAYS; }

static DWORD x_blend(uint32_t v)
{
    switch (v) {
    case 0: return D3DBLEND_ZERO;
    case 1: return D3DBLEND_ONE;
    case 0x300: return D3DBLEND_SRCCOLOR;
    case 0x301: return D3DBLEND_INVSRCCOLOR;
    case 0x302: return D3DBLEND_SRCALPHA;
    case 0x303: return D3DBLEND_INVSRCALPHA;
    case 0x304: return D3DBLEND_DESTALPHA;
    case 0x305: return D3DBLEND_INVDESTALPHA;
    case 0x306: return D3DBLEND_DESTCOLOR;
    case 0x307: return D3DBLEND_INVDESTCOLOR;
    case 0x308: return D3DBLEND_SRCALPHASAT;
    default: return D3DBLEND_ONE;          /* constant colour/alpha: no D3D8 equivalent */
    }
}

static DWORD x_blendop(uint32_t v)
{
    switch (v) {
    case 0x800A: return D3DBLENDOP_SUBTRACT;
    case 0x800B: return D3DBLENDOP_REVSUBTRACT;
    case 0x8007: return D3DBLENDOP_MIN;
    case 0x8008: return D3DBLENDOP_MAX;
    default: return D3DBLENDOP_ADD;
    }
}

static DWORD x_stencilop(uint32_t v)
{
    switch (v) {
    case 0: return D3DSTENCILOP_ZERO;
    case 0x1E01: return D3DSTENCILOP_REPLACE;
    case 0x1E02: return D3DSTENCILOP_INCRSAT;
    case 0x1E03: return D3DSTENCILOP_DECRSAT;
    case 0x150A: return D3DSTENCILOP_INVERT;
    case 0x8507: return D3DSTENCILOP_INCR;
    case 0x8508: return D3DSTENCILOP_DECR;
    default: return D3DSTENCILOP_KEEP;
    }
}

static DWORD x_top(uint32_t v)
{
    static const DWORD map[] = {
        0, D3DTOP_DISABLE, D3DTOP_SELECTARG1, D3DTOP_SELECTARG2, D3DTOP_MODULATE, D3DTOP_MODULATE2X,
        D3DTOP_MODULATE4X, D3DTOP_ADD, D3DTOP_ADDSIGNED, D3DTOP_ADDSIGNED2X, D3DTOP_SUBTRACT,
        D3DTOP_ADDSMOOTH, D3DTOP_BLENDDIFFUSEALPHA, D3DTOP_BLENDCURRENTALPHA, D3DTOP_BLENDTEXTUREALPHA,
        D3DTOP_BLENDFACTORALPHA, D3DTOP_BLENDTEXTUREALPHAPM, D3DTOP_PREMODULATE,
        D3DTOP_MODULATEALPHA_ADDCOLOR, D3DTOP_MODULATECOLOR_ADDALPHA, D3DTOP_MODULATEINVALPHA_ADDCOLOR,
        D3DTOP_MODULATEINVCOLOR_ADDALPHA, D3DTOP_DOTPRODUCT3, D3DTOP_MULTIPLYADD, D3DTOP_LERP,
        D3DTOP_BUMPENVMAP, D3DTOP_BUMPENVMAPLUMINANCE
    };
    return v < sizeof map / sizeof map[0] && map[v] ? map[v] : D3DTOP_DISABLE;
}

static DWORD x_addr(uint32_t v) { return (v >= 1 && v <= 4) ? v : D3DTADDRESS_CLAMP; }

#include "hle_d3d8_psh.inc"

static void ps_dump_once(void)
{
    static uint32_t seen[64];
    static int nseen;
    int i;
    if (!s_pshader || nseen >= 64) return;
    for (i = 0; i < nseen; i++) if (seen[i] == s_pshader) return;
    seen[nseen++] = s_pshader;
    fprintf(stderr, "[PSH] %08X count=%X modes=%X dotmap=%X inputtex=%X cmp=%X fin=%X/%X fc=%X/%X tex=%X,%X,%X,%X\n",
            s_pshader, XRS(53), XRS(136), XRS(55), XRS(56), XRS(42), XRS(8), XRS(9), XRS(43), XRS(44),
            s_tex[0], s_tex[1], s_tex[2], s_tex[3]);
    for (i = 0; i < 8; i++)
        fprintf(stderr, "[PSH]   %d rgbin=%08X rgbout=%08X ain=%08X aout=%08X c0=%08X c1=%08X\n", i,
                XRS(34 + i), XRS(45 + i), XRS(0 + i), XRS(26 + i), XRS(10 + i), XRS(18 + i));
    fflush(stderr);
}

static void apply_state(void)
{
    int st, active = 1, use_ps = 0;
    uint32_t cw = XRS(XRS_COLORWRITEENABLE);
    IDirect3DDevice8 *d = s_dev;
    float f;

    if (getenv("CW_PSH_LOG")) ps_dump_once();
    if (s_pshader && !getenv("CW_NO_PSH"))
        use_ps = psh_bind();
    if (!use_ps)
        IDirect3DDevice8_SetPixelShader(d, 0);
    /* v1 (the specular colour) reaches a pixel shader only with specular on. */
    IDirect3DDevice8_SetRenderState(d, D3DRS_SPECULARENABLE, use_ps ? TRUE : FALSE);

    {
        /* Culling stays off except for stencil passes: the title draws its
         * stencil shadow volumes (the hailfire droids' among them) in two
         * passes, front faces and back faces, and relies on culling to split
         * them. With culling off both passes saw every face and the stencil
         * count came out wrong -- a solid dark wedge over the ground.
         * Xbox D3DRS_CULLMODE: 0 none, 0x900 CW, 0x901 CCW (PC 1, 2, 3). */
        DWORD cull = D3DCULL_NONE;
        if (XRS(XRS_STENCILENABLE) && !getenv("CW_NO_STENCIL_CULL")) {
            uint32_t c = XRS(XRS_CULLMODE);
            cull = c == 0x900 ? D3DCULL_CW : c == 0x901 ? D3DCULL_CCW : D3DCULL_NONE;
        }
        IDirect3DDevice8_SetRenderState(d, D3DRS_CULLMODE, cull);
    }
    /* Fixed-function lighting as the title set it (lights and material are
     * mirrored by the SetLight/LightEnable/SetMaterial wrappers). Models
     * whose vertices carry no colour rely on it: with lighting off their
     * diffuse reaches a pixel shader as zero. Deferred states 102..115. */
    IDirect3DDevice8_SetRenderState(d, D3DRS_LIGHTING, XRS(102) ? TRUE : FALSE);
    IDirect3DDevice8_SetRenderState(d, D3DRS_LOCALVIEWER, XRS(104) ? TRUE : FALSE);
    IDirect3DDevice8_SetRenderState(d, D3DRS_COLORVERTEX, XRS(105) ? TRUE : FALSE);
    IDirect3DDevice8_SetRenderState(d, D3DRS_SPECULARMATERIALSOURCE, XRS(110) <= 2 ? XRS(110) : 0);
    IDirect3DDevice8_SetRenderState(d, D3DRS_DIFFUSEMATERIALSOURCE, XRS(111) <= 2 ? XRS(111) : 0);
    IDirect3DDevice8_SetRenderState(d, D3DRS_AMBIENTMATERIALSOURCE, XRS(112) <= 2 ? XRS(112) : 0);
    IDirect3DDevice8_SetRenderState(d, D3DRS_EMISSIVEMATERIALSOURCE, XRS(113) <= 2 ? XRS(113) : 0);
    IDirect3DDevice8_SetRenderState(d, D3DRS_AMBIENT, XRS(115));
    IDirect3DDevice8_SetRenderState(d, D3DRS_NORMALIZENORMALS, XRS(142) ? TRUE : FALSE);
    /* Point sprites (particles, blaster bolts): deferred states 116..123 --
     * POINTSIZE, POINTSIZE_MIN, POINTSPRITEENABLE, POINTSCALEENABLE,
     * POINTSCALE_A/B/C, POINTSIZE_MAX (floats stored as their bits). */
    {
        /* Sizes are pixels at 640x480: scale to the render size. With
         * POINTSCALEENABLE the runtime scales by viewport height itself. */
        float ps, pmin, pmax;
        DWORD u;
        u = XRS(116); memcpy(&ps, &u, 4);
        u = XRS(117); memcpy(&pmin, &u, 4);
        u = XRS(123); memcpy(&pmax, &u, 4);
        if (!XRS(119)) ps *= s_hs;
        pmin *= s_hs; pmax *= s_hs;
        memcpy(&u, &ps, 4);   IDirect3DDevice8_SetRenderState(d, D3DRS_POINTSIZE, u);
        memcpy(&u, &pmin, 4); IDirect3DDevice8_SetRenderState(d, D3DRS_POINTSIZE_MIN, u);
        memcpy(&u, &pmax, 4); IDirect3DDevice8_SetRenderState(d, D3DRS_POINTSIZE_MAX, u);
    }
    IDirect3DDevice8_SetRenderState(d, D3DRS_POINTSPRITEENABLE, XRS(118) ? TRUE : FALSE);
    IDirect3DDevice8_SetRenderState(d, D3DRS_POINTSCALEENABLE, XRS(119) ? TRUE : FALSE);
    IDirect3DDevice8_SetRenderState(d, D3DRS_POINTSCALE_A, XRS(120));
    IDirect3DDevice8_SetRenderState(d, D3DRS_POINTSCALE_B, XRS(121));
    IDirect3DDevice8_SetRenderState(d, D3DRS_POINTSCALE_C, XRS(122));
    IDirect3DDevice8_SetRenderState(d, D3DRS_ZENABLE, XRS(XRS_ZENABLE) ? D3DZB_TRUE : D3DZB_FALSE);
    IDirect3DDevice8_SetRenderState(d, D3DRS_ZWRITEENABLE, XRS(XRS_ZWRITEENABLE) ? TRUE : FALSE);
    IDirect3DDevice8_SetRenderState(d, D3DRS_ZFUNC, x_cmp(XRS(XRS_ZFUNC)));
    {
        DWORD be = XRS(XRS_ALPHABLENDENABLE) ? TRUE : FALSE;
        static int hack = -1;
        if (hack < 0) { const char *e = getenv("CW_BLEND_HACK"); hack = e && *e == '1'; }
        if (hack && !be && !(XRS(XRS_SRCBLEND) == 1 && XRS(XRS_DESTBLEND) == 0) &&
            !(XRS(XRS_SRCBLEND) == 0x302 && XRS(XRS_DESTBLEND) == 0x303))
            be = TRUE;                           /* experiment: see CW_BLEND_HACK */
        IDirect3DDevice8_SetRenderState(d, D3DRS_ALPHABLENDENABLE, be);
    }
    IDirect3DDevice8_SetRenderState(d, D3DRS_SRCBLEND, x_blend(XRS(XRS_SRCBLEND)));
    IDirect3DDevice8_SetRenderState(d, D3DRS_DESTBLEND, x_blend(XRS(XRS_DESTBLEND)));
    IDirect3DDevice8_SetRenderState(d, D3DRS_BLENDOP, x_blendop(XRS(XRS_BLENDOP)));
    IDirect3DDevice8_SetRenderState(d, D3DRS_ALPHATESTENABLE, XRS(XRS_ALPHATESTENABLE) ? TRUE : FALSE);
    IDirect3DDevice8_SetRenderState(d, D3DRS_ALPHAFUNC, x_cmp(XRS(XRS_ALPHAFUNC)));
    IDirect3DDevice8_SetRenderState(d, D3DRS_ALPHAREF, XRS(XRS_ALPHAREF) & 0xFF);
    IDirect3DDevice8_SetRenderState(d, D3DRS_COLORWRITEENABLE,
        ((cw & 0x00010000u) ? D3DCOLORWRITEENABLE_RED : 0) | ((cw & 0x00000100u) ? D3DCOLORWRITEENABLE_GREEN : 0) |
        ((cw & 0x00000001u) ? D3DCOLORWRITEENABLE_BLUE : 0) | ((cw & 0x01000000u) ? D3DCOLORWRITEENABLE_ALPHA : 0));
    IDirect3DDevice8_SetRenderState(d, D3DRS_STENCILENABLE, XRS(XRS_STENCILENABLE) ? TRUE : FALSE);
    IDirect3DDevice8_SetRenderState(d, D3DRS_STENCILFUNC, x_cmp(XRS(XRS_STENCILFUNC)));
    IDirect3DDevice8_SetRenderState(d, D3DRS_STENCILREF, XRS(XRS_STENCILREF));
    IDirect3DDevice8_SetRenderState(d, D3DRS_STENCILMASK, XRS(XRS_STENCILMASK));
    IDirect3DDevice8_SetRenderState(d, D3DRS_STENCILWRITEMASK, XRS(XRS_STENCILWRITEMASK));
    IDirect3DDevice8_SetRenderState(d, D3DRS_STENCILFAIL, x_stencilop(XRS(XRS_STENCILFAIL)));
    IDirect3DDevice8_SetRenderState(d, D3DRS_STENCILZFAIL, x_stencilop(XRS(XRS_STENCILZFAIL)));
    IDirect3DDevice8_SetRenderState(d, D3DRS_STENCILPASS, x_stencilop(XRS(XRS_STENCILPASS)));
    IDirect3DDevice8_SetRenderState(d, D3DRS_TEXTUREFACTOR, XRS(XRS_TEXTUREFACTOR));
    IDirect3DDevice8_SetRenderState(d, D3DRS_SHADEMODE, XRS(XRS_SHADEMODE) == 0x1D00 ? D3DSHADE_FLAT : D3DSHADE_GOURAUD);
    IDirect3DDevice8_SetRenderState(d, D3DRS_FILLMODE,
        XRS(XRS_FILLMODE) == 0x1B00 ? D3DFILL_POINT : XRS(XRS_FILLMODE) == 0x1B01 ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
    IDirect3DDevice8_SetRenderState(d, D3DRS_FOGENABLE, XRS(XRS_FOGENABLE) ? TRUE : FALSE);
    IDirect3DDevice8_SetRenderState(d, D3DRS_FOGCOLOR, XRS(XRS_FOGCOLOR));
    IDirect3DDevice8_SetRenderState(d, D3DRS_FOGTABLEMODE, XRS(XRS_FOGTABLEMODE) <= 3 ? XRS(XRS_FOGTABLEMODE) : 0);
    f = 0; memcpy(&f, gptr(0x002E4250u + 4u * XRS_FOGSTART), 4);
    IDirect3DDevice8_SetRenderState(d, D3DRS_FOGSTART, XRS(XRS_FOGSTART));
    IDirect3DDevice8_SetRenderState(d, D3DRS_FOGEND, XRS(XRS_FOGEND));
    IDirect3DDevice8_SetRenderState(d, D3DRS_FOGDENSITY, XRS(XRS_FOGDENSITY));
    (void)f;

    for (st = 0; st < 4; st++) {
        IDirect3DTexture8 *t = get_texture(s_tex[st]);
        DWORD cop = x_top(XTSS(st, XTSS_COLOROP)), aop = x_top(XTSS(st, XTSS_ALPHAOP));
        IDirect3DDevice8_SetTexture(d, st, (IDirect3DBaseTexture8 *)t);
        if (use_ps) {
            /* The PC pixel shader combines; stage ops only need to stay valid.
             * Stage n reads texture coordinate set n -- but a fixed-function
             * draw whose vertex format carries fewer sets makes the PC reject
             * the draw outright (the player's tank hull: two-texture shader,
             * one coordinate set). Clamp to the last set the format has. */
            DWORD tci = (DWORD)st;
            if (!(s_vshader & 1)) {
                DWORD ntex = (s_vshader >> 8) & 0xF;
                tci = ntex == 0 ? 0 : (tci >= ntex ? ntex - 1 : tci);
            }
            IDirect3DDevice8_SetTextureStageState(d, st, D3DTSS_TEXCOORDINDEX, tci);
            cop = st == 0 ? D3DTOP_MODULATE : D3DTOP_DISABLE;
            aop = cop;
            if (st == 0) {
                IDirect3DDevice8_SetTextureStageState(d, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                IDirect3DDevice8_SetTextureStageState(d, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
            }
            if (st > 0) cop = aop = D3DTOP_MODULATE;   /* keep every stage's texture live */
        } else if (s_pshader) {
            /* An Xbox pixel shader (register combiners) replaces the stage
             * setup and is not translated yet: texture 0 times diffuse. */
            if (st == 0) {
                cop = t ? D3DTOP_MODULATE : D3DTOP_SELECTARG2;
                aop = t ? D3DTOP_MODULATE : D3DTOP_SELECTARG2;
                IDirect3DDevice8_SetTextureStageState(d, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                IDirect3DDevice8_SetTextureStageState(d, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
                IDirect3DDevice8_SetTextureStageState(d, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                IDirect3DDevice8_SetTextureStageState(d, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
            } else {
                cop = aop = D3DTOP_DISABLE;
            }
        } else {
            if (!active) cop = D3DTOP_DISABLE;
            IDirect3DDevice8_SetTextureStageState(d, st, D3DTSS_COLORARG0, XTSS(st, XTSS_COLORARG0));
            IDirect3DDevice8_SetTextureStageState(d, st, D3DTSS_COLORARG1, XTSS(st, XTSS_COLORARG1));
            IDirect3DDevice8_SetTextureStageState(d, st, D3DTSS_COLORARG2, XTSS(st, XTSS_COLORARG2));
            IDirect3DDevice8_SetTextureStageState(d, st, D3DTSS_ALPHAARG0, XTSS(st, XTSS_ALPHAARG0));
            IDirect3DDevice8_SetTextureStageState(d, st, D3DTSS_ALPHAARG1, XTSS(st, XTSS_ALPHAARG1));
            IDirect3DDevice8_SetTextureStageState(d, st, D3DTSS_ALPHAARG2, XTSS(st, XTSS_ALPHAARG2));
            IDirect3DDevice8_SetTextureStageState(d, st, D3DTSS_TEXCOORDINDEX, XTSS(st, XTSS_TEXCOORDINDEX) & 0x7);
        }
        if (cop == D3DTOP_DISABLE) { active = 0; aop = D3DTOP_DISABLE; }
        IDirect3DDevice8_SetTextureStageState(d, st, D3DTSS_COLOROP, cop);
        IDirect3DDevice8_SetTextureStageState(d, st, D3DTSS_ALPHAOP, aop);
        IDirect3DDevice8_SetTextureStageState(d, st, D3DTSS_ADDRESSU, x_addr(XTSS(st, XTSS_ADDRESSU)));
        IDirect3DDevice8_SetTextureStageState(d, st, D3DTSS_ADDRESSV, x_addr(XTSS(st, XTSS_ADDRESSV)));
        IDirect3DDevice8_SetTextureStageState(d, st, D3DTSS_ADDRESSW, x_addr(XTSS(st, XTSS_ADDRESSW)));
        IDirect3DDevice8_SetTextureStageState(d, st, D3DTSS_MAGFILTER, XTSS(st, XTSS_MAGFILTER) <= 3 ? XTSS(st, XTSS_MAGFILTER) : D3DTEXF_LINEAR);
        IDirect3DDevice8_SetTextureStageState(d, st, D3DTSS_MINFILTER, XTSS(st, XTSS_MINFILTER) <= 3 ? XTSS(st, XTSS_MINFILTER) : D3DTEXF_LINEAR);
        IDirect3DDevice8_SetTextureStageState(d, st, D3DTSS_MIPFILTER, XTSS(st, XTSS_MIPFILTER) <= 2 ? XTSS(st, XTSS_MIPFILTER) : D3DTEXF_LINEAR);
        IDirect3DDevice8_SetTextureStageState(d, st, D3DTSS_BORDERCOLOR, XTSS(st, XTSS_BORDERCOLOR));
        IDirect3DDevice8_SetTextureStageState(d, st, D3DTSS_TEXTURETRANSFORMFLAGS, XTSS(st, XTSS_TEXTURETRANSFORMFLAGS) & 0x10F);
    }
}

/* One-time dump of the title's render-state and texture-stage arrays, to pin
 * down the Xbox enum layout from real values. */
static void dump_states(void)
{
    static int done;
    int i;
    if (done || s_frames < 120)
        return;
    done = 1;
    fprintf(stderr, "[D3D8] render states (D3D_g_RenderState 0x2E4250):");
    for (i = 0; i < 166; i++)
        fprintf(stderr, "%s%d=%X", (i % 8) ? " " : "\n  ", i, MEM32(0x2E4250 + i * 4));
    fprintf(stderr, "\n[D3D8] texture stage states (0x2E4050), stage 0/1:");
    for (i = 0; i < 64; i++)
        fprintf(stderr, "%s%d.%d=%X", (i % 8) ? " " : "\n  ", i / 32, i % 32, MEM32(0x2E4050 + i * 4));
    fprintf(stderr, "\n");
    fflush(stderr);
}

static void cap_draw(const char *kind, uint32_t xprim, uint32_t nverts, uint32_t pidx);
static const uint8_t *rhw_map(const uint8_t *v, uint32_t lo, uint32_t hi, uint32_t stride);
static void rhw_inset_quads(const uint8_t *v, uint32_t nverts, uint32_t stride);

/* Draw calls with their result checked: a draw the PC runtime rejects
 * (invalid state for it) is otherwise simply missing from the frame. */
static void draw_failed(HRESULT hr, const char *what)
{
    static int n;
    if (n++ < 40)
        fprintf(stderr, "[D3D8] %s failed hr=0x%08lX vs=%08X ps=%08X tex=%08X,%08X frame %u\n", what,
                (unsigned long)hr, s_vshader, s_pshader, s_tex[0], s_tex[1], s_frames);
}
static void chk_DIPUP(IDirect3DDevice8 *d, D3DPRIMITIVETYPE pt, UINT lo, UINT n, UINT prims,
                      const void *idx, D3DFORMAT f, const void *v, UINT stride)
{
    HRESULT hr = IDirect3DDevice8_DrawIndexedPrimitiveUP(d, pt, lo, n, prims, idx, f, v, stride);
    if (FAILED(hr)) draw_failed(hr, "DrawIndexedPrimitiveUP");
}
static void chk_DPUP(IDirect3DDevice8 *d, D3DPRIMITIVETYPE pt, UINT prims, const void *v, UINT stride)
{
    HRESULT hr = IDirect3DDevice8_DrawPrimitiveUP(d, pt, prims, v, stride);
    if (FAILED(hr)) draw_failed(hr, "DrawPrimitiveUP");
}

#include "hle_d3d8_vsh.inc"
int hle_vs_consts_set(int n) { s_vs_consts = n; return n; }

static void draw(uint32_t xprim, uint32_t nverts, uint32_t start, uint32_t pidx)
{
    D3DPRIMITIVETYPE pt;
    UINT prims;
    uint32_t vb = s_stream_vb[0], stride = s_stream_stride[0], base;
    const uint8_t *verts;

    s_draws++;
    if (!ensure_device())
        return;
    if (rt_offscreen()) {
        s_offscreen_draws++;
        return;
    }
    if (s_vshader & 1) {                  /* Xbox vertex program: hle_d3d8_vsh.inc */
        if (draw_vs(xprim, nverts, start, pidx))
            s_draws_done++;
        else
            s_draws_skipped_vs++;
        return;
    }
    if (!vb || !stride)
        return;
    {
        /* CW_FVF_LOG=1: each distinct fixed-function vertex format once. */
        static int on = -1, n;
        static uint32_t seen[64];
        int k;
        if (on < 0) { const char *e = getenv("CW_FVF_LOG"); on = e && *e == '1'; }
        if (on && n < 64) {
            for (k = 0; k < n; k++) if (seen[k] == s_vshader) break;
            if (k == n) {
                seen[n++] = s_vshader;
                fprintf(stderr, "[FVF] %08X stride %u prim %u verts %u tex0 %08X frame %u\n",
                        s_vshader, stride, xprim, nverts, s_tex[0], s_frames);
            }
        }
    }
    if (!prim_convert(xprim, nverts, &pt, &prims))
        return;

    base = phys_to_va(MEM32(vb + 4));
    apply_state();
    cap_draw("ff", xprim, nverts, pidx);
    {
        /* CW_FF_HILITE=1: fixed-function draws in solid magenta, opaque. */
        static int hl = -1;
        if (hl < 0) { const char *e = getenv("CW_FF_HILITE"); hl = e && *e == '1'; }
        if (hl && !(s_vshader & 0x004)) {             /* not pre-transformed (XYZRHW) */
            static const float magenta[4] = { 1, 0, 1, 1 };
            psh_solid(magenta);
            IDirect3DDevice8_SetRenderState(s_dev, D3DRS_ALPHABLENDENABLE, FALSE);
        }
    }
    IDirect3DDevice8_SetVertexShader(s_dev, s_vshader);

    if (!pidx) {
        verts = (const uint8_t *)gptr(base + start * stride);
        if ((s_vshader & D3DFVF_POSITION_MASK) == D3DFVF_XYZRHW && nverts) {
            verts = rhw_map(verts, 0, nverts - 1, stride);
            if (xprim == 8) rhw_inset_quads(verts, nverts, stride);
        }
        if (xprim == 8) {                 /* quads -> indexed triangles */
            static uint16_t idx[65536 / 4 * 6];
            uint32_t q, nq = nverts / 4;
            if (nq > 65536 / 4) nq = 65536 / 4;
            for (q = 0; q < nq; q++) {
                idx[q*6+0] = (uint16_t)(q*4); idx[q*6+1] = (uint16_t)(q*4+1); idx[q*6+2] = (uint16_t)(q*4+2);
                idx[q*6+3] = (uint16_t)(q*4); idx[q*6+4] = (uint16_t)(q*4+2); idx[q*6+5] = (uint16_t)(q*4+3);
            }
            chk_DIPUP(s_dev, D3DPT_TRIANGLELIST, 0, nq * 4, nq * 2,
                                                    idx, D3DFMT_INDEX16, verts, stride);
        } else {
            chk_DPUP(s_dev, pt, prims, verts, stride);
        }
    } else {
        const uint16_t *gi = (const uint16_t *)gptr(pidx);
        static uint16_t idx[65536 / 4 * 6];
        uint32_t i, lo = 0xFFFF, hi = 0, n = nverts;
        if (n > 65536) n = 65536;
        for (i = 0; i < n; i++) {
            if (gi[i] < lo) lo = gi[i];
            if (gi[i] > hi) hi = gi[i];
        }
        if (lo > hi)
            return;
        verts = (const uint8_t *)gptr(base + s_ib_base * stride);
        if ((s_vshader & D3DFVF_POSITION_MASK) == D3DFVF_XYZRHW)
            verts = rhw_map(verts, lo, hi, stride);
        if (xprim == 8) {
            uint32_t q, nq = n / 4;
            for (q = 0; q < nq; q++) {
                idx[q*6+0] = gi[q*4]; idx[q*6+1] = gi[q*4+1]; idx[q*6+2] = gi[q*4+2];
                idx[q*6+3] = gi[q*4]; idx[q*6+4] = gi[q*4+2]; idx[q*6+5] = gi[q*4+3];
            }
            chk_DIPUP(s_dev, D3DPT_TRIANGLELIST, lo, hi - lo + 1,
                                                    nq * 2, idx, D3DFMT_INDEX16, verts, stride);
        } else {
            chk_DIPUP(s_dev, pt, lo, hi - lo + 1, prims,
                                                    gi, D3DFMT_INDEX16, verts, stride);
        }
    }
    s_draws_done++;
}

/* ---- transforms and viewport ------------------------------------------- */

/* Xbox D3DTRANSFORMSTATETYPE: 0 view, 1 projection, 2-5 texture0-3, 6-9 world0-3. */
/* ---- widescreen and resolution mapping ---------------------------------- */

/* Widescreen is done by the game itself. Its camera update, sub_00227960
 * (thiscall, the camera), derives the horizontal field of view from the
 * viewport's half-width at +0x2AC over its half-height at +0x2B0 (320 and
 * -240) and builds the projection at +0xD0 and the culling planes at
 * +0x1AC.. from it: aspect = hw / -hh / [+0x290] (a pixel-aspect factor,
 * 1.0). For a full-screen camera that factor is scaled by k for the duration
 * of the call only, so the game renders -- and culls -- the wider view.
 * Leaving a changed value behind fed back: at gameplay start the game
 * re-derives its viewport and that factor from the camera's fields (a 747
 * pixel viewport, off-centre crosshair). Scaling clip space in the renderer
 * instead left the culling at 4:3: terrain was missing at the sides. The
 * renderer's own factor (s_k_cur) stays 1. */
void sub_00227960_gen(void);
void sub_00227960(void)
{
    uint32_t cam = g_ecx;
    if (hle_on() && s_k < 1.0f && cam &&
        MEM32(cam + 0x29C) == 0 && MEM32(cam + 0x2A0) == 0 &&
        MEM32(cam + 0x2A4) == 639 && MEM32(cam + 0x2A8) == 479) {
        uint32_t keep = MEM32(cam + 0x290);
        MEMF(cam + 0x290) = MEMF(cam + 0x290) * s_k;
        sub_00227960_gen();
        MEM32(cam + 0x290) = keep;
        return;
    }
    sub_00227960_gen();
}

/* The title's viewport and projection are for 640x480. A full-screen viewport
 * becomes the whole render target (in 4:3 mode, the centred 4:3 area), and the horizontal extent of perspective
 * 3D is narrowed by s_k in clip space -- the view widens instead of the image
 * stretching. Smaller viewports (a 3D model inside a menu panel) keep their
 * place in the centred 4:3 layout, unwidened. Fixed-function draws get the
 * factor through the projection matrix, vertex programs through PC constant
 * s_hor_c = (k, 1-k, 1, 0), applied where their clip-space w is not 1 (w 1
 * means an orthographic, screen-aligned draw). */
static D3DVIEWPORT8 s_gvp;
static int   s_gvp_have;
static D3DMATRIX s_proj;
static int   s_proj_have;

static void proj_apply(void)
{
    D3DMATRIX m;
    if (!s_proj_have) return;
    m = s_proj;
    if (m._34 != 0.0f && s_k_cur > 0.0f && s_k_cur != 1.0f) {
        m._11 *= s_k_cur; m._21 *= s_k_cur; m._31 *= s_k_cur; m._41 *= s_k_cur;
    }
    IDirect3DDevice8_SetTransform(s_dev, D3DTS_PROJECTION, &m);
}

static void vp_apply(void)
{
    D3DVIEWPORT8 v, g = s_gvp;
    float k = 1.0f;
    if (!s_gvp_have) { g.X = 0; g.Y = 0; g.Width = 640; g.Height = 480; g.MinZ = 0.0f; g.MaxZ = 1.0f; }
    v.MinZ = g.MinZ; v.MaxZ = g.MaxZ;
    if (g.X == 0 && g.Y == 0 && g.Width >= 640 && g.Height >= 480 && s_k < 1.0f) {
        v.X = 0; v.Y = 0; v.Width = (DWORD)s_bbw; v.Height = (DWORD)s_bbh;
        /* The game widens its own camera (sub_00227960 above); CW_WIDE_RENDER=1
         * does it here in clip space instead (no culling fix). */
        k = getenv("CW_WIDE_RENDER") ? s_k : 1.0f;
    } else {
        float x0 = s_hx0 + (float)g.X * s_hs, y0 = s_hy0 + (float)g.Y * s_hs;
        float x1 = s_hx0 + (float)(g.X + g.Width) * s_hs, y1 = s_hy0 + (float)(g.Y + g.Height) * s_hs;
        if (x0 < 0) x0 = 0;
        if (y0 < 0) y0 = 0;
        if (x1 > (float)s_bbw) x1 = (float)s_bbw;
        if (y1 > (float)s_bbh) y1 = (float)s_bbh;
        v.X = (DWORD)(x0 + 0.5f); v.Y = (DWORD)(y0 + 0.5f);
        v.Width = x1 > x0 ? (DWORD)(x1 - x0 + 0.5f) : 1; v.Height = y1 > y0 ? (DWORD)(y1 - y0 + 0.5f) : 1;
        if (v.X + v.Width > (DWORD)s_bbw) v.Width = (DWORD)s_bbw - v.X;
        if (v.Y + v.Height > (DWORD)s_bbh) v.Height = (DWORD)s_bbh - v.Y;
    }
    IDirect3DDevice8_SetViewport(s_dev, &v);
    if (k != s_k_cur) {
        s_k_cur = k;
        proj_apply();
        if (s_hor_c >= 0) {
            float c[4] = { k, 1.0f - k, 1.0f, 0.0f };
            IDirect3DDevice8_SetVertexShaderConstant(s_dev, (DWORD)s_hor_c, c, 1);
        }
    }
}

/* A texture big enough to be screen art (either side over 64 texels). */
static int tex_is_picture(uint32_t res)
{
    IDirect3DTexture8 *t;
    D3DSURFACE_DESC d;
    if (!res || !(t = get_texture(res)) || FAILED(IDirect3DTexture8_GetLevelDesc(t, 0, &d)))
        return 0;
    return d.Width > 64 || d.Height > 64;
}

/* Screen-space (pre-transformed) vertices: 640x480 -> render target. */
static const uint8_t *rhw_map(const uint8_t *v, uint32_t lo, uint32_t hi, uint32_t stride)
{
    static uint8_t *buf;
    static size_t cap;
    size_t need = (size_t)(hi + 1) * stride;
    float minx = 1e9f, maxx = -1e9f, sx, sy, ox, oy;
    uint32_t i;
    if (s_bbw == 640 && s_bbh == 480) return v;
    if (need > cap) {
        free(buf);
        cap = need + need / 2;
        buf = (uint8_t *)malloc(cap);
        if (!buf) { cap = 0; return v; }
    }
    memcpy(buf + (size_t)lo * stride, v + (size_t)lo * stride, (size_t)(hi - lo + 1) * stride);
    for (i = lo; i <= hi; i++) {
        float x;
        memcpy(&x, buf + (size_t)i * stride, 4);
        if (x < minx) minx = x;
        if (x > maxx) maxx = x;
    }
    /* Spanning the screen and plain -- untextured, or a small texture (fades
     * draw a tiny texture tinted by vertex colour; cutscene bars) -- or
     * blended over the scene (a vignette fade with a large radial texture):
     * stretch across. Opaque pictures on large textures -- full-screen 2D
     * art such as loading and title screens -- stay 4:3, pillarboxed. */
    if (minx <= 1.0f && maxx >= 638.0f && (!tex_is_picture(s_tex[0]) || XRS(XRS_ALPHABLENDENABLE))) {
        sx = (float)s_bbw / 640.0f; sy = (float)s_bbh / 480.0f; ox = 0; oy = 0;
    } else {
        sx = sy = s_hs; ox = s_hx0; oy = s_hy0;
    }
    for (i = lo; i <= hi; i++) {
        float *p = (float *)(buf + (size_t)i * stride);
        /* Edges map to edges: the title puts quad edges on whole pixels
         * (0 and 640), so a plain scale keeps full-screen pieces flush. */
        p[0] = ox + p[0] * sx;
        p[1] = oy + p[1] * sy;
    }
    return buf;
}

/* Scaled-up 2D quads cut from a shared texture (font glyphs, HUD icons): at
 * 640x480 each pixel sampled a texel centre, but enlarged, the filter reaches
 * half a texel past each quad's edge into the neighbouring glyph -- stray
 * strokes beside letters. Pull each quad's texture coordinates in by half a
 * texel. v: the vertices rhw_map returned (a private copy). */
static void rhw_inset_quads(const uint8_t *vconst, uint32_t nverts, uint32_t stride)
{
    uint8_t *v = (uint8_t *)vconst;
    IDirect3DTexture8 *t;
    D3DSURFACE_DESC d;
    uint32_t off = 16, q, i;
    float hu, hv;
    if ((s_bbw == 640 && s_bbh == 480) || !s_tex[0] || ((s_vshader >> 8) & 0xF) == 0) return;
    if (s_vshader & D3DFVF_DIFFUSE) off += 4;
    if (s_vshader & D3DFVF_SPECULAR) off += 4;
    if (off + 8 > stride) return;
    t = get_texture(s_tex[0]);
    if (!t || FAILED(IDirect3DTexture8_GetLevelDesc(t, 0, &d)) || !d.Width || !d.Height) return;
    hu = 0.5f / (float)d.Width; hv = 0.5f / (float)d.Height;
    for (q = 0; q + 4 <= nverts; q += 4) {
        float umin = 1e9f, umax = -1e9f, vmin = 1e9f, vmax = -1e9f;
        for (i = q; i < q + 4; i++) {
            const float *uv = (const float *)(v + (size_t)i * stride + off);
            if (uv[0] < umin) umin = uv[0];
            if (uv[0] > umax) umax = uv[0];
            if (uv[1] < vmin) vmin = uv[1];
            if (uv[1] > vmax) vmax = uv[1];
        }
        if (umax > 2.0f || vmax > 2.0f) return;          /* texel-unit coordinates: leave */
        for (i = q; i < q + 4; i++) {
            float *uv = (float *)(v + (size_t)i * stride + off);
            if (umax - umin > 2.0f * hu) uv[0] += uv[0] == umin ? hu : uv[0] == umax ? -hu : 0.0f;
            if (vmax - vmin > 2.0f * hv) uv[1] += uv[1] == vmin ? hv : uv[1] == vmax ? -hv : 0.0f;
        }
    }
}

static void m_set_transform(uint32_t state, uint32_t pm)
{
    D3DTRANSFORMSTATETYPE t;
    if (!ensure_device() || !pm)
        return;
    if (state == 0) t = D3DTS_VIEW;
    else if (state == 1) t = D3DTS_PROJECTION;
    else if (state >= 2 && state <= 5) t = (D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + (state - 2));
    else if (state >= 6 && state <= 9) t = D3DTS_WORLDMATRIX(state - 6);
    else return;
    if (t == D3DTS_PROJECTION) {
        memcpy(&s_proj, gptr(pm), sizeof s_proj);
        s_proj_have = 1;
        proj_apply();
        return;
    }
    IDirect3DDevice8_SetTransform(s_dev, t, (const D3DMATRIX *)gptr(pm));
}

/* D3DLIGHT8 and D3DMATERIAL8 have the PC layout on the Xbox. */
static void m_set_light(uint32_t index, uint32_t plight)
{
    if (!ensure_device() || !plight)
        return;
    IDirect3DDevice8_SetLight(s_dev, index, (const D3DLIGHT8 *)gptr(plight));
}

static void m_light_enable(uint32_t index, uint32_t enable)
{
    if (!ensure_device())
        return;
    IDirect3DDevice8_LightEnable(s_dev, index, enable ? TRUE : FALSE);
}

static void m_set_material(uint32_t pmat)
{
    if (!ensure_device() || !pmat)
        return;
    IDirect3DDevice8_SetMaterial(s_dev, (const D3DMATERIAL8 *)gptr(pmat));
}

static void m_set_viewport(uint32_t pvp)
{
    if (!ensure_device() || !pvp)
        return;
    memcpy(&s_gvp, gptr(pvp), sizeof s_gvp);
    s_gvp_have = 1;
    vp_apply();
}

/* ---- device-level mirrors ---------------------------------------------- */

/* Clear(Count, pRects, Flags, Color, Z, Stencil). Xbox splits the target flag
 * per channel (0xF0); depth and stencil bits match the PC. */
static FILE *s_cap;                        /* open while a frame is being captured */
static int s_cap_seq, s_cap_draw;

static void m_clear(const uint32_t *a)
{
    DWORD flags = 0;
    float z;
    if (!ensure_device() || rt_offscreen())
        return;
    if (a[2] & 0x000000F0u) flags |= D3DCLEAR_TARGET;
    if (a[2] & 0x00000001u) flags |= D3DCLEAR_ZBUFFER;
    if (a[2] & 0x00000002u) flags |= D3DCLEAR_STENCIL;
    memcpy(&z, &a[4], 4);
    if (s_cap)
        fprintf(s_cap, "%4d clr flags=%08X (pc %lX) colour=%08X z=%g stencil=%u\n", s_cap_draw++, a[2],
                (unsigned long)flags, a[3], z, a[5]);
    IDirect3DDevice8_Clear(s_dev, 0, NULL, flags ? flags : D3DCLEAR_TARGET,
                           (D3DCOLOR)a[3], z, a[5]);
}

/* CW_D3D8_DUMP=<prefix>: save the back buffer as <prefix>NNN.bmp every
 * 3 seconds of presented frames, 80 files at most -- what the window shows, for runs nobody
 * watches. 24-bit bottom-up BMP, same as the runtime's surface dumps. */
/* hle_input.c's live control file asks for a screenshot with "SHOT"; it is
 * written to CW_SHOT_FILE (default shot.bmp) at the next Swap. */
volatile int hle_shot_request;

static void dump_backbuffer_to(const char *forced);
static void dump_backbuffer(void)
{
    extern char hle_shot_path[260];
    if (hle_shot_request) {
        const char *p = getenv("CW_SHOT_FILE");
        hle_shot_request = 0;
        if (hle_shot_path[0]) { dump_backbuffer_to(hle_shot_path); hle_shot_path[0] = 0; }
        else dump_backbuffer_to(p && *p ? p : "shot.bmp");
    }
    dump_backbuffer_to(NULL);
}

static void dump_backbuffer_to(const char *forced)
{
    static int seq;
    const char *prefix = getenv("CW_D3D8_DUMP");
    IDirect3DSurface8 *bb = NULL, *copy = NULL;
    D3DSURFACE_DESC desc;
    D3DLOCKED_RECT lr;
    char path[512];
    FILE *f;
    uint32_t w, h, y, x, row_bytes, pad, filesz;
    uint8_t hdr[54];

    static DWORD last;
    DWORD now = GetTickCount();
    if (!forced) {
        static DWORD interval;
        if (!interval) {
            const char *e = getenv("CW_D3D8_DUMP_SECONDS");   /* default 5 s */
            interval = (e && atoi(e) > 0) ? (DWORD)atoi(e) * 1000u : 5000u;
        }
        if (!prefix || !*prefix || seq >= 400 || (last && now - last < interval))
            return;
        last = now;
    }
    if (FAILED(IDirect3DDevice8_GetBackBuffer(s_dev, 0, D3DBACKBUFFER_TYPE_MONO, &bb)))
        return;
    IDirect3DSurface8_GetDesc(bb, &desc);
    if (FAILED(IDirect3DDevice8_CreateImageSurface(s_dev, desc.Width, desc.Height,
                                                   desc.Format, &copy))) {
        IDirect3DSurface8_Release(bb);
        return;
    }
    if (FAILED(IDirect3DDevice8_CopyRects(s_dev, bb, NULL, 0, copy, NULL)) ||
        FAILED(IDirect3DSurface8_LockRect(copy, &lr, NULL, D3DLOCK_READONLY))) {
        IDirect3DSurface8_Release(copy);
        IDirect3DSurface8_Release(bb);
        return;
    }
    w = desc.Width; h = desc.Height;
    row_bytes = w * 3; pad = (4 - (row_bytes & 3)) & 3;
    filesz = 54 + (row_bytes + pad) * h;
    if (forced) snprintf(path, sizeof path, "%s", forced);
    else snprintf(path, sizeof path, "%s%03d.bmp", prefix, seq++);
    f = fopen(path, "wb");
    if (f) {
        memset(hdr, 0, sizeof hdr);
        hdr[0] = 'B'; hdr[1] = 'M';
        memcpy(hdr + 2, &filesz, 4);
        hdr[10] = 54; hdr[14] = 40;
        memcpy(hdr + 18, &w, 4); memcpy(hdr + 22, &h, 4);
        hdr[26] = 1; hdr[28] = 24;
        fwrite(hdr, 1, sizeof hdr, f);
        for (y = h; y-- > 0; ) {
            const uint32_t *row = (const uint32_t *)((const uint8_t *)lr.pBits + (size_t)y * lr.Pitch);
            for (x = 0; x < w; x++) {
                uint8_t bgr[3] = { (uint8_t)row[x], (uint8_t)(row[x] >> 8), (uint8_t)(row[x] >> 16) };
                fwrite(bgr, 1, 3, f);
            }
            if (pad) { static const uint8_t z[3]; fwrite(z, 1, pad, f); }
        }
        fclose(f);
        d3d_log("frame %u saved to %s", s_frames, path);
    }
    IDirect3DSurface8_UnlockRect(copy);
    IDirect3DSurface8_Release(copy);
    IDirect3DSurface8_Release(bb);
}

/* ---- gameplay recording and glitch captures ------------------------------ */

/* Folder beside the exe: <exe dir>\captures. */
static const char *capture_dir(void)
{
    static char dir[MAX_PATH];
    if (!dir[0]) {
        char *slash;
        GetModuleFileNameA(NULL, dir, MAX_PATH);
        slash = strrchr(dir, '\\');
        if (slash) *slash = 0;
        strncat(dir, "\\captures", MAX_PATH - strlen(dir) - 1);
        CreateDirectoryA(dir, NULL);
    }
    return dir;
}

/* F12 (or the live-control CAPTURE line): the next frame is captured in
 * detail -- captures\glitch-NNN.bmp (the picture) and glitch-NNN.txt (every
 * draw with its shaders, textures and blend / depth state), so a glitch seen
 * in play can be traced to the draw that caused it. */
volatile int hle_capture_request;
static void cap_draw(const char *kind, uint32_t xprim, uint32_t nverts, uint32_t pidx)
{
    if (!s_cap) return;
    fprintf(s_cap, "%4d %-3s vs=%08X ps=%08X prim=%u n=%u idx=%u tex=%08X,%08X,%08X,%08X "
            "blend=%u %X>%X op=%X atest=%u %X/%02X z=%u zw=%u zf=%X cw=%08X sten=%u fog=%u "
            "light=%u tf=%08X stf=%X ref=%X fail=%X zfail=%X pass=%X cull=%X front=%X "
            "addr0=%X/%X bc0=%08X addr1=%X/%X ttf0=%X tci0=%X ttf1=%X tci1=%X\n",
            s_cap_draw++, kind, s_vshader, s_pshader, xprim, nverts, pidx ? 1 : 0,
            s_tex[0], s_tex[1], s_tex[2], s_tex[3],
            XRS(XRS_ALPHABLENDENABLE), XRS(XRS_SRCBLEND), XRS(XRS_DESTBLEND), XRS(XRS_BLENDOP),
            XRS(XRS_ALPHATESTENABLE), XRS(XRS_ALPHAFUNC), XRS(XRS_ALPHAREF) & 0xFF,
            XRS(XRS_ZENABLE), XRS(XRS_ZWRITEENABLE), XRS(XRS_ZFUNC), XRS(XRS_COLORWRITEENABLE),
            XRS(XRS_STENCILENABLE), XRS(XRS_FOGENABLE), XRS(102), XRS(XRS_TEXTUREFACTOR),
            XRS(XRS_STENCILFUNC), XRS(XRS_STENCILREF), XRS(XRS_STENCILFAIL), XRS(XRS_STENCILZFAIL),
            XRS(XRS_STENCILPASS), XRS(XRS_CULLMODE), XRS(XRS_FRONTFACE),
            XTSS(0, XTSS_ADDRESSU), XTSS(0, XTSS_ADDRESSV), XTSS(0, XTSS_BORDERCOLOR),
            XTSS(1, XTSS_ADDRESSU), XTSS(1, XTSS_ADDRESSV),
            XTSS(0, XTSS_TEXTURETRANSFORMFLAGS), XTSS(0, XTSS_TEXCOORDINDEX),
            XTSS(1, XTSS_TEXTURETRANSFORMFLAGS), XTSS(1, XTSS_TEXCOORDINDEX));
}

static void capture_poll(void)
{
    static int f12_was;
    int f12 = (GetAsyncKeyState(VK_F12) & 0x8000) != 0 && GetForegroundWindow() == s_hwnd;
    char path[MAX_PATH];
    if (s_cap) {                            /* the captured frame is complete */
        fclose(s_cap); s_cap = NULL;
        snprintf(path, sizeof path, "%s\\glitch-%03d.bmp", capture_dir(), s_cap_seq);
        dump_backbuffer_to(path);
        fprintf(stderr, "[CAPTURE] glitch-%03d saved (%d draws)\n", s_cap_seq, s_cap_draw);
        fflush(stderr);
        MessageBeep(MB_OK);
    }
    if ((f12 && !f12_was) || hle_capture_request) {
        hle_capture_request = 0;
        do {
            snprintf(path, sizeof path, "%s\\glitch-%03d.txt", capture_dir(), ++s_cap_seq);
        } while (GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES && s_cap_seq < 999);
        s_cap = fopen(path, "w");
        s_cap_draw = 0;
        if (s_cap) {
            SYSTEMTIME t;
            GetLocalTime(&t);
            fprintf(s_cap, "frame %u  %02u:%02u:%02u\n", s_frames, t.wHour, t.wMinute, t.wSecond);
        }
    }
    f12_was = f12;
}

/* The clock input recording and replay run on: segment = number of pauses
 * of over a second with no frame presented (movies, level loads), offset =
 * frames since the last one. How many frames the intro screens take depends
 * on the host clock; counting from the last pause keeps a replay aligned. */
static unsigned s_seg, s_seg_start;
static DWORD s_seg_last;
static void seg_note_swap(void)
{
    DWORD now = GetTickCount();
    if (s_seg_last && now - s_seg_last > 3000) {
        /* Two pauses less than 30 frames apart (movies played back to back,
         * with or without a frame between them depending on timing) are one
         * pause, counted from its end. */
        if (!s_seg || s_frames - s_seg_start >= 30)
            s_seg++;
        s_seg_start = s_frames;
        fprintf(stderr, "[INPUT] segment %u starts at frame %u (pause %lu ms)\n", s_seg, s_frames,
                (unsigned long)(now - s_seg_last));
    }
    s_seg_last = now;
}
unsigned hle_frame_count(void) { return s_frames; }

/* Live-control "FXAA 0|1": switch the smoothing pass for comparisons. */
void hle_set_fxaa(int on) { if (s_ps_fxaa && s_ps_luma) s_fxaa = on; }
void hle_frame_key(unsigned *seg, unsigned *off) { *seg = s_seg; *off = s_frames - s_seg_start; }

static void ovl_composite(void);
static void present_frame(int from_swap);
static uint32_t s_last_swap;

/* The Xbox presents on vertical blank (presentation interval one), so the
 * title never runs faster than 60 frames a second, and its gameplay code was
 * tuned at that rate or below. The runtime's Swap returns at once; pace it
 * here. CW_FPS_CAP=<n> changes the cap, 0 removes it. */
static void pace_frame(void)
{
    static LARGE_INTEGER freq, next;
    static double period = -1.0;
    LARGE_INTEGER now;
    if (period < 0) {
        const char *e = getenv("CW_FPS_CAP");
        int cap = e && *e ? atoi(e) : 60;
        period = cap > 0 ? 1.0 / cap : 0.0;
        QueryPerformanceFrequency(&freq);
        timeBeginPeriod(1);
    }
    if (period <= 0.0)
        return;
    QueryPerformanceCounter(&now);
    if (!next.QuadPart || now.QuadPart > next.QuadPart + (LONGLONG)(freq.QuadPart * period * 4)) {
        next.QuadPart = now.QuadPart;          /* start, or fell far behind: resync */
    }
    next.QuadPart += (LONGLONG)(freq.QuadPart * period);
    for (;;) {
        LONGLONG left;
        QueryPerformanceCounter(&now);
        left = next.QuadPart - now.QuadPart;
        if (left <= 0) break;
        if (left > freq.QuadPart / 500) Sleep(1);   /* > 2 ms: sleep, else spin */
    }
}

/* Frame times: the interval between presents, every frame, for measuring
 * lows. Live control "FTRESET" starts a sample, "FTREPORT" logs average,
 * 1% / 0.1% lows (the average of the slowest 1% / 0.1% of frames, as fps),
 * the worst frame and counts of frames over 20 ms and 33 ms. */
#define FT_MAX 216000                       /* one hour at 60 fps */
static float  *s_ft;
static unsigned s_ft_n;
static LARGE_INTEGER s_ft_last, s_ft_freq;

static void ft_note_present(void)
{
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    if (!s_ft_freq.QuadPart) QueryPerformanceFrequency(&s_ft_freq);
    if (!s_ft) s_ft = (float *)malloc(FT_MAX * sizeof(float));
    if (s_ft && s_ft_last.QuadPart && s_ft_n < FT_MAX)
        s_ft[s_ft_n++] = (float)((double)(now.QuadPart - s_ft_last.QuadPart) * 1000.0 / (double)s_ft_freq.QuadPart);
    s_ft_last = now;
}

void hle_ft_reset(void) { s_ft_n = 0; s_ft_last.QuadPart = 0; }

static int ft_cmp_desc(const void *a, const void *b)
{
    float x = *(const float *)a, y = *(const float *)b;
    return x < y ? 1 : x > y ? -1 : 0;
}

void hle_ft_report(void)
{
    unsigned n = s_ft_n, i, over20 = 0, over33 = 0, n1, n01;
    double sum = 0, s1 = 0, s01 = 0;
    float *c;
    if (!s_ft || n < 10) { fprintf(stderr, "[FT] too few frames (%u)\n", n); fflush(stderr); return; }
    c = (float *)malloc(n * sizeof(float));
    if (!c) return;
    memcpy(c, s_ft, n * sizeof(float));
    for (i = 0; i < n; i++) { sum += c[i]; if (c[i] > 20.0f) over20++; if (c[i] > 33.4f) over33++; }
    qsort(c, n, sizeof(float), ft_cmp_desc);
    n1 = n / 100 ? n / 100 : 1; n01 = n / 1000 ? n / 1000 : 1;
    for (i = 0; i < n1; i++) s1 += c[i];
    for (i = 0; i < n01; i++) s01 += c[i];
    fprintf(stderr, "[FT] %u frames over %.1f s: average %.1f fps, 1%% low %.1f fps, 0.1%% low %.1f fps, "
            "worst frame %.1f ms, frames over 20 ms: %u, over 33 ms: %u\n",
            n, sum / 1000.0, n * 1000.0 / sum, 1000.0 / (s1 / n1), 1000.0 / (s01 / n01), c[0], over20, over33);
    fflush(stderr);
    free(c);
}

static void m_swap(void)
{
    if (!ensure_device())
        return;
    pace_frame();
    {
        extern void xbox_TscFrameTick(void);    /* RECOMP_FIXED_TIME: 1/60 s per frame */
        xbox_TscFrameTick();
    }
    seg_note_swap();
    rt_note_shown();
    {
        /* The live control file (hle_input.c) is read here too: during some
         * screens the title stops polling the pad. */
        extern void hle_live_poll(void);
        hle_live_poll();
    }
    s_last_swap = (uint32_t)GetTickCount();
    IDirect3DDevice8_EndScene(s_dev);
    ovl_composite();
    present_frame(1);
    pump_messages();
    IDirect3DDevice8_BeginScene(s_dev);
    dump_states();
    {
        /* Frames per second over each 5 s window (always logged; cheap). */
        static uint32_t t0, n0;
        uint32_t t = (uint32_t)GetTickCount();
        if (!t0) { t0 = t; n0 = s_frames; }
        else if (t - t0 >= 5000) {
            extern uint32_t xbox_ContiguousInUse(void);
            extern void xbox_ContiguousReport(void);
            static unsigned nrep;
            if (getenv("CW_CONTIG_REPORT") && (++nrep % 36) == 0)   /* every 3 minutes */
                xbox_ContiguousReport();
            fprintf(stderr, "[FPS] %.1f  gpu-mem %u KB\n", (s_frames - n0) * 1000.0 / (t - t0),
                    xbox_ContiguousInUse() / 1024);
            t0 = t; n0 = s_frames;
        }
    }
    if ((++s_frames % 300) == 1)
        d3d_log("frame %u: draws %u, drawn %u, skipped (vertex shader) %u; vertex programs %d, program draws %u failed %u; off-screen %u",
                s_frames, s_draws, s_draws_done, s_draws_skipped_vs, s_nvsh, s_vsh_draws, s_vsh_fail_draws,
                s_offscreen_draws);
}

/* ---- video overlay ------------------------------------------------------ */

/* The Xbox shows movies through the GPU's video overlay (PVIDEO): the XMV
 * player decodes each frame into a YUY2 surface and hands it to
 * D3DDevice_UpdateOverlay, and the scanout blends it over the frame buffer
 * without any draw call or Swap. Here each update is converted to RGB,
 * drawn over the whole window and presented. */
static IDirect3DTexture8 *s_ovl_tex;
static uint32_t s_ovl_w, s_ovl_h;

static uint8_t clamp8(int v) { return (uint8_t)(v < 0 ? 0 : v > 255 ? 255 : v); }

/* Overlay composition. The Xbox scans the overlay out together with the frame
 * buffer: into DstRect, and -- with the colour key on -- only where the frame
 * buffer holds the key colour (menus cut a window for a video panel that
 * way). A movie with no game frames around it is shown directly; otherwise
 * the latest video frame is composited into each presented frame. */
static uint32_t *s_ovl_rgb;                    /* host copy of the video frame */
static uint32_t  s_ovl_rgb_n;
static int32_t   s_ovl_dst[4];                 /* left, top, right, bottom */
static uint32_t  s_ovl_key, s_ovl_keyen, s_ovl_last, s_last_swap;
static IDirect3DTexture8 *s_comp_tex;

static void ovl_quad(IDirect3DTexture8 *t, float x0, float y0, float x1, float y1)
{
    struct { float x, y, z, rhw; float u, v; } quad[4];
    static const float ux[4] = {0, 1, 0, 1}, vy[4] = {0, 0, 1, 1};
    int i;
    for (i = 0; i < 4; i++) {
        quad[i].x = x0 + ux[i] * (x1 - x0) - 0.5f;
        quad[i].y = y0 + vy[i] * (y1 - y0) - 0.5f;
        quad[i].z = 0.0f; quad[i].rhw = 1.0f;
        quad[i].u = ux[i]; quad[i].v = vy[i];
    }
    IDirect3DDevice8_SetPixelShader(s_dev, 0);
    IDirect3DDevice8_SetVertexShader(s_dev, D3DFVF_XYZRHW | D3DFVF_TEX1);
    IDirect3DDevice8_SetRenderState(s_dev, D3DRS_ZENABLE, FALSE);
    IDirect3DDevice8_SetRenderState(s_dev, D3DRS_ALPHABLENDENABLE, FALSE);
    IDirect3DDevice8_SetRenderState(s_dev, D3DRS_ALPHATESTENABLE, FALSE);
    IDirect3DDevice8_SetRenderState(s_dev, D3DRS_FOGENABLE, FALSE);
    IDirect3DDevice8_SetRenderState(s_dev, D3DRS_STENCILENABLE, FALSE);
    IDirect3DDevice8_SetRenderState(s_dev, D3DRS_COLORWRITEENABLE, 0xF);
    IDirect3DDevice8_SetRenderState(s_dev, D3DRS_CULLMODE, D3DCULL_NONE);
    IDirect3DDevice8_SetRenderState(s_dev, D3DRS_LIGHTING, FALSE);
    IDirect3DDevice8_SetTexture(s_dev, 0, (IDirect3DBaseTexture8 *)t);
    IDirect3DDevice8_SetTextureStageState(s_dev, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    IDirect3DDevice8_SetTextureStageState(s_dev, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    IDirect3DDevice8_SetTextureStageState(s_dev, 0, D3DTSS_TEXCOORDINDEX, 0);
    IDirect3DDevice8_SetTextureStageState(s_dev, 0, D3DTSS_TEXTURETRANSFORMFLAGS, 0);
    IDirect3DDevice8_SetTextureStageState(s_dev, 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    IDirect3DDevice8_SetTextureStageState(s_dev, 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    IDirect3DDevice8_SetTextureStageState(s_dev, 0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
    IDirect3DDevice8_SetTextureStageState(s_dev, 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    IDirect3DDevice8_SetTextureStageState(s_dev, 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    IDirect3DDevice8_SetTextureStageState(s_dev, 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    {
        /* Whole-target quads must not be clipped by the title's viewport. */
        D3DVIEWPORT8 keep, full = { 0, 0, 0, 0, 0.0f, 1.0f };
        IDirect3DSurface8 *rt = NULL;
        D3DSURFACE_DESC d;
        IDirect3DDevice8_GetViewport(s_dev, &keep);
        if (SUCCEEDED(IDirect3DDevice8_GetRenderTarget(s_dev, &rt))) {
            IDirect3DSurface8_GetDesc(rt, &d);
            full.Width = d.Width; full.Height = d.Height;
            IDirect3DSurface8_Release(rt);
            IDirect3DDevice8_SetViewport(s_dev, &full);
        }
        IDirect3DDevice8_DrawPrimitiveUP(s_dev, D3DPT_TRIANGLESTRIP, 2, quad, sizeof quad[0]);
        IDirect3DDevice8_SetViewport(s_dev, &keep);
    }
    IDirect3DDevice8_SetTexture(s_dev, 0, NULL);
}

/* The overlay's DstRect is in the 640x480 layout: map it (movies stay 4:3). */
static void ovl_quad_guest(IDirect3DTexture8 *t)
{
    ovl_quad(t, s_hx0 + (float)s_ovl_dst[0] * s_hs, s_hy0 + (float)s_ovl_dst[1] * s_hs,
             s_hx0 + (float)s_ovl_dst[2] * s_hs, s_hy0 + (float)s_ovl_dst[3] * s_hs);
}

/* ---- present: scene -> back buffer, with FXAA ---------------------------- */

/* Shader tokens (D3D8 encoding). */
#define PS_DST(type, reg, mask, mod) (0x80000000u | ((DWORD)(type) << 28) | (mask) | (mod) | (DWORD)(reg))
#define PS_SRC(type, reg, swz, mod)  (0x80000000u | ((DWORD)(type) << 28) | ((DWORD)(swz) << 16) | (mod) | (DWORD)(reg))
#define R_T 0                                   /* rN */
#define R_C 2                                   /* cN */
#define R_X 3                                   /* tN */
#define M_ALL 0x000F0000u
#define M_RGB 0x00070000u
#define M_A   0x00080000u
#define DM_D2 0x0F000000u                       /* _d2 */
#define SM_X2 0x07000000u                       /* _x2 */
#define SW_ID 0xE4
#define SW_A  0xFF
#define SW_B  0xAA
#define COISSUE 0x40000000u

/* Pass 1 (ps.1.1): colour with luma in alpha. c0 = luma weights. */
static const DWORD k_ps_luma[] = {
    0xFFFF0101u,
    D3DSIO_TEX, PS_DST(R_X, 0, M_ALL, 0),
    D3DSIO_DP3, PS_DST(R_T, 1, M_RGB, 0), PS_SRC(R_X, 0, SW_ID, 0), PS_SRC(R_C, 0, SW_ID, 0),
    D3DSIO_MOV, PS_DST(R_T, 0, M_RGB, 0), PS_SRC(R_X, 0, SW_ID, 0),
    COISSUE | D3DSIO_MOV, PS_DST(R_T, 0, M_A, 0), PS_SRC(R_T, 1, SW_B, 0),
    0x0000FFFFu
};

/* FXAA in shader model 1.4, which has no divide: three passes.
 *  1 (k_ps_luma): colour with luma in alpha.
 *  2 (k_ps_fxdir): the four diagonal neighbours' luma give FXAA's edge
 *    direction, dir = (-(a1+a2), a1-a2) with a1 = NW-SE, a2 = NE-SW. Its
 *    normalisation -- dir / (min(|dir.x|,|dir.y|) + reduce), clamped to 8
 *    pixels -- needs a divide, so it is precomputed: a1, a2 index a 256x256
 *    table (s_fx_lut) that holds the finished direction, encoded 0..1.
 *  3 (k_ps_fxblend): sample the frame at +-dir/6 and +-dir/2 along the edge
 *    and average them as FXAA's "rgbB" (half the inner pair, half the outer).
 * Assembled from scratchpad fxdir.psh / fxblend.psh with the DX8 SDK's
 * psa.exe (it validates; comment tokens dropped). */
static const DWORD k_ps_fxdir[] = {   /* texld r1..r4, t1..t4; sub r1,r1,r4; sub r2,r2,r3;
                                         mad r5,r1.a,c0,c1; mad r5,r2.a,c2,r5; phase; texld r0,r5 */
    0xFFFF0104u, 0x00000042u, 0x800F0001u, 0xB0E40001u, 0x00000042u, 0x800F0002u, 0xB0E40002u,
    0x00000042u, 0x800F0003u, 0xB0E40003u, 0x00000042u, 0x800F0004u, 0xB0E40004u, 0x00000003u,
    0x800F0001u, 0x80E40001u, 0x80E40004u, 0x00000003u, 0x800F0002u, 0x80E40002u, 0x80E40003u,
    0x00000004u, 0x800F0005u, 0x80FF0001u, 0xA0E40000u, 0xA0E40001u, 0x00000004u, 0x800F0005u,
    0x80FF0002u, 0xA0E40002u, 0x80E40005u, 0x0000FFFDu, 0x00000042u, 0x800F0000u, 0x80E40005u,
    0x0000FFFFu
};
static const DWORD k_ps_fxblend[] = { /* texld r0,t0; texcrd r5.rgb,t0; mul r4.rgb,r0_bx2,c0;
                                         add r1.rgb,r5,r4; sub r2.rgb,r5,r4; add r3.rgb,r1,r4_x2;
                                         sub r4.rgb,r2,r4_x2; phase; texld r1..r4,r1..r4;
                                         add_d2 r1,r1,r2; add_d2 r3,r3,r4; add_d2 r0,r1,r3 */
    0xFFFF0104u, 0x00000042u, 0x800F0000u, 0xB0E40000u, 0x00000040u, 0x80070005u, 0xB0E40000u,
    0x00000005u, 0x80070004u, 0x84E40000u, 0xA0E40000u, 0x00000002u, 0x80070001u, 0x80E40005u,
    0x80E40004u, 0x00000003u, 0x80070002u, 0x80E40005u, 0x80E40004u, 0x00000002u, 0x80070003u,
    0x80E40001u, 0x87E40004u, 0x00000003u, 0x80070004u, 0x80E40002u, 0x87E40004u, 0x0000FFFDu,
    0x00000042u, 0x800F0001u, 0x80E40001u, 0x00000042u, 0x800F0002u, 0x80E40002u, 0x00000042u,
    0x800F0003u, 0x80E40003u, 0x00000042u, 0x800F0004u, 0x80E40004u, 0x00000002u, 0x8F0F0001u,
    0x80E40001u, 0x80E40002u, 0x00000002u, 0x8F0F0003u, 0x80E40003u, 0x80E40004u, 0x00000002u,
    0x8F0F0000u, 0x80E40001u, 0x80E40003u, 0x0000FFFFu
};
/* Video overlay with a colour key (menu video panels), done in the first
 * present pass instead of on the CPU: t0 = frame, t1 = video mapped onto the
 * panel. out = |frame - key|^2 > eps ? frame : video; alpha = luma, so the
 * result also feeds FXAA. c0 key colour, c1 eps, c2 luma weights. The CPU
 * version (read the frame back, test every pixel) held menus at 16 fps at
 * 1920x1080. Source: scratchpad keyluma.psh, assembled with psa.exe. */
static const DWORD k_ps_keyluma[] = { /* texld r0,t0; texld r1,t1; sub r2,r0,c0; dp3 r2,r2,r2;
                                         sub r2,r2,c1; cmp r0.rgb,r2,r0,r1; dp3 r3,r0,c2; mov r0.a,r3.b */
    0xFFFF0104u, 0x00000042u, 0x800F0000u, 0xB0E40000u, 0x00000042u, 0x800F0001u, 0xB0E40001u,
    0x00000003u, 0x800F0002u, 0x80E40000u, 0xA0E40000u, 0x00000008u, 0x800F0002u, 0x80E40002u,
    0x80E40002u, 0x00000003u, 0x800F0002u, 0x80E40002u, 0xA0E40001u, 0x00000058u, 0x80070000u,
    0x80E40002u, 0x80E40000u, 0x80E40001u, 0x00000008u, 0x800F0003u, 0x80E40000u, 0xA0E40002u,
    0x00000001u, 0x80080000u, 0x80AA0003u, 0x0000FFFFu
};
static DWORD s_ps_keyluma;
static IDirect3DTexture8 *s_fx_lut, *s_dir_tex;
static IDirect3DSurface8 *s_dir_surf;

/* The direction table: texel (i, j) holds FXAA's normalised direction for
 * a1 = (i + .5) / 256 * 2 - 1, a2 likewise from j, as (dx, dy) / 8 * .5 + .5
 * in red and green. */
static int fx_lut_fill(void)
{
    D3DLOCKED_RECT lr;
    int i, j;
    if (FAILED(IDirect3DTexture8_LockRect(s_fx_lut, 0, &lr, NULL, 0))) return 0;
    for (j = 0; j < 256; j++) {
        uint32_t *row = (uint32_t *)((uint8_t *)lr.pBits + (size_t)j * lr.Pitch);
        float a2 = ((float)j + 0.5f) / 128.0f - 1.0f;
        for (i = 0; i < 256; i++) {
            float a1 = ((float)i + 0.5f) / 128.0f - 1.0f;
            float dx = -(a1 + a2), dy = a1 - a2;
            float ax = dx < 0 ? -dx : dx, ay = dy < 0 ? -dy : dy;
            float rcp = 1.0f / ((ax < ay ? ax : ay) + 1.0f / 24.0f);   /* FXAA dirReduce */
            int r, g;
            dx *= rcp; dy *= rcp;
            if (dx > 8.0f) dx = 8.0f;
            if (dx < -8.0f) dx = -8.0f;
            if (dy > 8.0f) dy = 8.0f;
            if (dy < -8.0f) dy = -8.0f;
            r = (int)((dx / 8.0f * 0.5f + 0.5f) * 255.0f + 0.5f);
            g = (int)((dy / 8.0f * 0.5f + 0.5f) * 255.0f + 0.5f);
            row[i] = 0xFF000000u | ((uint32_t)r << 16) | ((uint32_t)g << 8);
        }
    }
    IDirect3DTexture8_UnlockRect(s_fx_lut, 0);
    return 1;
}
static void post_init(void)
{
    HRESULT hr;
    hr = IDirect3DDevice8_CreateTexture(s_dev, (UINT)s_bbw, (UINT)s_bbh, 1, D3DUSAGE_RENDERTARGET,
                                        D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &s_scene_tex);
    if (SUCCEEDED(hr))
        hr = IDirect3DDevice8_CreateDepthStencilSurface(s_dev, (UINT)s_bbw, (UINT)s_bbh, D3DFMT_D24S8,
                                                        D3DMULTISAMPLE_NONE, &s_scene_ds);
    if (SUCCEEDED(hr))
        hr = IDirect3DTexture8_GetSurfaceLevel(s_scene_tex, 0, &s_scene_surf);
    if (SUCCEEDED(hr))
        hr = IDirect3DDevice8_SetRenderTarget(s_dev, s_scene_surf, s_scene_ds);
    if (FAILED(hr)) {
        d3d_log("scene target failed hr=0x%08lX -- drawing to the back buffer, no FXAA", (unsigned long)hr);
        if (s_scene_surf) IDirect3DSurface8_Release(s_scene_surf);
        if (s_scene_ds) IDirect3DSurface8_Release(s_scene_ds);
        if (s_scene_tex) IDirect3DTexture8_Release(s_scene_tex);
        s_scene_surf = s_scene_ds = NULL; s_scene_tex = NULL;
        s_fxaa = 0;
        return;
    }
    IDirect3DDevice8_Clear(s_dev, 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1.0f, 0);
    {
        D3DCAPS8 caps;
        if (SUCCEEDED(IDirect3DDevice8_GetDeviceCaps(s_dev, &caps)) && (caps.PixelShaderVersion & 0xFFFF) >= 0x0104 &&
            FAILED(IDirect3DDevice8_CreatePixelShader(s_dev, k_ps_keyluma, &s_ps_keyluma)))
            s_ps_keyluma = 0;
    }
    if (s_fxaa) {
        hr = IDirect3DDevice8_CreateTexture(s_dev, (UINT)s_bbw, (UINT)s_bbh, 1, D3DUSAGE_RENDERTARGET,
                                            D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &s_luma_tex);
        const char *step = "luma target";
        if (SUCCEEDED(hr)) hr = IDirect3DTexture8_GetSurfaceLevel(s_luma_tex, 0, &s_luma_surf);
        if (SUCCEEDED(hr)) {
            step = "direction target";
            hr = IDirect3DDevice8_CreateTexture(s_dev, (UINT)s_bbw, (UINT)s_bbh, 1, D3DUSAGE_RENDERTARGET,
                                                D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &s_dir_tex);
        }
        if (SUCCEEDED(hr)) hr = IDirect3DTexture8_GetSurfaceLevel(s_dir_tex, 0, &s_dir_surf);
        if (SUCCEEDED(hr)) {
            step = "direction table";
            hr = IDirect3DDevice8_CreateTexture(s_dev, 256, 256, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &s_fx_lut);
            if (SUCCEEDED(hr) && !fx_lut_fill()) hr = E_FAIL;
        }
        if (SUCCEEDED(hr)) { step = "luma shader"; hr = IDirect3DDevice8_CreatePixelShader(s_dev, k_ps_luma, &s_ps_luma); }
        if (SUCCEEDED(hr)) { step = "direction shader"; hr = IDirect3DDevice8_CreatePixelShader(s_dev, k_ps_fxdir, &s_ps_fxdir); }
        if (SUCCEEDED(hr)) { step = "blend shader"; hr = IDirect3DDevice8_CreatePixelShader(s_dev, k_ps_fxblend, &s_ps_fxaa); }
        if (FAILED(hr)) {
            d3d_log("FXAA setup failed at %s hr=0x%08lX -- off", step, (unsigned long)hr);
            s_fxaa = 0;
        }
    }
}

static void post_states(void)
{
    int st;
    IDirect3DDevice8_SetRenderState(s_dev, D3DRS_ZENABLE, FALSE);
    IDirect3DDevice8_SetRenderState(s_dev, D3DRS_ZWRITEENABLE, FALSE);
    IDirect3DDevice8_SetRenderState(s_dev, D3DRS_ALPHABLENDENABLE, FALSE);
    IDirect3DDevice8_SetRenderState(s_dev, D3DRS_ALPHATESTENABLE, FALSE);
    IDirect3DDevice8_SetRenderState(s_dev, D3DRS_FOGENABLE, FALSE);
    IDirect3DDevice8_SetRenderState(s_dev, D3DRS_STENCILENABLE, FALSE);
    IDirect3DDevice8_SetRenderState(s_dev, D3DRS_COLORWRITEENABLE, 0xF);
    IDirect3DDevice8_SetRenderState(s_dev, D3DRS_CULLMODE, D3DCULL_NONE);
    IDirect3DDevice8_SetRenderState(s_dev, D3DRS_LIGHTING, FALSE);
    IDirect3DDevice8_SetRenderState(s_dev, D3DRS_SPECULARENABLE, FALSE);
    for (st = 0; st < 5; st++) {
        IDirect3DDevice8_SetTextureStageState(s_dev, st, D3DTSS_TEXCOORDINDEX, (DWORD)st);
        IDirect3DDevice8_SetTextureStageState(s_dev, st, D3DTSS_TEXTURETRANSFORMFLAGS, 0);
        IDirect3DDevice8_SetTextureStageState(s_dev, st, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
        IDirect3DDevice8_SetTextureStageState(s_dev, st, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
        IDirect3DDevice8_SetTextureStageState(s_dev, st, D3DTSS_MIPFILTER, D3DTEXF_NONE);
        IDirect3DDevice8_SetTextureStageState(s_dev, st, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
        IDirect3DDevice8_SetTextureStageState(s_dev, st, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    }
}

/* Full-target quad with five texture coordinate sets: the pixel and its four
 * diagonal neighbours. */
static void post_quad_ex(int w, int h, const float *vid)
{
    struct { float x, y, z, rhw; float t[5][2]; } q[4];
    static const float ux[4] = {0, 1, 0, 1}, vy[4] = {0, 0, 1, 1};
    static const float ox[5] = {0, -1, 1, -1, 1}, oy[5] = {0, -1, -1, 1, 1};
    float px = 1.0f / (float)w, py = 1.0f / (float)h;
    D3DVIEWPORT8 full = { 0, 0, (DWORD)w, (DWORD)h, 0.0f, 1.0f };
    int i, k;
    for (i = 0; i < 4; i++) {
        q[i].x = ux[i] * (float)w - 0.5f;
        q[i].y = vy[i] * (float)h - 0.5f;
        q[i].z = 0.0f; q[i].rhw = 1.0f;
        for (k = 0; k < 5; k++) {
            q[i].t[k][0] = ux[i] + ox[k] * px;
            q[i].t[k][1] = vy[i] + oy[k] * py;
        }
        if (vid) {          /* t1: video coordinates, 0..1 across the panel (x0 y0 x1 y1 in pixels) */
            q[i].t[1][0] = (ux[i] * (float)w - vid[0]) / (vid[2] - vid[0]);
            q[i].t[1][1] = (vy[i] * (float)h - vid[1]) / (vid[3] - vid[1]);
        }
    }
    IDirect3DDevice8_SetViewport(s_dev, &full);
    IDirect3DDevice8_SetVertexShader(s_dev, D3DFVF_XYZRHW | D3DFVF_TEX5);
    IDirect3DDevice8_DrawPrimitiveUP(s_dev, D3DPT_TRIANGLESTRIP, 2, q, sizeof q[0]);
}
static void post_quad(int w, int h) { post_quad_ex(w, h, NULL); }

/* A colour-keyed video overlay is being fed (menu panels): composite it in
 * the first present pass. */
static int ovl_keyed_gpu(void)
{
    return s_ps_keyluma && s_scene_surf && s_ovl_tex && s_ovl_rgb && s_ovl_keyen &&
           (uint32_t)GetTickCount() - s_ovl_last <= 250 &&
           s_ovl_dst[2] > s_ovl_dst[0] && s_ovl_dst[3] > s_ovl_dst[1];
}

/* Draw the frame with the overlay keyed in, luma in alpha (current target). */
static void post_keyed_pass(void)
{
    const float lumaw[4] = { 0.299f, 0.587f, 0.114f, 0.0f };
    uint32_t k = s_ovl_key;
    float key[4] = { (float)((k >> 16) & 0xFF) / 255.0f, (float)((k >> 8) & 0xFF) / 255.0f,
                     (float)(k & 0xFF) / 255.0f, 0.0f };
    float eps[4] = { 0.5f / 65025.0f, 0.5f / 65025.0f, 0.5f / 65025.0f, 0.5f / 65025.0f };
    float vid[4] = { s_hx0 + (float)s_ovl_dst[0] * s_hs, s_hy0 + (float)s_ovl_dst[1] * s_hs,
                     s_hx0 + (float)s_ovl_dst[2] * s_hs, s_hy0 + (float)s_ovl_dst[3] * s_hs };
    IDirect3DDevice8_SetPixelShader(s_dev, s_ps_keyluma);
    IDirect3DDevice8_SetPixelShaderConstant(s_dev, 0, key, 1);
    IDirect3DDevice8_SetPixelShaderConstant(s_dev, 1, eps, 1);
    IDirect3DDevice8_SetPixelShaderConstant(s_dev, 2, lumaw, 1);
    IDirect3DDevice8_SetTexture(s_dev, 0, (IDirect3DBaseTexture8 *)s_scene_tex);
    IDirect3DDevice8_SetTexture(s_dev, 1, (IDirect3DBaseTexture8 *)s_ovl_tex);
    post_quad_ex(s_bbw, s_bbh, vid);
}

/* End of frame: the scene goes to the back buffer (through FXAA when on),
 * which is what screenshots read and what is presented. The scene target and
 * the title's viewport are then put back for the next frame. */
static void present_frame(int from_swap)
{
    IDirect3DSurface8 *bb = NULL;
    if (s_scene_surf && SUCCEEDED(IDirect3DDevice8_GetBackBuffer(s_dev, 0, D3DBACKBUFFER_TYPE_MONO, &bb))) {
        int st;
        post_states();
        if (s_fxaa) {
            static const float luma[4] = { 0.299f, 0.587f, 0.114f, 0.0f };
            static const float lut[3][4] = { { 0.5f, 0, 0, 0 }, { 0.5f, 0.5f, 0, 0 }, { 0, 0.5f, 0, 0 } };
            /* direction (texels, /8 encoded) -> texture step for +-dir/6 */
            float step[4] = { 8.0f / 6.0f / (float)s_bbw, 8.0f / 6.0f / (float)s_bbh, 0, 0 };
            /* 1: luma into alpha */
            IDirect3DDevice8_SetRenderTarget(s_dev, s_luma_surf, NULL);
            IDirect3DDevice8_BeginScene(s_dev);
            if (ovl_keyed_gpu()) {
                post_keyed_pass();                  /* video panel keyed in here */
            } else {
                IDirect3DDevice8_SetPixelShader(s_dev, s_ps_luma);
                IDirect3DDevice8_SetPixelShaderConstant(s_dev, 0, luma, 1);
                IDirect3DDevice8_SetTexture(s_dev, 0, (IDirect3DBaseTexture8 *)s_scene_tex);
                post_quad(s_bbw, s_bbh);
            }
            IDirect3DDevice8_EndScene(s_dev);
            /* 2: edge direction, normalised through the table */
            IDirect3DDevice8_SetRenderTarget(s_dev, s_dir_surf, NULL);
            IDirect3DDevice8_BeginScene(s_dev);
            IDirect3DDevice8_SetPixelShader(s_dev, s_ps_fxdir);
            IDirect3DDevice8_SetPixelShaderConstant(s_dev, 0, lut, 3);
            IDirect3DDevice8_SetTexture(s_dev, 0, (IDirect3DBaseTexture8 *)s_fx_lut);
            for (st = 1; st < 5; st++)
                IDirect3DDevice8_SetTexture(s_dev, (DWORD)st, (IDirect3DBaseTexture8 *)s_luma_tex);
            post_quad(s_bbw, s_bbh);
            IDirect3DDevice8_EndScene(s_dev);
            /* 3: blend along the edge into the back buffer */
            IDirect3DDevice8_SetRenderTarget(s_dev, bb, NULL);
            IDirect3DDevice8_BeginScene(s_dev);
            IDirect3DDevice8_SetPixelShader(s_dev, s_ps_fxaa);
            IDirect3DDevice8_SetPixelShaderConstant(s_dev, 0, step, 1);
            IDirect3DDevice8_SetTexture(s_dev, 0, (IDirect3DBaseTexture8 *)s_dir_tex);
            /* colours from pass 1's output: the frame with any video keyed in */
            for (st = 1; st < 5; st++)
                IDirect3DDevice8_SetTexture(s_dev, (DWORD)st, (IDirect3DBaseTexture8 *)s_luma_tex);
            post_quad(s_bbw, s_bbh);
            IDirect3DDevice8_EndScene(s_dev);
            for (st = 0; st < 5; st++)
                IDirect3DDevice8_SetTexture(s_dev, (DWORD)st, NULL);
            IDirect3DDevice8_SetPixelShader(s_dev, 0);
        } else {
            IDirect3DDevice8_SetRenderTarget(s_dev, bb, NULL);
            IDirect3DDevice8_BeginScene(s_dev);
            if (ovl_keyed_gpu()) {
                post_keyed_pass();
                IDirect3DDevice8_SetTexture(s_dev, 1, NULL);
                IDirect3DDevice8_SetPixelShader(s_dev, 0);
            } else {
                ovl_quad(s_scene_tex, 0.0f, 0.0f, (float)s_bbw, (float)s_bbh);
            }
            IDirect3DDevice8_EndScene(s_dev);
        }
        IDirect3DDevice8_SetTextureStageState(s_dev, 4, D3DTSS_COLOROP, D3DTOP_DISABLE);
    }
    dump_backbuffer();
    if (from_swap) capture_poll();
    IDirect3DDevice8_Present(s_dev, NULL, NULL, NULL, NULL);
    ft_note_present();
    if (bb) {
        IDirect3DDevice8_SetRenderTarget(s_dev, s_scene_surf, s_scene_ds);
        IDirect3DSurface8_Release(bb);
        if (s_k >= 1.0f && (s_bbw != 640 || s_bbh != 480)) {
            /* 4:3 picture on a wider target: keep the side bars black. */
            D3DVIEWPORT8 full = { 0, 0, (DWORD)s_bbw, (DWORD)s_bbh, 0.0f, 1.0f };
            IDirect3DDevice8_SetViewport(s_dev, &full);
            IDirect3DDevice8_Clear(s_dev, 0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0);
        }
        vp_apply();
    }
}

/* Called from m_swap between EndScene and Present. */
static void ovl_composite(void)
{
    uint32_t now = (uint32_t)GetTickCount();
    if (!s_ovl_tex || !s_ovl_rgb || now - s_ovl_last > 250)
        return;                                   /* no overlay being fed */
    if (!s_ovl_keyen) {
        IDirect3DDevice8_BeginScene(s_dev);
        ovl_quad_guest(s_ovl_tex);
        IDirect3DDevice8_EndScene(s_dev);
        return;
    }
    if (ovl_keyed_gpu())
        return;                                   /* keyed in by present_frame */
    {
        /* Key test on the CPU: read the frame back, put video pixels where it
         * holds the key colour, draw the result over the whole frame. The
         * frame is at render size; DstRect is in the 640x480 layout. */
        IDirect3DSurface8 *bb = NULL, *img = NULL;
        D3DSURFACE_DESC desc;
        D3DLOCKED_RECT src, dst;
        int32_t x, y, dw = s_ovl_dst[2] - s_ovl_dst[0], dh = s_ovl_dst[3] - s_ovl_dst[1];
        uint32_t key = s_ovl_key & 0xFFFFFFu;
        float inv = 1.0f / s_hs;
        if (dw <= 0 || dh <= 0) return;
        if (s_scene_surf) { bb = s_scene_surf; IDirect3DSurface8_AddRef(bb); }
        else if (FAILED(IDirect3DDevice8_GetBackBuffer(s_dev, 0, D3DBACKBUFFER_TYPE_MONO, &bb))) return;
        IDirect3DSurface8_GetDesc(bb, &desc);
        if (!s_comp_tex &&
            FAILED(IDirect3DDevice8_CreateTexture(s_dev, desc.Width, desc.Height, 1, 0, D3DFMT_X8R8G8B8,
                                                  D3DPOOL_MANAGED, &s_comp_tex))) {
            IDirect3DSurface8_Release(bb); return;
        }
        if (FAILED(IDirect3DDevice8_CreateImageSurface(s_dev, desc.Width, desc.Height, desc.Format, &img))) {
            IDirect3DSurface8_Release(bb); return;
        }
        if (SUCCEEDED(IDirect3DDevice8_CopyRects(s_dev, bb, NULL, 0, img, NULL)) &&
            SUCCEEDED(IDirect3DSurface8_LockRect(img, &src, NULL, D3DLOCK_READONLY))) {
            if (SUCCEEDED(IDirect3DTexture8_LockRect(s_comp_tex, 0, &dst, NULL, 0))) {
                for (y = 0; y < (int32_t)desc.Height; y++) {
                    const uint32_t *sr = (const uint32_t *)((const uint8_t *)src.pBits + y * src.Pitch);
                    uint32_t *dr = (uint32_t *)((uint8_t *)dst.pBits + y * dst.Pitch);
                    int32_t gy = (int32_t)(((float)y + 0.5f - s_hy0) * inv);
                    for (x = 0; x < (int32_t)desc.Width; x++) {
                        uint32_t c = sr[x];
                        int32_t gx = (int32_t)(((float)x + 0.5f - s_hx0) * inv);
                        if ((c & 0xFFFFFFu) == key && gx >= s_ovl_dst[0] && gx < s_ovl_dst[2] &&
                            gy >= s_ovl_dst[1] && gy < s_ovl_dst[3]) {
                            uint32_t vx = (uint32_t)((gx - s_ovl_dst[0]) * (int32_t)s_ovl_w / dw);
                            uint32_t vy = (uint32_t)((gy - s_ovl_dst[1]) * (int32_t)s_ovl_h / dh);
                            c = s_ovl_rgb[vy * s_ovl_w + vx];
                        }
                        dr[x] = c;
                    }
                }
                IDirect3DTexture8_UnlockRect(s_comp_tex, 0);
            }
            IDirect3DSurface8_UnlockRect(img);
            IDirect3DDevice8_BeginScene(s_dev);
            ovl_quad(s_comp_tex, 0.0f, 0.0f, (float)desc.Width, (float)desc.Height);
            IDirect3DDevice8_EndScene(s_dev);
        }
        IDirect3DSurface8_Release(img);
        IDirect3DSurface8_Release(bb);
    }
}

static void m_update_overlay(const uint32_t *a)
{
    uint32_t surf = a[0], data, fmt, size, w, h, pitch, y, x;
    const uint8_t *src;
    D3DLOCKED_RECT lr;
    struct { float x, y, z, rhw; float u, v; } quad[4];

    if (!surf || !ensure_device())
        return;
    data  = phys_to_va(MEM32(surf + 4));
    fmt   = (MEM32(surf + 12) >> 8) & 0xFF;
    size  = MEM32(surf + 16);
    w     = (size & 0xFFF) + 1;
    h     = ((size >> 12) & 0xFFF) + 1;
    pitch = (((size >> 24) & 0xFF) + 1) * 64;
    if (!data || w > 2048 || h > 2048)
        return;
    if (!s_ovl_tex || s_ovl_w != w || s_ovl_h != h) {
        if (s_ovl_tex)
            IDirect3DTexture8_Release(s_ovl_tex);
        s_ovl_tex = NULL;
        if (FAILED(IDirect3DDevice8_CreateTexture(s_dev, w, h, 1, 0, D3DFMT_X8R8G8B8,
                                                  D3DPOOL_MANAGED, &s_ovl_tex)))
            return;
        s_ovl_w = w; s_ovl_h = h;
        d3d_log("overlay %ux%u format 0x%02X pitch %u", w, h, fmt, pitch);
    }
    if (!s_ovl_rgb || s_ovl_rgb_n < w * h) {
        free(s_ovl_rgb);
        s_ovl_rgb = (uint32_t *)malloc((size_t)w * h * 4);
        s_ovl_rgb_n = s_ovl_rgb ? w * h : 0;
        if (!s_ovl_rgb) return;
    }
    if (FAILED(IDirect3DTexture8_LockRect(s_ovl_tex, 0, &lr, NULL, 0)))
        return;
    src = (const uint8_t *)gptr(data);
    for (y = 0; y < h; y++) {
        const uint8_t *s = src + (size_t)y * pitch;
        uint32_t *d = (uint32_t *)((uint8_t *)lr.pBits + (size_t)y * lr.Pitch);
        for (x = 0; x + 1 < w; x += 2, s += 4) {
            /* YUY2: Y0 U Y1 V, BT.601 studio range. */
            int u = s[1] - 128, v = s[3] - 128, k;
            for (k = 0; k < 2; k++) {
                int c = 298 * ((k ? s[2] : s[0]) - 16);
                d[x + k] = s_ovl_rgb[y * w + x + k] = 0xFF000000u
                         | ((uint32_t)clamp8((c + 409 * v + 128) >> 8) << 16)
                         | ((uint32_t)clamp8((c - 100 * u - 208 * v + 128) >> 8) << 8)
                         |  (uint32_t)clamp8((c + 516 * u + 128) >> 8);
            }
        }
    }
    IDirect3DTexture8_UnlockRect(s_ovl_tex, 0);

    /* Destination rectangle (null = whole screen), colour key. */
    if (a[2]) {
        s_ovl_dst[0] = (int32_t)MEM32(a[2]);     s_ovl_dst[1] = (int32_t)MEM32(a[2] + 4);
        s_ovl_dst[2] = (int32_t)MEM32(a[2] + 8); s_ovl_dst[3] = (int32_t)MEM32(a[2] + 12);
    } else {
        s_ovl_dst[0] = 0; s_ovl_dst[1] = 0; s_ovl_dst[2] = 640; s_ovl_dst[3] = 480;
    }
    s_ovl_keyen = a[3];
    s_ovl_key = a[4];
    s_ovl_last = (uint32_t)GetTickCount();
    (void)quad;
    if (s_ovl_last - s_last_swap > 500) {
        /* A bare movie: the game presents nothing else, so show it now. */
        D3DVIEWPORT8 full = { 0, 0, (DWORD)s_bbw, (DWORD)s_bbh, 0.0f, 1.0f };
        IDirect3DDevice8_EndScene(s_dev);
        IDirect3DDevice8_SetViewport(s_dev, &full);
        IDirect3DDevice8_Clear(s_dev, 0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0);
        vp_apply();
        IDirect3DDevice8_BeginScene(s_dev);
        ovl_quad_guest(s_ovl_tex);
        IDirect3DDevice8_EndScene(s_dev);
        present_frame(0);
        pump_messages();
        IDirect3DDevice8_BeginScene(s_dev);
    }
}

#define MIRROR_CLEAR        m_clear(a)
#define MIRROR_SWAP         m_swap()
#define MIRROR_SETTEX       do { if (a[0] < 4) s_tex[a[0]] = a[1]; } while (0)
#define MIRROR_STREAM       do { if (a[0] < 16) { s_stream_vb[a[0]] = a[1]; s_stream_stride[a[0]] = a[2]; } } while (0)
#define MIRROR_INDICES      (s_ib_base = a[1])
#define MIRROR_VSHADER      (s_vshader = a[0])
#define MIRROR_PSHADER      (s_pshader = a[0])
#define MIRROR_DRAW         draw(a[0], a[2], a[1], 0)
#define MIRROR_DRAWIDX      draw(a[0], a[1], 0, a[2])
#define MIRROR_TRANSFORM    m_set_transform(a[0], a[1])
#define MIRROR_VIEWPORT     m_set_viewport(a[0])
#define MIRROR_OVERLAY      m_update_overlay(a)
#define MIRROR_CREATEVS     vsh_capture(a[0], a[1], a[2])
#define MIRROR_SETLIGHT     m_set_light(a[0], a[1])
#define MIRROR_LIGHTENABLE  m_light_enable(a[0], a[1])
#define MIRROR_SETMATERIAL  m_set_material(a[0])

#else  /* no d3d8: pass-through wrappers */

#define MIRROR_CLEAR        ((void)0)
#define MIRROR_SWAP         ((void)0)
#define MIRROR_SETTEX       ((void)0)
#define MIRROR_STREAM       ((void)0)
#define MIRROR_INDICES      ((void)0)
#define MIRROR_VSHADER      ((void)0)
#define MIRROR_PSHADER      ((void)0)
#define MIRROR_DRAW         ((void)0)
#define MIRROR_DRAWIDX      ((void)0)
#define MIRROR_TRANSFORM    ((void)0)
#define MIRROR_VIEWPORT     ((void)0)
#define MIRROR_OVERLAY      ((void)0)
#define MIRROR_CREATEVS     ((void)0)
#define MIRROR_SETLIGHT     ((void)0)
#define MIRROR_LIGHTENABLE  ((void)0)
#define MIRROR_SETMATERIAL  ((void)0)

volatile int hle_shot_request;   /* live-control SHOT: no host renderer to capture */
volatile int hle_capture_request;
unsigned hle_frame_count(void) { return 0; }
void hle_set_fxaa(int on) { (void)on; }
void hle_skip_vs(uint32_t h) { (void)h; }
void hle_show_va(uint32_t h) { (void)h; }
void hle_dump_vc(uint32_t h) { (void)h; }
void hle_vs_tablefog(int on) { (void)on; }
void hle_ft_reset(void) { }
void hle_ft_report(void) { }
void sub_00227960_gen(void);
void sub_00227960(void) { sub_00227960_gen(); }
void hle_frame_key(unsigned *seg, unsigned *off) { *seg = 0; *off = 0; }

#endif

/* ---- wrapped entry points ------------------------------------------------ */

/* 0x002DC250 D3DDevice_Clear(Count, pRects, Flags, Color, Z, Stencil) */
void sub_002DC250_gen(void);
void sub_002DC250(void) { D3D_WRAP_BODY(sub_002DC250_gen, 6, MIRROR_CLEAR) }

/* 0x002E0870 D3DDevice_Swap(Flags) */
void sub_002E0870_gen(void);
void sub_002E0870(void) { D3D_WRAP_BODY(sub_002E0870_gen, 1, MIRROR_SWAP) }

/* 0x002D6FE0 D3DDevice_SetTexture(Stage, pTexture) */
void sub_002D6FE0_gen(void);
void sub_002D6FE0(void) { D3D_WRAP_BODY(sub_002D6FE0_gen, 2, MIRROR_SETTEX) }

/* 0x002D82B0 D3DDevice_SetStreamSource(StreamNumber, pStreamData, Stride) */
void sub_002D82B0_gen(void);
void sub_002D82B0(void) { D3D_WRAP_BODY(sub_002D82B0_gen, 3, MIRROR_STREAM) }

/* 0x002D7190 D3DDevice_SetIndices(pIndexData, BaseVertexIndex) */
void sub_002D7190_gen(void);
void sub_002D7190(void) { D3D_WRAP_BODY(sub_002D7190_gen, 2, MIRROR_INDICES) }

/* 0x002D8690 D3DDevice_SetVertexShader(Handle) */
void sub_002D8690_gen(void);
void sub_002D8690(void) { D3D_WRAP_BODY(sub_002D8690_gen, 1, MIRROR_VSHADER) }

/* 0x002E0E70 D3DDevice_SetPixelShader(Handle) */
void sub_002E0E70_gen(void);
void sub_002E0E70(void) { D3D_WRAP_BODY(sub_002E0E70_gen, 1, MIRROR_PSHADER) }

/* 0x002E0A20 D3DDevice_DrawVertices(PrimitiveType, StartVertex, VertexCount) */
void sub_002E0A20_gen(void);
void sub_002E0A20(void) { D3D_WRAP_BODY(sub_002E0A20_gen, 3, MIRROR_DRAW) }

/* 0x002E0AC0 D3DDevice_DrawIndexedVertices(PrimitiveType, VertexCount, pIndexData) */
void sub_002E0AC0_gen(void);
void sub_002E0AC0(void) { D3D_WRAP_BODY(sub_002E0AC0_gen, 3, MIRROR_DRAWIDX) }

/* 0x002D62E0 D3DDevice_SetTransform(State, pMatrix) */
void sub_002D62E0_gen(void);
void sub_002D62E0(void)
{
    uint32_t ret = MEM32(g_esp);
    D3D_WRAP_BODY(sub_002D62E0_gen, 2, MIRROR_TRANSFORM)
    if (a[0] == 1 && a[1] && getenv("CW_PROJ_LOG")) {
        /* CW_PROJ_LOG=1: who sets the projection, and to what (first 8). */
        static int n;
        if (n++ < 8) {
            const float *m = (const float *)XBOX_PTR(a[1]);
            fprintf(stderr, "[PROJ] from %08X: %g %g %g %g / %g %g %g %g / %g %g %g %g / %g %g %g %g\n", ret,
                    m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11],
                    m[12], m[13], m[14], m[15]);
            fflush(stderr);
        }
    }
}

/* 0x002D6BB0 D3DDevice_SetViewport(pViewport) */
void sub_002D6BB0_gen(void);
void sub_002D6BB0(void) { D3D_WRAP_BODY(sub_002D6BB0_gen, 1, MIRROR_VIEWPORT) }

/* 0x002D3690 D3DDevice_SetRenderState_Simple(ecx = push-buffer method header,
 * edx = value). The encode table at 0x00334894 lists each simple state's
 * header in render-state order from 57. */
void sub_002D3690_gen(void);
void sub_002D3690(void)
{
    uint32_t method = g_ecx, value = g_edx;
    sub_002D3690_gen();
#if HLE_D3D8_AVAILABLE
    {
        int k;
        for (k = 0; k < 35; k++) {
            if (MEM32(0x00334894u + 4u * (uint32_t)k) == method) {
                s_simple_val[k] = value;
                s_simple_arr[k] = MEM32(0x002E4250u + 4u * (uint32_t)(57 + k));
                s_simple_set[k] = 1;
                break;
            }
        }
    }
#else
    (void)method; (void)value;
#endif
}

/* 0x002D45B0 D3D_CommonSetRenderTarget(pRenderTarget, pZStencil, pDevice); a
 * null target keeps the current one. Tracked in both builds (cheap). */
void sub_002D45B0_gen(void);
void sub_002D45B0(void)
{
    uint32_t rt = ARG(0), z = ARG(1);
    sub_002D45B0_gen();
#if HLE_D3D8_AVAILABLE
    if (rt)
        s_cur_rt = rt;
    s_cur_z = z;
    {
        static uint32_t seen[32][2];
        static int n;
        int k;
        for (k = 0; k < n; k++) if (seen[k][0] == rt && seen[k][1] == z) break;
        if (k == n && n < 32 && getenv("CW_RT_LOG")) {
            seen[n][0] = rt; seen[n][1] = z; n++;
            fprintf(stderr, "[RT] target %08X (fmt %X size %08X) depth %08X (fmt %X size %08X) frame %u\n",
                    rt, rt ? (MEM32(rt + 0xC) >> 8) & 0xFF : 0, rt ? MEM32(rt + 0x10) : 0,
                    z, z ? (MEM32(z + 0xC) >> 8) & 0xFF : 0, z ? MEM32(z + 0x10) : 0, s_frames);
        }
    }
#else
    (void)rt;
#endif
}

/* 0x002D6D10 D3DDevice_SetLight(Index, pLight) */
void sub_002D6D10_gen(void);
void sub_002D6D10(void) { D3D_WRAP_BODY(sub_002D6D10_gen, 2, MIRROR_SETLIGHT) }

/* 0x002D6EF0 D3DDevice_LightEnable(Index, bEnable) */
void sub_002D6EF0_gen(void);
void sub_002D6EF0(void) { D3D_WRAP_BODY(sub_002D6EF0_gen, 2, MIRROR_LIGHTENABLE) }

/* 0x002D6440 D3DDevice_SetMaterial(pMaterial) */
void sub_002D6440_gen(void);
void sub_002D6440(void) { D3D_WRAP_BODY(sub_002D6440_gen, 1, MIRROR_SETMATERIAL) }

/* 0x002D7F20 D3DDevice_CreateVertexShader(pDeclaration, pFunction, pHandle, Usage) */
void sub_002D7F20_gen(void);
void sub_002D7F20(void) { D3D_WRAP_BODY(sub_002D7F20_gen, 4, MIRROR_CREATEVS) }

/* 0x002E15A0 D3DDevice_UpdateOverlay(pSurface, SrcRect, DstRect, EnableColorKey, ColorKey) */
void sub_002E15A0_gen(void);
void sub_002E15A0(void) { D3D_WRAP_BODY(sub_002E15A0_gen, 5, MIRROR_OVERLAY) }

/* 0x002D7350 D3DDevice_GetDisplayFieldStatus(pFieldStatus) -- both builds.
 *
 * This D3D keeps no vblank counter of its own: the device word the title's
 * version reports (+0x1988) only advances inside Swap, kept in step with the
 * encoder's field, and its field bit (+0x1998) is never written at all. The
 * XMV player times frames on VBlankCount and starts a movie only on an even
 * field, so with Swap running below 60 Hz under the runtime the movie ran at
 * a third of real time, and with the field stuck ODD it never started.
 * Report the runtime's own 60 Hz vblank count instead, and the field from its
 * parity -- what the hardware would have said. */
extern volatile unsigned long xbox_av_field_counter;
void sub_002D7350(void)
{
    uint32_t dev = MEM32(0x002E44E8);          /* D3D__pDevice */
    uint32_t out = ARG(0);                     /* D3DFIELD_STATUS* */
    uint32_t count = (uint32_t)xbox_av_field_counter;
    MEM32(out + 4) = count;                    /* VBlankCount */
    if (MEM32(dev + 0x197C) & 0x01200000u)     /* interlaced / field mode */
        MEM32(out) = (count & 1) ? 1u : 2u;    /* D3DFIELD_ODD : D3DFIELD_EVEN */
    else
        MEM32(out) = 3u;                       /* D3DFIELD_PROGRESSIVE */
    g_esp += 8;                                /* ret 4 */
}
