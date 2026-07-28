"""Per-function Ghidra decompiler draft - the mature-decompiler counterpart
to tools/m2c_draft.py.

Neither this nor m2c_draft.py produces a matching candidate - both are
reading aids for hand-writing one. They fail differently, so it's worth
pulling both before committing to a translation: m2c's ad-hoc capstone-based
lifting is more literal (closer to the actual instruction sequence, which
sometimes helps for register/scheduling-sensitive code) but can badly mangle
local stack structs/arrays; Ghidra's mature SSA-based decompiler models
those correctly far more often, at the cost of sometimes restructuring
control flow further from what actually has to be reproduced. Try the one
that reads clearer for a given function, or both.

This is deliberately NOT a batch-export-everything tool: most functions in
a 1500-function backlog never get looked at in a given pass, so drafting
them all up front is wasted work that would also go stale as Ghidra's
analysis or this ROM's understanding improves. Run it per function as you
pick targets, the same way you'd run m2c_draft.py.

Slow to start (JVM + Ghidra project load each invocation, unlike m2c_draft's
near-instant pure-Python pipeline) - expect several seconds, not
milliseconds. If you're drafting more than one or two functions in a
sitting, consider `--name` twice in one interactive pyghidra session instead
of re-invoking this per function (see notes/ghidra-setup.md).

Usage:
    python tools/ghidra_draft.py --name FUN_02321940
    python tools/ghidra_draft.py --module arm7 --addr 0x022ce8b0
"""
import argparse
import os
import pathlib
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import funcs as F  # noqa: E402

GHIDRA_INSTALL_DIR = os.environ.get("GHIDRA_INSTALL_DIR", r"G:\Progams\ghidra_12.1.2_PUBLIC")
PROJECT_DIR = REPO / "ghidra_project"
PROJECT_NAME = "ndsfw"
# See notes/ghidra-setup.md: the ARM9 side needs the multi-segment import
# (pictochat_arm9_segmented.bin) - the flat one has wrong addresses past the
# first ~3.2KB. ARM7 has no segmentation issue (single flat image).
PROGRAM_FOR = {
    "arm7": "pictochat_arm7_analyzed.bin",
}
DEFAULT_PROGRAM = "pictochat_arm9_segmented.bin"


def resolve(name, module, addr):
    for f in F.load_funcs():
        if f["module"] is None:
            continue
        if (name and f["name"] == name) or (addr is not None and f["addr"] == addr
                                             and (not module or f["module"] == module)):
            return f
    sys.exit(f"function not found ({name or (module, hex(addr or 0))}) - "
             f"check extracted/pictochat_funcs.json is current")


def draft(module, addr):
    """The decompiled C text for one function. Starts pyghidra and opens the
    project fresh each call - see the module docstring for why that's fine
    for the intended per-function usage."""
    os.environ.setdefault("GHIDRA_INSTALL_DIR", GHIDRA_INSTALL_DIR)
    import pyghidra
    pyghidra.start()

    from ghidra.base.project import GhidraProject
    from ghidra.app.decompiler import DecompInterface
    from ghidra.util.task import ConsoleTaskMonitor

    program_name = PROGRAM_FOR.get(module, DEFAULT_PROGRAM)
    proj = GhidraProject.openProject(str(PROJECT_DIR), PROJECT_NAME, True)
    try:
        prog = proj.openProgram("/", program_name, True)
        try:
            fm = prog.getFunctionManager()
            af = prog.getAddressFactory()
            decomp = DecompInterface()
            decomp.openProgram(prog)

            func = fm.getFunctionContaining(af.getAddress(f"{addr:08x}"))
            if func is None:
                sys.exit(f"no function containing 0x{addr:08x} in {program_name} - "
                         f"Ghidra's own boundaries may disagree with the cache; "
                         f"see notes/pictochat-layout.md")
            res = decomp.decompileFunction(func, 30, ConsoleTaskMonitor())
            if not res.decompileCompleted():
                sys.exit(f"decompilation failed: {res.getErrorMessage()}")
            return res.getDecompiledFunction().getC()
        finally:
            proj.close(prog)
    finally:
        proj.close()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--name", default=None)
    ap.add_argument("--module", default=None)
    ap.add_argument("--addr", type=lambda x: int(x, 0), default=None)
    args = ap.parse_args()
    if not args.name and args.addr is None:
        ap.error("need --name or --addr")

    f = resolve(args.name, args.module, args.addr)
    sys.stderr.write(f"[{f['module']} {f['name']} @ 0x{f['addr']:08x} size 0x{f['size']:x} "
                     f"{f['mode']}] Ghidra decompiler draft, NOT a matching candidate\n")
    print(draft(f["module"], f["addr"]))


if __name__ == "__main__":
    main()
