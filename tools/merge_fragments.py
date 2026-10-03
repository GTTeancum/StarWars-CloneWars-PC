#!/usr/bin/env python3
"""Re-join functions the disassembler split after an early `ret`.

The disassembler starts a new function at the first instruction after a `ret`
when nothing marks it as interior code. MSVC emits early returns in the middle
of a body, so a routine like

    0x000FCE70  ... jne 0x000FCF4F ... ret      <- detected end
    0x000FCED8  push ebx ... 0x000FCF4F: pop edi; pop esi; add esp, 0x18; ret

comes out as two functions, and the branch from the first half into the
*interior* of the second (0x000FCF4F) has no target: the recompiler turns it
into a call to a no-op stub, so the routine returns without its epilogue and
its caller's stack is off by the frame size.

Seeding the interior target as a function start is the wrong fix -- a seed is
a hard boundary, and it truncated sub_000191C0's destructor at 0x00019D23.
Instead this extends the first function's range over the contiguous piece it
branches into, so the target becomes a local label. The second piece keeps its
own entry (overlapping ranges are fine), so direct callers of it still link.

Only forward, contiguous merges: the piece must start exactly where the
function ends (or after nothing but nop/int3 padding -- none in practice,
since padding means a real boundary, so padding stops the merge). Repeats to a
fixpoint, because an extended range can expose further out-of-range branches.

Usage:
    python tools/merge_fragments.py tools/disasm_seedsNNN/functions.json
Rewrites the file in place (a .premerge copy is kept) and prints the merges.
"""

import bisect
import json
import os
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(HERE)), "xboxrecomp"))

import capstone  # noqa: E402
from tools.xbe_parser.xbe_parser import XBEParser  # noqa: E402

XBE = os.path.join(os.path.dirname(HERE), "game", "default.xbe")
MAX_SPAN = 0x10000


def main():
    path = sys.argv[1]
    funcs = json.load(open(path))
    xbe = XBEParser(XBE).parse()

    def read(va, n):
        for s in xbe.sections:
            if s.virtual_addr <= va < s.virtual_addr + s.virtual_size:
                off = va - s.virtual_addr
                n = min(n, s.raw_size - off)
                if n <= 0:
                    return b""
                return xbe.raw_data[s.raw_addr + off:s.raw_addr + off + n]
        return b""

    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    by_start = {int(f["start"], 16): f for f in funcs}
    starts = sorted(by_start)
    ends = {a: int(by_start[a]["end"], 16) for a in starts}

    def containing(t):
        """Function whose original range holds t, starting at or after lo."""
        i = bisect.bisect_right(starts, t) - 1
        while i >= 0:
            a = starts[i]
            if a < t < ends[a]:
                return a
            if t - a > MAX_SPAN:
                break
            i -= 1
        return None

    merged = 0
    for a in starts:
        f = by_start[a]
        if f.get("section", ".text") != ".text":
            continue
        end = ends[a]
        joined = []
        changed = True
        while changed:
            changed = False
            code = read(a, end - a)
            for insn in md.disasm(code, a):
                if not insn.group(capstone.CS_GRP_JUMP):
                    continue
                if not insn.operands or insn.operands[0].type != capstone.x86.X86_OP_IMM:
                    continue
                t = insn.operands[0].imm
                if a <= t < end or t in by_start:
                    continue
                if t < end:
                    continue            # backward: not handled here
                # Walk contiguous pieces from `end` until one contains t.
                cur, chain = end, []
                while cur <= t and cur - a < MAX_SPAN:
                    if cur not in by_start:
                        chain = None
                        break
                    chain.append(cur)
                    cur = max(cur, ends[cur])
                    if cur > t:
                        break
                if not chain or cur <= t:
                    continue
                end = cur
                joined.extend(chain)
                changed = True
                break
        if end != ends[a]:
            print("  %s: end 0x%08X -> 0x%08X (joins %s)"
                  % (f["name"], ends[a], end, " ".join("0x%08X" % c for c in joined)))
            f["end"] = "0x%08X" % end
            f["size"] = end - a
            merged += 1

    shutil.copyfile(path, path + ".premerge")
    json.dump(funcs, open(path, "w"), indent=2)
    print("merged %d functions" % merged)


if __name__ == "__main__":
    main()
