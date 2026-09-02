//cpp
// decomp: module=unk_autoload_0 addr=0x02323068 name=FUN_02323068
// flags: -thumb
#pragma thumb on
extern "C" void FUN_02337238(void *, void *, int);
extern "C" void FUN_02323d00(void *, int, int, int, int);

extern "C" asm void FUN_02323068(void *arg0) {
    push {r4, r5, r6, r7, lr}
    sub sp, #0xc
    add r5, r0, #0
    mov r2, #0x8a
    ldr r0, =0x023956d0
    ldr r1, =0x02393450
    lsl r2, r2, #6
    ldr r4, [r5, #8]
    blx FUN_02337238
    ldrh r0, [r5, #0x26]
    cmp r0, #0
    beq @check_active
    sub r0, r0, #1
    ldr r2, [r4, #0]
    lsl r0, r0, #3
    add r1, r2, r0
    ldrh r2, [r2, r0]
    lsl r0, r2, #0x10
    lsr r0, r0, #0x18
    lsl r2, r2, #0x18
    str r0, [sp, #0]
    lsr r3, r2, #0x18
    ldrh r2, [r1, #2]
    lsl r2, r2, #0x18
    lsr r2, r2, #0x18
    add r2, r3, r2
    str r2, [sp, #4]
    ldrh r1, [r1, #2]
    mov r2, #8
    lsl r1, r1, #0x10
    lsr r1, r1, #0x18
    add r0, r0, r1
    str r0, [sp, #8]
    ldr r0, =0x02393450
    mov r1, #0xc8
    bl FUN_02323d00
@check_active:
    ldr r4, [r5, #0]
    cmp r4, #0
    beq @ret
    ldr r0, [r5, #4]
    cmp r0, #0
    beq @ret
    ldrh r2, [r5, #0x26]
    ldrh r1, [r0, #6]
    cmp r1, r2
    beq @ret
    ldrh r0, [r4, #6]
    cmp r0, r2
    beq @ret
    cmp r0, #0
    beq @ret
    cmp r0, r1
    bne @loop_multi
    ldr r7, =0x02393450
    mov r6, #4
@loop_single:
    ldrh r1, [r4, #0]
    mov r2, #8
    lsl r0, r1, #0x10
    lsr r0, r0, #0x18
    lsl r1, r1, #0x18
    str r0, [sp, #0]
    lsr r3, r1, #0x18
    ldrh r1, [r4, #2]
    lsl r1, r1, #0x18
    lsr r1, r1, #0x18
    add r1, r3, r1
    str r1, [sp, #4]
    ldrh r1, [r4, #2]
    lsl r1, r1, #0x10
    lsr r1, r1, #0x18
    add r0, r0, r1
    str r0, [sp, #8]
    add r0, r7, #0
    mov r1, #0xc8
    bl FUN_02323d00
    ldrsh r0, [r4, r6]
    lsl r0, r0, #3
    add r4, r4, r0
    ldr r0, [r5, #0]
    cmp r4, r0
    bne @loop_single
    b @ret
@loop_multi:
    ldr r7, =0x02393450
    mov r6, #4
@loop_multi_inner:
    ldrh r1, [r4, #0]
    mov r2, #8
    lsl r0, r1, #0x10
    lsr r0, r0, #0x18
    lsl r1, r1, #0x18
    str r0, [sp, #0]
    lsr r3, r1, #0x18
    ldrh r1, [r4, #2]
    add r0, r0, #1
    lsl r1, r1, #0x18
    lsr r1, r1, #0x18
    add r1, r3, r1
    str r1, [sp, #4]
    str r0, [sp, #8]
    add r0, r7, #0
    mov r1, #0xc8
    bl FUN_02323d00
    ldrh r0, [r4, #0]
    ldrh r2, [r4, #2]
    lsl r1, r0, #0x10
    lsl r2, r2, #0x10
    lsl r0, r0, #0x18
    lsr r1, r1, #0x18
    lsr r2, r2, #0x18
    add r1, r1, r2
    lsr r3, r0, #0x18
    sub r0, r1, #1
    str r0, [sp, #0]
    ldrh r0, [r4, #2]
    mov r2, #8
    lsl r0, r0, #0x18
    lsr r0, r0, #0x18
    add r0, r3, r0
    str r0, [sp, #4]
    str r1, [sp, #8]
    add r0, r7, #0
    mov r1, #0xc8
    bl FUN_02323d00
    ldrh r1, [r4, #0]
    mov r2, #8
    lsl r0, r1, #0x10
    lsl r1, r1, #0x18
    lsr r0, r0, #0x18
    lsr r3, r1, #0x18
    add r1, r0, #1
    str r1, [sp, #0]
    add r1, r3, #1
    str r1, [sp, #4]
    ldrh r1, [r4, #2]
    lsl r1, r1, #0x10
    lsr r1, r1, #0x18
    add r0, r0, r1
    sub r0, r0, #1
    str r0, [sp, #8]
    add r0, r7, #0
    mov r1, #0xc8
    bl FUN_02323d00
    ldrh r0, [r4, #0]
    ldrh r2, [r4, #2]
    lsl r1, r0, #0x10
    lsl r0, r0, #0x18
    lsl r2, r2, #0x18
    lsr r1, r1, #0x18
    lsr r0, r0, #0x18
    lsr r2, r2, #0x18
    add r3, r0, r2
    add r0, r1, #1
    str r0, [sp, #0]
    str r3, [sp, #4]
    ldrh r0, [r4, #2]
    mov r2, #8
    sub r3, r3, #1
    lsl r0, r0, #0x10
    lsr r0, r0, #0x18
    add r0, r1, r0
    sub r0, r0, #1
    str r0, [sp, #8]
    add r0, r7, #0
    mov r1, #0xc8
    bl FUN_02323d00
    ldrsh r0, [r4, r6]
    lsl r0, r0, #3
    add r4, r4, r0
    ldr r0, [r5, #0]
    cmp r4, r0
    bne @loop_multi_inner
@ret:
    add sp, #0xc
    pop {r4, r5, r6, r7, pc}
}
