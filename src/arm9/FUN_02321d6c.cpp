//cpp
// decomp: module=unk_autoload_0 addr=0x02321d6c name=FUN_02321d6c
// flags: -thumb
#pragma thumb on
extern "C" asm int FUN_02321d6c(void) {
    ldr r0, =0x0238e028
    ldr r1, [r0, #0x14]
    mov r0, #0
    mvn r0, r0
    cmp r1, r0
    beq @is_neg1
    mov r0, #1
    bx lr
@is_neg1:
    mov r0, #0
    bx lr
}
