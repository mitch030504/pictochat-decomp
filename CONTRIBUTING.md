# Contributing to ndsDecomp

This is a from-scratch **matching** decompilation of PictoChat: every
function landed is hand-written C that compiles to bytes **identical** to the
real binary.

## What you need

You bring your own dumps. Nothing copyrighted lives in this repo.

1. **Your own firmware dump** (`assets/firmware.bin`) and **BIOS dumps**
   (`assets/bios7.bin`, `assets/bios9.bin`) - needed for the KEY1 tooling if
   you touch the firmware investigation side.
2. **Your own copy of the PictoChat title** (`assets/pictochat.nds`) - the
   actual decomp target.
3. **mwccarm** - already checked out under `tools/mwccarm/` on this machine;
   see [notes/setup-mwccarm.md](notes/setup-mwccarm.md) if you need to set it
   up fresh elsewhere.
4. **dsd** (the ds-decomp toolkit): already fetched to `tools/bin/dsd.exe`
   (gitignored - re-download from
   https://github.com/AetiasHax/ds-decomp/releases if missing).
5. **Python 3** plus:
   ```
   pip install ndspy capstone pyelftools pyghidra
   ```

## First-time setup

```
pip install ndspy capstone pyelftools pyghidra
python tools/extract_pictochat.py   # -> populates extracted/ (git-ignored)
```

## Check for a free match first

Before hand-writing anything, run the free-match tools - between
cross-project matching and the template tier, they've already landed 44 of
this project's matches with no hand-writing at all:

```
python tools/sync_ledger.py --apply    # catch your local ledger up to committed src/ first
python tools/cross_match.py --other-repo ../sm64ds-decomp   # free matches from another decomp project
python tools/templates.py --apply      # free matches from trivial leaf shapes (self-verified)
```

See [notes/tooling.md](notes/tooling.md) for what each tool does and
[notes/cross-project-matching.md](notes/cross-project-matching.md) for how
`cross_match.py` works.

## The matching loop

1. **Pick an unmatched function.** Read [notes/pictochat-layout.md](notes/pictochat-layout.md)
   first - almost everything interesting lives in the `unk_autoload_0`
   module at `0x02320000`, not the tiny `main` crt0 stub at `0x02000400`.
   If you're about to spend real time on it, say so in a draft PR or issue
   first - see [CLAIMS.md](CLAIMS.md).
2. **Read it** - a few options, roughly in order of how much the function
   deserves:
   ```
   python tools/disasm.py --module unk_autoload_0 --addr 0x... --length 0x... --thumb
   python tools/m2c_draft.py --module unk_autoload_0 --addr 0x...   # semantic C draft (see notes/tooling.md)
   ```
   or open `ghidra_project/` (see [notes/ghidra-setup.md](notes/ghidra-setup.md))
   and read the decompiler's output as a *reading aid* - neither this nor
   `m2c_draft.py`'s output matches on its own; both are for understanding the
   function well enough to hand-write it.
3. **Write C** for it (a draft is fine to start).
4. **Compile + byte-diff**, relocation-aware:
   ```
   python tools/match.py --c yourfile.c --func name --addr 0x0232... --size 0x.. \
       --module unk_autoload_0 --version dsi/1.3
   ```
   A match means every instruction word and every relocation slot lines up.
   If it's close but not quite there, `tools/fdiff.py` always shows the full
   per-instruction diff and flags which mismatches are same-shape/different-
   register (permuter-fixable) versus a real logic difference. If a
   near-right candidate's only problem is register coloring or instruction
   order, decomp-permuter finishes it for free - see notes/tooling.md. The
   compiler version isn't confirmed yet for this title - see
   [notes/setup-mwccarm.md](notes/setup-mwccarm.md) - so if `dsi/1.3` never
   quite lines up on an otherwise-correct-looking function, try `--trio` to
   sweep the rest of the `dsi/` builds, or `--all` for everything including
   the (less likely) NTR-era versions. `tools/probe_versions.py` can help pin
   the real version if you find a construct that discriminates them.
5. **Promote** the matched C via `tools/ledger.py` (or just write it to
   `src/arm9/`/`src/arm7/` by hand, following notes/matching-style.md - the
   ledger's `bank()` isn't required, just recommended so
   `tools/progress.py`/`tools/sync_ledger.py` see it cleanly).
6. **If it's logic-correct but genuinely can't be made byte-exact** at this
   compiler tier, park it with `tools/nonmatching.py add` instead of leaving
   it in limbo - see notes/tooling.md.

See [notes/matching-style.md](notes/matching-style.md) for the full
conventions (one function per file, naming, "import knowledge write code").

## Ground rules

- **Never commit copyrighted material.** No firmware dump, no ROM, no
  `pictochat.nds`, no extracted assets, no `mwccarm`. `.gitignore` already
  enforces this - don't override it.
- **Import knowledge, write code.** You may use publicly known symbol names
  or struct/field offsets, but all C in `src/` must be hand-written from
  scratch against your own extracted binary.
- **Match to the byte.** A function counts only when its compiled bytes equal
  the real binary's.
- **Stay on one toolchain per matched function.** Note which `mwccarm`
  version you actually matched with (most functions should land on whatever
  gets pinned as canonical - see the README).

## If you pick the firmware investigation back up instead

That's real, valuable, unsolved work (Part 3's compression format) - see
[notes/firmware-investigation.md](notes/firmware-investigation.md) for
exactly how far it got and what the productive next step is (dynamic
analysis, not more static tracing). It's a separate track from the PictoChat
matching loop above and doesn't block it.
