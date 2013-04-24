#include "internal.h"
#include "page.h"
#include "nodemask.h"
#include "mmzone.h"
#include "pfn.h"

#define MAX_ACTIVE_REGIONS 256
static struct node_active_region early_node_map[MAX_ACTIVE_REGIONS];
static int nr_nodemap_entries;

/*
 * Array of node states.
 */
nodemask_t node_states[NR_NODE_STATES] = {
  [N_POSSIBLE] = ~0L, /*NODE_MASK_ALL,*/
  [N_ONLINE] = { { [0] = 1UL } },
  [N_NORMAL_MEMORY] = { { [0] = 1UL } },
  [N_HIGH_MEMORY] = { { [0] = 1UL } },
  [N_CPU] = { { [0] = 1UL } },
};

/* Basic iterator support to walk early_node_map[] */
#define for_each_active_range_index_in_nid(i, nid) \
  for (i = first_active_region_index_in_nid(nid); i != -1; \
       i = next_active_region_index_in_nid(i, nid))

/*
 * Basic iterator support. Return the first range of PFNs for a node
 * Note: nid == MAX_NUMNODES returns first region regardless of node
 */
static int first_active_region_index_in_nid(int nid)
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
static int next_active_region_index_in_nid(int index, int nid)
{
  for (index = index + 1; index < nr_nodemap_entries; index++)
    if (nid == MAX_NUMNODES || early_node_map[index].nid == nid)
      return index;

  return -1;
}

void free_bootmem_with_active_regions(int nid,
                                      unsigned long max_low_pfn) {
  int i;

  for_each_active_range_index_in_nid(i, nid) {
    unsigned long size_pages = 0;
    unsigned long end_pfn = early_node_map[i].end_pfn;

    if (early_node_map[i].start_pfn >= max_low_pfn) {
      continue;
    }

    if (end_pfn > max_low_pfn) {
      end_pfn = max_low_pfn;
    }

    size_pages = end_pfn - early_node_map[i].start_pfn;
    free_bootmem_node(NODE_DATA(early_node_map[i].nid),
                      PFN_PHYS(early_node_map[i].start_pfn),
                      size_pages << PAGE_SHIFT);
  };
}

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
void add_active_range(unsigned nid, unsigned long start_pfn,
                      unsigned long end_pfn) {
  
}
