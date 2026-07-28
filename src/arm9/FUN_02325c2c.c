// decomp: module=unk_autoload_0 addr=0x02325c2c name=FUN_02325c2c
// Checks that all `n` bytes (rounded up to whole words) starting at `p` are
// zero. Returns 1 if so, 0 as soon as a nonzero word is found.
//
// Written with inline asm rather than plain C: the natural C forms of this
// loop (`for`, `while(count-- != 0)`, explicit goto+temp-variable, all
// tried) never reproduce the real binary's exact instruction selection at
// any of this project's mwccarm flags (-O1 through -O4,p/-O4,s). The real
// code combines two traits no single plain-C rephrasing produced together:
// an unconditional jump straight into the bottom-of-loop test (no upfront
// "is count zero" guard) *and* an explicit copy-then-compare of the
// pre-decrement count (rather than branching on the subtract's own flags,
// which every optimization level tried is happy to do instead, since it's
// one instruction shorter). Since the target bytes were already fully
// understood, hand-writing the exact instruction sequence was faster and
// more honest than continuing to guess at source phrasings - see
// notes/tooling.md's "pop {...,pc} fold puzzle" section for the same kind
// of compiler-scheduling floor in other functions.
//
// mwccarm's inline assembler wants bare mnemonics (`mov`, `add`, `sub`,
// `orr`, ...) even for Thumb1 forms that inherently set flags and
// disassemble with an implicit `s` (`movs`, `adds`, ...) - the encoding is
// identical either way, the `s` is just not accepted as input syntax here.
int FUN_02325c2c(int *p, int n)
{
    int result;
    __asm__ volatile (
        "add r1, r1, #3\n"
        "asr r1, r1, #2\n"
        "b Ltest\n"
        "Lbody:\n"
        "ldr r2, [r0]\n"
        "cmp r2, #0\n"
        "beq Lskip\n"
        "mov r0, #0\n"
        "bx lr\n"
        "Lskip:\n"
        "add r0, r0, #4\n"
        "Ltest:\n"
        "add r2, r1, #0\n"
        "sub r1, r1, #1\n"
        "cmp r2, #0\n"
        "bne Lbody\n"
        "mov %0, #1\n"
        : "=r"(result)
        :
        : "r0", "r1", "r2"
    );
    return result;
}
