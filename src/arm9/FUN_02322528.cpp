//cpp
// decomp: module=unk_autoload_0 addr=0x02322528 name=FUN_02322528
// flags: -thumb
#pragma thumb on
extern "C" int FUN_02335b8c(int, int);

extern "C" asm void *FUN_02322528(void *arg0, int arg1) {
    push {r3, r4, r5, lr}
    add r4, r1, #0
    add r5, r0, #0
    add r0, r4, #0
    mov r1, #0xa
    blx FUN_02335b8c
    mov r1, #0xa
    blx FUN_02335b8c
    ldr r0, =0x0233a01c
    lsl r1, r1, #1
    ldrh r0, [r0, r1]
    mov r1, #0xa
    strh r0, [r5, #0]
    add r0, r4, #0
    blx FUN_02335b8c
    ldr r0, =0x0233a01c
    lsl r1, r1, #1
    ldrh r0, [r0, r1]
    strh r0, [r5, #2]
    mov r0, #0
    strh r0, [r5, #4]
    add r0, r5, #4
    pop {r3, r4, r5, pc}
}
