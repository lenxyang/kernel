#include <asm/e820.h>
#include <linux/kernel.h>
#include <linux/init.h>

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


