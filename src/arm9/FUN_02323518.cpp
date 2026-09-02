//cpp
// decomp: module=unk_autoload_0 addr=0x02323518 name=FUN_02323518
// flags: -thumb
#pragma thumb on
extern "C" void FUN_0232344c(int);
extern "C" void FUN_023206f8(int);

extern "C" asm void FUN_02323518(int arg0) {
    push {r4, r5, lr}
    sub sp, #0x14
    ldr r5, =0x0233a050
    add r2, r0, #0
    add r4, sp, #0
    mov r3, #2
@copy_loop:
    ldmia r5!, {r0, r1}
    stmia r4!, {r0, r1}
    sub r3, r3, #1
    bne @copy_loop
    ldr r0, [r5, #0]
    str r0, [r4, #0]
    ldr r0, =0x02fffce4
    ldrh r0, [r0, #0]
    lsl r0, r0, #0x1d
    lsr r0, r0, #0x1d
    bne @branch_calc
    mov r0, #1
    str r0, [sp, #0]
    mov r0, #0
    str r0, [sp, #4]
    mov r0, #2
    str r0, [sp, #8]
@branch_calc:
    lsl r1, r2, #2
    add r0, sp, #0
    ldr r0, [r0, r1]
    bl FUN_0232344c
    mov r0, #0x14
    bl FUN_023206f8
    add sp, #0x14
    pop {r4, r5, pc}
}
