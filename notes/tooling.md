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
3. **`tools/templates.py --apply --extend 0x10`** - free matches from trivial
   leaf shapes (empty stubs, constant/argument returns, single-field get/set,
   two-arg arithmetic, simple global getters) - no hand-writing needed, every
   candidate is oracle-verified before banking. Cheap to re-run periodically
   as more functions get discovered. Always pass `--extend` (see its section
   below) - a one-off run without it missed 31 real matches this session,
   purely because Ghidra's cached size cut off before each one's trailing
   literal pool.
3b. **`tools/find_duplicates.py --apply`** - free matches for any function
   byte-identical to an already-matched one (real example: two functions
   this session turned out to be exact copies of each other). Cheap, worth
   running after every batch of new matches, not just once.
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

**`--extend N`**: also retries every rule against windows up to N bytes
*larger* than Ghidra's cached size, reading the extra bytes straight from
the ROM (`rom_bytes()`) rather than trusting `extracted/pictochat_funcs.json`
verbatim. This exists because of a real, recurring bug: Ghidra's function
boundary is code-flow-derived, not from a real linker symbol table (this
ROM has none), so for a leaf like `rule_pool_const`'s `ldr r0,[pc,#0]; bx
lr; .word G` shape, Ghidra sometimes draws the boundary right at `bx lr`
and silently excludes the trailing pool word - the rule then never fires
because the cached bytes are too short to see the `.word`. One `--extend
0x10` pass across the whole project found and banked **31 previously-
invisible matches** this way (mostly `global_getter`/`global_setter`/
`global_store_const`), on top of `templates.py`'s normal count. Cheap to
re-run periodically (same as the base scan), and safe: every hit is still
compiled and byte-diffed at whatever size actually matched (which may
differ from `extracted/pictochat_funcs.json` and is what gets banked -
`main()`'s hit records carry the corrected size, not the cached one)
before being reported, so a rule misfiring at some extended size just
means no candidate at that size, same as the base scan.

Don't confuse this with the OTHER Ghidra-undercounting failure mode this
project has hit repeatedly on hand-matched (non-template) functions - a
trailing pool AFTER a `bx lr`/`pop`, or case bodies past an unresolved
jump-table dispatch (see "Ghidra's function size can exclude a trailing
literal pool" below). `--extend` only helps the *template* tier, where a
byte-pattern rule can cheaply re-check a wider window; for a hand-drafted
candidate hitting the same root cause, find the true size manually the
way that section describes.

### tools/find_duplicates.py

Groups every Ghidra-known function by raw byte content - a function
compiled from a genuinely identical body anywhere else in the ROM is byte-
identical to it (real example: `FUN_0232df74` turned out to be an exact
copy of `FUN_0232df40`, banked for zero extra work). A group with an
already-matched member makes every other member free: same source, new
`// decomp:` marker, still re-verified through the normal compile+compare
oracle before banking (`--apply` does this automatically; dry-run just
reports).

**Guards against a sharp false-positive edge**: the exact same Ghidra
undercounting bug that motivates `templates.py --extend` also **creates
fake duplicate groups**. A pool-const getter's cached bytes are just `ldr
r0,[pc,#0]; bx lr` with the differentiating pool word truncated off - so
ten completely different global-pointer getters, each returning a
different global, all look byte-identical by that truncated cache. Banking
one as the "seed" for the rest would have produced nine plausible-looking
but silently wrong matches (`tools/match.py` would only ever check the 4
cached bytes, never the pool word that's the actual point of the
function). `false_duplicate()` catches this by reading `size + 16` bytes
straight from the ROM for every group member before trusting the group -
a real duplicate stays identical there too; a false one diverges
immediately. Concretely: of 30 raw "duplicate" groups found this session,
**25 were false positives** from exactly this bug - only 5 were real.

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

## tools/match.py's relocation wildcarding had an alignment bug

Fixed this session: `extract_func()` marked a relocation's compare-word as
`o & ~3` (round the reloc's byte offset down to the enclosing 4-byte word)
and wildcarded only that one word. That's wrong whenever the relocation
itself doesn't start on a 4-byte boundary - a Thumb `bl`/`R_ARM_THM_CALL`
relocation is 4 bytes wide but can legitimately start at a 2-mod-4 offset
inside a function (there's no requirement that call instructions land on
word boundaries), in which case its second half spills into the *next*
4-byte word, which was real-compared instead of wildcarded. Concretely: a
function calling the same helper three times back-to-back can have its
third `bl` start 2 bytes into a word - that call's true target-address
encoding was being byte-compared against a placeholder, guaranteeing a
spurious 1-word mismatch no matter how correct the C was. `extract_func()`
now adds both `o & ~3` and `(o+3) & ~3` per relocation, covering both
words a straddling reloc can touch.

This can only have caused **false rejections**, never false acceptances -
wildcarding too little makes the check stricter, not more lenient, so
nothing already banked is affected. But it means some function you tried
earlier and set aside as "close, 1 word off, couldn't figure out why"
might have actually been correct - worth a second look with `--all` (or
even just re-running the same candidate) now that this is fixed, especially
for anything that calls the same helper multiple times or otherwise has a
`bl` landing at an odd position.

## tools/fdiff.py

Like `tools/match.py` but always verbose: a per-instruction table showing
target vs. candidate, with a note on which mismatches are "same shape,
different register" (regalloc/coloring - what the permuter fixes) versus a
real structural difference (what you need to fix by hand). Use this instead
of `match.py`'s plain pass/fail once you're iterating on something that's
close but not there yet.

## The `pop {...,pc}` fold puzzle

A recurring near-miss shape: a candidate matches target instruction-for-
instruction (same operations, same order, sometimes even the same byte
count) except the target's last instruction is a folded `pop {r4-r7,pc}`
while the candidate always emits a separate `pop {r4-r7}` + `bx lr`. This
isn't a source-structure issue - synthetic single-return, zero-branch test
functions reproduce it too - and it isn't rare: of the functions matched via
`tools/templates.py` and the manual loop so far, **none** compile to the
fold form under this project's pinned flags
(`-O4,p` = `-opt level=4 -opt speed`; check with `mwccarm -help opt=O4`).

Switching to `-O4,s` (`-opt space`) reliably produces the fold in synthetic
tests, and re-validating all currently-matched functions under `-O4,s`
instead of `-O4,p` leaves the great majority still matching byte-identical
(as of the 54-function checkpoint, 51/54 - the 3 regressions were unrelated
trivial-template/anomaly cases). So `-O4,s` is a real, safe-to-try lever,
**not** a free fix: on two real fold-blocked near-misses
(`FUN_02323d44`, `FUN_0232dbd0`) it changed *other* codegen choices too
(register allocation, prologue shape, or in one case introduced an
unrelated fixed 6-byte NOP trailer present at every `-O1`-`-O4,s` level
alike, so unrelated to the fold at all) rather than landing a byte-exact
match outright. Treat an `-O4,s` recompile as a fresh iteration starting
point for a fold-blocked candidate - re-derive the exact source shape
against the new flag's behavior rather than expecting the `-O4,p`-tuned
source to carry over unchanged.

Until a specific function's fold is actually cracked, the cheap workaround
is avoidance: before drafting a candidate, check the target's own real
tail bytes (`tools/disasm.py`) and prefer functions that already end in
separate `pop`+`bx lr` (the vast majority) over ones ending in the fold.

## Ghidra's function size can exclude a trailing literal pool

Second recurring near-miss shape: a candidate compiles to *more* bytes than
the target even though every real instruction matches. Ghidra's cached
function boundary (what `tools/funcs.py`'s `f["size"]` reports) is based on
code-flow analysis, not a linker symbol table (the shipped binary has none)
- so for a function whose `ldr rX,[pc,#N]` pool constants live physically
after its last `bx lr`/`pop`, Ghidra sometimes draws the boundary right at
the return and silently excludes the pool. `FUN_02000e78.c` hit this first
(its comment documents a 48-byte trailing pool past a 184-byte Ghidra
boundary - true size 232, not 184). `FUN_023226d4` hit it again this
session: Ghidra said 46 bytes, but `tools/disasm.py` on the bytes just past
that boundary showed the exact same alignment-NOP + pool-word pattern an
isolated single-function compile of the same source produces, confirming
the real function is 56 bytes (0x38), not 46 (0x2e).

If a candidate is consistently N bytes *too long* and every instruction up
to the target's declared end already matches, don't assume the source is
wrong - check `tools/disasm.py --addr <target_end> --length 0x10` first.
If it decodes as an alignment NOP followed by recognizable pool
constant(s), use the corrected size (round up to include them) as `--size`
instead of trusting the Ghidra cache verbatim.

For a run of tiny sibling functions packed back-to-back (e.g. several
`arg & G[N]`-style one-liners at consecutive addresses a few bytes apart),
the trailing pool word Ghidra excluded from function A can be sitting
*inside* what looks like function B's own address range - i.e. two
adjacent undercounted functions can appear to overlap until both are
corrected. Don't let that overlap read as a sign something's wrong; just
correct each one's size independently from its own `bx lr`/`pop` forward,
the same way as any other instance of this bug.

The same undercounting hits switch/jump-table functions too, for a
different reason: Ghidra's code-flow analysis can't always statically
resolve an indirect `add pc,rX` dispatch, so it stops the function right
at the table instead of including the case bodies and default handler
past it. `FUN_02329270` and `FUN_02329a08` both hit this. **Don't assume
the true boundary is wherever the next function's first real instruction
starts, though** - `FUN_02329a08`'s cached size (0x2a) undercounted by
more than just the missing case bodies; the correct boundary (0x32) sits
2 bytes *before* the next function's own first instruction (0x34), with
one inter-function alignment NOP in between that belongs to neither
function's own compiled output. Confirm the exact boundary empirically:
try the size that ends right after the last real `bx lr`/`pop` first (a
clean, zero-mismatch `tools/match.py` run confirms it), before assuming
you need to include trailing bytes up to the next function.

## A few more codegen idioms worth trying before you conclude "stuck"

Found while hand-matching `unk_autoload_0` 0x02328000-0x02330000:

- **`x &= ~C` for a small constant `C`**: if the compiler is pool-loading a
  full-width mask and ANDing instead of emitting a `BICS`, try keeping the
  intermediate value as a plain `int` (not `unsigned short`/other narrow
  type) and writing the result straight back through the pointer, rather
  than caching it in a local first. This nudged mwccarm into the shorter
  `BICS`-based form that matches this kind of clear-some-bits target.
- **Register-pairing swaps in loop/pointer-walk shapes**: if two variables
  come out in the opposite registers from target (a pure recolor, same
  instruction shape otherwise), try declaring the plain scalar(s) (loop
  counters, indices) *before* the pointer variable in source order. Fixed
  one real case this way - but isn't universal, a sibling case with the
  same symptom didn't respond to it, so treat it as one more thing to try,
  not a rule.
- **A pointer register reused via cheap arithmetic to materialize an
  unrelated second constant**: e.g. having just computed `base+0x50` for
  one purpose, the compiler notices `base+0x50` is *also* useful nearby
  and re-derives it with a cheap `add r4,#0x50` instead of a fresh pool
  load, even where the two uses aren't obviously related in the source.
  This is a genuine optimizer behavior (CSE/rematerialization across
  supposedly-unrelated expressions), not something source phrasing
  reliably suppresses - if you hit it, it's likely a real, if frustrating,
  compiler floor rather than a bug in your candidate.

## Inline asm is not a shortcut for ordinary application logic

It's tempting, once a candidate is byte-for-byte identical to the target
except for one compiler-scheduling choice (an instruction order a dozen
plain-C rephrasings won't reproduce), to just hand-write the exact target
instruction sequence as inline asm and call it matched. **Don't, for
normal application code.** It technically passes `tools/match.py`, but it
isn't decompilation - it's re-encoding the same bytes in a different
syntax, and defeats the actual point of this project: source a human can
read as an honest account of what the function does. This was tried and
reverted for two ordinary functions (an all-zero-word check, a linked-list
append) this session - both got stuck on a two-or-three-instruction
scheduling swap that no statement reordering reproduced, and asm was
reached for instead of pushing further on the C or parking the function.

`src/arm9/FUN_02000e78.c` is **not** a precedent for this - it's a
different category. That function's "logic" is a fixed CP15/cache/MPU
register-programming sequence where the instruction order essentially *is*
the specification (mirrors sm64ds-decomp's own hand-asm `SystemSetup`
equivalent); there's no higher-level C shape being discarded by writing it
as asm, because there isn't a meaningfully different one. A linked-list
insert or a loop, by contrast, has a real C shape, and if the compiler's
scheduler won't reproduce the target's exact ordering, the right moves are
(in order): try more source-level rephrasings, try decomp-permuter, try
other flags/optimization levels, and only if genuinely stuck, park the
function with `tools/nonmatching.py` rather than banking asm as if it were
a match.

If a genuine hardware/runtime-primitive case like `FUN_02000e78.c` does
come up again, two mwccarm inline-asm gotchas are worth knowing up front:

- mwccarm's inline assembler wants **bare** mnemonics even for Thumb1 forms
  that inherently set flags and disassemble with an implicit `s` - write
  `mov`/`add`/`sub`/`orr`/`asr`/... not `movs`/`adds`/`subs`/`orrs`/`asrs`/...
  The encoding is identical either way (Thumb1 `movs` and ARM-syntax `mov`
  in this context assemble to the same bits); the `s` form is simply not
  accepted as *input* syntax by this assembler, only produced by the
  disassembler reading it back. Numeric local labels (`1f`/`1b`/`0:`) also
  aren't accepted - use named labels (`Lbody:`, `b Lbody`) instead.
- `tools/match.py`'s CLI auto-strips `-thumb` from the compile flags
  whenever the source text contains the literal word `asm` (see
  `main()`'s `-thumb`-stripping block) - a heuristic for the CP15-style
  ARM-mode asm functions like `FUN_02000e78.c`, where `-thumb` would make
  mwccarm reject coprocessor instructions. That heuristic is wrong for a
  Thumb-mode function whose inline asm is itself genuinely Thumb code -
  compiling it in ARM mode instead just produces confusing "illegal
  operand" errors. Pass `--flags "..."` explicitly (copy `DEFAULT_FLAGS`
  from `tools/match.py` verbatim, keeping `-thumb`) to override the
  heuristic for this case.

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
