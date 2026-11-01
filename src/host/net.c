/*
 * net.c -- the Xbox network chip, carried over UDP.
 *
 * The title links XNet, which drives the MCPX Ethernet controller directly:
 * registers at 0xFEF00000, and two rings of 8-byte descriptors in contiguous
 * memory, { physical buffer, flags|length }.
 *
 *   TX ring  (reg 0x100, size (reg 0x108 & 0xFFFF) + 1): the driver fills a
 *            descriptor and sets bit 31 to hand it over; bit 16 marks the last
 *            fragment of a frame (a frame that crosses a page takes two). It
 *            then writes 1 to 0x144 and later reclaims every descriptor whose
 *            bit 31 the chip has cleared.
 *   RX ring  (reg 0x104, size (reg 0x108 >> 16) + 1): every descriptor starts
 *            as 0x800007FD, bit 31 = free for the chip. The chip writes a frame
 *            into the buffer and the descriptor as 0x00010000 | length.
 *
 * Interrupts: the ISR (0x002AAB31) masks the chip (reg 0x004 = 0) and queues
 * a DPC; the DPC (0x002AB16B) reads the status at reg 0x000, acknowledges it
 * by writing it back, services both rings and repeats until status reads 0,
 * then unmasks with 0x5F. The aperture here is plain memory, so the write-back
 * cannot clear anything and that loop would never end: sub_002AB16B below is
 * the same loop with the status kept on this side.
 *
 * This file is the chip: net_periodic() runs on the kernel timer thread in
 * guest context (chained from the DirectSound periodic hook), moves frames
 * between the rings and the network, and raises the interrupt.
 *
 * Transport: Ethernet frames inside UDP datagrams. The host is a switch --
 * joiners send everything to it, it learns which peer each MAC is behind and
 * forwards, flooding broadcasts (System Link discovery is broadcast). Only
 * joiner<->host paths ever need to get through a router.
 *
 * Join codes: creating a System Link game (XNetCreateKey) makes this machine
 * the host, picks a code and lists it in the BitTorrent DHT (dht.c); the code
 * goes on the clipboard and in the window title. Starting a search asks for a
 * code; the joiner looks it up, lists itself under the code's join entry so
 * the host can send first (hole punching), and both send HELLO until one gets
 * through. The host's session then shows up in the game's own search list.
 * The host sees the code below the menus (d3d8.c), in the window title and
 * on the clipboard.
 *
 *   CW_NET_HOST=1            host without a code, on CW_NET_PORT (default 34100)
 *   CW_NET_JOIN=addr[:port]  join a host directly, without a code
 *   CW_NET_OFF=1             no network play at all
 *   CW_NET_LOG=1             one line per frame
 *   CW_NET_PCAP=file         every frame, in and out, as a capture file
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <bcrypt.h>
#include <stdarg.h>

#include "recomp/gen/recomp_types.h"
#include "recomp/gen/recomp_funcs.h"
#include "xbox_memory_layout.h"
#include "dht.h"

#define NIC_BASE        0xFEF00000u
#define NIC_IRQ_STATUS  (NIC_BASE + 0x000)
#define NIC_IRQ_MASK    (NIC_BASE + 0x004)
#define NIC_MAC_LO      (NIC_BASE + 0x0A8)
#define NIC_MAC_HI      (NIC_BASE + 0x0AC)
#define NIC_TX_RING     (NIC_BASE + 0x100)
#define NIC_RX_RING     (NIC_BASE + 0x104)
#define NIC_RING_SIZES  (NIC_BASE + 0x108)

#define IRQ_RX          0x02u
#define IRQ_TX          0x10u
#define DESC_OWN        0x80000000u
#define DESC_LAST       0x00010000u
#define RX_VALID        0x00010000u

#define NIC_ISR         0x002AAB31u

#define CONTIG_BASE     0x80000000u
#define CONTIG_SIZE     (64u * 1024u * 1024u)

#define MAX_FRAME       1536
#define MAX_PEERS       16
#define RXQ_LEN         256

/* Datagram: 'C' 'W' version type, then the frame for type FRAME. */
#define PKT_FRAME       0
#define PKT_HELLO       1
#define PKT_WELCOME     2               /* host -> joiner: HELLO received */
#define PKT_VERSION     1

uint32_t xbox_GetConnectedInterrupt(uint32_t vector);
unsigned char xbox_EnterDispatchLevel(void);
void xbox_LeaveDispatchLevel(unsigned char old);

static int s_mode;                       /* 0 off, 1 host, 2 join */
static int s_log;
static int s_ready;                      /* socket open */
static SOCKET s_sock = INVALID_SOCKET;

static struct peer {
    struct sockaddr_in addr;
    DWORD seen;
    uint8_t mac[8][6];
    int nmac;
} s_peers[MAX_PEERS];
static int s_npeers;
static CRITICAL_SECTION s_lock;

static struct { uint16_t len; uint8_t data[MAX_FRAME]; } s_rxq[RXQ_LEN];
static unsigned s_rxq_head, s_rxq_tail;  /* guarded by s_lock */

static volatile LONG s_status;           /* pending interrupt causes */
static uint32_t s_tx_ring, s_rx_ring;    /* ring bases the indices belong to */
static unsigned s_tx_i, s_rx_i;
static uint8_t s_tx_buf[MAX_FRAME];
static unsigned s_tx_len;
static int s_tx_contig;                  /* this frame's buffers are contiguous */

static void local_mac(uint8_t mac[6])
{
    uint32_t lo = MEM32(NIC_MAC_LO), hi = MEM32(NIC_MAC_HI);
    memcpy(mac, &lo, 4);
    memcpy(mac + 4, &hi, 2);
}

static uint32_t phys_to_va(uint32_t p)
{
    return p < CONTIG_SIZE ? p + CONTIG_BASE : p;
}

/* A send buffer's "physical" address is ambiguous. MmGetPhysicalAddress here
 * strips the contiguous window's base and leaves any other address as it is,
 * so a low value is either. The rings and receive buffers are contiguous; a
 * send buffer may be pool memory. A frame starts with two MAC addresses and
 * the second is this machine's, which settles it. */
static uint32_t tx_buf_va(uint32_t p)
{
    uint8_t me[6];
    if (p >= CONTIG_SIZE)
        return p;
    local_mac(me);
    return memcmp((const void *)XBOX_PTR(p + 6), me, 6) == 0 ? p : p + CONTIG_BASE;
}

static FILE *s_pcap;

static void log_frame(const char *dir, const uint8_t *f, unsigned len)
{
    if (s_pcap) {
        LARGE_INTEGER t, fq;
        uint32_t rec[4];
        QueryPerformanceCounter(&t); QueryPerformanceFrequency(&fq);
        rec[0] = (uint32_t)(t.QuadPart / fq.QuadPart);
        rec[1] = (uint32_t)((t.QuadPart % fq.QuadPart) * 1000000 / fq.QuadPart);
        rec[2] = rec[3] = len;
        fwrite(rec, 4, 4, s_pcap); fwrite(f, 1, len, s_pcap); fflush(s_pcap);
    }
    if (!s_log || len < 14)
        return;
    fprintf(stderr, "[NET] %s %u bytes %02X:%02X:%02X:%02X:%02X:%02X -> "
            "%02X:%02X:%02X:%02X:%02X:%02X type %02X%02X\n", dir, len,
            f[6], f[7], f[8], f[9], f[10], f[11],
            f[0], f[1], f[2], f[3], f[4], f[5], f[12], f[13]);
}

/* ---- transport --------------------------------------------------------- */

static void send_to(const struct sockaddr_in *to, int type, const uint8_t *f, unsigned len)
{
    uint8_t pkt[4 + MAX_FRAME];
    pkt[0] = 'C'; pkt[1] = 'W'; pkt[2] = PKT_VERSION; pkt[3] = (uint8_t)type;
    if (len) memcpy(pkt + 4, f, len);
    sendto(s_sock, (const char *)pkt, (int)(4 + len), 0,
           (const struct sockaddr *)to, sizeof *to);
}

static void rxq_push(const uint8_t *f, unsigned len)
{
    EnterCriticalSection(&s_lock);
    if (s_rxq_head - s_rxq_tail < RXQ_LEN) {
        s_rxq[s_rxq_head % RXQ_LEN].len = (uint16_t)len;
        memcpy(s_rxq[s_rxq_head % RXQ_LEN].data, f, len);
        s_rxq_head++;
    }
    LeaveCriticalSection(&s_lock);
}

static int peer_has_mac(const struct peer *p, const uint8_t *mac)
{
    for (int i = 0; i < p->nmac; i++)
        if (!memcmp(p->mac[i], mac, 6)) return 1;
    return 0;
}

/* Host: deliver a frame that came from peer `from` (-1 = this machine).
 * Called with s_lock held. */
static void host_switch(int from, const uint8_t *f, unsigned len)
{
    uint8_t me[6];
    int to = -2;                         /* -2 flood, -1 local, else peer */

    if (from >= 0 && !peer_has_mac(&s_peers[from], f + 6)) {
        struct peer *p = &s_peers[from];
        memcpy(p->mac[p->nmac < 8 ? p->nmac++ : 7], f + 6, 6);
    }
    if (!(f[0] & 1)) {
        local_mac(me);
        if (!memcmp(f, me, 6))
            to = -1;
        else
            for (int i = 0; i < s_npeers; i++)
                if (peer_has_mac(&s_peers[i], f)) { to = i; break; }
    }
    if (from >= 0 && (to == -2 || to == -1)) {
        LeaveCriticalSection(&s_lock);
        rxq_push(f, len);
        EnterCriticalSection(&s_lock);
    }
    for (int i = 0; i < s_npeers; i++)
        if (i != from && (to == -2 || to == i))
            send_to(&s_peers[i].addr, PKT_FRAME, f, len);
}

static int same_addr(const struct sockaddr_in *a, const struct sockaddr_in *b)
{
    return a->sin_addr.s_addr == b->sin_addr.s_addr && a->sin_port == b->sin_port;
}

static int peer_index(const struct sockaddr_in *a)
{
    for (int i = 0; i < s_npeers; i++)
        if (same_addr(&s_peers[i].addr, a))
            return i;
    if (s_npeers == MAX_PEERS)
        return -1;
    memset(&s_peers[s_npeers], 0, sizeof s_peers[0]);
    s_peers[s_npeers].addr = *a;
    fprintf(stderr, "[NET] player at %s:%u joined\n",
            inet_ntoa(a->sin_addr), ntohs(a->sin_port));
    return s_npeers++;
}

/* ---- what the player sees ---------------------------------------------- */

static HWND s_hwnd;

static BOOL CALLBACK find_window(HWND h, LPARAM unused)
{
    DWORD pid;
    char cls[32];
    (void)unused;
    GetWindowThreadProcessId(h, &pid);
    if (pid == GetCurrentProcessId() && GetClassNameA(h, cls, sizeof cls) && !strcmp(cls, "CloneWarsDX8")) {
        s_hwnd = h;
        return FALSE;
    }
    return TRUE;
}

static void set_title(const char *extra)
{
    char t[160];
    if (!s_hwnd) EnumWindows(find_window, 0);
    if (!s_hwnd) return;
    snprintf(t, sizeof t, "Star Wars: The Clone Wars%s%s", extra ? "  --  " : "", extra ? extra : "");
    SetWindowTextA(s_hwnd, t);
}

static DWORD WINAPI notice_thread(LPVOID text)
{
    MessageBoxA(NULL, (const char *)text, "Network play",
                MB_OK | MB_ICONINFORMATION | MB_TOPMOST | MB_SETFOREGROUND);
    free(text);
    return 0;
}

/* A message box that does not hold up the caller (or the game). */
static void notice(const char *fmt, ...)
{
    va_list ap;
    char *text = (char *)malloc(512);
    if (!text) return;
    va_start(ap, fmt);
    vsnprintf(text, 512, fmt, ap);
    va_end(ap);
    fprintf(stderr, "[NET] %s\n", text);
    CloseHandle(CreateThread(NULL, 0, notice_thread, text, 0, NULL));
}

static void copy_to_clipboard(const char *s)
{
    HGLOBAL g;
    size_t n = strlen(s) + 1;
    if (!OpenClipboard(NULL)) return;
    EmptyClipboard();
    if ((g = GlobalAlloc(GMEM_MOVEABLE, n)) != NULL) {
        memcpy(GlobalLock(g), s, n);
        GlobalUnlock(g);
        SetClipboardData(CF_TEXT, g);
    }
    CloseClipboard();
}

/* The code box: a dialog built in memory, so there is no resource file. */
#define ID_CODE 100
static char s_entered[32];

static INT_PTR CALLBACK code_dlg(HWND d, UINT msg, WPARAM wp, LPARAM lp)
{
    (void)lp;
    if (msg == WM_INITDIALOG) {
        SetWindowPos(d, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        SetForegroundWindow(d);
        SendDlgItemMessageA(d, ID_CODE, EM_LIMITTEXT, 12, 0);
        SetFocus(GetDlgItem(d, ID_CODE));
        return FALSE;
    }
    if (msg == WM_COMMAND && LOWORD(wp) == ID_CODE && HIWORD(wp) == EN_CHANGE) {
        WCHAR w[32];
        GetDlgItemTextW(d, ID_CODE, w, 32);
        WideCharToMultiByte(CP_ACP, 0, w, -1, s_entered, sizeof s_entered, NULL, NULL);
        return TRUE;
    }
    if (msg == WM_COMMAND && (LOWORD(wp) == IDOK || LOWORD(wp) == IDCANCEL)) {
        EndDialog(d, LOWORD(wp));
        return TRUE;
    }
    return FALSE;
}

static WORD *dlg_add(WORD *p, DWORD style, short x, short y, short cx, short cy,
                     WORD id, WORD cls, const char *text)
{
    DLGITEMTEMPLATE *it;
    p = (WORD *)(((ULONG_PTR)p + 3) & ~(ULONG_PTR)3);
    it = (DLGITEMTEMPLATE *)p;
    it->style = style | WS_CHILD | WS_VISIBLE;
    it->dwExtendedStyle = 0;
    it->x = x; it->y = y; it->cx = cx; it->cy = cy; it->id = id;
    p = (WORD *)(it + 1);
    *p++ = 0xFFFF; *p++ = cls;
    p += MultiByteToWideChar(CP_ACP, 0, text, -1, (LPWSTR)p, 128);
    *p++ = 0;
    return p;
}

/* Ask for a join code. Returns 1 with a normalised code, 0 if none. */
static int ask_code(char *code, size_t n)
{
    static WORD buf[1024];
    DLGTEMPLATE *t = (DLGTEMPLATE *)buf;
    WORD *p;
    const char *s;
    size_t k = 0;

    memset(buf, 0, sizeof buf);
    s_entered[0] = 0;
    t->style = DS_MODALFRAME | DS_CENTER | DS_SETFONT | WS_POPUP | WS_CAPTION | WS_SYSMENU;
    t->cdit = 4;
    t->cx = 200; t->cy = 74;
    p = (WORD *)(t + 1);
    *p++ = 0; *p++ = 0;                               /* no menu, default class */
    p += MultiByteToWideChar(CP_ACP, 0, "Join a friend's game", -1, (LPWSTR)p, 64);
    *p++ = 9;                                         /* DS_SETFONT: 9 pt */
    p += MultiByteToWideChar(CP_ACP, 0, "Segoe UI", -1, (LPWSTR)p, 32);
    p = dlg_add(p, SS_LEFT, 8, 6, 184, 20, 0xFFFF, 0x0082,
                "Type the code your friend got when they created the game. "
                "Leave it empty to search your home network only.");
    p = dlg_add(p, ES_UPPERCASE | ES_CENTER | WS_BORDER | WS_TABSTOP, 50, 30, 100, 14, ID_CODE, 0x0081, "");
    p = dlg_add(p, BS_DEFPUSHBUTTON | WS_TABSTOP, 50, 52, 48, 14, IDOK, 0x0080, "Join");
    p = dlg_add(p, BS_PUSHBUTTON | WS_TABSTOP, 102, 52, 48, 14, IDCANCEL, 0x0080, "Cancel");

    {
        INT_PTR r = DialogBoxIndirectParamA(GetModuleHandleA(NULL), t, NULL, code_dlg, 0);
        if (r != IDOK) {
            if (r < 0)
                fprintf(stderr, "[NET] the join-code box failed to open (error %lu)\n", GetLastError());
            return 0;
        }
    }
    for (s = s_entered; *s && k + 1 < n; s++)
        if ((*s >= 'A' && *s <= 'Z') || (*s >= '0' && *s <= '9'))
            code[k++] = *s;
        else if (*s >= 'a' && *s <= 'z')
            code[k++] = (char)(*s - 32);
    code[k] = 0;
    return k > 0;
}

/* ---- join codes --------------------------------------------------------- */

/* Seven characters from an alphabet without look-alikes (no O/0, I/1):
 * 32^7, about 34 billion codes. Shown as ABCD-EFG; typed with or without the
 * dash, in either case. */
static const char CODE_ALPHABET[] = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";
static char s_code[16];                  /* hosting under this code */
static char s_join_code[16];             /* joining under this code */
static char s_code_shown[16];            /* hosting code, ABCD-EFG */
static volatile LONG s_host_gen;          /* bumps when a new game is created */
static volatile LONG s_asking;
static DWORD s_last_search;
static unsigned short s_port;

static DWORD WINAPI host_thread(LPVOID genp)
{
    LONG gen = (LONG)(LONG_PTR)genp;
    uint8_t ih[20], jh[20];
    struct sockaddr_in found[DHT_MAX_PEERS];
    DWORD last_announce = 0;
    int announces = 0;
    dht_code_hash(s_code, 0, ih);
    dht_code_hash(s_code, 1, jh);
    while (s_host_gen == gen) {
        /* Listed at once, again shortly after (more of the DHT has seen this
         * machine by then), then every ten minutes; entries expire in 30. */
        DWORD every = announces < 3 ? 30 * 1000 : 10 * 60 * 1000;
        if (!last_announce || GetTickCount() - last_announce > every) {
            announces++;
            dht_lookup(ih, 1, s_port, NULL, 0, 6000);
            last_announce = GetTickCount();
            fprintf(stderr, "[NET] code %s is listed\n", s_code);
        }
        /* Joiners list themselves under the code's join entry. Sending to
         * them first is what opens this side's router to them. */
        /* A joiner that went quiet (left, or reached us twice by two
         * routes and kept only one) stops being sent frames. */
        EnterCriticalSection(&s_lock);
        for (int i = 0; i < s_npeers; ) {
            if (GetTickCount() - s_peers[i].seen > 15000) {
                fprintf(stderr, "[NET] player at %s:%u left\n",
                        inet_ntoa(s_peers[i].addr.sin_addr), ntohs(s_peers[i].addr.sin_port));
                s_peers[i] = s_peers[--s_npeers];
            } else {
                i++;
            }
        }
        LeaveCriticalSection(&s_lock);
        int n = dht_lookup(jh, 0, 0, found, DHT_MAX_PEERS, 4000);
        for (int i = 0; i < n; i++)
            for (int k = 0; k < 3; k++)
                send_to(&found[i], PKT_HELLO, NULL, 0);
        Sleep(3000);
    }
    return 0;
}

static void start_hosting(void)
{
    uint8_t r[8];
    char shown[16], msg[128];
    BCryptGenRandom(NULL, r, sizeof r, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    for (int i = 0; i < 7; i++) s_code[i] = CODE_ALPHABET[r[i] & 31];
    s_code[7] = 0;
    snprintf(shown, sizeof shown, "%.4s-%.3s", s_code, s_code + 4);
    s_mode = 1;
    LONG gen = InterlockedIncrement(&s_host_gen);
    CloseHandle(CreateThread(NULL, 0, host_thread, (LPVOID)(LONG_PTR)gen, 0, NULL));
    snprintf(msg, sizeof msg, "Join code %s", shown);
    set_title(msg);
    copy_to_clipboard(shown);
    snprintf(s_code_shown, sizeof s_code_shown, "%s", shown);
    fprintf(stderr, "[NET] your join code is %s\n", shown);
}

/* The code as the menus show it (src/platform/d3d8.c draws it below the UI),
 * or "" when this machine is not hosting. */
const char *cw_net_join_code(void)
{
    return s_mode == 1 ? s_code_shown : "";
}

/* Where a joiner sends HELLO: everything the DHT lists under the code, plus
 * the game's own port at each of those addresses. Filled by lookup_thread
 * while join_thread keeps knocking, so a slow or partly wrong lookup costs
 * time rather than the connection. */
static struct sockaddr_in s_cand[64];
static volatile LONG s_ncand;
static volatile LONG s_listed;               /* addresses the DHT returned */

static void cand_add(const struct sockaddr_in *a)
{
    EnterCriticalSection(&s_lock);
    for (int i = 0; i < s_ncand; i++)
        if (same_addr(&s_cand[i], a)) { LeaveCriticalSection(&s_lock); return; }
    if (s_ncand < (LONG)(sizeof s_cand / sizeof s_cand[0]))
        s_cand[s_ncand++] = *a;
    LeaveCriticalSection(&s_lock);
}

static DWORD WINAPI lookup_thread(LPVOID unused)
{
    uint8_t ih[20], jh[20];
    struct sockaddr_in found[DHT_MAX_PEERS];
    (void)unused;
    dht_code_hash(s_join_code, 0, ih);
    dht_code_hash(s_join_code, 1, jh);
    for (int round = 0; round < 6 && s_mode == 2 && !s_npeers; round++) {
        if (round % 2 == 0)
            dht_lookup(jh, 1, s_port, NULL, 0, 6000);    /* so the host can find us */
        int n = dht_lookup(ih, 0, 0, found, DHT_MAX_PEERS, 8000);
        for (int i = 0; i < n; i++) {
            struct sockaddr_in a = found[i];
            fprintf(stderr, "[NET] code %s is listed at %s:%u\n", s_join_code,
                    inet_ntoa(a.sin_addr), ntohs(a.sin_port));
            InterlockedIncrement(&s_listed);
            cand_add(&a);
            /* Some DHT nodes record the port wrongly (implied_port read as
             * the port itself, so the entry says port 1); most home routers
             * keep the game's own. */
            a.sin_port = htons(34100);
            cand_add(&a);
        }
    }
    return 0;
}

static DWORD WINAPI join_thread(LPVOID unused)
{
    char code[32];
    (void)unused;

    if (!ask_code(code, sizeof code)) {
        fprintf(stderr, "[NET] no join code entered; searching this network only\n");
        s_asking = 0;
        return 0;
    }
    if (strlen(code) != 7) {
        notice("\"%s\" isn't a join code. Codes look like ABCD-EFG.", code);
        s_asking = 0;
        return 0;
    }
    fprintf(stderr, "[NET] looking up join code %s\n", code);
    snprintf(s_join_code, sizeof s_join_code, "%s", code);
    set_title("Looking up the code...");
    s_ncand = 0;
    s_listed = 0;
    /* A host on the same network or the same PC is behind the same public
     * address, which many routers will not loop back. (CW_NET_NO_LAN=1
     * leaves these out, to test the internet path from one PC.) */
    if (!getenv("CW_NET_NO_LAN")) {
        struct sockaddr_in a = {0};
        a.sin_family = AF_INET;
        a.sin_port = htons(34100);
        a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        cand_add(&a);
        a.sin_addr.s_addr = htonl(INADDR_BROADCAST);
        cand_add(&a);
    }
    s_mode = 2;
    CloseHandle(CreateThread(NULL, 0, lookup_thread, NULL, 0, NULL));
    for (int t = 0; t < 300 && !s_npeers; t++) {          /* a minute */
        for (int i = 0; i < s_ncand; i++)
            send_to(&s_cand[i], PKT_HELLO, (const uint8_t *)code, 7);
        Sleep(200);
    }
    if (!s_npeers) {
        s_mode = 0;
        set_title(NULL);
        notice(!s_listed ? "No game found for code %s. Check the code with your friend."
                         : "Found the game for code %s but couldn't connect to it.", code);
        s_asking = 0;
        return 0;
    }
    set_title("Connected");
    while (s_mode == 2) {                                 /* keeps the router's mapping open */
        send_to(&s_peers[0].addr, PKT_HELLO, (const uint8_t *)code, 7);
        Sleep(1000);
    }
    return 0;
}

/* HELLO and WELCOME carry the join code (empty for a direct connection), so
 * a joiner only takes the host it asked for -- DHT answers can include
 * strangers, and a broadcast reaches every game on the network -- and a host
 * only takes joiners holding its code. */
static int code_matches(const uint8_t *payload, int n, const char *code)
{
    return n == 0 || !code[0] || (n == 7 && !memcmp(payload, code, 7));
}

static DWORD WINAPI recv_thread(LPVOID unused)
{
    uint8_t pkt[4 + MAX_FRAME + 64];
    (void)unused;
    for (;;) {
        struct sockaddr_in from;
        int fl = sizeof from;
        int n = recvfrom(s_sock, (char *)pkt, sizeof pkt, 0, (struct sockaddr *)&from, &fl);
        if (n > 0 && pkt[0] == 'd') {
            dht_on_packet(pkt, n, &from);
            continue;
        }
        if (n < 4 || pkt[0] != 'C' || pkt[1] != 'W' || pkt[2] != PKT_VERSION)
            continue;
        if (s_mode == 1) {
            EnterCriticalSection(&s_lock);
            int p = -1;
            if (pkt[3] == PKT_HELLO && code_matches(pkt + 4, n - 4, s_code)) {
                p = peer_index(&from);
                if (p >= 0) s_peers[p].seen = GetTickCount();
                send_to(&from, PKT_WELCOME, (const uint8_t *)s_code, (unsigned)strlen(s_code));
            } else {
                for (int i = 0; i < s_npeers; i++)
                    if (same_addr(&s_peers[i].addr, &from)) p = i;
            }
            if (p >= 0) {
                s_peers[p].seen = GetTickCount();
                if (pkt[3] == PKT_FRAME && n >= 4 + 14)
                    host_switch(p, pkt + 4, (unsigned)(n - 4));
            }
            LeaveCriticalSection(&s_lock);
        } else if (s_mode == 2) {
            if (pkt[3] == PKT_WELCOME && !s_npeers && code_matches(pkt + 4, n - 4, s_join_code)) {
                s_peers[0].addr = from;
                s_npeers = 1;
                fprintf(stderr, "[NET] connected to the host at %s:%u\n",
                        inet_ntoa(from.sin_addr), ntohs(from.sin_port));
            } else if (pkt[3] == PKT_FRAME && n >= 4 + 14 && s_npeers &&
                       same_addr(&from, &s_peers[0].addr)) {
                rxq_push(pkt + 4, (unsigned)(n - 4));
            }
        }
    }
}

/* Direct connections for testing, without a code: CW_NET_JOIN=addr[:port]. */
static DWORD WINAPI direct_join_thread(LPVOID addrp)
{
    struct sockaddr_in a = *(struct sockaddr_in *)addrp;
    free(addrp);
    for (int t = 0; t < 600 && !s_npeers; t++) {
        send_to(&a, PKT_HELLO, NULL, 0);
        Sleep(200);
    }
    while (s_mode == 2) {
        send_to(&s_peers[0].addr, PKT_HELLO, NULL, 0);
        Sleep(1000);
    }
    return 0;
}

static void net_start(void)
{
    static int started;
    const char *join = getenv("CW_NET_JOIN"), *port_s = getenv("CW_NET_PORT");
    unsigned short port = (unsigned short)(port_s ? atoi(port_s) : 34100);
    WSADATA wsa;
    struct sockaddr_in me = {0};
    BOOL yes = TRUE;

    if (started) return;
    started = 1;
    if (getenv("CW_NET_OFF")) return;
    s_log = getenv("CW_NET_LOG") != NULL;
    if (getenv("CW_NET_PCAP") && (s_pcap = fopen(getenv("CW_NET_PCAP"), "wb")) != NULL) {
        static const uint32_t hdr[6] = { 0xA1B2C3D4u, 0x00040002u, 0, 0, 65535, 1 };
        fwrite(hdr, 4, 6, s_pcap);
    }

    InitializeCriticalSection(&s_lock);
    WSAStartup(MAKEWORD(2, 2), &wsa);
    s_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    setsockopt(s_sock, SOL_SOCKET, SO_BROADCAST, (const char *)&yes, sizeof yes);
    me.sin_family = AF_INET;
    me.sin_port = htons(port);
    if (bind(s_sock, (struct sockaddr *)&me, sizeof me) != 0) {
        me.sin_port = 0;                  /* taken (a second copy on this PC) */
        if (bind(s_sock, (struct sockaddr *)&me, sizeof me) != 0) {
            fprintf(stderr, "[NET] cannot open a UDP port (error %d); network play off\n",
                    WSAGetLastError());
            return;
        }
    }
    {
        int ml = sizeof me;
        getsockname(s_sock, (struct sockaddr *)&me, &ml);
        s_port = ntohs(me.sin_port);
    }
    s_ready = 1;
    dht_init(s_sock);
    CloseHandle(CreateThread(NULL, 0, recv_thread, NULL, 0, NULL));
    fprintf(stderr, "[NET] network play on UDP port %u\n", s_port);

    if (getenv("CW_NET_HOST")) {
        s_mode = 1;
    } else if (join) {
        char host[128];
        const char *colon;
        struct addrinfo hints = {0}, *res = NULL;
        struct sockaddr_in *a;
        unsigned short jport = 34100;
        snprintf(host, sizeof host, "%s", join);
        colon = strrchr(host, ':');
        if (colon) { jport = (unsigned short)atoi(colon + 1); host[colon - host] = 0; }
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        if (getaddrinfo(host, NULL, &hints, &res) != 0 || !res) {
            fprintf(stderr, "[NET] cannot resolve %s\n", host);
            return;
        }
        a = (struct sockaddr_in *)malloc(sizeof *a);
        *a = *(struct sockaddr_in *)res->ai_addr;
        a->sin_port = htons(jport);
        freeaddrinfo(res);
        s_mode = 2;
        CloseHandle(CreateThread(NULL, 0, direct_join_thread, a, 0, NULL));
    }
}

/* 0x002A2EDA -- XNetCreateKey(pxnkid, pxnkey). The game calls it when a player
 * creates a System Link game, so this is where hosting starts. */
void sub_002A2EDA_gen(void);
void sub_002A2EDA(void)
{
    sub_002A2EDA_gen();
    if (s_ready && s_mode != 2)
        start_hosting();
}

/* Called for every frame this machine sends. A System Link search is a
 * broadcast to port 3074 from a machine that is not hosting; the first one of
 * a search asks the player for a join code. */
static void watch_for_search(const uint8_t *f, unsigned len)
{
    DWORD now = GetTickCount();
    if (len < 42 || memcmp(f, "\xFF\xFF\xFF\xFF\xFF\xFF", 6) || f[12] != 0x08 || f[13] != 0 ||
        f[23] != 17 || (f[36] << 8 | f[37]) != 3074)
        return;
    if (s_mode == 1)
        return;
    if (now - s_last_search > 10000 && s_mode == 0 &&
        InterlockedCompareExchange(&s_asking, 1, 0) == 0)
        CloseHandle(CreateThread(NULL, 0, join_thread, NULL, 0, NULL));
    s_last_search = now;
}

/* ---- the chip ---------------------------------------------------------- */

/* DHCP, answered by the chip.
 *
 * XNet asks for an address before it does anything else, and System Link
 * advertises a session with the host's address in it. With nothing answering,
 * XNet waits about 25 seconds, gives up, and advertises 0.0.0.0, which a
 * joiner then cannot reach. A home router would answer; here the chip does,
 * with an address derived from the MAC so that every copy's is different:
 * 10.47.m4.m5 on 255.255.0.0. */
static uint16_t ip_checksum(const uint8_t *p, unsigned n)
{
    uint32_t s = 0;
    for (unsigned i = 0; i + 1 < n; i += 2) s += (uint32_t)(p[i] << 8 | p[i + 1]);
    while (s >> 16) s = (s & 0xFFFF) + (s >> 16);
    return (uint16_t)~s;
}

static int dhcp_answer(const uint8_t *f, unsigned len)
{
    static const uint8_t server_mac[6] = { 0x00, 0x50, 0xF2, 0x00, 0x00, 0x01 };
    static const uint8_t server_ip[4] = { 10, 47, 0, 1 };
    const uint8_t *bootp;
    uint8_t r[14 + 20 + 8 + 300], *ip, *udp, *b, *o, kind = 0, mine[4];
    unsigned ihl;

    if (len < 14 + 20 + 8 + 240 || f[12] != 0x08 || f[13] != 0x00 || f[23] != 17)
        return 0;
    ihl = (f[14] & 15) * 4;
    if ((f[14 + ihl + 2] << 8 | f[14 + ihl + 3]) != 67)
        return 0;
    bootp = f + 14 + ihl + 8;
    for (unsigned i = 240; i + 2 < len - 14 - ihl - 8 && bootp[i] != 255; i += 2 + bootp[i + 1])
        if (bootp[i] == 53) kind = bootp[i + 2];
        else if (bootp[i] == 0) i--;
    if (kind != 1 && kind != 3)                  /* DISCOVER, REQUEST */
        return 1;

    mine[0] = 10; mine[1] = 47; mine[2] = f[10]; mine[3] = f[11];
    if (mine[3] == 0 || mine[3] == 255) mine[3] = 7;
    if (mine[2] == 0 && mine[3] == 1) mine[3] = 2;

    memset(r, 0, sizeof r);
    memset(r, 0xFF, 6);
    memcpy(r + 6, server_mac, 6);
    r[12] = 0x08;
    ip = r + 14; udp = ip + 20; b = udp + 8;
    ip[0] = 0x45; ip[2] = (20 + 8 + 300) >> 8; ip[3] = (20 + 8 + 300) & 0xFF;
    ip[8] = 64; ip[9] = 17;
    memcpy(ip + 12, server_ip, 4);
    memset(ip + 16, 0xFF, 4);
    { uint16_t c = ip_checksum(ip, 20); ip[10] = (uint8_t)(c >> 8); ip[11] = (uint8_t)c; }
    udp[1] = 67; udp[3] = 68; udp[4] = (8 + 300) >> 8; udp[5] = (8 + 300) & 0xFF;
    b[0] = 2; b[1] = 1; b[2] = 6;
    memcpy(b + 4, bootp + 4, 4);                 /* transaction id */
    memcpy(b + 10, bootp + 10, 2);               /* flags */
    memcpy(b + 16, mine, 4);                     /* your address */
    memcpy(b + 20, server_ip, 4);
    memcpy(b + 28, bootp + 28, 16);              /* client hardware address */
    b[236] = 99; b[237] = 130; b[238] = 83; b[239] = 99;
    o = b + 240;
    *o++ = 53; *o++ = 1; *o++ = kind == 1 ? 2 : 5;   /* OFFER / ACK */
    *o++ = 54; *o++ = 4; memcpy(o, server_ip, 4); o += 4;
    *o++ = 51; *o++ = 4; *o++ = 0x7F; *o++ = 0xFF; *o++ = 0xFF; *o++ = 0xFF;
    *o++ = 1;  *o++ = 4; *o++ = 255; *o++ = 255; *o++ = 0; *o++ = 0;
    *o++ = 255;
    rxq_push(r, sizeof r);
    if (kind == 3)
        fprintf(stderr, "[NET] this copy's network address 10.47.%u.%u\n", mine[2], mine[3]);
    return 1;
}

static void frame_out(const uint8_t *f, unsigned len)
{
    log_frame("out", f, len);
    if (dhcp_answer(f, len))
        return;
    watch_for_search(f, len);
    if (s_mode == 1) {
        EnterCriticalSection(&s_lock);
        host_switch(-1, f, len);
        LeaveCriticalSection(&s_lock);
    } else if (s_mode == 2 && s_npeers) {
        send_to(&s_peers[0].addr, PKT_FRAME, f, len);
    }
}

static void raise_irq(void)
{
    for (uint32_t v = 0; v < 32; v++) {
        uint32_t k = xbox_GetConnectedInterrupt(v);
        if (k && MEM32(k) == NIC_ISR) {
            uint32_t sp = g_esp, a = g_eax, b = g_ebx, c = g_ecx, d = g_edx,
                     si = g_esi, di = g_edi, bp = g_ebp;
            PUSH32(g_esp, MEM32(k + 4));          /* context */
            PUSH32(g_esp, k);                     /* interrupt object */
            PUSH32(g_esp, 0);                     /* return address */
            {   /* never alongside a thread at DISPATCH_LEVEL; see kernel_hal.c */
                unsigned char irql = xbox_EnterDispatchLevel();
                sub_002AAB31();
                xbox_LeaveDispatchLevel(irql);
            }
            g_esp = sp; g_eax = a; g_ebx = b; g_ecx = c; g_edx = d;
            g_esi = si; g_edi = di; g_ebp = bp;
            return;
        }
    }
}

void cw_net_periodic(void)
{
    uint32_t tx_ring = MEM32(NIC_TX_RING), rx_ring = MEM32(NIC_RX_RING);
    uint32_t sizes = MEM32(NIC_RING_SIZES);
    unsigned tx_n = (sizes & 0xFFFF) + 1, rx_n = (sizes >> 16) + 1;

    net_start();
    if (!tx_ring || !rx_ring)
        return;
    if (tx_ring != s_tx_ring || rx_ring != s_rx_ring) {
        s_tx_ring = tx_ring; s_rx_ring = rx_ring;
        s_tx_i = s_rx_i = 0; s_tx_len = 0;
        fprintf(stderr, "[NET] chip started: %u send / %u receive slots\n", tx_n, rx_n);
    }

    /* Send: take every descriptor the driver has handed over, in order. */
    for (unsigned guard = 0; guard < tx_n; guard++) {
        uint32_t d = phys_to_va(tx_ring) + s_tx_i * 8;
        uint32_t fl = MEM32(d + 4);
        if (!(fl & DESC_OWN))
            break;
        unsigned part = (fl & 0xFFFF) + 1;
        if (s_tx_len == 0)
            s_tx_contig = tx_buf_va(MEM32(d)) != MEM32(d);
        if (s_tx_len + part <= MAX_FRAME) {
            uint32_t p = MEM32(d);
            memcpy(s_tx_buf + s_tx_len,
                   (const void *)XBOX_PTR(s_tx_contig && p < CONTIG_SIZE ? p + CONTIG_BASE : p), part);
            s_tx_len += part;
        }
        MEM32(d + 4) = fl & ~DESC_OWN;
        if (fl & DESC_LAST) {
            if (s_tx_len >= 14 && s_ready)
                frame_out(s_tx_buf, s_tx_len);
            s_tx_len = 0;
            InterlockedOr(&s_status, IRQ_TX);
        }
        s_tx_i = (s_tx_i + 1) % tx_n;
    }

    /* Receive: fill free descriptors from the queue. */
    if (s_ready) {
        EnterCriticalSection(&s_lock);
        while (s_rxq_tail != s_rxq_head) {
            uint32_t d = phys_to_va(rx_ring) + s_rx_i * 8;
            uint32_t fl = MEM32(d + 4);
            unsigned len = s_rxq[s_rxq_tail % RXQ_LEN].len;
            if (!(fl & DESC_OWN))
                break;                               /* ring full: keep it queued */
            if (len <= (fl & 0xFFFF) + 1) {
                memcpy((void *)XBOX_PTR(phys_to_va(MEM32(d))),
                       s_rxq[s_rxq_tail % RXQ_LEN].data, len);
                MEM32(d + 4) = RX_VALID | len;
                log_frame("in ", s_rxq[s_rxq_tail % RXQ_LEN].data, len);
                InterlockedOr(&s_status, IRQ_RX);
                s_rx_i = (s_rx_i + 1) % rx_n;
            }
            s_rxq_tail++;
        }
        LeaveCriticalSection(&s_lock);
    }

    if (s_status && MEM32(NIC_IRQ_MASK))
        raise_irq();
}

/* 0x0016752A -- ExQueryNonVolatileSetting(index, *type, value, size, *result).
 *
 * XNet reads the console's Ethernet address from setting 0x101 (the factory
 * EEPROM). There is no EEPROM here and the setting comes back as six zeros, so
 * every copy of the game would share address 00:00:00:00:00:00 and no two of
 * them could talk. Each run gets its own: Microsoft's 00:50:F2 prefix, which is
 * what the driver itself falls back to, and three random bytes. */
void sub_0016752A_gen(void);
void sub_0016752A(void)
{
    static uint8_t mac[6];
    if (MEM32(g_esp + 4) == 0x101 && MEM32(g_esp + 16) >= 6) {
        uint32_t type = MEM32(g_esp + 8), buf = MEM32(g_esp + 12), got = MEM32(g_esp + 20);
        if (!mac[1]) {
            LARGE_INTEGER t;
            uint32_t r;
            QueryPerformanceCounter(&t);
            r = (uint32_t)t.QuadPart ^ (GetCurrentProcessId() * 2654435761u);
            mac[0] = 0x00; mac[1] = 0x50; mac[2] = 0xF2;
            mac[3] = (uint8_t)(r >> 16); mac[4] = (uint8_t)(r >> 8); mac[5] = (uint8_t)r;
            fprintf(stderr, "[NET] this copy's address %02X:%02X:%02X:%02X:%02X:%02X\n",
                    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        }
        memcpy((void *)XBOX_PTR(buf), mac, 6);
        if (type) MEM32(type) = 3;                   /* REG_BINARY */
        if (got) MEM32(got) = 6;
        g_eax = 0;
        g_esp += 24;                                 /* ret 0x14 */
        return;
    }
    sub_0016752A_gen();
}

/* 0x002AB16B -- the NIC DPC body (thiscall, adapter in ecx). See the top of
 * the file: the status lives here, so acknowledging it actually clears it. */
void sub_002AB16B(void)
{
    uint32_t adapter = g_ecx;
    uint32_t b = g_ebx, bp = g_ebp, si = g_esi, di = g_edi;

    while (InterlockedExchange(&s_status, 0)) {
        PUSH32(g_esp, 0); g_ecx = adapter; sub_002AAB3D();   /* receive */
        PUSH32(g_esp, 0); g_ecx = adapter; sub_002AAC91();   /* send completions */
    }
    MEM32(NIC_IRQ_MASK) = 0x5F;
    g_ebx = b; g_ebp = bp; g_esi = si; g_edi = di;
    g_esp += 4;                                           /* ret */
}
