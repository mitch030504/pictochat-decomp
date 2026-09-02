//cpp
// decomp: module=unk_autoload_0 addr=0x02321f54 name=FUN_02321f54
// flags: -thumb
#pragma thumb on
extern "C" void FUN_0232109c(void *);

extern "C" asm void FUN_02321f54(int arg0, int arg1, int arg2) {
    push {r3, r4, r5, r6, r7, lr}
    mov r5, #0
    add r7, r0, #0
    add r0, r1, #0
    add r6, r2, #0
    add r3, r5, #0
@loop:
    mov r1, #0x54
    add r2, r5, #0
    mul r2, r1
    ldr r1, =0x0238e134
    add r4, r1, r2
    mov r1, #0
    mvn r1, r1
    str r1, [r4, #4]
    ldr r1, =0x0233a004
    lsl r2, r5, #2
    ldr r1, [r1, r2]
    str r3, [r4, #0x10]
    tst r1, r6
    beq @case_eq
    mov r1, #1
    str r1, [r4, #0xc]
    str r3, [r4, #8]
    b @next
@case_eq:
    str r3, [r4, #0xc]
    mov r1, #0x18
    str r1, [r4, #8]
@next:
    add r5, r5, #1
    cmp r5, #2
    blt @loop
    ldr r1, =0x0238e11c
    str r7, [r1, #0x18]
    str r0, [r1, #0x6c]
    mov r0, #1
    str r6, [r1, #0]
    tst r0, r6
    beq @flag_zero
    mov r0, #0
    b @store_flags
@flag_zero:
    mov r0, #0x18
@store_flags:
    str r0, [r1, #4]
    ldr r0, =0x0238e11c
    mov r1, #1
    str r1, [r0, #8]
    ldr r0, =0x0238e128
    bl FUN_0232109c
    pop {r3, r4, r5, r6, r7, pc}
}
