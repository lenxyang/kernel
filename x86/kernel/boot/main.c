#include "boot/boot.h"

struct boot_params boot_params __attribute__((aligned(16)));

int main(void) {
  memcpy(&boot_params.hdr, &hdr, sizeof hdr);

  detect_memory();
  go_to_protected_mode();
  return 0;
}
