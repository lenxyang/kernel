#include "bootparam.h"
#include "e820.h"
#include "init.h"
#include "page.h"
#include "kernel.h"
#include "bootmem.h"

struct screen_info screen_info;
struct boot_params __initdata boot_params;

/*
 * end_pfn only includes RAM, while max_pfn_mapped includes all e820 entries.
 * The direct mapping extends to max_pfn_mapped, so that we can directly access
 * apertures, ACPI and other tables without having to play with fixmaps.
 */

unsigned long max_low_pfn_mapped;
unsigned long max_pfn_mapped;

// arch/x86/mm/init_32.c
void initmem_init(unsigned long start_pfn, unsigned long end_pfn) {
  
}

void setup_arch(void) {  
  setup_memory_map();

  finish_e820_parsing();

  max_pfn = e820_end_of_ram_pfn();

  /* preallocate 4k for mptable mpc */
  /*
  early_reserve_e820_mpc_new();
  */
  
  /* update e820 for memory not covered by WB MTRRs */
  /*
  mtrr_bp_init();
  if (mtrr_trim_uncached_memory(max_pfn)) {
    max_pfn = e820_end_of_ram_pfn();
  }
  */

  /* max_low_pfn get updated here */
  find_low_pfn_range();

  printk(KERN_DEBUG "initial memory mapped : 0 - %08lx\n",
         max_pfn_mapped<<PAGE_SHIFT);

  max_low_pfn_mapped = init_memory_mapping(0, max_low_pfn<<PAGE_SHIFT);
  max_pfn_mapped = max_low_pfn_mapped;

  initmem_init(0, max_pfn);
}
