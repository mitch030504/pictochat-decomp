//cpp
// decomp: module=unk_autoload_0 addr=0x023345d0 name=FUN_023345d0
// Find the first matching signed byte, including the string terminator.
extern "C" char *FUN_023345d0(char *str, int value) {
    char target;
    char *cursor;
    char ch;

    target = (char)value;
    cursor = str;
    ch = *cursor++;
    while (ch != 0) {
        if (ch == target) return cursor - 1;
        ch = *cursor++;
    }
    if (target != 0) return 0;
    return cursor - 1;
}
