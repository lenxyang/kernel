#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/mm.h>

asmlinkage void start_kernel(void) {

  page_address_init();
  
  while (1) {
  }
}
