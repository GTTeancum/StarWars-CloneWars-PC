/* dht.h -- join codes through the BitTorrent DHT (see dht.c). */
#ifndef CW_DHT_H
#define CW_DHT_H

#include <stdint.h>
#include <winsock2.h>

#define DHT_MAX_PEERS 32

void dht_init(SOCKET sock);
void dht_on_packet(const uint8_t *p, int n, const struct sockaddr_in *from);
void dht_sha1(const void *data, unsigned len, uint8_t out[20]);
/* The info-hash for a join code; join_side selects the joiners' entry. */
void dht_code_hash(const char *code, int join_side, uint8_t out[20]);
/* Look up who is listed under `hash`, optionally listing this machine there
 * too (on `port`). Blocks up to timeout_ms; returns the number of peers. */
int dht_lookup(const uint8_t hash[20], int announce, unsigned short port,
               struct sockaddr_in *peers, int max_peers, int timeout_ms);

#endif
