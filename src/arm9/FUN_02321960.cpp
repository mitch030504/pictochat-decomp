//cpp
// decomp: module=unk_autoload_0 addr=0x02321960 name=FUN_02321960
// flags: -thumb
#pragma thumb on

extern "C" void FUN_0232d828(int, int, int);
extern "C" void FUN_0232dca8(int, int);

extern "C" asm void FUN_02321960(void) {
    push {r4, r5, r6, lr}
    ldr r0, =0x0238db74
    ldr r1, =0x0233c568
    ldr r2, =0x0233bbd4
    bl FUN_0232d828
    ldr r0, =0x0238db94
    ldr r1, =0x0233ea04
    ldr r2, =0x0233bdbc
    bl FUN_0232d828
    ldr r0, =0x0238dbb4
    ldr r1, =0x0233c458
    ldr r2, =0x0233bb88
    bl FUN_0232d828
    ldr r0, =0x0238dbd4
    ldr r1, =0x0233bec4
    ldr r2, =0x0233bb14
    bl FUN_0232d828
    mov r5, #0
    ldr r4, =0x0238db74
    ldr r6, =0x0233ad28
    b @check
@loop:
    ldrh r1, [r6, #0]
    lsl r0, r5, #5
    add r0, r4, r0
    bl FUN_0232dca8
    add r5, r5, #1
@check:
    cmp r5, #4
    blo @loop
    pop {r4, r5, r6, pc}
}
