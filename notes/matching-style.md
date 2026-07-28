# Matching conventions

Adopted from `sm64ds-decomp` - same standard the N64 `sm64` project and the
whole pret/decomp community holds to.

## What counts as a match

Source that, compiled with the pinned `mwccarm` version and flags, produces
object code **byte-identical** to the real binary - every instruction word,
and every relocation slot pointing at the correct destination (a raw byte
diff alone would wildcard a wrong callee/global sitting in a reloc slot;
`tools/match.py` checks both).

Nothing is "close enough." A function that's off by one instruction is not
matched - keep iterating, or leave it unmatched and move on rather than
committing a near-miss as if it were done.

## One function per file

`src/arm9/<name>.c` or `src/arm7/<name>.c`, named after the function's real
symbol name if known, otherwise `func_<hex_address>.c` (matching the address
in the module it lives in - see `notes/pictochat-layout.md` for which module
is which). Small groups of tightly related functions (e.g. a getter/setter
pair) in one file are fine; whole subsystems in one file are not.

## Import knowledge, write code

You may use publicly known symbol names, struct layouts, or field offsets
(from community reverse-engineering of PictoChat/NitroSDK, if such exists) -
but all C in `src/` must be written from scratch against this project's own
extracted binary. Never paste source from another project.

## Workflow

1. Pick an unmatched function. `python tools/disasm.py --module <mod> --addr
   0x... --length 0x...` to read it (or open it in Ghidra - see
   `notes/ghidra-setup.md`).
2. Write C for it (scratch/draft is fine to start).
3. `python tools/match.py --c yourfile.c --func name --addr 0x... --size 0x..
   --module <mod>` and iterate until every word matches.
4. Move the finished file into `src/<cpu>/`.

## Codegen habits worth knowing up front

Not yet written for this compiler/title specifically - `sm64ds-decomp`'s
`notes/mwccarm-codegen.md` and `notes/pret-idioms.md` are excellent reading
for the same compiler family (struct copies, bitfield shifts, the
register-allocation/materialization idioms mwccarm 1.x/2.x favor) even though
they were written against a different game; worth skimming before writing
much C here. Once this project accumulates its own hard-won codegen notes,
they belong in a `notes/mwccarm-codegen.md` here too.
