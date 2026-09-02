//cpp
// decomp: module=unk_autoload_0 addr=0x02321aa0 name=FUN_02321aa0
// flags: -thumb
#pragma thumb on

extern "C" asm unsigned short *FUN_02321aa0(int arg0, int arg1) {
    push {r3, r4, r5, r6, r7, lr}
    lsl r3, r1, #3
    ldr r1, =0x0238dbfc
    lsl r0, r0, #16
    ldr r2, [r1, r3]
    ldr r1, =0x0238dbf8
    lsr r0, r0, #14
    ldr r1, [r1, r3]
    ldr r5, =0x0238dc28
    ldr r0, [r1, r0]
    mov r1, #0x3f
    add r4, r2, r0
    mov r0, #0x0f
    b @check
@loop:
    cmp r2, #0x80
    bhs @ge_80
    strh r2, [r5, #0]
    add r4, r4, #1
    b @advance
@ge_80:
    cmp r2, #0xe0
    blo @case_2byte
    ldrb r3, [r4, #1]
    lsl r2, r2, #28
    lsr r6, r2, #24
    asr r2, r3, #2
    and r2, r0
    orr r2, r6
    lsl r3, r3, #30
    lsr r6, r3, #24
    ldrb r3, [r4, #2]
    lsl r2, r2, #8
    add r4, r4, #3
    and r3, r1
    orr r3, r6
    orr r2, r3
    strh r2, [r5, #0]
    b @advance
@case_2byte:
    mov r3, #0xe0
    and r3, r2
    cmp r3, #0xc0
    bne @advance
    ldrb r7, [r4, #1]
    mov r3, #0x1c
    and r3, r2
    lsl r2, r2, #30
    mov r6, #0x3f
    lsr r2, r2, #24
    and r6, r7
    asr r3, r3, #2
    lsl r2, r2, #16
    lsl r6, r6, #16
    lsl r3, r3, #16
    lsr r2, r2, #16
    lsr r6, r6, #16
    lsr r3, r3, #8
    orr r2, r6
    orr r2, r3
    strh r2, [r5, #0]
    add r4, r4, #2
@advance:
    add r5, r5, #2
@check:
    ldrb r2, [r4, #0]
    cmp r2, #0
    bne @loop
    mov r0, #0
    strh r0, [r5, #0]
    ldr r0, =0x0238dc28
    pop {r3, r4, r5, r6, r7, pc}
}
