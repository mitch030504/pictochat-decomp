//cpp
// decomp: module=unk_autoload_0 addr=0x02321d84 name=FUN_02321d84
// flags: -thumb
#pragma thumb on
extern "C" int FUN_02335b8c(int);
extern "C" void FUN_02322298(void);
extern "C" void FUN_023222a4(void);
extern "C" void FUN_02321b74(void);

extern "C" asm void FUN_02321d84(void) {
    push {r4, lr}
    ldr r0, =0x0238e028
    ldr r1, [r0, #0x20]
    lsl r2, r1, #1
    ldr r1, =0x0233a000
    ldrh r2, [r1, r2]
    ldr r1, [r0, #0xc]
    cmp r1, r2
    bge @ge_target
    add r1, #0x10
    str r1, [r0, #0xc]
    b @check_active
@ge_target:
    cmp r1, r2
    ble @check_active
    sub r1, #0x10
    str r1, [r0, #0xc]
@check_active:
    ldr r0, =0x0238e028
    ldr r1, [r0, #0x14]
    cmp r1, #0
    blt @done
    ldr r2, [r0, #0x18]
    cmp r2, #2
    bne @not_mode2
    mov r4, #0x90
    b @check_state
@not_mode2:
    mov r4, #0x78
@check_state:
    ldr r0, =0x0238e028
    ldr r1, [r0, #0x1c]
    cmp r1, #0
    beq @state_0
    cmp r1, #1
    beq @state_1
    cmp r1, #2
    beq @state_2
    pop {r4, pc}
@state_0:
    ldr r1, [r0, #0x10]
    add r2, r1, #1
    str r2, [r0, #0x10]
    cmp r2, #0xf
    bgt @state0_finish
    mov r1, #0xf
    sub r0, r1, r2
    mul r0, r4
    blx FUN_02335b8c
    ldr r1, =0x0238e028
    str r0, [r1, #0]
    pop {r4, pc}
@state0_finish:
    ldr r0, [r0, #0x18]
    cmp r0, #1
    bne @skip_2298
    bl FUN_02322298
@skip_2298:
    ldr r0, =0x0238e028
    mov r1, #0
    str r1, [r0, #0x10]
    mov r1, #1
    str r1, [r0, #0x1c]
    pop {r4, pc}
@state_1:
    cmp r2, #0
    bne @state1_not0
    ldr r1, [r0, #0x10]
    add r1, r1, #1
    str r1, [r0, #0x10]
    cmp r1, #0xb4
    ble @done
    mov r1, #0
    str r1, [r0, #0x10]
    mov r1, #2
    str r1, [r0, #0x1c]
    pop {r4, pc}
@state1_not0:
    cmp r2, #1
    bne @state1_mode2
    ldr r1, [r0, #0x10]
    add r1, r1, #1
    str r1, [r0, #0x10]
    ldr r0, [r0, #0x24]
    cmp r0, #0
    beq @done
    bl FUN_023222a4
    ldr r0, =0x0238e028
    ldr r1, [r0, #0x10]
    cmp r1, #0x1e
    ble @done
    mov r1, #0
    str r1, [r0, #0x10]
    mov r1, #2
    str r1, [r0, #0x1c]
    pop {r4, pc}
@state1_mode2:
    cmp r2, #2
    bne @done
    ldr r1, [r0, #0x10]
    add r1, r1, #1
    str r1, [r0, #0x10]
    cmp r1, #0x1e
    ble @done
    ldr r1, [r0, #0x24]
    cmp r1, #0
    beq @done
    mov r1, #0
    str r1, [r0, #0x10]
    mov r1, #2
    str r1, [r0, #0x1c]
    pop {r4, pc}
@state_2:
    ldr r1, [r0, #0x10]
    add r2, r1, #1
    str r2, [r0, #0x10]
    cmp r2, #0xf
    bgt @state2_finish
    mov r1, #0xf
    sub r0, r1, r2
    mul r0, r4
    blx FUN_02335b8c
    sub r1, r4, r0
    ldr r0, =0x0238e028
    str r1, [r0, #0]
    pop {r4, pc}
@state2_finish:
    bl FUN_02322298
    bl FUN_02321b74
@done:
    pop {r4, pc}
}
