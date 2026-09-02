//cpp
// decomp: module=unk_autoload_0 addr=0x023226e8 name=FUN_023226e8
// flags: -thumb
#pragma thumb on
extern "C" void FUN_0232d208(void);
extern "C" void FUN_02330368(int, int);
extern "C" void FUN_023360b4(void);
extern "C" int FUN_0233255c(void);
extern "C" int FUN_02339264(int, int, int);
extern "C" int FUN_02332530(void);
extern "C" void FUN_023360e8(void);
extern "C" void *FUN_023222cc(void);
extern "C" void FUN_023390f4(int, int);
extern "C" void FUN_0232d144(void);
extern "C" int FUN_0232d17c(void);
extern "C" void FUN_02339160(void);
extern "C" void FUN_02331ff4(void);

extern "C" asm void FUN_023226e8(void) {
    push {r3, lr}
    ldr r0, =0x02ffffa8
    ldr r2, =0x0238eda0
    ldrh r0, [r0, #0]
    mov r1, #2
    lsl r1, r1, #0xe
    ldr r3, [r2, #0]
    lsr r2, r1, #6
    and r0, r1
    and r2, r3
    lsr r1, r1, #6
    asr r0, r0, #0xf
    cmp r2, r1
    bne @lbl_708
    mov r1, #1
    b @lbl_70a
@lbl_708:
    mov r1, #0
@lbl_70a:
    mvn r2, r0
    eor r1, r0
    tst r1, r2
    beq @lbl_720
    ldr r2, =0x0238eda0
    mov r1, #1
    ldr r3, [r2, #0]
    lsl r1, r1, #8
    orr r1, r3
    str r1, [r2, #0]
    b @lbl_72a
@lbl_720:
    ldr r2, =0x0238eda0
    ldr r1, =0xfffffeff
    ldr r3, [r2, #0]
    and r1, r3
    str r1, [r2, #0]
@lbl_72a:
    cmp r0, #0
    beq @lbl_73c
    ldr r1, =0x0238eda0
    mov r0, #2
    ldr r2, [r1, #0]
    lsl r0, r0, #8
    orr r0, r2
    str r0, [r1, #0]
    b @lbl_746
@lbl_73c:
    ldr r1, =0x0238eda0
    ldr r0, =0xfffffdff
    ldr r2, [r1, #0]
    and r0, r2
    str r0, [r1, #0]
@lbl_746:
    ldr r2, =0x0238eda0
    mov r1, #2
    ldr r0, [r2, #0]
    and r0, r1
    cmp r0, #2
    beq @lbl_806
    ldr r3, [r2, #0]
    lsl r0, r1, #0xb
    and r3, r0
    cmp r3, r0
    beq @lbl_806
    ldr r0, =0x02ffffa8
    ldrh r3, [r0, #0]
    lsl r0, r1, #0xe
    and r0, r3
    asr r0, r0, #0xf
    beq @lbl_806
    ldr r1, [r2, #0]
    mov r0, #0x40
    orr r0, r1
    str r0, [r2, #0]
    ldr r0, [r2, #8]
    cmp r0, #0
    beq @lbl_778
    blx r0
@lbl_778:
    bl FUN_0232d208
    mov r0, #1
    add r1, r0, #0
    blx FUN_02330368
    bl FUN_023360b4
    ldr r2, =0x04001000
    ldr r0, =0xfffeffff
    ldr r1, [r2, #0]
    and r0, r1
    str r0, [r2, #0]
    mov r0, #1
    add r1, r0, #0
    blx FUN_02330368
    bl FUN_0233255c
    cmp r0, #0
    beq @lbl_7c6
    ldr r0, =0x0238eda0
    ldr r1, [r0, #0]
    mov r0, #1
    and r0, r1
    cmp r0, #1
    bne @lbl_7b2
    mov r1, #2
    b @lbl_7b4
@lbl_7b2:
    mov r1, #0
@lbl_7b4:
    mov r0, #0x14
    orr r0, r1
    mov r1, #0
    add r2, r1, #0
    bl FUN_02339264
    bl FUN_02332530
    b @lbl_7e4
@lbl_7c6:
    ldr r0, =0x0238eda0
    ldr r1, [r0, #0]
    mov r0, #1
    and r0, r1
    cmp r0, #1
    bne @lbl_7d6
    mov r1, #2
    b @lbl_7d8
@lbl_7d6:
    mov r1, #0
@lbl_7d8:
    mov r0, #4
    orr r0, r1
    mov r1, #0
    add r2, r1, #0
    bl FUN_02339264
@lbl_7e4:
    mov r0, #1
    add r1, r0, #0
    blx FUN_02330368
    mov r0, #1
    add r1, r0, #0
    blx FUN_02330368
    bl FUN_023360e8
    ldr r2, =0x04001000
    mov r0, #1
    ldr r1, [r2, #0]
    lsl r0, r0, #0x10
    orr r0, r1
    str r0, [r2, #0]
    b @lbl_846
@lbl_806:
    bl FUN_023222cc
    ldr r0, [r0, #0]
    lsl r0, r0, #0x16
    lsr r0, r0, #0x1f
    beq @lbl_846
    bl FUN_023222cc
    ldr r0, [r0, #0]
    lsl r0, r0, #0x1b
    lsr r0, r0, #0x1f
    bne @lbl_846
    ldr r0, =0x02ffffa8
    ldrh r1, [r0, #0]
    mov r0, #2
    lsl r0, r0, #0xe
    and r0, r1
    asr r0, r0, #0xf
    beq @lbl_836
    mov r0, #2
    mov r1, #0
    bl FUN_023390f4
    b @lbl_846
@lbl_836:
    bl FUN_023222cc
    ldr r0, [r0, #0]
    lsl r0, r0, #0x15
    lsr r0, r0, #0x1f
    beq @lbl_846
    bl FUN_0232d144
@lbl_846:
    ldr r0, =0x0238eda0
    ldr r1, [r0, #0]
    mov r0, #0x40
    and r0, r1
    cmp r0, #0x40
    bne @lbl_894
    bl FUN_0232d17c
    bl FUN_0232d208
    bl FUN_0232d17c
    cmp r0, #0
    bne @lbl_87a
    ldr r0, =0x02fff890
    mov r1, #1
    ldr r0, [r0, #0]
    tst r0, r1
    bne @lbl_86e
    mov r1, #0
@lbl_86e:
    cmp r1, #0
    beq @lbl_87e
    bl FUN_02332530
    cmp r0, #0
    beq @lbl_87e
@lbl_87a:
    bl FUN_02339160
@lbl_87e:
    ldr r0, =0x0238eda0
    ldr r0, [r0, #0xc]
    cmp r0, #0
    beq @lbl_888
    blx r0
@lbl_888:
    ldr r1, =0x0238eda0
    mov r0, #0x40
    ldr r2, [r1, #0]
    bic r2, r0
    str r2, [r1, #0]
    pop {r3, pc}
@lbl_894:
    bl FUN_0232d17c
    cmp r0, #0
    bne @lbl_8b4
    ldr r0, =0x02fff890
    mov r1, #1
    ldr r0, [r0, #0]
    tst r0, r1
    bne @lbl_8a8
    mov r1, #0
@lbl_8a8:
    cmp r1, #0
    beq @lbl_8c4
    bl FUN_02332530
    cmp r0, #0
    beq @lbl_8c4
@lbl_8b4:
    ldr r0, =0x0238eda0
    ldr r0, [r0, #8]
    cmp r0, #0
    beq @lbl_8be
    blx r0
@lbl_8be:
    bl FUN_02331ff4
    pop {r3, pc}
@lbl_8c4:
    ldr r1, =0x0238eda0
    mov r0, #1
    ldr r2, [r1, #0]
    lsl r0, r0, #8
    and r2, r0
    cmp r2, r0
    bne @lbl_8da
    ldr r0, [r1, #0x10]
    cmp r0, #0
    beq @lbl_8da
    blx r0
@lbl_8da:
    pop {r3, pc}
}
