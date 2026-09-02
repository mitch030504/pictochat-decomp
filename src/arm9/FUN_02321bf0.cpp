//cpp
// decomp: module=unk_autoload_0 addr=0x02321bf0 name=FUN_02321bf0
// flags: -thumb
#pragma thumb on
extern "C" void FUN_023209fc(void);
extern "C" void FUN_02320b94(int, int, int, int);
extern "C" void FUN_02320e48(int, int, int, int);
extern "C" int FUN_02321168(int, int);
extern "C" void FUN_02320be0(int, int, int, int);

extern "C" asm void FUN_02321bf0(void) {
    push {r3, r4, lr}
    sub sp, #0xc
    ldr r0, =0x0238e028
    ldr r1, [r0, #0x14]
    cmp r1, #0
    blt @done
    ldr r0, [r0, #0x18]
    cmp r0, #2
    bne @skip_2
    bl FUN_023209fc
    ldr r1, =0x0238e028
    mov r3, #4
    ldr r2, [r1, #0xc]
    str r2, [sp, #0]
    ldr r1, [r1, #0]
    ldr r2, =0x0233b59c
    str r1, [sp, #4]
    ldr r1, =0x02349a1c
    bl FUN_02320b94
    ldr r0, =0x0238e028
    ldr r2, [r0, #0]
    cmp r2, #0x4c
    bgt @skip_4c_1
    mov r1, #0
    ldr r0, =0x0238e09c
    add r3, r1, #0
    str r1, [sp, #0]
    bl FUN_02320e48
@skip_4c_1:
    ldr r0, =0x0238e028
    ldr r2, [r0, #0]
    cmp r2, #0x4c
    bgt @skip_4c_2
    mov r1, #0
    ldr r0, =0x0238e0dc
    add r3, r1, #0
    str r1, [sp, #0]
    bl FUN_02320e48
@skip_4c_2:
    ldr r0, =0x0238e050
    mov r1, #1
    bl FUN_02321168
    add r4, r0, #0
    bl FUN_023209fc
    mov r1, #0
    str r1, [sp, #0]
    ldr r1, =0x0238e028
    ldr r2, =0x0233b59c
    ldr r1, [r1, #0]
    mov r3, #2
    str r1, [sp, #4]
    ldr r1, =0x02349a1c
    str r4, [sp, #8]
    bl FUN_02320be0
    ldr r0, =0x0238e050
    mov r1, #0
    bl FUN_02321168
    add r4, r0, #0
    bl FUN_023209fc
    mov r1, #0
    str r1, [sp, #0]
    ldr r1, =0x0238e028
    ldr r2, =0x0233b59c
    ldr r1, [r1, #0]
    mov r3, #3
    str r1, [sp, #4]
    ldr r1, =0x02349a1c
    str r4, [sp, #8]
    bl FUN_02320be0
@skip_2:
    mov r1, #0
    ldr r2, =0x0238e028
    str r1, [sp, #0]
    ldr r3, [r2, #0]
    ldr r2, [r2, #4]
    ldr r0, =0x0238e05c
    add r2, r3, r2
    add r3, r1, #0
    bl FUN_02320e48
    bl FUN_023209fc
    mov r1, #0
    str r1, [sp, #0]
    ldr r1, =0x0238e028
    mov r3, #1
    ldr r2, [r1, #0]
    ldr r1, [r1, #8]
    add r1, r2, r1
    str r1, [sp, #4]
    ldr r1, =0x02349a1c
    ldr r2, =0x0233b59c
    bl FUN_02320b94
    bl FUN_023209fc
    mov r3, #0
    ldr r1, =0x0238e028
    str r3, [sp, #0]
    ldr r2, [r1, #0]
    ldr r1, [r1, #4]
    add r1, r2, r1
    str r1, [sp, #4]
    ldr r1, =0x02349a1c
    ldr r2, =0x0233b59c
    bl FUN_02320b94
@done:
    add sp, #0xc
    pop {r3, r4, pc}
}
