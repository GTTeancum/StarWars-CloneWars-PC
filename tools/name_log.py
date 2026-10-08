#!/usr/bin/env python3
"""Annotate a log (or any text) with the original function names recovered
from the GameCube decompilation: every 6-8 digit hex address that falls in
a named Xbox function gets "<name+off>" appended.

Names: game/gc_names.csv and game/gc_callnames.csv (tools/gc_names.py,
tools/gc_callnames.py). Function extents: game/xbox_literals.json.

Usage: python tools/name_log.py <log> [> annotated.txt]
       python tools/name_log.py --lookup 0006BC80
"""
import bisect, csv, json, os, re, sys

HERE = os.path.dirname(os.path.abspath(__file__))
GAME = os.path.join(os.path.dirname(HERE), "game")


def load():
    names = {}
    for f in ("gc_names.csv", "gc_callnames.csv"):
        p = os.path.join(GAME, f)
        if os.path.exists(p):
            for r in csv.DictReader(open(p)):
                names.setdefault(int(r["xbox_va"], 16), r["name"])
    ext = json.load(open(os.path.join(GAME, "xbox_literals.json")))
    starts = sorted(names)
    ends = {va: int(ext[k]["end"], 16) for k in ext for va in [int(k, 16)] if va in names}
    return names, starts, ends


def lookup(va, names, starts, ends):
    i = bisect.bisect_right(starts, va) - 1
    if i < 0:
        return None
    s = starts[i]
    if va == s:
        return names[s]
    if va < ends.get(s, s + 1):
        return "%s+%X" % (names[s], va - s)
    return None


def main():
    names, starts, ends = load()
    if sys.argv[1:2] == ["--lookup"]:
        for a in sys.argv[2:]:
            print(a, lookup(int(a, 16), names, starts, ends) or "?")
        return
    pat = re.compile(r"\b(?:0x)?([0-9A-Fa-f]{6,8})\b")

    def sub(m):
        n = lookup(int(m.group(1), 16), names, starts, ends)
        return m.group(0) + ("<%s>" % n if n else "")

    for line in open(sys.argv[1], encoding="latin1"):
        sys.stdout.write(pat.sub(sub, line))


if __name__ == "__main__":
    main()
