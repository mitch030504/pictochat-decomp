// decomp: module=unk_autoload_0 addr=0x0232981c name=FUN_0232981c
extern char G_023bd5e0[];
extern void FUN_023298c0(void);
extern void FUN_02329858(void);
extern void FUN_0232987c(void);

int FUN_0232981c(void) {
    int r = 0;
    if (*(int *)(G_023bd5e0 + 0x10) != 0) {
        r = 1;
    } else {
        switch (*(int *)(G_023bd5e0 + 0x18)) {
        case 1:
            FUN_023298c0();
            r = 1;
            break;
        case 2:
            FUN_02329858();
            r = 1;
            break;
        case 3:
            FUN_0232987c();
            r = 1;
            break;
        }
    }
    return r;
}
