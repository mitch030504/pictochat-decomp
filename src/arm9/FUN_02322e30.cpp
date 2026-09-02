//cpp
// decomp: module=unk_autoload_0 addr=0x02322e30 name=FUN_02322e30
// flags: -thumb
#pragma thumb on
extern "C" void FUN_0232e340(void *);

extern "C" asm void FUN_02322e30(void) {
    push {r3, lr}
    ldr r0, =0x0238ef04
    ldr r0, [r0, #4]
    cmp r0, #0
    beq @ret
    bl FUN_0232e340
    ldr r0, =0x0238ef04
    mov r1, #0
    str r1, [r0, #4]
@ret:
    pop {r3, pc}
}
