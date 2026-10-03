/*
 * hle_d3d8_vsh.inc -- Xbox (NV2A) vertex programs on the PC's vs.1.1.
 *
 * Included by hle_d3d8.c inside its HLE_D3D8_AVAILABLE section; uses that
 * file's device and captured state.
 *
 * D3DDevice_CreateVertexShader hands the Xbox D3D a declaration (the same
 * token scheme as the PC's, with Xbox data types) and a program: a header
 * dword (type 'x', version 0x20, instruction count) followed by 128-bit NV2A
 * instructions. Each instruction pairs a MAC op (mov mul add mad dp3 dph dp4
 * dst min max slt sge arl) with an ILU op (mov rcp rcc rsq exp log lit); the
 * field layout below is the one the Xbox emulators (Cxbx-Reloaded, xemu)
 * decode. Each program is translated once into D3D8 vs.1.1 tokens and
 * created on the PC device; the PC runtime validates the result, so a
 * decoding mistake shows up as a rejected shader rather than wrong pixels.
 *
 * Differences handled:
 *  - The XDK appends a screen-space tail to every program: the body writes
 *    clip-space position to R12 (which is oPos), then "mul oPos.xyz, R12,
 *    c[58]; rcc R1.x, R12.w; mad oPos.xyz, R12, R1.x, c[59]" scales it by the
 *    viewport. Writes to the real oPos are dropped when the program writes
 *    R12, and "mov oPos, R12" ends the PC program instead.
 *  - R12 has no PC register: it is given a temp the program does not use.
 *  - DPH has no vs.1.1 opcode: dp3 into a spare temp, then add B.w.
 *  - A MAC and an ILU op in one slot read their sources before either
 *    writes; when one writes what the other reads, the ILU result goes
 *    through a spare temp.
 *  - oB0/oB1 (back-face colours) have no PC output and are dropped.
 *  - Constants: the Xbox D3D mirrors every constant it loads into a table at
 *    0x002E3430 (192 x float4, hardware slot order); that table is uploaded
 *    before each draw. Slot numbers are used unchanged (c0..c191).
 *  - Vertex data: Xbox types the PC lacks (NORMPACKED3, NORMSHORT, PBYTE,
 *    SHORT1/3) are converted on the CPU. Every input the program reads is
 *    fed as FLOAT4 from one user-pointer stream.
 */

#define VSH_SHADOW_CONSTANTS 0x002E3430u
#define VSH_MAX_RECORDS      512

typedef struct {
    uint32_t handle;            /* Xbox handle (pointer | 1)               */
    uint32_t decl[96];          /* Xbox declaration tokens, END included  */
    int      ndecl;
    uint32_t prog[136 * 4];     /* instructions, 4 dwords each            */
    int      ninsn;
    int      translated;        /* 0 not yet, 1 ok, -1 failed             */
    DWORD    pc_vs;             /* PC shader handle                       */
    /* Variant with clip z clamped to the far plane, for stencil shadow
     * volumes (see draw_vs): translated 0/1/-1, handle, the constant whose
     * w holds the clamp factor. */
    int      clamp_state, clamp_c;
    DWORD    pc_vs_clamp;
    uint16_t inputs;            /* v registers the program reads          */
    /* parsed declaration: per v register */
    int8_t   in_stream[16];     /* -1 = not declared                      */
    uint16_t in_offset[16];
    uint8_t  in_type[16];
    int      logged;
    int      hor_c;             /* PC constant for the widescreen factor, -1 none */
    int      writes_fog;        /* the program writes oFog */
    /* Constants below slot 96 (the D3D's own: c58/c59 viewport scale and
     * offset) when the PC has only 96: each gets a PC constant the program
     * never reads, loaded from the shadow table at every draw. */
    int      nlow, low_x[4], low_pc[4];
} vsh_rec;

static vsh_rec *s_vsh[VSH_MAX_RECORDS];
static int      s_vs_consts = 96;          /* PC device's MaxVertexShaderConst */
static int      s_nvsh;
static unsigned s_vsh_fail_draws, s_vsh_draws;

static int vsh_log_on(void)
{
    static int on = -1;
    if (on < 0) { const char *e = getenv("CW_VSH_LOG"); on = e && *e == '1'; }
    return on;
}

static vsh_rec *vsh_find(uint32_t handle)
{
    int i;
    for (i = 0; i < s_nvsh; i++)
        if (s_vsh[i]->handle == handle)
            return s_vsh[i];
    return NULL;
}

/* ---- declaration --------------------------------------------------------- */

static int xtype_size(uint32_t t)
{
    switch (t) {
    case 0x12: return 4;  case 0x22: return 8;  case 0x32: return 12; case 0x42: return 16;
    case 0x40: return 4;  case 0x25: return 4;  case 0x45: return 8;
    case 0x11: return 2;  case 0x21: return 4;  case 0x31: return 6;  case 0x41: return 8;
    case 0x16: return 4;  case 0x15: return 2;  case 0x35: return 6;
    case 0x14: return 1;  case 0x24: return 2;  case 0x34: return 3;  case 0x44: return 4;
    case 0x72: return 12; case 0x02: return 0;
    default:   return -1;
    }
}

static void vsh_parse_decl(vsh_rec *r)
{
    int i, stream = 0;
    uint32_t offset = 0;
    memset(r->in_stream, -1, sizeof r->in_stream);
    for (i = 0; i < r->ndecl; i++) {
        uint32_t t = r->decl[i];
        uint32_t kind = t >> 29;
        if (t == 0xFFFFFFFFu)
            break;
        if (kind == 1) {                              /* STREAM */
            stream = (int)(t & 0xF);
            offset = 0;
        } else if (kind == 2) {                       /* STREAMDATA */
            if ((t & 0x18000000u) == 0x18000000u)     /* SKIPBYTES */
                offset += (t >> 16) & 0xFFu;
            else if (t & 0x10000000u)                 /* SKIP dwords */
                offset += 4 * ((t >> 16) & 0xFu);
            else {
                uint32_t reg = t & 0x1F, type = (t >> 16) & 0xFF;
                int sz = xtype_size(type);
                if (reg < 16) {
                    r->in_stream[reg] = (int8_t)stream;
                    r->in_offset[reg] = (uint16_t)offset;
                    r->in_type[reg] = (uint8_t)type;
                }
                if (sz > 0) offset += (uint32_t)sz;
            }
        } else if (kind == 4) {                       /* CONSTMEM: data follows */
            i += 4 * (int)((t >> 25) & 0xF);
        } else if (kind == 5) {                       /* EXT: data follows */
            i += (int)((t >> 24) & 0x1F);
        }
    }
}

/* One attribute to float4. */
static void vsh_fetch(uint32_t type, const uint8_t *p, float o[4])
{
    o[0] = 0.0f; o[1] = 0.0f; o[2] = 0.0f; o[3] = 1.0f;
    switch (type) {
    case 0x12: memcpy(o, p, 4);  break;
    case 0x22: memcpy(o, p, 8);  break;
    case 0x32: memcpy(o, p, 12); break;
    case 0x42: memcpy(o, p, 16); break;
    case 0x72: memcpy(o, p, 8); memcpy(&o[3], p + 8, 4); break;   /* FLOAT2H: x, y, w */
    case 0x40:                                                     /* D3DCOLOR: BGRA bytes */
        o[0] = p[2] / 255.0f; o[1] = p[1] / 255.0f; o[2] = p[0] / 255.0f; o[3] = p[3] / 255.0f;
        break;
    case 0x15: case 0x25: case 0x35: case 0x45: {                  /* SHORT1-4 */
        int n = (int)(type >> 4), k;
        for (k = 0; k < n; k++) { int16_t v; memcpy(&v, p + 2 * k, 2); o[k] = (float)v; }
        break;
    }
    case 0x11: case 0x21: case 0x31: case 0x41: {                  /* NORMSHORT1-4 */
        int n = (int)(type >> 4), k;
        for (k = 0; k < n; k++) { int16_t v; memcpy(&v, p + 2 * k, 2); o[k] = v / 32767.0f; }
        break;
    }
    case 0x14: case 0x24: case 0x34: case 0x44: {                  /* PBYTE1-4 */
        int n = (int)(type >> 4), k;
        for (k = 0; k < n; k++) o[k] = p[k] / 255.0f;
        break;
    }
    case 0x16: {                                                   /* NORMPACKED3 11:11:10 */
        uint32_t v; int32_t x, y, z;
        memcpy(&v, p, 4);
        x = (int32_t)(v << 21) >> 21;
        y = (int32_t)(v << 10) >> 21;
        z = (int32_t)v >> 22;
        o[0] = x / 1023.0f; o[1] = y / 1023.0f; o[2] = z / 511.0f;
        break;
    }
    default: break;
    }
}

/* ---- program translation --------------------------------------------------- */

static uint32_t vfield(const uint32_t *in, int sub, int start, int len)
{
    return (in[sub] >> start) & ((1u << len) - 1u);
}

#define F_ILU(i)      vfield(i, 1, 25, 3)
#define F_MAC(i)      vfield(i, 1, 21, 4)
#define F_CONST(i)    vfield(i, 1, 13, 8)
#define F_V(i)        vfield(i, 1,  9, 4)
#define F_A_NEG(i)    vfield(i, 1,  8, 1)
#define F_A_SWZ(i)    vfield(i, 1,  0, 8)     /* x:7-6 y:5-4 z:3-2 w:1-0 */
#define F_A_R(i)      vfield(i, 2, 28, 4)
#define F_A_MUX(i)    vfield(i, 2, 26, 2)
#define F_B_NEG(i)    vfield(i, 2, 25, 1)
#define F_B_SWZ(i)    vfield(i, 2, 17, 8)
#define F_B_R(i)      vfield(i, 2, 13, 4)
#define F_B_MUX(i)    vfield(i, 2, 11, 2)
#define F_C_NEG(i)    vfield(i, 2, 10, 1)
#define F_C_SWZ(i)    vfield(i, 2,  2, 8)
#define F_C_R(i)      ((vfield(i, 2, 0, 2) << 2) | vfield(i, 3, 30, 2))
#define F_C_MUX(i)    vfield(i, 3, 28, 2)
#define F_MAC_MASK(i) vfield(i, 3, 24, 4)
#define F_OUT_R(i)    vfield(i, 3, 20, 4)
#define F_ILU_MASK(i) vfield(i, 3, 16, 4)
#define F_O_MASK(i)   vfield(i, 3, 12, 4)
#define F_ORB(i)      vfield(i, 3, 11, 1)     /* 1 = output register, 0 = constant */
#define F_O_ADDR(i)   vfield(i, 3,  3, 8)
#define F_O_MUX(i)    vfield(i, 3,  2, 1)     /* 0 = MAC result, 1 = ILU result */
#define F_A0X(i)      vfield(i, 3,  1, 1)
#define F_FINAL(i)    vfield(i, 3,  0, 1)

enum { MUX_R = 1, MUX_V = 2, MUX_C = 3 };

typedef struct {
    int mux, reg, neg, rel;
    uint8_t swz[4];             /* component per position, 0..3 */
} vsrc;

static vsrc vsh_src(const uint32_t *in, int which)
{
    vsrc s;
    uint32_t swz;
    memset(&s, 0, sizeof s);
    if (which == 0) { s.mux = (int)F_A_MUX(in); s.reg = (int)F_A_R(in); s.neg = (int)F_A_NEG(in); swz = F_A_SWZ(in); }
    else if (which == 1) { s.mux = (int)F_B_MUX(in); s.reg = (int)F_B_R(in); s.neg = (int)F_B_NEG(in); swz = F_B_SWZ(in); }
    else { s.mux = (int)F_C_MUX(in); s.reg = (int)F_C_R(in); s.neg = (int)F_C_NEG(in); swz = F_C_SWZ(in); }
    s.swz[0] = (uint8_t)((swz >> 6) & 3); s.swz[1] = (uint8_t)((swz >> 4) & 3);
    s.swz[2] = (uint8_t)((swz >> 2) & 3); s.swz[3] = (uint8_t)(swz & 3);
    if (s.mux == MUX_V) s.reg = (int)F_V(in);
    if (s.mux == MUX_C) { s.reg = (int)F_CONST(in); s.rel = (int)F_A0X(in); }
    return s;
}

/* Which Xbox sources each op reads: bit 0 A, 1 B, 2 C. */
static int mac_srcs(int op)
{
    switch (op) {
    case 1: case 13: return 1;          /* mov, arl */
    case 3: return 5;                   /* add: A, C */
    case 4: return 7;                   /* mad */
    case 0: return 0;
    default: return 3;                  /* mul dp3 dph dp4 dst min max slt sge */
    }
}

static int input_w_is_one(const vsh_rec *r, int reg);

/* PC token assembly. */
typedef struct {
    DWORD tok[1024];
    int   n;
    int   r12;                  /* PC temp standing in for R12 */
    int   scratch[2];           /* spare temps */
    int   bad;
    char  why[96];
    const vsh_rec *rec;
    int   cur;                  /* Xbox instruction being translated */
} vsh_out;

#define PC_SPR_TEMP 0
#define PC_SPR_INPUT 1
#define PC_SPR_CONST 2
#define PC_SPR_ADDR 3
#define PC_SPR_RASTOUT 4
#define PC_SPR_ATTROUT 5
#define PC_SPR_TEXCRDOUT 6

static void emit(vsh_out *o, DWORD t) { if (o->n < 1023) o->tok[o->n++] = t; else o->bad = 1; }

static DWORD pc_dst(int type, int reg, int mask)
{
    return 0x80000000u | ((DWORD)type << 28) | ((DWORD)(mask & 0xF) << 16) | (DWORD)reg;
}

static DWORD pc_src_tok(vsh_out *o, const vsrc *s)
{
    int type, reg = s->reg;
    DWORD swz = (DWORD)s->swz[0] | ((DWORD)s->swz[1] << 2) | ((DWORD)s->swz[2] << 4) | ((DWORD)s->swz[3] << 6);
    if (s->mux == MUX_R) { type = PC_SPR_TEMP; if (reg == 12) reg = o->r12; else if (reg > 12) o->bad = 1; }
    else if (s->mux == MUX_V) type = PC_SPR_INPUT;
    else if (s->mux == MUX_C) {
        /* Xbox c0..c95 are hardware slots 96..191, and PC vs.1.1 guarantees
         * only 96 constants: slot 96+n is PC cN. Slots below 96 (the D3D's
         * reserved c-96..c-1) go above, which needs 192 PC constants. */
        type = PC_SPR_CONST;
        if (reg < 96 && s_vs_consts < 192) {
            int k;
            for (k = 0; k < o->rec->nlow && o->rec->low_x[k] != reg; k++) ;
            if (k < o->rec->nlow && !s->rel) reg = o->rec->low_pc[k];
            else o->bad = 1;
        } else {
            reg = reg >= 96 ? reg - 96 : reg + 96;
            if (reg >= 96 && s_vs_consts < 192) o->bad = 1;
        }
    }
    else { o->bad = 1; type = PC_SPR_TEMP; reg = 0; }
    return 0x80000000u | ((DWORD)type << 28) | (swz << 16) | (s->neg ? (1u << 24) : 0u)
         | (s->rel ? (1u << 13) : 0u) | (DWORD)reg;
}

static vsrc src_temp(int reg, int comp)       /* rN.cccc, reg is a PC temp */
{
    vsrc s;
    memset(&s, 0, sizeof s);
    s.mux = -1; s.reg = reg;
    s.swz[0] = s.swz[1] = s.swz[2] = s.swz[3] = (uint8_t)comp;
    return s;
}

static DWORD pc_src_any(vsh_out *o, const vsrc *s)
{
    if (s->mux == -1) {                        /* already a PC temp */
        DWORD swz = (DWORD)s->swz[0] | ((DWORD)s->swz[1] << 2) | ((DWORD)s->swz[2] << 4) | ((DWORD)s->swz[3] << 6);
        return 0x80000000u | ((DWORD)PC_SPR_TEMP << 28) | (swz << 16) | (DWORD)s->reg;
    }
    return pc_src_tok(o, s);
}

static vsrc replicate(vsrc s, int pos)
{
    uint8_t c = s.swz[pos];
    s.swz[0] = s.swz[1] = s.swz[2] = s.swz[3] = c;
    return s;
}

/* Xbox mask bit3=x..bit0=w -> PC bit0=x..bit3=w. */
static int pc_mask(uint32_t xm)
{
    return ((xm >> 3) & 1) | (((xm >> 2) & 1) << 1) | (((xm >> 1) & 1) << 2) | ((xm & 1) << 3);
}

/* Destination description. */
typedef struct { int type, reg, mask; } vdst;

/* Emit one PC op. nsrc sources in s[]. */
static void emit_op(vsh_out *o, DWORD opcode, vdst d, const vsrc *s, int nsrc)
{
    int k;
    vsrc tmp[3];
    for (k = 0; k < nsrc; k++) tmp[k] = s[k];
    /* oFog / oPts are scalar: PC wants .x; move the written component there. */
    if (d.type == PC_SPR_RASTOUT && d.reg != 0) {
        int c = (d.mask & 1) ? 0 : (d.mask & 2) ? 1 : (d.mask & 4) ? 2 : 3;
        for (k = 0; k < nsrc; k++) tmp[k].swz[0] = tmp[k].swz[c];
        d.mask = 1;
    }
    emit(o, opcode);
    emit(o, pc_dst(d.type, d.reg, d.mask));
    for (k = 0; k < nsrc; k++) emit(o, pc_src_any(o, &tmp[k]));
}

/* Map an Xbox output address to a PC output; returns 0 to drop. */
static int out_reg(uint32_t addr, vdst *d)
{
    switch (addr) {
    case 0:  d->type = PC_SPR_RASTOUT; d->reg = 0; return 1;          /* oPos */
    case 3:  d->type = PC_SPR_ATTROUT; d->reg = 0; return 1;          /* oD0 */
    case 4:  d->type = PC_SPR_ATTROUT; d->reg = 1; return 1;          /* oD1 */
    case 5:  d->type = PC_SPR_RASTOUT; d->reg = 1; return 1;          /* oFog */
    case 6:  d->type = PC_SPR_RASTOUT; d->reg = 2; return 1;          /* oPts */
    case 9: case 10: case 11: case 12:
             d->type = PC_SPR_TEXCRDOUT; d->reg = (int)addr - 9; return 1;
    default: return 0;                                                /* oB0, oB1 */
    }
}

/* A temp (0..11) whose value is dead across instruction i of the program:
 * not read by i, and its next access after i is a full-mask write or there
 * is none. For a program that uses all twelve temps somewhere, this finds
 * one to borrow for a single expansion step. -1 if none. */
static int vsh_dead_temp(const vsh_out *o, int i, int avoid)
{
    const vsh_rec *r = o->rec;
    int t;
    for (t = 0; t < 12; t++) {
        int j, dead = 1;
        if (avoid & (1 << t)) continue;
        for (j = i; j < r->ninsn && dead; j++) {
            const uint32_t *in = &r->prog[j * 4];
            int mac = (int)F_MAC(in), ilu = (int)F_ILU(in), w, reads = 0, wmask = 0;
            for (w = 0; w < 3; w++) {
                vsrc sw = vsh_src(in, w);
                if ((((mac_srcs(mac) >> w) & 1) || (w == 2 && ilu)) && sw.mux == MUX_R && sw.reg == t) reads = 1;
            }
            if (reads) { dead = 0; break; }
            if (j == i) continue;
            if (mac && mac != 13 && F_MAC_MASK(in) && (int)F_OUT_R(in) == t) wmask |= (int)F_MAC_MASK(in);
            if (ilu && F_ILU_MASK(in) && (mac ? 1 : (int)F_OUT_R(in)) == t) wmask |= (int)F_ILU_MASK(in);
            if (wmask == 0xF) break;              /* overwritten whole: dead */
            if (wmask) { dead = 0; break; }       /* partial write: keep it simple */
            if (F_FINAL(in)) break;
        }
        if (dead) return t;
    }
    return -1;
}

/* MAC op body into destination d (a PC register). */
static void emit_mac(vsh_out *o, int op, vdst d, const vsrc *A, const vsrc *B, const vsrc *C)
{
    vsrc s[3];
    switch (op) {
    case 1:  s[0] = *A; emit_op(o, D3DSIO_MOV, d, s, 1); break;
    case 2:  s[0] = *A; s[1] = *B; emit_op(o, D3DSIO_MUL, d, s, 2); break;
    case 3:  s[0] = *A; s[1] = *C; emit_op(o, D3DSIO_ADD, d, s, 2); break;
    case 4:  s[0] = *A; s[1] = *B; s[2] = *C; emit_op(o, D3DSIO_MAD, d, s, 3); break;
    case 5:  s[0] = *A; s[1] = *B; emit_op(o, D3DSIO_DP3, d, s, 2); break;
    case 7:  s[0] = *A; s[1] = *B; emit_op(o, D3DSIO_DP4, d, s, 2); break;
    case 8:  s[0] = *A; s[1] = *B; emit_op(o, D3DSIO_DST, d, s, 2); break;
    case 9:  s[0] = *A; s[1] = *B; emit_op(o, D3DSIO_MIN, d, s, 2); break;
    case 10: s[0] = *A; s[1] = *B; emit_op(o, D3DSIO_MAX, d, s, 2); break;
    case 11: s[0] = *A; s[1] = *B; emit_op(o, D3DSIO_SLT, d, s, 2); break;
    case 12: s[0] = *A; s[1] = *B; emit_op(o, D3DSIO_SGE, d, s, 2); break;
    case 6: {                                                         /* dph */
        /* dp3 then add B.w. Into the destination itself when it is a temp
         * and B.w survives the dp3 write; otherwise through a spare temp. */
        int comp = (d.mask & 1) ? 0 : (d.mask & 2) ? 1 : (d.mask & 4) ? 2 : 3;
        int in_place = d.type == PC_SPR_TEMP &&
            !(B->mux == MUX_R && (B->reg == 12 ? o->r12 : B->reg) == d.reg &&
              (d.mask & (1 << B->swz[3])));
        if (in_place) {
            s[0] = *A; s[1] = *B; emit_op(o, D3DSIO_DP3, d, s, 2);
            s[0] = src_temp(d.reg, comp); s[1] = replicate(*B, 3);
            emit_op(o, D3DSIO_ADD, d, s, 2);
        } else if (o->scratch[0] >= 0) {
            vdst t = { PC_SPR_TEMP, o->scratch[0], 1 };
            s[0] = *A; s[1] = *B; emit_op(o, D3DSIO_DP3, t, s, 2);
            s[0] = src_temp(o->scratch[0], 0); s[1] = replicate(*B, 3);
            emit_op(o, D3DSIO_ADD, d, s, 2);
        } else if (A->mux == MUX_V && A->swz[3] == 3 && input_w_is_one(o->rec, A->reg)) {
            s[0] = *A; s[1] = *B; emit_op(o, D3DSIO_DP4, d, s, 2);   /* A.w == 1 */
        } else {
            int avoid = (d.type == PC_SPR_TEMP ? 1 << d.reg : 0);
            int t = vsh_dead_temp(o, o->cur, avoid);
            if (t >= 0) {
                vdst tt = { PC_SPR_TEMP, t, 1 };
                s[0] = *A; s[1] = *B; emit_op(o, D3DSIO_DP3, tt, s, 2);
                s[0] = src_temp(t, 0); s[1] = replicate(*B, 3);
                emit_op(o, D3DSIO_ADD, d, s, 2);
            } else {
                o->bad = 1; snprintf(o->why, sizeof o->why, "dph needs a spare temp");
            }
        }
        break;
    }
    default: o->bad = 1; snprintf(o->why, sizeof o->why, "mac op %d", op); break;
    }
}

static void emit_ilu(vsh_out *o, int op, vdst d, const vsrc *C)
{
    vsrc s = *C;
    switch (op) {
    case 1: emit_op(o, D3DSIO_MOV, d, &s, 1); break;
    case 2: case 3: s = replicate(s, 0); emit_op(o, D3DSIO_RCP, d, &s, 1); break;
    case 4: s = replicate(s, 0); emit_op(o, D3DSIO_RSQ, d, &s, 1); break;
    case 5: s = replicate(s, 0); emit_op(o, D3DSIO_EXPP, d, &s, 1); break;
    case 6: s = replicate(s, 0); emit_op(o, D3DSIO_LOGP, d, &s, 1); break;
    case 7: emit_op(o, D3DSIO_LIT, d, &s, 1); break;
    default: o->bad = 1; snprintf(o->why, sizeof o->why, "ilu op %d", op); break;
    }
}

static int reads_temp(const vsrc *s, int xreg) { return s->mux == MUX_R && s->reg == xreg; }

/* Source reads the constant slot `slot`. */
static int reads_const(const vsrc *s, int slot) { return s->mux == MUX_C && s->reg == slot && !s->rel; }

/* An input whose declared type leaves w at its default 1.0. */
static int input_w_is_one(const vsh_rec *r, int reg)
{
    if (reg < 0 || reg > 15 || r->in_stream[reg] < 0)
        return 1;
    switch (r->in_type[reg]) {
    case 0x42: case 0x40: case 0x45: case 0x41: case 0x44: case 0x72: return 0;
    default: return 1;
    }
}

/*
 * Position. oPos and R12 are one register on the NV2A: the body writes
 * clip-space position to oPos (or R12), may read it back as R12, and the
 * screen-space pair the XDK appends -- "mul oPos.xyz, R12, c58 (+ rcc R1.x,
 * R12.w)" and "mad oPos.xyz, R12, R1.x, c59" -- turns it into window
 * coordinates. Here position lives in a spare temp (o->r12) the whole way,
 * the two viewport MAC ops are dropped, and "mov oPos, temp" ends the
 * program. A program that leaves no spare temp writes oPos directly; its
 * other reads of R12 (the w clamps some programs do) are then dropped.
 */
static int vsh_translate(vsh_rec *r, vsh_out *o, int clamp)
{
    int i, used = 0, pos_written = 0, k, nfree = 0, free_t[12], direct = 0, dropped = 0;
    memset(o, 0, sizeof *o);
    o->rec = r;
    r->inputs = 0;
    r->nlow = 0;
    if (s_vs_consts < 192) {
        /* Slots below 96 the program reads get free PC constants (see
         * vsh_rec.low_x): free = never read by the program, counting
         * everything at or above a relatively addressed read as read. */
        uint8_t usedc[96];
        int j, w2, pc;
        memset(usedc, 0, sizeof usedc);
        for (j = 0; j < r->ninsn; j++) {
            const uint32_t *in = &r->prog[j * 4];
            for (w2 = 0; w2 < 3; w2++) {
                vsrc sw = vsh_src(in, w2);
                if (sw.mux != MUX_C || sw.reg < 96) continue;
                pc = sw.reg - 96;
                if (sw.rel) memset(usedc + pc, 1, (size_t)(96 - pc));
                else usedc[pc] = 1;
            }
            if (F_FINAL(in)) break;
        }
        pc = 95;
        for (j = 0; j < r->ninsn; j++) {
            const uint32_t *in = &r->prog[j * 4];
            for (w2 = 0; w2 < 3; w2++) {
                vsrc sw = vsh_src(in, w2);
                int k;
                if (sw.mux != MUX_C || sw.reg >= 96 || sw.rel) continue;
                for (k = 0; k < r->nlow && r->low_x[k] != sw.reg; k++) ;
                if (k < r->nlow || r->nlow >= 4) continue;
                while (pc >= 0 && usedc[pc]) pc--;
                if (pc < 0) break;
                usedc[pc] = 1;
                r->low_x[r->nlow] = sw.reg; r->low_pc[r->nlow] = pc; r->nlow++;
            }
            if (F_FINAL(in)) break;
        }
    }

    for (i = 0; i < r->ninsn; i++) {
        const uint32_t *in = &r->prog[i * 4];
        int mac = (int)F_MAC(in), ilu = (int)F_ILU(in), w;
        vsrc s[3];
        for (w = 0; w < 3; w++) {
            s[w] = vsh_src(in, w);
            if (((mac_srcs(mac) >> w) & 1) || (w == 2 && ilu)) {
                if (s[w].mux == MUX_R && s[w].reg < 12) used |= 1 << s[w].reg;
                if (s[w].mux == MUX_V) r->inputs |= (uint16_t)(1u << s[w].reg);
            }
        }
        if (mac && mac != 13 && F_MAC_MASK(in) && F_OUT_R(in) < 12) used |= 1 << F_OUT_R(in);
        if (ilu && F_ILU_MASK(in)) {
            int ir = mac ? 1 : (int)F_OUT_R(in);
            if (ir < 12) used |= 1 << ir; else pos_written = 1;
        }
        if (mac && mac != 13 && F_MAC_MASK(in) && F_OUT_R(in) == 12) pos_written = 1;
        if (F_O_MASK(in) && F_ORB(in) && F_O_ADDR(in) == 0) pos_written = 1;
        if (F_O_MASK(in) && F_ORB(in) && F_O_ADDR(in) == 5) r->writes_fog = 1;   /* oFog */
        if (F_FINAL(in)) break;
    }
    for (k = 0; k < 12; k++)
        if (!(used & (1 << k))) free_t[nfree++] = k;
    {
        int got = 0;
        o->r12 = o->scratch[0] = o->scratch[1] = -1;
        if (pos_written) {
            if (got < nfree) o->r12 = free_t[got++];
            else {
                /* No temp is free for the whole program; one whose last
                 * access comes before the first position write can hold
                 * position from there on. */
                int first_pos = r->ninsn, last[12], j, t;
                for (t = 0; t < 12; t++) last[t] = -1;
                for (j = 0; j < r->ninsn; j++) {
                    const uint32_t *in = &r->prog[j * 4];
                    int mac = (int)F_MAC(in), ilu = (int)F_ILU(in), w;
                    for (w = 0; w < 3; w++) {
                        vsrc sw = vsh_src(in, w);
                        if ((((mac_srcs(mac) >> w) & 1) || (w == 2 && ilu)) && sw.mux == MUX_R && sw.reg < 12)
                            last[sw.reg] = j;
                    }
                    if (mac && mac != 13 && F_MAC_MASK(in) && F_OUT_R(in) < 12) last[F_OUT_R(in)] = j;
                    if (ilu && F_ILU_MASK(in)) { int ir = mac ? 1 : (int)F_OUT_R(in); if (ir < 12) last[ir] = j; }
                    if (first_pos == r->ninsn &&
                        ((F_O_MASK(in) && F_ORB(in) && F_O_ADDR(in) == 0) ||
                         (mac && mac != 13 && F_MAC_MASK(in) && F_OUT_R(in) == 12)))
                        first_pos = j;
                    if (F_FINAL(in)) break;
                }
                for (t = 0; t < 12; t++)
                    if (last[t] < first_pos) { o->r12 = t; break; }
                if (o->r12 < 0) direct = 1;
            }
        }
        if (got < nfree) o->scratch[0] = free_t[got++];
        if (got < nfree) o->scratch[1] = free_t[got++];
    }

    emit(o, D3DVS_VERSION(1, 1));
    {
        /* NV2A starts every program with the colour outputs at (0,0,0,1);
         * PC vs.1.1 leaves unwritten components undefined, and a program
         * that never writes oD0.w then draws with alpha 0. Set the defaults
         * first (slt x,x -> 0, sge x,x -> 1; c0 is only a convenient
         * operand) and let the program's own writes override them. */
        vsrc c; int d;
        memset(&c, 0, sizeof c);
        c.mux = MUX_C; c.reg = 96;                 /* PC c0 */
        c.swz[0] = 0; c.swz[1] = 1; c.swz[2] = 2; c.swz[3] = 3;
        for (d = 0; d < 2 && !getenv("CW_VSH_NODEFAULTS"); d++) {
            vdst rgb = { PC_SPR_ATTROUT, d, 0x7 }, a = { PC_SPR_ATTROUT, d, 0x8 };
            vsrc two[2]; two[0] = c; two[1] = c;
            emit_op(o, D3DSIO_SLT, rgb, two, 2);
            emit_op(o, D3DSIO_SGE, a, two, 2);
        }
    }
    for (i = 0; i < r->ninsn && !o->bad; i++) {
        const uint32_t *in = &r->prog[i * 4];
        int mac = (int)F_MAC(in), ilu = (int)F_ILU(in);
        uint32_t macm = F_MAC_MASK(in), ilum = F_ILU_MASK(in), om = F_O_MASK(in);
        int outr = (int)F_OUT_R(in), ilur = mac ? 1 : outr;
        vsrc A = vsh_src(in, 0), B = vsh_src(in, 1), C = vsh_src(in, 2);
        vdst od, pd;
        int have_out = 0, via_scratch = 0, out_is_pos = 0, mac_on = mac != 0;
        o->cur = i;

        memset(&od, 0, sizeof od);
        if (om) {
            if (!F_ORB(in)) {
                o->bad = 1; snprintf(o->why, sizeof o->why, "writes constant c%u", F_O_ADDR(in));
                break;
            }
            have_out = out_reg(F_O_ADDR(in), &od);
            od.mask = pc_mask(om);
            out_is_pos = have_out && F_O_ADDR(in) == 0;
        }
        /* Viewport pair: drop the MAC op's position write, keep any ILU
         * (rcc R1.x). When the same op also writes a temp (a program
         * computing screen coordinates, 0101D0B1: "mul r2.xy + oPos.xyz,
         * R12, c58"), that temp write stays. */
        if (mac && out_is_pos && F_O_MUX(in) == 0 &&
            (reads_const(&A, 58) || reads_const(&B, 58) || reads_const(&C, 58) ||
             reads_const(&A, 59) || reads_const(&B, 59) || reads_const(&C, 59))) {
            if (!macm) mac_on = 0;
            have_out = 0;
        }
        if (direct && mac_on && mac != 13 &&
            (((mac_srcs(mac) & 1) && reads_temp(&A, 12)) || ((mac_srcs(mac) & 2) && reads_temp(&B, 12)) ||
             ((mac_srcs(mac) & 4) && reads_temp(&C, 12)))) {
            mac_on = 0;
            if (F_O_MUX(in) == 0) have_out = 0;
            dropped++;
        }
        if (direct && ilu && reads_temp(&C, 12)) {
            if (F_O_MUX(in) == 1) have_out = 0;
            ilum = 0;
            dropped++;
        }
        /* Position writes go to the position temp (or oPos when direct). */
        if (out_is_pos && have_out && !direct) {
            od.type = PC_SPR_TEMP; od.reg = o->r12;
        }
        pd.type = direct ? PC_SPR_RASTOUT : PC_SPR_TEMP;
        pd.reg = direct ? 0 : o->r12;
        pd.mask = 0xF;

        if (mac_on && ilu) {
            int mac_reads_ilu = (reads_temp(&A, ilur) && (mac_srcs(mac) & 1)) ||
                                (reads_temp(&B, ilur) && (mac_srcs(mac) & 2)) ||
                                (reads_temp(&C, ilur) && (mac_srcs(mac) & 4));
            int ilu_reads_mac = mac != 13 && macm && reads_temp(&C, outr);
            via_scratch = (mac_reads_ilu && ilum) || ilu_reads_mac;
        }
        if (via_scratch && o->scratch[1] < 0) {
            o->bad = 1; snprintf(o->why, sizeof o->why, "paired op needs a spare temp");
            break;
        }
        if (ilu && via_scratch) {
            vdst t = { PC_SPR_TEMP, o->scratch[1], 0xF };
            emit_ilu(o, ilu, t, &C);
        }
        if (mac_on && mac == 13) {                    /* arl */
            vdst a0 = { PC_SPR_ADDR, 0, 1 };
            vsrc s = replicate(A, 0);
            emit_op(o, D3DSIO_MOV, a0, &s, 1);
        } else if (mac_on) {
            if (macm) {
                vdst t = { PC_SPR_TEMP, outr, pc_mask(macm) };
                if (outr == 12) { t.type = pd.type; t.reg = pd.reg; }
                emit_mac(o, mac, t, &A, &B, &C);
            }
            if (have_out && F_O_MUX(in) == 0)
                emit_mac(o, mac, od, &A, &B, &C);
        }
        if (ilu) {
            vsrc sc = src_temp(o->scratch[1] < 0 ? 0 : o->scratch[1], 0);
            sc.swz[0] = 0; sc.swz[1] = 1; sc.swz[2] = 2; sc.swz[3] = 3;
            if (ilum) {
                vdst t = { PC_SPR_TEMP, ilur, pc_mask(ilum) };
                if (ilur == 12) { t.type = pd.type; t.reg = pd.reg; }
                if (via_scratch) emit_op(o, D3DSIO_MOV, t, &sc, 1);
                else emit_ilu(o, ilu, t, &C);
            }
            if (have_out && F_O_MUX(in) == 1) {
                if (via_scratch) emit_op(o, D3DSIO_MOV, od, &sc, 1);
                else emit_ilu(o, ilu, od, &C);
            }
        }
        if (F_FINAL(in))
            break;
    }
    /* Only for CW_WIDE_RENDER=1 (clip-space widescreen): the game widens its
     * own camera otherwise, and these five instructions pushed the longest
     * programs past vs.1.1's 128-instruction limit -- the hailfire droids'
     * program then failed to build and its draws were skipped. */
    r->hor_c = getenv("CW_WIDE_RENDER") ? s_hor_c : -2;
    if (r->hor_c == -1 || clamp) {
        /* The PC device has only the 96 constants the program's own map to:
         * take the highest one this program never reads (anything at or above
         * a relatively addressed read counts as read), and none handed to a
         * low slot (low_pc). */
        uint8_t usedc[96];
        int j, w2;
        memset(usedc, 0, sizeof usedc);
        for (j = 0; j < r->nlow; j++) usedc[r->low_pc[j]] = 1;
        for (j = 0; j < r->ninsn; j++) {
            const uint32_t *in = &r->prog[j * 4];
            for (w2 = 0; w2 < 3; w2++) {
                vsrc sw = vsh_src(in, w2);
                int pc;
                if (sw.mux != MUX_C) continue;
                pc = sw.reg >= 96 ? sw.reg - 96 : sw.reg + 96;
                if (pc >= 96) continue;
                if (sw.rel) memset(usedc + pc, 1, (size_t)(96 - pc));
                else usedc[pc] = 1;
            }
            if (F_FINAL(in)) break;
        }
        for (j = 95; j >= 0 && usedc[j]; j--) ;
        if (r->hor_c == -1) r->hor_c = j;
        if (clamp) r->clamp_c = (r->hor_c >= 0) ? r->hor_c : j;
    }
    if (r->hor_c < 0) r->hor_c = -1;
    if (clamp && pos_written && !direct && r->clamp_c >= 0) {
        /* Stencil shadow volumes are extruded past the far plane and the
         * title relies on the NV2A's depth clamp to keep their far caps; the
         * PC clips them, and the count for ground in front of such a volume
         * comes out -1 -> a straight-edged dark region. Clamp clip z to just
         * inside the far plane: mul t.x, p.w, c.w ; min p.z, p.z, t.x
         * (c.w = 0.9999, loaded at draw). */
        int t = o->r12 == 0 ? 1 : 0;
        vdst tx = { PC_SPR_TEMP, t, 1 }, pz = { PC_SPR_TEMP, o->r12, 4 };
        vsrc c, a[2];
        memset(&c, 0, sizeof c);
        c.mux = MUX_C; c.reg = r->clamp_c + 96;
        c.swz[0] = c.swz[1] = c.swz[2] = c.swz[3] = 3;
        a[0] = src_temp(o->r12, 3); a[1] = c;
        emit_op(o, D3DSIO_MUL, tx, a, 2);
        a[0] = src_temp(o->r12, 2); a[1] = src_temp(t, 0);
        emit_op(o, D3DSIO_MIN, pz, a, 2);
    }
    if (pos_written && !direct && r->hor_c >= 0) {
        /* Widescreen: clip x *= k, except for screen-aligned draws (w == 1).
         * PC c[hor_c] = (k, 1-k, 1, 0) is loaded with each draw. Every temp
         * but the position is dead here, so one serves as scratch.
         *   sge t.x, p.w, c.z ; sge t.y, c.z, p.w   -> both 1 only when w == 1
         *   mul t.x, t.x, t.y ; mad t.x, t.x, c.y, c.x ; mul p.x, p.x, t.x */
        int t = o->r12 == 0 ? 1 : 0;
        vdst tx = { PC_SPR_TEMP, t, 1 }, ty = { PC_SPR_TEMP, t, 2 }, px = { PC_SPR_TEMP, o->r12, 1 };
        vsrc c, a[3];
        memset(&c, 0, sizeof c);
        c.mux = MUX_C; c.reg = r->hor_c + 96;
        a[0] = src_temp(o->r12, 3); a[1] = c; a[1].swz[0] = a[1].swz[1] = a[1].swz[2] = a[1].swz[3] = 2;
        emit_op(o, D3DSIO_SGE, tx, a, 2);
        a[0] = a[1]; a[1] = src_temp(o->r12, 3);
        emit_op(o, D3DSIO_SGE, ty, a, 2);
        a[0] = src_temp(t, 0); a[1] = src_temp(t, 1);
        emit_op(o, D3DSIO_MUL, tx, a, 2);
        a[0] = src_temp(t, 0); a[1] = c; a[1].swz[0] = a[1].swz[1] = a[1].swz[2] = a[1].swz[3] = 1;
        a[2] = c; a[2].swz[0] = a[2].swz[1] = a[2].swz[2] = a[2].swz[3] = 0;
        emit_op(o, D3DSIO_MAD, tx, a, 3);
        a[0] = src_temp(o->r12, 0); a[1] = src_temp(t, 0);
        emit_op(o, D3DSIO_MUL, px, a, 2);
    }
    if (pos_written && !direct) {
        vdst p = { PC_SPR_RASTOUT, 0, 0xF };
        vsrc s = src_temp(o->r12, 0);
        s.swz[0] = 0; s.swz[1] = 1; s.swz[2] = 2; s.swz[3] = 3;
        emit_op(o, D3DSIO_MOV, p, &s, 1);
    }
    emit(o, D3DVS_END());
    if (dropped && vsh_log_on())
        fprintf(stderr, "[VSH] handle %08X: no spare temp, %d position reads dropped\n", r->handle, dropped);
    if (o->bad && !o->why[0]) snprintf(o->why, sizeof o->why, "encoding");
    return !o->bad;
}

static void vsh_dump(const vsh_rec *r, const vsh_out *o)
{
    static const char *mac_n[] = {"nop","mov","mul","add","mad","dp3","dph","dp4","dst","min","max","slt","sge","arl","?","?"};
    static const char *ilu_n[] = {"nop","mov","rcp","rcc","rsq","exp","log","lit"};
    int i;
    fprintf(stderr, "[VSH] handle %08X: %d insns, inputs %04X, pc %d tokens%s%s\n",
            r->handle, r->ninsn, r->inputs, o->n, o->bad ? " FAILED: " : "", o->bad ? o->why : "");
    for (i = 0; i < r->ninsn; i++) {
        const uint32_t *in = &r->prog[i * 4];
        static const char xyzw[] = "xyzw";
        char sw[3][6];
        uint32_t swz[3] = { F_A_SWZ(in), F_B_SWZ(in), F_C_SWZ(in) }, neg[3] = { F_A_NEG(in), F_B_NEG(in), F_C_NEG(in) };
        int k;
        for (k = 0; k < 3; k++) {
            sw[k][0] = neg[k] ? '-' : '.';
            sw[k][1] = xyzw[(swz[k] >> 6) & 3]; sw[k][2] = xyzw[(swz[k] >> 4) & 3];
            sw[k][3] = xyzw[(swz[k] >> 2) & 3]; sw[k][4] = xyzw[swz[k] & 3]; sw[k][5] = 0;
        }
        fprintf(stderr, "[VSH]   %2d %-3s/%-3s A%d.%d%s B%d.%d%s C%d.%d%s c%u v%u out r%u m%X i%X o%X%s%u mux%u%s%s\n", i,
                mac_n[F_MAC(in)], ilu_n[F_ILU(in)], F_A_MUX(in), F_A_R(in), sw[0], F_B_MUX(in), F_B_R(in), sw[1],
                F_C_MUX(in), F_C_R(in), sw[2], F_CONST(in), F_V(in), F_OUT_R(in), F_MAC_MASK(in), F_ILU_MASK(in),
                F_O_MASK(in), F_ORB(in) ? " o" : " c", F_O_ADDR(in), F_O_MUX(in), F_A0X(in) ? " a0" : "",
                F_FINAL(in) ? " END" : "");
    }
    fflush(stderr);
}

static int vsh_prepare2(vsh_rec *r, int clamp)
{
    vsh_out *o;
    DWORD pc_decl[40];
    int n = 0, k;
    int *state = clamp ? &r->clamp_state : &r->translated;
    HRESULT hr;

    if (*state)
        return *state > 0;
    *state = -1;
    o = (vsh_out *)calloc(1, sizeof *o);
    if (!o)
        return 0;
    if (!clamp) vsh_parse_decl(r);
    if (vsh_translate(r, o, clamp)) {
        pc_decl[n++] = D3DVSD_STREAM(0);
        for (k = 0; k < 16; k++)
            if (r->inputs & (1u << k))
                pc_decl[n++] = D3DVSD_REG(k, D3DVSDT_FLOAT4);
        pc_decl[n++] = D3DVSD_END();
        hr = IDirect3DDevice8_CreateVertexShader(s_dev, pc_decl, o->tok, clamp ? &r->pc_vs_clamp : &r->pc_vs, 0);
        if (SUCCEEDED(hr))
            *state = 1;
        else {
            o->bad = 1;
            snprintf(o->why, sizeof o->why, "CreateVertexShader hr=0x%08lX", (unsigned long)hr);
        }
    }
    {
        static int dumped;
        if (vsh_log_on() || (o->bad && dumped < 12)) {
            dumped++;
            vsh_dump(r, o);
        }
    }
    free(o);
    return *state > 0;
}
static int vsh_prepare(vsh_rec *r) { return vsh_prepare2(r, 0); }

/* ---- capture ------------------------------------------------------------- */

static void vsh_capture(uint32_t pdecl, uint32_t pfunc, uint32_t phandle)
{
    vsh_rec *r;
    uint32_t handle, hdr;
    int n;
    if (!phandle || !(handle = MEM32(phandle)))
        return;
    r = vsh_find(handle);
    if (!r) {
        if (s_nvsh >= VSH_MAX_RECORDS)
            return;
        r = (vsh_rec *)calloc(1, sizeof *r);
        if (!r)
            return;
        s_vsh[s_nvsh++] = r;
    } else {
        if (r->translated > 0)
            IDirect3DDevice8_DeleteVertexShader(s_dev, r->pc_vs);
        memset(r, 0, sizeof *r);
    }
    r->handle = handle;
    for (n = 0; pdecl && n < 96; n++) {
        r->decl[n] = MEM32(pdecl + 4 * n);
        if (r->decl[n] == 0xFFFFFFFFu) { n++; break; }
    }
    r->ndecl = n;
    if (pfunc) {
        hdr = MEM32(pfunc);
        n = (int)((hdr >> 16) & 0xFF);
        if ((hdr & 0xFF) != 0x78 || n <= 0 || n > 136) {
            r->translated = -1;              /* state / write shader: not drawn */
            return;
        }
        r->ninsn = n;
        memcpy(r->prog, gptr(pfunc + 4), (size_t)n * 16);
    } else {
        r->translated = -1;                  /* pass-through program: later */
    }
}

/* ---- draw ------------------------------------------------------------------ */

static float *s_vconv;
static size_t s_vconv_cap;

/* Convert vertices [lo, hi] (Xbox vertex numbers) to float4 per input. */
static const float *vsh_build_vertices(const vsh_rec *r, uint32_t lo, uint32_t hi, int *stride_out)
{
    int regs[16], nregs = 0, k;
    uint32_t base[16], stride[16], v;
    size_t need;
    float *dst;

    for (k = 0; k < 16; k++)
        if (r->inputs & (1u << k)) regs[nregs++] = k;
    for (k = 0; k < 16; k++) {
        uint32_t vb = s_stream_vb[k];
        base[k] = vb ? phys_to_va(MEM32(vb + 4)) : 0;
        stride[k] = s_stream_stride[k];
    }
    need = (size_t)(hi - lo + 1) * (size_t)nregs * 4;
    if (need > s_vconv_cap) {
        free(s_vconv);
        s_vconv_cap = need + need / 2;
        s_vconv = (float *)malloc(s_vconv_cap * sizeof(float));
        if (!s_vconv) { s_vconv_cap = 0; return NULL; }
    }
    dst = s_vconv;
    for (v = lo; v <= hi; v++) {
        for (k = 0; k < nregs; k++, dst += 4) {
            int reg = regs[k], st = r->in_stream[reg];
            if (st < 0 || !base[st])
                { dst[0] = dst[1] = dst[2] = 0.0f; dst[3] = 1.0f; continue; }
            vsh_fetch(r->in_type[reg],
                      (const uint8_t *)gptr(base[st] + v * stride[st] + r->in_offset[reg]), dst);
        }
    }
    *stride_out = nregs * 16;
    return s_vconv;
}

/* Live control "SKIPVS <handle hex>" (0 = none): leave that program's draws
 * out, to see what a pass contributes. */
static uint32_t s_skip_vs, s_show_va, s_dump_vc;
static int s_vs_tablefog = -1;            /* live "VSFOG 1": old behaviour, for comparison */
void hle_vs_tablefog(int on) { s_vs_tablefog = on; }
void hle_skip_vs(uint32_t h) { s_skip_vs = h; }
void hle_dump_vc(uint32_t h) { s_dump_vc = h; }
void hle_show_va(uint32_t h) { s_show_va = h; }

static int draw_vs(uint32_t xprim, uint32_t nverts, uint32_t start, uint32_t pidx)
{
    vsh_rec *r = vsh_find(s_vshader);
    D3DPRIMITIVETYPE pt;
    UINT prims;
    int stride;
    const float *verts;
    static uint16_t idx[65536 / 4 * 6];

    int clamp_load = 0;
    s_vsh_draws++;
    if (s_skip_vs && s_vshader == s_skip_vs)
        return 1;
    if (s_dump_vc && s_vshader == s_dump_vc) {
        /* Live control "DUMPVC <handle>": that program's constants c96..c111
         * (as the program numbers them) and its code, at its next draw. */
        int k;
        s_dump_vc = 0;
        for (k = 96; k < 112; k++) {
            const float *c = (const float *)gptr(VSH_SHADOW_CONSTANTS + (uint32_t)k * 16u);
            fprintf(stderr, "[VC] c%d = %g %g %g %g\n", k, c[0], c[1], c[2], c[3]);
        }
        if (r) { vsh_out *o = (vsh_out *)calloc(1, sizeof *o); if (o) { vsh_dump(r, o); free(o); } }
        fflush(stderr);
    }
    if (!r || !vsh_prepare(r) || !prim_convert(xprim, nverts, &pt, &prims)) {
        s_vsh_fail_draws++;
        cap_draw("VS!", xprim, nverts, pidx);        /* not drawn */
        return 0;
    }
    apply_state();
    if (s_vs_tablefog < 0) s_vs_tablefog = getenv("CW_VS_TABLEFOG") ? 1 : 0;
    if (!s_vs_tablefog && r->writes_fog) {
        /* A vertex program supplies the fog factor itself (oFog: the terrain
         * programs compute 1 near, 0 past ~1500 units). The title leaves
         * FOGTABLEMODE at LINEAR with start == end; on the PC a table mode
         * makes the runtime ignore oFog and fog by pixel depth instead, which
         * hazed nearby ground drawn by the fogged terrain program and left a
         * light/dark seam against unfogged neighbouring patches. Only for
         * programs that write oFog: without one the PC treats the factor as
         * 0 (fully fogged) -- the player's tank turned hazy brown. */
        IDirect3DDevice8_SetRenderState(s_dev, D3DRS_FOGTABLEMODE, D3DFOG_NONE);
        IDirect3DDevice8_SetRenderState(s_dev, D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
    }
    cap_draw("vs", xprim, nverts, pidx);
    if (s_show_va && s_pshader == s_show_va) {
        /* Live control "SHOWVA <pixel shader handle>": draw that shader's
         * vertex alpha (v0.a) as grey, opaque -- e.g. the terrain detail
         * pass's distance fade. */
        static DWORD ps;
        static const DWORD tok[] = { 0xFFFF0101u, D3DSIO_MOV, 0x800F0000u, 0x90FF0000u, 0x0000FFFFu };
        if (!ps) IDirect3DDevice8_CreatePixelShader(s_dev, tok, &ps);
        if (ps) {
            IDirect3DDevice8_SetPixelShader(s_dev, ps);
            IDirect3DDevice8_SetRenderState(s_dev, D3DRS_ALPHABLENDENABLE, FALSE);
            IDirect3DDevice8_SetRenderState(s_dev, D3DRS_FOGENABLE, FALSE);
        }
    }
    {
        /* CW_VSH_HILITE=all: every vertex program's draws in a flat colour
         * (program index i -> palette[i % 12], index as in [VSH] logs). */
        static int hl = -1;
        if (hl < 0) { const char *e = getenv("CW_VSH_HILITE"); hl = e && *e ? 1 : 0; }
        if (hl) {
            static const float pal[12][4] = {
                {1,0,0,1},{0,1,0,1},{0,0,1,1},{1,1,0,1},{1,0,1,1},{0,1,1,1},
                {1,1,1,1},{1,.5f,0,1},{.5f,0,1,1},{0,.5f,0,1},{.5f,.5f,.5f,1},{.5f,.25f,0,1} };
            int k;
            for (k = 0; k < s_nvsh; k++) if (s_vsh[k] == r) break;
            psh_solid(pal[k % 12]);
            IDirect3DDevice8_SetRenderState(s_dev, D3DRS_ALPHABLENDENABLE, FALSE);
            if (!r->logged) {
                r->logged = 1;
                fprintf(stderr, "[VSH] program %d = handle %08X colour %d\n", k, r->handle, k % 12);
            }
        }
    }
    {
        /* CW_VSH_STATE=<handle hex>: log the pipeline state of that program's
         * draws, once per distinct combination (up to 16). */
        static uint32_t want = 1, seen[16][4];
        static int nseen;
        if (want == 1) { const char *e = getenv("CW_VSH_STATE"); want = e ? (uint32_t)strtoul(e, NULL, 16) : 0; }
        if (want && r->handle == want && nseen < 16) {
            uint32_t key[4] = { s_pshader, XRS(XRS_ALPHABLENDENABLE) | XRS(XRS_SRCBLEND) << 4 | XRS(XRS_DESTBLEND) << 16,
                                XRS(XRS_ALPHATESTENABLE) | XRS(XRS_ZWRITEENABLE) << 1 | XRS(XRS_ZENABLE) << 2 | XRS(XRS_ALPHAFUNC) << 4,
                                s_tex[0] };
            int k;
            for (k = 0; k < nseen; k++) if (!memcmp(seen[k], key, sizeof key)) break;
            if (k == nseen) {
                memcpy(seen[nseen++], key, sizeof key);
                fprintf(stderr, "[VSTATE] ps=%08X blend=%u %X->%X atest=%u func=%X ref=%X z=%u zw=%u cw=%08X "
                        "tex0=%08X fmt=%X op0=%u a1=%u a2=%u aop0=%u op1=%u fog=%u\n",
                        s_pshader, XRS(XRS_ALPHABLENDENABLE), XRS(XRS_SRCBLEND), XRS(XRS_DESTBLEND),
                        XRS(XRS_ALPHATESTENABLE), XRS(XRS_ALPHAFUNC), XRS(XRS_ALPHAREF), XRS(XRS_ZENABLE),
                        XRS(XRS_ZWRITEENABLE), XRS(XRS_COLORWRITEENABLE), s_tex[0],
                        s_tex[0] ? (MEM32(s_tex[0] + 0xC) >> 8) & 0xFF : 0xFFu,
                        XTSS(0, XTSS_COLOROP), XTSS(0, XTSS_COLORARG1), XTSS(0, XTSS_COLORARG2),
                        XTSS(0, XTSS_ALPHAOP), XTSS(1, XTSS_COLOROP), XRS(XRS_FOGENABLE));
            }
        }
    }
    {
        /* Stencil shadow-volume passes (stencil on, depth test GEQUAL = the
         * z-fail method) use the far-clamped variant. CW_NO_ZCLAMP=1 reverts. */
        static int off = -1;
        int clamp;
        clamp_load = 0;
        if (off < 0) off = getenv("CW_NO_ZCLAMP") ? 1 : 0;
        clamp = !off && XRS(XRS_STENCILENABLE) && XRS(XRS_ZFUNC) == 0x206 && vsh_prepare2(r, 1);
        IDirect3DDevice8_SetVertexShader(s_dev, clamp ? r->pc_vs_clamp : r->pc_vs);
        clamp_load = clamp && r->clamp_c >= 0 && r->clamp_c != r->hor_c;
    }
    IDirect3DDevice8_SetVertexShaderConstant(s_dev, 0, gptr(VSH_SHADOW_CONSTANTS + 96 * 16), 96);
    if (clamp_load) {                       /* after the bulk load, which covers c0..c95 */
        float cc[4] = { 1.0f, 0.0f, 1.0f, 0.9999f };
        IDirect3DDevice8_SetVertexShaderConstant(s_dev, (DWORD)r->clamp_c, cc, 1);
    }
    if (s_vs_consts >= 192)
        IDirect3DDevice8_SetVertexShaderConstant(s_dev, 96, gptr(VSH_SHADOW_CONSTANTS), 96);
    {
        int k;
        for (k = 0; k < r->nlow; k++)
            IDirect3DDevice8_SetVertexShaderConstant(s_dev, (DWORD)r->low_pc[k],
                gptr(VSH_SHADOW_CONSTANTS + (uint32_t)r->low_x[k] * 16u), 1);
    }
    if (r->hor_c >= 0) {
        float hc[4] = { s_k_cur > 0.0f ? s_k_cur : 1.0f, 0.0f, 1.0f, 0.9999f };
        hc[1] = 1.0f - hc[0];
        IDirect3DDevice8_SetVertexShaderConstant(s_dev, (DWORD)r->hor_c, hc, 1);
    }

    if (!pidx) {
        verts = vsh_build_vertices(r, start, start + nverts - 1, &stride);
        if (!verts) return 0;
        if (xprim == 8) {
            uint32_t q, nq = nverts / 4;
            if (nq > 65536 / 4) nq = 65536 / 4;
            for (q = 0; q < nq; q++) {
                idx[q*6+0] = (uint16_t)(q*4); idx[q*6+1] = (uint16_t)(q*4+1); idx[q*6+2] = (uint16_t)(q*4+2);
                idx[q*6+3] = (uint16_t)(q*4); idx[q*6+4] = (uint16_t)(q*4+2); idx[q*6+5] = (uint16_t)(q*4+3);
            }
            chk_DIPUP(s_dev, D3DPT_TRIANGLELIST, 0, nq * 4, nq * 2,
                                                    idx, D3DFMT_INDEX16, verts, (UINT)stride);
        } else {
            chk_DPUP(s_dev, pt, prims, verts, (UINT)stride);
        }
    } else {
        const uint16_t *gi = (const uint16_t *)gptr(pidx);
        uint32_t i, lo = 0xFFFF, hi = 0, n = nverts > 65536 ? 65536 : nverts;
        const uint8_t *vbase;
        for (i = 0; i < n; i++) {
            if (gi[i] < lo) lo = gi[i];
            if (gi[i] > hi) hi = gi[i];
        }
        if (lo > hi) return 0;
        verts = vsh_build_vertices(r, s_ib_base + lo, s_ib_base + hi, &stride);
        if (!verts) return 0;
        vbase = (const uint8_t *)verts - (size_t)lo * (size_t)stride;   /* index lo -> element 0 */
        if (xprim == 8) {
            uint32_t q, nq = n / 4;
            for (q = 0; q < nq; q++) {
                idx[q*6+0] = gi[q*4]; idx[q*6+1] = gi[q*4+1]; idx[q*6+2] = gi[q*4+2];
                idx[q*6+3] = gi[q*4]; idx[q*6+4] = gi[q*4+2]; idx[q*6+5] = gi[q*4+3];
            }
            chk_DIPUP(s_dev, D3DPT_TRIANGLELIST, lo, hi - lo + 1,
                                                    nq * 2, idx, D3DFMT_INDEX16, vbase, (UINT)stride);
        } else {
            chk_DIPUP(s_dev, pt, lo, hi - lo + 1, prims,
                                                    gi, D3DFMT_INDEX16, vbase, (UINT)stride);
        }
    }
    return 1;
}
