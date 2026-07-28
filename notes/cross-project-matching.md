# Cross-project matching

A lot of what's in any `mwccarm`-compiled DS binary isn't game-specific: the
MSL (Metrowerks Standard Library) runtime and the low-level SDK hardware
abstraction layer (cache/MPU control, interrupt enable, fast-copy primitives)
get statically linked in largely unchanged across projects, compiler version
permitting. If another decomp project has already matched one of those
functions, its C is very likely a free match here too - no need to
re-derive it from scratch.

## The tool

`tools/cross_match.py` compares this project's Ghidra-detected functions
against another project's *verified* (real matched C, not just a detected
boundary) function inventory, looking for:

1. **Byte-identical matches** - the strong, trustworthy signal. A leaf
   function with no internal calls or absolute-address references compiles
   to literally the same bytes regardless of where it's linked, so an exact
   byte match on a non-trivial function is essentially never a coincidence.
2. **Structural matches** (opcode-mnemonic sequence, addresses normalized
   out) - a weaker signal, useful as a strong starting draft but not a free
   match; report only, don't auto-promote.

```
python tools/cross_match.py --other-repo ../sm64ds-decomp
```

Requires the other project's `extracted/arm9_dec.bin` (run its own
`tools/unpack.py` first) and this project's function inventory dumped from
Ghidra to `extracted/pictochat_funcs.json` (see `notes/ghidra-setup.md`).

## What NOT to trust from a byte-identical hit

Tiny (4-byte) matches are frequently **not actually the same function** -
they're coincidental collisions between unrelated trivial compiler-generated
stubs (e.g. dozens of different C++ virtual-override methods across SM64DS
all happen to compile to the identical single `mov pc, lr`-style 4-byte body,
since an empty override has no distinguishing code at all). `cross_match.py`
will report all of them; only promote a match when the function is large
enough, or distinctive enough, that a coincidence is implausible. As a rule
of thumb: be very skeptical below ~8 bytes, and always sanity-check that a
match's *name* makes sense for what the function is actually doing (a "cache
flush" name landing on code that touches CP15 is a good sign; landing on
something in graphics/UI code with no such semantics is a red flag).

## First batch (2026-07-28)

24 functions cross-matched from `sm64ds-decomp`, all in the SDK hardware
abstraction layer - none of this is PictoChat-specific code:

- **`CP15` class** (system control coprocessor - cache/MPU): `FlushDataCache()`,
  `FlushDataCache(u32,u32)`, `InvalidateDataCache(u32,u32)`, `DrainWriteBuffer()`,
  `EnableMPU()`, `DisableMPU()`
- **`IRQ` class** (interrupt controller): `Enable()`, `Disable()`, `Restore(u32)`,
  `DisableAll()`, `RestoreAll(u32)` - each present, separately, on **both**
  ARM9 and ARM7 (statically linked into each CPU's own image)
- **`ARMProcessorMode()`** - free function, also present on both CPUs
- **`Multi{Store,Copy}*` family** - `MultiStore16`, `MultiCopyHalf`,
  `MultiStore_Int` (found at *three* separate addresses - main module, the
  main application module, and ARM7 - apparently duplicated per translation
  unit rather than deduplicated by the linker), `MultiCopy_Int`,
  `MultiStore32Bytes`, `MultiCopy32Bytes`

One near-miss not promoted: `CP15::WaitForInterrupt()` matched structurally
(0.989 ratio) but not byte-for-byte (size 12 vs 16) - worth a manual look
later, not free.

Most of these needed `-thumb` dropped from the default compile flags to
match (see `notes/setup-mwccarm.md` and each file's header) - `tools/match.py`
now auto-detects hand-`asm` blocks and drops it automatically, but a couple
(`MultiStore16`) are plain C compiled ARM-mode in the original for reasons
that aren't visible from the source, and need `--flags` passed explicitly
(documented in the file's own header comment).

Every promoted file carries a header noting where it came from and the exact
verification command - see `src/arm9/` and `src/arm7/` for the files
themselves. Credit: [tangosdev/sm64ds-decomp](https://github.com/tangosdev/sm64ds-decomp).
