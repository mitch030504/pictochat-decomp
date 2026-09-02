//cpp
// decomp: module=unk_autoload_0 addr=0x023222b0 name=FUN_023222b0
// flags: -thumb
#pragma thumb on
extern "C" asm void FUN_023222b0(void) {
    ldr r1, =0x0238e1dc
    mov r2, #0
    mov r0, #2
    str r2, [r1, #0]
    lsl r0, r0, #8
    str r2, [r1, #4]
    orr r2, r0
    lsl r0, r0, #1
    orr r0, r2
    str r0, [r1, #0]
    bx lr
}
