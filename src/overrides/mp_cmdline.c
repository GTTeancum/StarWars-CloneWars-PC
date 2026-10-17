/*
 * mp_cmdline.c -- local players for a multiplayer map started from the
 * command line ("multi4.wld -multiplayer" in RECOMP_CMDLINE).
 *
 * The title's own development path means to add two local players: game init
 * (0x0001A5E0) looks the command-line world up in the splitscreen and online
 * map tables and calls the add-player function for slots 0 and 1. But those
 * tables are filled from MultiplayerButtons.cfg / OnlineButtons.cfg by
 * 0x0001B250, which the main loop only runs after game init -- so the lookup
 * finds nothing and the map starts with no players: no splitscreen, nobody
 * spawns, and scripts that size themselves by player count (Academy) run
 * their setup for zero players.
 *
 * 0x0001B250 is wrapped here: once it has parsed the tables, and before the
 * mission loads, the lookup and the player adds the title would have done are
 * done. Players: CW_MP_PLAYERS (1-4, default 2); each player's slot is a
 * controller port, so set CW_PADS to the same number for virtual pads.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "recomp/gen/recomp_types.h"
#include "recomp/gen/recomp_funcs.h"

void sub_0001B250_gen(void);

#define MP_ENABLED      0x005EC730u   /* multiplayer flag (byte) */
#define MP_NUM_PLAYERS  0x005EC72Cu   /* MissionUtility::MPGetNumPlayers reads this */
#define CMDLINE_WORLD   0x003C302Cu   /* world name from the command line */
#define LEVEL_NAME2     0x003FC180u   /* second name the level loader takes */
#define SPLIT_RECS      0x003FC3B0u   /* splitscreen map records ... */
#define SPLIT_COUNT     0x0041AE50u
#define ONLINE_RECS     0x0041BDD8u   /* ... and online map records */
#define ONLINE_COUNT    0x0043A878u
#define REC_SIZE        0x4E8u        /* file name at +0x80, second name at +0xC0 */
#define FN_PROFILE      0x001165C0u   /* current profile; map index at +0x24 */
#define FN_ADD_PLAYER   0x00075640u   /* cdecl (slot, pick, name, team, b18, b1c, b20) */

/* Copy a guest string, lower-cased and without its extension. */
static void world_key(uint32_t va, char *out, int n)
{
    int k;
    for (k = 0; k < n - 1; k++) {
        char c = (char)MEM8(va + (uint32_t)k);
        if (!c || c == '.') break;
        out[k] = (char)tolower((unsigned char)c);
    }
    out[k] = 0;
}

static int find_record(uint32_t base, uint32_t count_va, const char *world)
{
    uint32_t i, n = MEM32(count_va);
    char key[64];
    for (i = 0; i < n && i < 64; i++) {
        world_key(base + i * REC_SIZE + 0x80, key, sizeof key);
        if (key[0] && !strcmp(key, world))
            return (int)i;
    }
    return -1;
}

/* Call a title function: cdecl, arguments pushed right to left. */
static uint32_t call_guest(uint32_t va, int argc, const uint32_t *argv)
{
    recomp_func_t fn = recomp_lookup(va);
    uint32_t saved = g_esp;
    int k;
    for (k = argc - 1; k >= 0; k--) PUSH32(g_esp, argv[k]);
    PUSH32(g_esp, 0);              /* return address */
    fn();
    g_esp = saved;
    return g_eax;
}

static void add_cmdline_players(void)
{
    char world[64];
    uint32_t base, profile, k;
    int idx, players;
    const char *e;

    if (!MEM8(MP_ENABLED) || !MEM8(CMDLINE_WORLD) || MEM32(MP_NUM_PLAYERS) != 0)
        return;
    world_key(CMDLINE_WORLD, world, sizeof world);
    base = SPLIT_RECS;
    idx = find_record(SPLIT_RECS, SPLIT_COUNT, world);
    if (idx < 0) { base = ONLINE_RECS; idx = find_record(ONLINE_RECS, ONLINE_COUNT, world); }
    if (idx < 0) {
        fprintf(stderr, "[MP] %s: not in the multiplayer map tables; no players added\n", world);
        return;
    }
    players = 2;
    if ((e = getenv("CW_MP_PLAYERS")) && *e >= '1' && *e <= '4') players = *e - '0';

    profile = call_guest(FN_PROFILE, 0, NULL);
    if (profile) MEM32(profile + 0x24) = (uint32_t)idx;
    for (k = 0; k < 0x40; k++) {
        uint8_t c = MEM8(base + (uint32_t)idx * REC_SIZE + 0xC0 + k);
        MEM8(LEVEL_NAME2 + k) = c;
        if (!c) break;
    }
    for (k = 0; k < (uint32_t)players; k++) {
        /* The name is copied, so it can live on the guest stack for the call. */
        uint32_t saved = g_esp, name, args[7];
        const char *s = "Player 1";
        int i;
        g_esp -= 16;
        name = g_esp;
        for (i = 0; s[i]; i++) MEM8(name + (uint32_t)i) = (uint8_t)s[i];
        MEM8(name + 7) = (uint8_t)('1' + k);
        MEM8(name + 8) = 0;
        args[0] = k; args[1] = 0; args[2] = name; args[3] = 0xFFFFFFFFu;  /* team: alternate */
        args[4] = 0; args[5] = 1; args[6] = 1;
        call_guest(FN_ADD_PLAYER, 7, args);
        g_esp = saved;
    }
    fprintf(stderr, "[MP] %s: %s map %d, %u player(s) added\n", world,
            base == SPLIT_RECS ? "splitscreen" : "online", idx, MEM32(MP_NUM_PLAYERS));
}

void sub_0001B250(void)
{
    uint32_t eax, ecx, edx;
    sub_0001B250_gen();
    eax = g_eax; ecx = g_ecx; edx = g_edx;
    add_cmdline_players();
    g_eax = eax; g_ecx = ecx; g_edx = edx;
}
