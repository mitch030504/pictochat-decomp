//cpp
// decomp: module=unk_autoload_0 addr=0x02322e0c name=FUN_02322e0c
// flags: -thumb
#pragma thumb on
extern "C" void *FUN_0232e320(void *, int, int);

extern "C" asm void FUN_02322e0c(void) {
    push {r3, lr}
    ldr r0, =0x0238ef04
    ldr r0, [r0, #4]
    cmp r0, #0
    bne @ret
    mov r1, #0x5f
    ldr r0, =0x02004000
    lsl r1, r1, #0xe
    mov r2, #0
    bl FUN_0232e320
    ldr r1, =0x0238ef04
    str r0, [r1, #4]
@ret:
    pop {r3, pc}
}
