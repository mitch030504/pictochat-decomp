//cpp
// decomp: module=unk_autoload_0 addr=0x02322968 name=FUN_02322968
// flags: -thumb
#pragma thumb on
extern "C" void FUN_02322928(void);

extern "C" asm void FUN_02322968(void *arg0) {
    push {r4, lr}
    add r4, r0, #0
    bl FUN_02322928
    ldr r2, =0x0238edc4
    ldmia r2!, {r0, r1}
    stmia r4!, {r0, r1}
    ldr r0, [r2, #0]
    str r0, [r4, #0]
    pop {r4, pc}
}
