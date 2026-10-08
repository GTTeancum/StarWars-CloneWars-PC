#!/usr/bin/env python3
"""Put the original names on our Xbox functions using the GameCube
decompilation (a collaborator's matching decomp of the PAL GameCube build).

Both builds came from the same 2002 source. A function keeps its string,
float and distinctive integer literals across compilers and CPUs, so each
reconstructed GameCube function (probes/src/*.cpp) is matched to the Xbox
function whose literal set it shares most, weighted by rarity. Accepted only
with a clear margin over the runner-up.

Inputs: game/xbox_literals.json (tools/xbox_literals.py) and the handoff
folder (default ../handoff/CloneWars_Handoff_2026-10-03).
Output: game/gc_names.csv  xbox_va,name,score,margin,shared

Usage: python tools/gc_names.py [handoff_dir]
"""
import collections, csv, glob, json, math, os, re, sys

HERE = os.path.dirname(os.path.abspath(__file__))
PROJECT = os.path.dirname(HERE)
DEFAULT_HANDOFF = os.path.join(os.path.dirname(PROJECT), "handoff", "CloneWars_Handoff_2026-10-03")
OUT = os.path.join(PROJECT, "game", "gc_names.csv")

STR = re.compile(r'"((?:[^"\\\n]|\\.)*)"')
FLT = re.compile(r'(?<![\w.])(\d+\.\d*|\.\d+|\d+(?:\.\d*)?[eE][-+]?\d+)[fF]?(?![\w.])')
HEX = re.compile(r'\b0x([0-9A-Fa-f]{5,8})\b')
DEFEND = re.compile(r'([A-Za-z_~][\w:<>,~]*)\s*\(([^;{}()]*(?:\([^;{}()]*\)[^;{}()]*)*)\)\s*(?:const\s*)?\{\Z')


def strip_comments(t):
    t = re.sub(r"/\*.*?\*/", " ", t, flags=re.S)
    return re.sub(r"//[^\n]*", " ", t)


def gc_functions(src_dir):
    """(name, file, literals) for every top-level function definition."""
    out = []
    for path in sorted(glob.glob(os.path.join(src_dir, "*.cpp"))):
        t = strip_comments(open(path, encoding="utf-8", errors="replace").read())
        i, depth, n = 0, 0, len(t)
        scope = []          # class/namespace names whose braces we are inside
        pending = None
        while i < n:
            c = t[i]
            if c == '"':
                j = i + 1
                while j < n and t[j] != '"':
                    j += 2 if t[j] == "\\" else 1
                i = j + 1
                continue
            if c == "{":
                head = t[max(0, i - 300):i]
                m = DEFEND.search(head + "{")
                kw = re.search(r"\b(class|struct|namespace|union|enum)\s+(\w+)[^;{}()]*$", head)
                if kw:
                    scope.append(kw.group(2) if kw.group(1) in ("class", "struct", "namespace") else None)
                    depth += 1
                    i += 1
                    continue
                if m and not re.search(r"\b(if|for|while|switch|return|sizeof)\s*$", m.group(1)):
                    # function body: scan to the matching brace
                    j, d = i + 1, 1
                    while j < n and d:
                        if t[j] == '"':
                            k = j + 1
                            while k < n and t[k] != '"':
                                k += 2 if t[k] == "\\" else 1
                            j = k + 1
                            continue
                        d += {"{": 1, "}": -1}.get(t[j], 0)
                        j += 1
                    body = t[i:j]
                    name = m.group(1).split()[-1]
                    owner = [s for s in scope if s]
                    if "::" not in name and owner:
                        name = "::".join(owner + [name])
                    lits = literals(body)
                    if lits:
                        out.append((name, os.path.basename(path), lits))
                    i = j
                    continue
                scope.append(None)
                depth += 1
            elif c == "}":
                if scope:
                    scope.pop()
                depth -= 1
            i += 1
    return out


def unescape(s):
    try:
        return bytes(s, "latin1").decode("unicode_escape")
    except Exception:
        return s


def literals(body):
    toks = set()
    for s in STR.findall(body):
        s = unescape(s)
        if len(s) >= 3:
            toks.add(("s", s))
    for f in FLT.findall(body):
        try:
            v = float(f)
        except ValueError:
            continue
        if v not in (0.0, 1.0) and 1e-6 < abs(v) < 1e7:
            toks.add(("f", float("%.6g" % v)))
    for h in HEX.findall(body):
        v = int(h, 16)
        if v > 0xFFFF:
            toks.add(("i", v))
    return toks


def main():
    handoff = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_HANDOFF
    xb = json.load(open(os.path.join(PROJECT, "game", "xbox_literals.json")))
    xtok = {}
    for va, r in xb.items():
        t = {("s", s) for s in r["s"]} | {("f", f) for f in r["f"]} | {("i", i) for i in r["i"]}
        xtok[va] = t
    df = collections.Counter(t for ts in xtok.values() for t in ts)
    N = len(xtok)
    idf = lambda t: math.log((N + 1) / (df.get(t, 0) + 1))
    index = collections.defaultdict(list)
    for va, ts in xtok.items():
        for t in ts:
            index[t].append(va)

    gc = gc_functions(os.path.join(handoff, "probes", "src"))
    rows = []
    for name, fname, toks in gc:
        known = [t for t in toks if t in df]
        if not known:
            continue
        score = collections.Counter()
        for t in known:
            w = idf(t) * (2.0 if t[0] == "s" else 1.0)
            for va in index[t]:
                score[va] += w
        # penalise Xbox functions carrying many literals the GameCube one lacks
        best = []
        for va, s in score.most_common(20):
            extra = len(xtok[va] - toks)
            best.append((s - 0.15 * extra, va))
        best.sort(reverse=True)
        top, va = best[0]
        second = best[1][0] if len(best) > 1 else 0.0
        shared = len(xtok[va] & toks)
        rows.append((va, name, fname, top, top - second, shared, len(toks)))

    # one name per Xbox function: keep the strongest claim
    by_va = {}
    for r in rows:
        if r[0] not in by_va or r[3] > by_va[r[0]][3]:
            by_va[r[0]] = r
    ok = [r for r in by_va.values() if r[3] >= 6.0 and r[4] >= 3.0 and r[5] >= 2]
    ok.sort()
    with open(OUT, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["xbox_va", "name", "file", "score", "margin", "shared", "gc_literals"])
        for r in ok:
            w.writerow([r[0], r[1], r[2], "%.1f" % r[3], "%.1f" % r[4], r[5], r[6]])
    print("%d GameCube functions with literals, %d named on Xbox -> %s" % (len(gc), len(ok), OUT))


if __name__ == "__main__":
    main()
