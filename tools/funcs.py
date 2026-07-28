"""Function inventory + module resolution for this project.

Single source of truth for "what functions are there and which module do they
belong to": extracted/pictochat_funcs.json (a Ghidra dump - see
notes/ghidra-setup.md). Unlike sm64ds-decomp's modules.py/sweep.py (which read
a fully-populated config/**/symbols.txt from dsd), our dsd init never
completes for this ROM (see notes/pictochat-layout.md - it terminates early on
this title's unusually small main module), so everything here reads the
Ghidra cache instead. Regenerate it if it goes stale.
"""
import json
import pathlib
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
from match import MODULES, DSD_EXTRACT  # noqa: E402

FUNCS_CACHE = REPO / "extracted" / "pictochat_funcs.json"

_MODULE_RANGES = None


def _module_ranges():
    """cpu -> [(module_name, base, size), ...], built from MODULES + the
    actual extracted binary sizes on disk."""
    global _MODULE_RANGES
    if _MODULE_RANGES is not None:
        return _MODULE_RANGES
    ranges = {"arm9": [], "arm7": []}
    for name, (relpath, base) in MODULES.items():
        p = DSD_EXTRACT / relpath
        if not p.is_file():
            continue
        cpu = "arm7" if name == "arm7" else "arm9"
        ranges[cpu].append((name, base, p.stat().st_size))
    _MODULE_RANGES = ranges
    return ranges


def module_for(cpu, addr):
    """Module name (a key of match.MODULES) containing addr, or None."""
    for name, base, size in _module_ranges().get(cpu, []):
        if base <= addr < base + size:
            return name
    return None


def src_dir_for(module):
    """This project splits src/ by CPU (arm9/arm7), not by module - all four
    ARM9 modules (main/itcm/unk_autoload_0/unk_autoload_2) share src/arm9/."""
    return REPO / "src" / ("arm7" if module == "arm7" else "arm9")


def load_funcs():
    """All Ghidra-known functions: [{cpu,name,addr,size,mode,bytes,module}, ...].

    `module` is resolved here from addr via the module table - the raw cache
    only records `cpu` (arm9/arm7), not which of arm9's four segments a
    function lives in.
    """
    if not FUNCS_CACHE.is_file():
        sys.exit(f"{FUNCS_CACHE} not found - see notes/ghidra-setup.md to (re)export it")
    out = []
    for f in json.loads(FUNCS_CACHE.read_text()):
        f = dict(f)
        f["module"] = module_for(f["cpu"], f["addr"])
        out.append(f)
    return out


def target_bytes(f):
    return bytes.fromhex(f["bytes"])
