"""Differential compiler probe: find C constructs whose codegen differs across
mwccarm versions, to pin the exact one PictoChat was built with.

Compiles one C file with each candidate version and compares every function's
bytes ACROSS versions (objects are unlinked, so reloc slots are zero in all
and a raw compare is valid). Reports which functions discriminate the
versions. Ported near-verbatim from sm64ds-decomp's tools/probe_versions.py -
tools/match.py already shares its compile_c/DEFAULT_FLAGS/PINNED names.

This project's compiler pin (dsi/1.3) is still just a best guess from
copyright-date reasoning (see notes/setup-mwccarm.md) - every real match so
far happens to compile identically across the whole dsi/ family, so nothing
has discriminated it yet. Feed this a C file that exercises something more
version-sensitive (struct layout edge cases, unusual optimizer decisions) to
actually narrow it down.

Usage:
    python tools/probe_versions.py --c tools/probes/discriminate.c
    python tools/probe_versions.py --c <file> --versions dsi/1.1,dsi/1.3,dsi/1.6sp2
"""
import argparse
import io
import pathlib
import sys

from elftools.elf.elffile import ELFFile

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from match import compile_c, DEFAULT_FLAGS, PINNED  # noqa: E402


def funcs_in(obj: bytes) -> dict:
    """name -> code bytes for every defined function symbol in the object."""
    elf = ELFFile(io.BytesIO(obj))
    st = elf.get_section_by_name(".symtab")
    out = {}
    for s in st.iter_symbols():
        if s["st_info"]["type"] == "STT_FUNC" and s["st_size"] > 0:
            sec = elf.get_section(s["st_shndx"])
            out[s.name] = sec.data()[s["st_value"]:s["st_value"] + s["st_size"]]
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--c", required=True)
    ap.add_argument("--versions", default=",".join(PINNED))
    ap.add_argument("--flags", default=DEFAULT_FLAGS)
    args = ap.parse_args()

    versions = [v.strip() for v in args.versions.split(",") if v.strip()]
    cfile = pathlib.Path(args.c)

    per_version = {}
    for v in versions:
        obj = compile_c(cfile, v, args.flags)
        if obj is None:
            print(f"  ! compile failed for {v}")
            continue
        per_version[v] = funcs_in(obj)

    if len(per_version) < 2:
        print("need >=2 versions compiling to compare")
        return

    names = sorted(set().union(*[set(d) for d in per_version.values()]))
    discriminating = []
    for name in names:
        variants = {}
        for v, d in per_version.items():
            variants.setdefault(d.get(name, b"").hex(), []).append(v)
        if len(variants) > 1:
            discriminating.append((name, variants))

    vs = list(per_version.keys())
    print(f"versions compared: {', '.join(vs)}")
    print(f"functions: {len(names)}   DISCRIMINATING: {len(discriminating)}\n")
    if not discriminating:
        print("=> all versions produced byte-identical code for every probe function.")
        print("   These builds are codegen-equivalent here; any is safe as canonical")
        print("   (this probe file doesn't narrow it down - try a different construct).")
        return
    for name, variants in discriminating:
        print(f"### {name} - splits the versions:")
        for hexb, vlist in variants.items():
            print(f"   [{', '.join(vlist)}]  len {len(hexb)//2}  {hexb}")
        print()


if __name__ == "__main__":
    main()
