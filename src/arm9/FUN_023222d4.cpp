//cpp
// decomp: module=unk_autoload_0 addr=0x023222d4 name=FUN_023222d4
// flags: -thumb
#pragma thumb on
extern "C" void *FUN_023222cc(void);
extern "C" int FUN_02322454(void);

extern "C" asm void FUN_023222d4(void) {
    push {r4, r5, r6, lr}
    bl FUN_023222cc
    add r4, r0, #0
    bl FUN_02322454
    ldr r1, [r4, #0]
    mov r2, #4
    ldr r3, =0x02fffce4
    bic r1, r2
    ldrh r2, [r3, #0]
    lsl r2, r2, #0x16
    lsr r2, r2, #0x1f
    lsl r2, r2, #0x1f
    lsr r2, r2, #0x1d
    orr r1, r2
    str r1, [r4, #0]
    ldrh r5, [r3, #0]
    lsl r1, r5, #0x15
    lsr r6, r1, #0x1f
    lsl r1, r5, #0x11
    lsr r3, r1, #0x1f
    lsl r1, r5, #0x14
    lsr r2, r1, #0x1f
    lsl r1, r5, #0x12
    lsl r5, r5, #0x10
    lsr r1, r1, #0x1f
    lsr r5, r5, #0x1f
    and r1, r5
    and r1, r2
    and r1, r3
    tst r1, r6
    bne @lbl_31a
    mov r5, #1
    b @lbl_31c
@lbl_31a:
    mov r5, #0
@lbl_31c:
    ldr r3, [r4, #0]
    mov r1, #1
    bic r3, r1
    mov r2, #1
    add r1, r5, #0
    and r1, r2
    orr r1, r3
    mov r3, #2
    str r1, [r4, #0]
    bic r1, r3
    ldr r3, =0x02fffce4
    add r6, r1, #0
    ldrh r3, [r3, #0]
    ldr r1, =0xfffffeff
    lsl r3, r3, #0x12
    lsr r3, r3, #0x1f
    lsl r3, r3, #0x1f
    lsr r3, r3, #0x1e
    orr r6, r3
    lsl r3, r6, #0x1f
    lsr r5, r3, #0x1f
    lsl r3, r6, #0x1d
    lsr r3, r3, #0x1f
    orr r3, r5
    lsl r3, r3, #0x1f
    and r1, r6
    lsr r3, r3, #0x17
    orr r1, r3
    str r1, [r4, #0]
    mov r1, #8
    tst r1, r0
    bne @lbl_35e
    mov r2, #0
@lbl_35e:
    ldr r3, [r4, #0]
    mov r1, #8
    bic r3, r1
    lsl r1, r2, #0x1f
    lsr r1, r1, #0x1c
    orr r1, r3
    str r1, [r4, #0]
    ldr r3, =0x02fff890
    mov r1, #0
    ldr r5, [r3, #0]
    mov r3, #2
    add r2, r1, #0
    tst r3, r5
    beq @lbl_37e
    mov r3, #1
    b @lbl_380
@lbl_37e:
    add r3, r1, #0
@lbl_380:
    cmp r3, #0
    beq @lbl_38e
    ldr r3, [r4, #0]
    lsl r3, r3, #0x17
    lsr r3, r3, #0x1f
    beq @lbl_38e
    mov r2, #1
@lbl_38e:
    cmp r2, #0
    beq @lbl_39c
    ldr r2, =0x00000d03
    and r0, r2
    cmp r0, r2
    bne @lbl_39c
    mov r1, #1
@lbl_39c:
    ldr r2, [r4, #0]
    mov r0, #0x40
    bic r2, r0
    lsl r0, r1, #0x1f
    lsr r0, r0, #0x19
    orr r0, r2
    str r0, [r4, #0]
    ldr r0, =0x02fff890
    mov r1, #0
    ldr r2, [r0, #0]
    mov r0, #2
    tst r0, r2
    beq @lbl_3ba
    mov r0, #1
    b @lbl_3bc
@lbl_3ba:
    add r0, r1, #0
@lbl_3bc:
    cmp r0, #0
    beq @lbl_3e8
    ldr r2, =0x02fff890
    mov r0, #0
    ldr r3, [r2, #0]
    mov r2, #2
    tst r2, r3
    beq @lbl_3d0
    mov r2, #1
    b @lbl_3d2
@lbl_3d0:
    add r2, r0, #0
@lbl_3d2:
    cmp r2, #0
    beq @lbl_3e2
    ldr r2, =0x02fffe1f
    ldrb r2, [r2, #0]
    lsl r2, r2, #0x1d
    lsr r2, r2, #0x1f
    beq @lbl_3e2
    mov r0, #1
@lbl_3e2:
    cmp r0, #0
    beq @lbl_3e8
    mov r1, #1
@lbl_3e8:
    ldr r3, [r4, #0]
    mov r2, #0x40
    add r0, r3, #0
    bic r0, r2
    lsl r2, r3, #0x19
    lsr r2, r2, #0x1f
    orr r1, r2
    lsl r1, r1, #0x1f
    lsr r1, r1, #0x19
    orr r0, r1
    str r0, [r4, #0]
    ldr r2, =0x02fffce4
    mov r0, #0
    ldrh r2, [r2, #0]
    add r1, r0, #0
    lsl r2, r2, #0x19
    lsr r2, r2, #0x1f
    beq @lbl_422
    ldr r2, =0x02fff890
    ldr r3, [r2, #0]
    mov r2, #2
    tst r2, r3
    beq @lbl_41a
    mov r2, #1
    b @lbl_41c
@lbl_41a:
    add r2, r0, #0
@lbl_41c:
    cmp r2, #0
    beq @lbl_422
    mov r1, #1
@lbl_422:
    cmp r1, #0
    beq @lbl_430
    ldr r1, [r4, #0]
    lsl r1, r1, #0x1c
    lsr r1, r1, #0x1f
    bne @lbl_430
    mov r0, #1
@lbl_430:
    ldr r2, [r4, #0]
    mov r1, #0x20
    lsl r0, r0, #0x1f
    bic r2, r1
    lsr r0, r0, #0x1a
    orr r0, r2
    str r0, [r4, #0]
    pop {r4, r5, r6, pc}
}
