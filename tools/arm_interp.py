#!/usr/bin/env python3
"""Minimal ARMv4T (ARM7TDMI) interpreter, purpose-built to dynamically trace
NDS firmware boot on the ARM7 side and observe what happens to Part 3's bytes
after the SPI read - the one thing static analysis (Ghidra) couldn't resolve.

Not a general-purpose emulator: implements only the instruction forms and
peripherals this specific codebase actually uses, extended incrementally as
unimplemented forms are hit (they raise NotImplementedError with full context
rather than silently doing the wrong thing).

Memory map (flat, simplified - no real WRAMCNT-based remapping):
  0x00000000-0x00003FFF  ARM7 BIOS (real bios7.bin, read-only)
  0x02000000-0x023FFFFF  Main RAM (shared)
  0x03000000-0x0380FFFF  Shared WRAM + ARM7 WRAM (flat, not bank-switched)
  0x04000000-0x04000FFF  I/O registers (special-cased)
  SPI flash chip is emulated behind SPICNT/SPIDATA, serving real firmware.bin
  bytes per the real protocol (cmd 0x03 + 3-byte big-endian address).
"""
import struct
import sys
import capstone

# ---- condition codes ----
COND_EQ, COND_NE, COND_CS, COND_CC = 0, 1, 2, 3
COND_MI, COND_PL, COND_VS, COND_VC = 4, 5, 6, 7
COND_HI, COND_LS, COND_GE, COND_LT = 8, 9, 10, 11
COND_GT, COND_LE, COND_AL = 12, 13, 14


class Memory:
    def __init__(self, bios7_path, firmware_path):
        self.bios = bytearray(open(bios7_path, "rb").read())
        self.firmware = bytearray(open(firmware_path, "rb").read())
        self.main_ram = bytearray(0x400000)   # 0x02000000-0x023FFFFF
        self.wram = bytearray(0x810000)       # 0x03000000-0x0380FFFF (generous flat map)
        self.io = {}                          # sparse regs not modeled below
        self.write_log = []                   # (addr, size, value) for post-hoc analysis
        self.fifo_sent = []                    # values written to IPCFIFOSEND
        self.log_writes_to = None              # (lo, hi) range to specially log, or None

        # SPI flash chip state
        self.spi_cs = False
        self.spi_cnt = 0
        self.spi_cmd = None
        self.spi_addr_bytes = []
        self.spi_read_addr = None
        self.spi_last_data = 0

        # timers / IRQ
        self.ime = 0
        self.ie = 0
        self.ifreg = 0
        self.postflg = 0

    # -- region dispatch --
    def _region(self, addr):
        if addr < 0x4000:
            return "bios"
        if 0x02000000 <= addr < 0x02400000:
            return "ram"
        if 0x03000000 <= addr < 0x03810000:
            return "wram"
        if 0x04000000 <= addr < 0x04001000:
            return "io"
        return "unmapped"

    def read(self, addr, size):
        addr &= 0xFFFFFFFF
        region = self._region(addr)
        if region == "bios":
            off = addr
            return int.from_bytes(self.bios[off:off + size], "little")
        if region == "ram":
            off = addr - 0x02000000
            return int.from_bytes(self.main_ram[off:off + size], "little")
        if region == "wram":
            off = addr - 0x03000000
            return int.from_bytes(self.wram[off:off + size], "little")
        if region == "io":
            return self._io_read(addr, size)
        return 0

    def write(self, addr, size, value):
        addr &= 0xFFFFFFFF
        value &= (1 << (size * 8)) - 1
        region = self._region(addr)
        self.write_log.append((addr, size, value))
        if self.log_writes_to and self.log_writes_to[0] <= addr < self.log_writes_to[1]:
            print(f"      [WRITE] {addr:#010x} <- {value:#010x} ({size} bytes)")
        if region == "ram":
            off = addr - 0x02000000
            self.main_ram[off:off + size] = value.to_bytes(size, "little")
        elif region == "wram":
            off = addr - 0x03000000
            self.wram[off:off + size] = value.to_bytes(size, "little")
        elif region == "io":
            self._io_write(addr, size, value)
        elif region == "bios":
            pass  # read-only, ignore
        # unmapped: ignore

    # -- I/O register model --
    def _io_read(self, addr, size):
        if addr == 0x04000208:  # IME
            return self.ime
        if addr == 0x04000210:  # IE
            return self.ie
        if addr == 0x04000214:  # IF
            return self.ifreg
        if addr == 0x04000300:  # POSTFLG
            return self.postflg
        if addr in (0x040001C0, 0x040001C1):  # SPICNT lo/hi byte access
            # Busy bit auto-clears: pretend the transfer always already finished.
            return self.spi_cnt & ~0x80
        if addr in (0x040001C2, 0x040001C3):  # SPIDATA
            return self.spi_last_data
        if addr == 0x04000184:  # IPCFIFOCNT - report both FIFOs empty/ready, not full
            return 0x0101
        return 0

    def _io_write(self, addr, size, value):
        if addr == 0x04000208:
            self.ime = value & 1
            return
        if addr == 0x04000210:
            self.ie = value
            return
        if addr == 0x04000214:
            self.ifreg &= ~value  # write-1-to-clear
            return
        if addr == 0x04000300:
            self.postflg = value
            return
        if addr in (0x040001C0, 0x040001C1):
            self._spi_cnt_write(addr, size, value)
            return
        if addr in (0x040001C2, 0x040001C3):
            self._spi_data_write(value & 0xFF)
            return
        if addr == 0x04000188:  # IPCFIFOSEND
            self.fifo_sent.append(value)
            return
        # unmodeled register: ignore silently

    def _spi_cnt_write(self, addr, size, value):
        self.spi_cnt = value
        cs_bit = bool(value & 0x2000)  # bit13 = hold chip-select (bit 11 on some docs; see note)
        # Real SPICNT: bit11=busy(RO), bit14=chipselect-hold(1=keep low). We treat
        # "not holding" transitions as an implicit deselect between commands only
        # when software clears it; simplest robust model: deselect on 0.
        if value == 0:
            self._spi_deselect()

    def _spi_deselect(self):
        self.spi_cmd = None
        self.spi_addr_bytes = []
        self.spi_read_addr = None

    def _spi_data_write(self, byte):
        if self.spi_cmd is None:
            self.spi_cmd = byte
            self.spi_addr_bytes = []
            self.spi_read_addr = None
            self.spi_last_data = 0
            return
        if self.spi_cmd == 0x03:  # READ
            if len(self.spi_addr_bytes) < 3:
                self.spi_addr_bytes.append(byte)
                self.spi_last_data = 0
                if len(self.spi_addr_bytes) == 3:
                    a0, a1, a2 = self.spi_addr_bytes
                    self.spi_read_addr = (a0 << 16) | (a1 << 8) | a2
                return
            # subsequent bytes clock out firmware data (byte written is dummy/ignored)
            off = self.spi_read_addr % len(self.firmware)
            self.spi_last_data = self.firmware[off]
            self.spi_read_addr += 1
            return
        # unhandled SPI command (e.g. 0x05 RDSR, 0x06 WREN): return 0
        self.spi_last_data = 0


MODE_USR, MODE_FIQ, MODE_IRQ, MODE_SVC, MODE_ABT, MODE_UND, MODE_SYS = \
    0x10, 0x11, 0x12, 0x13, 0x17, 0x1B, 0x1F


class CPU:
    def __init__(self, mem: Memory):
        self.mem = mem
        self.r = [0] * 16
        self.cpsr = MODE_SVC  # boot in supervisor mode like real reset
        self.banked = {  # mode -> {13: sp, 14: lr, 'spsr': v}
            MODE_SVC: {13: 0, 14: 0, "spsr": 0},
            MODE_IRQ: {13: 0, 14: 0, "spsr": 0},
            MODE_SYS: {13: 0, 14: 0, "spsr": 0},
            MODE_USR: {13: 0, 14: 0, "spsr": 0},
            MODE_ABT: {13: 0, 14: 0, "spsr": 0},
            MODE_UND: {13: 0, 14: 0, "spsr": 0},
            MODE_FIQ: {13: 0, 14: 0, "spsr": 0},
        }
        self.instr_count = 0
        self.trace = False
        self.md_arm = capstone.Cs(capstone.CS_ARCH_ARM, capstone.CS_MODE_ARM)
        self.md_thumb = capstone.Cs(capstone.CS_ARCH_ARM, capstone.CS_MODE_THUMB)
        self.md_arm.detail = True
        self.md_thumb.detail = True

    # -- flags --
    @property
    def thumb(self):
        return bool(self.cpsr & (1 << 5))

    @thumb.setter
    def thumb(self, v):
        if v:
            self.cpsr |= (1 << 5)
        else:
            self.cpsr &= ~(1 << 5)

    @property
    def mode(self):
        return self.cpsr & 0x1F

    def set_mode(self, new_mode):
        old = self.mode
        if old == new_mode:
            return
        self.banked.setdefault(old, {13: 0, 14: 0, "spsr": 0})
        self.banked[old][13] = self.r[13]
        self.banked[old][14] = self.r[14]
        self.cpsr = (self.cpsr & ~0x1F) | new_mode
        b = self.banked.setdefault(new_mode, {13: 0, 14: 0, "spsr": 0})
        self.r[13] = b[13]
        self.r[14] = b[14]

    def get_spsr(self):
        return self.banked[self.mode]["spsr"]

    def set_spsr(self, v):
        self.banked[self.mode]["spsr"] = v

    def flag(self, bit):
        return (self.cpsr >> bit) & 1

    def set_flag(self, bit, v):
        if v:
            self.cpsr |= (1 << bit)
        else:
            self.cpsr &= ~(1 << bit)

    N = property(lambda s: s.flag(31))
    Z = property(lambda s: s.flag(30))
    C = property(lambda s: s.flag(29))
    V = property(lambda s: s.flag(28))

    def cond_pass(self, cc):
        n, z, c, v = self.flag(31), self.flag(30), self.flag(29), self.flag(28)
        return {
            COND_EQ: z == 1, COND_NE: z == 0,
            COND_CS: c == 1, COND_CC: c == 0,
            COND_MI: n == 1, COND_PL: n == 0,
            COND_VS: v == 1, COND_VC: v == 0,
            COND_HI: c == 1 and z == 0, COND_LS: c == 0 or z == 1,
            COND_GE: n == v, COND_LT: n != v,
            COND_GT: z == 0 and n == v, COND_LE: z == 1 or n != v,
            COND_AL: True,
        }[cc]

    def pc(self):
        return self.r[15]

    def branch_to(self, addr, exchange=False):
        if exchange:
            self.thumb = bool(addr & 1)
        addr &= ~1 if self.thumb else ~3
        self.r[15] = addr

    def take_irq(self):
        if not self.mem.ime or not (self.mem.ie & self.mem.ifreg):
            return False
        return_addr = self.r[15] + (0 if self.thumb else 0)
        saved_cpsr = self.cpsr
        self.set_mode(MODE_IRQ)
        self.set_spsr(saved_cpsr)
        self.r[14] = return_addr + 4  # BIOS irq stub does `subs pc, lr, #4`
        self.thumb = False
        self.set_flag(7, 1)  # I bit: mask IRQ
        self.r[15] = 0x00000018  # ARM7 IRQ vector
        return True

    # ---------------------------------------------------------------- exec
    def reg_read(self, n, pc_offset):
        if n == 15:
            return (self.r[15] & ~1) + pc_offset
        return self.r[n] & 0xFFFFFFFF

    def reg_write(self, n, v):
        v &= 0xFFFFFFFF
        if n == 15:
            self.branch_to(v, exchange=False)
        else:
            self.r[n] = v

    def step(self):
        md = self.md_thumb if self.thumb else self.md_arm
        pc = self.r[15] & (~1 if self.thumb else ~3)
        code = bytes(self.mem.read(pc + i, 1) for i in range(4))
        insns = list(md.disasm(code, pc, count=1))
        if not insns:
            raise NotImplementedError(f"undecodable at {pc:#010x}: {code.hex()}")
        insn = insns[0]
        size = insn.size
        self.r[15] = pc + size  # default next; branches overwrite
        self._exec(insn, pc)
        self.instr_count += 1
        # service pending IRQ between instructions (simplified - real hw checks continuously)
        if self.mem.ifreg & self.mem.ie and self.mem.ime:
            self.take_irq()

    def _flags_nz(self, result):
        self.set_flag(31, (result >> 31) & 1)
        self.set_flag(30, (result & 0xFFFFFFFF) == 0)

    def _add(self, a, b, carry_in=0, set_flags=False):
        r = (a + b + carry_in) & 0xFFFFFFFF
        if set_flags:
            self._flags_nz(r)
            self.set_flag(29, (a + b + carry_in) > 0xFFFFFFFF)
            sa, sb, sr = a & 0x80000000, b & 0x80000000, r & 0x80000000
            self.set_flag(28, sa == sb and sr != sa)
        return r

    def _sub(self, a, b, borrow_in=1, set_flags=False):
        r = (a - b - (1 - borrow_in)) & 0xFFFFFFFF
        if set_flags:
            self._flags_nz(r)
            self.set_flag(29, a >= (b + (1 - borrow_in)))
            sa, sb, sr = a & 0x80000000, b & 0x80000000, r & 0x80000000
            self.set_flag(28, sa != sb and sr == sb)
        return r

    def _shift(self, val, shift_type, amount, set_c=False):
        val &= 0xFFFFFFFF
        c = self.flag(29)
        if amount == 0:
            return val, c
        if shift_type in ("lsl", 0):
            if amount >= 32:
                c = (val >> (32 - amount)) & 1 if amount == 32 else 0
                return (0, c)
            c = (val >> (32 - amount)) & 1
            return ((val << amount) & 0xFFFFFFFF, c)
        if shift_type in ("lsr", 1):
            if amount >= 32:
                return (0, (val >> 31) & 1 if amount == 32 else 0)
            c = (val >> (amount - 1)) & 1
            return (val >> amount, c)
        if shift_type in ("asr", 2):
            sval = val - 0x100000000 if val & 0x80000000 else val
            if amount >= 32:
                res = -1 if val & 0x80000000 else 0
                return (res & 0xFFFFFFFF, (val >> 31) & 1)
            c = (val >> (amount - 1)) & 1
            return ((sval >> amount) & 0xFFFFFFFF, c)
        if shift_type in ("ror", 3):
            amount %= 32
            if amount == 0:
                return (val, c)
            c = (val >> (amount - 1)) & 1
            res = ((val >> amount) | (val << (32 - amount))) & 0xFFFFFFFF
            return (res, c)
        raise NotImplementedError(f"shift type {shift_type}")

    def _mem_op(self, insn, pc):
        raise NotImplementedError

    def _exec(self, insn, pc):
        pc_off = 4 if self.thumb else 8
        mn = insn.mnemonic
        ops = insn.operands
        handler = HANDLERS.get(mn)
        if handler is None:
            base = mn.rstrip(".w")
            handler = HANDLERS.get(base)
        if handler is None:
            raise NotImplementedError(
                f"unhandled mnemonic {mn!r} {insn.op_str!r} at {pc:#010x} "
                f"(thumb={self.thumb}) bytes={insn.bytes.hex()}"
            )
        handler(self, insn, pc, pc_off)


def op_val(cpu, op, pc_off):
    """Resolve a capstone ARM operand to an integer value (for reading)."""
    t = op.type
    if t == capstone.arm.ARM_OP_IMM:
        return op.imm & 0xFFFFFFFF
    if t == capstone.arm.ARM_OP_REG:
        v = cpu.reg_read(op.reg - capstone.arm.ARM_REG_R0, pc_off) if capstone.arm.ARM_REG_R0 <= op.reg <= capstone.arm.ARM_REG_R12 else None
        if v is None:
            if op.reg == capstone.arm.ARM_REG_SP:
                v = cpu.reg_read(13, pc_off)
            elif op.reg == capstone.arm.ARM_REG_LR:
                v = cpu.reg_read(14, pc_off)
            elif op.reg == capstone.arm.ARM_REG_PC:
                v = cpu.reg_read(15, pc_off)
            else:
                raise NotImplementedError(f"reg operand {op.reg}")
        if op.shift.type != capstone.arm.ARM_SFT_INVALID and op.shift.value is not None:
            amt = op.shift.value
            styp = {capstone.arm.ARM_SFT_LSL: "lsl", capstone.arm.ARM_SFT_LSR: "lsr",
                    capstone.arm.ARM_SFT_ASR: "asr", capstone.arm.ARM_SFT_ROR: "ror"}.get(op.shift.type)
            if styp:
                v, _ = cpu._shift(v, styp, amt)
        return v
    raise NotImplementedError(f"op type {t}")


def reg_num(op):
    r = op.reg
    if capstone.arm.ARM_REG_R0 <= r <= capstone.arm.ARM_REG_R12:
        return r - capstone.arm.ARM_REG_R0
    if r == capstone.arm.ARM_REG_SP:
        return 13
    if r == capstone.arm.ARM_REG_LR:
        return 14
    if r == capstone.arm.ARM_REG_PC:
        return 15
    raise NotImplementedError(f"reg {r}")


def mem_addr(cpu, op, pc_off):
    base = reg_num(op.mem.base) if op.mem.base != 0 else None
    addr = cpu.reg_read(base, pc_off) if base is not None else 0
    disp = op.mem.disp
    if op.mem.index != 0:
        idxv = cpu.reg_read(reg_num(op.mem.index), pc_off)
        if op.shift.type != capstone.arm.ARM_SFT_INVALID and op.shift.value:
            styp = {capstone.arm.ARM_SFT_LSL: "lsl", capstone.arm.ARM_SFT_LSR: "lsr",
                    capstone.arm.ARM_SFT_ASR: "asr", capstone.arm.ARM_SFT_ROR: "ror"}.get(op.shift.type)
            idxv, _ = cpu._shift(idxv, styp, op.shift.value)
        addr += idxv
    else:
        addr += disp
    return addr & 0xFFFFFFFF


HANDLERS = {}


def h(*names):
    def deco(fn):
        for n in names:
            HANDLERS[n] = fn
        return fn
    return deco


def _set_dst(cpu, insn, val, set_flags):
    d = reg_num(insn.operands[0])
    if set_flags and d != 15:
        cpu._flags_nz(val)
    cpu.reg_write(d, val)


def _binop_operands(insn):
    """Return (dst_op, src1_op, src2_op) handling 2-op vs 3-op forms."""
    ops = insn.operands
    if len(ops) == 2:
        return ops[0], ops[0], ops[1]
    return ops[0], ops[1], ops[2]


@h("mov", "movs", "mov.w", "movw")
def _mov(cpu, insn, pc, pc_off):
    d = reg_num(insn.operands[0])
    v = op_val(cpu, insn.operands[1], pc_off)
    if insn.mnemonic in ("mov", "movs") and insn.cc != capstone.arm.ARM_CC_AL and not cpu.cond_pass(_capcc(insn.cc)):
        return
    if insn.mnemonic == "movs" and d != 15:
        cpu._flags_nz(v)
    cpu.reg_write(d, v)


def _capcc(cc):
    mapping = {
        capstone.arm.ARM_CC_EQ: COND_EQ, capstone.arm.ARM_CC_NE: COND_NE,
        capstone.arm.ARM_CC_HS: COND_CS, capstone.arm.ARM_CC_LO: COND_CC,
        capstone.arm.ARM_CC_MI: COND_MI, capstone.arm.ARM_CC_PL: COND_PL,
        capstone.arm.ARM_CC_VS: COND_VS, capstone.arm.ARM_CC_VC: COND_VC,
        capstone.arm.ARM_CC_HI: COND_HI, capstone.arm.ARM_CC_LS: COND_LS,
        capstone.arm.ARM_CC_GE: COND_GE, capstone.arm.ARM_CC_LT: COND_LT,
        capstone.arm.ARM_CC_GT: COND_GT, capstone.arm.ARM_CC_LE: COND_LE,
        capstone.arm.ARM_CC_AL: COND_AL,
    }
    return mapping.get(cc, COND_AL)


def _check_cond(cpu, insn):
    cc = getattr(insn, "cc", capstone.arm.ARM_CC_AL)
    if cc in (capstone.arm.ARM_CC_AL, capstone.arm.ARM_CC_INVALID):
        return True
    return cpu.cond_pass(_capcc(cc))


@h("mvn", "mvns")
def _mvn(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    v = (~op_val(cpu, insn.operands[1], pc_off)) & 0xFFFFFFFF
    _set_dst(cpu, insn, v, insn.mnemonic.endswith("s"))


@h("add", "adds", "add.w", "adc", "adcs")
def _add_i(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    dst, a, b = _binop_operands(insn)
    av = op_val(cpu, a, pc_off)
    bv = op_val(cpu, b, pc_off)
    carry_in = cpu.flag(29) if insn.mnemonic.startswith("adc") else 0
    setf = insn.mnemonic in ("adds",) or (insn.mnemonic == "add" and cpu.thumb and reg_num(dst) != 13 and reg_num(dst) != 15 and False)
    setf = insn.mnemonic.endswith("s")
    r = cpu._add(av, bv, carry_in, set_flags=setf)
    d = reg_num(dst)
    cpu.reg_write(d, r)


@h("sub", "subs", "sub.w", "sbc", "sbcs")
def _sub_i(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    dst, a, b = _binop_operands(insn)
    av = op_val(cpu, a, pc_off)
    bv = op_val(cpu, b, pc_off)
    borrow_in = cpu.flag(29) if insn.mnemonic.startswith("sbc") else 1
    setf = insn.mnemonic.endswith("s")
    r = cpu._sub(av, bv, borrow_in, set_flags=setf)
    cpu.reg_write(reg_num(dst), r)


@h("rsb", "rsbs")
def _rsb(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    dst, a, b = _binop_operands(insn)
    av = op_val(cpu, a, pc_off)
    bv = op_val(cpu, b, pc_off)
    setf = insn.mnemonic.endswith("s")
    r = cpu._sub(bv, av, 1, set_flags=setf)
    cpu.reg_write(reg_num(dst), r)


@h("cmp", "cmp.w")
def _cmp(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    a = op_val(cpu, insn.operands[0], pc_off)
    b = op_val(cpu, insn.operands[1], pc_off)
    cpu._sub(a, b, 1, set_flags=True)


@h("cmn")
def _cmn(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    a = op_val(cpu, insn.operands[0], pc_off)
    b = op_val(cpu, insn.operands[1], pc_off)
    cpu._add(a, b, 0, set_flags=True)


@h("tst")
def _tst(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    a = op_val(cpu, insn.operands[0], pc_off)
    b = op_val(cpu, insn.operands[1], pc_off)
    cpu._flags_nz(a & b)


@h("teq")
def _teq(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    a = op_val(cpu, insn.operands[0], pc_off)
    b = op_val(cpu, insn.operands[1], pc_off)
    cpu._flags_nz(a ^ b)


@h("and", "ands", "and.w")
def _and(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    dst, a, b = _binop_operands(insn)
    r = op_val(cpu, a, pc_off) & op_val(cpu, b, pc_off)
    _set_dst_full(cpu, dst, r, insn.mnemonic.endswith("s"))


@h("orr", "orrs", "orr.w")
def _orr(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    dst, a, b = _binop_operands(insn)
    r = op_val(cpu, a, pc_off) | op_val(cpu, b, pc_off)
    _set_dst_full(cpu, dst, r, insn.mnemonic.endswith("s"))


@h("eor", "eors", "eor.w")
def _eor(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    dst, a, b = _binop_operands(insn)
    r = op_val(cpu, a, pc_off) ^ op_val(cpu, b, pc_off)
    _set_dst_full(cpu, dst, r, insn.mnemonic.endswith("s"))


@h("bic", "bics", "bic.w")
def _bic(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    dst, a, b = _binop_operands(insn)
    r = op_val(cpu, a, pc_off) & ~op_val(cpu, b, pc_off) & 0xFFFFFFFF
    _set_dst_full(cpu, dst, r, insn.mnemonic.endswith("s"))


def _set_dst_full(cpu, dst_op, val, set_flags):
    d = reg_num(dst_op)
    if set_flags and d != 15:
        cpu._flags_nz(val)
    cpu.reg_write(d, val & 0xFFFFFFFF)


@h("lsl", "lsls", "lsl.w")
def _lsl(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    dst, a, b = _binop_operands(insn)
    av = op_val(cpu, a, pc_off)
    bv = op_val(cpu, b, pc_off) if insn.operands[-1].type != capstone.arm.ARM_OP_IMM or len(insn.operands) > 2 else insn.operands[-1].imm
    r, c = cpu._shift(av, "lsl", bv)
    if insn.mnemonic.endswith("s"):
        cpu.set_flag(29, c)
        cpu._flags_nz(r)
    cpu.reg_write(reg_num(dst), r)


@h("lsr", "lsrs", "lsr.w")
def _lsr(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    dst, a, b = _binop_operands(insn)
    av = op_val(cpu, a, pc_off)
    bv = op_val(cpu, b, pc_off)
    r, c = cpu._shift(av, "lsr", bv)
    if insn.mnemonic.endswith("s"):
        cpu.set_flag(29, c)
        cpu._flags_nz(r)
    cpu.reg_write(reg_num(dst), r)


@h("asr", "asrs", "asr.w")
def _asr(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    dst, a, b = _binop_operands(insn)
    av = op_val(cpu, a, pc_off)
    bv = op_val(cpu, b, pc_off)
    r, c = cpu._shift(av, "asr", bv)
    if insn.mnemonic.endswith("s"):
        cpu.set_flag(29, c)
        cpu._flags_nz(r)
    cpu.reg_write(reg_num(dst), r)


@h("ror", "rors", "ror.w")
def _ror(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    dst, a, b = _binop_operands(insn)
    av = op_val(cpu, a, pc_off)
    bv = op_val(cpu, b, pc_off)
    r, c = cpu._shift(av, "ror", bv)
    if insn.mnemonic.endswith("s"):
        cpu.set_flag(29, c)
        cpu._flags_nz(r)
    cpu.reg_write(reg_num(dst), r)


@h("mul", "muls")
def _mul(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    dst, a, b = _binop_operands(insn)
    r = (op_val(cpu, a, pc_off) * op_val(cpu, b, pc_off)) & 0xFFFFFFFF
    _set_dst_full(cpu, dst, r, insn.mnemonic.endswith("s"))


@h("adr")
def _adr(cpu, insn, pc, pc_off):
    d = reg_num(insn.operands[0])
    v = op_val(cpu, insn.operands[1], pc_off)
    cpu.reg_write(d, v)


def _ldr_size(mn):
    if "b" in mn.replace("bx", "").replace("bl", "").replace("bic", "").replace("bne", "").replace("beq", ""):
        pass
    return 4


@h("ldr", "ldr.w")
def _ldr(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    d = reg_num(insn.operands[0])
    addr = mem_addr(cpu, insn.operands[1], pc_off)
    v = cpu.mem.read(addr, 4)
    _writeback(cpu, insn, pc_off)
    cpu.reg_write(d, v)


@h("ldrb", "ldrb.w")
def _ldrb(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    d = reg_num(insn.operands[0])
    addr = mem_addr(cpu, insn.operands[1], pc_off)
    v = cpu.mem.read(addr, 1)
    _writeback(cpu, insn, pc_off)
    cpu.reg_write(d, v)


@h("ldrh", "ldrh.w")
def _ldrh(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    d = reg_num(insn.operands[0])
    addr = mem_addr(cpu, insn.operands[1], pc_off)
    v = cpu.mem.read(addr, 2)
    _writeback(cpu, insn, pc_off)
    cpu.reg_write(d, v)


@h("ldrsb")
def _ldrsb(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    d = reg_num(insn.operands[0])
    addr = mem_addr(cpu, insn.operands[1], pc_off)
    v = cpu.mem.read(addr, 1)
    if v & 0x80:
        v -= 0x100
    _writeback(cpu, insn, pc_off)
    cpu.reg_write(d, v & 0xFFFFFFFF)


@h("ldrsh")
def _ldrsh(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    d = reg_num(insn.operands[0])
    addr = mem_addr(cpu, insn.operands[1], pc_off)
    v = cpu.mem.read(addr, 2)
    if v & 0x8000:
        v -= 0x10000
    _writeback(cpu, insn, pc_off)
    cpu.reg_write(d, v & 0xFFFFFFFF)


@h("str", "str.w")
def _str(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    d = reg_num(insn.operands[0])
    addr = mem_addr(cpu, insn.operands[1], pc_off)
    cpu.mem.write(addr, 4, cpu.reg_read(d, pc_off))
    _writeback(cpu, insn, pc_off)


@h("strb", "strb.w")
def _strb(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    d = reg_num(insn.operands[0])
    addr = mem_addr(cpu, insn.operands[1], pc_off)
    cpu.mem.write(addr, 1, cpu.reg_read(d, pc_off) & 0xFF)
    _writeback(cpu, insn, pc_off)


@h("strh", "strh.w")
def _strh(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    d = reg_num(insn.operands[0])
    addr = mem_addr(cpu, insn.operands[1], pc_off)
    cpu.mem.write(addr, 2, cpu.reg_read(d, pc_off) & 0xFFFF)
    _writeback(cpu, insn, pc_off)


def _writeback(cpu, insn, pc_off):
    op = insn.operands[1]
    if op.type != capstone.arm.ARM_OP_MEM:
        return
    if not (op.mem.base != 0 and getattr(op, "subtracted", False) is not None):
        pass
    # capstone marks writeback via insn.writeback for ARM; for Thumb pre/post
    # forms it's encoded in the mnemonic/op_str already handled by mem_addr,
    # but explicit '!' writeback needs applying:
    if getattr(insn, "writeback", False):
        base = reg_num(op.mem.base)
        addr = mem_addr(cpu, op, pc_off)
        cpu.reg_write(base, addr)


@h("b", "b.w")
def _b(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    target = insn.operands[0].imm
    cpu.branch_to(target)


@h("bl", "bl.w", "blx")
def _bl(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    ret = pc + insn.size
    op = insn.operands[0]
    if op.type == capstone.arm.ARM_OP_REG:
        target = cpu.reg_read(reg_num(op), pc_off)
        cpu.r[14] = ret | 1 if cpu.thumb else ret
        cpu.branch_to(target, exchange=True)
    else:
        cpu.r[14] = (ret | 1) if cpu.thumb else ret
        exch = insn.mnemonic == "blx"
        if exch:
            cpu.branch_to(op.imm, exchange=True)
            cpu.thumb = not cpu.thumb if False else cpu.thumb  # blx imm toggles state; handled by exchange
        else:
            cpu.branch_to(op.imm)


@h("bx")
def _bx(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    target = cpu.reg_read(reg_num(insn.operands[0]), pc_off)
    cpu.branch_to(target, exchange=True)


@h("push")
def _push(cpu, insn, pc, pc_off):
    regs = [reg_num(o) for o in insn.operands]
    sp = cpu.reg_read(13, pc_off)
    for rn in sorted(regs, reverse=True):
        sp -= 4
        cpu.mem.write(sp, 4, cpu.reg_read(rn, pc_off))
    cpu.reg_write(13, sp)


@h("pop")
def _pop(cpu, insn, pc, pc_off):
    regs = [reg_num(o) for o in insn.operands]
    sp = cpu.reg_read(13, pc_off)
    for rn in sorted(regs):
        v = cpu.mem.read(sp, 4)
        sp += 4
        if rn == 15:
            cpu.branch_to(v, exchange=True)
        else:
            cpu.reg_write(rn, v)
    cpu.reg_write(13, sp)


@h("stmia", "stmea", "stm")
def _stmia(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    base_op = insn.operands[0]
    base = reg_num(base_op)
    addr = cpu.reg_read(base, pc_off)
    regs = [reg_num(o) for o in insn.operands[1:]]
    for rn in sorted(regs):
        cpu.mem.write(addr, 4, cpu.reg_read(rn, pc_off))
        addr += 4
    if getattr(insn, "writeback", True):
        cpu.reg_write(base, addr)


@h("ldmia", "ldmfd", "ldm")
def _ldmia(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    base_op = insn.operands[0]
    base = reg_num(base_op)
    addr = cpu.reg_read(base, pc_off)
    regs = [reg_num(o) for o in insn.operands[1:]]
    for rn in sorted(regs):
        v = cpu.mem.read(addr, 4)
        addr += 4
        if rn == 15:
            cpu.branch_to(v, exchange=True)
        else:
            cpu.reg_write(rn, v)
    if getattr(insn, "writeback", True) and base not in regs:
        cpu.reg_write(base, addr)


@h("ldmdb")
def _ldmdb(cpu, insn, pc, pc_off):
    if not _check_cond(cpu, insn):
        return
    base_op = insn.operands[0]
    base = reg_num(base_op)
    regs = sorted(reg_num(o) for o in insn.operands[1:])
    addr = cpu.reg_read(base, pc_off) - 4 * len(regs)
    a = addr
    for rn in regs:
        v = cpu.mem.read(a, 4)
        a += 4
        if rn == 15:
            cpu.branch_to(v, exchange=True)
        else:
            cpu.reg_write(rn, v)
    if getattr(insn, "writeback", True):
        cpu.reg_write(base, addr)


@h("svc", "swi")
def _svc(cpu, insn, pc, pc_off):
    raise SvcCall(insn.operands[0].imm if insn.operands else 0, pc)


class SvcCall(Exception):
    def __init__(self, num, pc):
        self.num = num
        self.pc = pc


@h("mrs")
def _mrs(cpu, insn, pc, pc_off):
    d = reg_num(insn.operands[0])
    cpu.reg_write(d, cpu.cpsr)


@h("msr")
def _msr(cpu, insn, pc, pc_off):
    src = insn.operands[-1]
    v = op_val(cpu, src, pc_off)
    flags_only = "f" in insn.op_str.split(",")[0].lower() and "c" not in insn.op_str.split(",")[0].lower()
    field = insn.op_str.split(",")[0].strip().lower()
    is_spsr = field.startswith("spsr")
    if "_" in field:
        mask_chars = field.split("_", 1)[1]
    else:
        mask_chars = "fc"
    mask = 0
    if "c" in mask_chars:
        mask |= 0x000000FF
    if "x" in mask_chars:
        mask |= 0x0000FF00
    if "s" in mask_chars:
        mask |= 0x00FF0000
    if "f" in mask_chars:
        mask |= 0xFF000000
    if is_spsr:
        cur = cpu.get_spsr()
        cpu.set_spsr((cur & ~mask) | (v & mask))
    else:
        new_mode = v & 0x1F if (mask & 0xFF) else cpu.mode
        cur = cpu.cpsr
        newcpsr = (cur & ~mask) | (v & mask)
        cpu.cpsr = newcpsr
        if mask & 0xFF:
            cpu.set_mode(new_mode)
            cpu.cpsr = (cpu.cpsr & ~0x1F) | new_mode


@h("nop")
def _nop(cpu, insn, pc, pc_off):
    pass


@h("mcr", "mrc")
def _cop(cpu, insn, pc, pc_off):
    pass  # ARM7 has no CP15; treat as no-op if it ever appears
