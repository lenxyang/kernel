
#include "init.h"
#include "page.h"
#include "pgtable.h"
#include "processor.h"

uint32 init_pg_tables_end = ~0UL;

extern unsigned int __text32_end;

static pmd_t *one_md_table_init(pgd_t *pgd) {
  pud_t *pud;
  pmd_t *pmd_table;

  pud = pud_offset(pgd, 0);
  pmd_table = pmd_offset(pud, 0);
  return pmd_table;
}

static pte_t *one_page_table_init(pmd_t * pmd) {
  if (pmd_none(*pmd)) {
    pte_t *page_table = (pte_t*)alloc_bootmem_low_pages(PAGE_SIZE);
    set_pmd(pmd, __pmd(__pa(page_table) | _PAGE_TABLE));
    if (page_table != pte_offset_kernel(pmd, 0)) 
      BUG();

    return page_table;
  }

  return pte_offset_kernel(pmd, 0);
}

static inline int is_kernel_text(unsigned long addr) {
  if (addr > PAGE_OFFSET && addr < __text32_end) {
    return 1;
  } else {
    return 0;
  }
}

static void kernel_physical_mapping_init(pgd_t* pgd_base) {
  unsigned long pfn;
  pgd_t *pgd;
  pmd_t *pmd;
  pte_t *pte;
  int pgd_idx, pmd_idx, pte_ofs;

  pgd_idx = pgd_index(PAGE_OFFSET);
  pgd = pgd_base + pgd_idx;
  pfn = 0;

  for (; pgd_idx < PTRS_PER_PGD; pgd++, pgd_idx++) {
    pmd = one_md_table_init(pgd);
    if (pfn >= max_low_pfn) {
      continue;
    }

    for (pmd_idx = 0; pmd_idx < PTRS_PER_PMD && pfn < max_low_pfn;
         pmd++, pmd_idx++) {
      unsigned int address = pfn * PAGE_SIZE + PAGE_OFFSET;
      pte =  one_page_table_init(pmd);
      for (pte_ofs = 0; pte_ofs < PTRS_PER_PTE && pfn < max_low_pfn;
           pfn++, pte_ofs++) {
        // 设置可执行属性
        if (is_kernel_text(address)) {
          set_pte(pte, pfn_pte(pfn, PAGE_KERNEL_EXEC));
        } else {
          set_pte(pte, pfn_pte(pfn, PAGE_KERNEL));
        }
      }
    }
  }
}

static void pagetable_init() {
  int i;
  pgd_t *pgd_base = swapper_pg_dir;
  kernel_physical_mapping_init(pgd_base);
  // remap_numa_kva();

  /*
  vaddr = __fix_to_virt(__end_of_fixed_addresses - 1) & PMD_MASK;
  page_table_range_init(vaddr, 0, pgd_base);
  */
}


void pagging_init(void) {
  pagetable_init();

  load_cr3(swapper_pg_dir);

  // kmap_init();
  // zone_sizes_init();
}
