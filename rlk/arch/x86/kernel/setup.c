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
  /**
   * setup_memory_map 定义在文件 arch/x86/kernel/e820.c 当中
   * 此函数的作用是 
   * 1. 将 boot_params 当中的 e820_map 保存在全局变量 e820 当中
   * 2. 按照
   */
  setup_memory_map();
}
