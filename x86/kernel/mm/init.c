#include "kernel.h"
#include "linkage.h"
#include "bootmem.h"
#include "pgtable.h"
#include "page.h"
#include "pfn.h"
#include "e820.h"
#include "setup.h"
#include "fixmap.h"
#include "bug.h"
#include "mm.h"
#include "nodemask.h"
#include "string.h"
#include "init.h"
#include "processor.h"

static unsigned int highmem_pages = -1;
pteval_t __supported_pte_mask = ~(_PAGE_NX | _PAGE_GLOBAL | _PAGE_IOMAP);

int after_bootmem;

#define MSG_HIGHMEM_TOO_BIG \
        "highmem size (%luMB) is bigger than pages available (%luMB)!\n"

#define MSG_LOWMEM_TOO_SMALL \
        "highmem size (%luMB) results in <64MB lowmem, ignoring it!\n"


extern unsigned int __end;
static inline int is_kernel_text(unsigned long addr) {
  if (addr >= PAGE_OFFSET && addr <= (unsigned long)__end) 
    return 1;
  else
    return 0;
}

static pte_t* one_page_table_init(pmd_t *pmd) {
  /*

  if (!(pmd_val(*pmd) & _PAGE_PRESENT)) {
    pte_t *page_table = NULL;

    if (after_bootmem) {
      if (!page_table)
        page_table = (pte_t*)alloc_bootmem_pages(PAGE_SIZE);
    } else {
      page_table = (pte_t*)alloc_low_page();
    }

    paravirt_alloc_pte(&init_mm, __pa(page_table) >> PAGE_SHIFT);
    set_pmd(pmd, __pmd(__pa(page_table) | PAGE_TABLE));
  }

  return pte_offset_kernel(pmd, 0);
  */
  return 0;
}

static pmd_t* one_md_table_init(pgd_t *pgd) {
  pud_t *pud;
  pmd_t *pmd_table;

  pud = pud_offset(pgd, 0);
  pmd_table = pmd_offset(pud, 0);
  return pmd_table;
}

unsigned long kernel_physical_mapping_init(unsigned long start,
                                           unsigned long end,
                                           unsigned long page_size_mask) {
  unsigned long start_pfn, end_pfn;
  pgd_t *pgd_base = (pgd_t*)swapper_pg_dir;
  int pgd_idx, pmd_idx, pte_ofs;
  unsigned long pfn;
  pgd_t *pgd;
  pmd_t *pmd;
  pte_t *pte;
  unsigned pages_2m, pages_4k;
  int mapping_iter = 1;

  start_pfn = start >> PAGE_SHIFT;
  end_pfn  = end >> PAGE_SHIFT;

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

    /*
     * 在两级分页模式下 PTRS_PER_PMD的值是 1， 因此一下循环只运行一次
     */

    pmd_idx = 0;
    for (; pmd_idx < PTRS_PER_PMD && pfn < end_pfn; pmd++, pmd_idx++) {
      unsigned int addr = pfn * PAGE_SIZE + PAGE_OFFSET;
      /* 初始化 pte */
      pte = one_page_table_init(pmd);
      pte_ofs = pte_index((pfn << PAGE_SHIFT) + PAGE_OFFSET);
      pte += pte_ofs;
      for (; pte_ofs < PTRS_PER_PTE && pfn < end_pfn; pte++, pfn++, pte_ofs++,
               addr += PAGE_SIZE) {
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
    mapping_iter = 2;
    goto repeat;
  }
  
  return 0;
}

static unsigned long setup_node_bootmem(int nodeid,
                                        unsigned long start_pfn,
                                        unsigned long end_pfn,
                                        unsigned long bootmap) {
  unsigned long bootmap_size;

  bootmap_size = init_bootmem_node(NODE_DATA(nodeid),
                                   bootmap >> PAGE_SHIFT,
                                   start_pfn, end_pfn);
  free_bootmem_with_active_regions(nodeid, end_pfn);
  early_res_to_bootmem(start_pfn << PAGE_SHIFT, end_pfn << PAGE_SHIFT);
  return bootmap + bootmap_size;
}

void setup_bootmem_allocator(void) {
  int nodeid;
  unsigned long bootmap_size, bootmap;
  bootmap_size = bootmem_bootmap_pages(max_low_pfn)<<PAGE_SHIFT;
  bootmap = find_e820_area(0, max_pfn_mapped<<PAGE_SHIFT, bootmap_size,
                           PAGE_SIZE);
  if (bootmap == -1L) {
    panic("Cannot find bootmem map of size %ld\n", bootmap_size);
  }

  reserve_early(bootmap, bootmap + bootmap_size, "BOOTMAP");

  for_each_online_node(nodeid) {
    unsigned int start_pfn, end_pfn;
    start_pfn = 0;
    end_pfn = max_low_pfn;

    bootmap = setup_node_bootmem(nodeid, start_pfn, end_pfn,
                                 bootmap);
  }

  after_bootmem = 1;
}

void lowmem_pfn_init(void) {
  max_low_pfn = max_pfn;

  if (highmem_pages == -1) {
    highmem_pages = 0;
  }

  if (highmem_pages >= max_pfn) {
    highmem_pages = 0;
    printk(KERN_ERR MSG_HIGHMEM_TOO_BIG,
             pages_to_mb(highmem_pages), pages_to_mb(max_pfn));
    highmem_pages = 0;
  }

  if (highmem_pages) {
    if (max_low_pfn - highmem_pages < 64*1024*1024/PAGE_SIZE) {
      printk(KERN_ERR MSG_LOWMEM_TOO_SMALL,
             pages_to_mb(highmem_pages));
      highmem_pages = 0; 
    }
    max_low_pfn -= highmem_pages;
  }
}

void  highmem_pfn_init(void) {
}

/*
 * Determine low and high memory ranges:
 */
void find_low_pfn_range(void)
{
  /* it could update max_pfn */

  if (max_pfn <= MAXMEM_PFN) {
    lowmem_pfn_init();
  } else {
    highmem_pfn_init();
  }
}

struct map_range {
  unsigned long start;
  unsigned long end;
  unsigned page_size_mask;
};

#define NR_RANGE_MR 3

static int save_mr(struct map_range*mr, int nr_range,
                   unsigned long start_pfn, unsigned long end_pfn,
                   unsigned long page_size_mask) {
  if (start_pfn < end_pfn) {
    if (nr_range >= NR_RANGE_MR) {
      panic("run out of range for init_memory_mapping.");
    }

    mr[nr_range].start = start_pfn<<PAGE_SHIFT;
    mr[nr_range].end   = end_pfn<<PAGE_SHIFT;
    mr[nr_range].page_size_mask = page_size_mask;
    nr_range++;
  }

  return nr_range;
}


/*
 * Setup the direct mapping of the physical memory at PAGE_OFFSET.
 * This runs before bootmem is initialized and gets pages directly from
 * the physical memory. To access them they are temporarily mapped.
 */
unsigned long init_memory_mapping(unsigned long start,
                                  unsigned long end) {
  unsigned long page_size_mask = 0;
  unsigned long start_pfn, end_pfn;
  unsigned long ret = 0;
  unsigned long pos;

  struct map_range mr[NR_RANGE_MR];
  int nr_range, i;
  int use_pse, use_gbpages;
  printk(KERN_INFO "init_memory_mapping: %016lx-%016lx\n", start, end);

  use_pse = use_gbpages = 0;

  if (use_gbpages) {
    page_size_mask |= 1 << PG_LEVEL_1G;
  }

  if (use_pse) {
    page_size_mask |= 1 << PG_LEVEL_2M;
  }

  memset(mr, 0, sizeof(mr));
  nr_range = 0;

  /*
   * 计算对其后的起始地址及起始页
  */
  start_pfn = start >> PAGE_SHIFT;
  pos = start_pfn << PAGE_SHIFT;

  if (pos == 0) {
    end_pfn = 1 << (PMD_SHIFT - PAGE_SHIFT);
  } else {
    end_pfn = ((pos + (PMD_SIZE - 1)) >> PMD_SHIFT)
        << (PMD_SHIFT - PAGE_SHIFT);
  }

  if (end_pfn > (end >> PAGE_SHIFT)) {
    end_pfn = end >> PAGE_SHIFT;
  }

  if (start_pfn < end_pfn) {
    nr_range = save_mr(mr, nr_range, start_pfn, end_pfn, 0);
    pos = end_pfn << PAGE_SHIFT;
  }

  start_pfn = pos >> PAGE_SHIFT;
  end_pfn = pos >> PAGE_SHIFT;
  nr_range = save_mr(mr, nr_range, start_pfn, end_pfn, 0);

  for (i = 0; i < nr_range - 1 && nr_range > 1; i++) {
    unsigned long old_start;
    if (mr[i].end != mr[i+1].start ||
        mr[i].page_size_mask != mr[i+1].page_size_mask)
      continue;

    old_start = mr[i].start;
    memmove(&mr[i], &mr[i+1],
            (nr_range - 1 - i) * sizeof(struct map_range));
    mr[i--].start = old_start;
    nr_range--;
  }

  for (i = 0; i < nr_range; i++) {
    /*
    printk(KERN_DEBUG " %010lx - %010lx page %s\n",
           mr[i].start, mr[i].end,
           (mr[i].page_size_mask & (1<<PG_LEVEL_1G))?"1G":(
               (mr[i].page_size_mask & (1<<PG_LEVEL_2M))?"2M":"4k"));
    */
  }

  /*
  if (!after_bootmem) {
    find_early_table_space(end, use_pse, use_gbpages);
  }
  */

  for (i = 0; i < nr_range; i++) {
    kernel_physical_mapping_init(mr[i].start, mr[i].end,
                                 mr[i].page_size_mask);
  }
  ret = end;

  early_ioremap_page_table_range_init();
  load_cr3((pgd_t*)swapper_pg_dir);


  if (!after_bootmem) {
    early_memtest(start, end);
  }

  return ret >> PAGE_SHIFT;
}



void early_ioremap_page_table_range_init(void) {
}

