/*
 * hle_d3d8_psh.inc -- Xbox pixel shaders (NV2A register combiners) on the
 * PC's ps.1.1. Included by hle_d3d8.c after apply_state's helpers.
 *
 * An Xbox pixel shader is a D3DPIXELSHADERDEF the D3D copies into its render
 * state array (indices 0..56, texture modes at 136): up to 8 general combiner
 * stages and a final combiner. Each stage takes four inputs A B C D per
 * portion (RGB and alpha) -- a register, a channel, and an input mapping --
 * and writes A*B (or A.B), C*D (or C.D) and their sum (or a mux on R0.a) to
 * registers, with an output scale/bias. The final combiner computes
 * A*B + (1-A)*C + D for RGB and G for alpha.
 *
 * Each distinct definition (constants excluded) is translated once to ps.1.1
 * tokens and created on the PC device, which validates it; a definition that
 * does not translate or validate falls back to the stage-state path.
 *
 * Mapping:
 *  - registers: V0/V1 -> v0/v1, T0-3 -> t0-3, R0/R1 -> r0/r1, C0/C1 -> PC
 *    constants allocated per shader and loaded per draw from the stage's
 *    PSConstant0/1 (or stage 0's when the unique-constant flags are clear),
 *    ZERO -> a zero constant (UNSIGNED_INVERT of it is 1).
 *  - input mappings -> source modifiers: invert 1-x, expand _bx2, half-bias
 *    _bias, negations; unsigned identity is used unclamped.
 *  - output shift -> _x2/_x4/_d2; bias variants through a following
 *    "mov d, d_bias".
 *  - FOG: the final combiner's lerp(FOG.a, X, FOG.rgb) is the fixed fog
 *    blend, which the PC applies after the pixel shader anyway, so it
 *    reduces to X. FOG elsewhere reads as the fog colour, alpha 1.
 *  - texture modes: PROJECT2D/3D/CUBEMAP -> tex, PASSTHRU -> texcoord;
 *    other modes (bump, dot-product addressing) are not translated.
 */

#define PSH_MAX 128

typedef struct {
    uint32_t key[64];           /* RS 0..56 minus constants, + modes */
    DWORD    pc_ps;
    int      ok;
    int      nconst;
    int16_t  const_src[8];      /* RS index per PC constant, -1 = zero */
} psh_rec;

static psh_rec *s_psh[PSH_MAX];
static int      s_npsh;
static unsigned s_psh_fail_draws;

typedef struct {
    DWORD tok[512];
    int   n, bad, arith;
    char  why[80];
    int   nconst;
    int16_t const_src[8];
    int   stage;
    int   unique_c0, unique_c1;
    int   last_op;              /* token index of the last opcode */
    int8_t vmap[2];             /* V0/V1 redirected to t#, -1 = still v# */
    int   fuse_stage;           /* stage whose R0 sum also adds V1 (V1R0_SUM folded) */
    int   fused;
} psh_out;

enum { XR_ZERO = 0, XR_C0, XR_C1, XR_FOG, XR_V0, XR_V1, XR_T0 = 8, XR_T1, XR_T2, XR_T3, XR_R0, XR_R1,
       XR_V1R0, XR_EF };

#define PS_TEMP 0
#define PS_INPUT 1
#define PS_CONST 2
#define PS_TEX 3

static void pemit(psh_out *o, DWORD t) { if (o->n < 511) o->tok[o->n++] = t; else o->bad = 1; }

/* PC constant for a stage constant (RS index), or the zero constant (-1). */
static int psh_const(psh_out *o, int rs)
{
    int i;
    for (i = 0; i < o->nconst; i++)
        if (o->const_src[i] == rs) return i;
    if (o->nconst >= 8) { o->bad = 1; snprintf(o->why, sizeof o->why, "more than 8 constants"); return 0; }
    o->const_src[o->nconst] = (int16_t)rs;
    return o->nconst++;
}

/* Source token for one combiner input byte. alpha_portion: the input is used
 * by the alpha half, where channel bit clear means blue. */
static DWORD psh_src(psh_out *o, uint32_t in, int alpha_portion, int final)
{
    uint32_t reg = in & 0xF, alpha = in & 0x10, map = in & 0xE0;
    DWORD type = PS_TEMP, idx = 0, mod = 0, swz = 0xE4;
    switch (reg) {
    case XR_ZERO: type = PS_CONST; idx = (DWORD)psh_const(o, -1); break;
    case XR_C0:
        type = PS_CONST;
        idx = (DWORD)psh_const(o, final ? 43 : 10 + (o->unique_c0 ? o->stage : 0));
        break;
    case XR_C1:
        type = PS_CONST;
        idx = (DWORD)psh_const(o, final ? 44 : 18 + (o->unique_c1 ? o->stage : 0));
        break;
    case XR_FOG: type = PS_CONST; idx = (DWORD)psh_const(o, 1000); break;   /* fog colour */
    case XR_V0: case XR_V1:
        if (o->vmap[reg - XR_V0] >= 0) { type = PS_TEX; idx = (DWORD)o->vmap[reg - XR_V0]; }
        else { type = PS_INPUT; idx = reg - XR_V0; }
        break;
    case XR_T0: case XR_T1: case XR_T2: case XR_T3: type = PS_TEX; idx = reg - XR_T0; break;
    case XR_R0: type = PS_TEMP; idx = 0; break;
    case XR_R1: type = PS_TEMP; idx = 1; break;
    default: o->bad = 1; snprintf(o->why, sizeof o->why, "input register %u", reg); break;
    }
    if (alpha) swz = 0xFF;                       /* .a replicate */
    else if (alpha_portion) swz = 0xAA;          /* .b replicate */
    switch (map) {
    case 0x00: mod = 0; break;                   /* unsigned identity */
    case 0x20: mod = D3DSPSM_COMP; break;        /* 1 - x */
    case 0x40: mod = D3DSPSM_SIGN; break;        /* 2x - 1 */
    case 0x60: mod = D3DSPSM_SIGNNEG; break;
    case 0x80: mod = D3DSPSM_BIAS; break;        /* x - 0.5 */
    case 0xA0: mod = D3DSPSM_BIASNEG; break;
    case 0xC0: mod = 0; break;                   /* signed identity */
    case 0xE0: mod = D3DSPSM_NEG; break;
    }
    return 0x80000000u | (type << 28) | (swz << 16) | mod | idx;
}

static int in_is_zero(uint32_t in) { return (in & 0xF) == XR_ZERO && (in & 0xE0) != 0x20; }
static int in_is_one(uint32_t in)  { return (in & 0xF) == XR_ZERO && (in & 0xE0) == 0x20; }

static DWORD psh_dst(uint32_t reg, int mask, DWORD shift, psh_out *o)
{
    DWORD type, idx;
    switch (reg) {
    case XR_R0: type = PS_TEMP; idx = 0; break;
    case XR_R1: type = PS_TEMP; idx = 1; break;
    case XR_T0: case XR_T1: case XR_T2: case XR_T3: type = PS_TEX; idx = reg - XR_T0; break;
    case XR_V0: case XR_V1:
        if (o->vmap[reg - XR_V0] >= 0) { type = PS_TEX; idx = (DWORD)o->vmap[reg - XR_V0]; break; }
        /* fall through */
    default: o->bad = 1; snprintf(o->why, sizeof o->why, "output register %u", reg); type = PS_TEMP; idx = 0; break;
    }
    return 0x80000000u | (type << 28) | ((DWORD)mask << 16) | (shift << 24) | idx;
}

static void pop(psh_out *o, DWORD op, DWORD dst, const DWORD *src, int n)
{
    int k;
    o->last_op = o->n;
    pemit(o, op);
    pemit(o, dst);
    for (k = 0; k < n; k++) pemit(o, src[k]);
    if (op < D3DSIO_TEXCOORD || op > D3DSIO_TEXM3x3VSPEC) o->arith++;   /* texture ops are not arithmetic */
}

/* Does input byte `in` read register `reg`? */
static int reads_reg(uint32_t in, uint32_t reg) { return (in & 0xF) == reg; }

/* One portion of one stage. inputs: A B C D bytes; out: outputs word. */
static void psh_portion(psh_out *o, uint32_t inputs, uint32_t outputs, int alpha)
{
    uint32_t A = (inputs >> 24) & 0xFF, B = (inputs >> 16) & 0xFF, C = (inputs >> 8) & 0xFF, D = inputs & 0xFF;
    uint32_t cd_dst = outputs & 0xF, ab_dst = (outputs >> 4) & 0xF, sum_dst = (outputs >> 8) & 0xF;
    uint32_t flags = (outputs >> 12) & 0xFF;
    int mask = alpha ? 0x8 : 0x7;
    int ab_dot = !alpha && (flags & 0x02), cd_dot = !alpha && (flags & 0x01), mux = flags & 0x04;
    uint32_t scale = flags & 0x38;
    DWORD shift = 0, s[3];
    int bias = 0;

    switch (scale) {
    case 0x08: bias = 1; break;
    case 0x10: shift = 1; break;
    case 0x18: shift = 1; bias = 1; break;
    case 0x20: shift = 2; break;
    case 0x30: shift = 0xF; break;
    }
    if (!ab_dst && !cd_dst && !sum_dst)
        return;
    /* Blue-to-alpha (RGB portion): the product's blue also lands in alpha.
     * A dot product is replicated to all four channels anyway, so the write
     * mask just grows; a plain product gets a second, alpha-only multiply of
     * the blue channels. */
    if (alpha && (flags & 0xC0)) { o->bad = 1; snprintf(o->why, sizeof o->why, "blue-to-alpha on alpha"); return; }

    /* Products into their own destinations first (when requested). */
    if (ab_dst) {
        int b2a = (flags & 0x80) != 0;
        s[0] = psh_src(o, A, alpha, 0); s[1] = psh_src(o, B, alpha, 0);
        pop(o, ab_dot ? D3DSIO_DP3 : D3DSIO_MUL, psh_dst(ab_dst, (b2a && ab_dot) ? 0xF : mask, shift, o), s, 2);
        if (b2a && !ab_dot) {
            if (reads_reg(A, ab_dst) || reads_reg(B, ab_dst)) { o->bad = 1; snprintf(o->why, sizeof o->why, "b2a alias"); return; }
            s[0] = psh_src(o, A, 1, 0); s[1] = psh_src(o, B, 1, 0);
            pop(o, D3DSIO_MUL, psh_dst(ab_dst, 0x8, shift, o), s, 2);
        }
    }
    if (cd_dst) {
        int b2a = (flags & 0x40) != 0;
        s[0] = psh_src(o, C, alpha, 0); s[1] = psh_src(o, D, alpha, 0);
        if ((ab_dst && (reads_reg(C, ab_dst) || reads_reg(D, ab_dst)))) {
            o->bad = 1; snprintf(o->why, sizeof o->why, "cd reads ab output"); return;
        }
        pop(o, cd_dot ? D3DSIO_DP3 : D3DSIO_MUL, psh_dst(cd_dst, (b2a && cd_dot) ? 0xF : mask, shift, o), s, 2);
        if (b2a && !cd_dot) {
            s[0] = psh_src(o, C, 1, 0); s[1] = psh_src(o, D, 1, 0);
            pop(o, D3DSIO_MUL, psh_dst(cd_dst, 0x8, shift, o), s, 2);
        }
    }
    if (sum_dst) {
        if (ab_dot || cd_dot) {
            if (!ab_dst || !cd_dst) { o->bad = 1; snprintf(o->why, sizeof o->why, "dot sum"); return; }
            s[0] = psh_src(o, 0xC0 | (ab_dst), alpha, 0); s[1] = psh_src(o, 0xC0 | (cd_dst), alpha, 0);
            pop(o, D3DSIO_ADD, psh_dst(sum_dst, mask, 0, o), s, 2);
        } else if (mux) {
            /* sum = r0.a > 0.5 ? CD : AB */
            DWORD cond = 0x80000000u | (PS_TEMP << 28) | (0xFFu << 16) | 0;
            if (ab_dst && cd_dst) {
                s[0] = cond; s[1] = psh_src(o, 0xC0 | cd_dst, alpha, 0); s[2] = psh_src(o, 0xC0 | ab_dst, alpha, 0);
                pop(o, D3DSIO_CND, psh_dst(sum_dst, mask, 0, o), s, 3);
            } else if (in_is_one(B) && in_is_one(D)) {
                s[0] = cond; s[1] = psh_src(o, C, alpha, 0); s[2] = psh_src(o, A, alpha, 0);
                pop(o, D3DSIO_CND, psh_dst(sum_dst, mask, shift, o), s, 3);
            } else {
                o->bad = 1; snprintf(o->why, sizeof o->why, "mux of products"); return;
            }
        } else if (ab_dst && cd_dst) {
            s[0] = psh_src(o, 0xC0 | ab_dst, alpha, 0); s[1] = psh_src(o, 0xC0 | cd_dst, alpha, 0);
            pop(o, D3DSIO_ADD, psh_dst(sum_dst, mask, 0, o), s, 2);
        } else {
            /* sum = A*B + C*D straight into sum_dst. */
            int ab0 = in_is_zero(A) || in_is_zero(B), cd0 = in_is_zero(C) || in_is_zero(D);
            if (cd0 && ab0) {
                s[0] = psh_src(o, 0, alpha, 0);
                pop(o, D3DSIO_MOV, psh_dst(sum_dst, mask, 0, o), s, 1);
            } else if (cd0 || ab0) {
                uint32_t X = cd0 ? A : C, Y = cd0 ? B : D;
                if (in_is_one(Y)) { s[0] = psh_src(o, X, alpha, 0); pop(o, D3DSIO_MOV, psh_dst(sum_dst, mask, shift, o), s, 1); }
                else if (in_is_one(X)) { s[0] = psh_src(o, Y, alpha, 0); pop(o, D3DSIO_MOV, psh_dst(sum_dst, mask, shift, o), s, 1); }
                else { s[0] = psh_src(o, X, alpha, 0); s[1] = psh_src(o, Y, alpha, 0); pop(o, D3DSIO_MUL, psh_dst(sum_dst, mask, shift, o), s, 2); }
            } else if (in_is_one(B) && in_is_one(D)) {
                s[0] = psh_src(o, A, alpha, 0); s[1] = psh_src(o, C, alpha, 0);
                pop(o, D3DSIO_ADD, psh_dst(sum_dst, mask, shift, o), s, 2);
            } else {
                /* mul sum, A, B ; mad sum, C, D, sum -- sum must not be C or D
                 * (else swap the halves); both -> not translated. */
                uint32_t P = A, Q = B, R = C, S = D;
                if (reads_reg(C, sum_dst) || reads_reg(D, sum_dst)) { P = C; Q = D; R = A; S = B; }
                if (reads_reg(R, sum_dst) || reads_reg(S, sum_dst)) {
                    o->bad = 1; snprintf(o->why, sizeof o->why, "sum aliases inputs"); return;
                }
                s[0] = psh_src(o, P, alpha, 0); s[1] = psh_src(o, Q, alpha, 0);
                if (!alpha && o->stage == o->fuse_stage && sum_dst == XR_R0 && !shift) {
                    /* R0 + V1 is all the final combiner reads of R0's colour:
                     * start the sum from V1 instead of adding it later. */
                    s[2] = psh_src(o, 0xC0 | XR_V1, 0, 0);
                    pop(o, D3DSIO_MAD, psh_dst(sum_dst, mask, 0, o), s, 3);
                    o->fused = 1;
                } else
                pop(o, D3DSIO_MUL, psh_dst(sum_dst, mask, 0, o), s, 2);
                s[0] = psh_src(o, R, alpha, 0); s[1] = psh_src(o, S, alpha, 0);
                s[2] = psh_src(o, 0xC0 | sum_dst, alpha, 0);
                pop(o, D3DSIO_MAD, psh_dst(sum_dst, mask, shift, o), s, 3);
            }
        }
    }
    if (bias) {
        uint32_t regs[3] = { ab_dst, cd_dst, sum_dst };
        int k;
        for (k = 0; k < 3; k++) {
            if (!regs[k]) continue;
            s[0] = psh_src(o, 0x80 | regs[k], alpha, 0);   /* reg_bias (unsigned read) */
            pop(o, D3DSIO_MOV, psh_dst(regs[k], mask, shift == 1 ? 1 : 0, o), s, 1);
        }
    }
}

/* Is Xbox register `reg` read by stage `from` or later (either portion), or
 * by the final combiner? */
static int psh_reads_from(uint32_t reg, int from, int nstages)
{
    int st, k;
    for (st = from; st < nstages; st++) {
        uint32_t w[2] = { XRS(34 + st), XRS(0 + st) };
        int p;
        for (p = 0; p < 2; p++)
            for (k = 0; k < 4; k++)
                if (((w[p] >> (8 * k)) & 0xF) == reg) return 1;
    }
    for (k = 0; k < 4; k++) {
        if (((XRS(8) >> (8 * k)) & 0xF) == reg) return 1;
        if (k > 0 && ((XRS(9) >> (8 * k)) & 0xF) == reg) return 1;
    }
    if (reg == XR_V1 || reg == XR_R0) {           /* V1R0_SUM reads both */
        for (k = 0; k < 4; k++) {
            if (((XRS(8) >> (8 * k)) & 0xF) == XR_V1R0) return 1;
            if (k > 0 && ((XRS(9) >> (8 * k)) & 0xF) == XR_V1R0) return 1;
        }
    }
    return 0;
}

/* Does stage st write Xbox register reg (any output of either portion)? */
static int psh_stage_writes(int st, uint32_t reg)
{
    uint32_t w[2] = { XRS(45 + st), XRS(26 + st) };
    int p;
    for (p = 0; p < 2; p++)
        if ((w[p] & 0xF) == reg || ((w[p] >> 4) & 0xF) == reg || ((w[p] >> 8) & 0xF) == reg) return 1;
    return 0;
}

static int psh_translate(psh_out *o)
{
    uint32_t count = XRS(53), modes = XRS(136), abcd = XRS(8), efg = XRS(9);
    int nstages = (int)(count & 0xFF), st, t;
    memset(o, 0, sizeof *o);
    o->unique_c0 = (count & 0x1000) != 0;
    o->unique_c1 = (count & 0x10000) != 0;
    if (nstages < 1 || nstages > 8) { o->bad = 1; snprintf(o->why, sizeof o->why, "stages %d", nstages); return 0; }

    pemit(o, D3DPS_VERSION(1, 1));
    for (t = 0; t < 4; t++) {
        uint32_t m = (modes >> (5 * t)) & 0x1F;
        DWORD dst = 0x80000000u | (PS_TEX << 28) | (0xFu << 16) | (DWORD)t;
        /* Source for the address modes that read an earlier stage: t0 for
         * stage 1, PSInputTexture picks for stages 2 and 3; PSDotMapping
         * other than 0..1 means a signed [-1,1] read (_bx2). */
        uint32_t in_t = t <= 1 ? 0 : t == 2 ? ((XRS(56) >> 16) & 1) : ((XRS(56) >> 20) & 3);
        uint32_t dmap = t >= 1 ? (XRS(55) >> (4 * (t - 1))) & 7 : 0;
        DWORD src = 0x80000000u | (PS_TEX << 28) | (0xE4u << 16) | (dmap ? D3DSPSM_SIGN : 0) | in_t;
        DWORD srcp = 0x80000000u | (PS_TEX << 28) | (0xE4u << 16) | in_t;
        if (m == 0) continue;
        if (m >= 1 && m <= 3) pop(o, D3DSIO_TEX, dst, NULL, 0);
        else if (m == 4) pop(o, D3DSIO_TEXCOORD, dst, NULL, 0);
        else if (m == 6) pop(o, D3DSIO_TEXBEM, dst, &srcp, 1);
        else if (m == 7) pop(o, D3DSIO_TEXBEML, dst, &srcp, 1);
        else if (m == 0x0F) pop(o, D3DSIO_TEXREG2AR, dst, &srcp, 1);
        else if (m == 0x10) pop(o, D3DSIO_TEXREG2GB, dst, &srcp, 1);
        else if (m == 0x11) {
            /* DOTPRODUCT pads the matrix op the next stage(s) finish. */
            uint32_t n1 = t < 3 ? (modes >> (5 * (t + 1))) & 0x1F : 0;
            uint32_t n2 = t < 2 ? (modes >> (5 * (t + 2))) & 0x1F : 0;
            if (n1 == 0x09) pop(o, D3DSIO_TEXM3x2PAD, dst, &src, 1);
            else if (n1 == 0x11 && (n2 == 0x0D || n2 == 0x0E || n2 == 0x0C || n2 == 0x0B))
                pop(o, D3DSIO_TEXM3x3PAD, dst, &src, 1);
            else if (t >= 1 && ((modes >> (5 * (t - 1))) & 0x1F) == 0x11 &&
                     (n1 == 0x0D || n1 == 0x0E || n1 == 0x0C || n1 == 0x0B))
                pop(o, D3DSIO_TEXM3x3PAD, dst, &src, 1);
            else { o->bad = 1; snprintf(o->why, sizeof o->why, "dotproduct chain on t%d", t); return 0; }
        }
        else if (m == 0x09) pop(o, D3DSIO_TEXM3x2TEX, dst, &src, 1);
        else if (m == 0x0D || m == 0x0E) pop(o, D3DSIO_TEXM3x3TEX, dst, &src, 1);
        else { o->bad = 1; snprintf(o->why, sizeof o->why, "texture mode %u on t%d", m, t); return 0; }
    }
    o->vmap[0] = o->vmap[1] = -1;
    o->fuse_stage = -1;
    {
        /* Fold the final combiner's V1R0_SUM into the last stage when that
         * stage's RGB half is a plain two-product sum into R0 and nothing in
         * the final combiner reads R0's colour directly (G may read R0.a). */
        uint32_t fin[4] = { (abcd >> 24) & 0xFF, (abcd >> 16) & 0xFF, (abcd >> 8) & 0xFF, abcd & 0xFF };
        uint32_t out = XRS(45 + nstages - 1), fl = efg & 0xFF;
        int k, sum = 0, r0rgb = 0;
        for (k = 0; k < 4; k++) {
            if ((fin[k] & 0xF) == XR_V1R0) sum = 1;
            if ((fin[k] & 0xF) == XR_R0 && !(fin[k] & 0x10)) r0rgb = 1;
            if ((fin[k] & 0xF) == XR_EF) r0rgb = 1;
        }
        if (sum && !r0rgb && !(fl & 0x60) && ((out >> 8) & 0xF) == XR_R0 && !(out & 0xFF) && !((out >> 12) & 0x3F))
            o->fuse_stage = nstages - 1;
    }
    for (st = 0; st < nstages && !o->bad; st++) {
        int a0, a1, rgb_last = -1, n1, v;
        o->stage = st;
        /* A stage that writes V0/V1 as a scratch register: the PC cannot
         * write v#, so the value goes to a texture register nothing reads
         * any more (its sample already consumed). */
        for (v = 0; v < 2; v++) {
            int t;
            if (o->vmap[v] >= 0 || !psh_stage_writes(st, XR_V0 + v)) continue;
            /* Free from the next stage on; within this stage only the RGB
             * half may read it (it reads its inputs before the write, and
             * psh_portion refuses the one aliasing order that would not). */
            for (t = 3; t >= 0; t--) {
                uint32_t ain = XRS(0 + st);
                int k, alpha_reads = 0;
                for (k = 0; k < 4; k++) if (((ain >> (8 * k)) & 0xF) == (uint32_t)(XR_T0 + t)) alpha_reads = 1;
                if (!alpha_reads && !psh_reads_from(XR_T0 + t, st + 1, nstages)) break;
            }
            if (t < 0) { o->bad = 1; snprintf(o->why, sizeof o->why, "no free register for V%d", v); return 0; }
            o->vmap[v] = (int8_t)t;
        }
        a0 = o->arith;
        psh_portion(o, XRS(34 + st), XRS(45 + st), 0);
        a1 = o->arith;
        if (a1 > a0) rgb_last = o->last_op;
        n1 = o->n;
        psh_portion(o, XRS(0 + st), XRS(26 + st), 1);
        /* ps.1.x runs an alpha-only op alongside an RGB-only op as one
         * instruction ("+" co-issue): pair the alpha half's single op with
         * the RGB half's last one when that one writes .rgb only. */
        if (o->arith - a1 == 1 && rgb_last >= 0 && o->tok[n1] != D3DSIO_DP3 &&
            ((o->tok[rgb_last + 1] >> 16) & 0xF) == 0x7 && ((o->tok[n1 + 1] >> 16) & 0xF) == 0x8) {
            o->tok[n1] |= D3DSI_COISSUE;
            o->arith--;
        }
    }
    if (o->bad) return 0;

    /* Final combiner: rgb = A*B + (1-A)*C + D, alpha = G. Result in r0. */
    if (abcd || efg) {
        uint32_t A = (abcd >> 24) & 0xFF, B = (abcd >> 16) & 0xFF, C = (abcd >> 8) & 0xFF, D = abcd & 0xFF;
        uint32_t G = (efg >> 8) & 0xFF;
        DWORD s[3];
        DWORD r0rgb = 0x80000000u | (PS_TEMP << 28) | (0x7u << 16);
        DWORD r0a = 0x80000000u | (PS_TEMP << 28) | (0x8u << 16);
        o->stage = 0;
        {
            /* V1R0_SUM (V1 + R0, flags may complement either) and EF_PROD
             * (E*F) are computed into r1 before the final op; only one of
             * them, and only when the final op does not read R1 itself. */
            uint32_t E = (efg >> 24) & 0xFF, F = (efg >> 16) & 0xFF, fl = efg & 0xFF;
            uint32_t ins[5] = { A, B, C, D, G };
            int use_sum = 0, use_ef = 0, use_r1 = 0, k;
            for (k = 0; k < 5; k++) {
                if ((ins[k] & 0xF) == XR_V1R0) use_sum = 1;
                if ((ins[k] & 0xF) == XR_EF) use_ef = 1;
                if ((ins[k] & 0xF) == XR_R1) use_r1 = 1;
            }
            if ((use_sum && use_ef) || ((use_sum || use_ef) && use_r1) ||
                (use_ef && ((E & 0xF) >= XR_V1R0 || (F & 0xF) >= XR_V1R0))) {
                o->bad = 1; snprintf(o->why, sizeof o->why, "final needs both V1R0 and EF"); return 0;
            }
            if (use_ef) {
                s[0] = psh_src(o, E, 0, 1); s[1] = psh_src(o, F, 0, 1);
                pop(o, D3DSIO_MUL, 0x80000000u | (PS_TEMP << 28) | (0xFu << 16) | 1u, s, 2);
            }
            if (use_sum && o->fused) {
                for (k = 0; k < 5; k++)
                    if ((ins[k] & 0xF) == XR_V1R0) ins[k] = (ins[k] & 0xF0) | XR_R0;
                A = ins[0]; B = ins[1]; C = ins[2]; D = ins[3]; G = ins[4];
                use_sum = 0;
            }
            if (use_sum) {
                s[0] = psh_src(o, XR_V1 | ((fl & 0x40) ? 0x20 : 0), 0, 1);
                s[1] = psh_src(o, XR_R0 | ((fl & 0x20) ? 0x20 : 0), 0, 1);
                pop(o, D3DSIO_ADD, 0x80000000u | (PS_TEMP << 28) | (0xFu << 16) | D3DSPDM_SATURATE | 1u, s, 2);
            }
            for (k = 0; k < 5; k++)
                if ((ins[k] & 0xF) == XR_V1R0 || (ins[k] & 0xF) == XR_EF)
                    ins[k] = (ins[k] & 0xF0) | XR_R1;
            A = ins[0]; B = ins[1]; C = ins[2]; D = ins[3]; G = ins[4];
        }
        if ((A & 0x1F) == (0x10 | XR_FOG) && (C & 0x1F) == XR_FOG && in_is_zero(D)) {
            /* fog blend: output B (PC fog is applied after the shader) */
            if (!((B & 0xF) == XR_R0 && (B & 0xE0) == 0 && !(B & 0x10))) {
                s[0] = psh_src(o, B, 0, 1);
                pop(o, D3DSIO_MOV, r0rgb, s, 1);
            }
        } else if (in_is_zero(A) || in_is_zero(B)) {
            if (in_is_zero(A)) {
                s[0] = psh_src(o, C, 0, 1); s[1] = psh_src(o, D, 0, 1);
                pop(o, in_is_zero(D) ? D3DSIO_MOV : D3DSIO_ADD, r0rgb, s, in_is_zero(D) ? 1 : 2);
            } else if ((A & 0xE0) == 0) {
                /* B zero: rgb = (1-A)*C + D. Seen as A = FOG.a, C = R0,
                 * D = 1-R0 (shader 00FE6100). */
                s[0] = psh_src(o, A | 0x20, 0, 1); s[1] = psh_src(o, C, 0, 1);
                if (in_is_zero(D)) {
                    pop(o, D3DSIO_MUL, r0rgb, s, 2);
                } else {                    /* one op: C and D may both read r0 */
                    s[2] = psh_src(o, D, 0, 1);
                    pop(o, D3DSIO_MAD, r0rgb, s, 3);
                }
            } else {
                o->bad = 1; snprintf(o->why, sizeof o->why, "final B zero, A mapped"); return 0;
            }
        } else {
            s[0] = psh_src(o, A, 0, 1); s[1] = psh_src(o, B, 0, 1); s[2] = psh_src(o, C, 0, 1);
            pop(o, D3DSIO_LRP, r0rgb, s, 3);
            if (!in_is_zero(D)) {
                s[0] = 0x80000000u | (PS_TEMP << 28) | (0xE4u << 16); s[1] = psh_src(o, D, 0, 1);
                pop(o, D3DSIO_ADD, r0rgb, s, 2);
            }
        }
        if (!((G & 0xF) == XR_R0 && (G & 0x10) && (G & 0xE0) == 0)) {
            s[0] = psh_src(o, G | 0x10, 1, 1);
            pop(o, D3DSIO_MOV, r0a, s, 1);
        }
    }
    if (getenv("CW_PSH_ALPHA1")) {
        /* experiment: force output alpha to 1 (1 - zero constant) */
        DWORD s1 = psh_src(o, 0x20, 1, 1);
        pop(o, D3DSIO_MOV, 0x80000000u | (PS_TEMP << 28) | (0x8u << 16), &s1, 1);
    }
    pemit(o, D3DPS_END());
    if (o->arith > 8) { o->bad = 1; snprintf(o->why, sizeof o->why, "%d arithmetic instructions", o->arith); }
    return !o->bad;
}

static void psh_key(uint32_t key[64])
{
    int i;
    for (i = 0; i < 57; i++)
        key[i] = (i >= 10 && i <= 25) || i == 43 || i == 44 ? 0 : XRS(i);
    key[57] = XRS(136);
    for (i = 58; i < 64; i++) key[i] = 0;
}

static void psh_load_constants(const psh_rec *r)
{
    float v[8][4];
    int i;
    for (i = 0; i < r->nconst; i++) {
        int rs = r->const_src[i];
        uint32_t c = rs < 0 ? 0 : rs == 1000 ? XRS(XRS_FOGCOLOR) : XRS(rs);
        v[i][0] = ((c >> 16) & 0xFF) / 255.0f; v[i][1] = ((c >> 8) & 0xFF) / 255.0f;
        v[i][2] = (c & 0xFF) / 255.0f;         v[i][3] = rs == 1000 ? 1.0f : ((c >> 24) & 0xFF) / 255.0f;
        if (rs < 0) v[i][0] = v[i][1] = v[i][2] = v[i][3] = 0.0f;
    }
    if (r->nconst)
        IDirect3DDevice8_SetPixelShaderConstant(s_dev, 0, v, (DWORD)r->nconst);
}

/* Solid-colour pixel shader for the debug highlight modes. */
static void psh_solid(const float c[4])
{
    static DWORD ps;
    static int tried;
    if (!tried) {
        static const DWORD solid[] = { 0xFFFF0101u, D3DSIO_MOV, 0x800F0000u, 0xA0E40000u, 0x0000FFFFu };
        tried = 1;
        IDirect3DDevice8_CreatePixelShader(s_dev, solid, &ps);
    }
    if (ps) {
        IDirect3DDevice8_SetPixelShader(s_dev, ps);
        IDirect3DDevice8_SetPixelShaderConstant(s_dev, 0, c, 1);
    }
}

/* Bind a PC pixel shader for the current Xbox one; 0 = use stage states. */
static int psh_bind(void)
{
    uint32_t key[64];
    psh_rec *r = NULL;
    int i;
    psh_key(key);
    for (i = 0; i < s_npsh; i++)
        if (!memcmp(s_psh[i]->key, key, sizeof key)) { r = s_psh[i]; break; }
    if (!r && s_npsh < PSH_MAX) {
        psh_out *o = (psh_out *)calloc(1, sizeof *o);
        r = (psh_rec *)calloc(1, sizeof *r);
        if (!o || !r) { free(o); free(r); return 0; }
        memcpy(r->key, key, sizeof key);
        if (psh_translate(o)) {
            if (getenv("CW_PSH_LOG")) {
                int k;
                fprintf(stderr, "[PSH]     tokens:");
                for (k = 0; k < o->n; k++) fprintf(stderr, " %08lX", (unsigned long)o->tok[k]);
                fputc(10, stderr);
            }
            HRESULT hr = IDirect3DDevice8_CreatePixelShader(s_dev, o->tok, &r->pc_ps);
            if (SUCCEEDED(hr)) {
                r->ok = 1;
                r->nconst = o->nconst;
                memcpy(r->const_src, o->const_src, sizeof r->const_src);
            } else {
                snprintf(o->why, sizeof o->why, "CreatePixelShader hr=0x%08lX", (unsigned long)hr);
            }
        }
        if (!r->ok || getenv("CW_PSH_LOG")) {
            int k;
            fprintf(stderr, "[PSH] shader %d: %s (%d arith) %s | count=%X modes=%X fin=%08X/%08X xbox=%08X tex0=%08X\n",
                    s_npsh, r->ok ? "ok" : "FAILED", o->arith, o->why, XRS(53), XRS(136), XRS(8), XRS(9),
                    s_pshader, s_tex[0]);
            fprintf(stderr, "[PSH]     blend %u %X->%X zwrite %u  texfmt %X %X %X %X  dotmap=%X intex=%X\n",
                    XRS(XRS_ALPHABLENDENABLE), XRS(XRS_SRCBLEND), XRS(XRS_DESTBLEND), XRS(XRS_ZWRITEENABLE),
                    s_tex[0] ? (MEM32(s_tex[0] + 0xC) >> 8) & 0xFF : 0xFFu, s_tex[1] ? (MEM32(s_tex[1] + 0xC) >> 8) & 0xFF : 0xFFu,
                    s_tex[2] ? (MEM32(s_tex[2] + 0xC) >> 8) & 0xFF : 0xFFu, s_tex[3] ? (MEM32(s_tex[3] + 0xC) >> 8) & 0xFF : 0xFFu,
                    XRS(55), XRS(56));
            for (k = 0; k < (int)(XRS(53) & 0xF) && k < 8; k++)
                fprintf(stderr, "[PSH]     st%d rgb %08X->%08X  a %08X->%08X  c0=%08X c1=%08X\n", k,
                        XRS(34 + k), XRS(45 + k), XRS(k), XRS(26 + k), XRS(10 + k), XRS(18 + k));
        }
        free(o);
        s_psh[s_npsh++] = r;
    }
    if (!r || !r->ok) {
        s_psh_fail_draws++;
        return 0;
    }
    {
        /* CW_PSH_SKIP=i,j,...: leave those shader indices on the stage-state
         * fallback (bisecting a wrong translation). */
        static char list[128] = "-";
        int idx;
        if (list[0] == '-') {
            const char *e = getenv("CW_PSH_SKIP");
            snprintf(list, sizeof list, ",%s,", e ? e : "");
        }
        for (idx = 0; idx < s_npsh; idx++)
            if (s_psh[idx] == r) {
                char key[16];
                snprintf(key, sizeof key, ",%d,", idx);
                if (strstr(list, key)) return 0;
                break;
            }
    }
    {
        /* CW_PSH_HILITE=<n>: draw translated shader n in solid magenta, to
         * see on screen which objects it covers. */
        static int hl = -2;
        static DWORD hl_ps;
        if (hl == -2) {
            const char *e = getenv("CW_PSH_HILITE");
            hl = e ? (strcmp(e, "all") ? atoi(e) : 999) : -1;
            if (hl >= 0) {
                static const DWORD solid[] = {
                    0xFFFF0101u,                          /* ps.1.1 */
                    D3DSIO_MOV, 0x800F0000u, 0xA0E40000u, /* mov r0, c0 */
                    0x0000FFFFu };
                IDirect3DDevice8_CreatePixelShader(s_dev, solid, &hl_ps);
            }
        }
        if (hl >= 0 && hl_ps) {
            /* 999 ("all"): every shader in its own colour, index i ->
             * palette[i % 12]; otherwise only shader hl, in magenta. */
            static const float pal[12][4] = {
                {1,0,0,1},{0,1,0,1},{0,0,1,1},{1,1,0,1},{1,0,1,1},{0,1,1,1},
                {1,1,1,1},{1,.5f,0,1},{.5f,0,1,1},{0,.5f,0,1},{.5f,.5f,.5f,1},{.5f,.25f,0,1} };
            int idx = -1, k;
            for (k = 0; k < s_npsh; k++) if (s_psh[k] == r) idx = k;
            if (hl == 999 || idx == hl) {
                IDirect3DDevice8_SetPixelShader(s_dev, hl_ps);
                IDirect3DDevice8_SetPixelShaderConstant(s_dev, 0, hl == 999 ? pal[idx % 12] : pal[4], 1);
                return 1;
            }
        }
    }
    IDirect3DDevice8_SetPixelShader(s_dev, r->pc_ps);
    psh_load_constants(r);
    return 1;
}
