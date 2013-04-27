#include <linux/init.h>
#include <linux/sched.h>

#include <asm/e820.h>
#include <asm/pgtable.h>
#include <asm/fixmap.h>

static unsigned long slot_virt[FIX_BTMAPS_SLOTS] __initdata;

void __init early_ioremap_init(void) {
  int i;
  pmd_t *pmd;
  for (i = 0; i < FIX_BTMAPS_SLOTS; i++) {
    slot_virt[i] = __fix_to_virt(FIX_BTMAP_BEGIN - NR_FIX_BTMAPS*i);
  }
}

static __initdata int after_paging_init = 0;
static pte_t bm_pte[PAGE_SIZE / sizeof(pte_t)] __page_aligned_bss;

void __init early_ioremap_reset(void) {
  after_paging_init = 1;
}
