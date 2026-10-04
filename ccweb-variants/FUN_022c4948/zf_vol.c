// decomp: module=arm7 addr=0x022c4948 name=FUN_022c4948
// flags: -O4,s -noThumb
// size: 0x74 - the nominal 0x68 excludes the three trailing pool words.

typedef volatile struct OSLockWord {
    unsigned int lockFlag;
    unsigned short ownerID;
    unsigned short extension;
} OSLockWord;

extern int G_03804f64;
extern void FUN_022c49bc(int delay);

void FUN_022c4948(void)
{
    OSLockWord *lockp = (OSLockWord *)0x02fffff0;

    if (G_03804f64 != 0) {
        return;
    }
    lockp->extension = 0;
    G_03804f64 = 1;
    while (lockp->ownerID != 0x7f) {
        FUN_022c49bc(0x400);
    }
    ((volatile unsigned int *)0x02ffffb8)[0] = 0xffffffff;
    ((volatile unsigned int *)0x02ffffb8)[1] = 0xffff0000;
    lockp->extension = 0xbf;
}
