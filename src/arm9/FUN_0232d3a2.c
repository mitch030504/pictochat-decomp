// decomp: module=unk_autoload_0 addr=0x0232d3a2 name=FUN_0232d3a2
// NOTE: verify with --addr 0x0232d3a2 --size 0x28, not the batch's wrongly
// declared FUN_0232d3c8/0x2e - that address is this function's own tail
// `bx lr`, not its start. This is a leaf function (no push/pop): entry is
// wherever the previous function's `bx lr` at 0x0232d3a0 ends.

int FUN_0232d3a2(unsigned char *p)
{
    if (p[0x50] > 0x1a)
        return 0;
    if (p[0x52] > 0x17 || p[0x53] > 0x3b)
        return 0;
    return 1;
}
