//cpp
// decomp: module=unk_autoload_0 addr=0x02321a84 name=FUN_02321a84
// flags: -thumb
#pragma thumb on
extern "C" void FUN_02321d20(void *, int);
extern "C" asm void FUN_02321a84(void) {
    ldr r1, =0x0238dbf4
    ldr r3, =FUN_02321d20
    ldr r1, [r1, #0]
    bx r3
}
