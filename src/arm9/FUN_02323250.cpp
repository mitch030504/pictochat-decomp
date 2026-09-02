//cpp
// decomp: module=unk_autoload_0 addr=0x02323250 name=FUN_02323250
// flags: -thumb
#pragma thumb on
extern "C" void FUN_023371d4(void *, void *, int);
extern "C" void FUN_023371c0(int, void *, int);

extern "C" asm void FUN_02323250(void *arg0, void *arg1, void *arg2) {
    push {r3, r4, r5, r6, r7, lr}
    add r5, r0, #0
    str r1, [sp, #0]
    add r4, r2, #0
    mov r7, #0
@outer_loop:
    mov r6, #0
@inner_loop:
    ldrb r0, [r4, #0]
    mov r2, #0x20
    lsl r1, r0, #5
    ldr r0, [sp, #0]
    add r0, r0, r1
    add r1, r5, #0
    blx FUN_023371d4
    add r0, r6, #1
    lsl r0, r0, #0x10
    lsr r6, r0, #0x10
    add r5, #0x20
    add r4, r4, #1
    cmp r6, #0x19
    blo @inner_loop
    add r0, r7, #1
    lsl r0, r0, #0x10
    lsr r7, r0, #0x10
    cmp r7, #0xb
    blo @outer_loop
    mov r0, #0
    add r1, r5, #0
    mov r2, #0x20
    blx FUN_023371c0
    pop {r3, r4, r5, r6, r7, pc}
}
