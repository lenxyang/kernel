#include "screen.h"

void kernel_main() {
  set_position(10, 0);
  char msg[] = "Hello, Kernel C!\r\n";
  puts(msg);

  while (1) {
  }
}
