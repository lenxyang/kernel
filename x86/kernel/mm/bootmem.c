
#include "page.h"
#include "pgtable.h"
#include "kernel.h"
#include "internal.h"
#include "segment.h"
#include "bootmem.h"
#include "bug.h"
#include "linkage.h"
#include "list.h"
#include "io.h"
#include "mm.h"
#include "pfn.h"
#include "errno.h"
#include "string.h"
#include "processor.h"

unsigned long max_low_pfn;
unsigned long min_low_pfn;
unsigned long max_pfn;

static struct list_head bdata_list = LIST_HEAD_INIT(bdata_list);

static unsigned long bootmap_bytes(unsigned long pages) {
  unsigned long bytes = (pages + 7) / 8;

  return ALIGN(bytes, sizeof(long));
}

static void __free(bootmem_data_t *bdata, unsigned long sidx,
                   unsigned long eidx) {
  unsigned long idx;

  if (bdata->hint_idx > sidx) {
    bdata->hint_idx = sidx;
  }

  for (idx = sidx; idx < eidx; idx++) {
    if (!test_and_clear_bit(idx, bdata->node_bootmem_map)) {
      BUG();
    }
  }
}

static int __reserve(bootmem_data_t* bdata, unsigned long sidx,
                     unsigned long eidx, int flags) {
  unsigned long idx;
  int exclusive  = flags & BOOTMEM_EXCLUSIVE;
  for (idx  = sidx; idx < eidx; idx++) {
    if (test_and_set_bit(idx, bdata->node_bootmem_map)) {
      if (exclusive) {
        __free(bdata, sidx, idx);
        return -EBUSY;
      }
    }
  }

  return 0;
}


/**
 * bootmem_bootmap_pages - calculate bitmap size in pages
 * @pages: number of pages the bitmap has to represent
 */
unsigned long bootmem_bootmap_pages(unsigned long pages)
{
  unsigned long bytes = bootmap_bytes(pages);

  return PAGE_ALIGN(bytes) >> PAGE_SHIFT;
}

static int mark_bootmem_node(bootmem_data_t *bdata,
                             unsigned long start, unsigned long end,
                             int reserve, int flags) {
  unsigned long sidx, eidx;

  sidx = start - bdata->node_min_pfn;
  eidx = end - bdata->node_min_pfn;

  if (reserve) {
    return __reserve(bdata, sidx, eidx, flags);
  } else {
    __free(bdata, sidx, eidx);
  }

  return 0;
}

static int mark_bootmem(unsigned long start, unsigned long end,
                        int reserve, int flags) {
  unsigned long pos;
  bootmem_data_t *bdata;

  pos = start;
  list_for_each_entry(bdata, &bdata_list, list) {
    int err;
    unsigned long max;

    if (pos < bdata->node_min_pfn ||
        pos >= bdata->node_low_pfn) {
      BUG_ON(pos != start);
      continue;
    }

    max = min(bdata->node_low_pfn, end);

    err = mark_bootmem_node(bdata, pos, max, reserve, flags);
    if (reserve && err) {
      mark_bootmem(start, pos, 0, 0);
      return err;
    }

    if (max == end)
      return 0;
    pos = bdata->node_low_pfn;
  }
  BUG();
}
                        

/*
 * free_bootmem_node - mark a page range as usabled
 */
void free_bootmem_node(pg_data_t *pgdat, unsigned long physaddr,
                       unsigned long size) {
  unsigned long start, end;

  start = PFN_UP(physaddr);
  end = PFN_DOWN(physaddr + size);
  mark_bootmem_node(pgdat->bdata, start, end, 0, 0);
}

static void link_bootmem(bootmem_data_t *bdata) {
  struct list_head *iter;

  list_for_each(iter, &bdata_list) {
    bootmem_data_t *ent;
    ent = list_entry(iter, bootmem_data_t, list);
    if (bdata->node_min_pfn < ent->node_min_pfn)
      break;
  }

  list_add_tail(&bdata->list, iter);
}

static unsigned long init_bootmem_core(bootmem_data_t *bdata,
                                       unsigned long mapstart,
                                       unsigned long start,
                                       unsigned long end) {
  unsigned long mapsize;

  // mminit_validate_memmodel_limits(&start, &end);
  bdata->node_bootmem_map = phys_to_virt(PFN_PHYS(mapstart));
  bdata->node_min_pfn = start;
  bdata->node_low_pfn = end;
  link_bootmem(bdata);

  /*
   * Initially all pages are reserved - setup_arch() has to
   * register free RAM areas explicitly.
   */
  mapsize = bootmap_bytes(end - start);
  memset(bdata->node_bootmem_map, 0xff, mapsize);

  return mapsize;
}
/**
 * init_bootmem_node - register a node as boot memory
 * @pgdat: node to register
 * @freepfn: pfn where the bitmap for this node is to be placed
 * @startpfn: first pfn on the node
 * @endpfn: first pfn after the node
 *
 * Returns the number of bytes needed to hold the bitmap for this node.
 */
unsigned long init_bootmem_node(pg_data_t *pgdat, unsigned long freepfn,
                                       unsigned long startpfn, unsigned long endpfn)
{
  return init_bootmem_core(pgdat->bdata, freepfn, startpfn, endpfn);
}

/**
 * init_bootmem - register boot memory
 * @start: pfn where the bitmap is to be placed
 * @pages: number of available physical pages
 *
 * Returns the number of bytes needed to hold the bitmap.
 */
unsigned long init_bootmem(unsigned long start, unsigned long pages)
{
  max_low_pfn = pages;
  min_low_pfn = start;
  return init_bootmem_core(NODE_DATA(0)->bdata, start, 0, pages);
}


/**
 * reserve_bootmem - mark a page range as usable
 * @addr: starting address of the range
 * @size: size of the range in bytes
 * @flags: reservation flags (see linux/bootmem.h)
 *
 * Partial pages will be reserved.
 *
 * The range must be contiguous but may span node boundaries.
 */
int reserve_bootmem(unsigned long addr, unsigned long size,
                    int flags) {
  unsigned long start, end;

  start = PFN_DOWN(addr);
  end = PFN_UP(addr + size);

  return mark_bootmem(start, end, 1, flags);
}

void reserve_bootmem_generic(unsigned long phys, unsigned long len,
                             int flags) {
  return reserve_bootmem(phys, len, flags);
}
