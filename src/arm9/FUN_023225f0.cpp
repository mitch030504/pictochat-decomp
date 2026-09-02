//cpp
// decomp: module=unk_autoload_0 addr=0x023225f0 name=FUN_023225f0
// flags: -thumb
#pragma thumb on
extern "C" asm int FUN_023225f0(void) {
    ldr r0, =0x02fffc82
    ldrb r0, [r0, #0]
    lsl r0, r0, #0x1c
    lsr r0, r0, #0x1c
    cmp r0, #0x10
    blt @ret
    mov r0, #1
@ret:
    bx lr
}
