#!/usr/bin/env python3
"""Extract the standalone PictoChat DSi title (assets/pictochat.nds).

Unlike the original DS firmware's Part 3 (still-unsolved custom KEY1+staging-
buffer loading - see notes/firmware-investigation.md), the DSi shipped
PictoChat as an ordinary NDS binary: same container format sm64ds-decomp's
tools/unpack.py handles, no runtime encryption, no custom compression. This
is the primary path to real PictoChat source.

Two extractions happen here:

1. A quick, flat ndspy extraction (arm9.bin/arm7.bin as single blobs). Good
   enough to confirm authenticity, wrong for anything past the first ~3.2KB
   of ARM9 - see notes/pictochat-layout.md for why.
2. The real one, via dsd (tools/bin/dsd.exe), which understands PictoChat's
   actual multi-segment ARM9 layout (main crt0 stub + itcm + the real
   application module + a small extra). This needs two fixups dsd expects
   that this particular dump doesn't have (real cartridge dumps have both;
   this digital title's dump has neither):
     - padding to the header's declared ROM capacity (real dumps keep their
       trailing 0xFF padding; this one had it stripped)
     - a validly KEY1-encrypted ARM9 "secure area" (real cartridges encrypt
       the first 2KB of ARM9, decrypting to a leading "encryObj" marker,
       which both real hardware and dsd check for; this dump has that region
       zeroed instead - no real code is lost, since the actual entry point
       sits exactly at the 2KB boundary, but dsd still needs a validly
       encrypted placeholder there to proceed)

Confirmed genuine: the ARM7 image opens with the identical crt0 prologue
found in the firmware's own decrypted boot code, and ARM9 contains readable
PictoChat UI strings ("Auto Mode", "Manual Mode", "Copy current time", etc.)
in eight languages.
"""
import os
import struct
import subprocess
import sys

import ndspy.rom
import ndspy.codeCompression as codeComp

sys.path.insert(0, os.path.dirname(__file__))
import key1  # noqa: E402

ROM_PATH = os.path.join("assets", "pictochat.nds")
BIOS7_PATH = os.path.join("assets", "bios7.bin")
EXTRACT_DIR = "extracted"
DSD_EXE = os.path.join("tools", "bin", "dsd.exe")
DSD_OUT = os.path.join(EXTRACT_DIR, "dsd")

ROM_DECLARED_CAPACITY_OFFSET = 0x14
GAMECODE_OFFSET = 0x0C
ARM9_ROM_OFFSET_FIELD = 0x20
SECURE_AREA_SIZE = 0x800


def flat_extract():
    if not os.path.exists(ROM_PATH):
        raise FileNotFoundError(f"{ROM_PATH} not found")
    os.makedirs(EXTRACT_DIR, exist_ok=True)

    rom = ndspy.rom.NintendoDSRom.fromFile(ROM_PATH)
    print("=== PictoChat ROM header ===")
    print(f"  Title code : {rom.name!r}")
    print(f"  Game code  : {rom.idCode!r}")
    print(f"  ARM9: {len(rom.arm9):,} bytes, load 0x{rom.arm9RamAddress:08X}, entry 0x{rom.arm9EntryAddress:08X}")
    print(f"  ARM7: {len(rom.arm7):,} bytes, load 0x{rom.arm7RamAddress:08X}, entry 0x{rom.arm7EntryAddress:08X}")

    open(os.path.join(EXTRACT_DIR, "pictochat_arm9.bin"), "wb").write(rom.arm9)
    open(os.path.join(EXTRACT_DIR, "pictochat_arm7.bin"), "wb").write(rom.arm7)

    def maybe_decompress(data, label):
        try:
            out = codeComp.decompress(data)
            if len(out) > len(data):
                print(f"  {label}: BLZ-compressed, {len(data):,} -> {len(out):,} bytes")
                return out
        except Exception:
            pass
        print(f"  {label}: not compressed, {len(data):,} bytes")
        return data

    arm9_dec = maybe_decompress(rom.arm9, "ARM9")
    arm7_dec = maybe_decompress(rom.arm7, "ARM7")
    open(os.path.join(EXTRACT_DIR, "pictochat_arm9_dec.bin"), "wb").write(arm9_dec)
    open(os.path.join(EXTRACT_DIR, "pictochat_arm7_dec.bin"), "wb").write(arm7_dec)
    print(f"\n[+] Flat extraction done -> {EXTRACT_DIR}/pictochat_arm{{9,7}}_dec.bin "
          f"(module layout: see notes/pictochat-layout.md before trusting any address past ~0x800 into ARM9)")


def build_secure_area(rom_bytes: bytes) -> bytes:
    """Reconstruct a validly KEY1-encrypted secure area for a dump whose real
    one was zeroed (see module docstring). Real hardware's decrypt sequence
    double-decrypts block 0 (level-2 keybuf, then level-3) and single-decrypts
    the rest (level-3 only) - confirmed against melonDS's Key1_InitKeycode/
    Key1_Decrypt sequence in NDSCart.cpp. To construct valid ciphertext we
    reverse that exactly, or dsd (and real hardware) will reject it."""
    idcode = struct.unpack_from("<I", rom_bytes, GAMECODE_OFFSET)[0]
    keybuf_l3 = key1.init_keycode(BIOS7_PATH, idcode, 3, 0x08)
    keybuf_l2 = key1.init_keycode(BIOS7_PATH, idcode, 2, 0x08)

    plaintext = b"encryObj" + b"\x00" * (SECURE_AREA_SIZE - 8)

    p0, p4 = struct.unpack_from("<II", plaintext, 0)
    i0, i4 = key1.encrypt_64bit_block(keybuf_l3, p0, p4)
    c0, c4 = key1.encrypt_64bit_block(keybuf_l2, i0, i4)
    out = bytearray(struct.pack("<II", c0, c4))

    for i in range(8, SECURE_AREA_SIZE, 8):
        p0, p4 = struct.unpack_from("<II", plaintext, i)
        e0, e4 = key1.encrypt_64bit_block(keybuf_l3, p0, p4)
        out += struct.pack("<II", e0, e4)
    return bytes(out)


def dsd_extract():
    if not os.path.exists(DSD_EXE):
        print(f"\n[!] {DSD_EXE} not found - skipping dsd extraction (the real, "
              f"multi-segment layout). Download it from "
              f"https://github.com/AetiasHax/ds-decomp/releases to tools/bin/dsd.exe.")
        return
    if not os.path.exists(BIOS7_PATH):
        print(f"\n[!] {BIOS7_PATH} not found - needed for the secure-area patch. Skipping dsd extraction.")
        return

    rom = bytearray(open(ROM_PATH, "rb").read())
    declared_capacity = 128 * 1024 * (2 ** rom[ROM_DECLARED_CAPACITY_OFFSET])
    if len(rom) < declared_capacity:
        print(f"[+] Padding ROM to declared capacity: {len(rom):,} -> {declared_capacity:,} bytes")
        rom += b"\xff" * (declared_capacity - len(rom))

    arm9_rom_off = struct.unpack_from("<I", rom, ARM9_ROM_OFFSET_FIELD)[0]
    secure_area = rom[arm9_rom_off:arm9_rom_off + SECURE_AREA_SIZE]
    if secure_area == b"\x00" * SECURE_AREA_SIZE:
        print("[+] Reconstructing valid KEY1-encrypted secure area (real one is zeroed in this dump)")
        patched = build_secure_area(bytes(rom))
        rom[arm9_rom_off:arm9_rom_off + SECURE_AREA_SIZE] = patched
    elif secure_area[:8] == b"encryObj":
        pass  # already plaintext/decrypted somehow; leave as-is
    else:
        print("[i] Secure area is neither zeroed nor a marker - assuming it's already validly encrypted")

    padded_path = os.path.join(EXTRACT_DIR, "pictochat_padded.nds")
    open(padded_path, "wb").write(rom)

    if os.path.exists(DSD_OUT):
        import shutil
        shutil.rmtree(DSD_OUT)
    r = subprocess.run([DSD_EXE, "rom", "extract", "--rom", padded_path,
                         "--arm7-bios", BIOS7_PATH, "--output-path", DSD_OUT],
                        capture_output=True, text=True)
    print(r.stdout)
    if r.returncode != 0:
        print(f"[!] dsd rom extract failed:\n{r.stderr}")
        return

    # dsd's config.yaml unconditionally references a dtcm section even when
    # this title has none; it just doesn't write the files for it.
    dtcm_bin = os.path.join(DSD_OUT, "arm9", "dtcm.bin")
    dtcm_yaml = os.path.join(DSD_OUT, "arm9", "dtcm.yaml")
    if not os.path.exists(dtcm_bin):
        open(dtcm_bin, "wb").close()
        open(dtcm_yaml, "w").write("base_address: 0\ncode_size: 0\nbss_size: 0\nkind: Dtcm\n")

    print(f"[+] dsd extraction done -> {DSD_OUT}/ (see notes/pictochat-layout.md for the real module layout,\n"
          f"    and notes/ghidra-setup.md for importing it correctly - dsd init does not yet complete for this\n"
          f"    title's unusually tiny main module; Ghidra's own analysis is the function/symbol source of truth)")


def main():
    flat_extract()
    dsd_extract()


if __name__ == "__main__":
    main()
