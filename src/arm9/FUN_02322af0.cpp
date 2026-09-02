//cpp
// decomp: module=unk_autoload_0 addr=0x02322af0 name=FUN_02322af0
// flags: -thumb
#pragma thumb on
extern "C" asm void FUN_02322af0(int arg0) {
    ldr r2, =0x0238ede0
    mov r3, #0
    add r0, #8
    mov r1, #0xf
    str r3, [r2, #4]
    and r0, r1
    str r0, [r2, #0xc]
    str r3, [r2, #8]
    bx lr
}
