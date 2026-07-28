"""Free template tier: auto-generate + self-verify candidate C for trivial
functions (empty stubs, constant/argument returns, single-field get/set,
two-arg arithmetic, simple global getters) - no hand-writing or LLM needed.

Ported and trimmed from sm64ds-decomp's tools/swarm.py (its leaf RULES only -
the C++-ABI rules there, vtable/ctor/dtor chains, are tuned to SM64DS's own
class layouts and need a populated config/relocs.txt to resolve callee names,
which this project doesn't have yet - see notes/pictochat-layout.md).

Every candidate is compiled with mwccarm and byte-diffed against the ROM
before being reported or banked, exactly like a hand-written match - a rule
misfiring just means "no candidate", never a wrong one.

Mode-aware: this project's code is mostly Thumb (unlike sm64ds-decomp's
mostly-ARM), so each rule is tried against the target's OWN encoding (the
"mode" field in extracted/pictochat_funcs.json, from Ghidra), decoding with
the matching capstone mode and compiling with/without -thumb to match.

Usage:
    python tools/templates.py                          # dry-run, report candidates
    python tools/templates.py --apply                  # bank verified wins (tools/ledger.py)
    python tools/templates.py --module unk_autoload_0 --max 0x20
    python tools/templates.py --verbose                 # print each candidate's C
"""
import argparse
import pathlib
import re
import sys
import tempfile

from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM, CS_MODE_THUMB

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import match as M   # noqa: E402
import funcs as F    # noqa: E402
import ledger as L   # noqa: E402

MD = {"arm": Cs(CS_ARCH_ARM, CS_MODE_ARM), "thumb": Cs(CS_ARCH_ARM, CS_MODE_THUMB)}


def squash(op_str):
    return op_str.replace(" ", "")


def imm(op_str):
    m = re.search(r"#(-?0x[0-9a-fA-F]+|-?\d+)", op_str)
    return int(m.group(1), 0) if m else None


def cfunc(name, ret, params, body):
    return f"{ret} {name}({params})\n{{\n" + (f"    {body}\n" if body else "") + "}\n"


def is_ret(i):
    return i.mnemonic == "bx" and squash(i.op_str) == "lr"


def _mn(i):
    """Mnemonic with a Thumb flags-setting suffix stripped (movs->mov,
    adds->add, ...) so one rule covers both ARM- and Thumb-encoded shapes."""
    m = i.mnemonic
    base = m[:-1]
    return base if m.endswith("s") and base in ("mov", "add", "sub", "and", "orr", "eor", "mul", "mvn") else m


LOADS = {
    "ldr": ("int", 4), "ldrb": ("unsigned char", 1), "ldrsb": ("signed char", 1),
    "ldrh": ("unsigned short", 2), "ldrsh": ("short", 2),
}
STORES = {"str": ("int", 4), "strb": ("char", 1), "strh": ("short", 2)}
ARITH = {"add": "+", "sub": "-", "orr": "|", "and": "&", "eor": "^", "mul": "*"}

MEM_RE = re.compile(r"^r0,\[r0(?:,#(-?0x[0-9a-fA-F]+|-?\d+))?\]$")
ST_RE = re.compile(r"^r1,\[r0(?:,#(-?0x[0-9a-fA-F]+|-?\d+))?\]$")
GG_RE = re.compile(r"^r0,\[r0(?:,#(0x[0-9a-fA-F]+|\d+))?\]$")


# --------------------------------------------------------------------- rules
# Each rule: (name, insns, raw_bytes) -> (c_source, rule_label) or None.
def rule_empty(name, ins, b):
    if len(ins) == 1 and is_ret(ins[0]):
        return cfunc(name, "void", "void", ""), "empty"
    return None


def rule_ret_const(name, ins, b):
    if len(ins) == 2 and is_ret(ins[1]):
        i, s, mn = ins[0], squash(ins[0].op_str), _mn(ins[0])
        if mn == "mov" and s.startswith("r0,#"):
            n = imm(s)
            if n is not None:
                return cfunc(name, "int", "void", f"return {n};"), "ret_const"
        if mn == "mvn" and s.startswith("r0,#"):
            n = imm(s)
            if n is not None:
                return cfunc(name, "int", "void", f"return {~n};"), "ret_const_mvn"
    return None


def rule_ret_arg(name, ins, b):
    if len(ins) == 2 and is_ret(ins[1]) and _mn(ins[0]) == "mov":
        m = re.fullmatch(r"r0,r([1-3])", squash(ins[0].op_str))
        if m:
            n = int(m.group(1))
            params = ", ".join(f"int a{k}" for k in range(n + 1))
            return cfunc(name, "int", params, f"return a{n};"), "ret_arg"
    return None


def rule_load(name, ins, b):
    if len(ins) == 2 and is_ret(ins[1]) and ins[0].mnemonic in LOADS:
        m = MEM_RE.match(squash(ins[0].op_str))
        if m:
            ptype, esize = LOADS[ins[0].mnemonic]
            off = int(m.group(1), 0) if m.group(1) else 0
            if off % esize == 0:
                return cfunc(name, "int", f"{ptype} *p", f"return p[{off // esize}];"), "load_field"
    return None


def rule_store(name, ins, b):
    if len(ins) == 2 and is_ret(ins[1]) and ins[0].mnemonic in STORES:
        m = ST_RE.match(squash(ins[0].op_str))
        if m:
            ptype, esize = STORES[ins[0].mnemonic]
            off = int(m.group(1), 0) if m.group(1) else 0
            if off % esize == 0:
                return cfunc(name, "void", f"{ptype} *p, int v",
                              f"p[{off // esize}] = v;"), "store_field"
    return None


def rule_arith(name, ins, b):
    if len(ins) == 2 and is_ret(ins[1]) and _mn(ins[0]) in ARITH:
        if re.fullmatch(r"r0,r0,r1", squash(ins[0].op_str)):
            return cfunc(name, "int", "int a, int b",
                          f"return a {ARITH[_mn(ins[0])]} b;"), "arith2"
    return None


def rule_global_getter(name, ins, b):
    # ldr r0,[pc,#x] ; <load> r0,[r0,#off] ; bx lr ; .word G -> a global field.
    # The .word is G's absolute address: a relocation the oracle wildcards.
    if (len(ins) >= 3 and ins[0].mnemonic == "ldr"
            and squash(ins[0].op_str).startswith("r0,[pc")
            and ins[1].mnemonic in LOADS and is_ret(ins[2])):
        m = GG_RE.match(squash(ins[1].op_str))
        if m:
            ptype, esize = LOADS[ins[1].mnemonic]
            off = int(m.group(1), 0) if m.group(1) else 0
            if off % esize == 0:
                src = (f"extern {ptype} G[];\n"
                       f"int {name}(void)\n{{\n    return G[{off // esize}];\n}}\n")
                return src, "global_getter"
    return None


RULES = [rule_empty, rule_ret_const, rule_ret_arg, rule_load, rule_store,
         rule_arith, rule_global_getter]


# -------------------------------------------------------------------- oracle
def flags_for(module, mode):
    base = M.DEFAULT_FLAGS_ARM7 if module == "arm7" else M.DEFAULT_FLAGS
    if mode == "arm" and "-thumb" in base:
        base = base.replace(" -thumb", "")
    return base


def oracle_ok(c_source, name, target, module, mode):
    flags = flags_for(module, mode)
    with tempfile.TemporaryDirectory() as td:
        cfile = pathlib.Path(td) / "cand.c"
        cfile.write_text(c_source)
        obj = M.compile_c(cfile, M.CANONICAL, flags)
    if obj is None:
        return False
    code, relocs = M.extract_func(obj, name)
    if code is None:
        return False
    ok, _ = M.compare(target, code, relocs, verbose=False)
    return ok


def is_thunk(ins):
    """Tail-call / linker veneer: a computed jump to anything but lr. Not
    reproducible from a single isolated C function."""
    return any(i.mnemonic == "bx" and squash(i.op_str) != "lr" for i in ins)


def scan(module_filter, min_size, max_size, mode_filter):
    done = L.load_done()
    hits = []
    for f in F.load_funcs():
        if f["module"] is None:
            continue
        if module_filter and f["module"] != module_filter:
            continue
        if not (min_size <= f["size"] <= max_size):
            continue
        if mode_filter and f["mode"] != mode_filter:
            continue
        if (f["module"], f["addr"]) in done:
            continue
        tgt = F.target_bytes(f)
        md = MD.get(f["mode"])
        if md is None:
            continue
        ins = list(md.disasm(tgt, 0))
        if not ins or is_thunk(ins):
            continue
        for rule in RULES:
            cand = rule(f["name"], ins, tgt)
            if not cand:
                continue
            csrc, label = cand
            if oracle_ok(csrc, f["name"], tgt, f["module"], f["mode"]):
                hits.append((f, csrc, label))
                break
    return hits


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--module", default=None, choices=list(M.MODULES))
    ap.add_argument("--min", type=lambda x: int(x, 0), default=0x2)
    ap.add_argument("--max", type=lambda x: int(x, 0), default=0x20)
    ap.add_argument("--mode", default=None, choices=["arm", "thumb"])
    ap.add_argument("--apply", action="store_true", help="bank wins via tools/ledger.py")
    ap.add_argument("--verbose", action="store_true", help="print each win's C")
    args = ap.parse_args()

    print(f"scanning unmatched functions (size 0x{args.min:x}-0x{args.max:x}"
          f"{', module ' + args.module if args.module else ''}"
          f"{', mode ' + args.mode if args.mode else ''})...")
    hits = scan(args.module, args.min, args.max, args.mode)

    by_label = {}
    for f, csrc, label in hits:
        by_label.setdefault(label, []).append(f["name"])
        print(f"  [{label}] {f['module']:16} {f['name']:30} @ 0x{f['addr']:08x} "
              f"(0x{f['size']:x}, {f['mode']})")
        if args.verbose:
            print("    " + csrc.strip().replace("\n", "\n    "))

    print(f"\n{len(hits)} candidate(s) found, by rule: "
          + ", ".join(f"{k}={len(v)}" for k, v in sorted(by_label.items())))

    if not args.apply:
        print("(dry-run: nothing banked; re-run with --apply)")
        return

    banked = 0
    for f, csrc, label in hits:
        st = L.bank({"addr": f["addr"], "name": f["name"], "size": f["size"],
                      "module": f["module"], "versions": [f"template:{label}"]}, csrc)
        banked += st == "banked"
        if st != "banked":
            print(f"  {f['name']}: {st}")
    print(f"banked {banked}/{len(hits)}")


if __name__ == "__main__":
    main()
