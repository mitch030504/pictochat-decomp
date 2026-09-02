//cpp
// decomp: module=unk_autoload_0 addr=0x02322b34 name=FUN_02322b34
// flags: -thumb
#pragma thumb on
extern "C" void FUN_0232e700(void *);
extern "C" void FUN_02322b70(void *, int);

extern "C" asm void FUN_02322b34(void) {
    push {r3, r4, r5, r6, r7, lr}
    ldr r1, =0x0233ae70
    ldr r0, =0x0238ee3c
    mov r4, #0
    ldr r6, =0x0238ee44
    str r1, [r0, #0]
    add r7, r4, #0
@loop:
    mov r0, #0x30
    mul r0, r4
    add r5, r6, r0
    add r0, r5, #0
    bl FUN_0232e700
    add r0, r5, #4
    mov r1, #0x10
    str r7, [r5, #0x20]
    bl FUN_02322b70
    add r0, r4, #1
    lsl r0, r0, #0x18
    lsr r4, r0, #0x18
    cmp r4, #4
    blo @loop
    pop {r3, r4, r5, r6, r7, pc}
}
