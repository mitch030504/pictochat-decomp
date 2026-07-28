#!/usr/bin/env python3
"""Nintendo DS Firmware Extractor

Parses assets/firmware.bin per GBATEK's "DS Firmware Header" layout.

Part 1 (ARM9 boot code) and Part 2 (ARM7 boot code) are KEY1-encrypted (see
tools/key1.py) then LZ77-compressed; this is fully solved and verified -
decrypting+decompressing them produces real, recognizable ARM9/ARM7 crt0 code
(IME disable at 0x04000208, mode/stack setup, MMU init). Both are extracted to
real decompressed binaries here.

Part 3 (ARM9 GUI code, includes PictoChat), Part 4 (ARM7 WiFi driver), and
Part 5 (data/gfx) are NOT KEY1-encrypted with the same key (verified: does
not decrypt to a valid header) and are documented by GBATEK as "LZ/Huffman
compression", but neither plain SWI-tagged LZ77 nor Huffman headers appear at
their (header-verified-correct) offsets. Their real format is still
unconfirmed - dumped raw here for further analysis rather than guessed at.
"""

import os
import struct
import hashlib

import key1

FIRMWARE_PATH = os.path.join("assets", "firmware.bin")
BIOS7_PATH = os.path.join("assets", "bios7.bin")
EXTRACT_DIR = "extracted"


def parse_header(fw):
    part3_raw, part4_raw, part34_crc, part12_crc = struct.unpack_from("<HHHH", fw, 0x00)
    header_id = fw[0x08:0x0C]
    part1_rom_raw, part1_ram_raw, part2_rom_raw, part2_ram_raw = struct.unpack_from("<HHHH", fw, 0x0C)
    shift = struct.unpack_from("<H", fw, 0x14)[0]
    part5_raw = struct.unpack_from("<H", fw, 0x16)[0]
    user_settings_raw = struct.unpack_from("<H", fw, 0x20)[0]

    shift1 = shift & 0x7
    shift2 = (shift >> 3) & 0x7
    shift3 = (shift >> 6) & 0x7
    shift4 = (shift >> 9) & 0x7
    chipsize_128k_units = (shift >> 12) & 0xF

    return {
        "header_id": header_id,
        "idcode": struct.unpack_from("<I", fw, 0x08)[0],
        "part1_off": part1_rom_raw * (2 ** (2 + shift1)),
        "part2_off": part2_rom_raw * (2 ** (2 + shift3)),
        "part3_off": part3_raw * 8,
        "part4_off": part4_raw * 8,
        "part5_off": part5_raw * 8,
        "user_settings_off": user_settings_raw * 8,
        "chipsize_bytes": chipsize_128k_units * 128 * 1024,
        # RAM load addresses per GBATEK: "2800000h-ramaddr" / "3810000h-ramaddr"
        "part1_ram": 0x2800000 - part1_ram_raw * (2 ** (2 + shift2)),
        "part2_ram": 0x3810000 - part2_ram_raw * (2 ** (2 + shift4)),
    }


def main():
    if not os.path.exists(FIRMWARE_PATH):
        raise FileNotFoundError(f"Firmware image not found at {FIRMWARE_PATH}")
    if not os.path.exists(BIOS7_PATH):
        raise FileNotFoundError(f"ARM7 BIOS not found at {BIOS7_PATH} (needed for KEY1 key table)")

    fw = open(FIRMWARE_PATH, "rb").read()
    size = len(fw)
    print(f"[+] Loaded firmware image ({size / 1024:.0f} KB)")
    print(f"    SHA256: {hashlib.sha256(fw).hexdigest()}")
    os.makedirs(EXTRACT_DIR, exist_ok=True)

    h = parse_header(fw)
    print(f"[+] Header ID: {h['header_id']!r}")
    print(f"    Chip size (from shift field): {h['chipsize_bytes'] / 1024:.0f} KB "
          f"({'MATCHES' if h['chipsize_bytes'] == size else 'MISMATCH vs'} actual file size)")

    with open(os.path.join(EXTRACT_DIR, "header.bin"), "wb") as f:
        f.write(fw[:0x200])

    us_off = h["user_settings_off"]
    with open(os.path.join(EXTRACT_DIR, "user_settings.bin"), "wb") as f:
        f.write(fw[us_off:us_off + 0x200])
    print(f"[+] Extracted User Settings (0x{us_off:X}-0x{us_off + 0x200:X})")

    # Part 1 / Part 2: KEY1-encrypted + LZ77-compressed, per GBATEK's
    # firmware_decryption pseudocode. init_keycode(idcode, level=2, mod=0x0C).
    keybuf = key1.init_keycode(BIOS7_PATH, h["idcode"], 2, 0x0C)
    for part, label, out_name in (("part1_off", "Part 1 (ARM9 boot)", "part1_arm9_boot.bin"),
                                   ("part2_off", "Part 2 (ARM7 boot)", "part2_arm7_boot.bin")):
        off = h[part]
        try:
            decompressed, consumed = key1.lz77_decompress_encrypted(fw, off, keybuf)
        except ValueError as e:
            print(f"[-] {label} @ 0x{off:X}: decrypt+decompress FAILED: {e}")
            continue
        out_path = os.path.join(EXTRACT_DIR, out_name)
        with open(out_path, "wb") as f:
            f.write(decompressed)
        ram_key = "part1_ram" if part == "part1_off" else "part2_ram"
        print(f"[+] {label} @ 0x{off:X}: decrypted+decompressed {len(decompressed)} bytes "
              f"(consumed {consumed} encrypted bytes), RAM load addr 0x{h[ram_key]:X} -> {out_path}")

    # Parts 3/4/5: format not yet confirmed (see module docstring). Dump raw
    # bytes from the header-verified-correct offset for further analysis.
    for part, label in (("part3_off", "Part 3 (ARM9 GUI code, includes PictoChat)"),
                         ("part4_off", "Part 4 (ARM7 WiFi code)"),
                         ("part5_off", "Part 5 (data/gfx)")):
        off = h[part]
        raw_name = f"{part.replace('_off', '')}_raw.bin"
        with open(os.path.join(EXTRACT_DIR, raw_name), "wb") as f:
            f.write(fw[off:])
        print(f"[+] {label}: offset 0x{off:X} (format unconfirmed) -> raw dump {raw_name}")

    print("\n[+] Extraction complete. Part 1/2 are real decompiled boot code. "
          "Part 3 (PictoChat's home) is still an open problem - see tools/key1.py "
          "and notes in extract_firmware.py's docstring.")


if __name__ == "__main__":
    main()
