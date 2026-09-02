//cpp
// decomp: module=unk_autoload_0 addr=0x02322560 name=FUN_02322560
// flags: -thumb
#pragma thumb on
extern "C" void *FUN_02322528(void *, int);

extern "C" asm void *FUN_02322560(void *arg0, int arg1) {
    push {r3, lr}
    cmp r1, #0xa
    bge @call_528
    lsl r2, r1, #1
    ldr r1, =0x0233a01c
    ldrh r1, [r1, r2]
    strh r1, [r0, #0]
    mov r1, #0
    strh r1, [r0, #2]
    add r0, r0, #2
    pop {r3, pc}
@call_528:
    bl FUN_02322528
    pop {r3, pc}
}
