#include <linux/kernel.h>

unsigned long long native_read_tsc(void)
{
  return __native_read_tsc();
}
