//cpp
// decomp: module=unk_autoload_0 addr=0x02322bcc name=FUN_02322bcc
// flags: -thumb
#pragma thumb on
extern "C" asm int FUN_02322bcc(void *arg0) {
    ldrb r2, [r0, #1]
    mov r1, #0
    cmp r2, #0
    beq @ret_zero
    cmp r2, #0x30
    bls @loop_init
@ret_zero:
    mov r0, #0
    bx lr
@loop_init:
    add r2, r1, #0
    add r2, r0, r2
    ldrb r2, [r2, #6]
    add r1, r1, #1
    lsl r1, r1, #0x18
    lsr r1, r1, #0x18
    cmp r2, #0x1b
    beq @found_break
    cmp r1, #0x11
    blo @loop_init
@found_break:
    mov r3, #0
    b @check_sub
@loop_sub:
    add r2, r0, r3
    ldrb r2, [r2, #6]
    cmp r2, #0x1d
    blo @next_sub
    mov r0, #0
    bx lr
@next_sub:
    add r2, r3, #1
    lsl r2, r2, #0x18
    lsr r3, r2, #0x18
@check_sub:
    cmp r3, r1
    blo @loop_sub
    cmp r1, #0x11
    blo @done
    mov r1, #0
@done:
    add r0, r1, #0
    bx lr
}
