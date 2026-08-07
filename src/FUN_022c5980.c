// decomp: module=arm7 addr=0x022c5980 name=FUN_022c5980
// flags: -noThumb

void FUN_022c5980(char *list, char *node)
{
    char *previous = *(char **)(list + 0x70);

    if (previous == 0) {
        *(char **)(list + 0x6C) = node;
    } else {
        *(char **)(previous + 0x10) = node;
    }
    *(char **)(node + 0x14) = previous;
    *(char **)(node + 0x10) = 0;
    *(char **)(list + 0x70) = node;
}
