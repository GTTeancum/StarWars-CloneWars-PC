#!/usr/bin/env python3
"""Second pass of tools/gc_names.py: name the functions that named functions
call, by lining up call sequences.

For each Xbox function already named from the GameCube decompilation, the
GameCube source gives the order of its calls (MissionUtility::AddObjective,
SetCurHealth, ...) and the Xbox code gives the order of its call targets.
Both compilers keep straight-line call order, so the two sequences align
like text. Votes from every aligned pair across all named functions decide
each callee's address; alignment is refined with the current mapping as
anchors and repeated.

Output: game/gc_callnames.csv  xbox_va,name,votes,share
"""
import collections, csv, difflib, json, os, re, sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import gc_names  # noqa: E402

PROJECT = os.path.dirname(HERE)
OUT = os.path.join(PROJECT, "game", "gc_callnames.csv")

CALL = re.compile(r"((?:[A-Za-z_]\w*::)*~?[A-Za-z_]\w*)\s*\(")
SKIP = {"if", "for", "while", "switch", "return", "sizeof", "int", "float", "bool", "char",
        "unsigned", "Vector", "Quat", "Timer", "static_cast", "reinterpret_cast", "const_cast"}


def gc_calls(body):
    body = gc_names.STR.sub('""', body)
    return [n for n in CALL.findall(body) if n.split("::")[-1] not in SKIP and not n.startswith("m")]


def bodies(src_dir):
    """name -> body text, for every top-level definition (same walk as gc_names)."""
    out = {}
    orig = gc_names.literals
    gc_names.literals = lambda body: {("b", body)}
    try:
        for name, fname, toks in gc_names.gc_functions(src_dir):
            out[name] = next(iter(toks))[1]
    finally:
        gc_names.literals = orig
    return out


def align(gseq, xseq, mapping):
    """Pairs (gc_name, xbox_va) from aligning the two call sequences."""
    g2 = [mapping.get(n, "?" + n) for n in gseq]
    sm = difflib.SequenceMatcher(None, g2, xseq, autojunk=False)
    pairs = []
    prev_a = prev_b = 0
    for a, b, size in sm.get_matching_blocks():
        # equal-length gaps between anchors pair up one to one
        if a - prev_a == b - prev_b and a - prev_a > 0:
            pairs += [(gseq[prev_a + k], xseq[prev_b + k]) for k in range(a - prev_a)]
        pairs += [(gseq[a + k], xseq[b + k]) for k in range(size)]
        prev_a, prev_b = a + size, b + size
    return pairs


def main():
    handoff = sys.argv[1] if len(sys.argv) > 1 else gc_names.DEFAULT_HANDOFF
    xb = json.load(open(os.path.join(PROJECT, "game", "xbox_literals.json")))
    named = {r["xbox_va"]: r["name"] for r in csv.DictReader(open(os.path.join(PROJECT, "game", "gc_names.csv")))}
    src = bodies(os.path.join(handoff, "probes", "src"))
    found = {}
    # rounds: callees named in one round are callers in the next (MissionUtility
    # bodies name the engine functions they call)
    for rnd in range(5):
        callers = dict(named)
        callers.update({va: r[1] for va, r in found.items()})
        work = []
        for va, name in callers.items():
            body = src.get(name)
            x = xb.get(va)
            if body and x and x.get("co"):
                g = gc_calls(body)
                if g:
                    work.append((g, x["co"]))
        votes = collections.defaultdict(collections.Counter)
        for g, x in work:
            for i, n in enumerate(g):
                c = int(i * len(x) / len(g))
                for j in range(max(0, c - 2), min(len(x), c + 3)):
                    votes[n][x[j]] += 1
        mapping = {v: k for k, v in callers.items()}
        for it in range(4):
            for n, c in votes.items():
                va, v = c.most_common(1)[0]
                if v >= 2 and v >= 0.5 * sum(c.values()) and n not in mapping:
                    mapping[n] = va
            votes = collections.defaultdict(collections.Counter)
            for g, x in work:
                for n, va in align(g, x, mapping):
                    votes[n][va] += 1
            mapping = {v: k for k, v in callers.items()}
        before = len(found)
        for n, c in votes.items():
            va, v = c.most_common(1)[0]
            share = v / sum(c.values())
            if v >= 2 and share >= 0.6 and va not in named:
                if va not in found or v > found[va][2]:
                    found[va] = (va, n, v, share)
        print("round %d: %d callers, %d callees named" % (rnd, len(work), len(found)))
        if len(found) == before:
            break
    rows = sorted(found.values())
    with open(OUT, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["xbox_va", "name", "votes", "share"])
        for r in rows:
            w.writerow([r[0], r[1], r[2], "%.2f" % r[3]])
    print("%d callees named -> %s" % (len(rows), OUT))


if __name__ == "__main__":
    main()
