//cpp
// decomp: module=unk_autoload_0 addr=0x02322a38 name=FUN_02322a38
// flags: -thumb
#pragma thumb on
extern "C" asm int FUN_02322a38(void) {
    ldr r0, =0x0233ae40
    ldr r1, [r0, #0]
    mov r0, #0
    mvn r0, r0
    cmp r1, r0
    bne @ret_zero
    mov r0, #1
    bx lr
@ret_zero:
    mov r0, #0
    bx lr
}
