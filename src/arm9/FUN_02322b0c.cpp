//cpp
// decomp: module=unk_autoload_0 addr=0x02322b0c name=FUN_02322b0c
// flags: -thumb
#pragma thumb on
extern "C" void FUN_023317bc(void);
extern "C" void FUN_023317d8(void *, int);
extern "C" void FUN_02331218(void);
extern "C" void FUN_023312a8(void);

extern "C" asm void FUN_02322b0c(void) {
    push {r3, lr}
    bl FUN_023317bc
    ldr r0, =0x02322d89
    mov r1, #0
    bl FUN_023317d8
    blx FUN_02331218
    blx FUN_023312a8
    pop {r3, pc}
}
