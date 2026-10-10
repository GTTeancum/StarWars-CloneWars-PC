#!/usr/bin/env python3
"""Where a recovered function and the Xbox build's translation of it differ.

Recovered source comes from the GameCube build (2002); the Xbox build shipped
later, and some mission scripts were changed in between. For every recovered
function bound to an Xbox address (src/game/glue/glue_gen.cpp hooks), this
lines up the two call sequences -- the native compile's calls, resolved to
Xbox addresses through the glue, against the Xbox function's own calls --
and prints each run of calls only one side makes, with names.

A few one-call differences are compiler noise (a helper inlined on one side).
Runs of several calls, or calls with distinct names, are real script changes.

Usage: python tools/gc_script_diff.py [Name::Function ...]
"""
import collections, difflib, json, os, re, sys, tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import gc_bind  # noqa: E402

PROJECT = os.path.dirname(HERE)
GAME = os.path.join(PROJECT, "game")


# Small value-type constructors: one compiler calls them, the other inlines
# them. Not script logic.
NOISE_SYM = re.compile(r"\?\?0(Vector|Quat|Target)@@")
NOISE_VA = {0x0006B1B0, 0x0006B1D0, 0x001A2F52, 0x0017971B}


_IMG = None


def xbox_call_sites(va):
    """(call address, target) for every direct call in the Xbox function at va."""
    global _IMG
    import capstone
    import xbox_literals
    if _IMG is None:
        _IMG = xbox_literals.Image(xbox_literals.XBE)
    ext = json.load(open(os.path.join(GAME, "xbox_literals.json")))
    end = int(ext["%08X" % va]["end"], 16)
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    out = []
    for ins in md.disasm(_IMG.read(va, end - va), va):
        if ins.mnemonic == "call" and ins.operands[0].type == capstone.x86.X86_OP_IMM:
            out.append((ins.address, ins.operands[0].imm & 0xFFFFFFFF))
    return out


def main():
    only = set(sys.argv[1:])
    glue = open(os.path.join(PROJECT, "src", "game", "glue", "glue_gen.cpp")).read()
    # mangled -> Xbox va, from thunks and hooks
    sym_va, va_name = {}, {}
    for block in re.split(r"\n(?=/\* )", glue):
        m = re.match(r"/\* (.*?) \*/", block)
        if not m:
            continue
        alt = re.search(r'/alternatename:(\S+?)=(\S+?)"', block)
        call = re.search(r"gb_call\(0x([0-9A-F]{8})u", block)
        hook = re.search(r"^void sub_([0-9A-F]{8})\(void\)", block, re.M)
        if call and "gb_trap" not in block and alt:
            sym_va[alt.group(1)] = int(call.group(1), 16)
            va_name[int(call.group(1), 16)] = gc_bind.short_name(m.group(1))
        if hook and alt:
            sym_va[alt.group(2)] = int(hook.group(1), 16)
            va_name[int(hook.group(1), 16)] = gc_bind.short_name(m.group(1).split("  <- ")[0])
    xb = json.load(open(os.path.join(GAME, "xbox_literals.json")))
    names = {}
    for f in ("gc_names.csv", "gc_callnames.csv"):
        p = os.path.join(GAME, f)
        if os.path.exists(p):
            import csv
            for r in csv.DictReader(open(p)):
                names.setdefault(int(r["xbox_va"], 16), r["name"])
    nm = lambda va: va_name.get(va) or names.get(va) or "sub_%08X" % va

    tmp = tempfile.mkdtemp()
    hooks = {sym: va for sym, va in sym_va.items()
             if re.search(r"^void sub_%08X\(void\)" % va, glue, re.M)}
    total = collections.Counter()
    for path in gc_bind.recovered_files():
        asm = gc_bind.compile_listing(path, tmp)
        if asm is None:
            continue
        funcs, _ = gc_bind.parse_listing(asm)
        # source line of each call, from the listing's "; NNN :" markers
        lines_of, cur, line = {}, None, 0
        for l in asm.splitlines():
            m = re.match(r"^(\?\S+|_\w+)\s+PROC", l)
            if m:
                cur = m.group(1); lines_of[cur] = []; continue
            m = re.match(r"^; (\d+)\s*:", l)
            if m:
                line = int(m.group(1)); continue
            m = re.search(r"	call\s+(\?\S+|_\w+)\s*(?:;|$)", l)
            if cur and m and m.group(1).startswith("?") and not NOISE_SYM.match(m.group(1)):
                lines_of[cur].append(line)
        for sym, calls in funcs.items():
            if sym not in hooks:
                continue
            va = hooks[sym]
            fname = nm(va)
            if only and fname not in only:
                continue
            nat = [sym_va.get(c, c) for c in calls if c.startswith("?") and not NOISE_SYM.match(c)]
            sites = [(s, tgt) for s, tgt in xbox_call_sites(va) if tgt not in NOISE_VA]
            x = [tgt for s, tgt in sites]
            if len(nat) == len(x) and nat == x:
                continue
            sm = difflib.SequenceMatcher(None, nat, x, autojunk=False)
            runs = [op for op in sm.get_opcodes() if op[0] != "equal"]
            if not runs:
                continue
            print("== %s  (%s)  recovered %d calls, Xbox %d" % (fname, os.path.basename(path), len(nat), len(x)))
            for tag, a1, a2, b1, b2 in runs:
                lhs = [nm(v) if isinstance(v, int) else v for v in nat[a1:a2]]
                rhs = [nm(v) for v in x[b1:b2]]
                total[tag] += max(a2 - a1, b2 - b1)
                print("   %-7s GC only: %s" % (tag, ", ".join(lhs) or "-"))
                if a2 > a1 and sym in lines_of:
                    print("           GC lines: %s" % " ".join(str(lines_of[sym][i]) for i in range(a1, a2)))
                print("   %-7s Xbox only: %s" % ("", ", ".join(rhs) or "-"))
                if b2 > b1:
                    print("           Xbox sites: %s" % " ".join("%08X" % sites[i][0] for i in range(b1, b2)))
    print("totals:", dict(total))


if __name__ == "__main__":
    main()
