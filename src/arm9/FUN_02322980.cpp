//cpp
// decomp: module=unk_autoload_0 addr=0x02322980 name=FUN_02322980
// flags: -thumb
#pragma thumb on
extern "C" void FUN_02322928(void);

extern "C" asm void FUN_02322980(void *arg0) {
    push {r4, lr}
    add r4, r0, #0
    bl FUN_02322928
    ldr r2, =0x0238edd0
    ldmia r2!, {r0, r1}
    stmia r4!, {r0, r1}
    ldmia r2!, {r0, r1}
    stmia r4!, {r0, r1}
    pop {r4, pc}
}
