//cpp
// decomp: module=arm7 addr=0x022c6d54 name=_ZN3IRQ7RestoreEj
// Cross-matched from sm64ds-decomp (https://github.com/tangosdev/sm64ds-decomp),
// src/_ZN3IRQ7RestoreEj.cpp. Compiles byte-identical here too (verified: tools/match.py
// --func _ZN3IRQ7RestoreEj --addr 0x022c6d54 --size 0x18 --module arm7
// --version dsi/1.3) - this is a compiler-runtime/SDK primitive (MSL or
// NitroSDK/TwlSDK hardware abstraction layer) statically linked unchanged
// across both projects, not game-specific code. See notes/cross-project-matching.md.
// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
extern "C" unsigned int _ZN3IRQ7RestoreEj(unsigned int);
extern "C" asm unsigned int _ZN3IRQ7RestoreEj(unsigned int)
{
    mrs r1, cpsr
    bic r2, r1, #0x80
    orr r2, r2, r0
    msr cpsr_c, r2
    and r0, r1, #0x80
    bx lr
}
