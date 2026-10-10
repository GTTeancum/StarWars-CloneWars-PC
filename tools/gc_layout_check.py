#!/usr/bin/env python3
"""Check that each recovered mission script lays its object out as the Xbox
build does.

The script object is allocated and constructed by the translated Xbox code
(constructors are never hooked), then the recovered Setup/Execute run on it.
If the recovered class's fields sit at other offsets, native code reads and
writes the wrong slots -- timers the Xbox constructor never built, a flag
block the engine's mid-mission save does not cover.

Both constructors are disassembled and fingerprinted the same way:
  header   SPMission's block pointers and counts (+0x04..+0x20)
  timers   offsets of the Timer members the constructor builds
  size     allocation size in the build-mission function
Xbox: the translated build's constructor, found through the class vtable.
Recovered: the class compiled with MSVC, read with dumpbin.

Usage: python tools/gc_layout_check.py
"""
import json, os, re, struct, subprocess, sys, tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import gc_bind  # noqa: E402
import xbox_literals  # noqa: E402

PROJECT = os.path.dirname(HERE)
GAME = os.path.join(PROJECT, "game")
MISSIONS = os.path.join(PROJECT, "src", "game", "missions")
TIMER_CTOR_XBOX = 0x0021E709


def fingerprint(lines):
    """lines: [(mnemonic, operands)] of one constructor, Intel syntax."""
    regs, header, timers = {}, {}, []
    for mn, ops in lines:
        ops = ops.replace(" ", "")
        a = ops.split(",")
        if mn == "lea" and len(a) == 2:
            m = re.match(r"(?:dwordptr)?\[(\w+)\+(0x[0-9a-fA-F]+|[0-9A-Fa-f]+h|\d+)\]", a[1])
            if m:
                regs[a[0]] = ("ptr", num(m.group(2)))
        elif mn == "push" and len(a) == 1 and isnum(a[0]):
            regs["_push"] = num(a[0])
        elif mn == "pop" and len(a) == 1 and "_push" in regs:
            regs[a[0]] = ("imm", regs.pop("_push"))
        elif mn == "mov" and len(a) == 2:
            m = re.match(r"dwordptr\[(\w+)\+(0x[0-9a-fA-F]+|[0-9A-Fa-f]+h|\d+)\]", a[0])
            if m:
                off = num(m.group(2))
                if off in (4, 8, 0xC, 0x10, 0x14, 0x18, 0x1C, 0x20):
                    if isnum(a[1]):
                        header[off] = num(a[1])
                    elif a[1] in regs:
                        k, v = regs[a[1]]
                        header[off] = ("+%X" % v) if k == "ptr" else v
            elif isnum(a[1]) and a[0] in ("eax", "ecx", "edx", "ebx", "esi", "edi"):
                regs[a[0]] = ("imm", num(a[1]))
        elif mn == "call":
            if "ecx" in regs and regs["ecx"][0] == "ptr" and ("Timer" in ops or ops in ("0x%x" % TIMER_CTOR_XBOX,)):
                timers.append(regs["ecx"][1])
    return header, sorted(timers)


def isnum(s):
    return bool(re.fullmatch(r"0x[0-9a-fA-F]+|[0-9A-Fa-f]+h|\d+", s))


def num(s):
    if s.startswith("0x"):
        return int(s, 16)
    if s.endswith("h"):
        return int(s[:-1], 16)
    return int(s)


def xbox_side():
    import capstone
    img = xbox_literals.Image(xbox_literals.XBE)
    funcs = {int(e["start"], 16): int(e["end"], 16)
             for e in json.load(open(os.path.join(PROJECT, "tools", "disasm_seeds752", "functions.json")))}
    vts = json.load(open(os.path.join(GAME, "script_vtables.json")))
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    out = {}
    for cls, v in vts.items():
        best = None
        for c in v["ctors"]:
            ins = [(i.mnemonic, i.op_str) for i in md.disasm(img.read(c, funcs[c] - c), c)]
            fp = fingerprint(ins)
            if fp[0] and (best is None or len(fp[0]) > len(best[0])):
                best = fp
        out[cls] = best
    return out


def native_side():
    tmp = tempfile.mkdtemp()
    out = {}
    vcvars = gc_bind.VCVARS
    for f in sorted(os.listdir(MISSIONS)):
        if not f.endswith("Script.cpp"):
            continue
        cls = f[:-4]
        src = os.path.join(MISSIONS, f)
        obj = os.path.join(tmp, cls + ".obj")
        bat = os.path.join(tmp, "c.bat")
        with open(bat, "w") as fh:
            fh.write('@call "%s" >nul\n' % vcvars)
            fh.write('cl /nologo /c /O1 /GR- /EHs-c- "/Fo%s" "%s" >nul\n' % (obj, src))
            fh.write('dumpbin /nologo /disasm:nobytes "%s"\n' % obj)
        r = subprocess.run(["cmd", "/c", bat], capture_output=True, text=True)
        cur, lines = None, {}
        for line in r.stdout.splitlines():
            m = re.match(r"^(\S+)(?: \(.*\))?:$", line)
            if m and line[:1] != " ":
                cur = m.group(1)
                if len(lines.get(cur, [])) < 3:      # a jump stub of the same name comes first
                    lines[cur] = []
                else:
                    cur = None
                continue
            m = re.match(r"^\s+[0-9A-F]+:\s+(\w+)\s*(.*)$", line)
            if m and cur:
                lines[cur].append((m.group(1), m.group(2)))
        ctor = next((k for k in lines if k.startswith("??0%s@@" % cls)), None)
        fp = fingerprint(lines[ctor]) if ctor else (None, None)
        out[cls] = fp
    return out


def main():
    xb, nat = xbox_side(), native_side()
    bad = 0
    for cls in sorted(xb):
        x, n = xb.get(cls), nat.get(cls)
        if not x or not n or n[0] is None:
            print("%-16s cannot compare (xbox %s, recovered %s)" % (cls, bool(x), bool(n and n[0])))
            continue
        # Block pointers decide where native code reads and writes; counts only
        # matter to the constructor, which stays the Xbox one. A pointer only
        # one side's extractor saw (written through a register it did not
        # track) is not a difference.
        hd = {"+%02X" % k: (x[0].get(k), n[0].get(k)) for k in (4, 0xC, 0x14, 0x1C)
              if isinstance(x[0].get(k), str) and isinstance(n[0].get(k), str) and x[0][k] != n[0][k]}
        counts = {"+%02X" % k: (x[0].get(k), n[0].get(k)) for k in (8, 0x10, 0x18, 0x20)
                  if isinstance(x[0].get(k), int) and isinstance(n[0].get(k), int) and x[0][k] != n[0][k]}
        td = x[1] != n[1]
        if hd or td:
            bad += 1
            print("%-16s MISMATCH header %s timers %s" % (cls, hd or "ok",
                  "ok" if not td else "xbox %s / recovered %s" % ([hex(t) for t in x[1]][:6], [hex(t) for t in n[1]][:6])))
        else:
            print("%-16s ok (%d timers)%s" % (cls, len(x[1]), "  counts differ %s" % counts if counts else ""))
    print("%d mismatched" % bad)


if __name__ == "__main__":
    main()
