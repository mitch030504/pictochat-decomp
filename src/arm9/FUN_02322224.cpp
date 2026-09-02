//cpp
// decomp: module=unk_autoload_0 addr=0x02322224 name=FUN_02322224
// flags: -thumb
#pragma thumb on
extern "C" asm void FUN_02322224(void) {
    ldr r1, =0x0238e11c
    mov r0, #1
    ldr r2, [r1, #0]
    orr r0, r2
    str r0, [r1, #0]
    bx lr
}
