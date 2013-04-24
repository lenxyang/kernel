#include "boot.h"
#include "processor-flags.h"

void initregs(struct biosregs *reg)
{
  memset(reg, 0, sizeof(struct biosregs));
  reg->eflags |= X86_EFLAGS_CF;
  reg->ds = ds();
  reg->es = ds();
  reg->fs = fs();
  reg->gs = gs();
}
