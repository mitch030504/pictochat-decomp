// decomp: module=arm7 addr=0x022c826c name=FUN_022c826c
extern unsigned short FUN_022c8600(int value);

void FUN_022c826c(void *object, int value)
{
    *(unsigned short *)((char *)object + 0x20) = FUN_022c8600(value);
}
