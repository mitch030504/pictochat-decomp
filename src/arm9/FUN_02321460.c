// decomp: module=unk_autoload_0 addr=0x02321460 name=FUN_02321460
#pragma thumb on

struct Obj02321460 {
    char pad[0x1e];
    unsigned short f1E;
    int f20;
    void *f24;
};

extern void FUN_023217ec(void *, void *, int, int, int, int, int, int);

void FUN_02321460(struct Obj02321460 *obj, int arg1)
{
    FUN_023217ec(obj->f24, obj, obj->f20, obj->f1E, 0x0000ffff, arg1, 0, -1);
}
