#ifndef __MMZONE_H__
#define __MMZONE_H__

#include "bitops.h"
#include "numa.h"
#include "kernel.h"

enum zone_stat_item {
  ZONE_DMA,
  ZONE_DMA32,
  ZONE_NORMAL,
  ZONE_HIGHMEM,
  ZONE_MOVABLE,
  __MAX_NR_ZONES,
};

#define MAX_NR_ZONES __MAX_NR_ZONES

#if MAX_NR_ZONES < 2
#define ZONES_SHIFT 0
#elif MAX_NR_ZONES <= 2
#define ZONES_SHIFT 1
#elif MAX_NR_ZONES <= 4
#define ZONES_SHIFT 2
#else
#error ZONES_SHIFT -- too many zones configured adjust calculation
#endif

// NUMA 时，它大于1jd
#define MAX_ZONELISTS 1

/* Maximum number of zones on a zonelist */
#define MAX_ZONES_PER_ZONELIST (MAX_NUMNODES * MAX_NR_ZONES)

extern struct page* mem_map;

enum zone_watermarks {
  WMARK_MIN,
  WMARK_LOW,
  WMARK_HIGH,
  NR_WMARK
};



struct zone {
  unsigned long watermark[NR_WMARK];
};

struct zonelist_cache {
  unsigned short z_to_n[MAX_ZONES_PER_ZONELIST];          /* zone->nid */
  DECLARE_BITMAP(fullzones, MAX_ZONES_PER_ZONELIST);      /* zone full? */
  unsigned long last_full_zap;            /* when last zap'd (jiffies) */
};

struct zoneref {
  struct zone* zone;
  int zone_idx;
};

struct zonelist {
  struct zonelist_cache* zlcache_ptr;
  struct zoneref _zonerefs[MAX_ZONES_PER_ZONELIST + 1];
};

typedef struct pglist_data {
  struct zone node_zone[MAX_NR_ZONES];
  struct zonelist node_zonelists[MAX_ZONELISTS];
  int nr_zones;

  struct bootmem_data* bdata;

  unsigned long node_start_pfn;
  unsigned long node_present_pfn;
  unsigned long node_spanned_pages;

  int node_id;
}pg_data_t;

struct node_active_region {
  unsigned long start_pfn;
  unsigned long end_pfn;
  int nid;
};

extern struct pglist_data *node_data[];
#define NODE_DATA(nid)  (node_data[nid])

#endif  // __MMZONE_H__
