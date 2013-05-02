
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/bootmem.h>

#include <asm/e820.h>
#include <asm/string.h>
#include <asm/boot.h>
#include <asm/setup.h>
#include <asm/proto.h>

#ifdef CONFIG_X86_32
# ifdef CONFIG_X86_PAE
#  define MAX_ARCH_PFN          (1ULL<<(36-PAGE_SHIFT))
# else
#  define MAX_ARCH_PFN          (1ULL<<(32-PAGE_SHIFT))
# endif
#else /* CONFIG_X86_32 */
# define MAX_ARCH_PFN MAXMEM>>PAGE_SHIFT
#endif


/*
 * The e820 map is the map that gets modified e.g. with command line parameters
 * and that is also registered with modifications in the kernel resource tree
 * with the iomem_resource as parent.
 *
 * The e820_saved is directly saved after the BIOS-provided memory map is
 * copied. It doesn't get modified afterwards. It's registered for the
 * /sys/firmware/memmap interface.
 *
 * That memory map is not modified and is used as base for kexec. The kexec'd
 * kernel should get the same memory map as the firmware provides. Then the
 * user can e.g. boot the original kernel with mem=1G while still booting the
 * next kernel with full memory.
 */
struct e820map e820;
struct e820map e820_saved;

/*
 * Add a memory region to the kernel e820 map.
 */
static void __init __e820_add_region(struct e820map *e820x, u64 start, u64 size,
                                     int type)
{
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

void __init e820_add_region(u64 start, u64 size, int type)
{
  __e820_add_region(&e820, start, size, type);
}

static int __init __append_e820_map(struct e820entry *biosmap, int nr_map)
{
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
 * Copy the BIOS e820 map into a safe place.
 *
 * Sanity-check it while we're at it..
 *
 * If we're lucky and live on a modern system, the setup code
 * will have given us a memory map that we can use to properly
 * set up memory.  If we aren't, we'll fake a memory map.
 */
static int __init append_e820_map(struct e820entry *biosmap, int nr_map)
{
  /* Only one memory region (or negative)? Ignore it */
  if (nr_map < 2)
    return -1;

  return __append_e820_map(biosmap, nr_map);
}

static void __init e820_print_type(u32 type)
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

static unsigned long __init e820_end_pfn(unsigned long limit_pfn, unsigned type) {
  int i;
  unsigned long last_pfn = 0;
  unsigned long max_arch_pfn = MAX_ARCH_PFN;

  for (i = 0; i < e820.nr_map; i++) {
    struct e820entry* ei = &e820.map[i];
    unsigned long start_pfn;
    unsigned long end_pfn;

    if (ei->type != type) {
      continue;
    }

    // >> PAGE_SHIFT 以便获得 pfn
    start_pfn = ei->addr >> PAGE_SHIFT;
    end_pfn = (ei->addr + ei->size) >> PAGE_SHIFT;

    if (start_pfn >= limit_pfn) {
      continue;
    }

    if (end_pfn > limit_pfn) {
      last_pfn = limit_pfn;
      break;
    }

    if (end_pfn > last_pfn) {
      last_pfn = end_pfn;
    }
  }

  if (last_pfn > max_arch_pfn) {
    last_pfn = max_arch_pfn;
  }

  printk(KERN_INFO "last_pfn = %#lx max_arch_pfn = %#lx\n",
         last_pfn, max_arch_pfn);

  return last_pfn;
}

unsigned long __init e820_end_of_ram_pfn(void) {
  return e820_end_pfn(MAX_ARCH_PFN, E820_RAM);
}

void __init e820_print_map(char *who)
{
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


char *__init default_machine_specific_memory_setup(void) {
  char *who = "BIOS-e820";
  u32 new_nr;
  /*
   * Try to copy the BIOS-supplied E820-map.
   *
   * Otherwise fake a memory map; one section from 0k->640k,
   * the next section from 1mb->appropriate_mem_k
   */
  new_nr = boot_params.e820_entries;
  // 此函数完成的功能是调整出现overlap区域的内存区域
  // 在qemu上从未出现过，因此此处忽略此函数的调用
  /* sanitize_e820_map(boot_params.e820_map,
                    ARRAY_SIZE(boot_params.e820_map),
                    &new_nr);
  */
  boot_params.e820_entries = new_nr;
  /**
   * 此处将 boot_params的e820_map，添加到全局变量 e820当中
   */
  if (append_e820_map(boot_params.e820_map, boot_params.e820_entries)
      < 0) {
    /**
     * 这部分代码不会执行
     */
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

  /* In case someone cares... */
  return who;
}

int __init e820_find_active_region(const struct e820entry *ei,
                                   unsigned long start_pfn,
                                   unsigned long last_pfn,
                                   unsigned long *ei_startpfn,
                                   unsigned long *ei_endpfn) {
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

  if (*ei_startpfn < start_pfn) {
    *ei_startpfn = start_pfn;
  }

  if (*ei_endpfn > last_pfn) {
    *ei_endpfn = last_pfn;
  }

  return 1;
}

/**
 *  early reserved memory areas
 *
 */
#define MAX_EARLY_RES 20

struct early_res {
  u64 start, end;
  char name[16];
  char overlap_ok;
};

static struct early_res early_res[MAX_EARLY_RES] __initdata = {
  {0, PAGE_SIZE, "BIOS data page"},
  {}
};

static void __init drop_range(int i) {
  int j;
  for (j = i + 1; j < MAX_EARLY_RES && early_res[i].end; j++)
    ;

  /* 此处为区域重叠的 memmove */
  memmove(&early_res[i], &early_res[i + 1],
          (j - 1 - i) * sizeof(struct early_res));
  early_res[j-1].end = 0;
}

static void __init drop_overlaps_that_are_ok(u64 start, u64 end) {
  int i;
  struct early_res *r;
  u64 lower_start, lower_end;
  u64 upper_start, upper_end;
  char name[16];

  for (i = 0; i < MAX_EARLY_RES && early_res[i].end; i++) {
    r = early_res + i;

    if (end <= r->start || start >= r->end) {
      continue;
    }

    if (!r->overlap_ok) {
      return;
    }

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

    drop_range(i);
    i--;

    if (lower_end) {
      reserve_early_overlap_ok(lower_start, lower_end, name);
    }

    if (upper_end) {
      reserve_early_overlap_ok(upper_start, upper_end, name);
    }
  }
}

/**
 * 函数 find_overlapped_early
 * 此函数的为何包含一个 overlapped
 */
static int __init find_overlapped_early(u64 start, u64 end) {
  int i;
  struct early_res *r;

  for (i = 0; i < MAX_EARLY_RES && early_res[i].end; i++) {
    r = &early_res[i];
    if (end > r->start && start < r->end) {
      break;
    }
  }

  return i;
}

static void __init __reserve_early(u64 start, u64 end, char* name,
                                   int overlap_ok) {
  int i;
  struct early_res *r;

  i = find_overlapped_early(start, end);
  if (i >= MAX_EARLY_RES) {
    panic("Too many early reservation.");
  }

  r = &early_res[i];
  if (r->end) {
    panic("Overlapping early reservations "
          "%llx-%llx %s to %llx-%llx %s\n",
          start, end - 1, name ? name: "", r->start,
          r->end - 1, r->name);
  }
  
  r->start = start;
  r->end = end;
  r->overlap_ok = overlap_ok;
  if (name) {
    strncpy(r->name, name, sizeof(r->name) - 1);
  }
}

static inline int __init bad_addr(u64 *addrp, u64 size, u64 align) {
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

u64 __init find_e820_area(u64 start, u64 end, u64 size, u64 align) {
  int i;
  for (i = 0; i < e820.nr_map; i++) {
    struct e820entry* ei = &e820.map[i];
    u64 addr, last;
    u64 ei_last;

    if (ei->type != E820_RAM) {
      continue;
    }

    addr = round_up(ei->addr, align);
    ei_last = ei->addr + ei->size;
    if (addr < start)
      addr = round_up(start, align);
    if (addr >= ei_last)
      continue;

    while (bad_addr(&addr, size, align) && addr + size <= ei_last) {
      ;
    }

    last = addr + size;
    if (last > ei_last) {
      continue;
    }
    if (last > end) {
      continue;
    }

    return addr;
  }

  return -1ULL;
}

void __init reserve_early_overlap_ok(u64 start, u64 end, char* name) {
  drop_overlaps_that_are_ok(start, end);
  __reserve_early(start, end, name, 1);
}

void __init reserve_early(u64 start, u64 end, char* name) {
  if (start >= end)
    return;

  drop_overlaps_that_are_ok(start, end);
  __reserve_early(start, end, name, 0);
}

void __init  setup_memory_map(void) {
  char *who;
  // who = x86_init.resources.memory_setup();
  default_machine_specific_memory_setup();
  memcpy(&e820_saved, &e820, sizeof(struct e820map));
  printk(KERN_INFO "BIOS-provided physical RAM map:\n");
  e820_print_map(who);
}

/* Walk the e820 map and register active regions within a node */
/* called by function initmem_init */
void __init e820_register_active_regions(int nid, unsigned long start_pfn,
                                         unsigned long last_pfn) {
  unsigned long ei_startpfn;
  unsigned long ei_endpfn;
  int i;

  for (i = 0; i < e820.nr_map; i++) {
    if (e820_find_active_region(&e820.map[i], start_pfn, last_pfn,
                                &ei_startpfn, &ei_endpfn)) {
      add_active_range(nid, ei_startpfn, ei_endpfn);
    }
  }
}

void __init early_res_to_bootmem(u64 start, u64 end)
{
  int i, count;
  u64 final_start, final_end;

  count  = 0;
  for (i = 0; i < MAX_EARLY_RES && early_res[i].end; i++)
    count++;

  printk(KERN_INFO "(%d early reservations) ==> bootmem [%010llx - %010llx]\n",
         count, start, end);
  for (i = 0; i < count; i++) {
    struct early_res *r = &early_res[i];
    printk(KERN_INFO "  #%d [%010llx - %010llx] %16s", i,
           r->start, r->end, r->name);
    final_start = max(start, r->start);
    final_end = min(end, r->end);
    if (final_start >= final_end) {
      printk(KERN_CONT "\n");
      continue;
    }
    printk(KERN_CONT " ==> [%010llx - %010llx]\n",
           final_start, final_end);
    reserve_bootmem_generic(final_start, final_end - final_start,
                            BOOTMEM_DEFAULT);
  }
}
