//cpp
// decomp: module=unk_autoload_0 addr=0x02322624 name=FUN_02322624
// flags: -thumb
#pragma thumb on
extern "C" void FUN_0232d77c(void *, void *, int, void *);
extern "C" void FUN_02331268(void *, int);
extern "C" void FUN_023366c0(void *, int, int);
extern "C" void FUN_023365c0(void *, int, int);

extern "C" asm void FUN_02322624(void) {
    push {r3, r4, r5, r6, r7, lr}
    ldr r0, =0x02fffec0
    ldr r1, =0x0238e230
    ldr r3, =0x0238e5d0
    mov r2, #8
    bl FUN_0232d77c
    mov r1, #0x3a
    ldr r0, =0x0238e230
    lsl r1, r1, #4
    blx FUN_02331268
    mov r6, #0x1d
    ldr r4, =0x0238e230
    mov r5, #0
    mov r7, #0x20
    lsl r6, r6, #0xc
@loop_tile:
    lsl r0, r5, #5
    lsl r1, r5, #7
    add r0, r4, r0
    add r1, r1, r6
    add r2, r7, #0
    bl FUN_023366c0
    add r5, r5, #1
    cmp r5, #0x1a
    blt @loop_tile
    mov r1, #0x1f
    ldr r0, =0x0233a030
    lsl r1, r1, #4
    mov r2, #2
    bl FUN_023365c0
    mov r5, #0
@loop_outer:
    lsl r0, r5, #0x1b
    lsr r2, r0, #0x18
    mov r0, #0xd
    add r4, r5, #0
    mul r4, r0
    ldr r0, =0x0238e230
    lsl r3, r4, #3
    mov r1, #0
    add r3, r0, r3
@loop_inner:
    lsl r6, r1, #3
    add r0, r3, r6
    lsl r6, r6, #0x17
    lsr r6, r6, #7
    add r7, r6, #0
    mov r6, #0xaa
    orr r7, r2
    lsl r6, r6, #4
    str r7, [r0, r6]
    mov r6, #0x3a
    add r7, r1, r4
    lsl r6, r6, #4
    add r7, r7, r6
    mov r6, #0xf
    lsl r6, r6, #0xc
    orr r7, r6
    ldr r6, =0x00000aa4
    add r1, r1, #1
    strh r7, [r0, r6]
    cmp r1, #0xd
    blt @loop_inner
    add r5, r5, #1
    cmp r5, #2
    blt @loop_outer
    pop {r3, r4, r5, r6, r7, pc}
}
