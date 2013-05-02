#include <linux/stddef.h>
#include <linux/mm.h>
#include <linux/types.h>
#include <linux/linkage.h>
#include <linux/mmzone.h>

#include "internal.h"

#define MAX_ACTIVE_REGIONS 256

static struct node_active_region __meminitdata early_node_map[MAX_ACTIVE_REGIONS];
static int __meminitdata nr_nodemap_entries;

/**
 * add_active_range - Register a range of PFNs backed by physical memory
 * @nid: The node ID the range resides on
 * @start_pfn: The start PFN of the available physical memory
 * @end_pfn: The end PFN of the available physical memory
 *
 * These ranges are stored in an early_node_map[] and later used by
 * free_area_init_nodes() to calculate zone sizes and holes. If the
 * range spans a memory hole, it is up to the architecture to ensure
 * the memory is not freed by the bootmem allocator. If possible
 * the range being registered will be merged with existing ranges.
 */
void __init add_active_range(unsigned int nid, unsigned long start_pfn,
                             unsigned long end_pfn) {
  int i;

  mminit_validate_memmodel_limits(&start_pfn, &end_pfn);

  for (i = 0; i < nr_nodemap_entries; i++) {
    if (early_node_map[i].nid != nid) {
      continue;
    }

    if (start_pfn >= early_node_map[i].start_pfn &&
        end_pfn <= early_node_map[i].end_pfn) {
      return;
    }

    if (start_pfn <= early_node_map[i].end_pfn &&
        end_pfn >= early_node_map[i].end_pfn) {
      return;
    }

    if (start_pfn < early_node_map[i].end_pfn &&
        end_pfn >= early_node_map[i].start_pfn) {
      early_node_map[i].start_pfn = start_pfn;
      return;
    }
  }

  if (i >= MAX_ACTIVE_REGIONS) {
    printk(KERN_CRIT "More than %d memory regions, truncating\n",
           MAX_ACTIVE_REGIONS);
    return;
  }

  early_node_map[i].nid = nid;
  early_node_map[i].start_pfn = start_pfn;
  early_node_map[i].end_pfn = end_pfn;
  nr_nodemap_entries = i + 1;
}

/* Basic iterator support to walk early_node_map[] */
#define for_each_active_range_index_in_nid(i, nid) \
  for (i = first_active_region_index_in_nid(nid); i != -1; \
       i = next_active_region_index_in_nid(i, nid))

/*
 * Basic iterator support. Return the first range of PFNs for a node
 * Note: nid == MAX_NUMNODES returns first region regardless of node
 */
static int __meminit first_active_region_index_in_nid(int nid)
{
  int i;

  for (i = 0; i < nr_nodemap_entries; i++)
    if (nid == MAX_NUMNODES || early_node_map[i].nid == nid)
      return i;

  return -1;
}

/*
 * Basic iterator support. Return the next active range of PFNs for a node
 * Note: nid == MAX_NUMNODES returns next region regardless of node
 */
static int __meminit next_active_region_index_in_nid(int index, int nid)
{
  for (index = index + 1; index < nr_nodemap_entries; index++)
    if (nid == MAX_NUMNODES || early_node_map[index].nid == nid)
      return index;

  return -1;
}

/**
 * sparse_memory_present_with_active_regions - Call memory_present
 * for each active range
 * @nid: The node to call memory_present for. If MAX_NUMNODES,
 * all nodes will be used.
 *
 * If an architecture guarantees that all ranges registered with
 * add_active_ranges() contain no holes and may be freed, this
 * function may be used instead of calling memory_present() manually.
 */
void __init sparse_memory_present_with_active_regions(int nid)
{
  int i;

  for_each_active_range_index_in_nid(i, nid)
      memory_present(early_node_map[i].nid,
                     early_node_map[i].start_pfn,
                     early_node_map[i].end_pfn);
}

/**
 * free_bootmem_with_active_regions - Call free_bootmem_node for each active range
 * @nid: The node to free memory on. If MAX_NUMNODES, all nodes are freed.
 * @max_low_pfn: The highest PFN that will be passed to free_bootmem_node
 *
 * If an architecture guarantees that all ranges registered with
 * add_active_ranges() contain no holes and may be freed, this
 * this function may be used instead of calling free_bootmem() manually.
 */
void __init free_bootmem_with_active_regions(int nid,
                                             unsigned long max_low_pfn)
{
  int i;

  for_each_active_range_index_in_nid(i, nid) {
    unsigned long size_pages = 0;
    unsigned long end_pfn = early_node_map[i].end_pfn;

    if (early_node_map[i].start_pfn >= max_low_pfn)
      continue;

    if (end_pfn > max_low_pfn)
      end_pfn = max_low_pfn;

    size_pages = end_pfn - early_node_map[i].start_pfn;
    free_bootmem_node(NODE_DATA(early_node_map[i].nid),
                      PFN_PHYS(early_node_map[i].start_pfn),
                      size_pages << PAGE_SHIFT);
  }
}
