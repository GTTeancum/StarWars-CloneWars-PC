#!/usr/bin/env python3
"""Third naming pass: fill in whole translation units between known names.

A linker keeps each translation unit's code together, and within a unit the
Xbox build lays functions out in the GameCube build's order or exactly in
reverse (MSVC emits a TU's functions back to front). So between two
consecutive named functions of one TU (anchors from tools/gc_names.py and
tools/gc_callnames.py), if the Xbox build has exactly as many functions
between them as the GameCube TU has, they pair up one to one -- overloads
included, which the literal and call passes cannot separate. New names are
anchors for the next round.

Inputs: the handoff's analysis/tu_functions.csv (every GameCube function with
its TU, address, size and signature), game/gc_names.csv,
game/gc_callnames.csv, generated function extents.
Output: game/gc_tunames.csv  xbox_va,signature,tu,gc_address,source
"""
import csv, glob, os, re, sys
from collections import defaultdict

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import gc_names  # noqa: E402

PROJECT = os.path.dirname(HERE)
GAME = os.path.join(PROJECT, "game")
OUT = os.path.join(GAME, "gc_tunames.csv")


def xbox_functions():
    pat = re.compile(r"^ \* Original: 0x([0-9A-F]+) - 0x([0-9A-F]+)", re.M)
    s = set()
    for f in glob.glob(os.path.join(PROJECT, "src", "recomp", "gen", "recomp_*.c")):
        s |= {(int(a, 16), int(b, 16)) for a, b in pat.findall(open(f, encoding="latin1").read())}
    return sorted(s)


def main():
    handoff = sys.argv[1] if len(sys.argv) > 1 else gc_names.DEFAULT_HANDOFF
    gcf = list(csv.DictReader(open(os.path.join(handoff, "analysis", "tu_functions.csv"))))
    by_tu = defaultdict(list)
    for r in gcf:
        r["gaddr"] = int(r["address"], 16)
        r["base"] = r["signature"].split("(")[0]
        by_tu[r["translation_unit"]].append(r)
    for tu in by_tu.values():
        tu.sort(key=lambda r: r["gaddr"])
    name_count = defaultdict(int)
    for r in gcf:
        name_count[r["base"]] += 1

    xf = xbox_functions()
    xstarts = [a for a, b in xf]
    xindex = {a: i for i, a in enumerate(xstarts)}

    # anchors: unambiguous names only (an overloaded name pins no single function)
    assigned = {}            # gc address -> xbox va
    source = {}
    for f, tag in (("gc_names.csv", "literals"), ("gc_callnames.csv", "calls")):
        for r in csv.DictReader(open(os.path.join(GAME, f))):
            va = int(r["xbox_va"], 16)
            if va not in xindex:
                continue
            cands = [g for g in gcf if g["base"] == r["name"] or g["base"].endswith("::" + r["name"])]
            if len(cands) == 1 and name_count[cands[0]["base"]] == 1:
                assigned[cands[0]["gaddr"]] = va
                source[cands[0]["gaddr"]] = tag

    for rnd in range(6):
        added = 0
        used = set(assigned.values())
        for tu, funcs in by_tu.items():
            idx = [i for i, g in enumerate(funcs) if g["gaddr"] in assigned]
            for a, b in zip(idx, idx[1:]):
                if b - a < 2:
                    continue
                xa, xb = xindex[assigned[funcs[a]["gaddr"]]], xindex[assigned[funcs[b]["gaddr"]]]
                gap_g = funcs[a + 1:b]
                if xa < xb:
                    gap_x = xstarts[xa + 1:xb]
                else:
                    gap_x = xstarts[xb + 1:xa][::-1]
                if len(gap_x) != len(gap_g) or any(x in used for x in gap_x):
                    continue
                # loose size sanity: PowerPC and x86 code are within a few times of each other
                gs = sum(int(g["size"]) for g in gap_g)
                xs = sum(xf[xindex[x]][1] - x for x in gap_x)
                if gs and not (0.2 <= xs / gs <= 5.0):
                    continue
                for g, x in zip(gap_g, gap_x):
                    assigned[g["gaddr"]] = x
                    source[g["gaddr"]] = "tu-order"
                    used.add(x)
                    added += 1
        print("round %d: +%d (total %d)" % (rnd, added, len(assigned)))
        if not added:
            break

    gmap = {g["gaddr"]: g for g in gcf}
    with open(OUT, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["xbox_va", "signature", "tu", "gc_address", "source"])
        for ga, va in sorted(assigned.items(), key=lambda kv: kv[1]):
            g = gmap[ga]
            w.writerow(["%08X" % va, g["signature"], g["translation_unit"], g["address"], source[ga]])
    print("%d functions -> %s" % (len(assigned), OUT))


if __name__ == "__main__":
    main()
