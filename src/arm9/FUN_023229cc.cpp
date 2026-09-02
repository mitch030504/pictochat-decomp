//cpp
// decomp: module=unk_autoload_0 addr=0x023229cc name=FUN_023229cc
// flags: -thumb
#pragma thumb on
extern "C" int FUN_02335b8c(int, int);
extern "C" void FUN_02336184(void *, int);

extern "C" asm void FUN_023229cc(void) {
    push {r4, lr}
    ldr r1, =0x0233ae40
    mov r4, #0
    ldr r3, [r1, #0]
    sub r0, r4, #1
    cmp r3, r0
    beq @ret
    ldr r0, =0x0238ede0
    ldr r2, [r0, #0x14]
    add r2, r2, #1
    str r2, [r0, #0x14]
    cmp r3, #0
    bne @check_one
    ldr r1, [r1, #4]
    sub r0, r1, r2
    lsl r0, r0, #4
    blx FUN_02335b8c
    add r4, r0, #0
    b @apply
@check_one:
    cmp r3, #1
    bne @apply
    ldr r1, [r1, #4]
    lsl r0, r2, #4
    blx FUN_02335b8c
    add r4, r0, #0
@apply:
    ldr r0, =0x0400006c
    add r1, r4, #0
    bl FUN_02336184
    ldr r0, =0x0400106c
    add r1, r4, #0
    bl FUN_02336184
    ldr r0, =0x0238ede0
    ldr r2, [r0, #0x14]
    ldr r0, =0x0233ae40
    ldr r1, [r0, #4]
    cmp r2, r1
    bne @ret
    mov r1, #0
    mvn r1, r1
    str r1, [r0, #0]
@ret:
    pop {r4, pc}
}
