// decomp: module=unk_autoload_0 addr=0x02321168 name=FUN_02321168
#pragma thumb on
extern int FUN_02321158(const void *p, int val);

int FUN_02321168(const void *p, int val)
{
    int r = FUN_02321158(p, val);
    switch (r) {
    case 0:
    case 1:
        return 3;
    case 2:
        return 15;
    default:
        return 2;
    }
}
