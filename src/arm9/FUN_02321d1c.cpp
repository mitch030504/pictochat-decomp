//cpp
// decomp: module=unk_autoload_0 addr=0x02321d1c name=FUN_02321d1c
// flags: -thumb
#pragma thumb on
extern "C" void FUN_02321cf4(int, int);
extern "C" void FUN_02321404(void *, void *, int);

extern "C" asm void FUN_02321d1c(int arg0, int arg1) {
    push {r3, lr}
    bl FUN_02321cf4
    ldr r2, =0x0238e028
    ldr r0, =0x0238e05c
    ldr r1, =0x0233b8c4
    ldr r2, [r2, #0x14]
    bl FUN_02321404
    pop {r3, pc}
}
