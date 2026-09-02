// decomp: module=unk_autoload_0 addr=0x02321364 name=FUN_02321364
#pragma thumb on
extern char _sym_0236db34[];
extern char _sym_0237db74[];
extern char _sym_0236db54[];

void *FUN_02321364(void)
{
    *(short *)(_sym_0236db34 + 0x30) = 0;
    *(void **)(_sym_0236db34 + 0x34) = _sym_0237db74;
    return _sym_0236db54;
}
