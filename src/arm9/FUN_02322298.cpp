//cpp
// decomp: module=unk_autoload_0 addr=0x02322298 name=FUN_02322298
// flags: -thumb
#pragma thumb on
extern "C" asm void FUN_02322298(void) {
    ldr r0, =0x0238e11c
    mov r1, #1
    str r1, [r0, #8]
    bx lr
}
