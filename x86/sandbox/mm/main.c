#include "screen.h"
#include "linkage.h"

asmlinkage void start_kernel(void) {
  set_position(10, 0);
  char msg[] = "Hello, Kernel C!\r\n";
  puts(msg);
  
  page_address_init();

  while (1) {
  }
}
