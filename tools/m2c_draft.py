"""Free semantic C draft for one function via m2c (matt-kempster's decompiler).

Converts our capstone disassembly of the ROM bytes into GAS-style ARM assembly
that m2c's arm-gcc-c target parses, then runs vendor/m2c on it and prints the
resulting C. The draft is gcc-flavored pseudo-C: a comprehension scaffold that
shows the control flow, callee names (where resolvable), and data flow of a
LARGE function. It is NOT a matching candidate and will not compile under
mwccarm as-is - read it, then hand-write (or iterate with tools/fdiff.py) a
real candidate from what it tells you.

Ported from sm64ds-decomp's tools/m2c_draft.py. Two real differences from
their version:
  - mode-aware: this project's code is mostly Thumb, not ARM (see the "mode"
    field in extracted/pictochat_funcs.json) - pass --thumb or it's inferred
    from the Ghidra cache automatically in --name/--addr (live) mode.
  - no config/relocs.txt: this project's dsd config never finished generating
    (see notes/pictochat-layout.md), so callee/global names can't be resolved
    from committed data yet. Every `bl`/pool reference that would normally
    show a real symbol name instead shows func_<addr> / 0x<value> - still a
    perfectly usable draft, just less self-documenting than sm64ds-decomp's.

Conversion rules:
  - each 4-byte (ARM) or 2-byte (Thumb) unit is decoded independently (no
    linear-sweep truncation at mid-function literal pools), then code vs pool
    data is classified by recursive descent from the entry point
  - branch targets become local labels (.L_<va>)
  - bl/blx targets become func_<addr> (no relocs.txt to resolve real names)
  - pc-relative ldr becomes the "ldr rX, =0x<word>" pseudo-instruction
  - add rX, pc, #imm (adr) becomes ldr rX, =<label-or-address>
  - pool/data words are emitted as labeled .word directives; words that point
    into the function body become .word .L_<va> and their targets are
    classified as code (word-table switches)

Usage:
    python tools/m2c_draft.py --name FUN_02322abc
    python tools/m2c_draft.py --module unk_autoload_0 --addr 0x02322abc
    python tools/m2c_draft.py --name X --dump-asm      # generated asm on stderr

Requires vendor/m2c: git clone https://github.com/matt-kempster/m2c vendor/m2c
"""
import argparse
import pathlib
import re
import subprocess
import sys
import tempfile

from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM, CS_MODE_THUMB

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import funcs as F   # noqa: E402

M2C_PY = REPO / "vendor" / "m2c" / "m2c.py"

MD = {"arm": Cs(CS_ARCH_ARM, CS_MODE_ARM), "thumb": Cs(CS_ARCH_ARM, CS_MODE_THUMB)}

COND = r"(?:eq|ne|cs|hs|cc|lo|mi|pl|vs|vc|hi|ls|ge|lt|gt|le|al)"
B_RE = re.compile(rf"^b({COND})?$")
BL_RE = re.compile(rf"^blx?({COND})?$")
LDR_RE = re.compile(rf"^ldr({COND})?$")            # plain word load only (can take =)
ANYLOAD_RE = re.compile(rf"^ldr(s?[bh])?({COND})?$")
RETBLK_RE = re.compile(rf"^(?:pop|ldm(?:ia|ib|da|db|fd|fa|ed|ea)?)({COND})?$")
PCREL_RE = re.compile(r"^(\w+),\[pc,#(-?0x[0-9a-fA-F]+|-?\d+)\]$")
ADRPC_RE = re.compile(r"^(\w+),pc,#(0x[0-9a-fA-F]+|\d+)$")
IMM_OP_RE = re.compile(r"^#(-?0x[0-9a-fA-F]+|-?\d+)$")
ADDPCPC_RE = re.compile(r"^pc,pc,(\w+),lsl#2$")


class M2CError(RuntimeError):
    pass


def sq(i):
    return i.op_str.replace(" ", "")


def decode_words(tgt, addr, md, step):
    """{offset: capstone insn or None} decoding at every 2-byte-aligned
    (Thumb) or 4-byte-aligned (ARM) offset independently, so a mid-function
    literal pool cannot truncate the disassembly.

    Thumb isn't fixed-width like ARM (T1/T2 16-bit encodings vs Thumb-2
    32-bit ones - bl, movw/movt, ldr.w, ...), so each offset gets a 4-byte
    lookahead window and capstone reports the real size (2 or 4) of whatever
    it decodes there; classify() advances by that real size, not by `step`."""
    out = {}
    win = 4 if step == 2 else step
    for off in range(0, len(tgt) - (step - 1), step):
        got = next(md.disasm(tgt[off:off + win], addr + off), None)
        out[off] = got
    return out


def _branch_target(i):
    m = IMM_OP_RE.match(sq(i))
    return int(m.group(1), 0) if m else None


def _ends_flow(i):
    mn, op = i.mnemonic, sq(i)
    if mn in ("b", "bx"):
        return True
    m = RETBLK_RE.match(mn)
    if m and m.group(1) is None and "pc" in op:
        return True
    if mn in ("mov", "ldr") and op.startswith("pc,"):
        return True
    return False


def classify(tgt, addr, ins, step):
    """Split the function's units into code and data offsets by recursive
    descent from offset 0, then a fixpoint pass catching word-table switches
    (a data word whose value points back into the function)."""
    size = len(tgt) // step * step
    code = set()

    def valid(off):
        return 0 <= off < size and off % step == 0 and ins.get(off) is not None

    def sweep(entry):
        work = [entry]
        while work:
            off = work.pop()
            while valid(off) and off not in code:
                i = ins[off]
                code.add(off)
                mn = i.mnemonic
                if BL_RE.match(mn) or B_RE.match(mn):
                    t = _branch_target(i)
                    if t is not None and 0 <= t - addr < size:
                        work.append(t - addr)
                elif step == 4 and ADDPCPC_RE.match(sq(i)) and mn.startswith("add"):
                    j = off + step
                    while valid(j) and (B_RE.match(ins[j].mnemonic)
                                        or ("pc" in sq(ins[j])
                                            and RETBLK_RE.match(ins[j].mnemonic))):
                        code.add(j)
                        t = _branch_target(ins[j])
                        if t is not None and 0 <= t - addr < size:
                            work.append(t - addr)
                        j += step
                    break
                if _ends_flow(i):
                    break
                off += i.size if step == 2 else step

    sweep(0)
    while True:
        entries = []
        for off in range(0, size, 4):
            if off in code or off + 4 > len(tgt):
                continue
            w = int.from_bytes(tgt[off:off + 4], "little")
            t = w - addr
            if 0 <= t < size and t % step == 0 and t not in code and valid(t):
                entries.append(t)
        if not entries:
            break
        for t in entries:
            sweep(t)
    return code


def build_asm(name, addr, tgt, mode, window=None):
    """The GAS-syntax translation unit m2c parses: one function, local
    labels, func_<addr> callees, =pool pseudo-loads, labeled .word data."""
    window = window if window is not None and len(window) >= len(tgt) else tgt
    step = 2 if mode == "thumb" else 4
    md = MD[mode]
    size = len(tgt) // step * step
    ins = decode_words(tgt, addr, md, step)
    code = classify(tgt, addr, ins, step)
    # bytes covered by the 2nd half of a 4-byte Thumb-2 instruction (bl,
    # movw/movt, ldr.w, ...) - never a separate label or line of their own.
    covered = {off + 2 for off in code if step == 2 and ins[off].size == 4}

    labels = set()
    rewrites = {}
    for off in sorted(code):
        i = ins[off]
        mn, s = i.mnemonic, sq(i)
        if BL_RE.match(mn) or B_RE.match(mn):
            t = _branch_target(i)
            if t is None:
                continue
            if 0 <= t - addr < size and (t - addr) in code:
                labels.add(t - addr)
                rewrites[off] = f"{mn} .L_{t:08x}"
            else:
                rewrites[off] = f"{mn} func_{t:08x}"
            continue
        pm = PCREL_RE.match(s)
        if pm and ANYLOAD_RE.match(mn):
            # ARM's PC reads as insn_addr+8; Thumb's as (insn_addr+4) word-aligned down.
            base = off + 8 if step == 4 else ((off + 4) & ~3)
            pool = base + int(pm.group(2), 0)
            if not (0 <= pool <= len(window) - 4):
                raise M2CError(f"pc-relative load at +0x{off:x} references outside "
                               f"the function (+0x{pool:x})")
            if not LDR_RE.match(mn):
                if pool >= size:
                    raise M2CError(f"non-word pc-relative load at +0x{off:x} "
                                   f"references past the function (+0x{pool:x})")
                labels.add(pool)
                rewrites[off] = f"{mn} {pm.group(1)}, .L_{addr + pool:08x}"
                continue
            val = f"0x{int.from_bytes(window[pool:pool + 4], 'little'):x}"
            rewrites[off] = f"{mn} {pm.group(1)}, ={val}"
            continue
        am = ADRPC_RE.match(s)
        if am and mn == "add":
            base = off + 8 if step == 4 else ((off + 4) & ~3)
            t = base + int(am.group(2), 0)
            if 0 <= t < size:
                labels.add(t)
                rewrites[off] = f"ldr {am.group(1)}, =.L_{addr + t:08x}"
            else:
                rewrites[off] = f"ldr {am.group(1)}, =0x{addr + t:x}"
            continue

    for off in range(0, size, 4):
        if off not in code and off not in covered:
            labels.add(off)

    lines = [".syntax unified", ".text", ".code " + ("16" if step == 2 else "32"),
             f".globl {name}", f"{name}:"]
    for off in range(0, size, step):
        if off in covered:
            continue
        if off in labels:
            lines.append(f".L_{addr + off:08x}:")
        if off in code:
            i = ins[off]
            lines.append("\t" + rewrites.get(off, f"{i.mnemonic} {i.op_str}"))
        elif off % 4 == 0:
            w = int.from_bytes(tgt[off:off + 4], "little") if off + 4 <= len(tgt) else 0
            if 0 <= w - addr < size and (w - addr) in code:
                lines.append(f"\t.word .L_{w:08x}")
            else:
                lines.append(f"\t.word 0x{w:x}")
    if len(tgt) != size:
        sys.stderr.write(f"warning: {len(tgt) - size} trailing byte(s) ignored\n")
    return "\n".join(lines) + "\n"


def run_m2c(asm_text):
    if not M2C_PY.is_file():
        raise M2CError("vendor/m2c not found. Run: "
                       "git clone https://github.com/matt-kempster/m2c vendor/m2c")
    with tempfile.TemporaryDirectory() as td:
        p = pathlib.Path(td) / "func.s"
        p.write_text(asm_text, encoding="utf-8")
        r = subprocess.run([sys.executable, str(M2C_PY), "-t", "arm-gcc-c",
                            str(p)], capture_output=True, text=True, timeout=300)
    return r.returncode, r.stdout, r.stderr


def draft(name, addr, tgt, mode, window=None):
    asm = build_asm(name, addr, tgt, mode, window=window)
    rc, out, err = run_m2c(asm)
    if rc != 0 or not out.strip():
        raise M2CError((err or out).strip() or f"m2c exited {rc}")
    return out


def _window(module, addr, pad=0x40):
    """Extra bytes past the function's own declared size, read straight from
    the module binary: mwccarm sometimes places the literal pool just past
    the symbol-table size, so a pc-relative load may reference beyond it."""
    import match as M
    rel_path, base = M.MODULES[module]
    data = (M.DSD_EXTRACT / rel_path).read_bytes()
    off = addr - base
    return data[off:off + pad] if 0 <= off < len(data) else b""


def resolve(name, module, addr):
    """(module, name, addr, size, tgt, mode, window) from the Ghidra function cache."""
    for f in F.load_funcs():
        if (name and f["name"] == name) or (addr is not None and f["addr"] == addr
                                             and (not module or f["module"] == module)):
            tgt = F.target_bytes(f)
            window = _window(f["module"], f["addr"], max(len(tgt) + 0x40, 0x80))
            return f["module"], f["name"], f["addr"], f["size"], tgt, f["mode"], window
    sys.exit(f"function not found ({name or (module, hex(addr or 0))}) - "
             f"check extracted/pictochat_funcs.json is current")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--name", default=None)
    ap.add_argument("--module", default=None)
    ap.add_argument("--addr", type=lambda x: int(x, 0), default=None)
    ap.add_argument("--dump-asm", action="store_true",
                    help="print the generated GAS asm to stderr")
    args = ap.parse_args()
    if not args.name and args.addr is None:
        ap.error("need --name or --addr")

    module, name, addr, size, tgt, mode, window = resolve(args.name, args.module, args.addr)
    try:
        asm = build_asm(name, addr, tgt, mode, window=window)
        if args.dump_asm:
            sys.stderr.write(asm)
        rc, out, err = run_m2c(asm)
    except M2CError as e:
        sys.exit(f"m2c_draft: {e}")
    if rc != 0 or not out.strip():
        sys.stderr.write(err or "")
        sys.exit(rc or 1)
    sys.stderr.write(f"[{module} {name} @ 0x{addr:08x} size 0x{size:x} {mode}] "
                     "gcc-flavored semantic draft, NOT a matching candidate\n")
    print(out, end="")


if __name__ == "__main__":
    main()
