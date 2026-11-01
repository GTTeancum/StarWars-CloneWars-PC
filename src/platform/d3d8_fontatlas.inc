/*
 * d3d8_fontatlas.inc -- the title's glyph sheets, re-packed with room.
 *
 * The title packs its glyphs edge to edge on a 128x128 sheet, which is fine
 * when the sheet is sampled at the size it was made for. Enlarged, a quad's
 * filter reaches past its glyph's box into the neighbour's, and the edges of
 * letters pick up slivers of the next one. Insetting the quads trades that
 * for clipped strokes.
 *
 * So each known sheet (d3d8_fonttab.inc) gets a second texture: every glyph
 * drawn at its size on screen -- from Michroma, or the title's own glyph,
 * cut out and enlarged on its own, where Michroma lacks one -- each in its
 * own cell with a clear border. A text
 * quad sampling a known glyph has its texture coordinates moved to that
 * glyph's cell and is drawn from the atlas. A batch with any quad that is
 * not a known glyph is left on the 4x sheet, the fallback.
 */

#define ATLAS_PAD 8                       /* clear texels around each glyph */
#define ATLAS_SIZE 1024

typedef struct {
    IDirect3DTexture8 *tex;
    const font_sheet *fs;
    uint8_t alpha[128 * 128];              /* the title's sheet, to tell blank quads */
    float u0[256], v0[256], u1[256], v1[256];   /* per glyph index in fs */
} font_atlas;

static void font_atlas_build(const font_sheet *fs, const uint8_t *a)
{
    font_atlas *at;
    D3DLOCKED_RECT lr;
    uint32_t x = ATLAS_PAD, y = ATLAS_PAD, rowh = 0;
    int i;
    s_last_atlas = NULL;
    if (!fs || fs->n > 256 || !(at = (font_atlas *)calloc(1, sizeof *at))) return;
    if (FAILED(IDirect3DDevice8_CreateTexture(s_dev, ATLAS_SIZE, ATLAS_SIZE, 1, 0, D3DFMT_A8R8G8B8,
                                              D3DPOOL_MANAGED, &at->tex)) ||
        FAILED(IDirect3DTexture8_LockRect(at->tex, 0, &lr, NULL, 0))) {
        if (at->tex) IDirect3DTexture8_Release(at->tex);
        free(at);
        return;
    }
    for (y = 0; y < ATLAS_SIZE; y++) memset((uint8_t *)lr.pBits + y * lr.Pitch, 0, ATLAS_SIZE * 4);
    y = ATLAS_PAD;
    for (i = 0; i < fs->n; i++) {
        const font_glyph *g = &fs->g[i];
        /* Each glyph at the size it appears on screen (the title's 2D layout
         * is 640x480, scaled by s_hs): sampled 1:1, thin strokes survive.
         * Shrinking the 4x copy instead skipped texels -- an i came out as a
         * colon, and every letter looked faded. */
        uint32_t w = (uint32_t)ceilf(g->w * s_hs), h = (uint32_t)ceilf(g->h * s_hs);
        uint8_t *cell;
        WCHAR c = g->c >= 'a' && g->c <= 'z' ? (WCHAR)(g->c - 32) : (WCHAR)g->c;
        if (w < 1) w = 1;
        if (h < 1) h = 1;
        if (x + w + ATLAS_PAD > ATLAS_SIZE) { x = ATLAS_PAD; y += rowh + 2 * ATLAS_PAD; rowh = 0; }
        if (y + h + ATLAS_PAD > ATLAS_SIZE) break;
        cell = (uint8_t *)lr.pBits + y * lr.Pitch + x * 4;
        if (g->c >= 0x80 || !michroma_glyph(c, (uint32_t *)cell, (uint32_t)lr.Pitch, (int)w, (int)h)) {
            /* The title's own glyph, cut out of the 1x sheet on its own (so
             * no neighbour can bleed in) and enlarged to the cell: bicubic,
             * then a narrow smoothstep around half coverage. */
            uint32_t r, q;
            for (r = 0; r < h; r++)
                for (q = 0; q < w; q++) {
                    float sx = ((float)q + 0.5f) * g->w / w - 0.5f, sy = ((float)r + 0.5f) * g->h / h - 0.5f, v = 0, t;
                    int ix = (int)floorf(sx), iy = (int)floorf(sy), kx, ky;
                    for (ky = 0; ky < 4; ky++) {
                        int yy = iy + ky - 1;
                        float wy = cubic_w(sy - (float)yy);
                        for (kx = 0; kx < 4; kx++) {
                            int xx = ix + kx - 1;
                            if (xx < 0 || yy < 0 || xx >= g->w || yy >= g->h) continue;   /* outside: clear */
                            v += a[(g->y + yy) * 128 + g->x + xx] * wy * cubic_w(sx - (float)xx);
                        }
                    }
                    t = (v / 255.0f - 0.40f) / 0.20f;
                    t = t < 0 ? 0 : t > 1 ? 1 : t;
                    ((uint32_t *)(cell + r * lr.Pitch))[q] =
                        ((uint32_t)(t * t * (3.0f - 2.0f * t) * 255.0f + 0.5f) << 24) | 0x00FFFFFFu;
                }
        }
        at->u0[i] = (float)x / ATLAS_SIZE;       at->v0[i] = (float)y / ATLAS_SIZE;
        at->u1[i] = (float)(x + w) / ATLAS_SIZE; at->v1[i] = (float)(y + h) / ATLAS_SIZE;
        x += w + 2 * ATLAS_PAD;
        if (h > rowh) rowh = h;
    }
    IDirect3DTexture8_UnlockRect(at->tex, 0);
    if (i < fs->n) { IDirect3DTexture8_Release(at->tex); free(at); return; }
    at->fs = fs;
    memcpy(at->alpha, a, sizeof at->alpha);
    s_last_atlas = at;
}

static font_atlas *font_atlas_of(uint32_t res)
{
    for (int i = 0; i < s_nfont; i++) if (s_font_res[i] == res) return (font_atlas *)s_font_atlas[i];
    return NULL;
}

/* A batch of 2D quads (vertices already mapped to the screen) on a known
 * sheet: return a copy whose texture coordinates point into the atlas, with
 * the atlas bound; NULL to leave the batch on the sheet. */
static const uint8_t *font_atlas_remap(const uint8_t *v, uint32_t nverts, uint32_t stride)
{
    static uint8_t *buf;
    static uint32_t cap;
    font_atlas *at = font_atlas_of(s_tex[0]);
    uint32_t off = 16, nq = nverts / 4, q, j;
    if (!at || !nq || ((s_vshader >> 8) & 0xF) == 0) return NULL;
    if (s_vshader & D3DFVF_DIFFUSE) off += 4;
    if (s_vshader & D3DFVF_SPECULAR) off += 4;
    if (off + 8 > stride) return NULL;
    if (cap < nverts * stride) {
        free(buf);
        cap = nverts * stride;
        if (!(buf = (uint8_t *)malloc(cap))) { cap = 0; return NULL; }
    }
    memcpy(buf, v, (size_t)nverts * stride);
    for (q = 0; q < nq; q++) {
        float umin = 1e9f, vmin = 1e9f, umax = -1e9f, vmax = -1e9f;
        float xmin = 1e9f, ymin = 1e9f, xmax = -1e9f, ymax = -1e9f, bx0, bx1, by0, by1;
        const font_glyph *fg;
        int g;
        for (j = 0; j < 4; j++) {
            const float *p = (const float *)(buf + (size_t)(q * 4 + j) * stride);
            const float *uv = (const float *)(buf + (size_t)(q * 4 + j) * stride + off);
            if (uv[0] < umin) umin = uv[0]; if (uv[0] > umax) umax = uv[0];
            if (uv[1] < vmin) vmin = uv[1]; if (uv[1] > vmax) vmax = uv[1];
            if (p[0] < xmin) xmin = p[0]; if (p[0] > xmax) xmax = p[0];
            if (p[1] < ymin) ymin = p[1]; if (p[1] > ymax) ymax = p[1];
        }
        umin *= 128.0f; vmin *= 128.0f; umax *= 128.0f; vmax *= 128.0f;
        for (g = 0; g < at->fs->n; g++)
            if (fabsf(umin - at->fs->g[g].x) < 0.75f && fabsf(vmin - at->fs->g[g].y) < 0.75f) break;
        if (g == at->fs->n || umax <= umin || vmax <= vmin) {
            /* Not a glyph we know. A quad over a blank patch of the sheet --
             * a space -- draws nothing either way: collapse it. Anything
             * else leaves the whole batch on the sheet. */
            int x, y, blank = 1;
            for (y = (int)vmin; y < (int)ceilf(vmax) && y < 128 && blank; y++)
                for (x = (int)umin; x < (int)ceilf(umax) && x < 128; x++)
                    if (y >= 0 && x >= 0 && at->alpha[y * 128 + x]) { blank = 0; break; }
            if (!blank) {
                static int said;
                if (!said++) d3d_log("font atlas: unknown glyph at %.1f,%.1f; batch left on the sheet", umin, vmin);
                return NULL;
            }
            for (j = 0; j < 4; j++) {
                float *p = (float *)(buf + (size_t)(q * 4 + j) * stride);
                p[0] = xmin; p[1] = ymin;
            }
            continue;
        }
        fg = &at->fs->g[g];
        /* The quad may cover more of the sheet than the glyph -- its advance,
         * blank on the title's sheet. Trim it to the glyph's box, on screen
         * and in the texture, so it never reaches the next atlas cell. */
        bx0 = (float)fg->x; bx1 = (float)(fg->x + fg->w);
        by0 = (float)fg->y; by1 = (float)(fg->y + fg->h);
        if (bx1 > umax) bx1 = umax;
        if (by1 > vmax) by1 = vmax;
        for (j = 0; j < 4; j++) {
            float *p = (float *)(buf + (size_t)(q * 4 + j) * stride);
            float *uv = (float *)(buf + (size_t)(q * 4 + j) * stride + off);
            int right = uv[0] * 128.0f > (umin + umax) * 0.5f, bottom = uv[1] * 128.0f > (vmin + vmax) * 0.5f;
            float su = right ? bx1 : bx0, sv = bottom ? by1 : by0;
            p[0] = xmin + (su - umin) / (umax - umin) * (xmax - xmin);
            p[1] = ymin + (sv - vmin) / (vmax - vmin) * (ymax - ymin);
            uv[0] = at->u0[g] + (su - fg->x) / (float)fg->w * (at->u1[g] - at->u0[g]);
            uv[1] = at->v0[g] + (sv - fg->y) / (float)fg->h * (at->v1[g] - at->v0[g]);
        }
    }
    {
        static int said;
        if (!said++) d3d_log("text drawn from the padded font atlas");
    }
    IDirect3DDevice8_SetTexture(s_dev, 0, (IDirect3DBaseTexture8 *)at->tex);
    IDirect3DDevice8_SetTextureStageState(s_dev, 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    IDirect3DDevice8_SetTextureStageState(s_dev, 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    IDirect3DDevice8_SetTextureStageState(s_dev, 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    IDirect3DDevice8_SetTextureStageState(s_dev, 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    IDirect3DDevice8_SetTextureStageState(s_dev, 0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
    return buf;
}
