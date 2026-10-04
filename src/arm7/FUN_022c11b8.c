// decomp: module=arm7 addr=0x022c11b8 name=FUN_022c11b8
// flags: -O4,s -noThumb
// size: 0x80 - the nominal 0x78 excludes the two trailing pool words
// (0x0380fff4 and the 0xbf1d tag).
//
// Counterpart of FUN_022c111c: frees a message block. Blocks without the
// 0xbf1d signature at +0xa are refused with 1. FUN_022c105c unlinks it from
// `owner`; only when that succeeds (0) is the memory returned, through
// whichever allocator +0x17c selects: 0 = the pool free FUN_022c5d90 over the
// heap pair at +0x180/+0x184, 1 = the indirect free whose pointer sits at
// +0x184.

typedef struct Ctx {
    unsigned char pad[0x17c];
    int allocator;  /* +0x17c */
    void *heap;     /* +0x180 */
    void *heapEnd;  /* +0x184 - also the function pointer for allocator 1 */
} Ctx;

typedef struct Blk {
    unsigned char pad[8];
    unsigned short state; /* +0x8 */
    unsigned short tag;   /* +0xa */
} Blk;

extern Ctx *G_0380fff4;
extern int FUN_022c105c(int owner, Blk *blk);
extern void FUN_022c5d90(void *heap, void *heapEnd, Blk *blk);

int FUN_022c11b8(int owner, Blk *blk)
{
    Ctx *c = G_0380fff4;
    int r;

    if (blk->tag != 0xbf1d) {
        return 1;
    }
    r = FUN_022c105c(owner, blk);
    if (r == 0) {
        switch (c->allocator) {
        case 0:
            FUN_022c5d90(c->heap, c->heapEnd, blk);
            break;
        case 1:
            ((void (*)(Blk *))c->heapEnd)(blk);
            break;
        }
    }
    return r;
}
