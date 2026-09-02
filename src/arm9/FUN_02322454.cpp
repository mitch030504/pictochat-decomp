//cpp
// decomp: module=unk_autoload_0 addr=0x02322454 name=FUN_02322454
// flags: -thumb
#pragma thumb on
extern "C" asm int FUN_02322454(void) {
    ldr r0, =0x04000130
    ldr r1, =0x02ffffa8
    ldrh r2, [r0, #0]
    ldrh r0, [r1, #0]
    orr r2, r0
    lsr r0, r1, #0xc
    eor r2, r0
    and r0, r2
    lsl r0, r0, #0x10
    lsr r0, r0, #0x10
    mov r1, #0x20
    and r1, r0
    asr r1, r1, #1
    mvn r2, r1
    mov r1, #0x40
    and r1, r0
    lsl r1, r1, #1
    mvn r1, r1
    and r0, r1
    and r0, r2
    lsl r0, r0, #0x10
    lsr r0, r0, #0x10
    bx lr
}
