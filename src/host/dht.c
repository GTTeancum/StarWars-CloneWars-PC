/*
 * dht.c -- join codes through the BitTorrent DHT.
 *
 * Network play has no server. A host publishes "code K7QX-2M is at this
 * address" in the BitTorrent network's public peer directory -- the Mainline
 * DHT (BEP 5) that torrent clients use to find each other -- and a joiner
 * looks the code up there. Nobody here runs or maintains anything: the
 * directory is millions of torrent clients, and an entry expires on its own.
 *
 * A code maps to an info-hash, SHA-1("CloneWarsRecomp/1/" + code). The host
 * announces itself under it; a joiner announces itself under the code's
 * "/join" hash so the host can find the joiner too and send first, which is
 * what gets a packet through the host's router (hole punching).
 *
 * Everything goes out on the game's own UDP socket, so the port the DHT
 * records (implied_port) is the very mapping the game traffic then uses.
 * net.c hands every datagram that starts with 'd' (a bencoded dictionary)
 * to dht_on_packet().
 *
 * Only what a lookup needs is here: get_peers and announce_peer as a client.
 * The node never answers queries and keeps no routing table between lookups.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <bcrypt.h>

#include "dht.h"

#define MAX_NODES   256
#define ALPHA       8
#define K_CLOSEST   16

static const char *const BOOTSTRAP[] = {
    "router.bittorrent.com", "dht.transmissionbt.com", "router.utorrent.com",
    "dht.libtorrent.org",
};
static const unsigned short BOOTSTRAP_PORT[] = { 6881, 6881, 6881, 25401 };

static SOCKET s_sock = INVALID_SOCKET;
static uint8_t s_id[20];
static CRITICAL_SECTION s_lock;
static int s_ready;

/* The lookup in progress. One at a time: lookups are serialised on s_busy. */
static struct node {
    uint8_t id[20];
    struct sockaddr_in addr;
    uint8_t queried, answered, has_id;
    uint8_t token[64];
    int token_len;
} s_nodes[MAX_NODES];
static int s_nnodes;
static uint8_t s_target[20];
static struct sockaddr_in s_peers[DHT_MAX_PEERS];
static int s_npeers;
static uint16_t s_session;               /* high byte of transaction ids */
static CRITICAL_SECTION s_busy;

void dht_sha1(const void *data, unsigned len, uint8_t out[20])
{
    BCRYPT_ALG_HANDLE alg;
    if (BCryptOpenAlgorithmProvider(&alg, BCRYPT_SHA1_ALGORITHM, NULL, 0) == 0) {
        BCryptHash(alg, NULL, 0, (PUCHAR)data, len, out, 20);
        BCryptCloseAlgorithmProvider(alg, 0);
    } else {
        memset(out, 0, 20);
    }
}

void dht_code_hash(const char *code, int join_side, uint8_t out[20])
{
    char buf[64];
    int n = snprintf(buf, sizeof buf, "CloneWarsRecomp/1/%s%s", code, join_side ? "/join" : "");
    dht_sha1(buf, (unsigned)n, out);
}

void dht_init(SOCKET sock)
{
    LARGE_INTEGER t;
    uint8_t seed[32];
    if (s_ready) return;
    s_sock = sock;
    InitializeCriticalSection(&s_lock);
    InitializeCriticalSection(&s_busy);
    if (BCryptGenRandom(NULL, s_id, 20, BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0) {
        QueryPerformanceCounter(&t);
        memcpy(seed, &t, 8);
        *(DWORD *)(seed + 8) = GetCurrentProcessId();
        dht_sha1(seed, 12, s_id);
    }
    s_ready = 1;
}

/* ---- bencode ------------------------------------------------------------ */

/* Skip one value starting at p; returns the position after it, or NULL. */
static const uint8_t *b_skip(const uint8_t *p, const uint8_t *end)
{
    if (p >= end) return NULL;
    if (*p == 'i') {
        while (p < end && *p != 'e') p++;
        return p < end ? p + 1 : NULL;
    }
    if (*p == 'l' || *p == 'd') {
        p++;
        while (p && p < end && *p != 'e') p = b_skip(p, end);
        return p && p < end ? p + 1 : NULL;
    }
    if (*p >= '0' && *p <= '9') {
        unsigned long n = 0;
        while (p < end && *p >= '0' && *p <= '9') n = n * 10 + (*p++ - '0');
        if (p >= end || *p != ':' || n > (unsigned long)(end - p - 1)) return NULL;
        return p + 1 + n;
    }
    return NULL;
}

/* A byte string at p: sets *s, *n and returns the position after it. */
static const uint8_t *b_str(const uint8_t *p, const uint8_t *end, const uint8_t **s, int *n)
{
    unsigned long len = 0;
    if (p >= end || *p < '0' || *p > '9') return NULL;
    while (p < end && *p >= '0' && *p <= '9') len = len * 10 + (*p++ - '0');
    if (p >= end || *p != ':' || len > (unsigned long)(end - p - 1)) return NULL;
    *s = p + 1; *n = (int)len;
    return p + 1 + len;
}

/* The value under `key` in the dictionary at p (which starts with 'd'). */
static const uint8_t *b_find(const uint8_t *p, const uint8_t *end, const char *key)
{
    size_t kl = strlen(key);
    if (p >= end || *p != 'd') return NULL;
    p++;
    while (p && p < end && *p != 'e') {
        const uint8_t *k; int n;
        p = b_str(p, end, &k, &n);
        if (!p) return NULL;
        if ((size_t)n == kl && !memcmp(k, key, kl)) return p;
        p = b_skip(p, end);
    }
    return NULL;
}

/* ---- the lookup --------------------------------------------------------- */

static int closer(const uint8_t *a, const uint8_t *b)   /* is a closer than b? */
{
    for (int i = 0; i < 20; i++) {
        uint8_t da = a[i] ^ s_target[i], db = b[i] ^ s_target[i];
        if (da != db) return da < db;
    }
    return 0;
}

static void add_node(const uint8_t *id, const struct sockaddr_in *a)
{
    int i;
    if (!a->sin_port || !a->sin_addr.s_addr) return;
    for (i = 0; i < s_nnodes; i++)
        if (s_nodes[i].addr.sin_addr.s_addr == a->sin_addr.s_addr &&
            s_nodes[i].addr.sin_port == a->sin_port) {
            if (id && !s_nodes[i].has_id) { memcpy(s_nodes[i].id, id, 20); s_nodes[i].has_id = 1; }
            return;
        }
    if (s_nnodes == MAX_NODES) {
        /* Full: replace the farthest unqueried node if this one is closer. */
        int worst = -1;
        if (!id) return;
        for (i = 0; i < s_nnodes; i++)
            if (!s_nodes[i].queried && s_nodes[i].has_id &&
                (worst < 0 || closer(s_nodes[worst].id, s_nodes[i].id)))
                worst = i;
        if (worst < 0 || !closer(id, s_nodes[worst].id)) return;
        i = worst;
    } else {
        i = s_nnodes++;
    }
    memset(&s_nodes[i], 0, sizeof s_nodes[i]);
    s_nodes[i].addr = *a;
    if (id) { memcpy(s_nodes[i].id, id, 20); s_nodes[i].has_id = 1; }
}

void dht_on_packet(const uint8_t *p, int n, const struct sockaddr_in *from)
{
    const uint8_t *end = p + n, *v, *r, *s;
    int len, idx;
    if (!s_ready || n < 2 || p[0] != 'd') return;
    v = b_find(p, end, "y");
    if (!v || !b_str(v, end, &s, &len) || len != 1 || s[0] != 'r') return;
    v = b_find(p, end, "t");
    if (!v || !b_str(v, end, &s, &len) || len != 4) return;
    if ((uint16_t)(s[0] << 8 | s[1]) != s_session) return;      /* an old lookup's */
    idx = s[2] << 8 | s[3];
    r = b_find(p, end, "r");
    if (!r) return;

    EnterCriticalSection(&s_lock);
    if (idx < s_nnodes &&
        s_nodes[idx].addr.sin_addr.s_addr == from->sin_addr.s_addr) {
        struct node *nd = &s_nodes[idx];
        nd->answered = 1;
        if ((v = b_find(r, end, "id")) && b_str(v, end, &s, &len) && len == 20 && !nd->has_id) {
            memcpy(nd->id, s, 20); nd->has_id = 1;
        }
        if ((v = b_find(r, end, "token")) && b_str(v, end, &s, &len) && len <= 64) {
            memcpy(nd->token, s, len); nd->token_len = len;
        }
    }
    if ((v = b_find(r, end, "nodes")) && b_str(v, end, &s, &len)) {
        for (int i = 0; i + 26 <= len; i += 26) {
            struct sockaddr_in a = {0};
            a.sin_family = AF_INET;
            memcpy(&a.sin_addr, s + i + 20, 4);
            memcpy(&a.sin_port, s + i + 24, 2);
            add_node(s + i, &a);
        }
    }
    if ((v = b_find(r, end, "values")) && *v == 'l') {
        v++;
        while (v < end && *v != 'e') {
            v = b_str(v, end, &s, &len);
            if (!v) break;
            if (len == 6 && s_npeers < DHT_MAX_PEERS) {
                struct sockaddr_in a = {0};
                int dup = 0;
                a.sin_family = AF_INET;
                memcpy(&a.sin_addr, s, 4);
                memcpy(&a.sin_port, s + 4, 2);
                for (int i = 0; i < s_npeers; i++)
                    if (s_peers[i].sin_addr.s_addr == a.sin_addr.s_addr && s_peers[i].sin_port == a.sin_port) dup = 1;
                if (!dup) s_peers[s_npeers++] = a;
            }
        }
    }
    LeaveCriticalSection(&s_lock);
}

static void send_query(int idx, int announce, unsigned short port)
{
    char m[512];
    int n;
    struct node *nd = &s_nodes[idx];
    unsigned char t[4] = { (unsigned char)(s_session >> 8), (unsigned char)s_session,
                           (unsigned char)(idx >> 8), (unsigned char)idx };
    if (announce) {
        n = snprintf(m, sizeof m, "d1:ad2:id20:");
        memcpy(m + n, s_id, 20); n += 20;
        n += snprintf(m + n, sizeof m - n, "12:implied_porti1e9:info_hash20:");
        memcpy(m + n, s_target, 20); n += 20;
        n += snprintf(m + n, sizeof m - n, "4:porti%ue5:token%d:", port, nd->token_len);
        memcpy(m + n, nd->token, nd->token_len); n += nd->token_len;
        n += snprintf(m + n, sizeof m - n, "e1:q13:announce_peer1:t4:");
    } else {
        n = snprintf(m, sizeof m, "d1:ad2:id20:");
        memcpy(m + n, s_id, 20); n += 20;
        n += snprintf(m + n, sizeof m - n, "9:info_hash20:");
        memcpy(m + n, s_target, 20); n += 20;
        n += snprintf(m + n, sizeof m - n, "e1:q9:get_peers1:t4:");
    }
    memcpy(m + n, t, 4); n += 4;
    n += snprintf(m + n, sizeof m - n, "1:y1:qe");
    sendto(s_sock, m, n, 0, (const struct sockaddr *)&nd->addr, sizeof nd->addr);
}

/* Indices of the answered nodes, closest first (at most k). */
static int closest_answered(int *out, int k)
{
    int n = 0;
    for (int i = 0; i < s_nnodes; i++) {
        if (!s_nodes[i].answered || !s_nodes[i].has_id) continue;
        int j = n < k ? n++ : k;
        if (j == k) {
            if (!closer(s_nodes[i].id, s_nodes[out[k - 1]].id)) continue;
            j = k - 1;
        }
        while (j > 0 && closer(s_nodes[i].id, s_nodes[out[j - 1]].id)) { out[j] = out[j - 1]; j--; }
        out[j] = i;
    }
    return n;
}

int dht_lookup(const uint8_t hash[20], int announce, unsigned short port,
               struct sockaddr_in *peers, int max_peers, int timeout_ms)
{
    DWORD start = GetTickCount();
    int found, idle_rounds = 0;
    /* Runs to convergence even once something is found: nodes far from the
     * hash sometimes answer with unrelated peers, and the real entry sits
     * with the nodes closest to it. */

    if (!s_ready) return -1;
    EnterCriticalSection(&s_busy);
    EnterCriticalSection(&s_lock);
    memcpy(s_target, hash, 20);
    s_nnodes = 0;
    s_npeers = 0;
    s_session++;
    LeaveCriticalSection(&s_lock);

    for (int i = 0; i < (int)(sizeof BOOTSTRAP / sizeof BOOTSTRAP[0]); i++) {
        struct addrinfo hints = {0}, *res = NULL, *it;
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        if (getaddrinfo(BOOTSTRAP[i], NULL, &hints, &res) != 0) continue;
        for (it = res; it; it = it->ai_next) {
            struct sockaddr_in a = *(struct sockaddr_in *)it->ai_addr;
            a.sin_port = htons(BOOTSTRAP_PORT[i]);
            EnterCriticalSection(&s_lock);
            add_node(NULL, &a);
            LeaveCriticalSection(&s_lock);
        }
        freeaddrinfo(res);
    }

    /* Iterate: query the closest nodes not yet asked, until the closest
     * answered set stops changing or time runs out. Nodes without a known id
     * (the bootstrap routers) go first, since nothing is known yet. */
    while ((int)(GetTickCount() - start) < timeout_ms) {
        int sent = 0;
        EnterCriticalSection(&s_lock);
        for (int r = 0; r < ALPHA; r++) {
            int best = -1;
            for (int i = 0; i < s_nnodes; i++) {
                if (s_nodes[i].queried) continue;
                if (best < 0 || (!s_nodes[i].has_id && s_nodes[best].has_id) ||
                    (s_nodes[i].has_id && s_nodes[best].has_id && closer(s_nodes[i].id, s_nodes[best].id)))
                    best = i;
            }
            if (best < 0) break;
            /* Stop widening once the candidate is farther than all of the
             * K closest answered nodes. */
            if (s_nodes[best].has_id) {
                int top[K_CLOSEST], nt = closest_answered(top, K_CLOSEST);
                if (nt == K_CLOSEST && !closer(s_nodes[best].id, s_nodes[top[K_CLOSEST - 1]].id)) break;
            }
            s_nodes[best].queried = 1;
            send_query(best, 0, 0);
            sent++;
        }
        found = s_npeers;
        LeaveCriticalSection(&s_lock);
        if (!sent) {
            if (++idle_rounds > 6) break;     /* nothing left to ask; give answers time */
        } else {
            idle_rounds = 0;
        }
        Sleep(250);
    }

    EnterCriticalSection(&s_lock);
    if (announce) {
        int top[K_CLOSEST], nt = closest_answered(top, K_CLOSEST);
        for (int i = 0; i < nt; i++)
            if (s_nodes[top[i]].token_len)
                send_query(top[i], 1, port);
    }
    found = s_npeers < max_peers ? s_npeers : max_peers;
    if (peers) memcpy(peers, s_peers, sizeof(struct sockaddr_in) * found);
    LeaveCriticalSection(&s_lock);
    LeaveCriticalSection(&s_busy);
    return found;
}
