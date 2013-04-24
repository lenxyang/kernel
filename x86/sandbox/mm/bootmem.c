#include "page.h"

unsigned long max_low_pfn;
unsigned long min_low_pfn;
unsigned long max_pfn;

static unsigned long init_bootmem_core(pg_data_t *pgdat,
                                       unsigned long mapstart,
                                       unsigned long start,
                                       unsigned long end) {
  bootmem_data_t *bdata = pgdat->bdata;
  unsigned long mapsize = ((end - start) + 7) / 8;

  pgdat->pgdat_next = pgdat_list;
  pgdat_list = pgdat;

  mapsize = (mapsize + (sizeof(long) - 1U)) & ~(sizeof(long) - 1UL);
}

unsigned long init_bootmem(unsigned long start, unsigned long pages) {
  max_low_pfn = pages;
  min_low_pfn = starts;
  return (init_bootmem_core(NODE_DATA(0), start, 0, pages));
}
