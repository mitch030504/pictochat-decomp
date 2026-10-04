// decomp: module=arm7 addr=0x022e0078 name=FUN_022e0078
// flags: -O4,s -noThumb
// size: 0x74 - the nominal 0x70 excludes the trailing pool word.
//
// Serialises the SSID element (id 0) at `dst` from the channel block at
// +0x344: length from +0x1e, bytes from +0x20. Returns the element's total
// length.

typedef unsigned short u16;

extern void FUN_022d8d40(unsigned char *p, unsigned char v);
extern unsigned char FUN_022d8d6c(const unsigned char *p);

int FUN_022e0078(unsigned char *dst)
{
    unsigned int i;
    int n;
    u16 len;
    unsigned char *p;

    n = 0;
    p = *(unsigned char **)0x0380fff4 + 0x344;
    len = *(u16 *)(p + 0x1e);

    FUN_022d8d40(dst + n, 0);
    FUN_022d8d40(dst + 1, len);
    n += 2;
    for (i = 0; i < len; i++) {
        FUN_022d8d40(dst + n, FUN_022d8d6c(p + 0x20 + i));
        n++;
    }
    return n;
}
