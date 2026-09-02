//cpp
// decomp: module=unk_autoload_0 addr=0x02322170 name=FUN_02322170
// flags: -thumb
#pragma thumb on
extern "C" asm void FUN_02322170(int arg0, int arg1) {
    add r2, r1, #0
    mov r3, #0x54
    ldr r1, =0x0238e134
    mul r2, r3
    ldr r1, [r1, r2]
    cmp r0, r1
    bne @diff
    ldr r1, =0x0238e138
    sub r3, #0x55
    ldr r1, [r1, r2]
    cmp r1, r3
    beq @ret
@diff:
    ldr r1, =0x0238e138
    str r0, [r1, r2]
    ldr r0, =0x0238e144
    mov r1, #1
    str r1, [r0, r2]
@ret:
    bx lr
}
