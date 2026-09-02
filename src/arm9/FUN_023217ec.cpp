//cpp
// decomp: module=unk_autoload_0 addr=0x023217ec name=FUN_023217ec
// flags: -thumb
#pragma thumb on

extern "C" void FUN_0232dd28(void *, int);
extern "C" void FUN_02321a84(void);
extern "C" void FUN_0232164c(void *, int, int *, int *, int *, int *, int);
extern "C" void FUN_0232de2c(void *, void *);
extern "C" void FUN_023215e4(int, int);
extern "C" void FUN_02321518(unsigned int, int *, int *);
extern "C" void FUN_02335b8c(int, int);
extern "C" void FUN_023216c0(void *, void *, int, int);

extern "C" asm void FUN_023217ec(void *arg0, void *arg1, void *arg2, int arg3) {
    push {r3, r4, r5, r6, r7, lr}
    sub sp, #0x50
    add r4, r1, #0
    ldr r1, =0x02339fe4
    add r5, r0, #0
    add r7, r2, #0
    ldr r0, [sp, #0x74]
    ldrh r2, [r1, #0]
    str r0, [sp, #0x74]
    add r0, sp, #0x34
    ldrh r1, [r1, #2]
    strh r2, [r0, #0]
    str r3, [sp, #0x20]
    strh r1, [r0, #2]
    ldr r0, [sp, #0x68]
    ldr r1, =0x0000ffff
    cmp r0, r1
    bne @use_1a84
    add r0, r7, #0
    add r1, r3, #0
    bl FUN_0232dd28
    ldr r0, [r0, #8]
    cmp r0, #0
    beq @fallback_34_1
    add r0, r7, r0
    str r0, [sp, #0x24]
    b @done_target
@fallback_34_1:
    add r0, sp, #0x34
    str r0, [sp, #0x24]
    b @done_target
@use_1a84:
    bl FUN_02321a84
    str r0, [sp, #0x24]
    ldrh r0, [r0, #0]
    cmp r0, #0
    bne @done_target
    add r0, sp, #0x34
    str r0, [sp, #0x24]
@done_target:
    ldr r0, [sp, #0x6c]
    cmp r0, #0
    beq @skip_6c
    str r0, [sp, #0x24]
@skip_6c:
    add r0, sp, #0x3c
    str r0, [sp, #0]
    add r0, sp, #0x38
    str r0, [sp, #4]
    ldr r0, [sp, #0x24]
    ldr r1, [sp, #0x20]
    str r0, [sp, #8]
    add r0, r7, #0
    add r2, sp, #0x44
    add r3, sp, #0x40
    bl FUN_0232164c
    str r5, [r4, #0x24]
    ldr r0, [sp, #0x68]
    str r7, [r4, #0x20]
    strh r0, [r4, #0x1c]
    ldr r0, [sp, #0x20]
    mov r1, #0
    strh r0, [r4, #0x1e]
    add r0, r4, #0
    add r0, #0x28
    strb r1, [r0, #0]
    ldr r1, [sp, #0x20]
    add r0, r7, #0
    bl FUN_0232dd28
    ldrh r0, [r0, #6]
    lsl r0, r0, #20
    lsr r0, r0, #28
    strh r0, [r4, #0x16]
    ldr r0, [sp, #0x44]
    strh r0, [r4, #8]
    ldr r0, [sp, #0x40]
    strh r0, [r4, #0xa]
    ldr r0, [sp, #0x3c]
    lsl r0, r0, #24
    lsr r0, r0, #24
    strh r0, [r4, #0xc]
    ldr r0, [sp, #0x38]
    lsl r0, r0, #24
    lsr r0, r0, #24
    strh r0, [r4, #0xe]
    ldr r0, [sp, #0x70]
    cmp r0, #0
    beq @case_no_70
    add r0, r5, #0
    add r1, r4, #0
    bl FUN_0232de2c
    ldrh r0, [r5, #0xe]
    strh r0, [r4, #0x14]
    ldr r0, [sp, #0x3c]
    ldr r1, [sp, #0x38]
    bl FUN_023215e4
    str r0, [r4, #0x10]
    add r1, sp, #0x4c
    add r2, sp, #0x48
    bl FUN_02321518
    ldr r0, [sp, #0x3c]
    ldr r1, [sp, #0x4c]
    sub r0, r0, #1
    blx FUN_02335b8c
    add r0, r0, #1
    strh r0, [r4, #0x18]
    ldr r0, [sp, #0x38]
    ldr r1, [sp, #0x48]
    sub r0, r0, #1
    blx FUN_02335b8c
    add r0, r0, #1
    strh r0, [r4, #0x1a]
    b @loop_init
@case_no_70:
    ldr r0, [r4, #0x10]
    add r1, sp, #0x4c
    add r2, sp, #0x48
    bl FUN_02321518
@loop_init:
    ldrh r0, [r4, #0x14]
    str r0, [sp, #0x30]
    mov r0, #0
    str r0, [sp, #0x28]
    b @outer_check
@outer_body:
    mov r0, #0
    str r0, [sp, #0x2c]
    b @inner_check
@inner_body:
    ldr r1, [sp, #0x4c]
    ldr r0, [sp, #0x48]
    mov ip, r1
    ldr r1, [sp, #0x28]
    ldr r3, [sp, #0x2c]
    mul r1, r0
    str r1, [sp, #0]
    mov r1, ip
    str r1, [sp, #4]
    str r0, [sp, #8]
    ldr r0, [sp, #0x30]
    mov r6, ip
    str r0, [sp, #0xc]
    ldr r0, [sp, #0x44]
    ldr r2, [sp, #0x20]
    str r0, [sp, #0x10]
    ldr r0, [sp, #0x40]
    add r1, r7, #0
    str r0, [sp, #0x14]
    ldr r0, [sp, #0x24]
    mul r3, r6
    str r0, [sp, #0x18]
    ldr r0, [sp, #0x74]
    str r0, [sp, #0x1c]
    add r0, r5, #0
    bl FUN_023216c0
    ldr r1, [sp, #0x30]
    add r0, r1, r0
    str r0, [sp, #0x30]
    ldr r0, [sp, #0x2c]
    add r0, r0, #1
    str r0, [sp, #0x2c]
@inner_check:
    ldrh r1, [r4, #0x18]
    ldr r0, [sp, #0x2c]
    cmp r0, r1
    blt @inner_body
    ldr r0, [sp, #0x28]
    add r0, r0, #1
    str r0, [sp, #0x28]
@outer_check:
    ldrh r1, [r4, #0x1a]
    ldr r0, [sp, #0x28]
    cmp r0, r1
    blt @outer_body
    ldr r0, [sp, #0x70]
    cmp r0, #0
    beq @done
    ldr r0, [sp, #0x30]
    strh r0, [r5, #0xe]
@done:
    add sp, #0x50
    pop {r3, r4, r5, r6, r7, pc}
}
