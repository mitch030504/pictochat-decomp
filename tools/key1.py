#!/usr/bin/env python3
"""NDS KEY1 (Blowfish-based) encryption, used to decrypt the firmware's Part 1
(ARM9 boot code) and Part 2 (ARM7 boot code) before they can be LZ77-decompressed.

Ported directly from GBATEK's literal pseudocode ("DS Encryption by
Gamecode/Idcode (KEY1)"), cross-checked against melonDS's NDSCart.cpp
Key1_Encrypt/Decrypt/ApplyKeycode/InitKeycode/LoadKeyBuf. Verified end-to-end:
decrypting+decompressing firmware Part 1/2 with this produces real ARM9/ARM7
boot code (recognizable crt0: IME disable at 0x04000208, SVC/IRQ/System mode
stack setup, MMU/cache init via mrc/mcr p15).

The key table (0x1048 bytes = 1042 u32 words: 18-entry P-array + four 256-entry
S-boxes) is NOT stored in firmware.bin - it lives in the console's own ARM7
BIOS at offset 0x30, which is why a real bios7.bin dump is required here.

NOTE: only Part 1/2 (the "bootcode") are known to be KEY1-encrypted per
GBATEK's firmware_decryption pseudocode. Part 3/4/5 (GUI/WiFi/data) are NOT
decrypted by this same key (verified: decrypting them with it produces
implausible headers) - their compression format is still unconfirmed.
"""
import struct

MASK = 0xFFFFFFFF


def load_keybuf(bios7_path):
    bios7 = open(bios7_path, "rb").read()
    raw = bios7[0x30:0x30 + 0x1048]
    return list(struct.unpack("<1042I", raw))


def _crypt_block(keybuf, ptr0, ptr4, index_range):
    y, x = ptr0, ptr4
    for i in index_range:
        z = keybuf[i] ^ x
        x = keybuf[0x012 + ((z >> 24) & 0xFF)]
        x = (x + keybuf[0x112 + ((z >> 16) & 0xFF)]) & MASK
        x = x ^ keybuf[0x212 + ((z >> 8) & 0xFF)]
        x = (x + keybuf[0x312 + (z & 0xFF)]) & MASK
        x = x ^ y
        y = z
    return x, y  # to be XORed by caller with the appropriate final keybuf words


def encrypt_64bit_block(keybuf, ptr0, ptr4):
    x, y = _crypt_block(keybuf, ptr0, ptr4, range(0x0, 0x10))
    return x ^ keybuf[0x10], y ^ keybuf[0x11]


def decrypt_64bit_block(keybuf, ptr0, ptr4):
    x, y = _crypt_block(keybuf, ptr0, ptr4, range(0x11, 0x1, -1))
    return x ^ keybuf[0x1], y ^ keybuf[0x0]


def _bswap32(v):
    return struct.unpack(">I", struct.pack("<I", v))[0]


def _apply_keycode(keybuf, keycode, word_modulo):
    keycode[1], keycode[2] = encrypt_64bit_block(keybuf, keycode[1], keycode[2])
    keycode[0], keycode[1] = encrypt_64bit_block(keybuf, keycode[0], keycode[1])

    for i in range(0, 0x12):
        keybuf[i] ^= _bswap32(keycode[i % word_modulo])

    s0, s4 = 0, 0
    for i in range(0, 0x412, 2):
        s0, s4 = encrypt_64bit_block(keybuf, s0, s4)
        keybuf[i] = s4
        keybuf[i + 1] = s0


def init_keycode(bios7_path, idcode, level, byte_modulo):
    """byte_modulo matches GBATEK's literal parameter (e.g. 0x0C for firmware)."""
    keybuf = load_keybuf(bios7_path)
    word_modulo = byte_modulo // 4
    keycode = [idcode & MASK, (idcode >> 1) & MASK, (idcode << 1) & MASK]
    if level >= 1:
        _apply_keycode(keybuf, keycode, word_modulo)
    if level >= 2:
        _apply_keycode(keybuf, keycode, word_modulo)
    keycode[1] = (keycode[1] << 1) & MASK
    keycode[2] = (keycode[2] >> 1) & MASK
    if level >= 3:
        _apply_keycode(keybuf, keycode, word_modulo)
    return keybuf


class IncrementalDecryptSource:
    """Decrypts 8-byte blocks lazily as the decompressor asks for more input -
    matches GBATEK's note that bootcode length is unknown until fully
    decompressed, so decryption must be interleaved with decompression rather
    than done as one upfront pass."""

    def __init__(self, data, start, keybuf):
        self.data = data
        self.start = start
        self.keybuf = keybuf
        self.plain = bytearray()

    def ensure(self, n):
        while len(self.plain) < n:
            off = self.start + len(self.plain)
            ptr0, ptr4 = struct.unpack_from("<II", self.data, off)
            d0, d4 = decrypt_64bit_block(self.keybuf, ptr0, ptr4)
            self.plain += struct.pack("<II", d0, d4)

    def byte(self, i):
        self.ensure(i + 1)
        return self.plain[i]


def lz77_decompress_encrypted(data, start, keybuf, max_size=0x400000):
    """Decrypts+decompresses a KEY1-encrypted, LZ77-compressed (SWI 0x11
    format) blob. Returns (decompressed_bytes, encrypted_bytes_consumed) or
    raises ValueError."""
    src = IncrementalDecryptSource(data, start, keybuf)
    src.ensure(4)
    header = struct.unpack_from("<I", src.plain, 0)[0]
    ctype = (header >> 4) & 0xF
    size = header >> 8
    if ctype != 1 or size == 0 or size > max_size:
        raise ValueError(f"not a valid LZ77 header after decrypt: {header:#010x}")

    pos = 4
    out = bytearray()
    while len(out) < size:
        src.ensure(pos + 1)
        flags = src.byte(pos)
        pos += 1
        for bit in range(8):
            if len(out) >= size:
                break
            if flags & (0x80 >> bit):
                src.ensure(pos + 2)
                b0, b1 = src.byte(pos), src.byte(pos + 1)
                pos += 2
                length = (b0 >> 4) + 3
                disp = (((b0 & 0xF) << 8) | b1) + 1
                if disp > len(out):
                    raise ValueError(f"bad back-reference at outlen={len(out)}")
                sc = len(out) - disp
                for i in range(length):
                    out.append(out[sc + i])
            else:
                src.ensure(pos + 1)
                out.append(src.byte(pos))
                pos += 1
    return bytes(out), pos
