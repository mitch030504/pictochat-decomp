# The firmware investigation (reference)

Before `assets/pictochat.nds` (a standalone DSi system title) was found, this
project spent considerable effort trying to extract PictoChat directly from
the original DS firmware chip dump (`assets/firmware.bin`). That's no longer
the primary path - `assets/pictochat.nds` gives real, unencrypted, byte-exact
Nintendo code with none of the problems below - but the work is real,
verified, and documented here because (a) parts of it are genuinely solved
and reusable, and (b) it independently cross-validated the PictoChat findings
(see [pictochat-layout.md](pictochat-layout.md)).

## What's fully solved

Per GBATEK's "DS Firmware Header" and "DS Encryption by Gamecode/Idcode
(KEY1)" pages, cross-verified instruction-for-instruction against the real
`bios7.bin`'s disassembly (via the
[PikalaxALT/ndsbios](https://github.com/PikalaxALT/ndsbios) project):

- **Firmware Part 1** (ARM9 boot code) and **Part 2** (ARM7 boot code) are
  KEY1 (Blowfish-based) encrypted, then LZ77-compressed. `tools/key1.py`
  implements this and is verified correct: it decrypts+decompresses both
  parts of a real firmware dump into genuine, recognizable ARM9/ARM7 crt0
  code (IME disable at `0x04000208`, SVC/IRQ/System mode stack setup, MMU
  init via `mrc`/`mcr p15`).
- The KEY1 key table comes from `bios7.bin` offset `0x30` - decrypting
  *anything* KEY1-protected on this console (firmware boot code, a
  cartridge's secure area) needs a real ARM7 BIOS dump for this reason.
- KEY1 encryption is used far more broadly than just firmware: it's also how
  retail cartridges protect their ARM9 secure area, and `tools/key1.py`'s
  primitives were reused as-is (see [pictochat-layout.md](pictochat-layout.md))
  to reconstruct a valid secure area for `dsd` on the *PictoChat* title,
  which needed the same algorithm applied with different parameters
  (`level=3, modulo=8`, gamecode-derived idcode) - a good example of the
  investigation paying off somewhere unexpected.
- Firmware Part 1's real RAM load address is `0x02320000` (from the header's
  documented `2800000h-ramaddr` field, not the naive guess of `0x02000000`
  this project started with).

## What's still open

**Firmware Part 3** (the "ARM9 GUI code" - includes PictoChat, Health &
Safety, Download Play, and Settings, all bundled together) never got solved.
Extensive static tracing (Ghidra, full BIOS disassembly review, checking every
open-source NDS decomp tool and emulator that could conceivably know) mapped
the loading architecture in detail:

- Part 3/4's raw bytes get read via a real SPI flash protocol (command `0x03`
  + 3-byte address) by ARM7, into a 256KB circular staging buffer at
  `0x022c0000`-`0x02300000` in shared Main RAM.
- Part 4 (ARM7 WiFi driver) decompresses/relocates to `0x02300000` onward -
  confirmed by finding ARM7's own code calling directly into ~31 external
  function pointers there (WiFi/DHCP-looking logic).
- Part 3 almost certainly follows the same pattern into ARM9-addressable RAM
  around `0x02320000` (matching PictoChat's own `unk_autoload_0` base - see
  above), handed off from ARM7 to ARM9 via the inter-processor IPC FIFO rather
  than a statically-known shared address.
- The exact byte-level compression/transform between the staged raw bytes and
  the final decompressed code was never found. Ruled out: plain SWI-style
  LZ77, plain Huffman, the "BLZ" backward-compression format `sm64ds-decomp`
  uses for game ROM `arm9.bin` (brute-forced every possible end-boundary,
  zero valid decompressions). The DS BIOS itself was checked exhaustively
  (every call site of every decompression SWI in `bios7.bin`) and does not
  touch Part 3/4/5 at all - whatever handles it is custom code inside the
  firmware's own Part 1/2, not anything documented anywhere public.

If this is revisited, the productive next step isn't more static tracing (it
had reached real diminishing returns) - it's dynamic analysis: either finish
the minimal ARM7 interpreter sketched in `tools/arm_interp.py` (incomplete,
paused when `pictochat.nds` was found), or run the firmware in melonDS/DeSmuME
with a real debugger and watch the transform happen live.

## Tools from this investigation still in the repo

- `tools/key1.py` - KEY1 Blowfish decrypt/encrypt, reused for both firmware
  and (via a hand patch) the PictoChat ROM's secure area.
- `tools/extract_firmware.py` - decrypts+decompresses firmware Part 1/2,
  dumps Part 3/4/5 raw (format still unknown).
- `tools/arm_interp.py` - incomplete ARMv4T interpreter, paused mid-build.
- `ghidra_project/` contains `part1_arm9_boot_analyzed.bin` and
  `part2_arm7_boot_analyzed.bin` alongside the PictoChat programs, fully
  analyzed and ready to pick back up.
