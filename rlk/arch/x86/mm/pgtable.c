#include <linux/mm.h>
#include <asm/pgtable.h>


int fixmaps_set;

void __native_set_fixmap(enum fixed_addresses idx, pte_t pte)
{
  unsigned long address = __fix_to_virt(idx);

  if (idx >= __end_of_fixed_addresses) {
    BUG();
    return;
  }
  set_pte_vaddr(address, pte);
  fixmaps_set++;
}

void native_set_fixmap(enum fixed_addresses idx, phys_addr_t phys,
                       pgprot_t flags)
{
  __native_set_fixmap(idx, pfn_pte(phys >> PAGE_SHIFT, flags));
}
