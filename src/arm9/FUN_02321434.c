// decomp: module=unk_autoload_0 addr=0x02321434 name=FUN_02321434
#pragma thumb on

struct Obj02321434 {
    char pad[0x24];
    void *f24;
};

extern void FUN_023217ec(void *, void *, int, int, int, int, int, int);

void FUN_02321434(struct Obj02321434 *obj, int arg1, int arg2)
{
    FUN_023217ec(obj->f24, obj, arg1, arg2, 0x0000ffff, 0, 0, -1);
}
