//cpp
// decomp: module=unk_autoload_0 addr=0x02322f74 name=FUN_02322f74
// flags: -thumb
#pragma thumb on
extern "C" void *FUN_023219e0(void);
extern "C" void FUN_02337238(void *, void *, int);
extern "C" void *FUN_023219d8(void);
extern "C" void FUN_0232d890(void *, void *, int, int, void *, void *);
extern "C" int FUN_0232dbe0(void *, int);
extern "C" int FUN_0232dc94(void *, int);
extern "C" void FUN_0232d9dc(void *, void *, int, int, int, int);

extern "C" asm void FUN_02322f74(void *arg0) {
    push {r4, r5, r6, r7, lr}
    sub sp, #0x34
    str r0, [sp, #8]
    bl FUN_023219e0
    ldrh r0, [r0, #0x16]
    mov r2, #0x8a
    ldr r1, =0x023956d0
    lsl r0, r0, #0x16
    lsr r6, r0, #0x1b
    ldr r0, [sp, #8]
    lsl r2, r2, #6
    ldr r0, [r0, #8]
    str r0, [sp, #0x14]
    ldr r0, [r0, #4]
    blx FUN_02337238
    bl FUN_023219d8
    mov r1, #0
    str r1, [sp, #0]
    str r0, [sp, #4]
    ldr r1, =0x023956d0
    add r0, sp, #0x24
    mov r2, #0xc8
    mov r3, #0x58
    bl FUN_0232d890
    ldr r0, [sp, #8]
    mov r5, #0
    ldr r1, [r0, #8]
    ldrh r2, [r1, #0xe]
    b @check_find
@find_loop:
    ldr r4, [r1, #0]
    lsl r3, r5, #3
    ldrh r0, [r1, #0xc]
    add r3, r4, r3
    ldrh r3, [r3, #6]
    add r0, r0, #1
    cmp r0, r3
    beq @found_slot
    add r0, r5, #1
    lsl r0, r0, #0x10
    lsr r5, r0, #0x10
@check_find:
    cmp r5, r2
    blo @find_loop
    mov r5, #0
@found_slot:
    lsr r0, r6, #0x1f
    add r0, r6, r0
    asr r0, r0, #1
    mov r4, #0
    str r0, [sp, #0xc]
    b @check_render
@render_loop:
    ldr r0, [sp, #0x14]
    ldr r6, [r0, #0]
    add r0, r5, r4
    lsl r0, r0, #3
    str r0, [sp, #0x1c]
    add r7, r6, r0
    ldr r0, [sp, #8]
    ldr r1, [r0, #0x14]
    lsl r0, r4, #1
    ldrh r0, [r1, r0]
    str r0, [sp, #0x10]
    bl FUN_023219e0
    ldr r1, [sp, #0x10]
    bl FUN_0232dbe0
    str r0, [sp, #0x20]
    bl FUN_023219e0
    ldr r1, [sp, #0x10]
    bl FUN_0232dc94
    add r2, r0, #0
    ldr r0, [sp, #0x1c]
    ldrh r1, [r7, #2]
    ldrh r0, [r6, r0]
    lsl r1, r1, #0x18
    lsl r0, r0, #0x18
    lsr r0, r0, #0x18
    lsr r1, r1, #0x19
    add r1, r0, r1
    ldr r0, [sp, #0x20]
    lsr r3, r0, #0x1f
    add r3, r0, r3
    asr r0, r3, #1
    sub r0, r1, r0
    sub r0, r0, r2
    str r0, [sp, #0x18]
    bl FUN_023219e0
    ldr r1, [sp, #0x10]
    ldr r3, [sp, #0x1c]
    str r1, [sp, #0]
    mov r1, #7
    str r1, [sp, #4]
    ldrh r3, [r6, r3]
    ldrh r6, [r7, #2]
    ldr r2, [sp, #0x18]
    lsl r3, r3, #0x10
    lsl r6, r6, #0x10
    lsr r3, r3, #0x18
    lsr r6, r6, #0x19
    add r6, r3, r6
    ldr r3, [sp, #0xc]
    add r1, sp, #0x24
    sub r3, r6, r3
    bl FUN_0232d9dc
    add r4, r4, #1
@check_render:
    ldr r0, [sp, #0x14]
    ldrh r0, [r0, #0xe]
    sub r0, r0, r5
    cmp r4, r0
    blt @render_loop
    add sp, #0x34
    pop {r4, r5, r6, r7, pc}
}
