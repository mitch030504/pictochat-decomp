//cpp
// decomp: module=unk_autoload_0 addr=0x02322260 name=FUN_02322260
// flags: -thumb
#pragma thumb on
extern "C" asm void FUN_02322260(void) {
    ldr r1, =0x0238e11c
    mov r0, #4
    ldr r2, [r1, #0]
    add r3, r2, #0
    tst r3, r0
    bne @skip
    orr r0, r2
    str r0, [r1, #0]
    mov r0, #1
    str r0, [r1, #0x7c]
@skip:
    bx lr
}
