//cpp
// decomp: module=unk_autoload_0 addr=0x02323240 name=FUN_02323240
// flags: -thumb
#pragma thumb on
extern "C" asm void FUN_02323240(void) {
    ldr r0, =0x0238ef0c
    mov r1, #0
    strh r1, [r0, #0x30]
    str r1, [r0, #0xc]
    bx lr
}
