//cpp
// decomp: module=unk_autoload_0 addr=0x02322c14 name=FUN_02322c14
// flags: -thumb
#pragma thumb on
extern "C" asm void FUN_02322c14(void) {
    push {r4, r5, r6, lr}
    ldr r0, =0x0238ee3c
    ldr r0, [r0, #0]
    cmp r0, #0
    beq @ret
    mov r1, #0
    add r2, r1, #0
    sub r3, r1, #1
    ldr r0, =0x0238ee44
    mov r4, #0x30
    b @check_loop
@find_loop:
    add r5, r1, #0
    mul r5, r4
    add r6, r0, r5
    ldr r5, [r6, #0x24]
    cmp r5, #0
    beq @found_free
    ldr r5, [r6, #0x20]
    cmp r3, r5
    bls @next_chan
    add r3, r5, #0
    add r2, r1, #0
@next_chan:
    add r1, r1, #1
    lsl r1, r1, #0x18
    lsr r1, r1, #0x18
@check_loop:
    cmp r1, #4
    blo @find_loop
@found_free:
    cmp r1, #4
    bne @use_chan
    add r1, r2, #0
@use_chan:
    mov r0, #0x30
    mul r0, r1
    ldr r2, =0x0238ee44
    mov r1, #1
    add r0, r2, r0
    str r1, [r0, #0x24]
    mov r2, #0
    add r1, r0, #0
    strh r2, [r0, #0x28]
    add r1, #0x2c
    strb r2, [r1, #0]
    ldr r2, =0x0238ee3c
    mov r1, #0xf
    ldr r3, [r2, #4]
    and r1, r3
    add r4, r1, #3
    add r1, r0, #0
    add r1, #0x2d
    strb r4, [r1, #0]
    str r3, [r0, #0x20]
    ldr r3, [r2, #0]
    add r2, r0, #4
    mov r1, #0x1a
@copy_loop:
    ldrb r0, [r3, #0]
    add r3, r3, #1
    strb r0, [r2, #0]
    add r2, r2, #1
    sub r1, r1, #1
    bne @copy_loop
    ldr r0, =0x0238ee3c
    mov r1, #0
    str r1, [r0, #0]
@ret:
    pop {r4, r5, r6, pc}
}
