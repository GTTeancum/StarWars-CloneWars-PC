/*
 * guest_bridge.c -- see include/game/guest_bridge.h.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "recomp/gen/recomp_types.h"
#include "xbox_memory_layout.h"
#include "game/guest_bridge.h"

void *gb_host(uint32_t guest)
{
    return guest ? (void *)XBOX_PTR(guest) : NULL;
}

/* The title's RAM as the host sees it (g_xbox_map_size bytes from guest 0).
 * Anything else -- the exe's own literals, the native stack, host heap -- is
 * host memory. */
uint32_t gb_guest(const void *host)
{
    intptr_t d = (intptr_t)host - (intptr_t)g_xbox_mem_offset;
    if (!host)
        return 0;
    if (d >= 0x10000 && (uintptr_t)d < (g_xbox_map_size ? g_xbox_map_size : g_xbox_total_ram))
        return (uint32_t)d;
    return 0;
}

/* ---- interned strings ------------------------------------------------------
 * A string a script passes may be kept by the engine (objective text, object
 * names), as the original passed .rdata literals that never move. So each
 * distinct string is copied once into title memory and kept. */
#define POOL_CHUNK 0x10000u
static uint32_t s_pool, s_pool_left;
static struct ent { uint32_t hash, guest; struct ent *next; } *s_tab[1024];

static uint32_t fnv(const char *s)
{
    uint32_t h = 2166136261u;
    while (*s) h = (h ^ (uint8_t)*s++) * 16777619u;
    return h;
}

static uint32_t intern(const char *s)
{
    uint32_t h = fnv(s), n = (uint32_t)strlen(s) + 1, g;
    struct ent *e;
    for (e = s_tab[h & 1023]; e; e = e->next)
        if (e->hash == h && !strcmp((const char *)XBOX_PTR(e->guest), s))
            return e->guest;
    if (n > s_pool_left) {
        uint32_t sz = n > POOL_CHUNK ? n : POOL_CHUNK;
        s_pool = xbox_ContiguousAlloc(sz, 16);
        s_pool_left = s_pool ? sz : 0;
        if (!s_pool) return 0;
    }
    g = s_pool;
    memcpy((void *)XBOX_PTR(g), s, n);
    s_pool += (n + 3) & ~3u;
    s_pool_left = s_pool_left > ((n + 3) & ~3u) ? s_pool_left - ((n + 3) & ~3u) : 0;
    e = (struct ent *)malloc(sizeof *e);
    e->hash = h; e->guest = g; e->next = s_tab[h & 1023]; s_tab[h & 1023] = e;
    return g;
}

/* ---- calls into the recompiled title -------------------------------------- */

static int spec_num(const char **p)
{
    int n = 0;
    while (**p >= '0' && **p <= '9') n = n * 10 + (*(*p)++ - '0');
    return n;
}

uint32_t gb_call(uint32_t va, uint32_t self, const char *spec, const uint32_t *args, double *fret,
                 const char *name)
{
    static int trace = -1;
    uint32_t ret_val;
    uint32_t sp0 = g_esp, vals[32], nv = 0, i;
    char kinds[32];
    struct { void *host; uint32_t guest, n; } back[8];
    int nback = 0;
    char ret = *spec++;
    const uint32_t *a = args;
    recomp_func_t fn = recomp_lookup(va);

    if (!fn) {
        fprintf(stderr, "[BRIDGE] no recompiled function at %08X\n", va);
        return 0;
    }
    /* Marshal left to right into dwords as they will sit on the guest stack. */
    while (*spec) {
        char c = *spec++;
        kinds[nv] = c;
        if (c == 'i') {
            vals[nv++] = *a++;
        } else if (c == 'q') {
            int n = spec_num(&spec), k;
            for (k = 0; k < (n + 3) / 4; k++) { kinds[nv] = 'i'; vals[nv++] = *a++; }
        } else if (c == 's') {
            const char *h = (const char *)(uintptr_t)*a++;
            uint32_t g = gb_guest(h);
            vals[nv++] = h ? (g ? g : intern(h)) : 0;
        } else if (c == 'p') {
            int n = spec_num(&spec);
            void *h = (void *)(uintptr_t)*a++;
            uint32_t g = gb_guest(h);
            if (h && !g) {
                g_esp = (g_esp - (uint32_t)n) & ~15u;   /* scratch below the args */
                g = g_esp;
                memcpy((void *)XBOX_PTR(g), h, (size_t)n);
                if (nback < 8) { back[nback].host = h; back[nback].guest = g; back[nback].n = (uint32_t)n; nback++; }
            }
            vals[nv++] = g;
        }
    }
    for (i = nv; i-- > 0;)
        PUSH32(g_esp, vals[i]);
    PUSH32(g_esp, 0x00000000u);             /* return address: none in the title */
    if (self) g_ecx = self;
    fn();
    g_esp = sp0;
    for (i = 0; i < (uint32_t)nback; i++)
        memcpy(back[i].host, (void *)XBOX_PTR(back[i].guest), back[i].n);
    if (ret == 'f') {
        double v = g_fp_stack[g_fp_top & 7];
        g_fp_top = (g_fp_top + 1) & 7;
        if (fret) *fret = v;
    }
    ret_val = ret == 'b' ? (g_eax & 0xFF) : g_eax;
    if (trace < 0) trace = getenv("CW_BRIDGE_TRACE") ? atoi(getenv("CW_BRIDGE_TRACE")) : 0;
    if (trace) {
        fprintf(stderr, "[CALL] %s", name ? name : "?");
        if (self) fprintf(stderr, "@%08X", self);
        for (i = 0; i < (uint32_t)nback; i++) fprintf(stderr, "{copy %p->%08X}", back[i].host, back[i].guest);
        fputc('(', stderr);
        for (i = 0; i < nv; i++) {
            char c = kinds[i];
            if (c == 's' && vals[i]) fprintf(stderr, "%s\"%s\"", i ? ", " : "", (const char *)XBOX_PTR(vals[i]));
            else fprintf(stderr, "%s%u", i ? ", " : "", vals[i]);
        }
        if (ret == 'f') fprintf(stderr, ") = %g\n", fret ? *fret : 0.0);
        else fprintf(stderr, ") = %u\n", ret_val);
        fflush(stderr);
    }
    return ret_val;
}

/* ---- hooks: native code standing in for recompiled functions ------------- */

uint32_t gb_this(void) { return g_ecx; }

/* Stack argument `index` of the hooked call (0 = first; the return address
 * is at g_esp). */
uint32_t gb_arg(int index) { return MEM32(g_esp + 4u + 4u * (uint32_t)index); }

void gb_return(uint32_t r, uint32_t popped)
{
    g_eax = r;
    g_esp += 4u + popped;
}

int gb_recovered_on(void)
{
    static int on = -1;
    if (on < 0) on = getenv("CW_RECOVERED") ? atoi(getenv("CW_RECOVERED")) : 1;
    return on;
}

void gb_note(const char *name)
{
    fprintf(stderr, "[RECOVERED] running %s\n", name);
    fflush(stderr);
}

void gb_trap(const char *name)
{
    fprintf(stderr, "[BRIDGE] unmapped engine function called from native code: %s\n", name);
    fflush(stderr);
}
