#include <linux/bootmem.h>
#include <linux/kmemleak.h>

#include <asm/pgtable.h>
#include <asm/processor.h>

#include "internal.h"

unsigned long max_low_pfn;
unsigned long min_low_pfn;
unsigned long max_pfn;

bootmem_data_t bootmem_node_data[MAX_NUMNODES] __initdata;

static struct list_head bdata_list __initdata = LIST_HEAD_INIT(bdata_list);

static int bootmem_debug;

static int __init bootmem_debug_setup(char *buf)
{
  bootmem_debug = 1;
  return 0;
}

#define bdebug(fmt, args...) ({                         \
      if (unlikely(bootmem_debug))                      \
        printk(KERN_INFO                                \
               "bootmem::%s " fmt,                      \
               __func__, ## args);                      \
    })


static unsigned long __init bootmap_bytes(unsigned long pages) {
  unsigned long bytes = (pages + 7) / 8;
  return ALIGN(bytes, sizeof(long));
}

/**
 * 计算需要多个少个页来保存 bootmap
 */
unsigned long __init bootmem_bootmap_pages(unsigned long pages) {
  unsigned long bytes = bootmap_bytes(pages);
  return PAGE_ALIGN(bytes) >> PAGE_SHIFT;
}

/*
 * link bdata in order
 */
static void __init link_bootmem(bootmem_data_t *bdata) {
  struct list_head *iter;

  list_for_each(iter, &bdata_list) {
    bootmem_data_t *ent;

    ent = list_entry(iter, bootmem_data_t, list);
    if (bdata->node_min_pfn < ent->node_min_pfn)
      break;
  }
  list_add_tail(&bdata->list, iter);
}


static void __init __free(bootmem_data_t *bdata,
                          unsigned long sidx, unsigned long eidx)
{
  unsigned long idx;

  bdebug("nid=%td start=%lx end=%lx\n", bdata - bootmem_node_data,
         sidx + bdata->node_min_pfn,
         eidx + bdata->node_min_pfn);

  if (bdata->hint_idx > sidx)
    bdata->hint_idx = sidx;

  for (idx = sidx; idx < eidx; idx++)
    if (!test_and_clear_bit(idx, bdata->node_bootmem_map))
      BUG();
}

static int __init __reserve(bootmem_data_t *bdata, unsigned long sidx,
                            unsigned long eidx, int flags)
{
  unsigned long idx;
  int exclusive = flags & BOOTMEM_EXCLUSIVE;

  bdebug("nid=%td start=%lx end=%lx flags=%x\n",
         bdata - bootmem_node_data,
         sidx + bdata->node_min_pfn,
         eidx + bdata->node_min_pfn,
         flags);

  for (idx = sidx; idx < eidx; idx++)
    if (test_and_set_bit(idx, bdata->node_bootmem_map)) {
      if (exclusive) {
        __free(bdata, sidx, idx);
        return -EBUSY;
      }
      bdebug("silent double reserve of PFN %lx\n",
             idx + bdata->node_min_pfn);
    }
  return 0;
}


static int __init mark_bootmem_node(bootmem_data_t *bdata,
                                    unsigned long start, unsigned long end,
                                    int reserve, int flags)
{
  unsigned long sidx, eidx;

  bdebug("nid=%td start=%lx end=%lx reserve=%d flags=%x\n",
         bdata - bootmem_node_data, start, end, reserve, flags);

  BUG_ON(start < bdata->node_min_pfn);
  BUG_ON(end > bdata->node_low_pfn);

  sidx = start - bdata->node_min_pfn;
  eidx = end - bdata->node_min_pfn;

  if (reserve)
    return __reserve(bdata, sidx, eidx, flags);
  else
    __free(bdata, sidx, eidx);
  return 0;
}

static int __init mark_bootmem(unsigned long start, unsigned long end,
                               int reserve, int flags)
{
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
 * Called once to set up the allocator itself.
 */
static unsigned long __init init_bootmem_core(bootmem_data_t *bdata,
                                              unsigned long mapstart,
                                              unsigned long start, unsigned long end)
{
  unsigned long mapsize;

  mminit_validate_memmodel_limits(&start, &end);
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

  bdebug("nid=%td start=%lx map=%lx end=%lx mapsize=%lx\n",
         bdata - bootmem_node_data, start, mapstart, end, mapsize);

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
unsigned long __init init_bootmem_node(pg_data_t *pgdat, unsigned long freepfn,
                                       unsigned long startpfn, unsigned long endpfn)
{
  return init_bootmem_core(pgdat->bdata, freepfn, startpfn, endpfn);
}

/**
 * free_bootmem_node - mark a page range as usable
 * @pgdat: node the range resides on
 * @physaddr: starting address of the range
 * @size: size of the range in bytes
 *
 * Partial pages will be considered reserved and left as they are.
 *
 * The range must reside completely on the specified node.
 */
void __init free_bootmem_node(pg_data_t *pgdat, unsigned long physaddr,
                              unsigned long size)
{
  unsigned long start, end;

  kmemleak_free_part(__va(physaddr), size);

  start = PFN_UP(physaddr);
  end = PFN_DOWN(physaddr + size);

  mark_bootmem_node(pgdat->bdata, start, end, 0, 0);
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
int __init reserve_bootmem(unsigned long addr, unsigned long size,
                           int flags)
{
  unsigned long start, end;

  start = PFN_DOWN(addr);
  end = PFN_UP(addr + size);

  return mark_bootmem(start, end, 1, flags);
}
