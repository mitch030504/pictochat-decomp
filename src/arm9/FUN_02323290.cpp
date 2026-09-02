//cpp
// decomp: module=unk_autoload_0 addr=0x02323290 name=FUN_02323290
// flags: -thumb
#pragma thumb on
extern "C" void FUN_02323250(void *, void *, void *);
extern "C" void FUN_023371d4(void *, void *, int);
extern "C" void FUN_02323d00(void *, int, int, int, int);
extern "C" void FUN_0232344c(int);

extern "C" char unk_0238ef0c;
extern "C" char unk_0233aef4;
extern "C" char unk_0234984c;
extern "C" char unk_02349674;
extern "C" char unk_023494e4;
extern "C" char unk_0233af2c;
extern "C" char unk_02349674_dup;
extern "C" char unk_02349a1c;
extern "C" char unk_0234984c_dup;
extern "C" char unk_0233af6c;
extern "C" char unk_02343d44;
extern "C" char unk_0238ef50;
extern "C" char unk_0233b15c;
extern "C" char unk_02399bd0;
extern "C" char unk_0233b270;
extern "C" char unk_02397950;
extern "C" char unk_0233b384;
extern "C" char unk_023911d0;
extern "C" char unk_02fffce4;

extern "C" asm void FUN_02323290(int arg0, int arg1, int arg2) {
    push {r3, r4, lr}
    sub sp, #0xc
    lsl r0, r0, #0x10
    ldr r3, =unk_0238ef0c
    lsr r0, r0, #0x10
    str r0, [r3, #4]
    lsl r0, r1, #0x10
    lsr r0, r0, #0x10
    str r0, [r3, #8]
    mov r0, #0
    str r0, [r3, #0x3c]
    str r2, [r3, #0x40]
    ldr r0, [r3, #0]
    cmp r0, #0
    beq @init_setup
    b @check_trigger
@init_setup:
    ldr r0, =unk_0233aef4
    ldr r2, =unk_0234984c
    ldr r1, [r0, #0x18]
    str r1, [r0, #8]
    str r1, [r0, #0]
    str r1, [r0, #0x10]
    ldr r1, [r0, #0x1c]
    str r1, [r0, #0x14]
    ldr r1, [r0, #0x20]
    str r1, [r0, #4]
    ldr r1, [r0, #0x24]
    str r1, [r0, #0xc]
    ldr r1, =unk_02349674
    ldr r0, =unk_023494e4
    sub r0, r1, r0
    ldr r1, =unk_0233af2c
    lsr r0, r0, #3
    strh r0, [r1, #0xe]
    ldr r0, =unk_02349674_dup
    sub r0, r2, r0
    lsr r0, r0, #3
    strh r0, [r1, #0x1e]
    ldr r2, =unk_02349a1c
    ldr r0, =unk_0234984c_dup
    sub r0, r2, r0
    lsr r2, r0, #3
    sub r0, r2, #3
    strh r0, [r1, #0x2e]
    lsl r0, r2, #0x10
    lsr r2, r0, #0x10
    strh r2, [r1, #0x3e]
    ldr r0, =unk_0233af6c
    ldr r1, =unk_02343d44
    strh r2, [r0, #0xe]
    ldr r0, =unk_0238ef50
    ldr r2, =unk_0233b15c
    bl FUN_02323250
    ldr r0, =unk_02399bd0
    ldr r1, =unk_02343d44
    ldr r2, =unk_0233b270
    bl FUN_02323250
    ldr r0, =unk_02397950
    ldr r1, =unk_02343d44
    ldr r2, =unk_0233b384
    bl FUN_02323250
    mov r2, #0x8a
    ldr r0, =unk_02397950
    ldr r1, =unk_023911d0
    lsl r2, r2, #6
    blx FUN_023371d4
    ldr r0, =unk_0233af2c
    mov r2, #2
    ldr r1, [r0, #0x30]
    mov r0, #0x6e
    lsl r0, r0, #2
    add r4, r1, r0
    ldrh r1, [r4, #0]
    lsl r0, r1, #0x10
    lsr r0, r0, #0x18
    lsl r1, r1, #0x18
    str r0, [sp, #0]
    lsr r3, r1, #0x18
    ldrh r1, [r4, #2]
    lsl r1, r1, #0x18
    lsr r1, r1, #0x18
    add r1, r3, r1
    str r1, [sp, #4]
    ldrh r1, [r4, #2]
    lsl r1, r1, #0x10
    lsr r1, r1, #0x18
    add r0, r0, r1
    add r0, r0, #1
    str r0, [sp, #8]
    ldr r0, =unk_023911d0
    mov r1, #0xc8
    bl FUN_02323d00
    ldrh r1, [r4, #8]
    lsl r0, r1, #0x10
    lsl r1, r1, #0x18
    lsr r0, r0, #0x18
    lsr r3, r1, #0x18
    str r0, [sp, #0]
    ldrh r1, [r4, #0xa]
    mov r2, #2
    lsl r1, r1, #0x18
    lsr r1, r1, #0x18
    add r1, r3, r1
    str r1, [sp, #4]
    ldrh r1, [r4, #0xa]
    lsl r1, r1, #0x10
    lsr r1, r1, #0x18
    add r0, r0, r1
    add r0, r0, #1
    str r0, [sp, #8]
    ldr r0, =unk_023911d0
    mov r1, #0xc8
    bl FUN_02323d00
    ldrh r1, [r4, #0x10]
    mov r2, #2
    lsl r0, r1, #0x10
    lsr r0, r0, #0x18
    lsl r1, r1, #0x18
    str r0, [sp, #0]
    lsr r3, r1, #0x18
    ldrh r1, [r4, #0x12]
    lsl r1, r1, #0x18
    lsr r1, r1, #0x18
    add r1, r3, r1
    str r1, [sp, #4]
    ldrh r1, [r4, #0x12]
    lsl r1, r1, #0x10
    lsr r1, r1, #0x18
    add r0, r0, r1
    add r0, r0, #1
    str r0, [sp, #8]
    ldr r0, =unk_023911d0
    mov r1, #0xc8
    bl FUN_02323d00
    ldr r0, =unk_0238ef0c
    mov r1, #0
    str r1, [r0, #0x18]
    str r1, [r0, #0x1c]
@check_trigger:
    ldr r0, =unk_0238ef0c
    ldr r1, [r0, #0x40]
    cmp r1, #0
    bne @check_key
    ldr r0, [r0, #0]
    cmp r0, #0
    bne @ret
@check_key:
    ldr r0, =unk_02fffce4
    ldrh r0, [r0, #0]
    lsl r0, r0, #0x1d
    lsr r0, r0, #0x1d
    bne @branch_zero
    ldr r0, =unk_0233af2c
    ldr r1, [r0, #0x10]
    ldrh r0, [r0, #0x1c]
    lsl r0, r0, #3
    add r1, r1, r0
    ldr r0, =unk_0238ef0c
    str r1, [r0, #0x38]
    mov r0, #1
    bl FUN_0232344c
    b @set_init_flag
@branch_zero:
    ldr r0, =unk_0233af2c
    ldr r1, [r0, #0]
    ldrh r0, [r0, #0xc]
    lsl r0, r0, #3
    add r1, r1, r0
    ldr r0, =unk_0238ef0c
    str r1, [r0, #0x38]
    mov r0, #0
    bl FUN_0232344c
@set_init_flag:
    ldr r0, =unk_0238ef0c
    mov r1, #1
    str r1, [r0, #0]
@ret:
    add sp, #0xc
    pop {r3, r4, pc}
}
