//cpp
// decomp: module=unk_autoload_0 addr=0x02321b34 name=FUN_02321b34
// flags: -thumb
#pragma thumb on
extern "C" void *FUN_02321348(void);
extern "C" void FUN_023371ec(void *, void *, int);
extern "C" void FUN_02331268(void *, int);
extern "C" void FUN_023212e8(int);

extern "C" asm void FUN_02321b34(void) {
    push {r3, lr}
    ldr r2, =0x0400100e
    mov r0, #0x43
    ldrh r1, [r2, #0]
    and r1, r0
    mov r0, #0x81
    lsl r0, r0, #4
    orr r0, r1
    strh r0, [r2, #0]
    bl FUN_02321348
    add r1, r0, #0
    mov r2, #6
    ldr r0, =0x008e008e
    lsl r2, r2, #8
    blx FUN_023371ec
    bl FUN_02321348
    mov r1, #6
    lsl r1, r1, #8
    blx FUN_02331268
    mov r0, #8
    bl FUN_023212e8
    pop {r3, pc}
}
