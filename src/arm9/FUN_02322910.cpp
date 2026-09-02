//cpp
// decomp: module=unk_autoload_0 addr=0x02322910 name=FUN_02322910
// flags: -thumb
#pragma thumb on
extern "C" void FUN_023226e8(void);
extern "C" void FUN_0232078c(void);
extern "C" int *FUN_023222cc(void);

extern "C" asm void FUN_02322910(void) {
    push {r3, lr}
    bl FUN_023226e8
    bl FUN_0232078c
    bl FUN_023222cc
    ldr r1, [r0, #4]
    add r1, r1, #1
    str r1, [r0, #4]
    pop {r3, pc}
}
