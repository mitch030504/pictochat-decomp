//cpp
// decomp: module=unk_autoload_0 addr=0x0233206c name=_ZN3IRQ6EnableEv
// Cross-matched from sm64ds-decomp (https://github.com/tangosdev/sm64ds-decomp),
// src/_ZN3IRQ6EnableEv.cpp. Compiles byte-identical here too (verified: tools/match.py
// --func _ZN3IRQ6EnableEv --addr 0x0233206c --size 0x14 --module unk_autoload_0
// --version dsi/1.3) - this is a compiler-runtime/SDK primitive (MSL or
// NitroSDK/TwlSDK hardware abstraction layer) statically linked unchanged
// across both projects, not game-specific code. See notes/cross-project-matching.md.
// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
extern "C" unsigned int _ZN3IRQ6EnableEv(void);
extern "C" asm unsigned int _ZN3IRQ6EnableEv(void)
{
    mrs r0, cpsr
    bic r1, r0, #0x80
    msr cpsr_c, r1
    and r0, r0, #0x80
    bx lr
}
