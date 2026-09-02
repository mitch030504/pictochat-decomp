//cpp
// decomp: module=unk_autoload_0 addr=0x02322604 name=FUN_02322604
// flags: -thumb
#pragma thumb on
extern "C" int FUN_02335b8c(int, int);

extern "C" asm int FUN_02322604(int arg0, int arg1) {
    push {r3, lr}
    lsl r0, r0, #4
    blx FUN_02335b8c
    lsl r0, r0, #0x10
    lsr r0, r0, #0x10
    pop {r3, pc}
}
