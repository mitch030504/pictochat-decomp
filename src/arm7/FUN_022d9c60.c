// decomp: module=arm7 addr=0x022d9c60 name=FUN_022d9c60
// flags: -O4,s -noThumb
// NONMATCHING: right size and shape; registers differ in both arms (the ROM keeps the state-slot address in r3 and reloads through it); volatile slot pointer is the closest spelling (div=19). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 2.0/sp1..sp2p4 (see notes/matching-style.md).
// Counts as decompiled, not matched.
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

extern char *G_0380fff4;

#define ST G_0380fff4

void FUN_022d9c60(int peer, unsigned int status)
{
    char *volatile *pp;
    int token = FUN_037c9084(0x01000000);

    pp = (char *volatile *)&G_0380fff4;

    if (status < 0x40) {
        *(u16 *)(*pp + 0x530) |= 1 << peer;
        *(u16 *)(*pp + 0x532) |= 1 << peer;
        if (*(u16 *)(*pp + 0x350) == 1 && FUN_022da0e8(peer) != 0) {
            FUN_022d9f7c(peer);
        }
    } else {
        *(u16 *)(*pp + 0x532) &= ~(1 << peer);
        if ((*(u16 *)(*pp + 0x52e) >> peer) & 1) {
            FUN_022d9dc4(peer);
        }
    }
    (*(Entry **)(ST + 0x31c))[peer].status = status;
    FUN_037c904c(token);
}
