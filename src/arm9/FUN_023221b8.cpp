//cpp
// decomp: module=unk_autoload_0 addr=0x023221b8 name=FUN_023221b8
// flags: -thumb
#pragma thumb on
extern "C" int FUN_02321144(void *, int);
extern "C" int FUN_0232103c(int);

extern "C" asm int FUN_023221b8(int arg0) {
    push {r4, r5, r6, lr}
    mov r1, #0x54
    ldr r2, =0x0238e134
    mul r1, r0
    add r1, r2, r1
    ldr r2, =0x0233a004
    lsl r3, r0, #2
    ldr r4, [r2, r3]
    ldr r2, =0x0238e11c
    ldr r3, [r2, #8]
    cmp r3, #0
    beq @set_0
    ldr r2, [r2, #0]
    tst r2, r4
    beq @set_0
    ldr r1, [r1, #8]
    cmp r1, #0
    bne @set_0
    mov r5, #1
    b @do_check
@set_0:
    mov r5, #0
@do_check:
    ldr r1, =0x0233a010
    lsl r4, r0, #3
    ldrh r1, [r1, r4]
    ldr r0, =0x0238e128
    bl FUN_02321144
    add r6, r0, #0
    ldr r0, =0x0233a012
    ldrh r0, [r0, r4]
    bl FUN_0232103c
    cmp r5, #0
    beq @ret_0
    cmp r6, #0
    bne @ret_1
    cmp r0, #0
    beq @ret_0
@ret_1:
    mov r0, #1
    pop {r4, r5, r6, pc}
@ret_0:
    mov r0, #0
    pop {r4, r5, r6, pc}
}
