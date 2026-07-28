# Claiming functions

If you're working the matching loop (see [CONTRIBUTING.md](CONTRIBUTING.md)),
say so before you sink time into a function, so two contributors don't
duplicate work on the same one.

sm64ds-decomp (whose tooling this project's is adapted from) coordinates this
through a small hosted lock service they run. This project doesn't have that
infrastructure, so coordination here is manual instead:

- **Open a draft PR early**, even before you're done, once you've picked a
  function or a batch. The PR title should name the function(s)/address
  range. This is the claim.
- **Or open an issue** naming the function/module/address range you're
  starting on, if you're not ready to push code yet.
- Before starting, skim open PRs/issues for a range that overlaps yours.
- A claim goes stale after a few days of inactivity - feel free to pick it up
  if the PR/issue has gone quiet and you ping first.

## Before you claim anything

Run the free-match tools first - they might already cover it:

```
python tools/sync_ledger.py --apply    # catch your local ledger up to committed src/
python tools/templates.py --apply      # free template-tier matches (leaf shapes)
python tools/cross_match.py --other-repo ../sm64ds-decomp   # free cross-project matches
```

See [CONTRIBUTING.md](CONTRIBUTING.md) for the full matching loop and
[notes/tooling.md](notes/tooling.md) for what each tool does.

## Batches

If you're running `tools/templates.py --apply` or
`tools/permuter/batch.py --seeds ...` over a wide range, mention the module
and size range in your PR - these can land dozens of functions at once, and
someone else hand-matching in that same range wants to know before they
start.
