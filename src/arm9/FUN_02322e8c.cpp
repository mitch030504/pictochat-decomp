//cpp
// decomp: module=unk_autoload_0 addr=0x02322e8c name=FUN_02322e8c
// flags: -thumb
#pragma thumb on
extern "C" asm void *FUN_02322e8c(void *arg0, int arg1, int arg2) {
    push {r4, r5, r6, r7, lr}
    sub sp, #0x1c
    add r7, r0, #0
    str r1, [sp, #0x18]
    ldr r1, [r7, #8]
    add r3, r2, #0
    ldr r0, [r1, #0]
    mov r2, #0
    str r0, [sp, #4]
    ldrh r0, [r1, #0xe]
    ldr r1, =0x0238ef0c
    str r0, [sp, #0]
    ldr r0, [r1, #8]
    ldr r1, [r1, #4]
    b @check_loop_1
@body_1:
    ldr r5, [sp, #4]
    lsl r6, r2, #3
    ldrh r5, [r5, r6]
    ldr r4, [sp, #4]
    str r5, [sp, #0xc]
    lsl r5, r5, #0x10
    lsr r5, r5, #0x18
    str r5, [sp, #8]
    add r5, r0, r5
    add r4, r4, r6
    cmp r5, r3
    bgt @next_1
    ldrh r5, [r4, #2]
    str r5, [sp, #0x10]
    lsl r5, r5, #0x10
    lsr r6, r5, #0x18
    ldr r5, [sp, #8]
    add r5, r5, r6
    add r5, r0, r5
    cmp r3, r5
    bge @next_1
    ldr r5, [sp, #0xc]
    lsl r5, r5, #0x18
    lsr r5, r5, #0x18
    str r5, [sp, #0x14]
    add r6, r1, r5
    ldr r5, [sp, #0x18]
    cmp r6, r5
    bgt @next_1
    ldr r5, [sp, #0x10]
    lsl r5, r5, #0x18
    lsr r6, r5, #0x18
    ldr r5, [sp, #0x14]
    add r5, r5, r6
    add r6, r1, r5
    ldr r5, [sp, #0x18]
    cmp r5, r6
    bge @next_1
    ldr r5, [r7, #0x34]
    cmp r5, #0
    beq @found_1
    ldrh r5, [r4, #6]
    cmp r5, #2
    beq @next_1
@found_1:
    add r0, r4, #0
    b @ret
@next_1:
    add r2, r2, #1
@check_loop_1:
    ldr r4, [sp, #0]
    cmp r2, r4
    blt @body_1
    ldr r2, [r7, #0x34]
    cmp r2, #0
    beq @ret_null
    mov r2, #0
    b @check_loop_2
@body_2:
    ldr r5, =0x0233af1c
    lsl r4, r2, #3
    add r7, r5, r4
    ldrh r4, [r5, r4]
    mov lr, r4
    lsl r4, r4, #0x10
    lsr r5, r4, #0x18
    add r4, r0, r5
    cmp r4, r3
    bgt @next_2
    ldrh r4, [r7, #2]
    mov ip, r4
    lsl r4, r4, #0x10
    lsr r4, r4, #0x18
    add r4, r5, r4
    add r4, r0, r4
    cmp r3, r4
    bge @next_2
    mov r4, lr
    lsl r4, r4, #0x18
    lsr r6, r4, #0x18
    ldr r4, [sp, #0x18]
    add r5, r1, r6
    cmp r5, r4
    bgt @next_2
    mov r4, ip
    lsl r4, r4, #0x18
    lsr r4, r4, #0x18
    add r4, r6, r4
    add r5, r1, r4
    ldr r4, [sp, #0x18]
    cmp r4, r5
    bge @next_2
    add r0, r7, #0
    b @ret
@next_2:
    add r2, r2, #1
@check_loop_2:
    cmp r2, #2
    blo @body_2
@ret_null:
    mov r0, #0
@ret:
    add sp, #0x1c
    pop {r4, r5, r6, r7, pc}
}
