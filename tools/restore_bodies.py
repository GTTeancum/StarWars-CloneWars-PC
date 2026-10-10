#!/usr/bin/env python3
"""Put translated bodies back, renamed to sub_X_gen, for functions that
tools/apply_manual.py removed and that a hook now wraps.

apply_manual.py removes a generated body outright when a hand-written file
defines the function, and renames it to sub_X_gen when the hand-written file
also calls sub_X_gen. A hook that was first generated as a replacement and
later as a wrapper leaves the body removed; this copies it back from a fresh
regeneration (tools.recomp into another directory) as the _gen definition.

Usage: python tools/restore_bodies.py <regenerated gen dir>
"""
import glob, os, re, sys

HERE = os.path.dirname(os.path.abspath(__file__))
GEN = os.path.join(os.path.dirname(HERE), "src", "recomp", "gen")
MARKER = re.compile(r"^/\* body replaced by recomp_manual\.c -- see tools/apply_manual\.py: (sub_[0-9A-F]{8}) \*/\n", re.M)


def body_of(src, name):
    m = re.search(r"^void " + name + r"\(void\)\n\{\n.*?^\}\n", src, re.M | re.S)
    return m.group(0) if m else None


def main():
    regen = sys.argv[1]
    hooked = set()
    glue = os.path.join(os.path.dirname(HERE), "src", "game", "glue", "glue_gen.cpp")
    for m in re.finditer(r"void (sub_[0-9A-F]{8})_gen\(void\);", open(glue).read()):
        hooked.add(m.group(1))
    fresh = {}
    for f in glob.glob(os.path.join(regen, "recomp_*.c")):
        fresh[os.path.basename(f)] = open(f, encoding="utf-8", errors="replace").read()
    done = 0
    for f in sorted(glob.glob(os.path.join(GEN, "recomp_*.c"))):
        src = open(f, encoding="utf-8", errors="replace").read()
        out = src
        for m in MARKER.finditer(src):
            name = m.group(1)
            if name not in hooked:
                continue
            body = None
            for text in fresh.values():
                body = body_of(text, name)
                if body:
                    break
            if not body:
                print("  no regenerated body for %s" % name)
                continue
            body = body.replace("void %s(void)\n{" % name, "void %s_gen(void)\n{" % name, 1)
            out = out.replace(m.group(0), body, 1)
            done += 1
        if out != src:
            open(f, "w", encoding="utf-8", newline="").write(out)
    print("%d bodies restored as _gen" % done)


if __name__ == "__main__":
    main()
