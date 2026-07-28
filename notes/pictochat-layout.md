# PictoChat's real memory layout

`assets/pictochat.nds` is not one flat ARM9 blob. Trusting `dsd`'s real ROM
parsing (rather than the naive single-base `ndspy` extraction the project
started with) revealed it's four separate pieces, each loaded to its own RAM
address at boot:

| module            | RAM base     | size (code)    | size (bss) | contents |
|--------------------|--------------|-----------------|------------|----------|
| `main`             | `0x02000400` | 3,232 B (0xca0) | 0          | crt0 only - stack/mode setup, BSS clear, cache invalidate, then jumps into `unk_autoload_0` |
| `itcm`             | `0x01ff8000` | 640 B (0x280)   | 0          | code resident in the ARM9's Instruction Tightly-Coupled Memory (fast, CPU-local) - almost certainly IRQ/exception handlers |
| `unk_autoload_0`   | `0x02320000` | 303,520 B       | 365,632 B  | **the real application** - UI, drawing canvas, room/chat logic, wireless comms glue |
| `unk_autoload_2`   | `0x03000000` | 96 B            | 32 B       | tiny, in shared WRAM - likely an ARM7 IPC stub |
| `arm7`             | `0x022c0000` | 145,200 B       | (n/a, single flat image) | ARM7 side: wireless hardware driver + IPC |

`tools/match.py` and `tools/disasm.py` both know this layout (see their
`MODULES` table) - always pass `--module`, never assume `main`.

## Why this matters

The first Ghidra import (before this was understood) treated the whole
307,524-byte `pictochat_arm9_dec.bin` as one contiguous image based at
`0x02000400`. That's correct for the first ~3.2KB (the `main` crt0 stub) but
wrong for everything after it - the file is the *concatenation* of the four
pieces above, and only `main` actually runs at the address matching its
position in the file. Re-importing with the correct per-segment bases (see
`tools/extract_pictochat.py`) took function detection from 879 to **1,108**,
and every address before that fix should be treated as unreliable.

## `0x02320000` is not a coincidence

That's the exact RAM address the firmware investigation (see
[firmware-investigation.md](firmware-investigation.md)) found the *original DS
firmware's* Part 1 (ARM9 boot/menu code) loads to, and the dispatch-table
pointer traced out of that firmware code (`0x02355608`) falls squarely inside
`unk_autoload_0`'s range (`0x02320000`-`0x023c3520` including its BSS). The
standalone DSi title and the firmware-embedded system menu clearly share the
same toolchain's memory-layout convention for this class of application. This
independently confirms the firmware tracing was accurate, even though it
wasn't the toolchain we ended up using to get real source.

## How this was derived

```
tools/extract_pictochat.py     # ndspy: pulls raw arm9/arm7 out of the .nds
                                # (works, but flat/single-base - see above)

# then, for the real per-segment layout:
tools/bin/dsd.exe rom extract --rom <padded+patched copy> --arm7-bios assets/bios7.bin \
    --output-path extracted/dsd
tools/bin/dsd.exe init --rom-config extracted/dsd/config.yaml \
    --output-path config --build-path build
```

`dsd` needed two things this ROM doesn't have that a normal cartridge dump
would:

1. **Padding to the declared capacity.** The header's device-capacity byte
   says 512KB; the actual file is ~468KB (45,120 bytes short). Real dumps are
   padded with `0xFF` to the declared size; this one had that padding
   stripped. Fixed by re-padding before handing it to `dsd`.
2. **A validly KEY1-encrypted ARM9 "secure area".** Real cartridges encrypt
   the first 2KB of ARM9 (decrypting to a leading `"encryObj"` marker, checked
   by both real hardware and `dsd`); this digital title's dump has that region
   entirely zeroed (the real entry point sits exactly at the 2KB boundary, so
   no real code is lost - it's genuinely just padding for this non-cartridge
   title). Reconstructed a validly-encrypted placeholder using
   `tools/key1.py`'s primitives (the same KEY1 implementation built and
   verified during the firmware investigation), matching the exact two-stage
   encrypt sequence real hardware uses (block 0 gets encrypted with the
   level-2 keybuf *then* the level-3 keybuf; every other block just level-3 -
   this asymmetry is easy to miss and is documented in
   [firmware-investigation.md](firmware-investigation.md)).

`dsd init` itself still doesn't fully complete for this ROM - its function-
start search finds 7 functions in `main` then terminates early on an unusual
epilogue shape it doesn't expect (this main module is unusually tiny; `dsd`
is built around full retail game ROMs with much bigger main modules). Ghidra's
own analysis doesn't have this problem and is the function/symbol source of
truth for now (see `ghidra_project/`, program `pictochat_arm9_segmented.bin`
and `pictochat_arm7_analyzed.bin`). Revisiting `dsd init` once more functions
are matched by hand is a reasonable follow-up, not a blocker.
