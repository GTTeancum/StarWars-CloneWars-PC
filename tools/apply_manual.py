#!/usr/bin/env python3
"""Apply recomp_manual.c's replacements to already-generated code.

`tools.recomp --exclude-manual` does this at generation time: it scans
recomp_manual.c for `void sub_XXXXXXXX(void)` definitions and declares those
addresses without generating a body, so the hand-written definition links
instead. Doing it that way costs a full re-lift -- about eight minutes here,
plus a full rebuild afterwards because every generated file's timestamp moves.

During bring-up that is the wrong cycle time. Each new override is one
function; this script removes just that function's generated body, so MSBuild
recompiles one file and relinks. Same end state, minutes instead of a quarter
of an hour.

Run it after editing recomp_manual.c and before building. It is idempotent: a
body already removed is left alone. When the set of overrides settles, a
regeneration with --exclude-manual reproduces exactly this, and the manual
file stays the single source of truth either way.

Usage:
    python tools/apply_manual.py [--check]

    --check   report what would change, touch nothing
"""

import argparse
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
PROJECT = os.path.dirname(HERE)
TOOLKIT = os.path.join(os.path.dirname(PROJECT), "xboxrecomp")

MANUAL = os.path.join(PROJECT, "src", "recomp_manual.c")
# High-level replacements of the title's statically linked XDK libraries live
# beside it as src/hle_*.c (hle_dsound.c: DirectSound). They override
# generated bodies exactly the way recomp_manual.c does.
HLE_GLOB = os.path.join(PROJECT, "src", "hle_*.c")
GEN = os.path.join(PROJECT, "src", "recomp", "gen")

# Detection is the toolkit's own, not a second regex that can disagree with it.
# manual_scan.py exists precisely because two regexes drifted apart and every
# disagreement was a link error.
sys.path.insert(0, TOOLKIT)
from tools.recomp.manual_scan import definition_names  # noqa: E402

MARKER = "/* body replaced by recomp_manual.c -- see tools/apply_manual.py:"


def body_re(name):
    """Match a whole generated definition of `name`, brace to closing brace.

    Generated bodies always open `{` on the line after the signature and close
    with `}` in column 0, so anchoring on those is exact -- no brace counting,
    and no risk of stopping at a nested block.
    """
    return re.compile(
        r"^void " + re.escape(name) + r"\(void\)\n\{\n.*?^\}\n",
        re.M | re.S)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--check", action="store_true",
                    help="report what would change, touch nothing")
    args = ap.parse_args()

    import glob
    files = [MANUAL] + sorted(glob.glob(HLE_GLOB))
    names = set()
    wrapped = set()
    for path in files:
        defs = set(definition_names(path))
        names |= defs
        # A replacement that calls sub_X_gen keeps the generated body under
        # that name instead of deleting it: hle_d3d8.c replaces Direct3D on
        # the 32-bit build and falls through to the title's own code on x64.
        with open(path, encoding="utf-8", errors="replace") as fh:
            text = fh.read()
        for n in defs:
            if n + "_gen(" in text:
                wrapped.add(n)
    if not names:
        print("no sub_XXXXXXXX replacements defined; nothing to do.")
        return 0

    print("replacements defined in %s: %s"
          % (", ".join(os.path.basename(f) for f in files),
             ", ".join(sorted(names))))

    sources = sorted(f for f in os.listdir(GEN) if f.endswith(".c"))
    removed, already, missing = [], [], set(names)

    for fname in sources:
        path = os.path.join(GEN, fname)
        with open(path, encoding="utf-8", errors="replace") as fh:
            src = fh.read()
        original = src

        for name in sorted(names):
            if ("\n" + MARKER + " " + name + " */\n") in src:
                already.append((name, fname))
                missing.discard(name)
                continue
            if name in wrapped:
                gen_sig = "\nvoid " + name + "_gen(void)\n{"
                if gen_sig in src:
                    already.append((name, fname))
                    missing.discard(name)
                    continue
                sig = "\nvoid " + name + "(void)\n{"
                if sig in src:
                    src = src.replace(sig, gen_sig, 1)
                    removed.append((name + " -> _gen", fname))
                    missing.discard(name)
                continue
            pat = body_re(name)
            if not pat.search(src):
                continue
            src = pat.sub(MARKER + " " + name + " */\n", src, count=1)
            removed.append((name, fname))
            missing.discard(name)

        if src != original and not args.check:
            with open(path, "w", encoding="utf-8", newline="") as fh:
                fh.write(src)

    for name, fname in removed:
        print("  %s body removed from %s" % (name, fname))
    for name, fname in already:
        print("  %s already removed from %s" % (name, fname))
    for name in sorted(missing):
        # Not fatal on its own: a replacement can legitimately name an address
        # the lifter never generated. It is worth a line, because the usual
        # cause is a typo'd address, and the symptom of that is an override
        # that silently never runs.
        print("  WARNING: %s is not defined by any generated file" % name)

    if args.check:
        print("\n--check: nothing written")
    return 0


if __name__ == "__main__":
    sys.exit(main())
