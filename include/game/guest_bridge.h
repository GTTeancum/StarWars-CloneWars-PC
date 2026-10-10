/*
 * guest_bridge.h -- native (recovered) game code calling the recompiled
 * title, and the recompiled title calling native code.
 *
 * Recovered C++ under src/game is compiled natively. Its objects live in the
 * title's memory (a host pointer is the guest address + a fixed offset), and
 * the engine functions it calls are still the recompiled ones. The generated
 * glue (src/game/glue/glue_gen.cpp, tools/gc_bind.py) gives every such engine
 * function a native stand-in that forwards through gb_call.
 *
 * Kept free of recomp_types.h on purpose: that header #defines eax, ecx, esp
 * and friends, which would break C++ code that includes it.
 */
#ifndef GAME_GUEST_BRIDGE_H
#define GAME_GUEST_BRIDGE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Guest address <-> host pointer. gb_guest returns 0 for a host pointer
 * outside the title's memory. */
void *gb_host(uint32_t guest);
uint32_t gb_guest(const void *host);

/* Call recompiled function `va`. `spec` is the return kind then one code per
 * parameter, as the native caller passed them (args: the caller's stack, one
 * dword per 4 bytes):
 *   return: v void, i 32-bit (eax), b bool (al), f float/double (x87 ST0)
 *   params: i 4-byte value (int, float, bool, char, enum)
 *           q<n> struct passed by value, n bytes
 *           s C string: a host string is interned into title memory
 *           p<n> pointer/reference to an n-byte object; a host object is
 *               copied into title memory for the call and copied back after
 * `self` is the guest `this` for a member function (0 otherwise).
 * A float/double result is stored in *fret. */
uint32_t gb_call(uint32_t va, uint32_t self, const char *spec, const uint32_t *args, double *fret,
                 const char *name);
/* CW_BRIDGE_TRACE=1 logs every gb_call with its name, arguments and result. */

/* Inside a hook (a native function standing in for recompiled sub_XXXXXXXX):
 * the guest `this` (ecx), and the return: eax plus the bytes of stack
 * arguments the callee pops (0 for cdecl and argument-less thiscall). */
uint32_t gb_this(void);
uint32_t gb_arg(int index);
void gb_return(uint32_t eax, uint32_t popped_arg_bytes);

/* Unmapped engine function reached from native code: logged once per name. */
void gb_trap(const char *name);

/* First entry into a recovered function (hooks log it once). */
void gb_note(const char *name);

/* 0 when CW_RECOVERED=0: hooks run the translated body instead. */
int gb_recovered_on(void);

#ifdef __cplusplus
}
#endif

#endif
