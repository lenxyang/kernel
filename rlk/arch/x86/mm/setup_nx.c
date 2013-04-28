#include <linux/init.h>
#include <linux/mm.h>

#ifdef CONFIG_X86_PAE
void __init set_nx(void) {
  /*
   * lots of code
   */
}
#else
void __init set_nx(void) {
}
#endif
