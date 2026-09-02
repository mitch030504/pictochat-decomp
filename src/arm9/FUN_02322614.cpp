//cpp
// decomp: module=unk_autoload_0 addr=0x02322614 name=FUN_02322614
// flags: -thumb
#pragma thumb on
extern "C" int FUN_02335b8c(int, int);

extern "C" asm int FUN_02322614(int arg0, int arg1, int arg2) {
    push {r3, lr}
    sub r0, r1, r0
    lsl r0, r0, #4
    blx FUN_02335b8c
    lsl r0, r0, #0x10
    lsr r0, r0, #0x10
    pop {r3, pc}
}
