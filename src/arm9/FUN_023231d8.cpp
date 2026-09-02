//cpp
// decomp: module=unk_autoload_0 addr=0x023231d8 name=FUN_023231d8
// flags: -thumb
#pragma thumb on
extern "C" asm void FUN_023231d8(int arg0, int arg1) {
    ldr r2, =0x0238ef0c
    str r0, [r2, #0x18]
    str r1, [r2, #0x1c]
    bx lr
}
