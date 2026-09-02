//cpp
// decomp: module=unk_autoload_0 addr=0x02321ba4 name=FUN_02321ba4
// flags: -thumb
#pragma thumb on
extern "C" void FUN_023213b4(void *, void *, void *, int);
extern "C" void FUN_023213e4(void *, void *, void *, int);
extern "C" void FUN_02321404(void *, void *, int);

extern "C" asm void FUN_02321ba4(void *arg0) {
    push {r4, lr}
    ldr r1, =0x0238e05c
    ldr r2, =0x0233b8c4
    mov r3, #0x50
    add r4, r0, #0
    bl FUN_023213b4
    ldr r1, =0x0238e09c
    ldr r2, =0x0233b8c4
    add r0, r4, #0
    mov r3, #1
    bl FUN_023213e4
    ldr r1, =0x0238e0dc
    ldr r2, =0x0233b8c4
    add r0, r4, #0
    mov r3, #2
    bl FUN_023213e4
    ldr r0, =0x0238e09c
    ldr r1, =0x0233b8c4
    mov r2, #0x2d
    bl FUN_02321404
    ldr r0, =0x0238e0dc
    ldr r1, =0x0233b8c4
    mov r2, #0x2e
    bl FUN_02321404
    pop {r4, pc}
}
