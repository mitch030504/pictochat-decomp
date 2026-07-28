# Tooling for contributors

This project's automation is adapted from
[sm64ds-decomp](https://github.com/tangosdev/sm64ds-decomp), which has built
a much larger tool pile (~90 scripts) over a longer-running, larger-scale
effort. Most of that is either game-specific (SM64DS actor names, C++ ABI
templates tuned to their class layouts) or built for a scale of coordinated
effort (a hosted claims-lock service, an LLM-fanout pipeline,
worklist/ledger dashboards) this project doesn't need yet. What's ported here
is the subset that's genuinely portable and useful at this project's size:
free-match generators, iteration aids, and the permuter bridge. This doc is
the map of what exists and when to reach for each one.

## The order to reach for things

1. **`tools/sync_ledger.py --apply`** - run this first, always, after cloning
   or pulling. It catches your local (gitignored) match ledger up to what's
   already committed in `src/`, so the free-match tools below don't waste
   time re-deriving functions that are already done.
2. **`tools/cross_match.py`** - free matches from another compatible decomp
   project's already-verified code (SDK/runtime primitives that get linked in
   byte-identical regardless of the game). See
   [cross-project-matching.md](cross-project-matching.md).
3. **`tools/templates.py --apply`** - free matches from trivial leaf shapes
   (empty stubs, constant/argument returns, single-field get/set, two-arg
   arithmetic, simple global getters) - no hand-writing needed, every
   candidate is oracle-verified before banking. Cheap to re-run periodically
   as more functions get discovered.
4. **Pick a real target** and read it: `tools/disasm.py` for a quick look.
   For anything past a few instructions, **`tools/m2c_draft.py`** and
   **`tools/ghidra_draft.py`** each give a real semantic C draft to start
   hand-writing a candidate from - see their sections below for why it's
   worth having both, not just one.
5. **Iterate** with `tools/match.py` (pass/fail + full diff) or
   **`tools/fdiff.py`** (always shows the per-instruction diff, with a note
   on which mismatches are "same shape, different register" - i.e.
   permuter-fixable vs a real logic difference).
6. **Close the last mile with decomp-permuter** if you have a candidate that
   compiles with the right *shape* but wrong register coloring/ordering - see
   the permuter section below. This is genuinely free: it brute-forces
   exactly the part that's tedious to do by hand.
7. **If it's logic-correct but genuinely can't be made byte-exact** at this
   compiler tier (a real codegen floor, not "haven't figured it out yet"),
   park it with `tools/nonmatching.py add` instead of leaving it unmatched
   forever.
8. **`tools/progress.py --write-readme`** to refresh the README's progress
   bar before you open a PR.

## tools/ledger.py, tools/sync_ledger.py

The match ledger (`progress/matched.jsonl`, `progress/nonmatching.jsonl`) is
local-only (gitignored) - two contributors' bookkeeping never conflicts on
disk. `tools/ledger.py` is the only code that should touch those files
(`bank()` for a verified match, `append_nonmatching()` for a park); it holds
a cross-process lock so two tools banking at once can't double-write, and it
"adopts" a merged PR's `src/` file cleanly even though the ledger never saw
it land.

Every file `ledger.bank()` writes gets a `// decomp: module=... addr=...
name=...` marker comment prepended. This is what lets `tools/sync_ledger.py`
rebuild the ledger from committed `src/` alone on a fresh clone - it's more
reliable than matching by filename, because a function's Ghidra-cached name
(`FUN_xxxxxxxx`) goes stale the moment it's renamed to a real symbol, and the
filename convention alone can't tell the difference.

## tools/templates.py

A port of sm64ds-decomp's `swarm.py` template-rule engine - every leaf rule
there (the ones needing just `(name, ins, bytes)`, no relocs/symbol table):
empty/constant/argument-return, field load/store/mask/bitfield, bit-test and
bitop patterns, global getters/setters/swaps/indexed access, small fixed-size
struct copies, multi-field constant initializers, and more (33 rules total -
see the file for the full list). Not ported: the C++-ABI rules there
(vtable/ctor/dtor chains, virtual/PMF calls) tuned to SM64DS's own class
layouts, and everything needing callee-name resolution via a populated
`config/relocs.txt`, which this project doesn't have (see
[pictochat-layout.md](pictochat-layout.md) - `dsd init` never finishes for
this ROM).

Mode-aware: tries each rule against the target's own ARM or Thumb encoding
(the `mode` field in `extracted/pictochat_funcs.json`), unlike sm64ds-decomp's
ARM-only version - Thumb mnemonics that implicitly set flags (`movs`/`adds`/
`ands`/...) are normalized to their ARM equivalents so one rule covers both
encodings. A few rules key on ARM's per-instruction condition codes
(`moveq`/`movne`, used for the classic "compare then materialize 0/1" idiom)
that don't exist in Thumb1 on this ARMv5TE-era ROM (no IT-block support) -
those are gated to ARM-mode targets only (`ARM_ONLY_RULES`).

Every proposed candidate is compiled and byte-diffed before being reported,
so a rule misfiring just means "no candidate," never a wrong one.

## tools/m2c_draft.py

Runs [m2c](https://github.com/matt-kempster/m2c) (matt-kempster's semantic
decompiler) against a function's disassembly, via a capstone-to-GAS-assembly
bridge (this project's version handles the ARM/Thumb mix; see the file's own
docstring for the encoding details). The output is a gcc-flavored semantic
draft - real control flow and data references, useful as a reading aid and a
starting point to hand-write from - but it will **not** compile under
`mwccarm` as-is and is not a matching candidate on its own. Requires
`vendor/m2c` (`git clone https://github.com/matt-kempster/m2c vendor/m2c`).

Two real limits versus sm64ds-decomp's version: no `config/relocs.txt` means
callee/global references show as `func_<addr>` / raw hex instead of real
symbol names (still useful, just less self-documenting); and very large
functions can still trip the pc-relative-pool-window heuristic on unusual
layouts (see `_window()` in the file).

## tools/ghidra_draft.py

Runs Ghidra's own decompiler (the mature, SSA-based one, via `pyghidra`
against `ghidra_project/` - see [ghidra-setup.md](ghidra-setup.md)) for one
function and prints its C. Same disclaimer as m2c: a reading aid, not a
matching candidate, won't compile under `mwccarm` as-is.

Worth pulling **both** this and `m2c_draft.py` rather than picking one -
they fail differently. `m2c`'s capstone-based lifting is more literal
(closer to the raw instruction sequence, which sometimes helps for
scheduling-sensitive code) but can badly mangle local stack structs/arrays;
Ghidra's decompiler models those correctly far more often, at the cost of
sometimes restructuring control flow further from what you actually have to
reproduce. Concretely: for `FUN_02321940`, `m2c_draft.py` produced a bogus
`&subroutine_arg0 - 4` pointer expression where a local stack array was
actually being passed by address - `ghidra_draft.py` got that one right
(`auStack_28`).

Deliberately **not** a batch-export-everything tool - see the module's own
docstring for why (most of a 1500-function backlog never gets looked at in
a given pass, so drafting it all up front is wasted, staleness-prone work).
Run it per function as you pick targets, same as `m2c_draft.py`. It's slower
to start than `m2c_draft.py` (a JVM + Ghidra project load per invocation,
not m2c's near-instant pure-Python pipeline) - expect several seconds.

```
python tools/ghidra_draft.py --name FUN_02321940
python tools/ghidra_draft.py --module arm7 --addr 0x022ce8b0
```

## decomp-permuter

[decomp-permuter](https://github.com/simonlindholm/decomp-permuter)
brute-forces the *exact-codegen* tail of matching (register allocation,
instruction ordering) by randomly applying semantics-preserving C
transformations and scoring each compile against the target. It's the
"stop paying hand-iteration time to nail register coloring" tool: if a
candidate already has the right structure and just doesn't line up in
registers, this finishes it for free.

Setup (one-time per clone - `vendor/` is gitignored):
```
git clone https://github.com/simonlindholm/decomp-permuter vendor/decomp-permuter
```
Then the Windows-compat patches below (also gitignored away with the rest of
`vendor/`, so they need reapplying on a fresh clone - see the diffs in this
project's own history, or ask another contributor for a patch file):
- `src/compiler.py`: route `compile.sh` through `bash` on Windows; when a
  `cc.txt` sidecar exists (written by `import_func.py`), call `mwccarm.exe`
  directly instead (skips a git-bash spawn per candidate, several times
  faster).
- `src/main.py`: skip the Unix executable-bit check on Windows (NTFS doesn't
  track it).
- `src/preprocess.py`: fall back to the in-process `pcpp` preprocessor when
  no external `cpp` binary is on PATH.
- `src/objdump.py` (`get_arch`): default to ARM32 when the file isn't a valid
  ELF (this project's `target.o` is a raw ROM byte slice, not an object file).

Per-function workflow:
```
python tools/permuter/import_func.py --module unk_autoload_0 --addr 0x02320a64 --base draft.c
python tools/permuter/winproc.py --secs 300 -- python vendor/decomp-permuter/permuter.py vendor/decomp-permuter/work/<name> --stop-on-zero -j 4
```
`import_func.py` resolves the function from `tools/funcs.py` (not a
`config/relocs.txt` symbol table, unlike sm64ds-decomp), writes the per-
function compile flags (module + ARM/Thumb aware - this project mixes both),
and wires `tools/permuter/cap_objdump.py` (a capstone-based objdump
replacement, so no external `arm-none-eabi-objdump` install is needed) as the
scorer's disassembler.

### Always run it through `tools/permuter/winproc.py`, never raw `timeout`

`-j 4` spawns multiprocessing workers, each spawning its own
`cap_objdump.py` -> `mwccarm.exe` subprocess. On Windows, killing a parent
process does **not** kill its children - neither a shell `timeout N cmd`
wrapper nor plain `subprocess.run(cmd, timeout=N)` reaches past the direct
child, so the permuter's own worker processes (and *their* `mwccarm.exe`
children) get orphaned and keep running indefinitely once the deadline
passes. This is exactly what happened repeatedly in early sessions - `python.exe`
and `mwccarm.exe` piling up long after a permuter run was reported "done".

`tools/permuter/winproc.py` fixes this properly instead of relying on
anyone remembering to clean up by hand: it assigns the launched process to a
Windows Job Object with `JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE`, so terminating
the job (on timeout, on success, or on any exception) recursively kills the
*entire* tree - direct child, its multiprocessing workers, and their
subprocesses - in one kernel call, no cooperation required from any of them.
No extra dependency (pure `ctypes` against `kernel32`).

- CLI, for interactive/ad-hoc runs: `python tools/permuter/winproc.py --secs N -- <command...>`.
- From Python (this is what `tools/permuter/batch.py` uses internally):
  `winproc.run_bounded(cmd, secs=N, **popen_kwargs)`.

Verified empirically: the same active `-j 4` workload left 5 orphaned
`python.exe` processes running after a plain `subprocess.run(timeout=15)`
call returned, versus zero with `winproc.run_bounded`. Never invoke
`permuter.py` (directly or via `batch.py`) through a raw shell `timeout` or
bare `subprocess` timeout again - always through `winproc`.

For a pile of near-misses at once (drafts that compile but don't byte-match -
often only coloring/ordering is off), use `tools/permuter/batch.py --seeds
nearmiss.jsonl`. A score-0 result is always independently re-verified with
`tools/match.py`'s own oracle before being banked - nothing is trusted on the
permuter's say-so.

## tools/probe_versions.py

Compiles a probe C file (`tools/probes/discriminate.c`, a spread of
constructs where CodeWarrior point-releases tend to diverge) with every
candidate `mwccarm` version and reports which functions produce different
bytes across versions. This is how you'd actually nail down the compiler pin
(`dsi/1.3` is currently just a copyright-date guess - see
[setup-mwccarm.md](setup-mwccarm.md)) instead of hoping every match keeps
landing on it by luck. So far the whole `dsi/` family is codegen-identical
for every construct in the default probe file - feed it something more
unusual if you want to actually narrow the pin down.

## tools/fdiff.py

Like `tools/match.py` but always verbose: a per-instruction table showing
target vs. candidate, with a note on which mismatches are "same shape,
different register" (regalloc/coloring - what the permuter fixes) versus a
real structural difference (what you need to fix by hand). Use this instead
of `match.py`'s plain pass/fail once you're iterating on something that's
close but not there yet.

## tools/nonmatching.py

The park hatch for a function that's logic-correct (compiles, and the oracle
divergence is small) but can't be made byte-exact at this compiler tier - a
real codegen floor, not "haven't figured it out." Discipline: only use this
when you're actually stuck on the *compiler*, not the logic; a fresh target
you haven't cracked yet belongs in the normal matching loop, not here.

## tools/progress.py

Regenerates the README's progress bar. Matched-function *count* is always
computable from committed `src/` alone; the total/percentage needs a local
`extracted/pictochat_funcs.json` (the Ghidra function cache), since this
project's `config/` never finished generating (unlike sm64ds-decomp, whose
`progress.py` can run from committed data alone with no ROM - see the file's
own docstring for why that's not true here yet).
