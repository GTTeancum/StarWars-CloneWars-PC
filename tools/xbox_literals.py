#!/usr/bin/env python3
"""Per-function literal sets for the Xbox default.xbe: the strings, float
constants and distinctive integer immediates each function references.

Both the Xbox and GameCube builds came from the same 2002 source, so these
sets are what a function keeps across compilers and CPUs. tools/gc_names.py
matches them against the GameCube decompilation's literals to put the
original names on our functions.

Function boundaries come from the generated code's "Original: 0xA - 0xB"
headers. Output: game/xbox_literals.json (generated, not committed).

Usage: python tools/xbox_literals.py
"""
import glob, json, os, re, struct, sys

import capstone

HERE = os.path.dirname(os.path.abspath(__file__))
PROJECT = os.path.dirname(HERE)
XBE = os.path.join(PROJECT, "game", "default.xbe")
GEN = os.path.join(PROJECT, "src", "recomp", "gen")
OUT = os.path.join(PROJECT, "game", "xbox_literals.json")


class Image:
    """The XBE's sections mapped at their virtual addresses."""

    def __init__(self, path):
        d = open(path, "rb").read()
        base, = struct.unpack_from("<I", d, 0x104)
        nsec, sec_va = struct.unpack_from("<II", d, 0x11C)
        self.secs = []
        for i in range(nsec):
            o = sec_va - base + i * 0x38
            flags, va, vsz, raw, rsz = struct.unpack_from("<IIIII", d, o)
            name_va, = struct.unpack_from("<I", d, o + 0x14)
            no = name_va - base
            name = d[no:d.index(b"\0", no)].decode("latin1")
            self.secs.append((name, va, vsz, d[raw:raw + rsz]))

    def read(self, va, n):
        for name, sva, vsz, data in self.secs:
            if sva <= va < sva + len(data):
                return data[va - sva:va - sva + n]
        return None

    def section(self, name):
        for s in self.secs:
            if s[0] == name:
                return s
        return None


def cstring(img, va):
    b = img.read(va, 200)
    if not b:
        return None
    end = b.find(b"\0")
    if end < 3:
        return None
    s = b[:end]
    if all(32 <= c < 127 or c in (9, 10, 13) for c in s):
        return s.decode("latin1")
    return None


def nice_float(bits):
    """A float bit pattern a programmer would have typed (0.5, 30.0, 1e-3...)."""
    if bits in (0, 0x80000000):
        return None
    f, = struct.unpack("<f", struct.pack("<I", bits))
    if not (1e-6 < abs(f) < 1e7) or f != f:
        return None
    return float("%.6g" % f)


def functions():
    pat = re.compile(r"^ \* Original: 0x([0-9A-F]+) - 0x([0-9A-F]+)", re.M)
    out = []
    for f in sorted(glob.glob(os.path.join(GEN, "recomp_*.c"))):
        out += [(int(a, 16), int(b, 16)) for a, b in pat.findall(open(f, encoding="latin1").read())]
    return sorted(set(out))


def main():
    img = Image(XBE)
    text = img.section(".text")
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    lo_img = 0x10000
    hi_img = max(va + vsz for _, va, vsz, _ in img.secs)
    res = {}
    for start, end in functions():
        code = img.read(start, end - start)
        if not code:
            continue
        strs, floats, ints, calls, order = set(), set(), set(), set(), []
        for ins in md.disasm(code, start):
            fp = ins.mnemonic.startswith("f")
            for op in ins.operands:
                if op.type == capstone.x86.X86_OP_IMM:
                    v = op.imm & 0xFFFFFFFF
                    if ins.mnemonic == "call":
                        calls.add(v)
                        order.append("%08X" % v)
                        continue
                    if lo_img <= v < hi_img:
                        s = cstring(img, v)
                        if s:
                            strs.add(s)
                        continue
                    fl = nice_float(v)
                    if fl is not None and ins.mnemonic in ("mov", "push") and op.size == 4 and v > 0xFFFF:
                        floats.add(fl)
                    elif v > 0xFFFF and v < 0xFFFF0000:
                        ints.add(v)
                elif op.type == capstone.x86.X86_OP_MEM and op.mem.base == 0 and op.mem.index == 0:
                    a = op.mem.disp & 0xFFFFFFFF
                    if fp and lo_img <= a < hi_img:
                        b = img.read(a, 8)
                        if b and op.size == 4:
                            fl = nice_float(struct.unpack_from("<I", b)[0])
                            if fl is not None:
                                floats.add(fl)
                        elif b and op.size == 8:
                            d, = struct.unpack_from("<d", b)
                            if 1e-6 < abs(d) < 1e7:
                                floats.add(float("%.6g" % d))
                    elif lo_img <= a < hi_img:
                        s = cstring(img, a)
                        if s:
                            strs.add(s)
        if strs or floats or ints or order:
            res["%08X" % start] = {"end": "%08X" % end, "s": sorted(strs), "f": sorted(floats),
                                   "i": sorted(ints), "co": order}
    json.dump(res, open(OUT, "w"))
    print("%d functions with literals -> %s" % (len(res), OUT))


if __name__ == "__main__":
    main()
