# Getting your own `assets/pictochat.nds`

This project ships no ROM and no Nintendo assets - see the "Legal and scope"
section of [README.md](../README.md). You need to dump `pictochat.nds`
yourself, from a console you own, before any of the tooling here can run.

## What you need

**A Nintendo DSi or DSi XL that you own.** Not a DS, DS Lite, 2DS, or 3DS -
the version of PictoChat this project targets is specifically the **DSi
system title**, factory-installed on every retail DSi/DSi XL as one of the
built-in DSiWare-style apps. The original DS/DS Lite's PictoChat was never a
cartridge product either - it was built into the DS's own system firmware
(see [firmware-investigation.md](firmware-investigation.md) for this
project's earlier attempt at that binary specifically) - but it's still a
different build from the DSi system title targeted here, and out of scope
for this doc. If you own a DSi, you already have a licensed copy of the DSi
title sitting in its NAND; nothing to buy or download.

## Dumping it

The general, well-established method the DS/DSi homebrew and preservation
community uses to back up your own console's installed titles:

1. Get homebrew running on your own DSi (a flashcard, or one of the
   browser/launcher exploits for an unpatched system - which route is
   applicable depends on your DSi's firmware version; search current
   homebrew guides for your specific version, since exploit availability
   changes over time and isn't this doc's concern).
2. Use a NAND/title dumping tool such as **GodMode9i** (the DSi title
   manager, modeled on 3DS's GodMode9) to browse your DSi's installed system
   titles and export the PictoChat title to your SD card as a decrypted
   `.app` file. Any equivalent, actively-maintained DSi title-dump homebrew
   works the same way - the exact menu path varies by tool version, so
   follow that tool's own current documentation.
3. Copy the exported file off the SD card, rename it to `pictochat.nds`, and
   place it at `assets/pictochat.nds` in this repo (the folder is
   git-ignored - it never gets committed).

You now have exactly the file `tools/extract_pictochat.py` and
`tools/match.py` expect. See [CONTRIBUTING.md](../CONTRIBUTING.md) for what
to run next.

## Why this is the only acceptable source

Downloading someone else's dump (from an archive site, a forum, a torrent,
etc.) is redistribution of Nintendo's copyrighted software, not a personal
backup, and archive sites hosting ROM dumps aren't a legal source just
because they're online - Nintendo actively pursues takedowns of exactly this
material. It's also unverifiable: this project's matching standard depends on
comparing against *your* real binary, and a dump from an unknown source could
be patched, corrupted, or simply mislabeled. Dumping your own title from your
own hardware is the only way this project's legal stance (and its
correctness guarantee) actually holds.

## One more file you'll need

`pictochat.nds` alone isn't quite enough to get the real, correctly-laid-out
extraction (see [pictochat-layout.md](pictochat-layout.md)) - you also need
your own **ARM7 BIOS dump** (`assets/bios7.bin`), used for `dsd`'s
`--arm7-bios` flag and to reconstruct this digital title's KEY1 secure-area
placeholder. See [CONTRIBUTING.md](../CONTRIBUTING.md) for the full list of
what's required versus what's only needed for the separate firmware
investigation.
