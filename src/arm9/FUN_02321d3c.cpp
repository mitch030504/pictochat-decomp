//cpp
// decomp: module=unk_autoload_0 addr=0x02321d3c name=FUN_02321d3c
// flags: -thumb
#pragma thumb on
extern "C" void FUN_02321cf4(int, int);
extern "C" void FUN_02321460(void *, int);

extern "C" asm void FUN_02321d3c(int arg0, int arg1, int arg2) {
    push {r4, lr}
    add r4, r2, #0
    bl FUN_02321cf4
    ldr r0, =0x0238e05c
    add r1, r4, #0
    bl FUN_02321460
    pop {r4, pc}
}
