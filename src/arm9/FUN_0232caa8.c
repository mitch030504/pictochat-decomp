// decomp: module=unk_autoload_0 addr=0x0232caa8 name=FUN_0232caa8
extern int FUN_0232c210(int a, int b);
extern void FUN_0232c3bc(int a, int b);
extern void FUN_0232c884(void);
extern int *FUN_0232c4d0(void);
extern int FUN_0232c408(int a, int b, int c, int d, int e, int f);

int FUN_0232caa8(int a0, int a1, int a2, int a3) {
    int r = FUN_0232c210(a0, a2);
    if (r == 0) {
        int *p;
        FUN_0232c3bc(0, a1);
        FUN_0232c884();
        p = FUN_0232c4d0();
        r = FUN_0232c408(0, 4, p[0], p[1], p[4], a3);
        if (r == 0) {
            r = 2;
        }
    }
    return r;
}
