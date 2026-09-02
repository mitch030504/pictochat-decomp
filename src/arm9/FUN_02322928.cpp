//cpp
// decomp: module=unk_autoload_0 addr=0x02322928 name=FUN_02322928
// flags: -thumb
#pragma thumb on
extern "C" int *FUN_023222cc(void);
extern "C" int FUN_02335d98(int, int);
extern "C" int FUN_0233971c(void *, void *);

extern "C" void *unk_0238edd0;
extern "C" void *unk_0238edc4;
extern "C" void *unk_0238edc4_dup;

extern "C" asm void FUN_02322928(void) {
    push {r3, lr}
    bl FUN_023222cc
    ldr r0, [r0, #4]
    mov r1, #0x14
    blx FUN_02335d98
    cmp r1, #0
    bne @ret
    ldr r0, =unk_0238edd0
    ldr r1, =unk_0238edc4
    bl FUN_0233971c
    cmp r0, #0
    beq @ret
    ldr r0, =unk_0238edc4_dup
    mov r2, #0
    str r2, [r0, #0xc]
    mov r1, #1
    str r1, [r0, #0x10]
    str r1, [r0, #0x14]
    str r2, [r0, #0x18]
    str r2, [r0, #0]
    str r2, [r0, #4]
    str r2, [r0, #8]
@ret:
    pop {r3, pc}
}
