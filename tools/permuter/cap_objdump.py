#!/usr/bin/env python3
"""Capstone-based objdump replacement for decomp-permuter, so it needs no
external arm-none-eabi-objdump. Emits `arm-none-eabi-objdump -drz`-style text
that the permuter's simplify_objdump() parses into Line(mnemonic, row,
has_symbol).

The permuter only ever compares OUR disassembly of the target against OUR
disassembly of each candidate, so we just need to be internally consistent
and parseable, not byte-identical to GNU objdump's formatting.

Ported from sm64ds-decomp's tools/permuter/cap_objdump.py. One real
difference: this project's code is mostly Thumb, not ARM, and mixes both
within the same binary - so unlike their ARM-only version, this one takes a
--thumb flag (tools/permuter/import_func.py bakes it into objdump_command per
function, from that function's own Ghidra-cached "mode").

Input: a path to an ELF .o (mwccarm output) OR a raw .bin of target bytes.
  - ELF:  disassemble the .text section; emit R_ARM_* reloc lines from
          .rel.text so the permuter wildcards relocated operands (bl targets,
          pool data refs).
  - raw:  disassemble the whole file (used for the raw-bytes target); reloc
          offsets come from a "<path>.relocs" sidecar.

Usage (permuter calls it via objdump_command):
    python cap_objdump.py [--thumb] <file>
"""
import os
import sys

from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM, CS_MODE_THUMB, CS_MODE_LITTLE_ENDIAN

_MD = {
    "arm": Cs(CS_ARCH_ARM, CS_MODE_ARM | CS_MODE_LITTLE_ENDIAN),
    "thumb": Cs(CS_ARCH_ARM, CS_MODE_THUMB | CS_MODE_LITTLE_ENDIAN),
}


def _elf_text_and_relocs(data):
    """Return (text_bytes, {offset: reloc_type}) for an ELF .o using pyelftools."""
    from io import BytesIO
    from elftools.elf.elffile import ELFFile

    elf = ELFFile(BytesIO(data))
    text = elf.get_section_by_name(".text")
    text_bytes = text.data()
    relocs = {}
    rel = elf.get_section_by_name(".rel.text") or elf.get_section_by_name(".rela.text")
    if rel is not None:
        try:
            for r in rel.iter_relocations():
                relocs[r["r_offset"]] = r["r_info_type"]
        except Exception:
            pass
    return text_bytes, relocs


def render(path, mode="arm"):
    """Return the objdump-style text lines for a file. Importable so the
    permuter can call it in-process (a real speedup over spawning python
    per candidate)."""
    md = _MD[mode]
    with open(path, "rb") as f:
        data = f.read()

    if data[:4] == b"\x7fELF":
        code, relocs = _elf_text_and_relocs(data)
    else:
        code = data
        relocs = {}
        sidecar = path + ".relocs"
        if os.path.exists(sidecar):
            for line in open(sidecar):
                line = line.split("#", 1)[0].strip()
                if not line:
                    continue
                parts = line.split(",")
                off = int(parts[0], 0)
                relocs[off] = int(parts[1], 0) if len(parts) > 1 else 28
        env = os.environ.get("CAP_OBJDUMP_RELOCS", "")
        for tok in filter(None, env.split(",")):
            relocs[int(tok, 0)] = 28

    out = ["cap_objdump: file format elf32-littlearm", "", "00000000 <func>:"]
    for ins in md.disasm(code, 0):
        if ins.address in relocs:
            # Reloc slot: a canonical placeholder so the target's real value and
            # the candidate's reloc-zero compare equal (wildcarded, like the oracle).
            out.append(f"{ins.address:8x}:\t00000000\treloc\t")
        else:
            out.append(f"{ins.address:8x}:\t{ins.bytes.hex()}\t{ins.mnemonic}\t{ins.op_str}")
    return out


def main():
    argv = sys.argv[1:]
    mode = "arm"
    if "--thumb" in argv:
        mode = "thumb"
        argv = [a for a in argv if a != "--thumb"]
    sys.stdout.write("\n".join(render(argv[0], mode)) + "\n")


if __name__ == "__main__":
    main()
