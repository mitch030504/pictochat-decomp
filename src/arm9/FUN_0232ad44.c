// decomp: module=unk_autoload_0 addr=0x0232ad44 name=FUN_0232ad44
extern char G_023bd7b4[];
extern void func_02337584(void *src, void *dst, int size);
extern void FUN_0232a9f8(void);
extern int FUN_0232996c(void);
extern void FUN_02329bd8(int a);

void FUN_0232ad44(unsigned char *p) {
    unsigned char buf[8];
    func_02337584(p, buf, 8);
    func_02337584(p + 8, G_023bd7b4, 0x60);
    FUN_0232a9f8();
    if (FUN_0232996c() == 8 || FUN_0232996c() == 9) {
        FUN_02329bd8(6);
    }
}
