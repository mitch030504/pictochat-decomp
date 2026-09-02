//cpp
// decomp: module=unk_autoload_0 addr=0x023225c4 name=FUN_023225c4
// flags: -thumb
#pragma thumb on
extern "C" void FUN_023371a4(void *, void *, int);

extern "C" asm void FUN_023225c4(void) {
    push {r3, lr}
    ldr r0, =0x02fffc9c
    ldr r1, =0x0238e1fa
    add r2, r0, #0
    add r2, #0x34
    ldrb r2, [r2, #0]
    lsl r2, r2, #1
    blx FUN_023371a4
    ldr r0, =0x02fffc9c
    mov r2, #0
    add r0, #0x34
    ldrb r0, [r0, #0]
    lsl r1, r0, #1
    ldr r0, =0x0238e1fa
    strh r2, [r0, r1]
    pop {r3, pc}
}
