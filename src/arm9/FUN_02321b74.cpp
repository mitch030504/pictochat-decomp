//cpp
// decomp: module=unk_autoload_0 addr=0x02321b74 name=FUN_02321b74
// flags: -thumb
#pragma thumb on
extern "C" void FUN_0232131c(void *);
extern "C" asm void FUN_02321b74(void) {
    ldr r0, =0x0238e028
    mov r1, #0x90
    str r1, [r0, #0]
    sub r1, #0x91
    str r1, [r0, #0x14]
    mov r1, #0
    str r1, [r0, #4]
    str r1, [r0, #8]
    str r1, [r0, #0xc]
    str r1, [r0, #0x10]
    str r1, [r0, #0x18]
    str r1, [r0, #0x1c]
    str r1, [r0, #0x20]
    str r1, [r0, #0x24]
    ldr r3, =FUN_0232131c
    ldr r0, =0x0238e050
    bx r3
}
