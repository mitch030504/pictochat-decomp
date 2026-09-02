//cpp
// decomp: module=unk_autoload_0 addr=0x0232344c name=FUN_0232344c
// flags: -thumb
#pragma thumb on
struct State {
    int unk0;
    int unk4;
    int unk8;
    int unkc;
    int unk10;
    void *unk14;
    int unk18;
    int unk1c;
    void *unk20;
    int unk24;
    int unk28;
    short unk2c;
    short unk2e;
    short unk30;
    short unk32;
    int unk34;
};

extern "C" State unk_0238ef0c;
extern "C" void *unk_0238ef30;
extern "C" void *unk_0238ef18;
extern "C" void *unk_0233af2c;
extern "C" void *unk_0233fc8c;
extern "C" void *unk_0233af3c;
extern "C" void *unk_0233fdb4;
extern "C" void *unk_0233af4c;
extern "C" void *unk_0233fd44;
extern "C" void *unk_0233af5c;
extern "C" void *unk_0233fef4;
extern "C" void *unk_0233af6c;
extern "C" void *unk_0233fe84;

extern "C" void FUN_02324f0c(void *, int, int);
extern "C" void FUN_02322f74(void *);
extern "C" void FUN_02323068(void *);
extern "C" void FUN_02323e74(void *);

extern "C" void FUN_0232344c(int arg0) {
    unk_0238ef0c.unk34 = arg0;
    unk_0238ef0c.unkc = 0;
    unk_0238ef0c.unk10 = 0;
    unk_0238ef0c.unk2c = 0;
    unk_0238ef0c.unk2e = 0;
    unk_0238ef0c.unk32 = 0;
    unk_0238ef0c.unk30 = 0;
    FUN_02324f0c(&unk_0238ef30, 0x28, 5);
    switch (arg0) {
        case 0:
            unk_0238ef0c.unk14 = &unk_0233af2c;
            unk_0238ef0c.unk20 = (char *)&unk_0233fc8c + 2;
            break;
        case 1:
            unk_0238ef0c.unk14 = &unk_0233af3c;
            unk_0238ef0c.unk20 = (char *)&unk_0233fdb4 + 2;
            unk_0238ef0c.unk32 = 4;
            break;
        case 2:
            unk_0238ef0c.unk14 = &unk_0233af4c;
            unk_0238ef0c.unk20 = (char *)&unk_0233fd44 + 2;
            break;
        case 3:
            unk_0238ef0c.unk14 = &unk_0233af5c;
            unk_0238ef0c.unk20 = (char *)&unk_0233fef4 + 2;
            break;
        case 4:
            unk_0238ef0c.unk14 = &unk_0233af6c;
            unk_0238ef0c.unk20 = (char *)&unk_0233fe84 + 2;
            break;
    }
    FUN_02322f74(&unk_0238ef18);
    FUN_02323068(&unk_0238ef18);
    FUN_02323e74(&unk_0238ef18);
}
