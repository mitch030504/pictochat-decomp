//cpp
// decomp: module=unk_autoload_0 addr=0x02321518 name=FUN_02321518
// flags: -thumb
#pragma thumb on

extern "C" asm void FUN_02321518(unsigned int format, int *w, int *h) {
    push {r3, r4, r5, lr}
    ldr r4, =0xc0008000
    cmp r0, r4
    bgt @gt_c0008000
    bge @case_32_64
    ldr r5, =0x80008000
    cmp r0, r5
    bgt @gt_80008000
    bge @case_16_32
    lsl r4, r5, #16
    cmp r0, r4
    bgt @gt_80000000
    beq @case_32_32
    b @store
@gt_80000000:
    ldr r4, =0x80004000
    cmp r0, r4
    beq @case_32_16
    b @store
@gt_80008000:
    mov r4, #3
    lsl r4, r4, #30
    cmp r0, r4
    bgt @gt_c0000000
    beq @case_64_64
    b @store
@gt_c0000000:
    asr r4, r5, #1
    cmp r0, r4
    beq @case_64_32
    b @store
@gt_c0008000:
    mov r4, #2
    lsl r4, r4, #14
    cmp r0, r4
    bgt @gt_00008000
    bge @case_8_16
    cmp r0, #0
    bgt @gt_0
    beq @case_8_8
    b @store
@gt_0:
    lsr r4, r4, #1
    cmp r0, r4
    beq @case_16_8
    b @store
@gt_00008000:
    ldr r4, =0x40004000
    cmp r0, r4
    bgt @gt_40004000
    bge @case_32_8
    lsl r4, r4, #16
    cmp r0, r4
    beq @case_16_16
    b @store
@gt_40004000:
    ldr r4, =0x40008000
    cmp r0, r4
    beq @case_8_32
    b @store

@case_8_8:
    mov r0, #8
    add r3, r0, #0
    b @store
@case_16_16:
    mov r0, #16
    add r3, r0, #0
    b @store
@case_32_32:
    mov r0, #32
    add r3, r0, #0
    b @store
@case_64_64:
    mov r0, #64
    add r3, r0, #0
    b @store
@case_16_8:
    mov r0, #16
    mov r3, #8
    b @store
@case_32_8:
    mov r0, #32
    mov r3, #8
    b @store
@case_32_16:
    mov r0, #32
    mov r3, #16
    b @store
@case_64_32:
    mov r0, #64
    mov r3, #32
    b @store
@case_8_16:
    mov r0, #8
    mov r3, #16
    b @store
@case_8_32:
    mov r0, #8
    mov r3, #32
    b @store
@case_16_32:
    mov r0, #16
    mov r3, #32
    b @store
@case_32_64:
    mov r0, #32
    mov r3, #64
@store:
    str r0, [r1]
    str r3, [r2]
    pop {r3, r4, r5, pc}
}
