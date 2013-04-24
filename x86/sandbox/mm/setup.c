#include "setup.h"
#include "page.h"

unsigned long find_max_low_pfn(void) {
  
}

static unsigned long setup_memory(void) {
  unsigned long bootmap_size, start_pfn, max_low_pfn;

  start_pfn = PFN_UP(init_pg_tables_end);
  find_max_pfn();
  max_low_pfn = find_max_low_pfn();

  bootmap_size = init_bootmem(start_pfn, max_low_pfn);
}

void setup_arch() {
  max_low_pfn = setup_memory();
}
