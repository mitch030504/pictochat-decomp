"""Look for functions shared between this project and another mwccarm-built
decomp project (default: sm64ds-decomp) - compiler-runtime (MSL) helpers and
common SDK routines get statically linked byte-for-byte into any project
built with a compatible compiler version, independent of the actual game.
Finding one means free, already-verified C source to adapt instead of a
from-scratch match.

Two matching tiers:
  1. Byte-identical: the strongest, zero-risk signal. Leaf functions with no
     internal calls or absolute-address references compile to literally the
     same bytes regardless of where they're linked. If found, the other
     project's matched C is very likely a drop-in match here too - just
     verify with tools/match.py.
  2. Structural (opcode-mnemonic sequence, addresses normalized out): a
     weaker signal - same shape, but calls/data refs will differ post-link.
     Worth a look as a strong starting draft, not a free match.

Usage:
    python tools/cross_match.py --other-repo ../sm64ds-decomp
    python tools/cross_match.py --other-repo ../sm64ds-decomp --min-fuzzy 0.9
"""
import argparse
import json
import pathlib
import re
import sys

import capstone

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
from match import MODULES  # noqa: E402

# where this project's Ghidra-dumped function inventory lives - see
# notes/ghidra-setup.md; regenerate with the dump script referenced there if
# missing or stale.
OUR_FUNCS_CACHE = REPO / "extracted" / "pictochat_funcs.json"


def load_our_funcs():
    if not OUR_FUNCS_CACHE.exists():
        sys.exit(f"{OUR_FUNCS_CACHE} not found - dump it from Ghidra first "
                  f"(see notes/ghidra-setup.md for the function-inventory export script)")
    return json.loads(OUR_FUNCS_CACHE.read_text())


def load_other_verified(other_repo: pathlib.Path):
    """Parse <other_repo>/config/arm9/symbols.txt + symbols/verified.tsv for
    every function that's both known (has a size) and actually matched (has
    real C source, not just a detected boundary)."""
    verified_path = other_repo / "symbols" / "verified.tsv"
    symbols_path = other_repo / "config" / "arm9" / "symbols.txt"
    if not verified_path.exists() or not symbols_path.exists():
        sys.exit(f"expected {verified_path} and {symbols_path} - is --other-repo "
                  f"pointing at a checked-out sm64ds-decomp-style project?")

    verified_names = set()
    for line in verified_path.read_text().splitlines():
        parts = line.split("\t")
        if len(parts) >= 3 and parts[2] == "function":
            verified_names.add(parts[1])

    sym_re = re.compile(r"^(\S+) kind:function\((thumb|arm),size=(0x[0-9a-fA-F]+)\) addr:(0x[0-9a-fA-F]+)")
    entries = []
    for line in symbols_path.read_text().splitlines():
        m = sym_re.match(line)
        if not m:
            continue
        name, mode, size_s, addr_s = m.groups()
        if name not in verified_names:
            continue
        entries.append({"name": name, "mode": mode, "size": int(size_s, 0), "addr": int(addr_s, 0)})

    arm9_bin = other_repo / "extracted" / "arm9_dec.bin"
    if not arm9_bin.exists():
        sys.exit(f"{arm9_bin} not found - run the other project's tools/unpack.py first")
    data = arm9_bin.read_bytes()
    # sm64ds-decomp's own tools/match.py hardcodes this - see its ARM9_BASE.
    base = 0x02004000
    for e in entries:
        off = e["addr"] - base
        e["bytes"] = data[off:off + e["size"]].hex() if 0 <= off and off + e["size"] <= len(data) else None
    return [e for e in entries if e["bytes"]]


def fingerprint(raw: bytes, mode: str) -> str:
    """Normalized opcode-mnemonic sequence: keep the mnemonic and small/safe
    immediates (shifts, comparisons, struct offsets), drop anything that
    looks like an absolute address or PC-relative literal load target (those
    are link-position-dependent and will differ between two binaries even
    for identical source)."""
    md = capstone.Cs(capstone.CS_ARCH_ARM, capstone.CS_MODE_THUMB if mode == "thumb" else capstone.CS_MODE_ARM)
    toks = []
    try:
        for insn in md.disasm(raw, 0):
            op = insn.op_str
            op = re.sub(r"#0x[0-9a-fA-F]{5,}", "#ADDR", op)  # big immediates -> likely addresses
            op = re.sub(r"\[pc,?[^\]]*\]", "[pc,ADDR]", op)   # PC-relative literal loads
            toks.append(f"{insn.mnemonic} {op}")
    except Exception:
        return ""
    return "|".join(toks)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--other-repo", required=True, type=pathlib.Path)
    ap.add_argument("--min-fuzzy", type=float, default=0.95,
                     help="min structural-similarity ratio to report a non-byte-identical candidate")
    args = ap.parse_args()

    other = load_other_verified(args.other_repo.resolve())
    ours = load_our_funcs()
    print(f"Loaded {len(other)} verified functions from {args.other_repo}, "
          f"{len(ours)} candidate functions here.")

    by_size_mode = {}
    for o in ours:
        by_size_mode.setdefault((o["size"], o["mode"]), []).append(o)

    exact, fuzzy = [], []
    for e in other:
        candidates = by_size_mode.get((e["size"], e["mode"]), [])
        if not candidates:
            continue
        efp = None
        for c in candidates:
            if c["bytes"] == e["bytes"]:
                exact.append((e, c))
                continue
            if efp is None:
                efp = fingerprint(bytes.fromhex(e["bytes"]), e["mode"])
            if not efp:
                continue
            cfp = fingerprint(bytes.fromhex(c["bytes"]), c["mode"])
            if not cfp:
                continue
            import difflib
            ratio = difflib.SequenceMatcher(None, efp, cfp).ratio()
            if ratio >= args.min_fuzzy and (e, c, ratio) not in fuzzy:
                fuzzy.append((e, c, ratio))

    print(f"\n=== BYTE-IDENTICAL matches: {len(exact)} ===")
    for e, c in exact:
        print(f"  {e['name']} (0x{e['addr']:08x}, {e['size']} bytes, {e['mode']}) "
              f"== {c['cpu']}:{c['name']} @ 0x{c['addr']:08x}")
        print(f"      src: {args.other_repo}/src/{e['name']}.c "
              f"(or grep src/ - not every function is named after its symbol on disk)")

    print(f"\n=== structural matches (ratio >= {args.min_fuzzy}), not byte-identical: {len(fuzzy)} ===")
    for e, c, ratio in sorted(fuzzy, key=lambda x: -x[2])[:40]:
        print(f"  {ratio:.3f}  {e['name']} (0x{e['addr']:08x}) ~ {c['cpu']}:{c['name']} @ 0x{c['addr']:08x} "
              f"(size {e['size']})")

    if not exact and not fuzzy:
        print("\nNo candidates found. Expected for small samples - re-run once "
              "more of this project's functions are in the Ghidra dump, or "
              "lower --min-fuzzy.")


if __name__ == "__main__":
    main()
