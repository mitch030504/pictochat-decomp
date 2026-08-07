// decomp: module=arm7 addr=0x022d6c8c name=FUN_022d6c8c
// flags: -noThumb

void FUN_022d6c8c(unsigned int *state, unsigned short value)
{
    state[0] = 0xffffffff;
    state[1] = 0xffffffff;
    ((unsigned short *)state)[4] = 0;
    ((unsigned short *)state)[5] = value;
}
