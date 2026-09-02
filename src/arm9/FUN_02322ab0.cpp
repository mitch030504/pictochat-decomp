//cpp
// decomp: module=unk_autoload_0 addr=0x02322ab0 name=FUN_02322ab0
// flags: -thumb
#pragma thumb on
extern "C" void FUN_023342e4(void *, void *, int);
extern "C" void FUN_02328b84(int);

extern "C" asm void FUN_02322ab0(void) {
    push {r3, lr}
    sub sp, #0x20
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
    ldr r0, =0x0238ede0
    ldr r0, [r0, #0xc]
    bl FUN_02328b84
    add sp, #0x20
    pop {r3, pc}
}
