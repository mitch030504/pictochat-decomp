// decomp: module=arm7 addr=0x022c8150 name=FUN_022c8150

typedef struct {
    int a;
    int b;
    int c;
} Small;

typedef struct {
    char pad0;
    unsigned char flag;
    char pad2[0x38 - 2];
    Small copy;
    int field44;
} State;

extern void FUN_022c864c(char *state, void *value);

int FUN_022c8150(State *state, Small *src, int value, void *arg)
{
    state->flag = 0;
    state->copy = *src;
    state->field44 = value;
    FUN_022c864c((char *)state, arg);
    return 1;
}
