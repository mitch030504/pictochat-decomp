//cpp
// decomp: module=unk_autoload_0 addr=0x02322e78 name=FUN_02322e78
// flags: -thumb
#pragma thumb on
extern "C" void FUN_0232e368(void *, void *);

extern "C" asm void FUN_02322e78(void *arg0) {
    push {r3, lr}
    add r1, r0, #0
    beq @ret
    ldr r0, =0x0238ef04
    ldr r0, [r0, #4]
    bl FUN_0232e368
@ret:
    pop {r3, pc}
}
