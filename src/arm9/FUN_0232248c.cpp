//cpp
// decomp: module=unk_autoload_0 addr=0x0232248c name=FUN_0232248c
// flags: -thumb
#pragma thumb on
extern "C" void *FUN_023222cc(void);
extern "C" int FUN_02322454(void);
extern "C" void FUN_02331e8c(void);

extern "C" asm void FUN_0232248c(void) {
    push {r4, r5, r6, lr}
    bl FUN_023222cc
    add r4, r0, #0
    bl FUN_02322454
    ldr r1, [r4, #0]
    lsl r2, r1, #0x19
    lsr r2, r2, #0x1f
    bne @lbl_ret
    lsl r2, r1, #0x17
    lsr r2, r2, #0x1f
    beq @lbl_check_pad
    ldr r2, =0x00000e03
    add r3, r0, #0
    and r3, r2
    cmp r3, r2
    bne @lbl_check_pad
    mov r0, #0x10
    orr r0, r1
    str r0, [r4, #0]
    pop {r4, r5, r6, pc}
@lbl_check_pad:
    ldr r1, =0x02fffce4
    ldrh r1, [r1, #0]
    lsl r1, r1, #0x19
    lsr r1, r1, #0x1f
    beq @lbl_ret
    ldr r1, =0x02fff890
    mov r2, #1
    ldr r1, [r1, #0]
    tst r1, r2
    bne @lbl_set_flag
    mov r2, #0
@lbl_set_flag:
    cmp r2, #0
    beq @lbl_ret
    ldr r1, [r4, #0]
    lsl r1, r1, #0x1c
    lsr r1, r1, #0x1f
    bne @lbl_ret
    ldr r1, =0x02fff890
    ldr r2, [r1, #0]
    mov r1, #2
    tst r1, r2
    beq @lbl_zero
    mov r1, #1
    b @lbl_check_r1
@lbl_zero:
    mov r1, #0
@lbl_check_r1:
    cmp r1, #0
    bne @lbl_loop_init
    ldr r1, [r4, #0]
    mov r0, #0x10
    orr r0, r1
    str r0, [r4, #0]
    pop {r4, r5, r6, pc}
@lbl_loop_init:
    mov r5, #0
    mov r6, #2
    b @lbl_loop_cond
@lbl_loop_body:
    bl FUN_02331e8c
    bl FUN_02322454
    add r5, r5, #1
    cmp r5, #0x1e
    blt @lbl_loop_cond
    ldr r1, [r4, #0]
    mov r0, #0x10
    orr r0, r1
    str r0, [r4, #0]
    pop {r4, r5, r6, pc}
@lbl_loop_cond:
    tst r0, r6
    bne @lbl_loop_body
@lbl_ret:
    pop {r4, r5, r6, pc}
}
