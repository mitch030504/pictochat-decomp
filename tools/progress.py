"""Report decomp completion: matched functions / bytes vs the whole game.

Ported from sm64ds-decomp's tools/progress.py, with one real difference: their
totals come from a fully-populated config/**/symbols.txt (dsd finishes for a
normal retail ROM), which lets --write-readme run from committed data alone,
with no ROM, for a hosted CI workflow. dsd's own function search never
finishes for this ROM (see notes/pictochat-layout.md), so this project's only
function-count source is the Ghidra dump (extracted/pictochat_funcs.json) -
which is gitignored (derived from copyrighted material), so it is NOT
available in a bare checkout. Consequence:

  * matched count: always computable from committed data alone (src/arm9 +
    src/arm7 file count, cross-referenced against tools/sync_ledger.py's
    marker-based lookup) - safe for CI if this project ever adds it.
  * total count / percentage: only available locally, when extracted/ exists
    (i.e. you've run tools/extract_pictochat.py + the Ghidra export). Without
    it, this reports the matched count alone and says so, rather than
    guessing or hand-waving a percentage.

Usage:
    python tools/progress.py                 # full report (needs extracted/)
    python tools/progress.py --bar           # ready-to-paste README block
    python tools/progress.py --write-readme  # rewrite that block in README.md
"""
import argparse
import pathlib
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
SRC = REPO / "src"
README = REPO / "README.md"
README_START = "<!-- progress:start -->"
README_END = "<!-- progress:end -->"

sys.path.insert(0, str(REPO / "tools"))
import ledger as L   # noqa: E402


def matched_files():
    """[(module_sub, path), ...] for every committed src/arm{9,7}/*.c[pp]."""
    out = []
    for sub in ("arm9", "arm7"):
        d = SRC / sub
        if not d.is_dir():
            continue
        for p in sorted(list(d.glob("*.c")) + list(d.glob("*.cpp"))):
            out.append((sub, p))
    return out


def totals_from_ghidra():
    """(count, bytes) from extracted/pictochat_funcs.json, if present locally."""
    import json
    cache = REPO / "extracted" / "pictochat_funcs.json"
    if not cache.is_file():
        return None
    funcs = json.loads(cache.read_text())
    return len(funcs), sum(f["size"] for f in funcs)


def matched_stats():
    """(matched_count, matched_bytes_if_known) from committed src/ + the
    Ghidra cache (for sizes only, when present - the count itself never
    depends on it).

    Sizes are joined by (module, addr) from each file's `// decomp:` marker,
    NOT by name: the Ghidra cache's names are placeholders (FUN_xxxxxxxx) as
    of whenever it was last exported, so a function renamed to a real symbol
    after that (every cross-matched one) would silently join to nothing."""
    files = matched_files()
    n = len(files)
    total_bytes = None
    cache = REPO / "extracted" / "pictochat_funcs.json"
    if cache.is_file():
        import funcs as F
        by_key = {(f["module"], f["addr"]): f["size"] for f in F.load_funcs()}
        sized, missed = 0, 0
        for _, p in files:
            head = p.read_text(encoding="utf-8", errors="ignore")[:400]
            m = L.MARKER_RE.search(head)
            if not m:
                missed += 1
                continue
            key = (m.group(1), int(m.group(2), 16))
            sized += by_key.get(key, 0)
        total_bytes = sized
    return n, total_bytes


def render_bar(matched, total, matched_bytes, total_bytes):
    if total is None:
        return (f"**{matched} function(s) matched** (byte-exact). Total function count "
                f"needs a local Ghidra export (extracted/pictochat_funcs.json) to report - "
                f"see notes/ghidra-setup.md.")
    pct = 100.0 * matched / total if total else 0.0
    bpct = (100.0 * matched_bytes / total_bytes) if total_bytes else 0.0
    filled = int(pct / 5)
    bar = "#" * filled + "-" * (20 - filled)
    return (f"**{matched} / {total} functions matched ({pct:.1f}%)**  `[{bar}]`\n"
            f"{matched_bytes} / {total_bytes} bytes ({bpct:.1f}%)")


def write_readme(block):
    text = README.read_text(encoding="utf-8")
    if README_START not in text or README_END not in text:
        pre = text.rstrip() + f"\n\n{README_START}\n{block}\n{README_END}\n"
        README.write_text(pre, encoding="utf-8")
        return
    pre, rest = text.split(README_START, 1)
    _, post = rest.split(README_END, 1)
    README.write_text(f"{pre}{README_START}\n{block}\n{README_END}{post}", encoding="utf-8")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--bar", action="store_true", help="print the README block only")
    ap.add_argument("--write-readme", action="store_true", help="rewrite it in place")
    args = ap.parse_args()

    matched, matched_b = matched_stats()
    totals = totals_from_ghidra()
    total_n, total_b = totals if totals else (None, None)

    block = render_bar(matched, total_n, matched_b, total_b)

    if args.write_readme:
        write_readme(block)
        print(f"README.md updated: {block.splitlines()[0]}")
        return
    if args.bar:
        print(block)
        return

    print(f"matched: {matched} function(s), {matched_b if matched_b is not None else '?'} bytes")
    if total_n is not None:
        print(f"total (Ghidra-known): {total_n} function(s), {total_b} bytes")
        print(f"progress: {100.0 * matched / total_n:.2f}% of functions, "
              f"{100.0 * (matched_b or 0) / total_b:.2f}% of bytes")
    else:
        print("total: unknown (no local extracted/pictochat_funcs.json - "
              "run tools/extract_pictochat.py + the Ghidra export first)")


if __name__ == "__main__":
    main()
