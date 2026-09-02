//cpp
// decomp: module=unk_autoload_0 addr=0x02321380 name=FUN_02321380
#pragma thumb on
extern "C" void FUN_0232de04(void *, int);
extern "C" void FUN_02321a78(int);
extern "C" void FUN_023371ec(int, void *, int);

extern "C" asm void FUN_02321380(void *obj, int arg1) {
    push {r3, r4, r5, lr}
    add r4, r1, #0
    mov r1, #0
    add r5, r0, #0
    bl FUN_0232de04
    ldr r0, =0x02fffce4
    ldrh r0, [r0]
    lsl r0, r0, #29
    lsr r0, r0, #29
    bl FUN_02321a78
    lsl r0, r4, #9
    lsr r0, r0, #16
    strh r0, [r5, #0xc]
    strh r0, [r5, #0xe]
    mov r2, #1
    ldr r1, [r5, #0x14]
    mov r0, #0
    lsl r2, r2, #16
    blx FUN_023371ec
    pop {r3, r4, r5, pc}
}
