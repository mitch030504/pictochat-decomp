//cpp
// decomp: module=unk_autoload_0 addr=0x02322e60 name=FUN_02322e60
// flags: -thumb
#pragma thumb on
extern "C" asm void FUN_02322e60(void *arg0, int arg1) {
    add r3, r0, #0
    add r2, r1, #0
    ldr r0, =0x0238ef04
    add r1, r3, #0
    ldr r3, =0x0232e5c9
    ldr r0, [r0, #4]
    bx r3
}
