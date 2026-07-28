# config/

Intended to hold `dsd`-format per-module symbol/relocation/delink config
(`config.yaml`, `symbols.txt`, `relocs.txt`, `delinks.txt` per module -
`arm9/`, `arm7/`), the same role this directory plays in `sm64ds-decomp`.

**Not yet populated.** `dsd init` doesn't complete for this title yet - it
finds 7 functions in the tiny `main` module and then terminates on an
unusual epilogue shape before producing config output. See
[../notes/pictochat-layout.md](../notes/pictochat-layout.md) for the full
story and [../notes/ghidra-setup.md](../notes/ghidra-setup.md) for how
function/symbol information is being tracked in the meantime (Ghidra's own
analysis, in `ghidra_project/`).

Revisiting this - either by fixing/working around the `dsd init` issue, or by
exporting Ghidra's analysis into these files by hand - is a reasonable next
step once a few functions are matched by hand and the shape of the codebase
is better understood.
