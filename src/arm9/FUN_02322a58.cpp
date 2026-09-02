//cpp
// decomp: module=unk_autoload_0 addr=0x02322a58 name=FUN_02322a58
// flags: -thumb
#pragma thumb on
extern "C" void FUN_023342e4(void *, void *, int);

extern "C" asm void FUN_02322a58(void) {
    push {r3, lr}
    sub sp, #0x20
    ldr r0, =0x0238ede0
    ldr r1, [r0, #8]
    add r1, r1, #1
    str r1, [r0, #8]
    cmp r1, #0xa
    ble @skip_inc
    mov r1, #0
    str r1, [r0, #8]
    ldr r1, [r0, #0xc]
    add r2, r1, #1
    str r2, [r0, #0xc]
    ldr r1, [r0, #4]
    cmp r2, r1
    bne @check_max
    add r1, r2, #1
    str r1, [r0, #0xc]
@check_max:
    ldr r0, =0x0238ede0
    ldr r1, [r0, #0xc]
    cmp r1, #0xf
    ble @skip_inc
    mov r1, #3
    str r1, [r0, #0xc]
@skip_inc:
    ldr r2, =0x0238ede0
    ldr r1, =0x0233ae48
    ldr r2, [r2, #0xc]
    add r0, sp, #0
    bl FUN_023342e4
    ldr r2, =0x0238ede0
    ldr r1, =0x0233ae58
    ldr r2, [r2, #4]
    add r0, sp, #0
    bl FUN_023342e4
    add sp, #0x20
    pop {r3, pc}
}
