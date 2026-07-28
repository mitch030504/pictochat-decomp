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
4. **Pick a real target** and read it: `tools/disasm.py` for a quick look, or
   `ghidra_project/` for the decompiler view (a *reading* aid, not a matching
   one). For anything past a few instructions, **`tools/m2c_draft.py`** gives
   a real semantic C draft (control flow, callee/global references) to start
   hand-writing a candidate from - see the m2c section below.
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

A trimmed port of sm64ds-decomp's `swarm.py` template-rule engine - just the
leaf rules (empty/constant/argument-return, single-field load/store, two-arg
arithmetic, simple `extern G[]` getters), not the C++-ABI rules (vtable/ctor/
dtor chains), which need a populated `config/relocs.txt` to resolve callee
names that this project doesn't have (see
[pictochat-layout.md](pictochat-layout.md) - `dsd init` never finishes for
this ROM). Mode-aware: tries each rule against the target's own ARM or Thumb
encoding (the `mode` field in `extracted/pictochat_funcs.json`), unlike
sm64ds-decomp's ARM-only version. Every proposed candidate is compiled and
byte-diffed before being reported, so a rule misfiring just means "no
candidate," never a wrong one.

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
python vendor/decomp-permuter/permuter.py vendor/decomp-permuter/work/<name> --stop-on-zero -j 4
```
`import_func.py` resolves the function from `tools/funcs.py` (not a
`config/relocs.txt` symbol table, unlike sm64ds-decomp), writes the per-
function compile flags (module + ARM/Thumb aware - this project mixes both),
and wires `tools/permuter/cap_objdump.py` (a capstone-based objdump
replacement, so no external `arm-none-eabi-objdump` install is needed) as the
scorer's disassembler.

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
