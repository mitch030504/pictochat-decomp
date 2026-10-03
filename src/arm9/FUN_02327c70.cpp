//cpp
// decomp: module=unk_autoload_0 addr=0x02327c70 name=FUN_02327c70
extern "C" int FUN_02325b74(unsigned int, void *, int);
extern "C" void FUN_02320978(int);
extern "C" void FUN_0232571c(void *);
extern "C" void FUN_02327c70(unsigned int *p) {
  unsigned char value[4];
  if (FUN_02325b74(p[15], value, 1)) FUN_02320978(4);
  else FUN_02320978(15);
  FUN_0232571c(p);
}
