#!/usr/bin/env python3
"""Bind the recovered source in src/game to the translated title.

1. Compile each recovered file with MSVC to an assembly listing.
2. For every function it defines that the Xbox build also has (named by
   tools/gc_names.py / gc_callnames.py / gc_tu_align.py), line up the
   function's direct calls with the Xbox function's: MSVC compiles the same
   source to the same call order, so the n-th call of one is the n-th call
   of the other. That pins the Xbox address of every callee -- overloads
   included -- by its exact C++ (mangled) name.
3. Write src/game/glue/glue_gen.cpp:
   - a native stand-in for every engine function the recovered code calls,
     forwarding to the translated function through gb_call (guest_bridge.c),
     bound to the C++ name with /alternatename;
   - a trap stand-in for any it calls that is not located yet;
   - a hook for every recovered function whose Xbox address is known, so the
     title runs the recovered code instead of the translation (recovered code
     wins). Constructors and destructors are not hooked: they would store a
     native vtable pointer into a title object.

Usage: python tools/gc_bind.py [--report]
"""
import collections, csv, difflib, glob, json, os, re, subprocess, sys, tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
PROJECT = os.path.dirname(HERE)
GAME = os.path.join(PROJECT, "game")
SRC_GAME = os.path.join(PROJECT, "src", "game")
OUT = os.path.join(SRC_GAME, "glue", "glue_gen.cpp")
FALLBACK = os.path.join(SRC_GAME, "glue", "glue_fallback.c")
VCVARS = r"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"

# Sizes of the value types recovered code passes by reference or value.
SIZES = {"Vector": 12, "Quat": 16, "Timer": 12, "Matrix": 64, "Target": 16, "ZeroVector3": 12,
         "ZeroQuaternion": 16, "ZeroMatrix": 64}


def recovered_files():
    out = []
    for root, dirs, files in os.walk(SRC_GAME):
        if os.path.basename(root) == "glue":
            continue
        out += [os.path.join(root, f) for f in files if f.endswith(".cpp")]
    # CWScript.cpp is the engine-side script host, not compiled yet (it
    # references engine data, which the bridge does not map)
    return sorted(f for f in out if os.path.basename(f) != "CWScript.cpp")


def compile_listing(path, tmp):
    base = os.path.splitext(os.path.basename(path))[0]
    asm = os.path.join(tmp, base + ".asm")
    bat = os.path.join(tmp, "cc.bat")
    with open(bat, "w") as f:
        f.write('@call "%s" >nul\n' % VCVARS)
        f.write('cl /nologo /c /O1 /GR- /EHs-c- /FAs "/Fa%s" "/Fo%s" "%s"\n'
                % (asm, os.path.join(tmp, base + ".obj"), path))
    r = subprocess.run(["cmd", "/c", bat], capture_output=True, text=True)
    if not os.path.exists(asm):
        print("compile failed: %s\n%s" % (path, r.stdout[-2000:]))
        return None
    return open(asm, encoding="latin1").read()


def parse_listing(asm):
    funcs, externs, cur = collections.OrderedDict(), set(), None
    for line in asm.splitlines():
        m = re.match(r"^EXTRN\s+(\S+):PROC", line)
        if m:
            externs.add(m.group(1))
            continue
        m = re.match(r"^(\?\S+|_\w+)\s+PROC", line)
        if m:
            cur = m.group(1)
            funcs[cur] = []
            continue
        if re.match(r"^\S+\s+ENDP", line):
            cur = None
            continue
        m = re.search(r"\tcall\s+(\?\S+|_\w+)\s*(?:;|$)", line)
        if cur and m:
            funcs[cur].append(m.group(1))
    return funcs, externs


def undname(names):
    """mangled -> undecorated, via the MSVC tool."""
    out = {}
    names = sorted(n for n in names if n.startswith("?"))
    tmp = tempfile.mkdtemp()
    bat = os.path.join(tmp, "u.bat")
    for i in range(0, len(names), 60):
        chunk = names[i:i + 60]
        with open(bat, "w") as f:
            f.write('@call "%s" >nul\n' % VCVARS)
            for n in chunk:
                f.write('@undname 0x0 "%s"\n' % n.replace("%", "%%"))
        r = subprocess.run(["cmd", "/c", bat], capture_output=True, text=True)
        cur = None
        for line in r.stdout.splitlines():
            m = re.match(r'^Undecoration of :- "(.*)"', line)
            if m:
                cur = m.group(1)
            m = re.match(r'^is :- "(.*)"', line)
            if m and cur:
                out[cur] = m.group(1)
                cur = None
    return out


def short_name(und):
    """'public: virtual void __thiscall Geonosis1Script::Execute(void)' -> 'Geonosis1Script::Execute'."""
    head = und.split("(")[0]
    return head.split()[-1] if head.split() else head


def split_params(s):
    out, depth, cur = [], 0, ""
    for c in s:
        if c in "<(":
            depth += 1
        elif c in ">)":
            depth -= 1
        if c == "," and depth == 0:
            out.append(cur.strip())
            cur = ""
        else:
            cur += c
    if cur.strip():
        out.append(cur.strip())
    return out


def type_code(t):
    """One parameter's code for gb_call, and its native stack dwords."""
    t = t.strip()
    if t in ("void", "..."):
        return None, 0
    base = re.sub(r"\b(class|struct|enum|const|volatile|__ptr64)\b", "", t).strip()
    if t.endswith("*") or t.endswith("&"):
        inner = base.rstrip("*& ").strip()
        if inner in ("char", "unsigned char", "signed char"):
            return "s", 1
        return "p%d" % SIZES.get(inner.split("::")[-1], 0), 1
    if t.startswith("enum") or base in ("int", "unsigned int", "long", "unsigned long", "float",
                                         "bool", "char", "unsigned char", "short", "unsigned short",
                                         "signed char"):
        return "i", 1
    if base == "double" or base == "__int64" or base == "unsigned __int64":
        return "q8", 2
    n = SIZES.get(base.split("::")[-1])
    if n:
        return "q%d" % n, (n + 3) // 4
    return None, -1


def signature(und):
    """(convention, return code, [param codes], stack dwords) or None."""
    m = re.match(r"^(?:(?:public|private|protected): )?(?:(virtual|static) )?(.*?)\s*"
                 r"(__cdecl|__thiscall|__stdcall|__fastcall)\s+([^(]+)\((.*)\)(?: const)?$", und)
    if not m:
        return None
    kind, ret, conv, name, params = m.groups()
    ret = ret.strip()
    if not ret and "operator " in name:          # conversion operator: Timer::operator float
        ret = name.split("operator ", 1)[1].strip()
    if ret.endswith("*") or ret.endswith("&"):
        rc = "P"
    elif ret == "void":
        rc = "v"
    elif ret in ("float", "double"):
        rc = "f"
    elif ret == "bool":
        rc = "b"
    elif re.sub(r"\b(class|struct)\b", "", ret).strip().split("::")[-1] in SIZES:
        rc = "S"                            # by value: hidden result pointer, first stack arg
    else:
        rc = "i"
    codes, dwords = [], 0
    if rc == "S":
        codes.append("p%d" % SIZES[re.sub(r"\b(class|struct)\b", "", ret).strip().split("::")[-1]])
        dwords = 1
    if params.strip() not in ("void", ""):
        for p in split_params(params):
            c, n = type_code(p)
            if c is None:
                return None
            codes.append(c)
            dwords += n
    return conv, rc, codes, dwords


def load_names():
    names = {}
    for f, col in (("gc_names.csv", "name"), ("gc_callnames.csv", "name"), ("gc_tunames.csv", "signature")):
        p = os.path.join(GAME, f)
        if not os.path.exists(p):
            continue
        for r in csv.DictReader(open(p)):
            n = r[col].split("(")[0]
            names.setdefault(n, set()).add(int(r["xbox_va"], 16))
    return names


def main():
    report = "--report" in sys.argv
    xb = json.load(open(os.path.join(GAME, "xbox_literals.json")))
    names = load_names()
    tmp = tempfile.mkdtemp()
    defined = {}                 # mangled -> file
    calls = {}                   # mangled -> native call list
    externs = set()
    for path in recovered_files():
        asm = compile_listing(path, tmp)
        if asm is None:
            continue
        f, e = parse_listing(asm)
        for k, v in f.items():
            defined.setdefault(k, os.path.relpath(path, PROJECT))
            calls[k] = v
        externs |= e
    und = undname(set(defined) | externs | {c for v in calls.values() for c in v})

    # Xbox address of each defined function, by its unique short name
    def_va = {}
    for k in defined:
        u = und.get(k)
        if not u:
            continue
        vs = names.get(short_name(u))
        if vs and len(vs) == 1:
            def_va[k] = next(iter(vs))

    # line up calls: exact one-for-one where the call counts agree, then
    # text-style alignment anchored on what that pinned (the two compilers
    # inline a few small functions differently)
    def native_list(k):
        return [c for c in calls.get(k, []) if c.startswith("?")]

    def xbox_list(va):
        return [int(c, 16) for c in xb.get("%08X" % va, {}).get("co", [])]

    votes = collections.defaultdict(collections.Counter)
    zipped, skipped = 0, []
    for k, va in def_va.items():
        n, x = native_list(k), xbox_list(va)
        if n and len(n) == len(x):
            zipped += 1
            for a, b in zip(n, x):
                votes[a][b] += 100
    callee_va = {}
    for rnd in range(3):
        callee_va = {}
        for k, c in votes.items():
            (va, v), = c.most_common(1)
            if v >= 0.9 * sum(c.values()):
                callee_va[k] = va
        votes2 = collections.defaultdict(collections.Counter)
        for k, c in votes.items():
            if any(v >= 100 for v in c.values()):
                votes2[k].update({va: v for va, v in c.items() if v >= 100})
        for k, va in def_va.items():
            n, x = native_list(k), xbox_list(va)
            if not n or len(n) == len(x):
                continue
            g = [callee_va.get(c, c) for c in n]
            sm = difflib.SequenceMatcher(None, g, x, autojunk=False)
            pa = pb = 0
            for a, b, size in sm.get_matching_blocks():
                if a - pa == b - pb:
                    for i in range(a - pa):
                        votes2[n[pa + i]][x[pb + i]] += 1
                for i in range(size):
                    votes2[n[a + i]][x[b + i]] += 1
                pa, pb = a + size, b + size
            if rnd == 2:
                skipped.append((und.get(k, k), len(n), len(x)))
        votes = votes2
    conflicts = []
    for k, c in votes.items():
        (va, v), = c.most_common(1)
        if v >= 0.9 * sum(c.values()):
            callee_va[k] = va
        else:
            conflicts.append((und.get(k, k), dict(c)))
    # fallback: an engine function not pinned by alignment but named, without
    # overloads, by the literal/call/TU passes
    overloads = collections.Counter(short_name(und[k]) for k in externs if k in und)
    for k in externs:
        if k in callee_va or k not in und:
            continue
        sn = short_name(und[k])
        vs = names.get(sn)
        if overloads[sn] == 1 and vs and len(vs) == 1:
            callee_va[k] = next(iter(vs))
    # a callee that is itself recovered code is hooked, not thunked
    for k, va in callee_va.items():
        if k in defined:
            def_va.setdefault(k, va)

    lines = ['/* glue_gen.cpp -- GENERATED by tools/gc_bind.py; do not edit.',
             ' *',
             ' * Stand-ins for the engine functions the recovered source calls',
             ' * (forwarded to the translated title), and hooks that run recovered',
             ' * functions in place of their translations. See include/game/guest_bridge.h. */',
             '#include "game/guest_bridge.h"', '', '#pragma warning(disable: 4100)', '']
    nthunk = ntrap = nhook = 0
    unhandled = []
    fallback = []
    for k in sorted(externs):
        if not k.startswith("?") or k in defined:
            continue
        u = und.get(k, k)
        sig = signature(u) if u else None
        va = callee_va.get(k)
        tag = "gbt_%d" % (nthunk + ntrap)
        if sig is None:
            unhandled.append(u)
        conv, rc, codes, dwords = sig if sig else ("__cdecl", "i", [], 0)
        member = conv == "__thiscall"
        params = ["uint32_t a%d" % i for i in range(dwords)]
        if member:
            decl_params = ["uint32_t self", "uint32_t edx_"] + params
            cconv, sym = "__fastcall", "@%s@%d" % (tag, 8 + 4 * dwords)
        else:
            decl_params = params or ["void"]
            cconv, sym = "__cdecl", "_" + tag
        rtype = {"v": "void", "f": "float", "b": "bool"}.get(rc, "uint32_t")
        lines.append("/* %s */" % u)
        lines.append('extern "C" %s %s %s(%s)' % (rtype, cconv, tag, ", ".join(decl_params)))
        lines.append("{")
        if va is None or sig is None:
            lines.append('    gb_trap("%s");' % u.replace('"', "'"))
            if rc == "f":
                lines.append("    return 0.0f;")
            elif rc != "v":
                lines.append("    return 0;")
            ntrap += 1
        else:
            if dwords:
                lines.append("    uint32_t a[] = { %s };" % ", ".join("a%d" % i for i in range(dwords)))
            arr = "a" if dwords else "0"
            spec = ("i" if rc in ("P", "S") else rc) + "".join(codes)
            self_ = "gb_guest((const void *)(uintptr_t)self)" if member else "0"
            call = 'gb_call(0x%08Xu, %s, "%s", %s, %s, "%s")' % (va, self_, spec, arr, "&r" if rc == "f" else "0", short_name(u))
            if rc == "f":
                lines.append("    double r = 0;")
                lines.append("    %s;" % call)
                lines.append("    return (float)r;")
            elif rc == "v":
                lines.append("    %s;" % call)
            elif rc == "b":
                lines.append("    return %s != 0;" % call)
            elif rc == "P":
                lines.append("    return (uint32_t)(uintptr_t)gb_host(%s);" % call)
            elif rc == "S":
                lines.append("    %s;" % call)
                lines.append("    return a0;                 /* the caller's result buffer */")
            else:
                lines.append("    return %s;" % call)
            nthunk += 1
        lines.append("}")
        lines.append('#pragma comment(linker, "/alternatename:%s=%s")' % (k, sym))
        lines.append("")

    # hooks
    for k in sorted(def_va, key=lambda k: def_va[k]):
        u = und.get(k, "")
        sn = short_name(u)
        cls, _, meth = sn.rpartition("::")
        if not u or meth.startswith("~") or meth == cls.split("::")[-1] or "`" in u or "operator" in meth:
            continue
        sig = signature(u)
        if sig is None:
            continue
        conv, rc, codes, dwords = sig
        va = def_va[k]
        tag = "gbh_%08X" % va
        stack_args = ", ".join("gb_arg(%d)" % i for i in range(dwords))
        if conv == "__thiscall":
            proto = "uint32_t self, uint32_t edx_" + "".join(", uint32_t" for _ in range(dwords))
            sym = "@%s@%d" % (tag, 8 + 4 * dwords)
            args = "(uint32_t)(uintptr_t)gb_host(gb_this()), 0" + (", " + stack_args if dwords else "")
            cconv, popped = "__fastcall", 4 * dwords
        else:
            proto = ", ".join("uint32_t" for _ in range(dwords)) or "void"
            sym = "_" + tag
            args = stack_args
            cconv, popped = "__cdecl", 0
        if any(c.startswith(("s", "p")) for c in codes):
            continue                         # pointer arguments would need guest->host conversion
        rtype = {"v": "void", "f": "float", "b": "bool"}.get(rc, "uint32_t")
        if rc in ("f", "P", "S"):
            continue                         # float/pointer/struct results: not handled by hooks yet
        lines.append("/* %s  <- %s */" % (u, defined[k]))
        lines.append('extern "C" %s %s %s(%s);' % (rtype, cconv, tag, proto))
        lines.append('#pragma comment(linker, "/alternatename:%s=%s")' % (sym, k))
        lines.append('extern "C" void sub_%08X_gen(void);' % va)
        lines.append('extern "C" void sub_%08X(void);' % va)
        lines.append("void sub_%08X(void)" % va)
        lines.append("{")
        lines.append("    if (!gb_recovered_on()) { sub_%08X_gen(); return; }" % va)
        lines.append('    static int seen; if (!seen++) gb_note("%s");' % sn)
        if rc == "v":
            lines.append("    %s(%s);" % (tag, args))
            lines.append("    gb_return(0, %d);" % popped)
        else:
            lines.append("    uint32_t r = (uint32_t)%s(%s);" % (tag, args))
            lines.append("    gb_return(r, %d);" % popped)
        lines.append("}")
        lines.append("")
        fallback.append(va)
        nhook += 1

    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    with open(OUT, "w", newline="\n") as f:
        f.write("\n".join(lines))
    # builds without the bridge (x64) keep the translated bodies
    with open(FALLBACK, "w", newline="\n") as f:
        f.write("/* glue_fallback.c -- GENERATED by tools/gc_bind.py; do not edit.\n"
                " * Builds without the recovered source: each hooked function is its translation. */\n")
        for va in fallback:
            f.write("extern void sub_%08X_gen(void);\nvoid sub_%08X(void) { sub_%08X_gen(); }\n" % (va, va, va))
    print("functions lined up: %d (skipped %d: call counts differ)" % (zipped, len(skipped)))
    print("engine stand-ins: %d forwarded, %d traps; recovered hooks: %d -> %s"
          % (nthunk, ntrap, nhook, os.path.relpath(OUT, PROJECT)))
    if report:
        for s in skipped:
            print("  skipped %s: native %d calls, xbox %d" % s)
        for c in conflicts:
            print("  conflict %s: %s" % c)
        for u in unhandled:
            print("  unhandled signature: %s" % u)


if __name__ == "__main__":
    main()
