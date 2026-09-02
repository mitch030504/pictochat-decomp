//cpp
// decomp: module=unk_autoload_0 addr=0x0232d398 name=FUN_0232d398
#pragma thumb on
extern "C" asm int FUN_0232d398(const unsigned char *p) {
    ldrb r1, [r0, #0x1a]
    cmp r1, #10
    bls @ok1
    mov r0, #0
    bx lr
@ok1:
    add r1, r0, #0
    add r1, #0x50
    ldrb r1, [r1]
    cmp r1, #26
    bls @ok2
    mov r0, #0
    bx lr
@ok2:
    add r1, r0, #0
    add r1, #0x52
    ldrb r1, [r1]
    cmp r1, #23
    bhi @fail
    add r0, #0x53
    ldrb r0, [r0]
    cmp r0, #59
    bls @ok3
@fail:
    mov r0, #0
    bx lr
@ok3:
    mov r0, #1
    bx lr
}
