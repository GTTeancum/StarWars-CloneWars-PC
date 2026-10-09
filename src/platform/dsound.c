/*
 * dsound.c -- silent high-level replacement for the title's DirectSound.
 *
 * Clone Wars links Microsoft's Xbox DirectSound into its own DSOUND section,
 * and that code drives the MCPX audio processor directly. The runtime has no
 * APU, so DirectSoundCreate reported DSERR_NODRIVER, every later call failed,
 * the title's sound manager gave up before it loaded its sound categories,
 * and the front end showed the dirty-disc screen on the missing "music"
 * category.
 *
 * This replaces the DirectSound entry points the title calls with an
 * implementation that behaves like the real one without producing sound:
 *
 *   - objects live in guest memory, so pointers the title keeps stay valid;
 *   - a buffer reports DSBSTATUS_PLAYING for as long as its data would take
 *     to play, then stops, so voice management sees sounds finish;
 *   - a stream completes packets on the same clock -- completed size, packet
 *     status, and the event or callback the title attached -- so music and
 *     voice-over code that waits on packets keeps moving;
 *   - the DSP effects image "downloads" with an empty effect table.
 *
 * Function addresses and argument counts come from XbSymbolDatabase's scan of
 * this XBE (XDK 5233) and were checked against each function's `ret n`.
 * Output to the host is a later step: the buffer and stream state here is
 * what a mixer would read.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "recomp/gen/recomp_types.h"
#include "recomp/gen/recomp_funcs.h"
#include "xbox_memory_layout.h"

extern int xbox_hle_set_event(uint32_t guest_handle);
extern void (*volatile xbox_hle_periodic_hook)(void);
extern unsigned long __stdcall GetTickCount(void);

/* Host output (dsound_mix.inc, included at the end). */
static void mix_note_format(uint32_t obj, uint32_t wfx);
static void mix_track_buffer(uint32_t b, int add);
static void mix_start(void);
static int mix_can_play(uint32_t s);

/* ---- constants -------------------------------------------------------- */

#define DS_OK                   0x00000000u
#define DSERR_INVALIDCALL       0x88780032u
#define DSERR_OUTOFMEMORY       0x8007000Eu
#define E_PENDING_HR            0x8000000Au
#define E_ABORT_HR              0x80004004u

#define DSBPLAY_LOOPING         0x00000001u
#define DSBSTATUS_PLAYING       0x00000001u
#define DSBSTATUS_LOOPING       0x00000004u

#define XMO_STATUSF_ACCEPT_INPUT_DATA 0x00000001u
#define DSSTREAMSTATUS_READY    0x00000001u
#define DSSTREAMSTATUS_PLAYING  0x00010000u
#define DSSTREAMSTATUS_PAUSED   0x00020000u
#define DSSTREAMSTATUS_STARVED  0x00040000u
/* Private flag: paused until IDirectSound_SynchPlayback (DSSTREAMPAUSE_SYNCHPLAYBACK). */
#define HLE_STREAM_DRAINING     0x20000000u
#define HLE_STREAM_SYNCH_WAIT   0x40000000u

#define HLE_MAGIC               0x534C4448u   /* "HDLS" */
#define HLE_KIND_DEVICE         1u
#define HLE_KIND_BUFFER         2u
#define HLE_KIND_STREAM         3u

#define HLE_MAX_PACKETS         64

/* Guest object layout. Only this file reads it; the title treats the
 * pointers it is handed as opaque, apart from the stream's XMediaObject
 * vtable at +0. */
enum {
    O_VTBL      = 0x00,
    O_MAGIC     = 0x04,
    O_KIND      = 0x08,
    O_REFS      = 0x0C,
    O_STATUS    = 0x10,   /* playing / looping / paused flags */
    O_VOLUME    = 0x14,
    O_FREQ      = 0x18,
    O_BPS       = 0x1C,   /* average bytes per second of the format */
    O_DATA      = 0x20,   /* buffer: title-owned sample data */
    O_BYTES     = 0x24,   /* buffer: data size */
    O_PSTART    = 0x28,   /* buffer: play region */
    O_PLEN      = 0x2C,
    O_T0        = 0x30,   /* tick the current play (or packet) began */
    O_CB        = 0x34,   /* stream: completion callback */
    O_CBCTX     = 0x38,   /* stream: callback stream context */
    O_NPKT      = 0x3C,   /* stream: queued packet count */
    O_PKT       = 0x40,   /* stream: queued XMEDIAPACKET copies */
    O_MAXPKT    = O_PKT + HLE_MAX_PACKETS * 0x18,   /* stream: dwMaxAttachedPackets */
    O_SIZE      = O_MAXPKT + 4
};

static void hle_stream_track(uint32_t s, int add);

/* Stream packet counters, logged from DirectSoundDoWork. */
static unsigned s_pkt_in, s_pkt_done, s_pkt_flushed, s_cb_calls;

/* ---- plumbing --------------------------------------------------------- */

/* CW_AUDIO_LOG=1: one line per buffer play/stop and stream packet/pause, with
 * the time, to see what the title asks for. */
static int alog_on(void)
{
    static int on = -1;
    if (on < 0) { const char *e = getenv("CW_AUDIO_LOG"); on = e && *e == '1'; }
    return on;
}
#define ALOG(...) do { if (alog_on()) { fprintf(stderr, "[ALOG] %u ", (unsigned)GetTickCount());     fprintf(stderr, __VA_ARGS__); fputc(10, stderr); } } while (0)


static void hle_log(const char *fmt, ...)
{
    static int budget = 400;
    va_list ap;
    if (budget <= 0 || !getenv("CW_HLE_LOG"))
        return;
    budget--;
    va_start(ap, fmt);
    fprintf(stderr, "[HLE-DSOUND] ");
    vfprintf(stderr, fmt, ap);
    fputc('\n', stderr);
    va_end(ap);
}

#define ARG(n)       MEM32(g_esp + 4 + 4 * (n))
/* stdcall: the callee pops its return address and its arguments. */
#define RET(nargs)   do { g_esp += 4 + 4 * (nargs); return; } while (0)

static uint32_t hle_new(uint32_t kind, uint32_t vtbl)
{
    uint32_t o = xbox_HeapAlloc(O_SIZE, 16), i;
    if (!o)
        return 0;
    for (i = 0; i < O_SIZE; i += 4)
        MEM32(o + i) = 0;
    MEM32(o + O_VTBL)  = vtbl;
    MEM32(o + O_MAGIC) = HLE_MAGIC;
    MEM32(o + O_KIND)  = kind;
    MEM32(o + O_REFS)  = 1;
    MEM32(o + O_FREQ)  = 48000;
    MEM32(o + O_BPS)   = 48000 * 4;
    return o;
}

static int hle_is(uint32_t o, uint32_t kind)
{
    return o >= 0x10000u && o < 0x04000000u &&
           MEM32(o + O_MAGIC) == HLE_MAGIC && MEM32(o + O_KIND) == kind;
}

/* WAVEFORMATEX: +0 tag, +2 channels, +4 rate, +8 avg bytes/s, +12 align. */
static void hle_set_format(uint32_t o, uint32_t wfx)
{
    uint32_t rate, bps;
    if (!wfx)
        return;
    mix_note_format(o, wfx);
    rate = MEM32(wfx + 4);
    bps  = MEM32(wfx + 8);
    if (rate)
        MEM32(o + O_FREQ) = rate;
    if (bps)
        MEM32(o + O_BPS) = bps;
}

/* Milliseconds a span of bytes takes to play at the object's rate. ADPCM
 * reports its real average bytes per second, so this holds for it too. */
static uint32_t hle_ms(uint32_t o, uint32_t bytes)
{
    uint32_t bps = MEM32(o + O_BPS);
    if (!bps)
        bps = 48000 * 4;
    return (uint32_t)(((uint64_t)bytes * 1000u) / bps);
}

/* Call a title function: stdcall with up to three arguments. */
static void hle_call_guest(uint32_t fn_va, uint32_t a0, uint32_t a1, uint32_t a2)
{
    recomp_func_t fn = recomp_lookup_manual(fn_va);
    uint32_t saved_esp = g_esp;
    if (!fn) fn = recomp_lookup(fn_va);
    if (!fn) {
        hle_log("callback 0x%08X is not code", fn_va);
        return;
    }
    PUSH32(g_esp, a2);
    PUSH32(g_esp, a1);
    PUSH32(g_esp, a0);
    PUSH32(g_esp, 0);          /* return address */
    fn();
    g_esp = saved_esp;         /* whatever convention it used */
}

/* ---- synthetic entry points for vtables ------------------------------- */

/* Stream objects need a real XMediaObject vtable because the title calls
 * Process/GetStatus through it. Its entries are synthetic addresses above
 * 0xFE000000 -- RECOMP_ICALL_IS_CODE accepts that range -- which
 * hle_dsound_lookup() below resolves. */
#define HLE_VA_BASE        0xFE400000u
#define HLE_VA(slot)       (HLE_VA_BASE + (slot) * 0x10u)
enum {
    SLOT_S_ADDREF, SLOT_S_RELEASE, SLOT_S_GETINFO, SLOT_S_GETSTATUS,
    SLOT_S_PROCESS, SLOT_S_DISCONTINUITY, SLOT_S_FLUSH, SLOT_GENERIC_OK1,
    SLOT_COUNT
};

static uint32_t s_stream_vtbl;
static uint32_t s_plain_vtbl;

static uint32_t hle_vtables_init(void)
{
    uint32_t i;
    if (s_stream_vtbl)
        return 1;
    s_stream_vtbl = xbox_HeapAlloc(16 * 4, 16);
    s_plain_vtbl  = xbox_HeapAlloc(16 * 4, 16);
    if (!s_stream_vtbl || !s_plain_vtbl)
        return 0;
    for (i = 0; i < 16; i++) {
        MEM32(s_stream_vtbl + i * 4) = HLE_VA(SLOT_GENERIC_OK1);
        MEM32(s_plain_vtbl + i * 4)  = HLE_VA(SLOT_GENERIC_OK1);
    }
    MEM32(s_stream_vtbl + 0x00) = HLE_VA(SLOT_S_ADDREF);
    MEM32(s_stream_vtbl + 0x04) = HLE_VA(SLOT_S_RELEASE);
    MEM32(s_stream_vtbl + 0x08) = HLE_VA(SLOT_S_GETINFO);
    MEM32(s_stream_vtbl + 0x0C) = HLE_VA(SLOT_S_GETSTATUS);
    MEM32(s_stream_vtbl + 0x10) = HLE_VA(SLOT_S_PROCESS);
    MEM32(s_stream_vtbl + 0x14) = HLE_VA(SLOT_S_DISCONTINUITY);
    MEM32(s_stream_vtbl + 0x18) = HLE_VA(SLOT_S_FLUSH);
    return 1;
}

/* ---- streams: packet completion --------------------------------------- */

/* XMEDIAPACKET: +0 pvBuffer, +4 dwMaxSize, +8 pdwCompletedSize,
 * +0xC pdwStatus, +0x10 hCompletionEvent / pContext, +0x14 prtTimestamp. */
#define PKT_SIZE 0x18
#define PKT_AT(s, i) ((s) + O_PKT + (i) * PKT_SIZE)

/* Process copies the packet, as DirectSound does: titles build it in a stack
 * local (the XMV player does), so a kept pointer reads a dead frame by the
 * time the packet completes. `pkt` is a host copy of the six fields. */
#define CBQ_SIZE 256
static uint32_t s_cbq[CBQ_SIZE][4];
static volatile uint32_t s_cbq_head, s_cbq_tail;

static void hle_packet_finish(uint32_t s, const uint32_t *pkt, uint32_t status)
{
    uint32_t pcompleted = pkt[2], pstatus = pkt[3];
    uint32_t done = (status == DS_OK) ? pkt[1] : 0;

    if (pcompleted) MEM32(pcompleted) = done;
    if (pstatus)    MEM32(pstatus) = status;
    if (status == DS_OK) s_pkt_done++; else s_pkt_flushed++;
    if (MEM32(s + O_CB)) {
        /* Queued: the callback is game code (the video player's audio
         * refill) and can run for tens of milliseconds. Running it with the
         * sound lock held stalled the host mixer and the timer thread --
         * audible as on/off gaps. hle_unlock runs the queue once the lock is
         * released. */
        uint32_t n = (s_cbq_tail + 1) % CBQ_SIZE;
        s_cb_calls++;
        if (n != s_cbq_head) {
            s_cbq[s_cbq_tail][0] = MEM32(s + O_CB);
            s_cbq[s_cbq_tail][1] = MEM32(s + O_CBCTX);
            s_cbq[s_cbq_tail][2] = pkt[4];
            s_cbq[s_cbq_tail][3] = status;
            s_cbq_tail = n;
        }
    } else if (pkt[4])
        xbox_hle_set_event(pkt[4]);
}

/* Remove the head packet into out[6]. */
static void hle_stream_pop(uint32_t s, uint32_t *out)
{
    uint32_t n = MEM32(s + O_NPKT), i, k;
    for (k = 0; k < 6; k++)
        out[k] = MEM32(PKT_AT(s, 0) + 4 * k);
    for (i = 1; i < n; i++)
        for (k = 0; k < 6; k++)
            MEM32(PKT_AT(s, i - 1) + 4 * k) = MEM32(PKT_AT(s, i) + 4 * k);
    MEM32(s + O_NPKT) = n - 1;
}

/* Complete every packet whose play time has elapsed. */
/* Set by the host mixer once its output is running: stream packets then
 * complete when the mixer has actually played them (dsound_mix.inc), the
 * way the hardware completes them. Completing on a wall clock while the mixer
 * rendered ahead of it meant the next packet was not queued yet when the mixer
 * got there -- regular dropouts during movies and music. */
static volatile int s_mixer_drives_streams;

static void hle_stream_advance(uint32_t s)
{
    uint32_t now = (uint32_t)GetTickCount();
    if (MEM32(s + O_STATUS) & DSSTREAMSTATUS_PAUSED)
        return;
    if (s_mixer_drives_streams && mix_can_play(s)) {
        if (!MEM32(s + O_NPKT) && (MEM32(s + O_STATUS) & HLE_STREAM_DRAINING)) {
            hle_stream_track(s, 0);
            MEM32(s + O_MAGIC) = 0;
            ALOG("stream %08X drained and retired", s);
        }
        return;
    }
    while (MEM32(s + O_NPKT)) {
        uint32_t pkt[6];
        uint32_t need = hle_ms(s, MEM32(PKT_AT(s, 0) + 4));
        if (now - MEM32(s + O_T0) < need)
            break;
        MEM32(s + O_T0) += need;
        hle_stream_pop(s, pkt);
        hle_packet_finish(s, pkt, DS_OK);
    }
    if (!MEM32(s + O_NPKT)) {
        MEM32(s + O_T0) = now;     /* starved: the next packet starts now */
        if (MEM32(s + O_STATUS) & HLE_STREAM_DRAINING) {
            hle_stream_track(s, 0);
            MEM32(s + O_MAGIC) = 0;
            ALOG("stream %08X drained and retired", s);
        }
    }
}

static void hle_stream_flush(uint32_t s)
{
    if (MEM32(s + O_NPKT)) ALOG("stream %08X flush with %u packets queued", s, MEM32(s + O_NPKT));
    while (MEM32(s + O_NPKT)) {
        uint32_t pkt[6];
        hle_stream_pop(s, pkt);
        hle_packet_finish(s, pkt, E_ABORT_HR);   /* XMEDIAPACKET_STATUS_FLUSHED */
    }
    MEM32(s + O_T0) = (uint32_t)GetTickCount();
}

/* A stream released with packets still queued keeps playing them out, as the
 * title expects: it releases a voice-over stream right after submitting the
 * file's last packet, with seconds of speech queued. The title no longer owns
 * the object, so completions are not reported back (their status words and
 * events may already be gone); the stream retires once it drains. */

static int hle_stream_release_last(uint32_t s)
{
    uint32_t i, n = MEM32(s + O_NPKT);
    if (!n)
        return 0;
    for (i = 0; i < n; i++) {
        MEM32(PKT_AT(s, i) + 8) = 0;      /* pdwCompletedSize */
        MEM32(PKT_AT(s, i) + 0xC) = 0;    /* pdwStatus */
        MEM32(PKT_AT(s, i) + 0x10) = 0;   /* event / context */
    }
    MEM32(s + O_CB) = 0;
    MEM32(s + O_STATUS) |= HLE_STREAM_DRAINING;
    MEM32(s + O_STATUS) &= ~(DSSTREAMSTATUS_PAUSED | HLE_STREAM_SYNCH_WAIT);
    ALOG("stream %08X released with %u packets queued: draining", s, n);
    return 1;
}

/* Every live stream, for DirectSoundDoWork. */
#define HLE_MAX_STREAMS 64
static uint32_t s_streams[HLE_MAX_STREAMS];

static void hle_stream_track(uint32_t s, int add)
{
    int i;
    for (i = 0; i < HLE_MAX_STREAMS; i++) {
        if (add && !s_streams[i]) { s_streams[i] = s; return; }
        if (!add && s_streams[i] == s) { s_streams[i] = 0; return; }
    }
}

/* Streams complete on a background clock (hle_dsound_periodic, run by the
 * kernel timer thread) as well as from the title's own calls, the way the
 * real library completes packets from its DPC. One recursive lock covers
 * every path that touches stream state: a completion callback may call back
 * into the stream. */
static CRITICAL_SECTION s_lock;
static CRITICAL_SECTION s_cb_lock;
static volatile LONG s_lock_ready;

static void hle_lock_init(void)
{
    if (!s_lock_ready) {
        InitializeCriticalSection(&s_cb_lock);
        InitializeCriticalSection(&s_lock);
        s_lock_ready = 1;
    }
}
/* Hold-time accounting (CW_AUDIO_LOG): who keeps the lock > 30 ms. */
static DWORD s_lock_t0, s_lock_depth;
static const char *s_lock_who;
static void hle_lock_tag(const char *who)
{
    if (!s_lock_ready) return;
    EnterCriticalSection(&s_lock);
    if (s_lock_depth++ == 0) { s_lock_t0 = GetTickCount(); s_lock_who = who; }
}
static void hle_lock(void) { hle_lock_tag("?"); }
/* Completion callbacks run here, after the sound lock is released, one at a
 * time (s_cb_lock) and never nested on one thread. The timer thread only
 * tries: it must not wait on game code. */
static __declspec(thread) int s_in_callbacks;

static void hle_run_callbacks(int may_wait)
{
    if (s_in_callbacks || s_cbq_head == s_cbq_tail)
        return;
    if (may_wait) EnterCriticalSection(&s_cb_lock);
    else if (!TryEnterCriticalSection(&s_cb_lock)) return;
    s_in_callbacks = 1;
    for (;;) {
        uint32_t c[4];
        EnterCriticalSection(&s_lock);
        if (s_cbq_head == s_cbq_tail) { LeaveCriticalSection(&s_lock); break; }
        memcpy(c, s_cbq[s_cbq_head], sizeof c);
        s_cbq_head = (s_cbq_head + 1) % CBQ_SIZE;
        LeaveCriticalSection(&s_lock);
        hle_call_guest(c[0], c[1], c[2], c[3]);
    }
    s_in_callbacks = 0;
    LeaveCriticalSection(&s_cb_lock);
}

static __declspec(thread) int s_unlock_no_wait;
static void hle_unlock(void)
{
    int outer;
    if (!s_lock_ready) return;
    outer = (--s_lock_depth == 0);
    if (outer) {
        DWORD held = GetTickCount() - s_lock_t0;
        if (held > 30 && alog_on())
            fprintf(stderr, "[ALOCK] %s held the sound lock %lu ms (thread %lu)\n", s_lock_who,
                    (unsigned long)held, (unsigned long)GetCurrentThreadId());
    }
    LeaveCriticalSection(&s_lock);
    if (outer)
        hle_run_callbacks(!s_unlock_no_wait);
}

static void hle_dsound_periodic(void)
{
    int i;
    /* Timer thread: never wait. Skip the pass if the lock is busy; run
     * callbacks only if nobody else is running them. */
    if (!s_lock_ready || !TryEnterCriticalSection(&s_lock))
        return;
    if (s_lock_depth++ == 0) { s_lock_t0 = GetTickCount(); s_lock_who = "periodic"; }
    for (i = 0; i < HLE_MAX_STREAMS; i++)
        if (s_streams[i] && hle_is(s_streams[i], HLE_KIND_STREAM))
            hle_stream_advance(s_streams[i]);
    s_unlock_no_wait = 1;
    hle_unlock();
    s_unlock_no_wait = 0;
}

/* ---- XMediaObject methods (stream vtable) ----------------------------- */

static void s_addref(void)
{
    uint32_t s = ARG(0);
    if (hle_is(s, HLE_KIND_STREAM))
        MEM32(s + O_REFS)++;
    g_eax = hle_is(s, HLE_KIND_STREAM) ? MEM32(s + O_REFS) : 0;
    RET(1);
}

static void s_release_body(void)
{
    uint32_t s = ARG(0), refs = 0;
    ALOG("stream %08X Release ret %08X", s, MEM32(g_esp));
    if (hle_is(s, HLE_KIND_STREAM)) {
        refs = --MEM32(s + O_REFS);
        if (!refs && !hle_stream_release_last(s)) {
            hle_stream_flush(s);
            hle_stream_track(s, 0);
            MEM32(s + O_MAGIC) = 0;   /* guest heap has no free for this; retire it */
        }
    }
    g_eax = refs;
    RET(1);
}
static void s_release(void) { hle_lock_tag("release"); s_release_body(); hle_unlock(); }

/* XMEDIAINFO: dwFlags, dwInputSize, dwOutputSize, dwMaxLookahead. */
static void s_getinfo(void)
{
    uint32_t info = ARG(1);
    if (info) {
        MEM32(info + 0) = 0;
        MEM32(info + 4) = 0;
        MEM32(info + 8) = 0;
        MEM32(info + 12) = 0;
    }
    g_eax = DS_OK;
    RET(2);
}

static void s_getstatus_body(void)
{
    uint32_t s = ARG(0), pdw = ARG(1), st = 0;
    /* DSSTREAMSTATUS_*: READY (= XMO accept-input) while a packet slot is
     * free, PLAYING while packets are queued and not paused, PAUSED, STARVED
     * when running dry. The title's voice-over player waits for PLAYING to
     * clear after its last packet before it tears the line down and starts
     * the next one, so reporting only READY cut every line off at once. */
    if (hle_is(s, HLE_KIND_STREAM)) {
        uint32_t maxp = MEM32(s + O_MAXPKT) ? MEM32(s + O_MAXPKT) : HLE_MAX_PACKETS;
        hle_stream_advance(s);
        if (MEM32(s + O_NPKT) < maxp && MEM32(s + O_NPKT) < HLE_MAX_PACKETS)
            st |= XMO_STATUSF_ACCEPT_INPUT_DATA;
        if (MEM32(s + O_STATUS) & DSSTREAMSTATUS_PAUSED)
            st |= DSSTREAMSTATUS_PAUSED;
        else if (MEM32(s + O_NPKT))
            st |= DSSTREAMSTATUS_PLAYING;
        else if (MEM32(s + O_STATUS) & DSSTREAMSTATUS_PLAYING)
            st |= DSSTREAMSTATUS_STARVED;
    }
    if (pdw)
        MEM32(pdw) = st;
    g_eax = DS_OK;
    RET(2);
}
static void s_getstatus(void) { hle_lock_tag("getstatus"); s_getstatus_body(); hle_unlock(); }

static void s_process_body(void)
{
    uint32_t s = ARG(0), in = ARG(1);
    g_eax = DS_OK;
    if (!hle_is(s, HLE_KIND_STREAM) || !in) {
        g_eax = DSERR_INVALIDCALL;
        RET(3);
    }
    hle_stream_advance(s);
    if (MEM32(s + O_NPKT) >= HLE_MAX_PACKETS ||
        (MEM32(s + O_MAXPKT) && MEM32(s + O_NPKT) >= MEM32(s + O_MAXPKT))) {
        ALOG("stream %08X Process REJECTED: %u queued, max %u", s, MEM32(s + O_NPKT), MEM32(s + O_MAXPKT));
        g_eax = DSERR_INVALIDCALL;
        RET(3);
    }
    if (MEM32(in + 0xC))
        MEM32(MEM32(in + 0xC)) = E_PENDING_HR;     /* XMEDIAPACKET_STATUS_PENDING */
    if (MEM32(in + 8))
        MEM32(MEM32(in + 8)) = 0;
    if (!MEM32(s + O_NPKT))
        MEM32(s + O_T0) = (uint32_t)GetTickCount();
    {
        uint32_t k, slot = PKT_AT(s, MEM32(s + O_NPKT));
        for (k = 0; k < 6; k++)
            MEM32(slot + 4 * k) = MEM32(in + 4 * k);
    }
    if (s_pkt_in < 12)
        hle_log("Process: stream 0x%08X packet %u bytes = %u ms (rate %u, %u B/s)",
                s, MEM32(in + 4), hle_ms(s, MEM32(in + 4)), MEM32(s + O_FREQ), MEM32(s + O_BPS));
    ALOG("stream %08X packet %u bytes (queued %u) rate %u bps %u", s, MEM32(in + 4), MEM32(s + O_NPKT) + 1,
         MEM32(s + O_FREQ), MEM32(s + O_BPS));
    s_pkt_in++;
    MEM32(s + O_NPKT)++;
    MEM32(s + O_STATUS) |= DSSTREAMSTATUS_PLAYING;
    RET(3);
}
static void s_process(void) { hle_lock_tag("process"); s_process_body(); hle_unlock(); }

static void s_discontinuity(void)
{
    g_eax = DS_OK;
    RET(1);
}

static void s_flush_body(void)
{
    uint32_t s = ARG(0);
    ALOG("stream %08X Flush() ret %08X", s, MEM32(g_esp));
    if (hle_is(s, HLE_KIND_STREAM))
        hle_stream_flush(s);
    g_eax = DS_OK;
    RET(1);
}
static void s_flush(void) { hle_lock_tag("flush"); s_flush_body(); hle_unlock(); }

/* A vtable slot nothing here implements. It cannot know the argument count,
 * so it pops only the return address and says so. */
static void generic_ok(void)
{
    hle_log("unimplemented vtable slot called (ret=0x%08X)", MEM32(g_esp));
    g_eax = DS_OK;
    g_esp += 4;
}

/* ---- device ----------------------------------------------------------- */

static uint32_t s_device;

/* 0x002F6D1B DirectSoundCreate(pguid, ppDirectSound, pUnk) */
void sub_002F6D1B(void)
{
    hle_lock_init();
    xbox_hle_periodic_hook = hle_dsound_periodic;
    mix_start();
    uint32_t pp = ARG(1);
    hle_vtables_init();
    if (!s_device)
        s_device = hle_new(HLE_KIND_DEVICE, s_plain_vtbl);
    else
        MEM32(s_device + O_REFS)++;
    if (pp)
        MEM32(pp) = s_device;
    g_eax = s_device ? DS_OK : DSERR_OUTOFMEMORY;
    hle_log("DirectSoundCreate -> 0x%08X", s_device);
    RET(3);
}

/* 0x002F49B5 IDirectSound_Release(pThis) */
void sub_002F49B5(void)
{
    uint32_t d = ARG(0);
    g_eax = 0;
    if (hle_is(d, HLE_KIND_DEVICE) && MEM32(d + O_REFS))
        g_eax = --MEM32(d + O_REFS);
    RET(1);
}

/* 0x002F49E1 DirectSoundUseLightHRTF(void) */
void sub_002F49E1(void) { RET(0); }

/* 0x002F4A09 DirectSoundOverrideSpeakerConfig(dwSpeakerConfig) */
void sub_002F4A09(void) { g_eax = DS_OK; RET(1); }

/* 0x002F578B IDirectSound_GetSpeakerConfig(pThis, pdwSpeakerConfig) */
void sub_002F578B(void)
{
    if (ARG(1))
        MEM32(ARG(1)) = 0x00000002u;   /* DSSPEAKER_STEREO */
    g_eax = DS_OK;
    RET(2);
}

/* 0x002F57A7 IDirectSound_DownloadEffectsImage(pThis, pvImage, dwSize,
 *            pImageLoc, ppImageDesc)
 *
 * DSEFFECTIMAGEDESC is { dwEffectCount, dwTotalScratchSize, aEffectMaps[] };
 * an empty one tells the title there are no DSP effects to address. */
void sub_002F57A7(void)
{
    static uint32_t desc;
    uint32_t pp = ARG(4);
    if (!desc) {
        desc = xbox_HeapAlloc(0x40, 16);
        if (desc) {
            uint32_t i;
            for (i = 0; i < 0x40; i += 4)
                MEM32(desc + i) = 0;
        }
    }
    if (pp)
        MEM32(pp) = desc;
    hle_log("DownloadEffectsImage size=%u -> empty effect table", ARG(2));
    g_eax = desc ? DS_OK : DSERR_OUTOFMEMORY;
    RET(5);
}

/* Device state the title sets and never reads back. */
void sub_002F57CE(void) { g_eax = DS_OK; RET(3); }   /* SetMixBinHeadroom   */
/* 0x002F57EE IDirectSound_SynchPlayback(pThis): start every stream that was
 * paused with DSSTREAMPAUSE_SYNCHPLAYBACK, together. The title's video player
 * queues its audio this way and waits for the packets to complete, so ignoring
 * it left the intro movie waiting forever. Defined after the stream helpers. */
static void hle_synch_playback(void);
void sub_002F57EE(void) { hle_synch_playback(); g_eax = DS_OK; RET(1); }
void sub_002F6075(void) { g_eax = DS_OK; RET(1); }   /* CommitDeferredSettings */
void sub_002F67F6(void) { g_eax = DS_OK; RET(3); }   /* SetAllParameters    */
void sub_002F6816(void) { g_eax = DS_OK; RET(3); }   /* SetI3DL2Listener    */

/* 0x002F5927 DirectSoundDoWork(void) -- the title's periodic audio tick. */
static void sub_002F5927_body(void)
{
    int i;
    static uint32_t last_log;
    uint32_t now = (uint32_t)GetTickCount();
    if (now - last_log > 2000u) {
        last_log = now;
        hle_log("DoWork: packets in %u, completed %u, flushed %u, callbacks %u",
                s_pkt_in, s_pkt_done, s_pkt_flushed, s_cb_calls);
    }
    for (i = 0; i < HLE_MAX_STREAMS; i++)
        if (s_streams[i] && hle_is(s_streams[i], HLE_KIND_STREAM))
            hle_stream_advance(s_streams[i]);
    RET(0);
}
void sub_002F5927(void) { hle_lock_tag("dowork"); sub_002F5927_body(); hle_unlock(); }

/* ---- buffers ---------------------------------------------------------- */

/* DSBUFFERDESC: dwSize, dwFlags, dwBufferBytes, lpwfxFormat, lpMixBins,
 * dwInputMixBin. */
/* 0x002F6B51 IDirectSound_CreateSoundBuffer(pThis, pdsbd, ppBuffer, pUnk) */
void sub_002F6B51(void)
{
    uint32_t desc = ARG(1), pp = ARG(2), b;
    hle_vtables_init();
    b = hle_new(HLE_KIND_BUFFER, s_plain_vtbl);
    if (b && desc) {
        MEM32(b + O_BYTES) = MEM32(desc + 8);
        hle_set_format(b, MEM32(desc + 12));
    }
    if (b)
        mix_track_buffer(b, 1);
    if (pp)
        MEM32(pp) = b;
    g_eax = b ? DS_OK : DSERR_OUTOFMEMORY;
    RET(4);
}

/* 0x002F49CB IDirectSoundBuffer_Release(pThis) */
void sub_002F49CB(void)
{
    uint32_t b = ARG(0);
    g_eax = 0;
    if (hle_is(b, HLE_KIND_BUFFER) && MEM32(b + O_REFS)) {
        g_eax = --MEM32(b + O_REFS);
        if (!g_eax) {
            MEM32(b + O_MAGIC) = 0;
            mix_track_buffer(b, 0);
        }
    } else if (hle_is(b, HLE_KIND_STREAM)) {
        /* The title releases streams through the same wrapper. */
        ALOG("stream %08X Release(buf wrapper) ret %08X", b, MEM32(g_esp));
        uint32_t refs = --MEM32(b + O_REFS);
        if (!refs && !hle_stream_release_last(b)) {
            hle_stream_flush(b);
            hle_stream_track(b, 0);
            MEM32(b + O_MAGIC) = 0;
        }
        g_eax = refs;
    }
    RET(1);
}

/* Remaining play time of a one-shot buffer, 0 once it has finished. */
static int hle_buffer_playing(uint32_t b)
{
    uint32_t st = MEM32(b + O_STATUS), len;
    if (!(st & DSBSTATUS_PLAYING))
        return 0;
    if (st & DSBSTATUS_LOOPING)
        return 1;
    len = MEM32(b + O_PLEN) ? MEM32(b + O_PLEN) : MEM32(b + O_BYTES);
    if ((uint32_t)GetTickCount() - MEM32(b + O_T0) >= hle_ms(b, len)) {
        MEM32(b + O_STATUS) = 0;
        return 0;
    }
    return 1;
}

/* 0x002F585A IDirectSoundBuffer_Play(pThis, r1, r2, dwFlags) */
void sub_002F585A(void)
{
    uint32_t b = ARG(0), flags = ARG(3);
    if (hle_is(b, HLE_KIND_BUFFER)) {
        ALOG("buf %08X play flags %X data %08X bytes %u plen %u rate %u bps %u vol %d", b, flags,
             MEM32(b + O_DATA), MEM32(b + O_BYTES), MEM32(b + O_PLEN), MEM32(b + O_FREQ), MEM32(b + O_BPS),
             (int)MEM32(b + O_VOLUME));
        MEM32(b + O_STATUS) = DSBSTATUS_PLAYING |
                              ((flags & DSBPLAY_LOOPING) ? DSBSTATUS_LOOPING : 0);
        MEM32(b + O_T0) = (uint32_t)GetTickCount();
    }
    g_eax = DS_OK;
    RET(4);
}

/* 0x002F587E IDirectSoundBuffer_Stop(pThis) */
void sub_002F587E(void)
{
    if (hle_is(ARG(0), HLE_KIND_BUFFER) && (MEM32(ARG(0) + O_STATUS) & DSBSTATUS_PLAYING))
        ALOG("buf %08X stop after %u ms (ret %08X)", ARG(0), (unsigned)GetTickCount() - MEM32(ARG(0) + O_T0), MEM32(g_esp));
    if (hle_is(ARG(0), HLE_KIND_BUFFER))
        MEM32(ARG(0) + O_STATUS) = 0;
    g_eax = DS_OK;
    RET(1);
}

/* 0x002F5896 IDirectSoundBuffer_StopEx(pThis, REFERENCE_TIME rt, dwFlags) */
void sub_002F5896(void)
{
    if (hle_is(ARG(0), HLE_KIND_BUFFER) && (MEM32(ARG(0) + O_STATUS) & DSBSTATUS_PLAYING))
        ALOG("buf %08X stopex after %u ms (ret %08X)", ARG(0), (unsigned)GetTickCount() - MEM32(ARG(0) + O_T0), MEM32(g_esp));
    if (hle_is(ARG(0), HLE_KIND_BUFFER))
        MEM32(ARG(0) + O_STATUS) = 0;
    g_eax = DS_OK;
    RET(4);
}

/* 0x002F58DA IDirectSoundBuffer_GetStatus(pThis, pdwStatus) */
void sub_002F58DA(void)
{
    uint32_t b = ARG(0), pdw = ARG(1), st = 0;
    if (hle_is(b, HLE_KIND_BUFFER) && hle_buffer_playing(b))
        st = MEM32(b + O_STATUS);
    if (pdw)
        MEM32(pdw) = st;
    g_eax = DS_OK;
    RET(2);
}

/* 0x002F6836 IDirectSoundBuffer_SetBufferData(pThis, pvData, dwBytes) */
void sub_002F6836(void)
{
    uint32_t b = ARG(0);
    if (hle_is(b, HLE_KIND_BUFFER)) {
        MEM32(b + O_DATA)   = ARG(1);
        MEM32(b + O_BYTES)  = ARG(2);
        MEM32(b + O_PSTART) = 0;
        MEM32(b + O_PLEN)   = 0;
    }
    g_eax = DS_OK;
    RET(3);
}

/* 0x002F612F IDirectSoundBuffer_SetPlayRegion(pThis, dwStart, dwLength) */
void sub_002F612F(void)
{
    uint32_t b = ARG(0);
    if (hle_is(b, HLE_KIND_BUFFER)) {
        MEM32(b + O_PSTART) = ARG(1);
        MEM32(b + O_PLEN)   = ARG(2);
    }
    g_eax = DS_OK;
    RET(3);
}

/* 0x002F608D IDirectSoundBuffer_SetFormat(pThis, pwfx) */
void sub_002F608D(void)
{
    if (hle_is(ARG(0), HLE_KIND_BUFFER))
        hle_set_format(ARG(0), ARG(1));
    g_eax = DS_OK;
    RET(2);
}

/* 0x002F5806 IDirectSoundBuffer_SetVolume(pThis, lVolume) */
void sub_002F5806(void)
{
    if (hle_is(ARG(0), HLE_KIND_BUFFER))
        MEM32(ARG(0) + O_VOLUME) = ARG(1);
    g_eax = DS_OK;
    RET(2);
}

/* 0x002F60A9 IDirectSoundBuffer_SetFrequency(pThis, dwFrequency)
 *
 * Changing the rate changes how long the data lasts; scale the byte rate so
 * one-shot durations stay right. */
void sub_002F60A9(void)
{
    uint32_t b = ARG(0), f = ARG(1);
    if (hle_is(b, HLE_KIND_BUFFER) && f && MEM32(b + O_FREQ)) {
        MEM32(b + O_BPS) = (uint32_t)(((uint64_t)MEM32(b + O_BPS) * f) / MEM32(b + O_FREQ));
        MEM32(b + O_FREQ) = f;
    }
    g_eax = DS_OK;
    RET(2);
}

/* Buffer state with no effect on timing. */
void sub_002F5822(void) { g_eax = DS_OK; RET(2); }   /* SetHeadroom   */
void sub_002F583E(void) { g_eax = DS_OK; RET(2); }   /* SetMixBins    */
void sub_002F58BA(void) { g_eax = DS_OK; RET(3); }   /* SetLoopRegion */
void sub_002F60C5(void) { g_eax = DS_OK; RET(5); }   /* SetPosition   */
void sub_002F60FA(void) { g_eax = DS_OK; RET(5); }   /* SetVelocity   */

/* ---- streams: creation and C wrappers --------------------------------- */

/* DSSTREAMDESC: dwFlags, dwMaxAttachedPackets, lpwfxFormat, lpfnCallback,
 * lpvContext, lpMixBins. */
/* 0x002F6D62 DirectSoundCreateStream(pdssd, ppStream) */
static void sub_002F6D62_body(void)
{
    uint32_t desc = ARG(0), pp = ARG(1), s;
    hle_vtables_init();
    s = hle_new(HLE_KIND_STREAM, s_stream_vtbl);
    if (s && desc) {
        hle_set_format(s, MEM32(desc + 8));
        MEM32(s + O_CB)    = MEM32(desc + 12);
        MEM32(s + O_CBCTX) = MEM32(desc + 16);
        MEM32(s + O_MAXPKT) = MEM32(desc + 4);
    }
    if (s) {
        MEM32(s + O_T0) = (uint32_t)GetTickCount();
        hle_stream_track(s, 1);
        ALOG("stream %08X created cb %08X maxpkt %u", s, MEM32(s + O_CB), MEM32(s + O_MAXPKT));
    }
    if (pp)
        MEM32(pp) = s;
    hle_log("DirectSoundCreateStream -> 0x%08X callback=0x%08X", s,
            s ? MEM32(s + O_CB) : 0);
    g_eax = s ? DS_OK : DSERR_OUTOFMEMORY;
    RET(2);
}
void sub_002F6D62(void) { hle_lock_tag("createstream"); sub_002F6D62_body(); hle_unlock(); }

/* Stream SetVolume: kept for the mixer (hundredths of a dB). */
void sub_002F58F6(void)
{
    if (hle_is(ARG(0), HLE_KIND_STREAM))
        MEM32(ARG(0) + O_VOLUME) = ARG(1);
    g_eax = DS_OK;
    RET(2);
}
void sub_002F58FB(void) { g_eax = DS_OK; RET(2); }   /* Stream SetHeadroom        */
void sub_002F5900(void) { g_eax = DS_OK; RET(2); }   /* Stream SetMixBins         */
void sub_002F5905(void) { g_eax = DS_OK; RET(2); }   /* Stream SetMixBinVolumes   */
void sub_002F6154(void) { g_eax = DS_OK; RET(5); }   /* Stream SetPosition        */
void sub_002F617D(void) { g_eax = DS_OK; RET(5); }   /* Stream SetVelocity        */

/* 0x002F614F IDirectSoundStream_SetFrequency(pThis, dwFrequency) */
void sub_002F614F(void)
{
    uint32_t s = ARG(0), f = ARG(1);
    if (hle_is(s, HLE_KIND_STREAM) && f && MEM32(s + O_FREQ)) {
        MEM32(s + O_BPS) = (uint32_t)(((uint64_t)MEM32(s + O_BPS) * f) / MEM32(s + O_FREQ));
        MEM32(s + O_FREQ) = f;
    }
    g_eax = DS_OK;
    RET(2);
}

/* 0x002F590A IDirectSoundStream_Pause(pThis, dwPause)
 * dwPause: 0 resume, 1 pause, 2 pause until IDirectSound_SynchPlayback. */
static void sub_002F590A_body(void)
{
    uint32_t s = ARG(0), mode = ARG(1);
    if (hle_is(s, HLE_KIND_STREAM)) {
        if (mode == 0) {
            MEM32(s + O_STATUS) &= ~(DSSTREAMSTATUS_PAUSED | HLE_STREAM_SYNCH_WAIT);
            MEM32(s + O_T0) = (uint32_t)GetTickCount();
        } else {
            MEM32(s + O_STATUS) |= DSSTREAMSTATUS_PAUSED;
            if (mode == 2)
                MEM32(s + O_STATUS) |= HLE_STREAM_SYNCH_WAIT;
        }
        hle_log("Stream 0x%08X pause mode %u", s, mode);
        ALOG("stream %08X pause %u", s, mode);
    }
    g_eax = DS_OK;
    RET(2);
}
void sub_002F590A(void) { hle_lock_tag("pause"); sub_002F590A_body(); hle_unlock(); }

static void hle_synch_playback_body(void)
{
    uint32_t now = (uint32_t)GetTickCount();
    int i;
    for (i = 0; i < HLE_MAX_STREAMS; i++) {
        uint32_t s = s_streams[i];
        if (s && hle_is(s, HLE_KIND_STREAM) && (MEM32(s + O_STATUS) & HLE_STREAM_SYNCH_WAIT)) {
            MEM32(s + O_STATUS) &= ~(DSSTREAMSTATUS_PAUSED | HLE_STREAM_SYNCH_WAIT);
            MEM32(s + O_T0) = now;
            hle_log("SynchPlayback: stream 0x%08X started", s);
        }
    }
}
static void hle_synch_playback(void) { hle_lock_tag("synch"); hle_synch_playback_body(); hle_unlock(); }

/* 0x002F590F IDirectSoundStream_FlushEx(pThis, REFERENCE_TIME rt, dwFlags) */
static void sub_002F590F_body(void)
{
    ALOG("stream %08X FlushEx ret %08X", ARG(0), MEM32(g_esp));
    if (hle_is(ARG(0), HLE_KIND_STREAM))
        hle_stream_flush(ARG(0));
    g_eax = DS_OK;
    RET(4);
}
void sub_002F590F(void) { hle_lock_tag("flushex"); sub_002F590F_body(); hle_unlock(); }

/* ---- synthetic address lookup ----------------------------------------- */

recomp_func_t hle_dsound_lookup(uint32_t va)
{
    if (va < HLE_VA_BASE || va >= HLE_VA(SLOT_COUNT))
        return NULL;
    switch ((va - HLE_VA_BASE) / 0x10u) {
    case SLOT_S_ADDREF:        return s_addref;
    case SLOT_S_RELEASE:       return s_release;
    case SLOT_S_GETINFO:       return s_getinfo;
    case SLOT_S_GETSTATUS:     return s_getstatus;
    case SLOT_S_PROCESS:       return s_process;
    case SLOT_S_DISCONTINUITY: return s_discontinuity;
    case SLOT_S_FLUSH:         return s_flush;
    default:                   return generic_ok;
    }
}

#include "dsound_mix.inc"
