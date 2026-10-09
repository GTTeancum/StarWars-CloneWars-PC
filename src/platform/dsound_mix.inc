/*
 * dsound_mix.inc -- host audio output for the DirectSound replacement.
 * Included at the end of dsound.c.
 *
 * The replacement above keeps every buffer's and stream's state the way the
 * title sees it: data pointer, format, volume, frequency, and the tick the
 * current play (or the head stream packet) began. That is enough to render
 * sound without touching the timing model: a mixer thread asks, for each
 * output sample, "which source frame is this voice at now?" -- elapsed time
 * since the voice's start tick times its rate -- and reads it from guest
 * memory. Stream packets complete on the same clock (hle_stream_advance),
 * so a stream's audio runs across packets exactly as fast as the title sees
 * them finish.
 *
 * Output: winmm waveOut, 44.1 kHz stereo 16-bit, four 40 ms blocks queued.
 * Formats: PCM 8/16-bit and Xbox ADPCM (tag 0x0069, 36 bytes per channel
 * per 64-sample block, IMA nibbles). Volume is hundredths of a dB. Mix bins,
 * 3D position and effects are ignored (a voice plays centred).
 *
 * CW_AUDIO=0 turns output off.
 */

#include <mmsystem.h>

#define MIX_RATE     44100
#define MIX_BLOCK    1764          /* 40 ms */
#define MIX_NBUF     6
#define MIX_MAXFMT   4096
#define MIX_MAXBUF   512

typedef struct {
    uint32_t obj;
    uint16_t tag, ch, bits, align;
} mix_fmt;

static mix_fmt  s_mfmt[MIX_MAXFMT];
static uint32_t s_mbufs[MIX_MAXBUF];
static int      s_mix_on = -1;

static mix_fmt *mix_fmt_of(uint32_t obj, int create)
{
    int i, free_i = -1;
    for (i = 0; i < MIX_MAXFMT; i++) {
        if (s_mfmt[i].obj == obj) return &s_mfmt[i];
        /* A slot whose object has been released is free again. This table
         * used to only grow: after ~1000 sound objects (about 15 minutes of
         * menus and levels) a new movie's audio stream got no entry, the
         * mixer skipped it, its packets never completed and the movie hung. */
        if (free_i < 0 && (!s_mfmt[i].obj ||
                           (!hle_is(s_mfmt[i].obj, HLE_KIND_BUFFER) && !hle_is(s_mfmt[i].obj, HLE_KIND_STREAM))))
            free_i = i;
    }
    if (!create || free_i < 0) return NULL;
    s_mfmt[free_i].obj = obj;
    return &s_mfmt[free_i];
}

/* Whether the mixer will play (and so complete) this stream's packets. If
 * not -- unknown format -- the stream completes on the wall clock instead,
 * so a title waiting on it never hangs. */
static int mix_can_play(uint32_t s)
{
    mix_fmt *f = mix_fmt_of(s, 0);
    return f && (f->tag == 0x0069 || f->tag == 1) && f->ch && (f->bits == 8 || f->bits == 16 || f->tag == 0x0069);
}

static void mix_note_format(uint32_t obj, uint32_t wfx)
{
    mix_fmt *f;
    if (!wfx || !(f = mix_fmt_of(obj, 1)))
        return;
    f->tag   = MEM16(wfx + 0);
    f->ch    = MEM16(wfx + 2);
    f->align = MEM16(wfx + 12);
    f->bits  = MEM16(wfx + 14);
    {
        static uint32_t seen[32];
        static int n;
        uint32_t key = (uint32_t)f->tag << 16 ^ (uint32_t)f->ch << 12 ^ (uint32_t)f->bits << 4 ^ MEM32(wfx + 4);
        int i;
        for (i = 0; i < n; i++) if (seen[i] == key) break;
        if (i == n && n < 32) {
            seen[n++] = key;
            fprintf(stderr, "[AUDIO] format tag %04X ch %u bits %u rate %u align %u avg %u (%s)\n",
                    f->tag, f->ch, f->bits, MEM32(wfx + 4), f->align, MEM32(wfx + 8),
                    hle_is(obj, HLE_KIND_STREAM) ? "stream" : "buffer");
        }
    }
}

static void mix_track_buffer(uint32_t b, int add)
{
    int i;
    for (i = 0; i < MIX_MAXBUF; i++) {
        if (add && !s_mbufs[i]) { s_mbufs[i] = b; return; }
        if (!add && s_mbufs[i] == b) { s_mbufs[i] = 0; return; }
    }
}

/* ---- sample access ------------------------------------------------------ */

static const int s_ima_step[89] = {
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
    50, 55, 60, 66, 73, 80, 88, 97, 107, 118, 130, 143, 157, 173, 190, 209, 230,
    253, 279, 307, 337, 371, 408, 449, 494, 544, 598, 658, 724, 796, 876, 963,
    1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024, 3327,
    3660, 4026, 4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487,
    12635, 13899, 15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767 };
static const int s_ima_index[16] = { -1, -1, -1, -1, 2, 4, 6, 8, -1, -1, -1, -1, 2, 4, 6, 8 };

/* One decoded ADPCM block cached per call site (the mixer handles voices one
 * at a time, so a single cache per voice pass is enough). */
typedef struct { uint32_t base, block; int ch; int16_t pcm[2][64]; } adpcm_cache;

static void adpcm_decode_block(adpcm_cache *c, uint32_t base, uint32_t block, int ch)
{
    uint32_t p = base + block * 36u * (uint32_t)ch;
    int k;
    c->base = base; c->block = block; c->ch = ch;
    for (k = 0; k < ch; k++) {
        int pred = (int16_t)MEM16(p + 4u * (uint32_t)k), idx = MEM8(p + 4u * (uint32_t)k + 2), n;
        if (idx > 88) idx = 88;
        /* The header sample is the block's first output sample; 63 nibbles
         * follow (checked sample-exact against ffmpeg's adpcm_ima_xbox on
         * data\sounds\music\Ep1_v1_t05.wav). */
        c->pcm[k][0] = (int16_t)pred;
        for (n = 0; n < 63; n++) {
            /* data: 4-byte words interleaved per channel, 8 nibbles each, low first */
            uint32_t word = (uint32_t)(n / 8), nib = (uint32_t)(n % 8);
            uint8_t byte = MEM8(p + 4u * (uint32_t)ch + (word * (uint32_t)ch + (uint32_t)k) * 4u + nib / 2u);
            int code = (nib & 1) ? (byte >> 4) : (byte & 15);
            int step = s_ima_step[idx], diff = step >> 3;
            if (code & 1) diff += step >> 2;
            if (code & 2) diff += step >> 1;
            if (code & 4) diff += step;
            if (code & 8) pred -= diff; else pred += diff;
            if (pred > 32767) pred = 32767; else if (pred < -32768) pred = -32768;
            idx += s_ima_index[code];
            if (idx < 0) idx = 0; else if (idx > 88) idx = 88;
            c->pcm[k][n + 1] = (int16_t)pred;
        }
    }
}

/* Frame `fr` of the data at `base` (bytes available `bytes`), as L/R floats.
 * Returns 0 past the end. */
static int mix_frame(const mix_fmt *f, uint32_t base, uint32_t bytes, uint32_t fr, adpcm_cache *c,
                     float *l, float *r)
{
    int ch = f->ch ? f->ch : 1;
    if (f->tag == 0x0069) {
        uint32_t block = fr / 64u, blockbytes = 36u * (uint32_t)ch;
        if ((uint64_t)(block + 1) * blockbytes > bytes) return 0;
        if (c->base != base || c->block != block || c->ch != ch) adpcm_decode_block(c, base, block, ch);
        *l = c->pcm[0][fr % 64u] / 32768.0f;
        *r = ch > 1 ? c->pcm[1][fr % 64u] / 32768.0f : *l;
        return 1;
    } else {
        int bps = (f->bits == 8 ? 1 : 2) * ch;
        uint32_t off = fr * (uint32_t)bps;
        if (off + (uint32_t)bps > bytes) return 0;
        if (f->bits == 8) {
            *l = (MEM8(base + off) - 128) / 128.0f;
            *r = ch > 1 ? (MEM8(base + off + 1) - 128) / 128.0f : *l;
        } else {
            *l = (int16_t)MEM16(base + off) / 32768.0f;
            *r = ch > 1 ? (int16_t)MEM16(base + off + 2) / 32768.0f : *l;
        }
        return 1;
    }
}

static float mix_gain(uint32_t obj)
{
    int32_t v = (int32_t)MEM32(obj + O_VOLUME);   /* hundredths of a dB, <= 0 */
    if (v >= 0) return 1.0f;
    if (v <= -9600) return 0.0f;
    return powf(10.0f, (float)v / 2000.0f);
}

/* ---- mixing ------------------------------------------------------------- */

static float s_acc[MIX_BLOCK * 2];

/* Add a buffer voice for output frames [0, MIX_BLOCK) starting at time t0
 * (milliseconds, same clock as GetTickCount). */
static void mix_buffer(uint32_t b, double t0)
{
    const mix_fmt *f = mix_fmt_of(b, 0);
    uint32_t st = MEM32(b + O_STATUS), data = MEM32(b + O_DATA), bytes = MEM32(b + O_BYTES);
    uint32_t pstart = MEM32(b + O_PSTART), plen = MEM32(b + O_PLEN), rate = MEM32(b + O_FREQ);
    adpcm_cache c;
    float g;
    int i, fpb;
    uint32_t frames;
    if (!f || !(st & DSBSTATUS_PLAYING) || !data || !rate) return;
    g = mix_gain(b);
    if (g <= 0.0f) return;
    if (plen) { data += pstart; bytes = plen; }
    fpb = f->tag == 0x0069 ? 0 : (f->bits == 8 ? 1 : 2) * (f->ch ? f->ch : 1);
    frames = f->tag == 0x0069 ? (bytes / (36u * (f->ch ? f->ch : 1))) * 64u : bytes / (uint32_t)fpb;
    if (!frames) return;
    memset(&c, 0, sizeof c); c.base = 0xFFFFFFFFu;
    for (i = 0; i < MIX_BLOCK; i++) {
        double t = t0 + i * (1000.0 / MIX_RATE) - (double)MEM32(b + O_T0);
        double pos = t * rate / 1000.0;
        uint32_t fr;
        float l, r;
        if (pos < 0) continue;
        fr = (uint32_t)pos;
        if (st & DSBSTATUS_LOOPING) fr %= frames;
        else if (fr >= frames) break;
        if (!mix_frame(f, data, bytes, fr, &c, &l, &r)) continue;
        s_acc[2 * i] += l * g; s_acc[2 * i + 1] += r * g;
    }
}

/* A stream plays its queued packets back to back. The play position (source
 * frames into the head packet, fractional) lives here on the host; when the
 * head packet is used up it completes and the next one starts, so packet
 * completion follows what was actually played. */
#define MIX_MAXSTREAMPOS 64
static struct { uint32_t obj; double pos; } s_spos[MIX_MAXSTREAMPOS];

static double *mix_stream_pos(uint32_t s)
{
    int k, free_k = -1;
    for (k = 0; k < MIX_MAXSTREAMPOS; k++) {
        if (s_spos[k].obj == s) return &s_spos[k].pos;
        if (free_k < 0 && (!s_spos[k].obj || !hle_is(s_spos[k].obj, HLE_KIND_STREAM))) free_k = k;
    }
    if (free_k < 0) return NULL;
    s_spos[free_k].obj = s; s_spos[free_k].pos = 0.0;
    return &s_spos[free_k].pos;
}

static uint32_t mix_packet_frames(const mix_fmt *f, uint32_t size)
{
    uint32_t ch = f->ch ? f->ch : 1u;
    return f->tag == 0x0069 ? (size / (36u * ch)) * 64u : size / ((f->bits == 8 ? 1u : 2u) * ch);
}

static void mix_stream(uint32_t s, double t0)
{
    const mix_fmt *f = mix_fmt_of(s, 0);
    uint32_t rate = MEM32(s + O_FREQ);
    double *pos = mix_stream_pos(s), step;
    adpcm_cache c;
    int i;
    float g;
    (void)t0;
    if (!f || !rate || !pos || !mix_can_play(s) || (MEM32(s + O_STATUS) & DSSTREAMSTATUS_PAUSED)) return;
    g = mix_gain(s);
    step = (double)rate / MIX_RATE;
    memset(&c, 0, sizeof c); c.base = 0xFFFFFFFFu;
    for (i = 0; i < MIX_BLOCK; i++) {
        uint32_t buf, size, pf, fr;
        float l, r;
        if (!MEM32(s + O_NPKT)) { *pos = 0.0; break; }         /* starved */
        buf = MEM32(PKT_AT(s, 0) + 0);
        size = MEM32(PKT_AT(s, 0) + 4);
        pf = mix_packet_frames(f, size);
        fr = (uint32_t)*pos;
        if (fr >= pf) {
            uint32_t pkt[6];
            *pos -= pf;
            if (*pos < 0) *pos = 0;
            hle_stream_pop(s, pkt);
            hle_packet_finish(s, pkt, DS_OK);
            MEM32(s + O_T0) = (uint32_t)GetTickCount();
            i--;                                               /* same sample, next packet */
            continue;
        }
        if (g > 0.0f && buf && mix_frame(f, buf, size, fr, &c, &l, &r)) {
            s_acc[2 * i] += l * g; s_acc[2 * i + 1] += r * g;
        }
        *pos += step;
    }
}

static DWORD WINAPI mix_thread(LPVOID unused)
{
    HWAVEOUT wo;
    WAVEFORMATEX wf;
    WAVEHDR hdr[MIX_NBUF];
    static int16_t pcm[MIX_NBUF][MIX_BLOCK * 2];
    double clock;
    int i, next = 0;
    (void)unused;
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    s_in_callbacks = 1;     /* never run game callbacks on the output thread */

    memset(&wf, 0, sizeof wf);
    wf.wFormatTag = WAVE_FORMAT_PCM; wf.nChannels = 2; wf.nSamplesPerSec = MIX_RATE;
    wf.wBitsPerSample = 16; wf.nBlockAlign = 4; wf.nAvgBytesPerSec = MIX_RATE * 4;
    if (waveOutOpen(&wo, WAVE_MAPPER, &wf, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
        fprintf(stderr, "[AUDIO] waveOutOpen failed; no sound\n");
        return 0;
    }
    memset(hdr, 0, sizeof hdr);
    for (i = 0; i < MIX_NBUF; i++) {
        hdr[i].lpData = (LPSTR)pcm[i];
        hdr[i].dwBufferLength = sizeof pcm[i];
        waveOutPrepareHeader(wo, &hdr[i], sizeof hdr[i]);
        hdr[i].dwFlags |= WHDR_DONE;
    }
    s_mixer_drives_streams = 1;
    fprintf(stderr, "[AUDIO] output open: %d Hz stereo, %d x %d ms\n", MIX_RATE, MIX_NBUF,
            MIX_BLOCK * 1000 / MIX_RATE);
    clock = (double)GetTickCount();
    for (;;) {
        WAVEHDR *h = &hdr[next];
        double now;
        if (!(h->dwFlags & WHDR_DONE)) { Sleep(5); continue; }
        /* Render the window that starts where the last one ended; if the
         * mixer fell behind (or ran ahead) of the tick clock by more than a
         * few blocks, jump back into step. */
        now = (double)GetTickCount();
        if (clock < now - 200.0 || clock > now + 400.0) clock = now;
        memset(s_acc, 0, sizeof s_acc);
        hle_lock_tag("mixer");
        for (i = 0; i < MIX_MAXBUF; i++)
            if (s_mbufs[i] && hle_is(s_mbufs[i], HLE_KIND_BUFFER))
                mix_buffer(s_mbufs[i], clock);
        for (i = 0; i < HLE_MAX_STREAMS; i++)
            if (s_streams[i] && hle_is(s_streams[i], HLE_KIND_STREAM))
                mix_stream(s_streams[i], clock);
        hle_unlock();
        for (i = 0; i < MIX_BLOCK * 2; i++) {
            float v = s_acc[i] * 0.5f;
            if (v > 1.0f) v = 1.0f; else if (v < -1.0f) v = -1.0f;
            pcm[next][i] = (int16_t)(v * 32767.0f);
        }
        {
            /* CW_AUDIO_WAV=<file>: also write the mixed output as a WAV file
             * (header sizes refreshed every block), to check sound without
             * listening. */
            static FILE *wav;
            static int tried;
            static uint32_t bytes;
            if (!tried) {
                const char *e = getenv("CW_AUDIO_WAV");
                tried = 1;
                if (e && *e && (wav = fopen(e, "wb")) != NULL) {
                    static const uint8_t hdr0[44] = { 'R','I','F','F',0,0,0,0,'W','A','V','E','f','m','t',' ',
                        16,0,0,0, 1,0, 2,0, 0x44,0xAC,0,0, 0x10,0xB1,0x02,0, 4,0, 16,0, 'd','a','t','a',0,0,0,0 };
                    fwrite(hdr0, 1, 44, wav);
                }
            }
            if (wav) {
                uint32_t v;
                fwrite(pcm[next], 1, sizeof pcm[next], wav);
                bytes += (uint32_t)sizeof pcm[next];
                v = bytes + 36; fseek(wav, 4, SEEK_SET); fwrite(&v, 4, 1, wav);
                fseek(wav, 40, SEEK_SET); fwrite(&bytes, 4, 1, wav);
                fseek(wav, 0, SEEK_END); fflush(wav);
            }
        }
        {
            /* Once a minute: how full the object tables are. */
            static DWORD last;
            DWORD t = GetTickCount();
            if (t - last > 60000) {
                int k, live = 0, used = 0;
                last = t;
                for (k = 0; k < MIX_MAXFMT; k++) if (s_mfmt[k].obj) {
                    used++;
                    if (hle_is(s_mfmt[k].obj, HLE_KIND_BUFFER) || hle_is(s_mfmt[k].obj, HLE_KIND_STREAM)) live++;
                }
                fprintf(stderr, "[AUDIO] format slots %d used, %d live, of %d\n", used, live, MIX_MAXFMT);
                fflush(stderr);
            }
        }
        clock += MIX_BLOCK * 1000.0 / MIX_RATE;
        waveOutWrite(wo, h, sizeof *h);
        next = (next + 1) % MIX_NBUF;
    }
}

static void mix_start(void)
{
    if (s_mix_on >= 0) return;
    {
        const char *e = getenv("CW_AUDIO");
        s_mix_on = !(e && *e == '0');
    }
    if (s_mix_on)
        CloseHandle(CreateThread(NULL, 0, mix_thread, NULL, 0, NULL));
}
