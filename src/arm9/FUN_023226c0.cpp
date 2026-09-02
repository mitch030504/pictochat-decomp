//cpp
// decomp: module=unk_autoload_0 addr=0x023226c0 name=FUN_023226c0
// flags: -thumb
#pragma thumb on
extern "C" void FUN_02339840(int, void *);

extern "C" asm void FUN_023226c0(void) {
    push {r3, lr}
    ldr r1, =0x0238eda0
    mov r0, #0
    str r0, [r1, #0]
    str r0, [r1, #8]
    str r0, [r1, #0xc]
    str r0, [r1, #4]
    ldr r1, =0x0238eda4
    bl FUN_02339840
    ldr r1, =0x0238eda4
    mov r0, #1
    bl FUN_02339840
    pop {r3, pc}
}
