/*
 * hle_xmvtrace.c -- diagnostic wrapper around the XMV player's frame step.
 *
 * 0x00301C83 is the title's linked XMV decoder "get next frame" routine
 * (this, ?, pStatus). With CW_XMV_TRACE=1 it logs the decoder fields that
 * gate stream start (IDirectSound_SynchPlayback at 0x0030213C) so a stalled
 * intro movie shows which condition it is waiting on.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "recomp/gen/recomp_types.h"
#include "recomp/gen/recomp_funcs.h"
#include "xbox_memory_layout.h"

extern unsigned long __stdcall GetTickCount(void);
extern volatile unsigned long xbox_av_field_counter;
#define ARG(n) MEM32(g_esp + 4 + 4 * (n))

/* Every trace in this file is diagnostic and off unless CW_TRACE=1. */
static int trace_on(void)
{
    static int on = -1;
    if (on < 0) { const char *e = getenv("CW_TRACE"); on = e && *e == '1'; }
    return on;
}
#define TRACE(...) do { if (trace_on()) fprintf(stderr, __VA_ARGS__); } while (0)

void sub_00301C83_gen(void);
void sub_00301C83(void)
{
    static int on = -1;
    static unsigned calls;
    uint32_t self = ARG(0), a1 = ARG(1), pst = ARG(2);
    sub_00301C83_gen();
    {
        /* CW_HEAP_TRACE=1: title heap counters once a second of calls. */
        static int ht = -1;
        static unsigned hc;
        if (ht < 0) { const char *e = getenv("CW_HEAP_TRACE"); ht = e && *e == '1'; }
        if (ht && (++hc % 300) == 0) {
            TRACE("[XMVT] t=%u f8=%u c8=%d b8=%u vbl=%u e4=%u c0=%u 3c=%u 60=%X 64=%X 68=%X 6c=%X 74=%X e8=%u\n",
                    (unsigned)GetTickCount(), MEM32(self + 0xf8), (int)MEM32(self + 0xc8),
                    MEM32(self + 0xb8), (unsigned)xbox_av_field_counter, MEM32(self + 0xe4),
                    MEM32(self + 0xc0), MEM32(self + 0x3c), MEM32(self + 0x60), MEM32(self + 0x64),
                    MEM32(self + 0x68), MEM32(self + 0x6c), MEM32(self + 0x74), MEM32(self + 0xe8));
            TRACE("[HEAPSTAT] t=%u free=%u used=%u peak=%u allocs=%u small=%u/%u\n",
                    (unsigned)GetTickCount(), MEM32(0x6086E0), MEM32(0x6086F0), MEM32(0x6086EC),
                    MEM32(0x608730), MEM32(0x6087D8), MEM32(0x6087D4));
            fflush(stderr);
        }
    }
    if (on < 0) { const char *e = getenv("CW_XMV_TRACE"); on = e && *e == '1'; }
    if (!on) return;
    calls++;
    if (calls <= 8 || (calls % 100) == 0) {
        fprintf(stderr,
            "[XMV] t=%u #%u this=%08X a1=%08X ret=%08X status=%u "
            "3c=%X 40=%X 50=%X 60=%X 64=%X 68=%X 6c=%X 74=%X "
            "b0=%X:%X e8=%X f8=%X ac=%X | dev=%08X vbl=%u flags=%08X f1998=%X\n",
            (unsigned)GetTickCount(), calls, self, a1, g_eax, pst ? MEM32(pst) : 0xFFFFFFFFu,
            MEM32(self + 0x3c), MEM32(self + 0x40), MEM32(self + 0x50),
            MEM32(self + 0x60), MEM32(self + 0x64), MEM32(self + 0x68),
            MEM32(self + 0x6c), MEM32(self + 0x74),
            MEM32(self + 0xb0), MEM32(self + 0xb4), MEM32(self + 0xe8),
            MEM32(self + 0xf8), MEM32(self + 0xac),
            MEM32(0x2E44E8), MEM32(0x2E44E8) ? MEM32(MEM32(0x2E44E8) + 0x1988) : 0,
            MEM32(0x2E44E8) ? MEM32(MEM32(0x2E44E8) + 0x197C) : 0,
            MEM32(0x2E44E8) ? MEM32(MEM32(0x2E44E8) + 0x1998) : 0);
        {
            int k;
            TRACE("[XMV]    n=%u b8=%u bc=%u c8=%u e4=%u c0=%u c4=%u", MEM32(self + 0x48), MEM32(self + 0xb8), MEM32(self + 0xbc), MEM32(self + 0xc8), MEM32(self + 0xe4), MEM32(self + 0xc0), MEM32(self + 0xc4));
            for (k = 0; k < 4; k++) {
                uint32_t p = MEM32(self + 0x140 + 4 * k);
                TRACE(" st%X=%08X:%08X", 0x140 + 4 * k, p, p ? MEM32(p) : 0);
            }
            TRACE("\n");
        }
        fflush(stderr);
    }
}


static int s_in_wav_init;

/* 0x0028A906: operator new (size, tag), logged while the WAV init runs. */
void sub_0028A906_gen(void);
void sub_0028A906(void)
{
    uint32_t size = MEM32(g_esp + 4), tag = MEM32(g_esp + 8);
    sub_0028A906_gen();
    if (s_in_wav_init)
        TRACE("[WAV]   new(%u, tag %08X) -> %08X\n", size, tag, g_eax);
    if (s_in_wav_init && !g_eax) {
        /* The title heap: blocks in address order linked by +0xC, magic at +0,
         * size at +4; FD37DA77 marks a free block. */
        uint32_t a = 0x01081040u, k, nfree = 0, nblk = 0, big = 0, tot = 0;
        TRACE("[WAV]   heap anchor=%08X f8=%08X dc=%08X f4=%08X\n",
                MEM32(0x6086FC), MEM32(0x6086F8), MEM32(0x6086DC), MEM32(0x6086F4));
        for (k = 0; k < 400000 && a >= 0x01081040u && a < 0x02A81010u; k++) {
            uint32_t m = MEM32(a), sz = MEM32(a + 4), nx = MEM32(a + 0xC);
            nblk++;
            if (m == 0xFD37DA77u) { nfree++; tot += sz; if (sz > big) big = sz; }
            else if (m != 0x8F241399u && m != 0x5A3F9219u && m != 0x3011DE23u
                     && m != 0x738F238Bu && m != 0x93445FF3u) {
                TRACE("[WAV]   heap: unknown magic %08X at %08X after %u blocks\n", m, a, nblk);
                break;
            }
            if (nx == 0x01B2FDA4u || (nx > 0x01B2FD00u && nx < 0x01B2FE00u)) {
                uint32_t q;
                TRACE("[WAV]   block %08X magic %08X size %08X prev %08X next %08X; words:",
                        a, m, sz, MEM32(a + 8), nx);
                for (q = 0x01B2FD80u; q < 0x01B2FDD0u; q += 4) TRACE(" %08X", MEM32(q));
                TRACE("\n");
            }
            if (nx <= a) { TRACE("[WAV]   heap: next %08X <= %08X\n", nx, a); break; }
            a = nx;
        }
        TRACE("[WAV]   heap: blocks %u, free %u totalling %u, largest %u, walk ended %08X\n",
                nblk, nfree, tot, big, a);
    }
}

/* 0x002701F1: the title's streamed-WAV media object Initialize (thiscall,
 * no stack args). CW_WAV_TRACE=1 logs the object before and after. */
void sub_002701F1_gen(void);
void sub_002701F1(void)
{
    uint32_t self = g_ecx;
    static int on = -1;
    if (on < 0) { const char *e = getenv("CW_WAV_TRACE"); on = e && *e == '1'; }
    if (on)
        TRACE("[WAV] init this=%08X vtbl=%08X +8=%08X +10=%08X\n",
                self, MEM32(self), MEM32(self + 8), MEM32(self + 0x10));
    sub_002701F1_gen();
    if (on) {
        TRACE("[WAV] init -> %08X vtbl=%08X +8=%08X +C=%08X\n",
                g_eax, MEM32(self), MEM32(self + 8), MEM32(self + 0xC));
        fflush(stderr);
    }
}

/* 0x0026F18D: factory (ppObject) that news the object above and inits it. */
void sub_0026F18D_gen(void);
void sub_0026F18D(void)
{
    uint32_t pp = MEM32(g_esp + 4);
    static int on = -1;
    if (on < 0) { const char *e = getenv("CW_WAV_TRACE"); on = e && *e == '1'; }
    s_in_wav_init = on;
    sub_0026F18D_gen();
    s_in_wav_init = 0;
    if (on) {
        TRACE("[WAV] factory(pp=%08X) -> %08X obj=%08X\n",
                pp, g_eax, pp ? MEM32(pp) : 0);
        fflush(stderr);
    }
}

/* 0x002447E0: texture object constructor (thiscall: width, height, pDesc).
 * Logs creations with a width or height below 1, which the title rejects
 * with "texture has invalid dimensions". */
void sub_002447E0_gen(void);
void sub_002447E0(void)
{
    uint32_t w = MEM32(g_esp + 4), h = MEM32(g_esp + 8), d = MEM32(g_esp + 12);
    if ((int32_t)w < 1 || (int32_t)h < 1) {
        int k, found = 0;
        TRACE("[TEX] bad size %d x %d desc=%08X ret=%08X callers:",
                (int32_t)w, (int32_t)h, d, MEM32(g_esp));
        for (k = 1; k < 48 && found < 6; k++) {
            uint32_t v = MEM32(g_esp + 4u * (uint32_t)k);
            if (v >= 0x00011000u && v < 0x00330000u) { TRACE(" %08X", v); found++; }
        }
        TRACE("\n");
        fflush(stderr);
    }
    sub_002447E0_gen();
}

/* 0x00244530: texture size validation (thiscall, name). Logs rejected ones. */
void sub_00244530_gen(void);
void sub_00244530(void)
{
    uint32_t tex = g_ecx, name = MEM32(g_esp + 4);
    int32_t w = (int32_t)MEM32(tex + 0x18), h = (int32_t)MEM32(tex + 0x1c), d = (int32_t)MEM32(tex + 0x20);
    if (w < 1 || h < 1 || d < 1 || (w & (w - 1)) || (h & (h - 1))) {
        char nm[80];
        int k;
        for (k = 0; k < 79 && name && MEM8(name + k); k++) nm[k] = (char)MEM8(name + k);
        nm[k] = 0;
        TRACE("[TEX] '%s' %d x %d x %d flags=%08X\n", nm, w, h, d, MEM32(tex + 0x58));
        fflush(stderr);
    }
    sub_00244530_gen();
}

/* 0x00229440: file object read(buf, size, count) (thiscall). Logs the 32-byte
 * header reads the texture loader makes: which path, result, first words. */
void sub_00229440_gen(void);
void sub_00229440(void)
{
    uint32_t self = g_ecx, buf = MEM32(g_esp + 4), size = MEM32(g_esp + 8), count = MEM32(g_esp + 12);
    uint32_t strm = MEM32(self + 0x20);
    uint32_t fn = strm ? MEM32(MEM32(strm) + 0x14) : 0;
    uint32_t before0 = MEM32(buf + 4);
    sub_00229440_gen();
    if (size == 0x20 && count == 1) {
        TRACE("[RD] file=%08X stream=%08X read_fn=%08X fd=%08X -> %d  hdr: %08X %08X %08X (w before %08X)\n",
                self, strm, fn, MEM32(self + 4), (int)g_eax,
                MEM32(buf), MEM32(buf + 4), MEM32(buf + 8), before0);
        fflush(stderr);
    }
}

/* 0x002E7A46: zlib inflate(strm, flush) (cdecl). Logs error returns and the
 * first calls for each stream, to see where pack decompression goes wrong. */
void sub_002E7A46_gen(void);
void sub_002E7A46(void)
{
    uint32_t z = MEM32(g_esp + 4), fl = MEM32(g_esp + 8);
    uint32_t in0 = z ? MEM32(z + 4) : 0, tin0 = z ? MEM32(z + 8) : 0;
    uint32_t mode0 = z && MEM32(z + 0x1c) ? MEM32(MEM32(z + 0x1c)) : 0xFFFFFFFFu;
    static unsigned n;
    sub_002E7A46_gen();
    if (((int32_t)g_eax < 0 && (int32_t)g_eax != -5) || n++ < 6) {
        uint32_t st = z ? MEM32(z + 0x1c) : 0;
        TRACE("[ZLIB] inflate(%08X, %u) -> %d  avail_in %u->%u total_in %u->%u total_out %u avail_out %u mode %u->%u msg=%08X\n",
                z, fl, (int)g_eax, in0, z ? MEM32(z + 4) : 0, tin0, z ? MEM32(z + 8) : 0,
                z ? MEM32(z + 0x14) : 0, z ? MEM32(z + 0x10) : 0, mode0, st ? MEM32(st) : 0xFFFFFFFFu,
                z ? MEM32(z + 0x18) : 0);
        fflush(stderr);
    }
}

/* 0x0022AA00: pack reader read(dest, size) (thiscall). */
void sub_0022AA00_gen(void);
void sub_0022AA00(void)
{
    uint32_t self = g_ecx, dest = MEM32(g_esp + 4), size = MEM32(g_esp + 8);
    static unsigned n;
    sub_0022AA00_gen();
    static int on = -1;
    if (on < 0) { const char *e = getenv("CW_PACK_TRACE"); on = e && *e == '1'; }
    if (on && n++ < 60000) {
        uint32_t z = self + 0x3c;
        TRACE("[PACK] read dest=%08X size=%u -> %d first=%08X zin=%u zout=%u avail_in=%u reads=%u\n",
                dest, size, (int)g_eax, dest ? MEM32(dest) : 0, MEM32(z + 8), MEM32(z + 0x14),
                MEM32(z + 4), MEM32(self + 0x214));
        fflush(stderr);
    }
}

/* 0x0022B5E0: pack file open/load (thiscall, name, mode). Logs .xbt loads:
 * the buffer it produced and its first words. */
void sub_0022B5E0_gen(void);
void sub_0022B5E0(void)
{
    uint32_t self = g_ecx, name = MEM32(g_esp + 4), mode = MEM32(g_esp + 8);
    char nm[96];
    int k, xbt;
    for (k = 0; k < 95 && name && MEM8(name + k); k++) nm[k] = (char)MEM8(name + k);
    nm[k] = 0;
    xbt = k > 4 && (nm[k-1] == 't' || nm[k-1] == 'T') && (nm[k-2] == 'b' || nm[k-2] == 'B');
    sub_0022B5E0_gen();
    if (xbt) {
        uint32_t buf = MEM32(self + 8), strm = MEM32(self + 0x20);
        TRACE("[OPEN] '%s' mode=%u -> state=%d buf=%08X size=%u cached=%u strm=%08X(%08X+%u) words: %08X %08X %08X\n",
                nm, mode, (int)MEM32(self + 4), buf, MEM32(self + 0xc), MEM8(self + 0x15), strm,
                strm ? MEM32(strm + 4) : 0, strm ? MEM32(strm + 8) : 0,
                buf > 0x10000 ? MEM32(buf) : 0, buf > 0x10000 ? MEM32(buf + 4) : 0, buf > 0x10000 ? MEM32(buf + 8) : 0);
        fflush(stderr);
    }
}

/* 0x0022AB70: pack index lookup by name hash (thiscall, hash) -> node. */
void sub_0022AB70_gen(void);
void sub_0022AB70(void)
{
    uint32_t self = g_ecx, hash = MEM32(g_esp + 4);
    static unsigned n;
    static int dumped;
    if (!dumped && self == 0x011427A0u && getenv("CW_INDEX_DUMP")) {
        /* Walk the archive index tree once: node +8 left, +0xC right,
         * +0x14 hash, +0x18 record {offset, size, csize}. */
        FILE *f = fopen(getenv("CW_INDEX_DUMP"), "w");
        uint32_t stack[4096]; int sp = 0;
        dumped = 1;
        if (f) {
            if (MEM32(self + 8)) stack[sp++] = MEM32(self + 8);
            while (sp > 0) {
                uint32_t nd = stack[--sp], rec = MEM32(nd + 0x18);
                fprintf(f, "%08X %u %u %u\n", MEM32(nd + 0x14), rec ? MEM32(rec) : 0,
                        rec ? MEM32(rec + 4) : 0, rec ? MEM32(rec + 8) : 0);
                if (MEM32(nd + 8) && sp < 4095) stack[sp++] = MEM32(nd + 8);
                if (MEM32(nd + 0xC) && sp < 4095) stack[sp++] = MEM32(nd + 0xC);
            }
            fclose(f);
        }
    }
    sub_0022AB70_gen();
    if (n++ < 3000) {
        uint32_t node = g_eax, rec = node ? MEM32(node + 0x18) : 0;
        TRACE("[FIND] idx=%08X hash=%08X -> node=%08X nodehash=%08X rec=%08X off=%u size=%u csize=%u\n",
                self, hash, node, node ? MEM32(node + 0x14) : 0, rec,
                rec ? MEM32(rec) : 0, rec ? MEM32(rec + 4) : 0, rec ? MEM32(rec + 8) : 0);
        if (hash == 0xC5CE2242u || hash == 0x20189DE4u || hash == 0xBAC4018Au) {
            int k, found = 0;
            TRACE("[FIND]   stack:");
            for (k = 0; k < 400 && found < 24; k++) {
                uint32_t v = MEM32(g_esp + 4u * (uint32_t)k);
                if (v >= 0x00011000u && v < 0x00330000u) { TRACE(" %08X", v); found++; }
            }
            TRACE("\n");
        }
    }
}

/* CRT FILE: _ptr, _cnt, _base, _flag, _file, _charbuf, _bufsiz. */
static void crt_file_dump(const char *tag, uint32_t f)
{
    if (!f) return;
    TRACE("%s FILE=%08X ptr=%08X cnt=%d base=%08X flag=%08X fd=%d bufsiz=%d (ptr-base=%d)\n",
            tag, f, MEM32(f), (int)MEM32(f + 4), MEM32(f + 8), MEM32(f + 12), (int)MEM32(f + 16),
            (int)MEM32(f + 24), (int)(MEM32(f) - MEM32(f + 8)));
}

static unsigned s_crt_logs;

/* 0x002674BE: fseek(FILE*, long offset, int whence) (cdecl). */
void sub_002674BE_gen(void);
void sub_002674BE(void)
{
    uint32_t f = MEM32(g_esp + 4), off = MEM32(g_esp + 8), wh = MEM32(g_esp + 12);
    int log = s_crt_logs < 400 && off > 1000000u && off < 2000000u;
    if (log) { s_crt_logs++; TRACE("[CRT] fseek(%u, %u)\n", off, wh); crt_file_dump("[CRT]   before", f); }
    sub_002674BE_gen();
    if (log) { TRACE("[CRT]   -> %d\n", (int)g_eax); crt_file_dump("[CRT]   after ", f); }
}

/* 0x00266D1A: fread(buf, size, count, FILE*) (cdecl). */
void sub_00266D1A_gen(void);
void sub_00266D1A(void)
{
    uint32_t buf = MEM32(g_esp + 4), sz = MEM32(g_esp + 8), cnt = MEM32(g_esp + 12), f = MEM32(g_esp + 16);
    int log = s_crt_logs > 0 && s_crt_logs < 400;
    sub_00266D1A_gen();
    if (log) {
        TRACE("[CRT] fread(%u x %u) -> %u first=%08X\n", sz, cnt, g_eax, MEM32(buf));
        crt_file_dump("[CRT]   after ", f);
    }
}

/* 0x00066E40: register a material setup callback (id, fn) (cdecl). */
void sub_00066E40_gen(void);
void sub_00066E40(void)
{
    static unsigned n;
    uint32_t a0 = MEM32(g_esp + 4), a1 = MEM32(g_esp + 8);
    sub_00066E40_gen();
    if (n++ < 200) { TRACE("[MAT] register %08X %08X\n", a0, a1); fflush(stderr); }
}

/* 0x00066E60: run every registered setup callback. */
void sub_00066E60_gen(void);
void sub_00066E60(void)
{
    TRACE("[MAT] run all setups (ret %08X)\n", MEM32(g_esp)); fflush(stderr);
    sub_00066E60_gen();
    TRACE("[MAT] run all setups done\n"); fflush(stderr);
}

/* 0x00099240: HeatMaterial setup (loads heat_bumpmap.xbt). */
void sub_00099240_gen(void);
void sub_00099240(void)
{
    TRACE("[MAT] HeatMaterial setup called (ret %08X)\n", MEM32(g_esp)); fflush(stderr);
    sub_00099240_gen();
}

/* Level-load steps in 0x000F3E50, logged to order file loads against them. */
void sub_000EDAC0_gen(void);
void sub_000EDAC0(void)
{
    TRACE("[STEP] enter 000EDAC0 (ret %08X)\n", MEM32(g_esp)); fflush(stderr);
    sub_000EDAC0_gen();
    TRACE("[STEP] leave 000EDAC0\n"); fflush(stderr);
}

void sub_000C2A80_gen(void);
void sub_000C2A80(void)
{
    TRACE("[STEP] enter 000C2A80 (ret %08X)\n", MEM32(g_esp)); fflush(stderr);
    sub_000C2A80_gen();
    TRACE("[STEP] leave 000C2A80\n"); fflush(stderr);
}

void sub_0005F180_gen(void);
void sub_0005F180(void)
{
    TRACE("[STEP] enter 0005F180 (ret %08X)\n", MEM32(g_esp)); fflush(stderr);
    sub_0005F180_gen();
    TRACE("[STEP] leave 0005F180\n"); fflush(stderr);
}

void sub_0005F170_gen(void);
void sub_0005F170(void)
{
    TRACE("[STEP] enter 0005F170 (ret %08X)\n", MEM32(g_esp)); fflush(stderr);
    sub_0005F170_gen();
    TRACE("[STEP] leave 0005F170\n"); fflush(stderr);
}

void sub_000F3E50_gen(void);
void sub_000F3E50(void)
{
    TRACE("[STEP] enter 000F3E50 (ret %08X)\n", MEM32(g_esp)); fflush(stderr);
    sub_000F3E50_gen();
    TRACE("[STEP] leave 000F3E50\n"); fflush(stderr);
}

/* 0x000A18B0: unlink a node (ecx) from its doubly linked list (+8 next,
 * +0xC prev). A node whose neighbours do not point back at it is not in a
 * consistent list; unlinking it writes through garbage. Log it once with every
 * guest dword that points at it (so the list holding it can be found), and
 * skip the unlink. */
void sub_000A18B0_gen(void);
void sub_000A18B0(void)
{
    static int reported;
    uint32_t n = g_ecx, next = MEM32(n + 8), prev = MEM32(n + 0xC);
    int bad = (next && (next >= 0x04000000u || MEM32(next + 0xC) != n)) ||
              (prev && (prev >= 0x04000000u || MEM32(prev + 8) != n));
    if (!bad) { sub_000A18B0_gen(); return; }
    if (reported++ < 8) {
        uint32_t a;
        int refs = 0;
        fprintf(stderr, "[LIST] bad node %08X ret=%08X words: %08X %08X %08X %08X %08X %08X\n",
                n, MEM32(g_esp), MEM32(n), MEM32(n + 4), next, prev, MEM32(n + 0x10), MEM32(n + 0x14));
        for (a = 0x00010000u; a < 0x04000000u && refs < 16; a += 4)
            if (MEM32(a) == n) { fprintf(stderr, "[LIST]   ref at %08X (-8: %08X %08X %08X %08X)\n",
                                          a, MEM32(a - 8), MEM32(a - 4), MEM32(a + 4), MEM32(a + 8)); refs++; }
        fflush(stderr);
    }
    g_esp += 4;   /* ret */
}

/* CRT x87 error-path entries (0x0026A68D one-arg, 0x0026A4BA two-arg): the
 * ABI check sees ebx come back changed through them. Log the first few with
 * the stack slot the epilogue pops ebx from. Diagnostic. */
static void fp_err_probe(const char *name, void (*fn)(void))
{
    static int shown;
    uint32_t b0 = g_ebx, s0 = g_esi, d0 = g_edi, sp0 = g_esp, bp0 = g_ebp;
    fn();
    if ((g_ebx != b0 || g_esi != s0 || g_edi != d0 || g_esp != sp0 + 4) && shown++ < 6) {
        TRACE("[FPERR] %s ret=%08X ebx %08X->%08X esi %08X->%08X edi %08X->%08X esp %08X->%08X "
                "g_ebp %08X slot(ebx)=%08X slot(ebp)=%08X\n",
                name, MEM32(sp0), b0, g_ebx, s0, g_esi, d0, g_edi, sp0, g_esp, bp0,
                MEM32(sp0 - 4 - 0x2D0 - 4), MEM32(sp0 - 4));
        fflush(stderr);
    }
}
void sub_0026A68D_gen(void);
void sub_0026A68D(void) { fp_err_probe("1arg", sub_0026A68D_gen); }
void sub_0026A4BA_gen(void);
void sub_0026A4BA(void) { fp_err_probe("2arg", sub_0026A4BA_gen); }

/* CW_REF_TRACE=1: reference-count calls on the two full-screen textures the
 * level render object creates (0x00245A50 stores them at this+4 / this+8):
 * AddRef 0x0022C770, Release 0x0022C7A0 (ecx = object, count = [obj+4] >> 9). */
static uint32_t s_reftrack[16];
static int s_nreftrack;
static int ref_on(void)
{
    static int on = -1;
    if (on < 0) { const char *e = getenv("CW_REF_TRACE"); on = e && *e == '1'; }
    return on;
}
static int ref_tracked(uint32_t o)
{
    int i;
    for (i = 0; i < s_nreftrack; i++) if (s_reftrack[i] == o) return 1;
    return 0;
}
static void ref_stack(void)
{
    int k, n = 0;
    for (k = 0; k < 64 && n < 8; k++) {
        uint32_t v = MEM32(g_esp + 4 * k);
        if (v > 0x11000u && v < 0x330000u) { fprintf(stderr, " %08X", v); n++; }
    }
}
void sub_00245A50_gen(void);
void sub_00245A50(void)
{
    uint32_t self = g_ecx;
    sub_00245A50_gen();
    if (ref_on()) {
        uint32_t a = MEM32(self + 4), b = MEM32(self + 8);
        if (a && !ref_tracked(a) && s_nreftrack < 16) s_reftrack[s_nreftrack++] = a;
        if (b && !ref_tracked(b) && s_nreftrack < 16) s_reftrack[s_nreftrack++] = b;
        fprintf(stderr, "[REF] level RT object %08X textures %08X (refs %u) %08X (refs %u)\n", self,
                a, a ? MEM32(a + 4) >> 9 : 0, b, b ? MEM32(b + 4) >> 9 : 0);
    }
}
/* 0x00251F00 (thiscall): hands the level texture to the material at
 * this+0x10 (slot 1) -- the reference that outlives the level. Track the
 * object and its material too: AddRef/Release are shared by all of the
 * engine's ref-counted objects. */
void sub_00251F00_gen(void);
void sub_00251F00(void)
{
    uint32_t self = g_ecx;
    if (ref_on() && MEM32(self + 0x18) && MEM32(self + 0x24) == 2) {
        uint32_t mat = MEM32(self + 0x10);
        static int n;
        if (!ref_tracked(self) && s_nreftrack < 16) s_reftrack[s_nreftrack++] = self;
        if (mat && !ref_tracked(mat) && s_nreftrack < 16) s_reftrack[s_nreftrack++] = mat;
        if (n++ < 6) {
            fprintf(stderr, "[REF] 251F00 object %08X (refs %u, vtbl %08X) material %08X (refs %u)\n",
                    self, MEM32(self + 4) >> 9, MEM32(self), mat, mat ? MEM32(mat + 4) >> 9 : 0);
            ref_stack(); fputc(10, stderr);
        }
    }
    sub_00251F00_gen();
}

void sub_0022C770_gen(void);
void sub_0022C770(void)
{
    uint32_t o = g_ecx;
    if (ref_on() && (ref_tracked(o) || (o > 0x10000u && o < 0x04000000u && MEM32(o) == 0x0037ACE4u))) {
        fprintf(stderr, "[REF] AddRef %08X %u->%u from", o, MEM32(o + 4) >> 9, (MEM32(o + 4) >> 9) + 1);
        ref_stack(); fputc(10, stderr);
    }
    sub_0022C770_gen();
}
void sub_0022C7A0_gen(void);
void sub_0022C7A0(void)
{
    uint32_t o = g_ecx;
    if (ref_on() && (ref_tracked(o) || (o > 0x10000u && o < 0x04000000u && MEM32(o) == 0x0037ACE4u))) {
        fprintf(stderr, "[REF] Release %08X %u->%u from", o, MEM32(o + 4) >> 9, (MEM32(o + 4) >> 9) - 1);
        ref_stack(); fputc(10, stderr);
    }
    sub_0022C7A0_gen();
}
