#include <asm/bootparam.h>
#include <asm/sections.h>
#include <asm/e820.h>
#include <asm/cpu.h>

/*
 * end_pfn only includes RAM, while max_pfn_mapped includes all e820 entries.
 * The direct mapping extends to max_pfn_mapped, so that we can directly access
 * apertures, ACPI and other tables without having to play with fixmaps.
 */
unsigned long max_low_pfn_mapped;
unsigned long max_pfn_mapped;

static unsigned long _brk_start = (unsigned long)__brk_base;
unsigned long _brk_end = (unsigned long)__brk_base;

struct boot_params boot_params;

struct cpuinfo_x86 boot_cpu_data = {0, 0, 0, 0, -1, 1, 0, 0, -1};


void setup_arch(char **cmdline_p) {
  setup_memory_map();
}
