#include <asm/e820.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <asm/string.h>
#include <asm/boot.h>
#include <asm/setup.h>

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

  /* In case someone cares... */
  return who;
}

void __init  setup_memory_map(void) {
  char *who;
  // who = x86_init.resources.memory_setup();
  default_machine_specific_memory_setup();
  memcpy(&e820_saved, &e820, sizeof(struct e820map));
  printk(KERN_INFO "BIOS-provided physical RAM map:\n");
  e820_print_map(who);
}
