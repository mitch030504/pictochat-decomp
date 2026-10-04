// decomp: module=arm7 addr=0x022d83e0 name=FUN_022d83e0
// flags: -O4,s -noThumb
// size: 0xd0 - the nominal 0xcc excludes the trailing pool word.
//
// Compares `n` bytes of a candidate SSID against the one configured in the
// channel block at +0x344: bytes are masked by the table at +0x40 (a set mask
// bit means "don't care") and compared with the stored SSID at +0x20.
// Lengths over 32 fail, a zero-length stored SSID matches anything, and in
// mode 0x13 a longer candidate is truncated to the stored length instead of
// being rejected.

typedef unsigned short u16;

extern unsigned char FUN_022d8d6c(const unsigned char *p);

int FUN_022d83e0(unsigned int n, const unsigned char *data)
{
    unsigned char *state = *(unsigned char **)0x0380fff4;
    unsigned char *p = state + 0x344;
    unsigned int len;
    const unsigned char *mask;
    const unsigned char *ssid;
    unsigned char m;
    unsigned int i;
    unsigned char b;
    unsigned char c;

    if (n > 0x20) {
        return 0;
    }
    len = *(u16 *)(p + 0x1e);
    if (len == 0) {
        return 1;
    }
    if (*(u16 *)(state + 0x404) == 0x13) {
        if (n < len) {
            return 0;
        }
        n = len;
    } else if (n != len) {
        return 0;
    }

    ssid = p + 0x20;
    mask = p + 0x40;
    for (i = 0; i < n; i++) {
        m = FUN_022d8d6c(mask++);
        b = FUN_022d8d6c(data++);
        c = FUN_022d8d6c(ssid++);
        if ((b | m) != (c | m)) {
            return 0;
        }
    }
    return 1;
}
