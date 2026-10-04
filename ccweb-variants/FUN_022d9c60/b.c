// decomp: module=arm7 addr=0x022d9c60 name=FUN_022d9c60
// flags: -noThumb
// size: 0xe8 - the nominal 0xe4 excludes the trailing pool word.
//
// Records a new status for peer `peer` under the interrupt lock. A status
// below 0x40 marks the peer present in both bitmaps at +0x530/+0x532 and,
// in link mode 1, kicks it when FUN_022da0e8 says it is ready; 0x40 and above
// clear it from +0x532 and drop it if it was still flagged at +0x52e. The
// status itself is stored in the peer's 0x1c-byte entry.

typedef unsigned short u16;

typedef struct Entry {
    u16 status;
    char pad[0x1a];
} Entry;

extern int FUN_037c9084(int lock);
extern void FUN_037c904c(int token);
extern unsigned short FUN_022da0e8(int idx);
extern void FUN_022d9f7c(int idx);
extern void FUN_022d9dc4(int idx);

#define ST (*(char **)0x0380fff4)

void FUN_022d9c60(int peer, unsigned int status)
{
    int token = FUN_037c9084(0x01000000);

    if (status < 0x40) {
        *(u16 *)(ST + 0x530) |= 1 << peer;
        *(u16 *)(ST + 0x532) |= 1 << peer;
        if (*(u16 *)(ST + 0x350) == 1 && FUN_022da0e8(peer) != 0) {
            FUN_022d9f7c(peer);
        }
    } else {
        *(u16 *)(ST + 0x532) &= ~(1 << peer);
        if ((*(u16 *)(ST + 0x52e) >> peer) & 1) {
            FUN_022d9dc4(peer);
        }
    }
    (*(Entry **)(ST + 0x31c))[peer].status = status;
    FUN_037c904c(token);
}
