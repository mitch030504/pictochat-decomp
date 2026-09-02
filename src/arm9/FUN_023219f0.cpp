//cpp
// decomp: module=unk_autoload_0 addr=0x023219f0 name=FUN_023219f0
// flags: -thumb
#pragma thumb on

extern "C" asm void FUN_023219f0(void) {
    push {r3, r4, r5, r6, r7, lr}
    sub sp, #0x18
    ldr r4, =0x02339fe8
    add r3, sp, #0
    mov r2, #3
@copy_init:
    ldmia r4!, {r0, r1}
    stmia r3!, {r0, r1}
    sub r2, r2, #1
    bne @copy_init
    mov r2, #0
    add r1, sp, #0
@outer_loop:
    lsl r0, r2, #2
    ldr r3, [r1, r0]
    ldr r5, =0x0238dbf8
    ldr r4, [r3, #0xc]
    lsl r0, r2, #3
    mov r7, #0
    add r6, r5, r0
    str r7, [r5, r0]
    add r0, r7, #0
    add r3, #0x20
    str r0, [r6, #4]
    add r5, r7, #0
    b @inner_check
@inner_body:
    ldr r0, [r3, #0]
    ldr r7, =0x31464e49
    cmp r0, r7
    beq @case_inf1
    ldr r7, =0x31544144
    cmp r0, r7
    beq @case_dat1
    b @inner_next
@case_inf1:
    add r0, r3, #0
    add r0, #0x10
    str r0, [r6, #0]
    b @inner_next
@case_dat1:
    add r0, r3, #0
    add r0, #8
    str r0, [r6, #4]
@inner_next:
    ldr r0, [r3, #4]
    add r5, r5, #1
    add r3, r3, r0
@inner_check:
    cmp r5, r4
    blt @inner_body
    add r2, r2, #1
    cmp r2, #6
    blt @outer_loop
    ldr r0, =0x02fffce4
    ldrh r0, [r0, #0]
    lsl r0, r0, #29
    lsr r1, r0, #29
    ldr r0, =0x0238dbf4
    str r1, [r0, #0]
    add sp, #0x18
    pop {r3, r4, r5, r6, r7, pc}
}
