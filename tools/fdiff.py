"""Iteration oracle for cracking ONE hard function: compile a candidate and
show the per-instruction byte diff against the ROM, with relocation slots
wildcarded.

Unlike tools/match.py's plain pass/fail, this localizes exactly which
instructions differ, so you can converge on a big function instead of
guessing blind. Ported from sm64ds-decomp's tools/fdiff.py.

Usage:
    python tools/fdiff.py --c cand.c --func NAME --module unk_autoload_0 \
        --addr 0x02320a64 --size 0x52
    python tools/fdiff.py --c cand.c --func NAME --target-hex <hex>
"""
import argparse
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import match as M   # noqa: E402

PCREL = re.compile(r"\[pc,\s*#-?(?:0x[0-9a-fA-F]+|[0-9]+)\]")
REG = re.compile(r"\b(?:r(?:1[0-2]|[0-9])|fp|ip|sb|sl|lr)\b")


def shape(ins):
    """Mnemonic + operands with PC-relative offsets and register numbers
    squashed, for a "same shape, different regalloc" eyeball comparison."""
    if ins.mnemonic.startswith("b"):
        return ins.mnemonic
    op = PCREL.sub("[pc]", ins.op_str.replace(" ", ""))
    op = REG.sub("r", op)
    return ins.mnemonic + " " + op


def diff(target: bytes, cand: bytes, relocs: set):
    """Print a per-word table; return (ok, n_mismatches)."""
    if len(target) != len(cand):
        print(f"  size differs: target 0x{len(target):x} vs candidate 0x{len(cand):x}")
        return False, 999
    ok, ndiff = True, 0
    for i in range(0, len(target), 4):
        tw, cw = target[i:i + 4], cand[i:i + 4]
        if i in relocs:
            tag, match = "reloc (wildcard)", True
        else:
            match = tw == cw
            tag = "OK" if match else "<<< MISMATCH"
        if not match:
            ok, ndiff = False, ndiff + 1
        ti = next(M.md.disasm(tw, 0), None)
        ci = next(M.md.disasm(cw, 0), None)
        ts = f"{ti.mnemonic} {ti.op_str}" if ti else tw.hex()
        cs = f"{ci.mnemonic} {ci.op_str}" if ci else cw.hex()
        same_shape = (ti is not None and ci is not None and not match
                      and shape(ti) == shape(ci))
        note = tag if not same_shape or match else tag + "  (same shape - regalloc/coloring only)"
        print(f"   +0x{i:02x} | {tw.hex()} {ts:26} | {cw.hex()} {cs:26} | {note}")
    return ok, ndiff


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--c", required=True)
    ap.add_argument("--func", required=True)
    ap.add_argument("--target-hex", default=None)
    ap.add_argument("--module", default="unk_autoload_0", choices=list(M.MODULES))
    ap.add_argument("--addr", type=lambda x: int(x, 0), default=None)
    ap.add_argument("--size", type=lambda x: int(x, 0), default=None)
    ap.add_argument("--version", default=M.CANONICAL)
    ap.add_argument("--flags", default=None)
    args = ap.parse_args()

    if args.target_hex:
        target = bytes.fromhex(args.target_hex)
    elif args.addr is not None and args.size is not None:
        target = M.target_bytes(args.module, args.addr, args.size)
    else:
        ap.error("give --target-hex, or --module/--addr/--size")

    flags = args.flags or (M.DEFAULT_FLAGS_ARM7 if args.module == "arm7" else M.DEFAULT_FLAGS)
    cfile = pathlib.Path(args.c)
    src_text = cfile.read_text(encoding="utf-8")
    if src_text.startswith("//cpp") and "-lang c99" in flags:
        flags = flags.replace("-lang c99", "-lang c++")
    if not args.flags and re.search(r"\basm\b", src_text) and "-thumb" in flags:
        flags = flags.replace(" -thumb", "")

    obj = M.compile_c(cfile, args.version, flags)
    if obj is None:
        sys.exit(1)
    code, relocs = M.extract_func(obj, args.func)
    if code is None:
        sys.exit(f"symbol '{args.func}' not found in compiled object")

    ok, ndiff = diff(target, code, relocs)
    print(f"\nRESULT match={ok} mismatches={ndiff}/{len(target)//4}")


if __name__ == "__main__":
    main()
