#ifndef __MM_H__
#define __MM_H__

#include "linkage.h"

#define PAGE_ALIGN(addr) ALIGN(addr, PAGE_SIZE)

extern void add_active_range(unsigned int nid, unsigned long start_pfn,
                             unsigned long end_pfn);

extern void update_page_count(int level, unsigned long pages);
extern unsigned long kernel_physical_mapping_init(unsigned long start,
                                           unsigned long end,
                                           unsigned long page_size_mask);


void early_ioremap_page_table_range_init(void);

void early_memtest(unsigned long start, unsigned long end);

#endif  // __MM_H__
