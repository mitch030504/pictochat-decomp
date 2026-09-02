//cpp
// decomp: module=unk_autoload_0 addr=0x023221a0 name=FUN_023221a0
// flags: -thumb
#pragma thumb on
extern "C" asm int FUN_023221a0(int arg0, int arg1) {
    mov r2, #0x54
    mul r2, r1
    ldr r1, =0x0238e134
    ldr r1, [r1, r2]
    cmp r0, r1
    bne @diff
    mov r0, #1
    bx lr
@diff:
    mov r0, #0
    bx lr
}
