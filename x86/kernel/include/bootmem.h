#ifndef __BOOTMEM_H__
#define __BOOTMEM_H__

#include "list.h"
#include "mmzone.h"

/*
 * Flags for reserve_bootmem (also if CONFIG_HAVE_ARCH_BOOTMEM_NODE,
 * the architecture-specific code should honor this).
 *
 * If flags is 0, then the return value is always 0 (success). If
 * flags contains BOOTMEM_EXCLUSIVE, then -EBUSY is returned if the
 * memory already was reserved.
 */
#define BOOTMEM_DEFAULT         0
#define BOOTMEM_EXCLUSIVE       (1<<0)

typedef struct bootmem_data {
  unsigned long node_min_pfn;
  unsigned long node_low_pfn;
  void* node_bootmem_map;
  unsigned long last_end_off;
  unsigned long hint_idx;
  struct list_head list;
} bootmem_data_t;

extern unsigned long max_low_pfn;
extern unsigned long min_low_pfn;
extern unsigned long max_pfn;

unsigned long bootmem_bootmap_pages(unsigned long pages);

int reserve_bootmem(unsigned long addr, unsigned long size,
                    int flags);

void reserve_bootmem_generic(unsigned long phys, unsigned long end,
                             int flags);

void free_bootmem_node(pg_data_t *pgdat, unsigned long phyaddr,
                       unsigned long size);

#endif  // __BOOTMEM_H__
