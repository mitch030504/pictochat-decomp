//cpp
// decomp: module=unk_autoload_0 addr=0x02321e84 name=FUN_02321e84
// flags: -thumb
#pragma thumb on
extern "C" void FUN_023210a8(void *, void *);
extern "C" int FUN_02321144(void *, int);
extern "C" int FUN_0232103c(int);
extern "C" void FUN_023206f8(int);

extern "C" asm int FUN_02321e84(void) {
    push {r4, r5, r6, lr}
    ldr r0, =0x0238e028
    mov r4, #0
    ldr r1, [r0, #0x18]
    mvn r4, r4
    cmp r1, #2
    bne @ret
    ldr r0, [r0, #0x1c]
    cmp r0, #1
    bne @ret
    ldr r0, =0x0238e050
    mov r6, #0
    ldr r1, =0x0233b4c4
    add r5, r6, #0
    bl FUN_023210a8
    ldr r0, =0x0238e050
    mov r1, #1
    bl FUN_02321144
    cmp r0, #0
    beq @not_btn1
    ldr r0, =0x0238e028
    mov r6, #1
    str r6, [r0, #0x20]
    b @check_keys
@not_btn1:
    ldr r0, =0x0238e050
    add r1, r6, #0
    bl FUN_02321144
    cmp r0, #0
    beq @check_keys
    ldr r0, =0x0238e028
    add r1, r6, #0
    str r1, [r0, #0x20]
    mov r5, #1
@check_keys:
    mov r0, #0x20
    bl FUN_0232103c
    cmp r0, #0
    beq @not_key20
    ldr r0, =0x0238e028
    mov r1, #0
    str r1, [r0, #0x20]
    mov r0, #0x1d
    bl FUN_023206f8
    b @ret
@not_key20:
    mov r0, #0x10
    bl FUN_0232103c
    cmp r0, #0
    beq @not_key10
    ldr r0, =0x0238e028
    mov r1, #1
    str r1, [r0, #0x20]
    mov r0, #0x1d
    bl FUN_023206f8
    b @ret
@not_key10:
    mov r0, #1
    bl FUN_0232103c
    cmp r0, #0
    bne @has_key1
    cmp r6, #0
    beq @check_key2
@has_key1:
    ldr r0, =0x0238e028
    ldr r4, [r0, #0x20]
    b @ret
@check_key2:
    mov r0, #2
    bl FUN_0232103c
    cmp r0, #0
    bne @has_key2
    cmp r5, #0
    beq @ret
@has_key2:
    mov r4, #0
@ret:
    add r0, r4, #0
    pop {r4, r5, r6, pc}
}
