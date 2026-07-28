# Ghidra setup

`ghidra_project/` (gitignored - it's a binary database derived from the
extracted binaries) holds a `pyghidra`-driven analysis, not the default
"import and auto-analyze" flow, because that default flow gets two things
wrong for this codebase and needs correcting every time it's rebuilt.

## Why not just `analyzeHeadless -import -analyze`

1. **Entry point.** A raw `BinaryLoader` import has no entry point, so
   Ghidra's function-start search misses most of the binary (for the
   firmware's Part 1, this was the difference between 249 and 746 detected
   functions). Always mark the real entry point and re-run analysis after:
   ```python
   prog.getSymbolTable().addExternalEntryPoint(entry)
   flat.disassemble(entry)
   flat.createFunction(entry, "_start")
   mgr = AutoAnalysisManager.getAnalysisManager(prog)
   mgr.reAnalyzeAll(None)
   mgr.startAnalysis(monitor)
   ```
2. **PictoChat's ARM9 is not one contiguous image.** See
   `notes/pictochat-layout.md`. Importing the whole `arm9_dec.bin` at a
   single base address gets the crt0 stub right and everything else wrong.
   The correct setup imports the `main` module at `0x02000400`, then adds
   separate memory blocks for `itcm` (`0x01ff8000`), `unk_autoload_0`
   (`0x02320000`, code + BSS), and `unk_autoload_2` (`0x03000000`, code +
   BSS) into the *same* program, so cross-references between them resolve.

## Rebuilding from scratch

```
pip install pyghidra
# GHIDRA_INSTALL_DIR must point at your Ghidra install, e.g.:
#   G:\Progams\ghidra_12.1.2_PUBLIC
```

There's no single canned script for this yet (it evolved as an interactive
investigation) - the working pattern is in the session history and should be
turned into a proper `tools/ghidra_setup.py` as a follow-up. Until then, the
key facts to reproduce it:

- ARM9 language: `ARM:LE:32:v5t`, compiler spec `default`.
- ARM7 language: `ARM:LE:32:v4t`, compiler spec `default`.
- Programs currently in the project:
  - `pictochat_arm9_segmented.bin` - the correct multi-segment ARM9 import
    (1,108 functions). Use this one, not the earlier flat
    `pictochat_arm9_analyzed.bin` (879 functions, wrong addresses past the
    first ~3.2KB).
  - `pictochat_arm7_analyzed.bin` - ARM7, single flat image at `0x022c0000`
    (534 functions; no segmentation issue here, ARM7 has no autoloads).
  - `part1_arm9_boot_analyzed.bin` / `part2_arm7_boot_analyzed.bin` - the
    firmware investigation's decrypted Part 1/2 (see
    `notes/firmware-investigation.md`), kept for reference.

## Decompiling a function from Python (no GUI needed)

```python
import os
os.environ["GHIDRA_INSTALL_DIR"] = r"G:\Progams\ghidra_12.1.2_PUBLIC"
import pyghidra
pyghidra.start()

from ghidra.base.project import GhidraProject
from ghidra.app.decompiler import DecompInterface
from ghidra.util.task import ConsoleTaskMonitor

proj = GhidraProject.openProject(r"C:\Users\Mitch\Projects\ndsDecomp\ghidra_project", "ndsfw", True)
prog = proj.openProgram("/", "pictochat_arm9_segmented.bin", True)
fm = prog.getFunctionManager()
af = prog.getAddressFactory()
decomp = DecompInterface()
decomp.openProgram(prog)

func = fm.getFunctionContaining(af.getAddress("02320a00"))
res = decomp.decompileFunction(func, 25, ConsoleTaskMonitor())
print(res.getDecompiledFunction().getC())

proj.close(prog)
```

Ghidra's decompiler output is a reading aid, never a source of truth - it
doesn't match on its own. Write real C and verify with `tools/match.py`.
