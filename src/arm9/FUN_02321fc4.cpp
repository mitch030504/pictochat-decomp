//cpp
// decomp: module=unk_autoload_0 addr=0x02321fc4 name=FUN_02321fc4
// flags: -thumb
#pragma thumb on
extern "C" void FUN_023213b4(void *, void *, void *, int);
extern "C" void FUN_02321404(void *, void *, int);

extern "C" asm void FUN_02321fc4(void *arg0) {
    push {r3, r4, r5, r6, r7, lr}
    add r7, r0, #0
    mov r5, #0
@loop:
    ldr r3, =0x0233a00c
    lsl r6, r5, #3
    add r3, r3, r6
    mov r0, #0x54
    add r4, r5, #0
    mul r4, r0
    ldr r1, =0x0238e134
    ldrh r3, [r3, #2]
    add r1, r1, r4
    ldr r2, =0x0233b80c
    add r0, r7, #0
    add r1, #0x14
    bl FUN_023213b4
    ldr r0, =0x0238e134
    ldr r2, =0x0238e134
    add r0, r0, r4
    ldr r1, =0x0233b80c
    ldr r2, [r2, r4]
    add r0, #0x14
    bl FUN_02321404
    add r5, r5, #1
    cmp r5, #2
    blt @loop
    pop {r3, r4, r5, r6, r7, pc}
}
