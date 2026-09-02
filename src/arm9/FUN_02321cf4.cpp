//cpp
// decomp: module=unk_autoload_0 addr=0x02321cf4 name=FUN_02321cf4
// flags: -thumb
#pragma thumb on
extern "C" void FUN_023222a4(void);
extern "C" void FUN_02321b74(void);

extern "C" asm void FUN_02321cf4(int arg0, int arg1) {
    push {r3, r4, r5, lr}
    add r5, r0, #0
    add r4, r1, #0
    bl FUN_023222a4
    bl FUN_02321b74
    ldr r0, =0x0238e028
    cmp r4, #2
    str r5, [r0, #0x14]
    str r4, [r0, #0x18]
    bne @skip
    mov r1, #0xd
    mvn r1, r1
    str r1, [r0, #4]
    mov r1, #8
    str r1, [r0, #8]
@skip:
    pop {r3, r4, r5, pc}
}
