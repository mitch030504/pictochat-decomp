//cpp
// decomp: module=unk_autoload_0 addr=0x0232200c name=FUN_0232200c
// flags: -thumb
#pragma thumb on
extern "C" int FUN_023209fc(void);
extern "C" int FUN_02321168(void *, int);
extern "C" void FUN_02320e48(void *, int, int, int);
extern "C" void FUN_02320be0(int, void *, void *, int);
extern "C" void FUN_02320b94(int, void *, void *, int);

extern "C" asm void FUN_0232200c(void) {
    push {r3, r4, r5, r6, r7, lr}
    sub sp, #0x10
    bl FUN_023209fc
    add r7, r0, #0
    mov r5, #0
@loop:
    mov r0, #0x54
    add r1, r5, #0
    mul r1, r0
    ldr r0, =0x0238e134
    add r4, r0, r1
    ldr r0, [r4, #0xc]
    cmp r0, #0
    beq @loop_next
    ldr r1, [r4, #8]
    cmp r1, #0x18
    beq @loop_next
    ldr r0, =0x0238e11c
    ldr r0, [r0, #4]
    add r6, r0, r1
    ldr r0, =0x0238e11c
    ldr r0, [r0, #8]
    cmp r0, #0
    beq @mode_0
    ldr r1, =0x0233a00c
    lsl r2, r5, #3
    add r1, r1, r2
    ldrh r1, [r1, #4]
    ldr r0, =0x0238e128
    bl FUN_02321168
    str r0, [sp, #0xc]
    b @do_e48
@mode_0:
    mov r0, #2
    str r0, [sp, #0xc]
@do_e48:
    mov r0, #0
    mov r1, #0
    add r4, #0x14
    str r0, [sp, #0]
    add r0, r4, #0
    add r2, r6, #0
    add r3, r1, #0
    bl FUN_02320e48
    mov r0, #0
    str r0, [sp, #0]
    ldr r0, [sp, #0xc]
    str r6, [sp, #4]
    str r0, [sp, #8]
    ldr r3, =0x0233a00c
    lsl r4, r5, #3
    ldrh r3, [r3, r4]
    ldr r1, =0x02349a1c
    ldr r2, =0x0233b574
    add r0, r7, #0
    bl FUN_02320be0
@loop_next:
    add r5, r5, #1
    cmp r5, #2
    blt @loop
    mov r3, #0
    ldr r0, =0x0238e11c
    str r3, [sp, #0]
    ldr r0, [r0, #4]
    ldr r1, =0x02349a1c
    neg r0, r0
    str r0, [sp, #4]
    ldr r2, =0x0233b574
    add r0, r7, #0
    bl FUN_02320b94
    mov r0, #0
    str r0, [sp, #0]
    ldr r0, =0x0238e11c
    ldr r1, =0x02349a1c
    ldr r0, [r0, #4]
    ldr r2, =0x0233b574
    str r0, [sp, #4]
    add r0, r7, #0
    mov r3, #1
    bl FUN_02320b94
    add sp, #0x10
    pop {r3, r4, r5, r6, r7, pc}
}
