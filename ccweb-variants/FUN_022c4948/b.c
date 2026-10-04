// decomp: module=arm7 addr=0x022c4948 name=FUN_022c4948
// flags: -O4,s -noThumb
// size: 0x74 - the nominal 0x68 excludes the three trailing pool words.
//
// One-time init of the ARM9/ARM7 handshake in shared main RAM: marks itself
// done, clears the sync word at 0x02fffff6, waits (0x400 ticks at a time)
// until the ARM9 posts 0x7f at 0x02fffff4, initialises the two lock-ID
// bitmaps at 0x02ffffb8 and answers 0xbf.

extern volatile int G_03804f64;
extern void FUN_022c49bc(int delay);

void FUN_022c4948(void)
{
    volatile unsigned short *sync = (volatile unsigned short *)0x02fffff0;

    if (G_03804f64 != 0) {
        return;
    }
    G_03804f64 = 1;
    sync[3] = 0;
    while (sync[2] != 0x7f) {
        FUN_022c49bc(0x400);
    }
    *(volatile unsigned int *)0x02ffffb8 = 0xffffffff;
    *(volatile unsigned int *)0x02ffffbc = 0xffff0000;
    sync[3] = 0xbf;
}
