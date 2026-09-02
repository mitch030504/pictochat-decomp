//cpp
// decomp: module=unk_autoload_0 addr=0x023216c0 name=FUN_023216c0
// flags: -thumb
#pragma thumb on

extern "C" void FUN_023219d8(void);
extern "C" void FUN_0232dd28(void *, int);
extern "C" void FUN_0232371ec(void *, void *, int);
extern "C" void FUN_0232d890(void *, void *, int, int, void *, void *);
extern "C" void FUN_0232dac0(void *, void *, void *, void *, void *, void *, void *, void *);

extern "C" asm int FUN_023216c0(void *arg0, int arg1, int arg2, int arg3) {
    push {r3, r4, r5, r6, r7, lr}
    sub sp, #0x30
    str r1, [sp, #0x10]
    ldr r1, [sp, #0x4c]
    add r5, r0, #0
    asr r0, r1, #2
    lsr r0, r0, #29
    add r0, r1, r0
    str r2, [sp, #0x14]
    asr r2, r0, #3
    ldr r1, [sp, #0x50]
    add r6, r2, #0
    asr r0, r1, #2
    lsr r0, r0, #29
    add r0, r1, r0
    asr r0, r0, #3
    mul r6, r0
    ldrh r0, [r5, #0xc]
    ldr r1, [sp, #0x54]
    str r3, [sp, #0x18]
    sub r0, r1, r0
    lsl r4, r0, #7
    bl FUN_023219d8
    add r7, r0, #0
    ldr r0, [sp, #0x10]
    ldr r1, [sp, #0x14]
    bl FUN_0232dd28
    ldr r1, [r5, #0x14]
    ldr r2, [sp, #0x4c]
    ldr r3, [sp, #0x50]
    mov r0, #0
    mul r3, r2
    lsr r2, r3, #31
    add r2, r3, r2
    add r1, r1, r4
    asr r2, r2, #1
    blx FUN_0232371ec
    mov r0, #0
    str r0, [sp, #0]
    str r7, [sp, #4]
    ldr r2, [sp, #0x4c]
    ldr r3, [sp, #0x50]
    ldr r1, [r5, #0x14]
    lsl r2, r2, #16
    lsl r3, r3, #16
    add r0, sp, #0x20
    add r1, r1, r4
    lsr r2, r2, #16
    lsr r3, r3, #16
    bl FUN_0232d890
    ldr r0, [sp, #0x10]
    ldr r1, [sp, #0x14]
    bl FUN_0232dd28
    ldrh r2, [r0, #0]
    add r1, sp, #0x1c
    ldr r3, [sp, #0x58]
    strh r2, [r1, #2]
    add r1, sp, #0x1c
    mov r2, #0
    add r1, #2
    ldrsh r1, [r1, r2]
    ldr r2, =0xfffff000
    add r5, r1, #0
    lsl r1, r1, #20
    asr r4, r1, #20
    ldr r1, [sp, #0x18]
    and r5, r2
    add r1, r3, r1
    sub r1, r4, r1
    lsl r1, r1, #16
    asr r3, r1, #16
    lsr r1, r2, #20
    and r1, r3
    add r3, r5, #0
    orr r3, r1
    add r1, sp, #0x1c
    add r1, #2
    strh r3, [r1]
    ldrh r3, [r0, #2]
    add r1, sp, #0x1c
    strh r3, [r1]
    add r3, sp, #0x1c
    mov r1, #0
    ldrsh r1, [r3, r1]
    ldr r3, [sp, #0x5c]
    add r5, r1, #0
    lsl r1, r1, #20
    asr r4, r1, #20
    ldr r1, [sp, #0x48]
    and r5, r2
    add r1, r3, r1
    sub r1, r4, r1
    lsl r1, r1, #16
    asr r3, r1, #16
    lsr r1, r2, #20
    and r1, r3
    add r3, r5, #0
    orr r3, r1
    add r1, sp, #0x1c
    strh r3, [r1]
    ldr r1, [sp, #0x64]
    asr r2, r2, #12
    cmp r1, r2
    bne @skip_h
    ldrh r1, [r0, #6]
    lsl r1, r1, #24
    lsr r1, r1, #24
@skip_h:
    mov r2, #4
    ldrsh r3, [r0, r2]
    add r4, sp, #0x1c
    lsl r3, r3, #24
    asr r3, r3, #24
    str r3, [sp, #0]
    ldrsh r2, [r0, r2]
    mov r3, sp
    sub r3, r3, #4
    lsl r2, r2, #16
    asr r2, r2, #24
    str r2, [sp, #4]
    ldr r2, [sp, #0x60]
    str r2, [sp, #8]
    str r1, [sp, #0xc]
    ldrh r0, [r0, #6]
    ldrh r2, [r4, #2]
    add r1, sp, #0x20
    lsl r0, r0, #16
    strh r2, [r3, #0]
    lsr r0, r0, #28
    lsl r0, r0, #5
    ldrh r4, [r4, #0]
    ldr r2, [r3, #0]
    add r0, r7, r0
    strh r4, [r3, #0]
    ldr r3, [r3, #0]
    bl FUN_0232dac0
    asr r0, r6, #1
    lsr r0, r0, #30
    add r0, r6, r0
    asr r0, r0, #2
    add sp, #0x30
    pop {r3, r4, r5, r6, r7, pc}
}
