//cpp
// decomp: module=unk_autoload_0 addr=0x0232227c name=FUN_0232227c
// flags: -thumb
#pragma thumb on
extern "C" asm void FUN_0232227c(void) {
    ldr r1, =0x0238e11c
    mov r0, #4
    ldr r2, [r1, #0]
    tst r0, r2
    beq @skip
    mov r0, #4
    bic r2, r0
    str r2, [r1, #0]
    mov r0, #1
    str r0, [r1, #0x7c]
@skip:
    bx lr
}
