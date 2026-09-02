//cpp
// decomp: module=unk_autoload_0 addr=0x02322c9c name=FUN_02322c9c
// flags: -thumb
#pragma thumb on
extern "C" void FUN_02322dc0(int);
extern "C" void FUN_0232e6cc(void *, int);
extern "C" void FUN_0232fbc0(void *, int, int, int);
extern "C" int FUN_02335d98(int, int);
extern "C" void FUN_0232e76c(void *, int, int);
extern "C" void FUN_0232e780(void *, int, int);

extern "C" asm void FUN_02322c9c(void) {
    push {r3, r4, r5, r6, r7, lr}
    sub sp, #8
    ldr r7, =0x0238ee44
    mov r6, #0
@loop:
    mov r0, #0x30
    mul r0, r6
    add r5, r7, r0
    ldr r0, [r5, #0x24]
    cmp r0, #0
    beq @next
    ldrh r0, [r5, #0x28]
    cmp r0, #0
    bne @dec_delay
    add r0, r5, #0
    add r0, #0x2c
    ldrb r0, [r0, #0]
    cmp r0, #0
    bne @check_limit
    ldrb r0, [r5, #5]
    strh r0, [r5, #0x2a]
@check_limit:
    add r0, r5, #0
    add r0, #0x2c
    ldrb r0, [r0, #0]
    cmp r0, #0x11
    blo @inc_step
    mov r4, #0x1b
    b @check_note
@inc_step:
    add r1, r5, #0
    add r1, #0x2c
    ldrb r1, [r1, #0]
    add r2, r1, #1
    add r1, r5, #0
    add r1, #0x2c
    strb r2, [r1, #0]
    add r1, r5, r0
    mov r0, #0xa
    ldrsb r4, [r1, r0]
@check_note:
    cmp r4, #0x18
    bgt @check_command
    add r0, r6, #0
    bl FUN_02322dc0
    add r0, r5, #0
    mov r1, #0
    bl FUN_0232e6cc
    mov r0, #0
    str r0, [sp, #0]
    str r0, [sp, #4]
    add r0, r5, #0
    add r1, r6, #0
    mov r2, #0
    mov r3, #0x60
    bl FUN_0232fbc0
    ldrb r1, [r5, #8]
    mov r0, #1
    tst r0, r1
    beq @set_vol
    add r1, r5, #0
    ldr r0, =0x0238ee3c
    add r1, #0x2d
    ldrb r1, [r1, #0]
    ldr r0, [r0, #4]
    blx FUN_02335d98
    mov r0, #3
    and r0, r1
    add r1, r5, #0
    add r1, #0x2d
    sub r0, r0, #2
    ldrb r1, [r1, #0]
    lsl r0, r0, #0x18
    asr r0, r0, #0x18
    add r2, r1, #1
    add r1, r5, #0
    add r0, r4, r0
    add r1, #0x2d
    lsl r0, r0, #0x18
    strb r2, [r1, #0]
    asr r4, r0, #0x18
    bpl @check_vol_high
    mov r4, #0
    b @set_vol
@check_vol_high:
    cmp r4, #0x18
    ble @set_vol
    mov r4, #0x18
@set_vol:
    add r0, r5, #0
    mov r1, #1
    lsl r2, r4, #6
    bl FUN_0232e76c
    ldrb r1, [r5, #8]
    mov r0, #4
    tst r0, r1
    beq @next
    add r0, r5, #0
    mov r1, #1
    mov r2, #0x28
    bl FUN_0232e780
    b @next
@check_command:
    cmp r4, #0x1a
    bne @check_stop
    add r0, r6, #0
    bl FUN_02322dc0
    add r0, r5, #0
    mov r1, #0
    bl FUN_0232e6cc
    b @next
@check_stop:
    cmp r4, #0x19
    bne @check_end
    add r0, r6, #0
    bl FUN_02322dc0
    b @next
@check_end:
    cmp r4, #0x1b
    bne @next
    mov r0, #0
    str r0, [r5, #0x24]
    add r0, r5, #0
    mov r1, #0
    bl FUN_0232e6cc
    b @next
@dec_delay:
    sub r0, r0, #1
    strh r0, [r5, #0x28]
@next:
    add r0, r6, #1
    lsl r0, r0, #0x18
    lsr r6, r0, #0x18
    cmp r6, #4
    bhs @done
    b @loop
@done:
    ldr r0, =0x0238ee3c
    ldr r1, [r0, #4]
    add r1, r1, #1
    str r1, [r0, #4]
    add sp, #8
    pop {r3, r4, r5, r6, r7, pc}
}
