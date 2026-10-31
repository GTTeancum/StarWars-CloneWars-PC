/*
 * input.c -- controller input for the recompiled title.
 *
 * The title links XAPI's USB gamepad stack, which talks to OHCI hardware
 * that does not exist here, so no pad is ever reported. These wrappers
 * replace the five XAPI entry points the game uses with a virtual gamepad
 * on port 0 (and ports 1-3 when a host pad is plugged in there):
 *
 *   0x0032D70D  XGetDeviceChanges(type, *insertions, *removals)  ret 0xC
 *   0x003332F7  XInputOpen(type, port, slot, params)             ret 0x10
 *   0x0033334D  XInputClose(handle)                              ret 4
 *   0x00333359  XInputGetCapabilities(handle, caps)              ret 8
 *   0x00333537  XInputGetState(handle, state)                    ret 8
 *   0x003335A3  XInputSetState(handle, feedback)                 ret 8
 *
 * Device types other than the gamepad (0x0032B878) -- memory units, voice --
 * fall through to the original code, which reports nothing connected.
 *
 * State comes from Windows XInput, OR'd with the keyboard while the game
 * window has focus (Enter=START, Esc=BACK, arrows=D-pad, Z/X/C/V=A/B/X/Y,
 * Q/E=triggers, WASD=left stick), OR'd with CW_INPUT_SCRIPT for unattended
 * runs: "sec:BUTTON[,sec:BUTTON...]", seconds since the game first polled a
 * pad; each press is held for 250 ms, or for D seconds with "sec:BUTTON/D".
 * Stick deflections LUP LDOWN LLEFT LRIGHT RUP RDOWN RLEFT RRIGHT hold the
 * stick fully over. BUTTON is otherwise one of START BACK UP DOWN
 * LEFT RIGHT A B X Y BLACK WHITE LT RT, or LS / RS (stick clicks).
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "recomp/gen/recomp_types.h"
#include "recomp/gen/recomp_funcs.h"
#include "xbox_memory_layout.h"

#include <windows.h>
#include <xinput.h>
typedef DWORD DWORD_;

#define ARG(n) MEM32(g_esp + 4 + 4 * (n))
#define GAMEPAD_TYPE   0x0032B878u
#define HANDLE_BASE    0x7EC0DE00u   /* fake handles: HANDLE_BASE + port */
#define ERR_NOT_CONN   0x48Fu

void sub_0032D70D_gen(void);
void sub_003332F7_gen(void);
void sub_0033334D_gen(void);
void sub_00333359_gen(void);
void sub_00333537_gen(void);
void sub_003335A3_gen(void);

static int is_fake(uint32_t h) { return (h & ~3u) == HANDLE_BASE; }

static int trace(void)
{
    static int on = -1;
    if (on < 0) { const char *e = getenv("CW_INPUT_TRACE"); on = e && *e == '1'; }
    return on;
}

/* ---- host state ------------------------------------------------------- */

typedef struct {
    uint16_t buttons;
    uint8_t analog[8];
    int16_t lx, ly, rx, ry;
} pad_t;

static int have_focus(void);

/* The controller counts only while the game window is in front: XInput
 * reports the pad to every process, so another game played alongside would
 * drive this one too. The pad still reads as connected when unfocused; it
 * just rests. Scripted presses (the live-control file) are unaffected. */
static int host_pad(unsigned port, pad_t *p)
{
    struct { DWORD_ pkt; uint16_t b; uint8_t lt, rt; int16_t lx, ly, rx, ry; } s;
    if (XInputGetState(port, &s) != 0)
        return 0;
    if (!have_focus())
        return 1;
    p->buttons |= s.b & 0x00FF;
    if (s.b & 0x1000) p->analog[0] = 255;
    if (s.b & 0x2000) p->analog[1] = 255;
    if (s.b & 0x4000) p->analog[2] = 255;
    if (s.b & 0x8000) p->analog[3] = 255;
    if (s.b & 0x0200) p->analog[4] = 255;   /* RB -> Black */
    if (s.b & 0x0100) p->analog[5] = 255;   /* LB -> White */
    if (s.lt > p->analog[6]) p->analog[6] = s.lt;
    if (s.rt > p->analog[7]) p->analog[7] = s.rt;
    p->lx = s.lx; p->ly = s.ly; p->rx = s.rx; p->ry = s.ry;
    return 1;
}

static int have_focus(void)
{
    DWORD_ pid = 0;
    void *w = GetForegroundWindow();
    if (!w) return 0;
    GetWindowThreadProcessId(w, &pid);
    return pid == GetCurrentProcessId();
}

static void keyboard_pad(pad_t *p)
{
    static const struct { int vk; int bit; int analog; } map[] = {
        { 0x0D, 0x10, -1 }, { 0x1B, 0x20, -1 },            /* Enter, Esc */
        { 0x26, 0x01, -1 }, { 0x28, 0x02, -1 },            /* arrows */
        { 0x25, 0x04, -1 }, { 0x27, 0x08, -1 },
        { 'Z', 0, 0 }, { 'X', 0, 1 }, { 'C', 0, 2 }, { 'V', 0, 3 },
        { 'B', 0, 4 }, { 'N', 0, 5 }, { 'Q', 0, 6 }, { 'E', 0, 7 },
    };
    unsigned i;
    if (!have_focus()) return;
    for (i = 0; i < sizeof map / sizeof map[0]; i++) {
        if (!(GetAsyncKeyState(map[i].vk) & 0x8000)) continue;
        if (map[i].analog < 0) p->buttons |= map[i].bit;
        else p->analog[map[i].analog] = 255;
    }
    if (GetAsyncKeyState('A') & 0x8000) p->lx = -32767;
    if (GetAsyncKeyState('D') & 0x8000) p->lx = 32767;
    if (GetAsyncKeyState('W') & 0x8000) p->ly = 32767;
    if (GetAsyncKeyState('S') & 0x8000) p->ly = -32767;
}

/* ---- autopilot ---------------------------------------------------------- */

/* "GOTO <player> <x> <z>" (live control) steers that player's vehicle to a
 * map point with the left stick and holds it there (with a radius, stops that
 * far out and keeps facing it); "GOTO <player> OFF"
 * stops. "WHERE" logs every player's position and facing. Player objects:
 * the mission manager at 0x3A2F0C holds the player table (+0x14, player
 * handle at +0xCC) and the handle -> object tree (+0x18, as 0x00014340
 * walks it); an object's matrix sits at +0x40 (rows right, up, front, pos). */
static struct { int on; float x, z, r; unsigned logged; } g_goto[4];

static uint32_t player_obj(int i)
{
    uint32_t mgr = MEM32(0x3A2F0Cu), arr, pl, h, node;
    if (!mgr || !(arr = MEM32(mgr + 0x14)) || !(pl = MEM32(arr + 4 * i)) || !(h = MEM32(pl + 0xCC)))
        return 0;
    node = MEM32(MEM32(mgr + 0x18) + 8);
    while (node) {
        uint32_t k = MEM32(node + 0x14);
        if (h < k) node = MEM32(node + 8);
        else if (h > k) node = MEM32(node + 0xC);
        else { uint32_t o = MEM32(node + 0x18); return o && (MEM32(o + 4) >> 8 & 1) ? o : 0; }
    }
    return 0;
}

static void where_report(void)
{
    int i;
    for (i = 0; i < 4; i++) {
        uint32_t o = player_obj(i);
        if (!o) { fprintf(stderr, "[WHERE] p%d none\n", i + 1); continue; }
        fprintf(stderr, "[WHERE] p%d obj=%08X pos %.1f %.1f %.1f front %.2f %.2f %.2f\n", i + 1, o,
                MEMF(o + 0x70), MEMF(o + 0x74), MEMF(o + 0x78), MEMF(o + 0x60), MEMF(o + 0x64), MEMF(o + 0x68));
    }
    fflush(stderr);
}

/* "OBJS": every live object with its position and the first readable name
 * reachable from its class (obj+0x20), for finding things on the map. */
static const char *obj_name(uint32_t cls)
{
    static char buf[40];
    int i, j;
    for (i = 0; i < 64 && cls; i++) {
        uint32_t a = cls + 4 * i, s = MEM32(a);
        for (j = 0; j < 2; j++, s = a) {
            int n = 0;
            if (s < 0x10000u || s >= 0x04000000u) continue;
            while (n < 39 && MEM8(s + n) >= 0x20 && MEM8(s + n) < 0x7F) n++;
            if (n >= 6 && !MEM8(s + n)) { memcpy(buf, (const void *)XBOX_PTR(s), n); buf[n] = 0; return buf; }
        }
    }
    return "?";
}

/* Current health, as MissionUtility::GetCurHealth reads it: the object's
 * health component (vtable +0x40, id 0xFBCD164A), float at +0x14. -1 if none. */
static float obj_health(uint32_t o)
{
    uint32_t sp = g_esp, b = g_ebx, si = g_esi, di = g_edi, a = g_eax, c = g_ecx, d = g_edx, comp;
    recomp_func_t fn = o ? recomp_lookup(MEM32(MEM32(o) + 0x40)) : NULL;
    if (!fn) return -1.0f;
    PUSH32(g_esp, 0xFBCD164Au); PUSH32(g_esp, 0);
    g_ecx = o;
    fn();
    comp = g_eax;
    g_esp = sp; g_ebx = b; g_esi = si; g_edi = di; g_eax = a; g_ecx = c; g_edx = d;
    return comp ? MEMF(comp + 0x14) : -1.0f;
}

static void objs_walk(uint32_t node, int depth)
{
    uint32_t o;
    if (!node || depth > 64) return;
    objs_walk(MEM32(node + 8), depth + 1);
    o = MEM32(node + 0x18);
    if (o) fprintf(stderr, "[OBJ] h=%08X obj=%08X pos %.0f %.0f %.0f hp %.0f %s\n", MEM32(node + 0x14), o,
                   MEMF(o + 0x70), MEMF(o + 0x74), MEMF(o + 0x78), obj_health(o), obj_name(MEM32(o + 0x20)));
    objs_walk(MEM32(node + 0xC), depth + 1);
}

static void autopilot(pad_t *p, unsigned port)
{
    uint32_t o;
    float dx, dz, dist, err;
    unsigned now = (unsigned)GetTickCount();
    if (port > 3 || !g_goto[port].on || !(o = player_obj((int)port))) return;
    dx = g_goto[port].x - MEMF(o + 0x70);
    dz = g_goto[port].z - MEMF(o + 0x78);
    dist = sqrtf(dx * dx + dz * dz);
    err = atan2f(dx, dz) - atan2f(MEMF(o + 0x60), MEMF(o + 0x68));
    while (err > 3.14159265f) err -= 6.2831853f;
    while (err < -3.14159265f) err += 6.2831853f;
    {
        float steer = err * 2.0f;
        if (steer > 1.0f) steer = 1.0f;
        if (steer < -1.0f) steer = -1.0f;
        p->lx = (int16_t)(steer * 32767.0f);
        if (dist > g_goto[port].r) p->ly = fabsf(err) < 1.2f ? 32767 : 12000;
    }
    if (now - g_goto[port].logged >= 1000) {
        g_goto[port].logged = now;
        fprintf(stderr, "[GOTO] p%u dist %.0f err %.0f deg\n", port + 1, dist, err * 57.3f);
        fflush(stderr);
    }
}

/* ---- scripted presses -------------------------------------------------- */

typedef struct { unsigned ms, dur; int bit; int analog; int port; } press_t;
static press_t g_script[4096];
static int g_nscript = -1;
static unsigned g_t0;

static int button_code(const char *name, int *bit, int *analog)
{
    static const struct { const char *n; int bit; int analog; } names[] = {
        { "UP", 0x01, -1 }, { "DOWN", 0x02, -1 }, { "LEFT", 0x04, -1 },
        { "RIGHT", 0x08, -1 }, { "START", 0x10, -1 }, { "BACK", 0x20, -1 },
        { "LS", 0x40, -1 }, { "RS", 0x80, -1 },              /* stick clicks */
        { "A", 0, 0 }, { "B", 0, 1 }, { "X", 0, 2 }, { "Y", 0, 3 },
        { "BLACK", 0, 4 }, { "WHITE", 0, 5 }, { "LT", 0, 6 }, { "RT", 0, 7 },
        /* sticks: analog 8 + axis*2 + (0 = negative, 1 = positive) */
        { "LLEFT", 0, 8 }, { "LRIGHT", 0, 9 }, { "LDOWN", 0, 10 }, { "LUP", 0, 11 },
        { "RLEFT", 0, 12 }, { "RRIGHT", 0, 13 }, { "RDOWN", 0, 14 }, { "RUP", 0, 15 },
    };
    unsigned i;
    for (i = 0; i < sizeof names / sizeof names[0]; i++)
        if (!strcmp(name, names[i].n)) { *bit = names[i].bit; *analog = names[i].analog; return 1; }
    return 0;
}

static void load_script(void)
{
    const char *s = getenv("CW_INPUT_SCRIPT");
    g_nscript = 0;
    g_t0 = (unsigned)GetTickCount();
    while (s && *s && g_nscript < 256) {
        char name[16];
        double sec;
        int n = 0, bit, analog;
        if (sscanf(s, "%lf:%15[A-Za-z]%n", &sec, name, &n) != 2) break;
        if (button_code(name, &bit, &analog)) {
            double dur = 0.25;
            int m = 0;
            if (s[n] == '/' && sscanf(s + n + 1, "%lf%n", &dur, &m) == 1) n += 1 + m;
            g_script[g_nscript].ms = (unsigned)(sec * 1000.0);
            g_script[g_nscript].dur = (unsigned)(dur * 1000.0);
            g_script[g_nscript].bit = bit;
            g_script[g_nscript].analog = analog;
            g_nscript++;
        }
        s += n;
        while (*s == ',' || *s == ' ') s++;
    }
    fprintf(stderr, "[INPUT] virtual gamepad on port 0, %d scripted presses\n", g_nscript);
}

/* CW_INPUT_FILE=<path>: live control. Lines appended to the file while the
 * game runs take effect at once: "BUTTON[/seconds]" presses (same names as
 * CW_INPUT_SCRIPT), "SHOT" saves the next frame (CW_SHOT_FILE). */
extern volatile int hle_shot_request;
static void live_input(void);
static void pads_report(void);

/* "WATCH <hex guest address>": report which recompiled functions write that
 * dword, using debug register DR0 on every thread of the process. A vectored
 * handler logs each distinct writer ([WATCH] sub_XXXXXXXX+off, found through
 * the PDB), up to 32. "WATCH 0" clears it. */
#include <tlhelp32.h>
typedef struct { ULONG SizeOfStruct, TypeIndex; ULONG64 Reserved[2]; ULONG Index, Size;
                 ULONG64 ModBase; ULONG Flags; ULONG64 Value, Address; ULONG Register, Scope, Tag;
                 ULONG NameLen, MaxNameLen; CHAR Name[1]; } watch_sym_t;
static BOOL (WINAPI *p_SymInitialize)(HANDLE, PCSTR, BOOL);
static BOOL (WINAPI *p_SymFromAddr)(HANDLE, DWORD64, DWORD64 *, watch_sym_t *);
static volatile LONG g_watch_hits;
static DWORD g_watch_eip[32];

static LONG CALLBACK watch_handler(EXCEPTION_POINTERS *ep)
{
    int k, n;
    DWORD eip;
    if (ep->ExceptionRecord->ExceptionCode != EXCEPTION_SINGLE_STEP || !(ep->ContextRecord->Dr6 & 1))
        return EXCEPTION_CONTINUE_SEARCH;
    ep->ContextRecord->Dr6 = 0;
#ifdef _M_IX86
    eip = ep->ContextRecord->Eip;
#else
    eip = (DWORD)ep->ContextRecord->Rip;
#endif
    n = (int)g_watch_hits;
    for (k = 0; k < n && k < 32; k++) if (g_watch_eip[k] == eip) return EXCEPTION_CONTINUE_EXECUTION;
    if (n < 32) {
        char buf[sizeof(watch_sym_t) + 256];
        watch_sym_t *s = (watch_sym_t *)buf;
        DWORD64 disp = 0;
        g_watch_eip[n] = eip;
        InterlockedIncrement(&g_watch_hits);
        memset(buf, 0, sizeof buf);
        s->SizeOfStruct = sizeof(watch_sym_t); s->MaxNameLen = 255;
        if (p_SymFromAddr && p_SymFromAddr(GetCurrentProcess(), eip, &disp, s))
            fprintf(stderr, "[WATCH] write from %s+0x%llX (eip %08lX, guest esp %08X)\n", s->Name,
                    (unsigned long long)disp, (unsigned long)eip, g_esp);
        else
            fprintf(stderr, "[WATCH] write from eip %08lX\n", (unsigned long)eip);
        fflush(stderr);
    }
    return EXCEPTION_CONTINUE_EXECUTION;
}

static DWORD WINAPI watch_apply(LPVOID arg)
{
    uint32_t guest = (uint32_t)(uintptr_t)arg;
    HANDLE snap;
    THREADENTRY32 te;
    DWORD me = GetCurrentThreadId(), pid = GetCurrentProcessId();
    uintptr_t host = guest ? (uintptr_t)XBOX_PTR(guest) : 0;
    {
        HMODULE dh = p_SymFromAddr ? NULL : LoadLibraryA("dbghelp.dll");
        if (dh) {
            p_SymInitialize = (BOOL (WINAPI *)(HANDLE, PCSTR, BOOL))GetProcAddress(dh, "SymInitialize");
            p_SymFromAddr = (BOOL (WINAPI *)(HANDLE, DWORD64, DWORD64 *, watch_sym_t *))GetProcAddress(dh, "SymFromAddr");
            if (p_SymInitialize) p_SymInitialize(GetCurrentProcess(), NULL, TRUE);
        }
    }
    g_watch_hits = 0;
    snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snap == INVALID_HANDLE_VALUE) return 0;
    te.dwSize = sizeof te;
    for (BOOL ok = Thread32First(snap, &te); ok; ok = Thread32Next(snap, &te)) {
        HANDLE th;
        CONTEXT c;
        if (te.th32OwnerProcessID != pid) continue;
        th = OpenThread(THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);
        if (!th) continue;
        if (te.th32ThreadID != me) SuspendThread(th);
        memset(&c, 0, sizeof c);
        c.ContextFlags = CONTEXT_DEBUG_REGISTERS;
        if (te.th32ThreadID == me || GetThreadContext(th, &c)) {
            c.ContextFlags = CONTEXT_DEBUG_REGISTERS;
            c.Dr0 = host;
            /* L0 enable, RW0 = 01 (write), LEN0 = 11 (4 bytes) */
            c.Dr7 = host ? (c.Dr7 & ~0x000F0003u) | 0x1u | (0x1u << 16) | (0x3u << 18) : (c.Dr7 & ~0x000F0003u);
            if (te.th32ThreadID != me) SetThreadContext(th, &c);
        }
        if (te.th32ThreadID != me) ResumeThread(th);
        CloseHandle(th);
    }
    CloseHandle(snap);
    fprintf(stderr, "[WATCH] %s %08X\n", guest ? "watching" : "cleared", guest);
    fflush(stderr);
    return 0;
}

/* A thread cannot load its own debug registers: a helper thread sets them on
 * every thread, the caller (the game's render thread) included. */
static void watch_set(uint32_t guest)
{
    static int init;
    HANDLE t;
    if (!init) { init = 1; AddVectoredExceptionHandler(1, watch_handler); }
    t = CreateThread(NULL, 0, watch_apply, (LPVOID)(uintptr_t)guest, 0, NULL);
    if (t) { WaitForSingleObject(t, 5000); CloseHandle(t); }
}
/* --headlessMuted status line for tools/smoke_maps.py, once a second from its
 * own thread so it keeps coming when the frame loop stalls: frames presented,
 * the mission state key (RUN 6D3CE530, PAUSE 37345010, SUCCESS 4CCBDEC7,
 * FAILURE A90C322D), the mission-running flag and the pending level name. */
unsigned hle_frame_count(void);
static DWORD WINAPI smoke_status(LPVOID unused)
{
    unsigned last_frames = 0, still = 0;
    (void)unused;
    for (;;) {
        uint32_t node = 0, key = 0, run = 0;
        char level[33];
        int k;
        __try {
            node = MEM32(0x005A71E8u);
            if (node >= 0x00010000u && node < 0x08000000u) key = MEM32(node + 0x14);
            run = MEM8(0x005A71CCu);
            for (k = 0; k < 32 && MEM8(0x003FC288u + k) >= 0x20; k++) level[k] = (char)MEM8(0x003FC288u + k);
            level[k] = 0;
        } __except (EXCEPTION_EXECUTE_HANDLER) { level[0] = 0; }
        fprintf(stderr, "[SMOKE] t=%lu frames=%u state=%08X run=%u level=%s\n",
                (unsigned long)GetTickCount(), hle_frame_count(), key, run, level);
        fflush(stderr);
        /* No new frame for 20 s after the game was drawing: say where it is. */
        if (hle_frame_count() != last_frames) { last_frames = hle_frame_count(); still = 0; }
        else if (last_frames && ++still == 20) {
            extern void hle_dump_main_stack(void);
            hle_dump_main_stack();
        }
        Sleep(1000);
    }
}
void hle_smoke_start(void)
{
    CloseHandle(CreateThread(NULL, 0, smoke_status, NULL, 0, NULL));
}

static unsigned g_tracef_addr[8], g_tracef_n[8];
static FILE *g_tracef_file[8];
void hle_live_poll(void)
{
    int t;
    live_input();
    for (t = 0; t < 8; t++) {
        unsigned k;
        if (!g_tracef_file[t]) continue;
        fprintf(g_tracef_file[t], "%u", (unsigned)GetTickCount());
        for (k = 0; k < g_tracef_n[t]; k++) {
            uint32_t v = MEM32(g_tracef_addr[t] + 4 * k);
            float f; memcpy(&f, &v, 4);
            fprintf(g_tracef_file[t], " %.4f", f);
        }
        fputc(10, g_tracef_file[t]);
        fflush(g_tracef_file[t]);
    }
}

static void live_input(void)
{
    static const char *path;
    static long pos;
    static unsigned last;
    unsigned now = (unsigned)GetTickCount();
    FILE *f;
    char line[512];
    if (!path) { path = getenv("CW_INPUT_FILE"); if (!path) path = ""; }
    if (!*path || now - last < 100) return;
    last = now;
    f = fopen(path, "rb");
    if (!f) {
        static int nfail;
        if (nfail++ < 20) fprintf(stderr, "[LIVE] cannot open %s\n", path);
        return;
    }
    fseek(f, pos, SEEK_SET);
    while (fgets(line, sizeof line, f)) {
        char name[16];
        double dur = 0.25;
        int bit, analog;
        if (!strchr(line, 10)) break;                 /* incomplete line: later */
        pos = ftell(f);
        if (trace()) { fprintf(stderr, "[LIVE] @%ld %s", pos, line); fflush(stderr); }
        int port = 0;
        const char *l = line;
        if (!strncmp(line, "SHOT", 4)) { hle_shot_request = 1; continue; }
        if (!strncmp(line, "PADS", 4)) { pads_report(); continue; }
        if (!strncmp(line, "WHERE", 5)) { where_report(); continue; }
        if (!strncmp(line, "GOTO ", 5)) {
            int pl = 0; float x = 0, z = 0, r = 20.0f;
            if (sscanf(line + 5, "%d %f %f %f", &pl, &x, &z, &r) >= 1 && pl >= 1 && pl <= 4) {
                g_goto[pl - 1].r = r;
                g_goto[pl - 1].on = strstr(line, "OFF") == NULL;
                g_goto[pl - 1].x = x; g_goto[pl - 1].z = z;
                fprintf(stderr, "[GOTO] p%d %s %.0f %.0f\n", pl, g_goto[pl - 1].on ? "to" : "off", x, z);
            }
            continue;
        }
        if (!strncmp(line, "VSFOG ", 6)) { extern void hle_vs_tablefog(int); hle_vs_tablefog(atoi(line + 6)); continue; }
        if (!strncmp(line, "DUMPVC ", 7)) { extern void hle_dump_vc(uint32_t); hle_dump_vc((uint32_t)strtoul(line + 7, NULL, 16)); continue; }
        if (!strncmp(line, "SHOWVA ", 7)) { extern void hle_show_va(uint32_t); hle_show_va((uint32_t)strtoul(line + 7, NULL, 16)); continue; }
        if (!strncmp(line, "SKIPVS ", 7)) { extern void hle_skip_vs(uint32_t); hle_skip_vs((uint32_t)strtoul(line + 7, NULL, 16)); continue; }
        if (!strncmp(line, "FTRESET", 7)) { extern void hle_ft_reset(void); hle_ft_reset(); continue; }
        if (!strncmp(line, "FTREPORT", 8)) { extern void hle_ft_report(void); hle_ft_report(); continue; }
        if (!strncmp(line, "FXAA ", 5)) { extern void hle_set_fxaa(int); hle_set_fxaa(atoi(line + 5)); continue; }
        if (!strncmp(line, "WATCH ", 6)) { watch_set((uint32_t)strtoul(line + 6, NULL, 16)); continue; }
        if (!strncmp(line, "CAPTURE", 7)) { extern volatile int hle_capture_request; hle_capture_request = 1; continue; }
        if (!strncmp(line, "CALLD ", 6)) {
            /* CALLD <hex va> <double>: call a cdecl guest function taking one
             * double, log ST0 afterwards (CRT math self-test). */
            unsigned va = 0; double x = 0;
            if (sscanf(line + 6, "%x %lf", &va, &x) == 2) {
                recomp_func_t fn = recomp_lookup(va);
                uint32_t sp = g_esp, lo, hi;
                memcpy(&lo, &x, 4); memcpy(&hi, (char *)&x + 4, 4);
                if (fn) {
                    PUSH32(g_esp, hi); PUSH32(g_esp, lo); PUSH32(g_esp, 0);
                    fn();
                    fprintf(stderr, "[CALLD] %08X(%g) -> %g\n", va, x, g_fp_stack[g_fp_top & 7]);
                    g_fp_top = (g_fp_top + 1) & 7;
                    g_esp = sp;
                }
                fflush(stderr);
            }
            continue;
        }
        if (!strncmp(line, "DUMP ", 5)) {
            /* DUMP <hex addr> <hex len> <path>: raw guest memory to a file. */
            char out[260] = {0};
            unsigned a = 0, n = 0;
            if (sscanf(line + 5, "%x %x %259s", &a, &n, out) == 3) {
                FILE *df = fopen(out, "wb");
                if (df) { fwrite((const void *)XBOX_PTR(a), 1, n, df); fclose(df); }
                fprintf(stderr, "[DUMP] %08X+%X -> %s\n", a, n, out); fflush(stderr);
            }
            continue;
        }
        if (!strncmp(line, "TRACEF ", 7)) {
            /* TRACEF <hex addr> <count> <path>: every presented frame, append
             * "tick f0 f1 ..." (count floats at addr) to path. TRACEF 0 stops. */
            unsigned a = 0, n = 0;
            char out[260] = {0};
            int t;
            sscanf(line + 7, "%x %u %259s", &a, &n, out);
            if (!a) {                                  /* TRACEF 0: stop all */
                for (t = 0; t < 8; t++) if (g_tracef_file[t]) { fclose(g_tracef_file[t]); g_tracef_file[t] = NULL; }
                continue;
            }
            for (t = 0; t < 8 && g_tracef_file[t]; t++) ;
            if (t < 8 && *out && (g_tracef_file[t] = fopen(out, "w")) != NULL) {
                g_tracef_addr[t] = a; g_tracef_n[t] = n > 32 ? 32 : n;
            }
            continue;
        }
        if (!strncmp(line, "FIND ", 5) || !strncmp(line, "POKE ", 5) || !strncmp(line, "PEEK ", 5)) {
            /* Debug memory access: FIND <hex bytes> (logs guest addresses of
             * matches), POKE <hex addr> <hex bytes>, PEEK <hex addr> <len>. */
            uint8_t pat[200];
            int np = 0, hi = -1;
            const char *q = line + 5;
            uint32_t addr = 0;
            if (line[0] != 'F') { addr = (uint32_t)strtoul(q, (char **)&q, 16); }
            if (line[0] == 'P' && line[1] == 'E') {
                uint32_t n = (uint32_t)strtoul(q, NULL, 10), k;
                if (n > 256) n = 256;
                fprintf(stderr, "[PEEK] %08X:", addr);
                for (k = 0; k < n; k++) fprintf(stderr, " %02X", MEM8(addr + k));
                fputc(10, stderr); fflush(stderr);
                continue;
            }
            for (; *q && np < 200; q++) {
                int v = (*q >= '0' && *q <= '9') ? *q - '0' : (*q >= 'a' && *q <= 'f') ? *q - 'a' + 10
                      : (*q >= 'A' && *q <= 'F') ? *q - 'A' + 10 : -1;
                if (v < 0) continue;
                if (hi < 0) hi = v; else { pat[np++] = (uint8_t)(hi << 4 | v); hi = -1; }
            }
            if (line[0] == 'P') {
                int k;
                for (k = 0; k < np; k++) MEM8(addr + (uint32_t)k) = pat[k];
                fprintf(stderr, "[POKE] %08X <- %d bytes\n", addr, np);
            } else if (np) {
                uint32_t a;
                int found = 0;
                for (a = 0x10000u; a + (uint32_t)np < 0x04000000u && found < 32; a++) {
                    int k;
                    if (MEM8(a) != pat[0]) continue;
                    for (k = 1; k < np; k++) if (MEM8(a + (uint32_t)k) != pat[k]) break;
                    if (k == np) { fprintf(stderr, "[FIND] match at %08X\n", a); found++; }
                }
                fprintf(stderr, "[FIND] %d matches\n", found);
            }
            fflush(stderr);
            continue;
        }
        if (l[0] >= '1' && l[0] <= '4' && l[1] == ':') { port = l[0] - '1'; l += 2; }
        if (g_nscript == 4096) {
            /* Full: drop presses that are over, or long unattended runs
             * (four bot-driven pads) stop taking input after a few minutes. */
            int i, n = 0;
            for (i = 0; i < g_nscript; i++)
                if (now - g_t0 < g_script[i].ms + g_script[i].dur) g_script[n++] = g_script[i];
            g_nscript = n;
        }
        if (sscanf(l, "%15[A-Za-z]/%lf", name, &dur) >= 1 && button_code(name, &bit, &analog) &&
            g_nscript < 4096) {
            g_script[g_nscript].port = port;
            g_script[g_nscript].ms = now - g_t0;
            g_script[g_nscript].dur = (unsigned)(dur * 1000.0);
            g_script[g_nscript].bit = bit;
            g_script[g_nscript].analog = analog;
            g_nscript++;
        }
    }
    fclose(f);
}

static void script_pad(pad_t *p, unsigned port)
{
    unsigned now;
    int i;
    live_input();
    now = (unsigned)GetTickCount() - g_t0;
    for (i = 0; i < g_nscript; i++) {
        int a = g_script[i].analog;
        if ((unsigned)g_script[i].port != port) continue;
        if (now < g_script[i].ms || now >= g_script[i].ms + g_script[i].dur) continue;
        if (a < 0) p->buttons |= g_script[i].bit;
        else if (a < 8) p->analog[a] = 255;
        else {
            int16_t v = (a & 1) ? 32767 : -32767;
            switch ((a - 8) >> 1) {
            case 0: p->lx = v; break;
            case 1: p->ly = v; break;
            case 2: p->rx = v; break;
            default: p->ry = v; break;
            }
        }
    }
    autopilot(p, port);
}

static unsigned connected_mask(void)
{
    unsigned m = 1, port, virt = 1;
    struct { DWORD_ pkt; uint8_t rest[12]; } s;
    const char *e = getenv("CW_PADS");            /* virtual pads always connected */
    if (e && *e >= '1' && *e <= '4') virt = (unsigned)(*e - '0');
    for (port = 1; port < 4; port++)
        if (port < virt || XInputGetState(port, &s) == 0) m |= 1u << port;
    return m;
}

/* ---- wrappers ------------------------------------------------------------ */

/* XGetDeviceChanges */
void sub_0032D70D(void)
{
    static unsigned prev;
    uint32_t type = ARG(0), pins = ARG(1), prem = ARG(2);
    unsigned cur;
    if (type != GAMEPAD_TYPE) { sub_0032D70D_gen(); return; }
    if (g_nscript < 0) load_script();
    cur = connected_mask();
    MEM32(pins) = cur & ~prev;
    MEM32(prem) = prev & ~cur;
    if (trace() && cur != prev)
        fprintf(stderr, "[INPUT] device changes ins=%X rem=%X\n", cur & ~prev, prev & ~cur);
    g_eax = (cur != prev);
    prev = cur;
    g_esp += 16;
}

/* XInputOpen */
void sub_003332F7(void)
{
    uint32_t type = ARG(0), port = ARG(1);
    if (type != GAMEPAD_TYPE) { sub_003332F7_gen(); return; }
    g_eax = HANDLE_BASE + (port & 3);
    if (trace()) fprintf(stderr, "[INPUT] open port %u -> %08X\n", port, g_eax);
    g_esp += 20;
}

/* XInputClose */
void sub_0033334D(void)
{
    if (!is_fake(ARG(0))) { sub_0033334D_gen(); return; }
    g_esp += 8;
}

/* XInputGetCapabilities */
void sub_00333359(void)
{
    uint32_t h = ARG(0), caps = ARG(1), i;
    if (!is_fake(h)) { sub_00333359_gen(); return; }
    /* XINPUT_CAPABILITIES: SubType, Reserved, In.Gamepad (18), Out.Rumble (4) */
    MEM8(caps + 0) = 1;                     /* XINPUT_DEVSUBTYPE_GC_GAMEPAD */
    MEM8(caps + 1) = 0;
    MEM16(caps + 2) = 0;
    MEM16(caps + 4) = 0x00FF;
    for (i = 0; i < 8; i++) MEM8(caps + 6 + i) = 0xFF;
    for (i = 0; i < 4; i++) MEM16(caps + 14 + 2 * i) = 0xFFFF;
    MEM16(caps + 22) = 0xFFFF;
    MEM16(caps + 24) = 0xFFFF;
    g_eax = 0;
    g_esp += 12;
}

/* ---- input recording and replay ------------------------------------------ */

/* CW_INPUT_RECORD=1: every change of pad 0's state is appended to
 * captures\input-<date>.txt beside the exe, as
 *   <segment> <frame> <ms> <buttons> <a0..a7> <lx> <ly> <rx> <ry>
 * where <segment> counts pauses in presenting (movies, loads) and <frame> is
 * frames since the last one (hle_frame_key), <ms> the time since the first
 * read. CW_INPUT_REPLAY=<file> plays such a file back on pad 0: each
 * change is applied at the first read in or after its frame, one change per
 * read so a press and release in the same frame both reach the game. Keyed on
 * frames, not reads: the game reads the pad hundreds of times during movies,
 * at a rate set by the host clock. Needs RECOMP_FIXED_TIME=1 on both runs
 * (1/60 s of game time per frame) for the replay to stay in step. */
char hle_shot_path[260];                  /* one-off screenshot path (d3d8.c) */
static FILE *s_rec;
static FILE *s_rep;
static uint32_t s_poll;
static struct { uint32_t seg, frame; pad_t p; int have; } s_rep_next;
extern unsigned hle_frame_count(void);
extern void hle_frame_key(unsigned *seg, unsigned *off);
static pad_t s_rep_cur;

static void rec_open(void)
{
    static int tried;
    const char *e;
    if (tried) return;
    tried = 1;
    e = getenv("CW_INPUT_REPLAY");
    if (e && *e) {
        s_rep = fopen(e, "r");
        fprintf(stderr, "[INPUT] replay %s: %s\n", e, s_rep ? "playing" : "cannot open");
    }
    e = getenv("CW_INPUT_RECORD");
    if (e && *e == '1') {
        char path[MAX_PATH], *slash;
        SYSTEMTIME t;
        GetModuleFileNameA(NULL, path, MAX_PATH);
        slash = strrchr(path, '\\');
        if (slash) *slash = 0;
        strncat(path, "\\captures", MAX_PATH - strlen(path) - 1);
        CreateDirectoryA(path, NULL);
        GetLocalTime(&t);
        snprintf(path + strlen(path), MAX_PATH - strlen(path), "\\input-%04u%02u%02u-%02u%02u%02u.txt",
                 t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
        s_rec = fopen(path, "w");
        fprintf(stderr, "[INPUT] recording controls to %s\n", s_rec ? path : "(cannot open)");
    }
}

static int rep_read_line(void)
{
    unsigned seg, frame, ms, b, a[8];
    int lx, ly, rx, ry, k;
    char line[256];
    while (fgets(line, sizeof line, s_rep)) {
        if (sscanf(line, "%u %u %u %x %u %u %u %u %u %u %u %u %d %d %d %d", &seg, &frame, &ms, &b,
                   &a[0], &a[1], &a[2], &a[3], &a[4], &a[5], &a[6], &a[7], &lx, &ly, &rx, &ry) != 16)
            continue;
        s_rep_next.seg = seg;
        s_rep_next.frame = frame;
        memset(&s_rep_next.p, 0, sizeof s_rep_next.p);
        s_rep_next.p.buttons = (uint16_t)b;
        for (k = 0; k < 8; k++) s_rep_next.p.analog[k] = (uint8_t)a[k];
        s_rep_next.p.lx = (int16_t)lx; s_rep_next.p.ly = (int16_t)ly;
        s_rep_next.p.rx = (int16_t)rx; s_rep_next.p.ry = (int16_t)ry;
        s_rep_next.have = 1;
        return 1;
    }
    s_rep_next.have = 0;
    return 0;
}

/* Pad 0, once per read by the game: record it, or replace it on replay. */
static void rec_pad0(pad_t *p)
{
    static pad_t last;
    static unsigned t0;
    static int first = 1;
    unsigned now = (unsigned)GetTickCount(), frame = hle_frame_count(), seg, off;
    hle_frame_key(&seg, &off);
    rec_open();
    if (first) { t0 = now; first = 0; if (s_rep) rep_read_line(); }
    if (s_rep) {
        if (s_rep_next.have && (s_rep_next.seg < seg || (s_rep_next.seg == seg && s_rep_next.frame <= off))) {
            s_rep_cur = s_rep_next.p;
            if (!rep_read_line()) {
                fprintf(stderr, "[INPUT] replay finished at frame %u\n", frame);
                fflush(stderr);
            }
        }
        /* Keyboard and script input still add on top (to take over after). */
        {
            pad_t extra = *p;
            *p = s_rep_cur;
            p->buttons |= extra.buttons;
            for (int k = 0; k < 8; k++) if (extra.analog[k] > p->analog[k]) p->analog[k] = extra.analog[k];
            /* a resting real stick reads a little off centre: ignore that */
            if (abs(extra.lx) > 8000) p->lx = extra.lx;
            if (abs(extra.ly) > 8000) p->ly = extra.ly;
            if (abs(extra.rx) > 8000) p->rx = extra.rx;
            if (abs(extra.ry) > 8000) p->ry = extra.ry;
        }
    }
    if (s_rec && (s_poll == 0 || memcmp(p, &last, sizeof *p))) {
        fprintf(s_rec, "%u %u %u %04X %u %u %u %u %u %u %u %u %d %d %d %d\n", seg, off, now - t0, p->buttons,
                p->analog[0], p->analog[1], p->analog[2], p->analog[3], p->analog[4], p->analog[5],
                p->analog[6], p->analog[7], p->lx, p->ly, p->rx, p->ry);
        fflush(s_rec);
        last = *p;
    }
    {
        /* CW_POLL_SHOTS=<dir>: a screenshot every 600 frames (10 s of game),
         * named by frame -- a recording and its replay can then be compared
         * frame for frame. */
        static const char *dir = (const char *)1;
        static unsigned shot_frame;
        extern volatile int hle_shot_request;
        if (dir == (const char *)1) dir = getenv("CW_POLL_SHOTS");
        if (dir && *dir && frame && frame % 600 == 0 && frame != shot_frame) {
            shot_frame = frame;
            snprintf(hle_shot_path, sizeof hle_shot_path, "%s\\frame-%06u.bmp", dir, frame);
            hle_shot_request = 1;
        }
    }
    s_poll++;
}

/* "PADS" (live control): how often each port was read since the last PADS,
 * and from which return addresses -- which code is polling which pad. */
static unsigned g_polls[4], g_poll_ret[4][8], g_poll_nret[4];
static void pads_report(void)
{
    unsigned p, k;
    for (p = 0; p < 4; p++) {
        fprintf(stderr, "[PADS] port %u: %u reads, from", p, g_polls[p]);
        for (k = 0; k < g_poll_nret[p]; k++) fprintf(stderr, " %08X", g_poll_ret[p][k]);
        fputc(10, stderr);
        g_polls[p] = 0; g_poll_nret[p] = 0;
    }
    fflush(stderr);
}

/* XInputGetState */
void sub_00333537(void)
{
    static uint32_t packet[4];
    static pad_t last[4];
    uint32_t h = ARG(0), st = ARG(1), i;
    unsigned port;
    pad_t p;
    if (!is_fake(h)) { sub_00333537_gen(); return; }
    port = h & 3;
    g_polls[port]++;
    {
        uint32_t ret = MEM32(g_esp), k;
        for (k = 0; k < g_poll_nret[port] && g_poll_ret[port][k] != ret; k++) ;
        if (k == g_poll_nret[port] && k < 8) g_poll_ret[port][g_poll_nret[port]++] = ret;
    }
    memset(&p, 0, sizeof p);
    host_pad(port, &p);
    if (port == 0) keyboard_pad(&p);
    script_pad(&p, port);
    if (port == 0) rec_pad0(&p);
    if (memcmp(&p, &last[port], sizeof p)) {
        packet[port]++;
        if (trace()) fprintf(stderr, "[INPUT] port %u buttons=%04X A=%u B=%u\n",
                             port, p.buttons, p.analog[0], p.analog[1]);
        last[port] = p;
    }
    MEM32(st) = packet[port];
    MEM16(st + 4) = p.buttons;
    for (i = 0; i < 8; i++) MEM8(st + 6 + i) = p.analog[i];
    MEM16(st + 14) = (uint16_t)p.lx;
    MEM16(st + 16) = (uint16_t)p.ly;
    MEM16(st + 18) = (uint16_t)p.rx;
    MEM16(st + 20) = (uint16_t)p.ry;
    g_eax = 0;
    g_esp += 12;
}

/* XInputSetState (rumble) */
void sub_003335A3(void)
{
    uint32_t h = ARG(0), fb = ARG(1);
    struct { uint16_t l, r; } v;
    if (!is_fake(h)) { sub_003335A3_gen(); return; }
    /* XINPUT_FEEDBACK: header {dwStatus, hEvent, Reserved[58]}, Rumble at +0x42 */
    v.l = MEM16(fb + 0x42);
    v.r = MEM16(fb + 0x44);
    {
        extern int cw_headless_muted(void);
        if (!cw_headless_muted())           /* unattended runs leave the pad still */
            XInputSetState(h & 3, &v);
    }
    MEM32(fb) = 0;                          /* completed */
    g_eax = 0;
    g_esp += 12;
}
