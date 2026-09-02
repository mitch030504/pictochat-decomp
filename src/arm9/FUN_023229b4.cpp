//cpp
// decomp: module=unk_autoload_0 addr=0x023229b4 name=FUN_023229b4
// flags: -thumb
#pragma thumb on
extern "C" asm void FUN_023229b4(void) {
    ldr r0, =0x0233ae40
    mov r1, #1
    str r1, [r0, #0]
    ldr r0, =0x0238ede0
    mov r1, #0
    str r1, [r0, #0x14]
    bx lr
}
