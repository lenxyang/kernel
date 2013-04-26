#include <linux/linkage.h>
#include <linux/types.h>
#include <linux/bootmem.h>
#include <asm/pgtable.h>
#include <asm/setup.h>

pteval_t __supported_pte_mask = ~(_PAGE_NX | _PAGE_GLOBAL | _PAGE_IOMAP);

static unsigned int highmem_pages = -1;

void __init lowmem_pfn_init(void) {
  max_low_pfn = max_pfn;

  if (highmem_pages == -1) {
    highmem_pages = 0;
  }

  /**
   * 对于支持 highmem 的内核来说，还需要一些额外操作
   */
}

void __init highmem_pfn_init(void) {
}

void __init find_low_pfn_range(void) {
  if (max_pfn <= MAXMEM_PFN) {
    lowmem_pfn_init();
  } else {
    highmem_pfn_init();
  }
}
