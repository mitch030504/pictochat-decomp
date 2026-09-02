//cpp
// decomp: module=unk_autoload_0 addr=0x02322580 name=FUN_02322580
// flags: -thumb
#pragma thumb on
extern "C" void FUN_02338fd0(int, int, int, int, int);

extern "C" asm void FUN_02322580(int arg0, int arg1, int arg2, int arg3) {
    push {r3, r4, r5, lr}
    add r5, r0, #0
    lsl r1, r1, #0x18
    add r4, r2, #0
    lsl r2, r5, #8
    lsr r1, r1, #0x18
    orr r1, r2
    lsl r1, r1, #0x10
    str r3, [sp, #0]
    mov r0, #0x14
    lsr r1, r1, #0x10
    mov r2, #0
    add r3, r4, #0
    bl FUN_02338fd0
    pop {r3, r4, r5, pc}
}
