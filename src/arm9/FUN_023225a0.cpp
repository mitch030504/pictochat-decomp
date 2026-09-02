//cpp
// decomp: module=unk_autoload_0 addr=0x023225a0 name=FUN_023225a0
// flags: -thumb
#pragma thumb on
extern "C" void FUN_023371a4(void *, void *, int);

extern "C" asm void FUN_023225a0(void) {
    push {r3, lr}
    ldr r0, =0x02fffc86
    ldr r1, =0x0238e1e4
    ldrb r2, [r0, #0x14]
    lsl r2, r2, #1
    blx FUN_023371a4
    ldr r0, =0x02fffc86
    mov r2, #0
    ldrb r0, [r0, #0x14]
    lsl r1, r0, #1
    ldr r0, =0x0238e1e4
    strh r2, [r0, r1]
    pop {r3, pc}
}
