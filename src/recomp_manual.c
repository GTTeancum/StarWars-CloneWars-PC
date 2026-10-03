/**
 * Manual function overrides and ICALL diagnostics
 *
 * This file provides:
 *   - recomp_lookup_manual()  : intercept specific Xbox VAs with hand-written code
 *   - recomp_icall_fail_log() : log when an indirect call target can't be resolved
 *   - ICALL trace ring buffer  : globals used by the RECOMP_ICALL macro
 *
 * The recomp pipeline generates an auto-dispatch table (recomp_lookup) that
 * resolves most function addresses. recomp_lookup_manual() is called FIRST,
 * giving you a chance to override any function with a custom implementation.
 *
 * Common reasons to add manual overrides:
 *   - Trace a function to understand call flow (wrap the generated version)
 *   - Fix a function the lifter translated incorrectly
 *   - Stub out a function that crashes (return early, set eax to a safe value)
 *   - Redirect a function to a native implementation (e.g., skip CRT init)
 *   - Intercept D3D/audio calls for custom rendering or sound
 */

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* The generated runtime header, rather than hand-written externs.
 *
 * It declares the ICALL trace ring buffer, recomp_func_t, g_xbox_mem_offset
 * and the register file, and it declares the register file RECOMP_TLS. That
 * last part is the reason to include it: the registers are thread-local, and
 * a plain `extern uint32_t g_eax;` does not name the calling thread's copy --
 * it names the image's TLS template. An override written against that
 * declaration reads and writes registers the generated code never sees, and
 * does so silently. The template's own declarations had exactly that shape.
 *
 * src/ is on the include path, so this resolves from src/recomp/gen/. */
#include "recomp/gen/recomp_types.h"
#include "recomp/gen/recomp_funcs.h"
#include "xbox_memory_layout.h"

enum {
    AUDIO_NOOP_TICK_VA = 0x00221EF0u,
    AUDIO_NOOP_SET_PTR_VA = 0x00221BD0u,
    AUDIO_NOOP_SET_FLOAT_A_VA = 0x00221CC0u,
    AUDIO_NOOP_SET_FLOAT_B_VA = 0x00222340u,
    AUDIO_NOOP_DESTROY_VA = 0x00221950u,
};

static uint32_t audio_stub_object;
static uint32_t audio_stub_vtable;
static uint32_t audio_stub_control_vtable;
static uint32_t audio_stub_controls[4];

static uint32_t manual_gap_logged;

void sub_0005FE10(void);
void sub_00062AA0(void);
void sub_00068D80(void);
void sub_00022AA0(void);
void sub_0006ABC0(void);
void sub_0007BB40(void);
void sub_00080790(void);
void sub_00083240(void);
void sub_00095480(void);
void sub_000990D0(void);
void sub_00099240(void);
void sub_0009B360(void);
void sub_0009C200(void);
void sub_0009E220(void);
void sub_000C5C60(void);
void sub_000C5C80(void);
void sub_000D93D0(void);
void sub_000ECD30(void);
void sub_000EFD50(void);
void sub_00106430(void);
void sub_00106470(void);
void sub_00107940(void);
void sub_0010ED90(void);
void sub_00112D90(void);
void sub_00115EF0(void);
void sub_00229CD0(void);
void sub_002578F0(void);
void sub_0032BB13(void);

static void audio_zero_block(uint32_t va, uint32_t size)
{
    for (uint32_t off = 0; off < size; off += 4)
        MEM32(va + off) = 0;
}

static uint32_t audio_alloc_zero(uint32_t size, uint32_t alignment)
{
    uint32_t va = xbox_HeapAlloc(size, alignment);
    if (va)
        audio_zero_block(va, size);
    return va;
}

static void audio_noop_tick(void)
{
    g_eax = 0;
    g_esp += 4; /* ret */
}

static void audio_noop_set_ptr(void)
{
    g_eax = 0;
    g_esp += 8; /* ret 4 */
}

static void audio_noop_set_float_a(void)
{
    g_eax = 0;
    g_esp += 8; /* ret 4 */
}

static void audio_noop_set_float_b(void)
{
    g_eax = 0;
    g_esp += 8; /* ret 4 */
}

static void audio_noop_destroy(void)
{
    g_eax = g_ecx;
    g_esp += 8; /* ret 4 */
}

static int crt_copy_args_valid(uint32_t dst, uint32_t src, uint32_t size)
{
    if (!g_xbox_mem_offset)
        return 0;
    if (size > 64u * 1024u * 1024u)
        return 0;
    if (size && (!dst || !src))
        return 0;
    if ((uint32_t)(dst + size) < dst || (uint32_t)(src + size) < src)
        return 0;
    return 1;
}

static void crt_memmove_tail(uint32_t va)
{
    static uint32_t bad_logged;
    uint32_t ebp = g_seh_ebp ? g_seh_ebp : g_ebp;
    uint32_t dst = 0;
    uint32_t src = 0;
    uint32_t size = 0;

    if (ebp) {
        dst = MEM32(ebp + 8);
        src = MEM32(ebp + 0xC);
        size = MEM32(ebp + 0x10);
    }

    if (crt_copy_args_valid(dst, src, size)) {
        memmove((void *)XBOX_PTR(dst), (const void *)XBOX_PTR(src), size);
    } else {
        uint32_t bit = (va >> 2) & 31u;
        uint32_t mask = 1u << bit;
        if (!(bad_logged & mask)) {
            bad_logged |= mask;
            fprintf(stderr,
                    "[CRT_COPY_RECOVER] bad_args va=0x%08X ebp=0x%08X dst=0x%08X src=0x%08X size=0x%08X -> skip copy\n",
                    va, ebp, dst, src, size);
            fflush(stderr);
        }
    }

    g_eax = dst;
    if (ebp) {
        POP32(g_esp, g_esi);
        POP32(g_esp, g_edi);
        g_esp = ebp;
        POP32(g_esp, g_ebp);
        g_esp += 4; /* ret */
    } else {
        g_esp += 4; /* best-effort ret */
    }
}

/* 0x00265210: the CRT's memcpy/memmove (one body, overlap-safe), cdecl
 * (dst, src, count) -> dst. The disassembler cuts it into a dozen fragments
 * joined by jump tables, and the backward-copy path's branch to 0x002653D4
 * lands in no fragment at all, so an overlapping copy over 32 bytes came out
 * partial. Replaced whole; the fragments are unreachable except through the
 * tail handlers below, kept for any stray indirect entry. */
void sub_00265210(void)
{
    uint32_t dst = MEM32(g_esp + 4), src = MEM32(g_esp + 8), size = MEM32(g_esp + 12);
    if (crt_copy_args_valid(dst, src, size) && size)
        memmove((void *)XBOX_PTR(dst), (const void *)XBOX_PTR(src), size);
    g_eax = dst;
    g_esp += 4; /* ret */
}

/* CRT transcendental math. The CRT's x87 dispatchers (0x0026A47C,
 * 0x0026A4BA, 0x0026A4F7, 0x0026A68D) run hand-written helpers that share the
 * dispatcher's frame and pop their own return addresses; lifted, one of them
 * leaves esp 4 bytes high, the dispatcher's saved ebx is overwritten by a
 * return address, and the caller's ebx comes back as garbage. The entry points
 * are replaced with host math instead. Each is named by the descriptor its
 * stub loads into edx. Two forms per function:
 *   name(double[, double])  cdecl, args on the stack, result pushed on ST0
 *   _CIname                 args in ST0 (and ST1), result replaces them
 * Error reporting (errno, _matherr) is not reproduced. */
#define FP_ST(i) g_fp_stack[(g_fp_top + (i)) & 7u]
static void fp_push_host(double v) { g_fp_top = (g_fp_top + 7) & 7; g_fp_stack[g_fp_top] = v; }
static void fp_pop_host(void) { g_fp_top = (g_fp_top + 1) & 7; }
static double arg_double(uint32_t off)
{
    double v;
    memcpy(&v, (const void *)XBOX_PTR(g_esp + off), sizeof v);
    return v;
}

void sub_00264C64(void) { fp_push_host(fmod(arg_double(4), arg_double(12))); g_esp += 4; }        /* fmod */
void sub_00264C6E(void) { double y = FP_ST(0), x = FP_ST(1); fp_pop_host(); FP_ST(0) = fmod(x, y); g_esp += 4; } /* _CIfmod */
void sub_00266C08(void) { fp_push_host(exp(arg_double(4))); g_esp += 4; }                          /* exp */
void sub_00266C0A(void) { FP_ST(0) = exp(FP_ST(0)); g_esp += 4; }                                  /* _CIexp */
void sub_00266C0E(void) { fp_push_host(exp(arg_double(4))); g_esp += 4; }
void sub_00266C18(void) { FP_ST(0) = exp(FP_ST(0)); g_esp += 4; }
void sub_00266894(void) { fp_push_host(sinh(arg_double(4))); g_esp += 4; }                         /* sinh */
void sub_0026689E(void) { fp_push_host(cosh(arg_double(4))); g_esp += 4; }                         /* cosh */
void sub_002668A5(void) { fp_push_host(tanh(arg_double(4))); g_esp += 4; }                         /* tanh */
void sub_002668AC(void) { FP_ST(0) = sinh(FP_ST(0)); g_esp += 4; }                                 /* _CIsinh */
void sub_002668B6(void) { FP_ST(0) = cosh(FP_ST(0)); g_esp += 4; }                                 /* _CIcosh */
void sub_002668BD(void) { FP_ST(0) = tanh(FP_ST(0)); g_esp += 4; }                                 /* _CItanh */
#undef FP_ST

static void crt_memmove_tail_00265380(void) { crt_memmove_tail(0x00265380u); }
static void crt_memmove_tail_00265394(void) { crt_memmove_tail(0x00265394u); }
static void crt_memmove_tail_0026540C(void) { crt_memmove_tail(0x0026540Cu); }
static void crt_memmove_tail_00265430(void) { crt_memmove_tail(0x00265430u); }
static void crt_memmove_tail_002654B4(void) { crt_memmove_tail(0x002654B4u); }

static void log_manual_gap(uint32_t va)
{
    uint32_t bit = 0;
    switch (va) {
    case 0x0005FE10u: bit = 1u << 0; break;
    case 0x00062AA0u: bit = 1u << 1; break;
    case 0x00068D80u: bit = 1u << 2; break;
    case 0x00080790u: bit = 1u << 3; break;
    case 0x00095480u: bit = 1u << 4; break;
    case 0x000990D0u: bit = 1u << 5; break;
    case 0x0009C200u: bit = 1u << 6; break;
    case 0x0009E220u: bit = 1u << 7; break;
    case 0x0032BB13u: bit = 1u << 8; break;
    case 0x00022AA0u: bit = 1u << 9; break;
    case 0x0006ABC0u: bit = 1u << 10; break;
    case 0x0007BB40u: bit = 1u << 11; break;
    case 0x0009B360u: bit = 1u << 12; break;
    case 0x00099240u: bit = 1u << 13; break;
    case 0x000C5C60u: bit = 1u << 14; break;
    case 0x000C5C80u: bit = 1u << 15; break;
    case 0x000D93D0u: bit = 1u << 16; break;
    case 0x000ECD30u: bit = 1u << 17; break;
    case 0x000EFD50u: bit = 1u << 18; break;
    case 0x00106430u: bit = 1u << 19; break;
    case 0x00106470u: bit = 1u << 20; break;
    case 0x00107940u: bit = 1u << 21; break;
    case 0x0010ED90u: bit = 1u << 22; break;
    case 0x00112D90u: bit = 1u << 23; break;
    case 0x00115EF0u: bit = 1u << 24; break;
    case 0x00229CD0u: bit = 1u << 25; break;
    case 0x002578F0u: bit = 1u << 26; break;
    case 0x00083240u: bit = 1u << 27; break;
    default: break;
    }
    if (bit && !(manual_gap_logged & bit)) {
        manual_gap_logged |= bit;
        fprintf(stderr, "[ICALL] manual gap callback 0x%08X\n", va);
        fflush(stderr);
    }
}

static uint32_t ensure_audio_stub(void)
{
    if (audio_stub_object)
        return audio_stub_object;

    audio_stub_vtable = audio_alloc_zero(0x20, 16);
    audio_stub_control_vtable = audio_alloc_zero(0x20, 16);
    audio_stub_object = audio_alloc_zero(0x90, 16);
    if (!audio_stub_vtable || !audio_stub_control_vtable || !audio_stub_object)
        return 0;

    g_ecx = audio_stub_object;
    PUSH32(g_esp, 0);
    sub_0021F470();

    MEM32(audio_stub_vtable) = AUDIO_NOOP_TICK_VA;
    MEM32(audio_stub_control_vtable) = AUDIO_NOOP_DESTROY_VA;
    MEM32(audio_stub_control_vtable + 8) = AUDIO_NOOP_SET_PTR_VA;
    MEM32(audio_stub_control_vtable + 0xC) = AUDIO_NOOP_SET_FLOAT_B_VA;
    MEM32(audio_stub_control_vtable + 0x10) = AUDIO_NOOP_SET_FLOAT_A_VA;

    MEM32(audio_stub_object) = audio_stub_vtable;
    MEM8(audio_stub_object + 0x4C) = 1;

    for (uint32_t i = 0; i < 4; i++) {
        audio_stub_controls[i] = audio_alloc_zero(0x20, 16);
        if (!audio_stub_controls[i])
            return 0;
        MEM32(audio_stub_controls[i]) = audio_stub_control_vtable;
        MEM32(audio_stub_object + 0x28 + i * 4) = audio_stub_controls[i];
    }

    return audio_stub_object;
}

/* ── Replaced functions ────────────────────────────────────── */

/*
 * Defining a function here as `void sub_XXXXXXXX(void)` replaces the lifted
 * body entirely: tools.recomp --exclude-manual scans this file, declares
 * those addresses but does not generate them, and the definition below links
 * in their place. That is the only way to intercept a *direct* call --
 * recomp_lookup_manual is consulted for indirect calls and dispatch-table
 * lookups, and generated code reaches a direct call through RECOMP_ABI_CALL,
 * which calls the C symbol and never asks the lookup.
 */

/*
 * 0x001616E3 -- boot-time disc media check.
 *
 * The title opens \Device\CdRom0 and sends it a SCSI pass-through command to
 * identify the disc it booted from. There is no Xbox DVD drive on a PC, so
 * NtOpenFile returns STATUS_OBJECT_PATH_NOT_FOUND; the title answers that by
 * writing a launch data page addressed to its own title ID (0x4C410004) and
 * calling HalReturnToFirmware(2). On hardware that reboots the console back
 * into the game; here it is simply an exit, sixteen kernel calls into the
 * boot and long before anything is drawn.
 *
 * The game already contains the branch this override takes. It only probes
 * the drive when the XBE certificate's AllowedMedia is exactly DVD_X2, and
 * returns 0 for every other media type -- a title installed to the hard disk
 * never touches the drive. Returning 0 is therefore the title's own answer
 * for the case that actually applies here: this build is not running from a
 * DVD.
 *
 * cdecl, no arguments, NTSTATUS in eax where 0 is success. The esp += 4 pops
 * the guest return address, exactly as the lifted `ret` it replaces would.
 */
void sub_001616E3(void)
{
    g_eax = 0;          /* STATUS_SUCCESS */
    g_esp += 4;         /* ret */
}

/*
 * 0x002DFD50 -- create the title's D3D device.
 *
 * The device uses +0x2C as its submitted command sequence and +0x30 as a
 * pointer to the GPU-owned completion sequence. The hardware writes the
 * latter asynchronously; register the runtime's general fence mirror once
 * the device pointer is published so command-queue waits observe completion.
 */
void sub_002DFD50(void)
{
    static int fence_registered;
    uint32_t ebp = g_ebp;
    int _flags = 0;
    uint32_t _fa = 0, _fb = 0;
    int32_t _fas = 0, _fbs = 0;
    (void)_flags; (void)_fa; (void)_fb; (void)_fas; (void)_fbs;

    PUSH32(g_esp, ebp);
    ebp = g_esp;
    g_ebp = ebp;
    g_seh_ebp = ebp;
    g_eax = MEM32(0x2E6840);
    if (!g_eax)
        MEM32(0x2E6840) = 0x80000;

    g_eax = MEM32(0x2E683C);
    if (!g_eax)
        MEM32(0x2E683C) = 0x8000;

    g_eax = MEM32(ebp + 0x14) & 0x10;
    g_ecx = MEM32(ebp + 0x18);
    PUSH32(g_esp, g_esi);
    g_esi = MEM32(0x2E44F8);
    PUSH32(g_esp, g_edi);
    g_edi = 0x2E44F0;
    g_esi |= g_eax;
    PUSH32(g_esp, g_ecx);
    g_ecx = g_edi;
    MEM32(0x2E44E8) = g_edi;
    MEM32(0x2E49EC) = 1;
    MEM32(0x2E44F8) = g_esi;
    if (!fence_registered) {
        xbox_Nv2aMirrorFence(0x2E44E8, 0x2C, 0x30);
        fence_registered = 1;
    }
    g_ebp = ebp;
    g_seh_ebp = ebp;
    PUSH32(g_esp, 0x002DFDAFu); RECOMP_ABI_CALL(0x002D9FE0u, sub_002D9FE0);

    g_esi = g_eax;
    if ((int32_t)g_esi < 0) {
        g_ecx = g_edi;
        g_ebp = ebp;
        g_seh_ebp = ebp;
        PUSH32(g_esp, 0x002DFDBCu); RECOMP_ABI_CALL(0x002DA540u, sub_002DA540);
        g_eax = MEM32(ebp + 0x1C);
        if (g_eax)
            MEM32(g_eax) = 0;
        g_eax = 0;
        for (g_ecx = 0; g_ecx < 0x810; g_ecx++)
            MEM32(g_edi + g_ecx * 4) = g_eax;
        POP32(g_esp, g_edi);
        g_eax = g_esi;
        MEM32(0x2E44E8) = 0;
        POP32(g_esp, g_esi);
        POP32(g_esp, g_ebp);
        g_esp += 28;
        return;
    }

    g_eax = MEM32(ebp + 0x1C);
    if (g_eax)
        MEM32(g_eax) = g_edi;
    POP32(g_esp, g_edi);
    g_eax = 0;
    POP32(g_esp, g_esi);
    POP32(g_esp, g_ebp);
    g_esp += 28;
}

/*
 * 0x00222090 -- initialise the streamed sound-bank resource.
 *
 * The title treats a missing DirectSound/APU resource as the same fatal path as
 * bad media: sub_00221F10 converts a null out-resource at audio object +0x7C
 * into E_FAIL, and the front-end reports the disc as dirty or damaged. Asset
 * I/O has already succeeded by this point; what is missing is enough DSOUND
 * backend state to publish a resource table. Keep audio non-fatal for boot by
 * returning a zeroed, table-shaped guest block and logging the recovery.
 */
/* Retired 2026-09-25: hle_dsound.c replaces DirectSound itself, so the
 * title's own sound-bank code runs. Kept for reference; not linked. */
static void cw_audio_bypass_00222090(void)
{
    enum { AUDIO_PLACEHOLDER_SIZE = 0x120 };
    static uint32_t placeholder;
    uint32_t out_resource = MEM32(g_esp + 20);

    if (!placeholder) {
        placeholder = xbox_HeapAlloc(AUDIO_PLACEHOLDER_SIZE, 16);
        if (placeholder) {
            for (uint32_t off = 0; off < AUDIO_PLACEHOLDER_SIZE; off += 4)
                MEM32(placeholder + off) = 0;
            /* The game rejects tables with more than 0x10 entries. Zero means
             * no playable regions, which is quiet but valid enough to boot. */
            MEM32(placeholder) = 0;
        }
        fprintf(stderr,
                "[AUDIO] sound-bank resource init bypass: placeholder=0x%08X\n",
                placeholder);
    }

    if (out_resource)
        MEM32(out_resource) = placeholder;
    g_eax = placeholder ? 0 : 0x8007000Eu; /* S_OK or E_OUTOFMEMORY */
    g_esp += 24; /* ret 20 */
}

/*
 * 0x00222360 -- attach/finalise the sound-bank resource.
 *
 * Paired with sub_00222090 above. The real DSOUND path expects a fully formed
 * resource table; while audio is intentionally bypassed for boot, finalising
 * that placeholder must be a quiet success instead of dereferencing it.
 */
/* Retired 2026-09-25: hle_dsound.c replaces DirectSound itself, so the
 * title's own sound-bank code runs. Kept for reference; not linked. */
static void cw_audio_bypass_00222360(void)
{
    fprintf(stderr, "[AUDIO] sound-bank finalise bypass\n");
    g_eax = 0;
    g_esp += 16; /* ret 12 */
}

/*
 * 0x00221D80 -- front-end audio system initialisation.
 *
 * The deeper DirectSound bring-up is still incomplete. Letting it run with
 * placeholder resources reaches the title's audio cleanup path and spins
 * before the menu. The boot front-end does require a published singleton,
 * though: it immediately dereferences the object and four child controls.
 * Publish a guest-side quiet object whose control vtables are no-ops so audio
 * remains non-fatal without driving the title into the dirty-disc UI.
 */
/* Retired 2026-09-25: the real audio bring-up runs again now that exception
 * delivery, the allocator flag bug and empty directory listings are fixed.
 * Kept for reference; not linked under the game's symbol. */
static void cw_audio_bypass_00221D80(void)
{
    uint32_t audio = ensure_audio_stub();
    fprintf(stderr, "[AUDIO] system init bypass: singleton=0x%08X\n", audio);
    MEM32(0x608450) = audio;
    if (audio && getenv("CW_ENABLE_AUDIO_POSTINIT")) {
        fprintf(stderr, "[AUDIO] post-publish setup enabled\n");
        g_ecx = audio;
        PUSH32(g_esp, 0x00221E61u);
        sub_00220ED0();
        PUSH32(g_esp, 6);
        PUSH32(g_esp, 0x00221ED4u);
        sub_00224100();
    }
    g_eax = audio ? 1 : 0;
    g_esp += 4; /* ret */
}

/*
 * Three entry points the disassembler never found: they sit in gaps between
 * functions and are reached only through vtables, so every call to them was
 * dropped ("[ICALL] Failed to resolve VA"). Hand-lifted from the bytes.
 *
 * 0x000A6AF0 -- this-adjusting thunk (add ecx, 4; jmp 0x000D84C0). Called
 *   ~240 times per level; skipped, objects never got the call meant for their
 *   base at +4 -- the leaked full-screen texture per level load traced to here
 *   and to 0x000637E0.
 * 0x000637E0 -- level shutdown: Release()s and clears the global render
 *   resources (a 20-entry table at 0x3FAEC8, stride 0xA8, three refs each,
 *   then the singles at 0x3FAE68..0x3FAEB8). Skipped, each level's set stayed
 *   alive.
 * 0x00120650 -- small frontend handler (two calls on the object 0x0016F910
 *   returns, the second a tail call).
 */
static void rel_obj(uint32_t obj, uint32_t ret_va)
{
    g_ecx = obj;
    PUSH32(g_esp, ret_va); RECOMP_ABI_CALL(0x0022C7A0u, sub_0022C7A0);
}

static void manual_sub_000A6AF0(void)
{
    g_ecx += 4;
    sub_000D84C0();                 /* tail jmp: same return address */
}

static void manual_sub_000637E0(void)
{
    uint32_t p, i;
    PUSH32(g_esp, g_esi);
    PUSH32(g_esp, g_edi);
    for (p = 0x3FAED0u; p < 0x3FBBF0u; p += 0xA8u) {
        if (MEM32(p - 8)) rel_obj(MEM32(p - 8), 0x000637FCu);
        MEM32(p - 8) = 0;
        if (MEM32(p)) rel_obj(MEM32(p), 0x0006380Au);
        MEM32(p) = 0;
        if (MEM32(p + 4)) rel_obj(MEM32(p + 4), 0x00063818u);
        MEM32(p + 4) = 0;
    }
    MEM32(0x3FAEA0u) = 0;
    for (i = 0; i < 0xC; i += 4) {          /* original: unconditional calls */
        if (MEM32(i + 0x3FAE94u)) rel_obj(MEM32(i + 0x3FAE94u), 0x0006383Cu);
        MEM32(i + 0x3FAE94u) = 0;
        if (MEM32(i + 0x3FAE7Cu)) rel_obj(MEM32(i + 0x3FAE7Cu), 0x0006384Du);
        MEM32(i + 0x3FAE7Cu) = 0;
    }
    for (p = 0x3FAE68u; p < 0x3FAE74u; p += 4) {
        if (MEM32(p)) rel_obj(MEM32(p), 0x00063867u);
        MEM32(p) = 0;
    }
    {
        static const uint32_t singles[] = { 0x3FAE90u, 0x3FAE8Cu, 0x3FAE88u, 0x3FAE78u, 0x3FAE74u,
                                            0x3FAEACu, 0x3FAEB0u, 0x3FAEB4u, 0x3FAEB8u };
        for (i = 0; i < sizeof singles / sizeof singles[0]; i++) {
            if (MEM32(singles[i])) rel_obj(MEM32(singles[i]), 0x000638D4u);
            MEM32(singles[i]) = 0;
        }
    }
    POP32(g_esp, g_edi);
    POP32(g_esp, g_esi);
    g_esp += 4; /* ret */
}

static void manual_sub_00120650(void)
{
    PUSH32(g_esp, 0);
    PUSH32(g_esp, 0x00120657u); RECOMP_ABI_CALL(0x0016F910u, sub_0016F910);
    g_ecx = g_eax;
    PUSH32(g_esp, 0x0012065Eu); RECOMP_ABI_CALL(0x0016F970u, sub_0016F970);   /* ret 4 pops the 0 */
    PUSH32(g_esp, 0x00120663u); RECOMP_ABI_CALL(0x0016F910u, sub_0016F910);
    g_ecx = g_eax;
    sub_0016E060();                 /* tail jmp */
}

/*
 * 0x0005FE10 -- resource/event callback registered by the frontend loading
 * table. The scanner missed this entrypoint because it sits in a gap between
 * generated functions, but the title passes it as a callback and later calls
 * it indirectly. This is a manual lift of the original dispatcher.
 */
static void manual_sub_0005FE10(void)
{
    uint32_t key = MEM32(g_esp + 4);
    uint32_t saved_ebx = g_ebx, saved_esi = g_esi, saved_edi = g_edi;
    uint32_t saved_ebp = g_ebp;
    log_manual_gap(0x0005FE10u);

    if (key == 0x2EC54500u) {
        PUSH32(g_esp, 0x0005FE2Au); RECOMP_ABI_CALL(0x0005F950u, sub_0005F950);
        g_ebx = saved_ebx; g_esi = saved_esi; g_edi = saved_edi; g_ebp = saved_ebp;
        sub_0005FC80();
        return;
    }

    if (key == 0x15F07936u)
        { PUSH32(g_esp, 0x0005FE35u); RECOMP_ABI_CALL(0x0005F9E0u, sub_0005F9E0); }

    g_edi = 1;
    g_ebx = 0x3FA440u;
    while (g_ebx < 0x3FA520u) {
        g_eax = MEM32(g_edi * 4 + 0x3FA598u);
        if (key == MEM32(g_eax + 0x24)) {
            PUSH32(g_esp, 0); PUSH32(g_esp, g_edi);
            PUSH32(g_esp, 0x0005FEE9u); RECOMP_ABI_CALL(0x00263060u, sub_00263060);
            g_esp += 8;
            if (g_eax == 0) {
                PUSH32(g_esp, 0x0005F2A0u);
                PUSH32(g_esp, 0x0005FF09u); RECOMP_ABI_CALL(0x00167BC0u, sub_00167BC0);
                g_esp += 4;
                MEM32(0x3F9DA4u) = 0x0005F1B0u;
                MEM32(0x3F9DA0u) = g_edi;
                break;
            }
            PUSH32(g_esp, 1); PUSH32(g_esp, g_edi);
            PUSH32(g_esp, 0x0005FEF8u); RECOMP_ABI_CALL(0x00263060u, sub_00263060);
            g_esp += 8;
            if (g_eax == 0) {
                PUSH32(g_esp, 0x0005F2A0u);
                PUSH32(g_esp, 0x0005FF09u); RECOMP_ABI_CALL(0x00167BC0u, sub_00167BC0);
                g_esp += 4;
                MEM32(0x3F9DA4u) = 0x0005F1B0u;
                MEM32(0x3F9DA0u) = g_edi;
            }
            break;
        }

        g_ecx = MEM32(g_edi * 4 + 0x3FA520u);
        if (key == MEM32(g_ecx + 0x24)) {
            PUSH32(g_esp, g_edi);
            PUSH32(g_esp, 0x0005FF24u); RECOMP_ABI_CALL(0x0005F2F0u, sub_0005F2F0);
            g_esp += 4;
            break;
        }

        g_esi = 0;
        while (g_esi < 2) {
            g_eax = g_ebx + g_esi * 4;
            g_edx = MEM32(g_eax);
            if (key == MEM32(g_edx + 0x24)) {
                PUSH32(g_esp, g_esi); PUSH32(g_esp, g_edi);
                PUSH32(g_esp, 0x0005FE8Du); RECOMP_ABI_CALL(0x002630F0u, sub_002630F0);
                g_esp += 8;
                uint32_t active = (g_eax & 0xFFu) ? 0u : 1u;
                PUSH32(g_esp, active); PUSH32(g_esp, g_esi); PUSH32(g_esp, g_edi);
                PUSH32(g_esp, 0x0005FE9Bu); RECOMP_ABI_CALL(0x002630C0u, sub_002630C0);
                g_esp += 12;
                PUSH32(g_esp, g_esi); PUSH32(g_esp, g_edi);
                PUSH32(g_esp, 0x0005FEA2u); RECOMP_ABI_CALL(0x002630F0u, sub_002630F0);
                g_esp += 8;
                g_eax = (g_eax & 0xFFu) ? 0x33D940u : 0x33D93Cu;
                PUSH32(g_esp, g_eax);
                g_eax = g_esi + g_edi * 2;
                g_ecx = MEM32(g_eax * 4 + 0x3FA2D8u);
                PUSH32(g_esp, 0x0005FEBEu); RECOMP_ABI_CALL(0x0005F130u, sub_0005F130);
                break;
            }
            g_esi++;
        }

        g_ecx = MEM32(g_edi * 4 + 0x3FA3C0u);
        if (key == MEM32(g_ecx + 0x24)) {
            g_edx = MEM32(g_edi * 4 + 0x3FA3C0u);
            PUSH32(g_esp, 0);
            MEMF(g_esp) = (float)((double)(int32_t)MEM32(g_edx + 0x28) *
                                  (double)MEMF(0x334C68u));
            PUSH32(g_esp, g_edi);
            PUSH32(g_esp, 0x0005FF43u); RECOMP_ABI_CALL(0x00263120u, sub_00263120);
            g_esp += 8;
            break;
        }

        g_ebx += 8;
        g_edi++;
    }

    g_esi = 1;
    while (g_esi < 0x44) {
        g_eax = MEM32(g_esi * 4 + 0x3FA0E0u);
        if (key == MEM32(g_eax + 0x24)) {
            PUSH32(g_esp, 0); PUSH32(g_esp, g_esi);
            PUSH32(g_esp, 0x0005FF7Bu); RECOMP_ABI_CALL(0x00263240u, sub_00263240);
            g_esp += 8;
            if (g_eax == 0) {
                PUSH32(g_esp, 0x0005F350u);
                PUSH32(g_esp, 0x0005FF9Bu); RECOMP_ABI_CALL(0x00167BD0u, sub_00167BD0);
                g_esp += 4;
                MEM32(0x3F9DA0u) = g_esi;
                MEM32(0x3F9DA4u) = 0x0005F390u;
                break;
            }
            PUSH32(g_esp, 1); PUSH32(g_esp, g_esi);
            PUSH32(g_esp, 0x0005FF8Au); RECOMP_ABI_CALL(0x00263240u, sub_00263240);
            g_esp += 8;
            if (g_eax == 0) {
                PUSH32(g_esp, 0x0005F350u);
                PUSH32(g_esp, 0x0005FF9Bu); RECOMP_ABI_CALL(0x00167BD0u, sub_00167BD0);
                g_esp += 4;
                MEM32(0x3F9DA0u) = g_esi;
                MEM32(0x3F9DA4u) = 0x0005F390u;
            }
            break;
        }
        g_ecx = MEM32(g_esi * 4 + 0x3F9FD0u);
        if (key == MEM32(g_ecx + 0x24)) {
            PUSH32(g_esp, g_esi);
            PUSH32(g_esp, 0x0005FFB9u); RECOMP_ABI_CALL(0x0005F480u, sub_0005F480);
            g_esp += 4;
            break;
        }
        g_esi++;
    }

    g_ebx = saved_ebx; g_esi = saved_esi; g_edi = saved_edi; g_ebp = saved_ebp;
    g_esp += 4; /* ret */
}

/*
 * 0x00068D80 -- setup callback for a small frontend object family. The
 * original function mainly allocates three resource descriptors and publishes
 * the first wrapper at 0x3FC074.
 */
static void manual_sub_00068D80(void)
{
    uint32_t saved_esi = g_esi, saved_edi = g_edi;
    log_manual_gap(0x00068D80u);

    PUSH32(g_esp, 0x14); PUSH32(g_esp, 0x10);
    PUSH32(g_esp, 0x00068DA1u); RECOMP_ABI_CALL(0x0022EF10u, sub_0022EF10);
    g_esp += 8;
    g_edi = g_eax;
    uint32_t vt = MEM32(g_edi);
    uint32_t saved = g_esp;
    PUSH32(g_esp, 0x14);
    g_ecx = g_edi;
    PUSH32(g_esp, 0x00068DAFu);
    {
        uint32_t target = MEM32(vt + 4);
        recomp_func_t fn = recomp_lookup_manual(target);
        if (!fn) fn = recomp_lookup(target);
        if (!fn) fn = recomp_lookup_kernel(target);
        if (fn) {
            RECOMP_ABI_CALL(target, fn);
        } else {
            recomp_icall_fail_log(target);
            g_esp = saved;
            g_eax = 0;
        }
    }
    g_esi = g_eax;
    MEM32(g_esi + 0x10) = g_edi;
    MEM32(g_esi + 4) = 3;
    MEM32(g_esi + 8) = 0;

    PUSH32(g_esp, 0x1C); PUSH32(g_esp, 0x20);
    PUSH32(g_esp, 0x00068DCEu); RECOMP_ABI_CALL(0x0022EF10u, sub_0022EF10);
    g_esp += 8;
    MEM32(g_esi + 4) = 0;
    MEM32(g_esi + 8) = 0;
    MEM32(g_esi + 0xC) = g_eax;
    MEM32(g_esi) = 0x33E58Cu;
    MEM32(0x3FC074u) = g_esi;

    for (uint32_t i = 0; i < 3; i++) {
        static const uint32_t tags[3] = { 0x4C504E54u, 0x4C535054u, 0x4C444952u };
        static const uint32_t names[3] = { 0x33E5B0u, 0x33E5A4u, 0x33E594u };
        static const uint32_t modes[3] = { 1u, 0u, 2u };
        PUSH32(g_esp, 4); PUSH32(g_esp, 1); PUSH32(g_esp, 0x9C);
        PUSH32(g_esp, 0x00068DF9u + i); RECOMP_ABI_CALL(0x002264D0u, sub_002264D0);
        g_esp += 12;
        if (g_eax) {
            PUSH32(g_esp, modes[i]); PUSH32(g_esp, names[i]); PUSH32(g_esp, tags[i]);
            g_ecx = g_eax;
            PUSH32(g_esp, 0x00068E1Fu + i); RECOMP_ABI_CALL(0x00068BB0u, sub_00068BB0);
        }
    }

    g_esi = saved_esi; g_edi = saved_edi;
    g_esp += 4; /* ret */
}

/*
 * 0x0009C200 -- particle/UI table initialiser. This entrypoint is registered
 * in many frontend factories but was absent from the generated dispatch table.
 */
static void manual_sub_0009C200(void)
{
    uint32_t saved_ecx = g_ecx, saved_ebx = g_ebx, saved_esi = g_esi;
    log_manual_gap(0x0009C200u);

    for (uint32_t i = 0; i < 0x32; i++) {
        PUSH32(g_esp, 0x0009C215u); RECOMP_ABI_CALL(0x00237E30u, sub_00237E30);
        double v = g_fp_stack[g_fp_top];
        v *= MEMF(0x335088u);
        v += MEMF(0x3353B8u);
        v *= (double)(int32_t)i;
        v *= MEMF(0x335038u);
        v += MEMF(0x334C68u);
        v = MEMF(0x334BE8u) - v;
        MEMF(i * 4 + 0x441A88u) = (float)v;
        g_fp_top = (g_fp_top + 1u) & 7u;

        PUSH32(g_esp, 0x0009C243u); RECOMP_ABI_CALL(0x00237E30u, sub_00237E30);
        g_fp_stack[g_fp_top] *= MEMF(0x33E114u);
        PUSH32(g_esp, 0x0009C24Eu); RECOMP_ABI_CALL(0x00263894u, sub_00263894);
        MEM8(i + 0x441A50u) = (uint8_t)(g_eax & 0xFFu);
    }

    for (uint32_t a = 0x442809u, n = 0; n < 0x10; n++, a += 0x55C)
        MEM8(a) = 0;
    for (uint32_t a = 0x44791Cu; a < 0x447A5Cu; a += 0x20) {
        MEM32(a - 4) = 0; MEM32(a) = 0; MEM32(a + 4) = 0;
        MEM32(a + 8) = 0; MEM32(a + 0x18) = 0;
    }
    for (uint32_t i = 1; i <= 0x100; i++) {
        double v = (double)(int32_t)(i - 1);
        v *= MEMF(0x3414BCu);
        v *= MEMF(0x447AA4u);
        MEMF(i * 4 + 0x441F4Cu) = (float)sin(v);
        MEMF(i * 4 + 0x441B4Cu) = (float)cos(v);
    }
    for (uint32_t a = 0x447AC0u; a < 0x4489C0u; a += 0x20)
        MEM32(a) = 0;
    for (uint32_t a = 0x4489B0u; a < 0x448A20u; a += 0x1C) {
        MEM32(a - 8) = 0; MEM32(a) = 0;
    }
    for (uint32_t a = 0x44B154u; a < 0x44B514u; a += 0x30)
        MEM32(a) = 0xBF800000u;
    for (uint32_t a = 0x448A20u; a < 0x44B120u; a += 0x270)
        MEM8(a) = 0xFF;
    for (uint32_t a = 0x447A58u; a < 0x447A98u; a += 0x10)
        MEM32(a) = 0;

    g_ecx = saved_ecx; g_ebx = saved_ebx; g_esi = saved_esi;
    g_esp += 4; /* ret */
}

/* ── Manual function overrides ─────────────────────────────── */

/*
 * Return a function pointer to override the given Xbox VA, or NULL
 * to fall through to the auto-generated dispatch table.
 *
 * This is called on every indirect call (RECOMP_ICALL) and every
 * direct call through the dispatch table, so keep it fast. A chain
 * of if-statements on uint32_t compiles to a simple comparison
 * sequence; for large override tables, consider a sorted array
 * with binary search.
 *
 * Examples of common override patterns:
 *
 *   // Trace wrapper: log entry/exit around the generated function
 *   extern void sub_00012345(void);
 *   static void traced_sub_00012345(void) {
 *       fprintf(stderr, "[TRACE] sub_00012345 entered, eax=0x%08X\n", g_eax);
 *       sub_00012345();
 *       fprintf(stderr, "[TRACE] sub_00012345 returned, eax=0x%08X\n", g_eax);
 *   }
 *
 *   // Stub: skip a function entirely (return 0 in eax)
 *   static void stub_00067890(void) {
 *       g_eax = 0;
 *   }
 *
 *   // Fix: replace a broken lifted function with correct C
 *   static void fixed_sub_000ABCDE(void) {
 *       // Read arguments from stack/registers per calling convention
 *       uint32_t arg1 = g_ecx;
 *       uint32_t arg2 = MEM32(g_esp + 4);
 *       // ... correct implementation ...
 *       g_eax = result;
 *   }
 */
recomp_func_t hle_dsound_lookup(uint32_t va);

recomp_func_t recomp_lookup_manual(uint32_t xbox_va)
{
    /* Synthetic entry points of the DirectSound replacement's vtables. */
    if (xbox_va >= 0xFE400000u && xbox_va < 0xFE500000u)
        return hle_dsound_lookup(xbox_va);

    /*
     * TODO: Add your overrides here. Examples:
     *
     * if (xbox_va == 0x00012345) return traced_sub_00012345;
     * if (xbox_va == 0x00067890) return stub_00067890;
     * if (xbox_va == 0x000ABCDE) return fixed_sub_000ABCDE;
     */

    /* The retired audio stub's no-op redirects (AUDIO_NOOP_*_VA) used to be
     * routed here. They reuse real sound-manager method addresses, so with the
     * title's own sound manager running they swallowed its virtual calls --
     * including the one that registers script.ssc, the sound-category script.
     * Removed 2026-09-25 along with the stub. */
    if (xbox_va == 0x00265380u) return crt_memmove_tail_00265380;
    if (xbox_va == 0x00265394u) return crt_memmove_tail_00265394;
    if (xbox_va == 0x0026540Cu) return crt_memmove_tail_0026540C;
    if (xbox_va == 0x00265430u) return crt_memmove_tail_00265430;
    if (xbox_va == 0x002654B4u) return crt_memmove_tail_002654B4;
    if (xbox_va == 0x000A6AF0u) return manual_sub_000A6AF0;
    if (xbox_va == 0x000637E0u) { log_manual_gap(xbox_va); return manual_sub_000637E0; }
    if (xbox_va == 0x00120650u) { log_manual_gap(xbox_va); return manual_sub_00120650; }
    if (xbox_va == 0x0005FE10u)
        { log_manual_gap(xbox_va); return sub_0005FE10; }
    if (xbox_va == 0x00062AA0u)
        { log_manual_gap(xbox_va); return sub_00062AA0; }
    if (xbox_va == 0x00068D80u)
        { log_manual_gap(xbox_va); return sub_00068D80; }
    if (xbox_va == 0x00022AA0u)
        { log_manual_gap(xbox_va); return sub_00022AA0; }
    if (xbox_va == 0x0006ABC0u)
        { log_manual_gap(xbox_va); return sub_0006ABC0; }
    if (xbox_va == 0x0007BB40u)
        { log_manual_gap(xbox_va); return sub_0007BB40; }
    if (xbox_va == 0x00080790u)
        { log_manual_gap(xbox_va); return sub_00080790; }
    if (xbox_va == 0x00083240u)
        { log_manual_gap(xbox_va); return sub_00083240; }
    if (xbox_va == 0x00095480u)
        { log_manual_gap(xbox_va); return sub_00095480; }
    if (xbox_va == 0x000990D0u)
        { log_manual_gap(xbox_va); return sub_000990D0; }
    if (xbox_va == 0x00099240u)
        { log_manual_gap(xbox_va); return sub_00099240; }
    if (xbox_va == 0x0009B360u)
        { log_manual_gap(xbox_va); return sub_0009B360; }
    if (xbox_va == 0x0009C200u)
        { log_manual_gap(xbox_va); return sub_0009C200; }
    if (xbox_va == 0x0009E220u)
        { log_manual_gap(xbox_va); return sub_0009E220; }
    if (xbox_va == 0x000C5C60u)
        { log_manual_gap(xbox_va); return sub_000C5C60; }
    if (xbox_va == 0x000C5C80u)
        { log_manual_gap(xbox_va); return sub_000C5C80; }
    if (xbox_va == 0x000D93D0u)
        { log_manual_gap(xbox_va); return sub_000D93D0; }
    if (xbox_va == 0x000ECD30u)
        { log_manual_gap(xbox_va); return sub_000ECD30; }
    if (xbox_va == 0x000EFD50u)
        { log_manual_gap(xbox_va); return sub_000EFD50; }
    if (xbox_va == 0x00106430u)
        { log_manual_gap(xbox_va); return sub_00106430; }
    if (xbox_va == 0x00106470u)
        { log_manual_gap(xbox_va); return sub_00106470; }
    if (xbox_va == 0x00107940u)
        { log_manual_gap(xbox_va); return sub_00107940; }
    if (xbox_va == 0x0010ED90u)
        { log_manual_gap(xbox_va); return sub_0010ED90; }
    if (xbox_va == 0x00112D90u)
        { log_manual_gap(xbox_va); return sub_00112D90; }
    if (xbox_va == 0x00115EF0u)
        { log_manual_gap(xbox_va); return sub_00115EF0; }
    if (xbox_va == 0x00229CD0u)
        { log_manual_gap(xbox_va); return sub_00229CD0; }
    if (xbox_va == 0x002578F0u)
        { log_manual_gap(xbox_va); return sub_002578F0; }
    if (xbox_va == 0x0032BB13u)
        { log_manual_gap(xbox_va); return sub_0032BB13; }

    return (recomp_func_t)0;
}

/* ── ICALL failure logging ─────────────────────────────────── */

/*
 * Called when RECOMP_ICALL cannot resolve a target address.
 * This usually means one of:
 *   - A vtable dispatch to an address not in the dispatch table
 *   - A function pointer loaded from uninitialized or corrupt memory
 *   - A kernel thunk address that the bridge doesn't handle
 *
 * During early bring-up you will see many of these. Most are harmless
 * (the ICALL macro pops the dummy return address and continues).
 * Focus on the ones that cause crashes or incorrect behavior.
 */
void recomp_icall_fail_log(uint32_t va)
{
    fprintf(stderr, "[ICALL] Failed to resolve VA 0x%08X (total calls: %llu)\n",
            va, (unsigned long long)g_icall_count);
    {
        /* Who made the call: the pushed return address, then likely callers. */
        int k, found = 0;
        fprintf(stderr, "  ret=0x%08X esp=0x%08X stack:", MEM32(g_esp), g_esp);
        for (k = 1; k < 64 && found < 8; k++) {
            uint32_t v = MEM32(g_esp + 4u * (uint32_t)k);
            if (v >= 0x00011000u && v < 0x00330000u) { fprintf(stderr, " %08X", v); found++; }
        }
        fprintf(stderr, "\n");
    }

    /* Dump last 16 call targets from the ring buffer */
    fprintf(stderr, "  Recent ICALL targets:\n");
    for (int i = 0; i < 16; i++) {
        int idx = (g_icall_trace_idx - 16 + i) & 15;
        if (g_icall_trace[idx])
            fprintf(stderr, "    [%2d] 0x%08X\n", i, g_icall_trace[idx]);
    }
    fflush(stderr);
}

/* An indirect call whose target is not code: a null or wild function pointer.
 *
 * Skipping these is right -- calling a data address is worse -- but skipping
 * them *silently* is not. They almost always arrive inside a loop, so the
 * symptom is a hang with no output rather than a diagnosable null vtable call.
 *
 * Rate-limited per address: a spin can produce millions of these, and the
 * useful information is which addresses occur, not how often.
 */
void recomp_icall_not_code_log(uint32_t va)
{
    enum { SLOTS = 16 };
    static uint32_t seen[SLOTS];
    static uint64_t hits[SLOTS];
    static int count;
    int i;

    for (i = 0; i < count; i++)
        if (seen[i] == va)
            break;
    if (i == count) {
        if (count == SLOTS)
            return;
        seen[count] = va;
        hits[count] = 0;
        count++;
    }
    hits[i]++;
    /* Report at 1, 10, 100, 1000 ... rather than once. A single line says a
     * wild pointer was skipped; the progression says it is being skipped in a
     * loop, which is the difference between a curiosity and the reason the
     * title is hung. */
    {
        uint64_t n = hits[i];
        while (n >= 10 && n % 10 == 0)
            n /= 10;
        if (n != 1)
            return;
    }
    {
        uint32_t ret = 0;
        if (g_esp >= 0x10000u && g_esp <= (0x08000000u - 4u))
            ret = MEM32(g_esp);
        fprintf(stderr, "[ICALL] target 0x%08X is not code -- skipped %llu time(s) "
                        "(null or wild function pointer, at call #%llu, ret=0x%08X esp=0x%08X)\n",
                va, (unsigned long long)hits[i],
                (unsigned long long)g_icall_count, ret, g_esp);
    }
    fflush(stderr);
}

