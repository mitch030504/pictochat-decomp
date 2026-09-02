//cpp
// decomp: module=unk_autoload_0 addr=0x02322b9c name=FUN_02322b9c
// flags: -thumb
#pragma thumb on
extern "C" int FUN_02322bcc(void *);

extern "C" asm void FUN_02322b9c(void *arg0) {
    push {r4, lr}
    add r4, r0, #0
    ldrb r1, [r4, #0]
    cmp r1, #1
    bne @check_validate
    ldr r1, =0x0233ae70
    ldr r0, =0x0238ee3c
    str r1, [r0, #0]
    pop {r4, pc}
@check_validate:
    bl FUN_02322bcc
    cmp r0, #0
    beq @clear
    ldr r0, =0x0238ee3c
    str r4, [r0, #0]
    pop {r4, pc}
@clear:
    ldr r0, =0x0238ee3c
    mov r1, #0
    str r1, [r0, #0]
    pop {r4, pc}
}
