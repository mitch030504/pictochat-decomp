// decomp: module=unk_autoload_0 addr=0x02331c10 name=FUN_02331c10
extern char G_023c0ae0[];
extern void FUN_02331be8(int a);
extern void FUN_02330728(int irq, void (*fn)(void));
extern void FUN_023307d4(int irq);
extern void FUN_02331c68(void);

void FUN_02331c10(void) {
    if (*(unsigned short *)G_023c0ae0 != 0) {
        return;
    }
    *(unsigned short *)G_023c0ae0 = 1;
    FUN_02331be8(0);
    *(int *)(G_023c0ae0 + 8) = 0;
    *(int *)(G_023c0ae0 + 0xc) = 0;
    *(volatile unsigned short *)0x04000102 = 0;
    *(volatile unsigned short *)0x04000100 = 0;
    *(volatile unsigned short *)0x04000102 = 0xc1;
    FUN_02330728(8, FUN_02331c68);
    FUN_023307d4(8);
    *(int *)(G_023c0ae0 + 4) = 0;
}
