// decomp: module=unk_autoload_0 addr=0x02321350 name=FUN_02321350
#pragma thumb on
extern char _sym_0236db34[];
extern char _sym_0236db74[];

void FUN_02321350(void)
{
    *(short *)(_sym_0236db34 + 0x10) = 1;
    *(void **)(_sym_0236db34 + 0x14) = _sym_0236db74;
}
