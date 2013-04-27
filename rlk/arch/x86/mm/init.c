#include <linux/mm.h>
#include <linux/string.h>

#include <asm/tlbflush.h>
#include <asm/e820.h>
#include <asm/sections.h>

unsigned long __initdata e820_table_start;
unsigned long __meminitdata e820_table_end;
unsigned long __meminitdata e820_table_top;

/**
 * 在真正的 linux 内核当中 after_bootmem 并没有被初始化为 0
 * 但实际上，在 arch/x86/kernel/head.S 当中会将整个
 * bss（未初始化的数据区） 区域初始化为 0，等同于将此
 * 变量进行了初始化
 */
int after_bootmem = 0;

struct map_range {
  unsigned long start;
  unsigned long end;
  unsigned page_size_mask;
};

#define NR_RANGE_MR 3

/**
 * 此函数的功能是完成对三个全局变量
 * e820_table_start, e820_table_end 和 e820_table_top 的功能
 */
static void __init find_early_table_space(unsigned long end, int use_pse,
                                          int use_gbpages) {
}

static int __meminit save_mr(struct map_range* mr, int nr_range,
                             unsigned long start_pfn,
                             unsigned long end_pfn,
                             unsigned long page_size_mask) {
  if (start_pfn < end_pfn) {
    if (nr_range >= NR_RANGE_MR) {
      panic("run out of range for init_memory_mapping\n");
    }

    mr[nr_range].start = start_pfn << PAGE_SHIFT;
    mr[nr_range].end = end_pfn << PAGE_SHIFT;
    mr[nr_range].page_size_mask = page_size_mask;
    nr_range++;
  }

  return nr_range;
}

/**
 * 函数 init_memory_mapping 用于计算分页的 mask 及区域
 * 并将这些信息保存在 map_range 的数组中，而后通过函数
 * init_physcial_memory.. 来真正完成内存映射
 * 最后通过 load_cr3 及 flush_tlb_cache 来更新页表
 */
unsigned long __init_refok init_memory_mapping(unsigned long start,
                                               unsigned long end) {
  unsigned long page_size_mask = 0;
  unsigned long start_pfn, end_pfn;
  unsigned long ret = 0;
  unsigned long pos;

  struct map_range mr[NR_RANGE_MR];
  int nr_range, i;
  int use_pse, use_gbpages;

  printk(KERN_INFO "init_memory_mapping: %016lx-%016lx\n", start, end);

  use_pse = 0;
  use_gbpages = 0;

  memset(mr, 0, sizeof(mr));
  nr_range = 0;

  start_pfn = start >> PAGE_SHIFT;
  pos = start_pfn << PAGE_SHIFT;

  /**
   * x86 仅仅采用两级分页架构，因此他没有 PMD 和 PUD
   * 对于这种情况，PMD 和 PUD 的值采用
   * include/asm-generic/pgtable-nopud.h 和
   * include/asm-generic/pgtable-nopmd.h 当中的定义
   * 其中 PMD_SHIFT = PUD_SHIFT = PGDIR_SHIFT = 22
   */
  if (pos == 0) {
    end_pfn = 1 << (PMD_SHIFT - PAGE_SHIFT);
  } else {
    end_pfn = ((pos  + (PMD_SIZE - 1)) >> PMD_SHIFT)
        << (PMD_SHIFT - PAGE_SHIFT);
  }

  if (end_pfn > (end >> PAGE_SHIFT)) {
    end_pfn = end >> PAGE_SHIFT;
  }

  /**
   * 第一个区域，这个区域从 0 到一个完整的 PGD 项，即 1024 项的区域
   * start_pfn = 0, end_pfn = 1024(phyaddr 0x400000)
   */
  if (start_pfn < end_pfn) {
    nr_range = save_mr(mr, nr_range, start_pfn, end_pfn, 0);
    pos = end_pfn << PAGE_SHIFT;
  }

  /**
   * 第二个区域, 这个区域覆盖到 end(物理内存结束处)
   * 0000400000 - 001fc00000 page 2M
   */
  start_pfn = ((pos + (PMD_SIZE - 1)) >> PMD_SHIFT)
      << (PMD_SHIFT - PAGE_SHIFT);
  end_pfn = (end >> PMD_SHIFT) << (PMD_SHIFT - PAGE_SHIFT);

  if (start_pfn < end_pfn) {
    nr_range = save_mr(mr, nr_range, start_pfn, end_pfn,
                       page_size_mask & (1<<PG_LEVEL_2M));
    pos = end_pfn << PAGE_SHIFT;
  }

  /**
   * 此处完成第三部分剩余寻址空间部分的映射
   * 即有物理内存以外一直到 4G 处
   */
  start_pfn = pos >> PAGE_SHIFT;
  end_pfn = end >> PAGE_SHIFT;
  nr_range = save_mr(mr, nr_range, start_pfn, end_pfn, 0);

  /* 这里的代码仅仅用来整理生成的 mr_range
   * 例如，如果仅仅有4M物理内存，那么第二段的开始及结尾相同
   * 它就没有存在的必要了。又或者，物理内存超过4G，此时第三段
   * 就不存在了。
   */
  for (i = 0; i < nr_range - 1 && nr_range > 1; i++) {
    unsigned long old_start;
    if (mr[i].end != mr[i+1].start ||
        mr[i].page_size_mask != mr[i+1].page_size_mask)
      continue;

    old_start = mr[i].start;
    memmove(&mr[i], &mr[i+1],
            (nr_range - 1 - i) * sizeof(struct map_range));
    nr_range--;
  }


  for (i = 0; i < nr_range; i++)
    printk(KERN_DEBUG " %010lx - %010lx page %s\n",
           mr[i].start, mr[i].end,
           (mr[i].page_size_mask & (1<<PG_LEVEL_1G))?"1G":(
               (mr[i].page_size_mask & (1<<PG_LEVEL_2M))?"2M":"4k"));

  if (!after_bootmem) {
    find_early_table_space(end, use_pse, use_gbpages);
  }

  /**
   * 根据配置，各个段创建实际的页表
   * 在三个段中，第一个段通常采用 4K 页大小，第二段采用 2M 页，而第三段
   * 它的 P 标志位未空，函数kernel_physcial_mapping_init 会据此初始化页表。
   */
  for (i = 0; i < nr_range; i++) {
    kernel_physical_mapping_init(mr[i].start, mr[i].end, mr[i].page_size_mask);
    ret = end;
  }

  /**
   * 函数 early_ioremap_page_table_range_init 定义在文件 init_32.c 当中
   * 
   */
  early_ioremap_page_table_range_init();

  /**
   * 加载页表并更新
   */
  load_cr3(swapper_pg_dir);
  __flush_tlb_all();

  /**
   * 在 e820 当中添加一个信箱, PGTALBE
   */
  if (!after_bootmem && e820_table_end > e820_table_start) {
    reserve_early(e820_table_start << PAGE_SHIFT,
                  e820_table_end << PAGE_SHIFT, "PGTABLE");
  }

  if (!after_bootmem)
    early_memtest(start, end);

  return ret >> PAGE_SHIFT;
}
