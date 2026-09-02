//cpp
// decomp: module=unk_autoload_0 addr=0x02322dc0 name=FUN_02322dc0
// flags: -thumb
#pragma thumb on
extern "C" int FUN_02335b8c(int, int);

extern "C" asm void FUN_02322dc0(int arg0) {
    push {r3, r4, r5, lr}
    mov r1, #0x30
    mul r1, r0
    ldr r2, =0x0238ee44
    mov r0, #2
    add r4, r2, r1
    ldrb r1, [r4, #8]
    ldrh r5, [r4, #0x2a]
    tst r0, r1
    beq @no_mod
    add r0, r4, #0
    add r0, #0x2c
    ldrb r0, [r0, #0]
    lsr r2, r0, #0x1f
    lsl r1, r0, #0x1f
    sub r1, r1, r2
    mov r0, #0x1f
    ror r1, r0
    add r0, r2, r1
    beq @neg_step
    add r0, r5, #0
    mov r1, #3
    blx FUN_02335b8c
    add r0, r5, r0
    strh r0, [r4, #0x28]
    pop {r3, r4, r5, pc}
@neg_step:
    add r0, r5, #0
    mov r1, #3
    blx FUN_02335b8c
    sub r0, r5, r0
    strh r0, [r4, #0x28]
    pop {r3, r4, r5, pc}
@no_mod:
    strh r5, [r4, #0x28]
    pop {r3, r4, r5, pc}
}
