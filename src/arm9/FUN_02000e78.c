// decomp: module=main addr=0x02000e78 name=FUN_02000e78
// ARM946E MPU/cache/TCM system-setup routine, run once from crt0. Same shape
// as sm64ds-decomp's src/_ZN4CP1511SystemSetupEv.c - this exact CP15 init
// sequence (disable MMU, flush/invalidate caches+TLB, program all 8 MPU
// regions, configure cacheability/write-buffer per region, re-enable) is
// standard NitroSDK/TwlSDK boilerplate, not PictoChat-specific code. The MPU
// region config constants below are this ROM's own values (extracted
// directly from its literal pool) - they legitimately differ from
// sm64ds-decomp's, since each game's actual RAM/section layout differs.
//
// Uses GCC-style extended asm (like sm64ds-decomp's version) rather than
// this project's usual per-instruction `asm { instr, cvar }` idiom: letting
// mwccarm materialize a small immediate (e.g. 0x4a) via a C variable always
// pool-loads it here instead of emitting `mov r0,#0x4a` like the real
// binary does - baking the literal MOV text directly into the asm string
// sidesteps that and reproduces the ROM exactly. See notes/cross-project-matching.md.
//
// NOTE: the real function's own trailing literal pool (12 words, 48 bytes)
// sits just past Ghidra's cached symbol boundary (184 bytes) - the true
// size for matching is 0xe8 (232), not Ghidra's 0xb8. See tools/match.py's
// verification command below and notes/matching-style.md.
// verified: tools/match.py --func FUN_02000e78 --addr 0x02000e78 --size 0xe8
// --module main --version dsi/1.3
void FUN_02000e78(void) {
    __asm__ volatile (
        "mrc p15, 0, r0, c1, c0, 0
"
        "ldr r1, =0xf9005
"
        "bic r0, r0, r1
"
        "mcr p15, 0, r0, c1, c0, 0
"
        "mov r0, #0
"
        "mcr p15, 0, r0, c7, c5, 0
"
        "mcr p15, 0, r0, c7, c6, 0
"
        "mcr p15, 0, r0, c7, c10, 4
"
        "ldr r0, =0x04000033
"
        "mcr p15, 0, r0, c6, c0, 0
"
        "ldr r0, =0x02000031
"
        "mcr p15, 0, r0, c6, c1, 0
"
        "ldr r0, =0x027ff017
"
        "mcr p15, 0, r0, c6, c2, 0
"
        "ldr r0, =0x08000035
"
        "mcr p15, 0, r0, c6, c3, 0
"
        "ldr r0, =0x03000000
"
        "orr r0, r0, #0x1a
"
        "orr r0, r0, #1
"
        "mcr p15, 0, r0, c6, c4, 0
"
        "ldr r0, =0x0100002f
"
        "mcr p15, 0, r0, c6, c5, 0
"
        "ldr r0, =0xffff001d
"
        "mcr p15, 0, r0, c6, c6, 0
"
        "ldr r0, =0x02fff017
"
        "mcr p15, 0, r0, c6, c7, 0
"
        "mov r0, #0x20
"
        "mcr p15, 0, r0, c9, c1, 1
"
        "ldr r0, =0x03000000
"
        "orr r0, r0, #0xa
"
        "mcr p15, 0, r0, c9, c1, 0
"
        "mov r0, #0x4a
"
        "mcr p15, 0, r0, c2, c0, 1
"
        "mov r0, #0x4a
"
        "mcr p15, 0, r0, c2, c0, 0
"
        "mov r0, #0xa
"
        "mcr p15, 0, r0, c3, c0, 0
"
        "ldr r0, =0x05101011
"
        "mcr p15, 0, r0, c5, c0, 3
"
        "ldr r0, =0x15111111
"
        "mcr p15, 0, r0, c5, c0, 2
"
        "mrc p15, 0, r0, c1, c0, 0
"
        "ldr r1, =0x5707d
"
        "orr r0, r0, r1
"
        "mcr p15, 0, r0, c1, c0, 0
"
        ::: "r0", "r1"
    );
}
