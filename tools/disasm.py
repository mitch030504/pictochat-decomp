"""Disassemble a region of an extracted PictoChat binary to ARM/Thumb assembly.

Quick scratch disassembler for eyeballing functions before matching them (see
tools/match.py) or reading them in Ghidra. Operates on extracted/dsd/ (see
tools/extract_pictochat.py). Knows this project's module layout so you pass a
module name instead of a raw file path - see notes/pictochat-layout.md for why
"unk_autoload_0" is the module that holds almost everything interesting.

Usage:
    # Disassemble 0x40 bytes of the main application module at 0x02320a00
    python tools/disasm.py --module unk_autoload_0 --addr 0x02320a00 --length 0x40

    # ARM7 is Thumb by default at its entry; pass --arm for ARM-mode regions
    python tools/disasm.py --module arm7 --addr 0x022c0100 --length 0x40 --thumb
"""
import argparse
import pathlib
import sys

try:
    from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM, CS_MODE_THUMB
except ImportError:
    sys.exit("capstone not installed. Run: pip install capstone")

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from match import MODULES, DSD_EXTRACT  # noqa: E402


def auto_int(x: str) -> int:
    return int(x, 0)


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--module", default="unk_autoload_0", choices=list(MODULES))
    ap.add_argument("--addr", type=auto_int, required=True, help="RAM address to start at")
    ap.add_argument("--length", type=auto_int, default=0x100, help="bytes to disassemble")
    ap.add_argument("--thumb", action="store_true", help="decode as Thumb instead of ARM")
    args = ap.parse_args()

    rel_path, base = MODULES[args.module]
    data = (DSD_EXTRACT / rel_path).read_bytes()
    off = args.addr - base
    if off < 0 or off >= len(data):
        sys.exit(f"address 0x{args.addr:08x} out of range for module {args.module} "
                  f"(base 0x{base:08x}, size 0x{len(data):x})")
    chunk = data[off:off + args.length]

    md = Cs(CS_ARCH_ARM, CS_MODE_THUMB if args.thumb else CS_MODE_ARM)
    md.detail = False

    for insn in md.disasm(chunk, args.addr):
        raw = insn.bytes.hex()
        print(f"{insn.address:08X}  {raw:<8}  {insn.mnemonic:<7} {insn.op_str}")


if __name__ == "__main__":
    main()
