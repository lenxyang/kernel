#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/mm.h>

char boot_cmdline[1024];
asmlinkage void start_kernel(void) {
  
  page_address_init();

  setup_arch(&boot_cmdline);
  while (1) {
  }
}
