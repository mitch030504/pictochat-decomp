//cpp
// decomp: module=unk_autoload_0 addr=0x023231e4 name=FUN_023231e4
// flags: -thumb
#pragma thumb on
extern "C" void FUN_0233718c(void *, void *, int);
extern "C" void FUN_02331268(void *, int);

extern "C" asm void FUN_023231e4(void *arg0, int arg1, int arg2) {
    push {r3, r4, r5, r6, r7, lr}
    add r7, r0, #0
    str r2, [sp, #0]
    mov r2, #2
    add r4, r1, #0
    ldr r0, =0x00000113
    add r1, r7, #0
    lsl r2, r2, #0xa
    blx FUN_0233718c
    mov r3, #0
    lsl r4, r4, #0xc
@outer_loop:
    mov r0, #0x19
    add r1, r3, #0
    mul r1, r0
    ldr r0, [sp, #0]
    mov r2, #0
    add r5, r0, r1
    lsl r0, r3, #6
    add r6, r7, r0
@inner_loop:
    add r0, r2, r5
    add r1, r4, #0
    orr r1, r0
    lsl r0, r2, #1
    add r2, r2, #1
    strh r1, [r6, r0]
    cmp r2, #0x19
    blt @inner_loop
    add r3, r3, #1
    cmp r3, #0xb
    blt @outer_loop
    mov r1, #2
    add r0, r7, #0
    lsl r1, r1, #0xa
    blx FUN_02331268
    pop {r3, r4, r5, r6, r7, pc}
}
