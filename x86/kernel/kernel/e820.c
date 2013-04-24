#include "e820.h"
#include "page.h"
#include "kernel.h"
#include "init.h"
#include "string.h"
#include "x86_init.h"
#include "setup.h"
#include "proto.h"
#include "bootparam.h"
#include "bootmem.h"

struct e820map e820;
struct e820map e820_saved;

extern struct boot_params __initdata boot_params;

#define MAX_ARCH_PFN          (1U<<(32-PAGE_SHIFT))


/*
 * Find the highest page frame number we have available
 */
static unsigned long e820_end_pfn(unsigned long limit_pfn, unsigned type)
{
  int i;
  unsigned long last_pfn = 0;
  unsigned long max_arch_pfn = MAX_ARCH_PFN;

  for (i = 0; i < e820.nr_map; i++) {
    struct e820entry *ei = &e820.map[i];
    unsigned long start_pfn;
    unsigned long end_pfn;

    if (ei->type != type)
      continue;

    start_pfn = ei->addr >> PAGE_SHIFT;
    end_pfn = (ei->addr + ei->size) >> PAGE_SHIFT;

    if (start_pfn >= limit_pfn)
      continue;
    if (end_pfn > limit_pfn) {
      last_pfn = limit_pfn;
      break;
    }
    if (end_pfn > last_pfn)
      last_pfn = end_pfn;
  }

  if (last_pfn > max_arch_pfn)
    last_pfn = max_arch_pfn;

  printk(KERN_INFO "last_pfn = %#lx max_arch_pfn = %#lx\n",
         last_pfn, max_arch_pfn);
  return last_pfn;
}

unsigned long e820_end_of_ram_pfn(void) {
  return e820_end_pfn(MAX_ARCH_PFN, E820_RAM);
}

static void __e820_add_region(struct e820map *e820x, u64 start, u64 size,
                                     int type) {
  int x = e820x->nr_map;

  if (x >= ARRAY_SIZE(e820x->map)) {
    printk(KERN_ERR "Ooops! Too many entries in the memory map!\n");
    return;
  }

  e820x->map[x].addr = start;
  e820x->map[x].size = size;
  e820x->map[x].type = type;
  e820x->nr_map++;
}

void e820_add_region(u64 start, u64 size, int type)
{
  __e820_add_region(&e820, start, size, type);
}

static int __append_e820_map(struct e820entry *biosmap, int nr_map) {
  while (nr_map) {
    u64 start = biosmap->addr;
    u64 size = biosmap->size;
    u64 end = start + size;
    u32 type = biosmap->type;

    /* Overflow in 64 bits? Ignore the memory map. */
    if (start > end)
      return -1;

    e820_add_region(start, size, type);

    biosmap++;
    nr_map--;
  }
  return 0;
}

/*
 * If we're lucky and live on a modern system, the setup code
 * will have given us a memory map that we can use to properly
 * set up memory.  If we aren't, we'll fake a memory map.
 */
static int append_e820_map(struct e820entry *biosmap, int nr_map)
{
        /* Only one memory region (or negative)? Ignore it */
        if (nr_map < 2)
                return -1;

        return __append_e820_map(biosmap, nr_map);
}

/*
 * Early reserved memory areas.
 */
#define MAX_EARLY_RES 20

struct early_res {
  u64 start, end;
  char name[16];
  char overlap_ok;
};
static struct early_res early_res[MAX_EARLY_RES] __initdata = {
  { 0, PAGE_SIZE, "BIOS data page" },     /* BIOS data page */
  {}
};


static int find_overlapped_early(u64 start, u64 end)
{
  int i;
  struct early_res *r;

  for (i = 0; i < MAX_EARLY_RES && early_res[i].end; i++) {
    r = &early_res[i];
    if (end > r->start && start < r->end)
      break;
  }

  return i;
}

/* Check for already reserved areas */
static inline int bad_addr(u64 *addrp, u64 size, u64 align)
{
  int i;
  u64 addr = *addrp;
  int changed = 0;
  struct early_res *r;
again:
  i = find_overlapped_early(addr, addr + size);
  r = &early_res[i];
  if (i < MAX_EARLY_RES && r->end) {
    *addrp = addr = round_up(r->end, align);
    changed = 1;
    goto again;
  }
  return changed;
}

/* Check for already reserved areas */
static inline int bad_addr_size(u64 *addrp, u64 *sizep, u64 align) {
  int i;
  u64 addr = *addrp, last;
  u64 size = *sizep;
  int changed = 0;
again:
  last = addr + size;
  for (i = 0; i < MAX_EARLY_RES && early_res[i].end; i++) {
    struct early_res *r = &early_res[i];
    if (last > r->start && addr < r->start) {
      size = r->start - addr;
      changed = 1;
      goto again;
    }
    if (last > r->end && addr < r->end) {
      addr = round_up(r->end, align);
      size = last - addr;
      changed = 1;
      goto again;
    }
    if (last <= r->end && addr >= r->start) {
      (*sizep)++;
      return 0;
    }
  }
  if (changed) {
    *addrp = addr;
    *sizep = size;
  }
  return changed;
}

u64 find_e820_area(u64 start, u64 end, u64 size, u64 align)
{
  int i;

  for (i = 0; i < e820.nr_map; i++) {
    struct e820entry *ei = &e820.map[i];
    u64 addr, last;
    u64 ei_last;

    if (ei->type != E820_RAM)
      continue;
    addr = round_up(ei->addr, align);
    ei_last = ei->addr + ei->size;
    if (addr < start)
      addr = round_up(start, align);
    if (addr >= ei_last)
      continue;
    while (bad_addr(&addr, size, align) && addr+size <= ei_last)
      ;
    last = addr + size;
    if (last > ei_last)
      continue;
    if (last > end)
      continue;
    return addr;
  }
  return -1ULL;
}


int sanitize_e820_map(struct e820entry *biosmap, int max_nr_map,
                             u32 *pnr_map) {
  struct change_member {
    struct e820entry *pbios; /* pointer to original bios entry */
    unsigned long long addr; /* address for this change point */
  };
  static struct change_member change_point_list[2*E820_X_MAX] __initdata;
  static struct change_member *change_point[2*E820_X_MAX] __initdata;
  static struct e820entry *overlap_list[E820_X_MAX] __initdata;
  static struct e820entry new_bios[E820_X_MAX] __initdata;
  struct change_member *change_tmp;
  unsigned long current_type, last_type;
  unsigned long long last_addr;
  int chgidx, still_changing;
  int overlap_entries;
  int new_bios_entry;
  int old_nr, new_nr, chg_nr;
  int i;

  /* if there's only one memory region, don't bother */
  if (*pnr_map < 2)
    return -1;

  old_nr = *pnr_map;
  // BUG_ON(old_nr > max_nr_map);

  /* bail out if we find any unreasonable addresses in bios map */
  for (i = 0; i < old_nr; i++)
    if (biosmap[i].addr + biosmap[i].size < biosmap[i].addr)
      return -1;
  
  /* create pointers for initial change-point information (for sorting) */
  for (i = 0; i < 2 * old_nr; i++)
    change_point[i] = &change_point_list[i];

  /* record all known change-points (starting and ending addresses),
     omitting those that are for empty memory regions */
  chgidx = 0;
  for (i = 0; i < old_nr; i++)    {
    if (biosmap[i].size != 0) {
      change_point[chgidx]->addr = biosmap[i].addr;
      change_point[chgidx++]->pbios = &biosmap[i];
      change_point[chgidx]->addr = biosmap[i].addr +
          biosmap[i].size;
      change_point[chgidx++]->pbios = &biosmap[i];
    }
  }
  chg_nr = chgidx;

  /* sort change-point list by memory addresses (low -> high) */
  still_changing = 1;
  while (still_changing)  {
    still_changing = 0;
    for (i = 1; i < chg_nr; i++)  {
      unsigned long long curaddr, lastaddr;
      unsigned long long curpbaddr, lastpbaddr;

      curaddr = change_point[i]->addr;
      lastaddr = change_point[i - 1]->addr;
      curpbaddr = change_point[i]->pbios->addr;
      lastpbaddr = change_point[i - 1]->pbios->addr;

      /*
       * swap entries, when:
       *
       * curaddr > lastaddr or
       * curaddr == lastaddr and curaddr == curpbaddr and
       * lastaddr != lastpbaddr
       */
      if (curaddr < lastaddr ||
          (curaddr == lastaddr && curaddr == curpbaddr &&
           lastaddr != lastpbaddr)) {
        change_tmp = change_point[i];
        change_point[i] = change_point[i-1];
        change_point[i-1] = change_tmp;
        still_changing = 1;
      }
    }
  }

  /* create a new bios memory map, removing overlaps */
  overlap_entries = 0;     /* number of entries in the overlap table */
  new_bios_entry = 0;      /* index for creating new bios map entries */
  last_type = 0;           /* start with undefined memory type */
  last_addr = 0;           /* start with 0 as last starting address */

  /* loop through change-points, determining affect on the new bios map */
  for (chgidx = 0; chgidx < chg_nr; chgidx++) {
    /* keep track of all overlapping bios entries */
    if (change_point[chgidx]->addr ==
        change_point[chgidx]->pbios->addr) {
      /*
       * add map entry to overlap list (> 1 entry
       * implies an overlap)
       */
      overlap_list[overlap_entries++] =
          change_point[chgidx]->pbios;
    } else {
      /*
       * remove entry from list (order independent,
       * so swap with last)
       */
      for (i = 0; i < overlap_entries; i++) {
        if (overlap_list[i] ==
            change_point[chgidx]->pbios)
          overlap_list[i] =
              overlap_list[overlap_entries-1];

      }
      overlap_entries--;
    }
    /*
     * if there are overlapping entries, decide which
     * "type" to use (larger value takes precedence --
     * 1=usable, 2,3,4,4+=unusable)
     */
    current_type = 0;
    for (i = 0; i < overlap_entries; i++)
      if (overlap_list[i]->type > current_type)
        current_type = overlap_list[i]->type;
    /*
     * continue building up new bios map based on this
     * information
     */
    if (current_type != last_type)  {
      if (last_type != 0)      {
        new_bios[new_bios_entry].size =
            change_point[chgidx]->addr - last_addr;
        /*
         * move forward only if the new size
         * was non-zero
         */
        if (new_bios[new_bios_entry].size != 0)
          /*
           * no more space left for new
           * bios entries ?
           */
          if (++new_bios_entry >= max_nr_map)
            break;
      }
      if (current_type != 0)  {
        new_bios[new_bios_entry].addr =
            change_point[chgidx]->addr;
        new_bios[new_bios_entry].type = current_type;
        last_addr = change_point[chgidx]->addr;
      }
      last_type = current_type;
    }
  }
  /* retain count for new bios entries */
  new_nr = new_bios_entry;

  /* copy new bios mapping into original location */
  memcpy(biosmap, new_bios, new_nr * sizeof(struct e820entry));
  *pnr_map = new_nr;

  return 0;
}

static void e820_print_type(u32 type)
{
  switch (type) {
    case E820_RAM:
    case E820_RESERVED_KERN:
      printk(KERN_CONT "(usable)");
      break;
    case E820_RESERVED:
      printk(KERN_CONT "(reserved)");
      break;
    case E820_ACPI:
      printk(KERN_CONT "(ACPI data)");
      break;
    case E820_NVS:
      printk(KERN_CONT "(ACPI NVS)");
      break;
    case E820_UNUSABLE:
      printk(KERN_CONT "(unusable)");
      break;
    default:
      printk(KERN_CONT "type %u", type);
      break;
  }
}


void e820_print_map(char *who) {
  int i;

  for (i = 0; i < e820.nr_map; i++) {
    printk(KERN_INFO " %s: %016Lx - %016Lx ", who,
           (unsigned long long) e820.map[i].addr,
           (unsigned long long)
           (e820.map[i].addr + e820.map[i].size));
    e820_print_type(e820.map[i].type);
    printk(KERN_CONT "\n");
  }
}

int e820_find_active_region(const struct e820entry* ei,
                            unsigned long start_pfn,
                            unsigned long last_pfn,
                            unsigned long* ei_startpfn,
                            unsigned long* ei_endpfn) {
  u64 align = PAGE_SIZE;
  *ei_startpfn = round_up(ei->addr, align) >> PAGE_SHIFT;
  *ei_endpfn = round_up(ei->addr + ei->size, align) >> PAGE_SHIFT;

  if (*ei_startpfn >= *ei_endpfn) {
    return 0;
  }

  if (ei->type != E820_RAM || *ei_endpfn <= start_pfn
      || *ei_startpfn >= last_pfn) {
    return 0;
  }

  if (*ei_startpfn <= start_pfn) {
    *ei_startpfn = start_pfn;
  }

  if (*ei_endpfn > last_pfn) {
    *ei_endpfn = last_pfn;
  }

  return 1;
}

void e820_register_active_regions(int nid, unsigned long start_pfn,
                                  unsigned long last_pfn) {
  unsigned long ei_startpfn;
  unsigned long ei_endpfn;
  int i;

  for (i = 0; i < e820.nr_map; ++i) {
    if (e820_find_active_region(&e820.map[i],
                                start_pfn, last_pfn,
                                &ei_startpfn, &ei_endpfn)) {
      add_active_range(nid, ei_startpfn, ei_endpfn);
    }
  }
}

char* default_machine_specific_memory_setup(void) {
  char *who = "BIOS-e820";
  u32 new_nr;

  new_nr = boot_params.e820_entries;
  sanitize_e820_map(boot_params.e820_map,
                    ARRAY_SIZE(boot_params.e820_map),
                    &new_nr);
  boot_params.e820_entries = new_nr;
  if (append_e820_map(boot_params.e820_map, boot_params.e820_entries)
      < 0) {
    u64 mem_size;
    /* compare results from other methods and take the greater */
    if (boot_params.alt_mem_k
        < boot_params.screen_info.ext_mem_k) {
      mem_size = boot_params.screen_info.ext_mem_k;
      who = "BIOS-88";
    } else {
      mem_size = boot_params.alt_mem_k;
      who = "BIOS-e801";
    }

    e820.nr_map = 0;
    e820_add_region(0, LOWMEMSIZE(), E820_RAM);
    e820_add_region(HIGH_MEMORY, mem_size << 10, E820_RAM);
  }
  return who;
}

void setup_memory_map(void) {
  char *who;
  who = x86_init.resources.memory_setup();
  memcpy(&e820_saved, &e820, sizeof(struct e820map));
  printk(KERN_INFO "BIOS-provided physical RAM map:\n");
  e820_print_map(who);
}

static int userdef = 0;
void finish_e820_parsing(void)
{
  if (userdef) {
    u32 nr = e820.nr_map;

    if (sanitize_e820_map(e820.map, ARRAY_SIZE(e820.map), &nr) < 0)
      panic("Invalid user supplied memory map");
    e820.nr_map = nr;

    printk(KERN_INFO "user-defined physical RAM map:\n");
    e820_print_map("user");
  }
}

u64 find_e820_area_size(u64 start, u64 *sizep, u64 align)
{
  int i;

  for (i = 0; i < e820.nr_map; i++) {
    struct e820entry *ei = &e820.map[i];
    u64 addr, last;
    u64 ei_last;

    if (ei->type != E820_RAM)
      continue;
    addr = round_up(ei->addr, align);
    ei_last = ei->addr + ei->size;
    if (addr < start)
      addr = round_up(start, align);
    if (addr >= ei_last)
      continue;
    *sizep = ei_last - addr;
    while (bad_addr_size(&addr, sizep, align) &&
           addr + *sizep <= ei_last)
      ;
    last = addr + *sizep;
    if (last > ei_last)
      continue;
    return addr;
  }

  return -1ULL;
}

/*
 * Drop the i-th range from the early reservation map,
 * by copying any higher ranges down one over it, and
 * clearing what had been the last slot.
 */
static void drop_range(int i)
{
  int j;

  for (j = i + 1; j < MAX_EARLY_RES && early_res[j].end; j++)
    ;

  memmove(&early_res[i], &early_res[i + 1],
          (j - 1 - i) * sizeof(struct early_res));

  early_res[j - 1].end = 0;
}


/*
 * Split any existing ranges that:
 *  1) are marked 'overlap_ok', and
 *  2) overlap with the stated range [start, end)
 * into whatever portion (if any) of the existing range is entirely
 * below or entirely above the stated range.  Drop the portion
 * of the existing range that overlaps with the stated range,
 * which will allow the caller of this routine to then add that
 * stated range without conflicting with any existing range.
 */
static void drop_overlaps_that_are_ok(u64 start, u64 end) {
  int i;
  struct early_res *r;
  u64 lower_start, lower_end;
  u64 upper_start, upper_end;
  char name[16];

  for (i = 0; i < MAX_EARLY_RES && early_res[i].end; i++) {
    r = &early_res[i];

    /* Continue past non-overlapping ranges */
    if (end <= r->start || start >= r->end)
      continue;

    /*
     * Leave non-ok overlaps as is; let caller
     * panic "Overlapping early reservations"
     * when it hits this overlap.
     */
    if (!r->overlap_ok)
      return;

    /*
     * We have an ok overlap.  We will drop it from the early
     * reservation map, and add back in any non-overlapping
     * portions (lower or upper) as separate, overlap_ok,
     * non-overlapping ranges.
     */

    /* 1. Note any non-overlapping (lower or upper) ranges. */
    strncpy(name, r->name, sizeof(name) - 1);

    lower_start = lower_end = 0;
    upper_start = upper_end = 0;
    if (r->start < start) {
      lower_start = r->start;
      lower_end = start;
    }
    if (r->end > end) {
      upper_start = end;
      upper_end = r->end;
    }

    /* 2. Drop the original ok overlapping range */
    drop_range(i);
    i--;            /* resume for-loop on copied down entry */

    /* 3. Add back in any non-overlapping ranges. */
    if (lower_end)
      reserve_early_overlap_ok(lower_start, lower_end, name);
    if (upper_end)
      reserve_early_overlap_ok(upper_start, upper_end, name);
  }
}

static void __reserve_early(u64 start, u64 end, char *name,
                                   int overlap_ok)
{
  int i;
  struct early_res *r;

  i = find_overlapped_early(start, end);
  if (i >= MAX_EARLY_RES)
    panic("Too many early reservations");
  r = &early_res[i];
  if (r->end)
    panic("Overlapping early reservations "
          "%llx-%llx %s to %llx-%llx %s\n",
          start, end - 1, name?name:"", r->start,
          r->end - 1, r->name);
  r->start = start;
  r->end = end;
  r->overlap_ok = overlap_ok;
  if (name)
    strncpy(r->name, name, sizeof(r->name) - 1);
}

/*
 * Most early reservations come here.
 *
 * We first have drop_overlaps_that_are_ok() drop any pre-existing
 * 'overlap_ok' ranges, so that we can then reserve this memory
 * range without risk of panic'ing on an overlapping overlap_ok
 * early reservation.
 */
void reserve_early(u64 start, u64 end, char *name)
{
  if (start >= end)
    return;

  drop_overlaps_that_are_ok(start, end);
  __reserve_early(start, end, name, 0);
}

/*
 * A few early reservtations come here.
 *
 * The 'overlap_ok' in the name of this routine does -not- mean it
 * is ok for these reservations to overlap an earlier reservation.
 * Rather it means that it is ok for subsequent reservations to
 * overlap this one.
 *
 * Use this entry point to reserve early ranges when you are doing
 * so out of "Paranoia", reserving perhaps more memory than you need,
 * just in case, and don't mind a subsequent overlapping reservation
 * that is known to be needed.
 *
 * The drop_overlaps_that_are_ok() call here isn't really needed.
 * It would be needed if we had two colliding 'overlap_ok'
 * reservations, so that the second such would not panic on the
 * overlap with the first.  We don't have any such as of this
 * writing, but might as well tolerate such if it happens in
 * the future.
 */
void reserve_early_overlap_ok(u64 start, u64 end, char *name)
{
        drop_overlaps_that_are_ok(start, end);
        __reserve_early(start, end, name, 1);
}

void early_res_to_bootmem(u32 start, u32 end) {
  int i, count;
  u32 final_start, final_end;
  
  count = 0;
  for (; i < MAX_EARLY_RES && early_res[i].end; ++i) {
    count++;
  }

  for (i = 0; i < count; ++i) {
    struct early_res *r = &early_res[i];
    final_start = max(start, r->start);
    final_end = min(end, r->end);
    if (final_start >= final_end) {
      continue;
    }

    reserve_bootmem_generic(final_start, final_end - final_start,
                            BOOTMEM_DEFAULT);
  }
}
