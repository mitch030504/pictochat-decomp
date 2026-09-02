//cpp
// decomp: module=unk_autoload_0 addr=0x02321f30 name=FUN_02321f30
// flags: -thumb
#pragma thumb on
extern "C" void FUN_02321404(void *, void *, int);

extern "C" asm void FUN_02321f30(void) {
    push {r3, lr}
    ldr r0, =0x0238e09c
    ldr r1, =0x0233b8c4
    mov r2, #0x2d
    bl FUN_02321404
    ldr r0, =0x0238e0dc
    ldr r1, =0x0233b8c4
    mov r2, #0x2e
    bl FUN_02321404
    pop {r3, pc}
}
