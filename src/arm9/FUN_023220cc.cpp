//cpp
// decomp: module=unk_autoload_0 addr=0x023220cc name=FUN_023220cc
// flags: -thumb
#pragma thumb on
extern "C" void FUN_02321404(void *, void *, int);
extern "C" void FUN_023210a8(void *, void *);

extern "C" asm void FUN_023220cc(void) {
    push {r3, r4, r5, r6, r7, lr}
    ldr r1, =0x0238e11c
    mov r0, #1
    ldr r2, [r1, #0]
    tst r0, r2
    ldr r0, [r1, #4]
    beq @case_eq1
    cmp r0, #0
    beq @after_header
    sub r0, r0, #2
    str r0, [r1, #4]
    b @after_header
@case_eq1:
    cmp r0, #0x18
    beq @after_header
    add r0, r0, #2
    str r0, [r1, #4]
@after_header:
    ldr r7, =0x0233a004
    ldr r6, =0x0238e134
    mov r4, #0
@loop:
    mov r0, #0x54
    mul r0, r4
    add r5, r6, r0
    ldr r0, [r5, #0x10]
    cmp r0, #0
    ldr r0, [r5, #8]
    beq @check_sub
    cmp r0, #0x18
    beq @at_18
    add r0, r0, #2
    str r0, [r5, #8]
    b @advance
@at_18:
    ldr r0, =0x0238e11c
    ldr r1, [r0, #0]
    lsl r0, r4, #2
    ldr r0, [r7, r0]
    tst r0, r1
    beq @tst_zero
    mov r0, #1
    b @set_c
@tst_zero:
    mov r0, #0
@set_c:
    str r0, [r5, #0xc]
    mov r0, #0
    ldr r2, [r5, #4]
    mvn r0, r0
    cmp r2, r0
    beq @skip_404
    add r0, r5, #0
    ldr r1, =0x0233b80c
    add r0, #0x14
    str r2, [r5, #0]
    bl FUN_02321404
    mov r0, #0
    mvn r0, r0
    str r0, [r5, #4]
@skip_404:
    mov r0, #0
    str r0, [r5, #0x10]
    b @advance
@check_sub:
    cmp r0, #0
    beq @advance
    sub r0, r0, #2
    str r0, [r5, #8]
@advance:
    add r4, r4, #1
    cmp r4, #2
    blt @loop
    ldr r0, =0x0238e128
    ldr r1, =0x0233b4b0
    bl FUN_023210a8
    pop {r3, r4, r5, r6, r7, pc}
}
