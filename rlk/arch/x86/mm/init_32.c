#include <linux/linkage.h>
#include <linux/types.h>
#include <linux/bootmem.h>
#include <linux/sched.h>
#include <linux/mm.h>

#include <asm/e820.h>
#include <asm/pgtable.h>
#include <asm/setup.h>
#include <asm/fixmap.h>
#include <asm/tlbflush.h>
#include <asm/pgalloc.h>

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

static inline int is_kernel_text(unsigned long addr) {
  if (addr >= PAGE_OFFSET && addr <= (unsigned long)__init_end) {
    return 1;
  }

  return 0;
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

static pte_t *__init one_page_table_init(pmd_t* pmd) {
  if (!(pmd_val(*pmd) & _PAGE_PRESENT)) {
    /**
     * 此处代码在初始化阶段不会执行
     */
    /*
    pte_t *page_table = NULL;
    if (after_bootmem) {
      if (!page_table)
        page_table = (pte_t*)alloc_bootmem_pages(PAGE_SIZE);
    } else {
      page_table = (pte_t*)alloc_low_page();
    }

    paravirt_alloc_pte(&init_mm, __pa(page_table) >> PAGE_SHIFT);
    set_pmd(pmd, __pmd(__pa(page_table) | _PAGE_TABLE));
    */
  }

  return pte_offset_kernel(pmd, 0);
}

static pmd_t *__init one_md_table_init(pgd_t* pgd) {
  pud_t *pud;
  pmd_t *pmd_table;

  pud = pud_offset(pgd, 0);
  pmd_table = pmd_offset(pud, 0);

  return pmd_table;
}

unsigned long __init kernel_physical_mapping_init(unsigned long start,
                                                  unsigned long end,
                                                  unsigned long page_size_mask) {
  int use_pse = page_size_mask == (1 << PG_LEVEL_2M);
  unsigned long start_pfn, end_pfn;
  pgd_t *pgd_base = swapper_pg_dir;
  int pgd_idx, pmd_idx, pte_ofs;
  unsigned long pfn;
  pgd_t *pgd;
  pmd_t *pmd;
  pte_t *pte;
  unsigned pages_2m, pages_4k;
  int mapping_iter;

  start_pfn = start >> PAGE_SHIFT;
  end_pfn   = end >> PAGE_SHIFT;

  mapping_iter = 1;

repeat:
  pages_2m = pages_4k = 0;
  pfn = start_pfn;
  pgd_idx = pgd_index((pfn << PAGE_SHIFT) + PAGE_OFFSET);
  pgd = pgd_base + pgd_idx;

  for (; pgd_idx < PTRS_PER_PGD; pgd++, pgd_idx++) {
    pmd = one_md_table_init(pgd);
    if (pfn >= end_pfn) {
      continue;
    }

    /** only when pae enabled, pmd_idx is usedable */
    pmd_idx = 0;

    for (; pmd_idx < PTRS_PER_PMD && pfn < end_pfn; pmd++, pmd_idx++) {
      unsigned int addr = pfn * PAGE_SIZE + PAGE_OFFSET;

      pte = one_page_table_init(pmd);
      pte_ofs = ((pfn << PAGE_SHIFT) + PAGE_OFFSET);
      pte += pte_ofs;

      for (; pte_ofs < PTRS_PER_PTE && pfn < end_pfn;
           pte++, pfn++, pte_ofs++, addr += PAGE_SIZE) {
        pgprot_t prot = PAGE_KERNEL;
        pgprot_t init_prot = __pgprot(PTE_IDENT_ATTR);
        if (is_kernel_text(addr)) {
          prot = PAGE_KERNEL_EXEC;
        }

        pages_4k++;
        if (mapping_iter == 1) {
          set_pte(pte, pfn_pte(pfn, init_prot));
        } else {
          set_pte(pte, pfn_pte(pfn, prot));
        }
      }
    }
  }
  if (mapping_iter == 1) {
    update_page_count(PG_LEVEL_4K, pages_4k);
    update_page_count(PG_LEVEL_2M, pages_2m);
    
    __flush_tlb_all();
    
    mapping_iter = 2;
    goto repeat;
  }

  return 0;
}

static pte_t *page_table_kmap_check(pte_t* pte, pmd_t *pmd,
                                    unsigned long vaddr, pte_t* lastpte) {
  /*
   * 此处代码大多数是为了初始 HIGHMEM
   */
  return pte;
}

void __init
page_table_range_init(unsigned long start, unsigned long end, pgd_t* pgd_base) {
  int pgd_idx, pmd_idx;
  unsigned long vaddr;
  pgd_t *pgd;
  pmd_t *pmd;
  pte_t *pte = NULL;

  vaddr = start;
  pgd_idx = pgd_index(vaddr);
  pmd_idx = pmd_index(vaddr);
  pgd = pgd_base + pgd_idx;

  for (; (pgd_idx < PTRS_PER_PGD) && (vaddr != end); pgd++, pgd_idx++) {
    pmd = one_md_table_init(pgd);
    pmd = pmd + pmd_index(vaddr);
    for (; (pmd_idx < PTRS_PER_PMD) && (vaddr != end);
         pmd++, pmd_idx++) {
      pte = page_table_kmap_check(one_page_table_init(pmd),
                                  pmd, vaddr, pte);
      vaddr += PMD_SIZE;
    }

    pmd_idx = 0;
  }
}

/*
 * Build a proper pagetable for the kernel mappings.  Up until this
 * point, we've been running on some set of pagetables constructed by
 * the boot process.
 *
 * If we're booting on native hardware, this will be a pagetable
 * constructed in arch/x86/kernel/head_32.S.  The root of the
 * pagetable will be swapper_pg_dir.
 *
 * If we're booting paravirtualized under a hypervisor, then there are
 * more options: we may already be running PAE, and the pagetable may
 * or may not be based in swapper_pg_dir.  In any case,
 * paravirt_pagetable_setup_start() will set up swapper_pg_dir
 * appropriately for the rest of the initialization to work.
 *
 * In general, pagetable_init() assumes that the pagetable may already
 * be partially populated, and so it avoids stomping on any existing
 * mappings.
 */
void __init early_ioremap_page_table_range_init(void) {
  pgd_t *pgd_base = swapper_pg_dir;
  unsigned long vaddr, end;

  /**
   * fixmap 包括多个区域，通过 enum 定义，其中最后一个是
   * __end_of_fixed_addresses
   * 函数 __fix_to_virt 将区域的 id （实际上是区域的下标)
   * 转换为虚拟地址
   * 转换的方式非常简单 FIXMAP_TOP - ((id << PAGE_SHIFT))
   */
  vaddr = __fix_to_virt(__end_of_fixed_addresses - 1) & PMD_MASK;
  end = (FIXADDR_TOP + PMD_SIZE - 1) & PMD_MASK;
  
  page_table_range_init(vaddr, end, pgd_base);
  early_ioremap_reset();
}
