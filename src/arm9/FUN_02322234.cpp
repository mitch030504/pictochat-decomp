//cpp
// decomp: module=unk_autoload_0 addr=0x02322234 name=FUN_02322234
// flags: -thumb
#pragma thumb on
extern "C" asm void FUN_02322234(void) {
    ldr r1, =0x0238e11c
    mov r0, #1
    ldr r2, [r1, #0]
    bic r2, r0
    str r2, [r1, #0]
    bx lr
}
