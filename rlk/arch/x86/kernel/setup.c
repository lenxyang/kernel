#include <linux/sched.h>
#include <linux/bootmem.h>

#include <asm/setup.h>
#include <asm/bootparam.h>
#include <asm/sections.h>
#include <asm/pgtable.h>
#include <asm/pgtable_32.h>
#include <asm/e820.h>
#include <asm/cpu.h>
#include <asm/io.h>

unsigned long mmu_cr4_features;

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

/*
 * Machine setup..
 */
static struct resource data_resource = {
  .name   = "Kernel data",
  .start  = 0,
  .end    = 0,
        .flags  = IORESOURCE_BUSY | IORESOURCE_MEM
};

static struct resource code_resource = {
  .name   = "Kernel code",
  .start  = 0,
  .end    = 0,
        .flags  = IORESOURCE_BUSY | IORESOURCE_MEM
};

static struct resource bss_resource = {
  .name   = "Kernel bss",
  .start  = 0,
  .end    = 0,
        .flags  = IORESOURCE_BUSY | IORESOURCE_MEM
};

static void __init e820_reserve_setup_data(void) {
}

static void __init reserve_brk(void) {
  if (_brk_end > _brk_start) {
    /* reserve_early 定义在 e820.c 当中 */
    reserve_early(__pa(_brk_start), __pa(_brk_end), "BRK");

    _brk_start = 0;
  }
}

void parse_setup_data(void) {
  /**
   * 解析setup_data
   */
}

/**
 * copy_edd() - Copy the BIOS EDD information
 *              from boot_params into a safe place.
 *
 * x86 BIOS Enhanced Disk Device (EDD) polling
 */
static inline void copy_edd(void) {
  // 不支持
}

void setup_arch(char **cmdline_p) {
  /**
   * setup_memory_map 定义在文件 arch/x86/kernel/e820.c 当中
   * 此函数的作用是 
   * 1. 将 boot_params 当中的 e820_map 保存在全局变量 e820 当中
   * 2. 增加一些新的区域
   */
  setup_memory_map();

  parse_setup_data();

  /**
   * 此函数的作用是为 setup_data 分配内存
   * 但实际上并没有 setup_data, 因此它的作用也就没有了
   */
  e820_reserve_setup_data();

  /**
   * 保存一些关键区域的地址，以备后面使用
   * 定义在了 mm/init-mm.c
   */
  init_mm.start_code = (unsigned long) _text;
  init_mm.end_code = (unsigned long) _etext;
  init_mm.end_data = (unsigned long) _edata;
  init_mm.brk = _brk_end;

  code_resource.start = virt_to_phys(_text);
  code_resource.end = virt_to_phys(_etext)-1;
  data_resource.start = virt_to_phys(_etext);
  data_resource.end = virt_to_phys(_edata)-1;
  bss_resource.start = virt_to_phys(&__bss_start);
  bss_resource.end = virt_to_phys(&__bss_stop)-1;

  /**
   * 解析启动参数，启动时他的内容是
   * "BOOT_IMAGE=/boot/vmlinuz ro"
   */
  // parse_early_param();

  /**
   * Virtual machine interface(VMI) 此处的实现是空的
   */
  // vmi_activate();

  /**
   * 此函数也没有作用
   */
  // reserve_early_setup_data();
  // finish_e820_parsing();
  /**
   * Desktop Management Interface(DMI)
   */
  // dmi_scan_machine();
  // dmi_check_system(bad_bios_dmi_table);

  /**
   * insert_resources
   */
  // insert_resource(&iomem_resource, &code_resource);
  // insert_resource(&iomem_resource, &data_resource);
  // insert_resource(&iomem_resource, &bss_resource);

  /**
   * max_pfn 声明在 asm/page_types.h
   * max_pfn 定义在 mm/bootmem.c
   */
  max_pfn = e820_end_of_ram_pfn();
  find_low_pfn_range();
  printk(KERN_DEBUG "initial memory mapped : 0 - %08lx\n",
         max_pfn_mapped<<PAGE_SHIFT);
  reserve_brk();

  // init_gbpages();

  /**
   * init_memory_mapping 定义在 arch/x86/mm/init.c 当中
   * 此函数会将内存分成三个区域 map_range 当中
   * 而后根据三个区域的属性完成对三个区域的映射
   */
  max_low_pfn_mapped = init_memory_mapping(0, max_low_pfn<<PAGE_SHIFT);
  max_pfn_mapped = max_low_pfn_mapped;

  /**
   * 为 initramfs 保存空间
   * initrams的相关参数在 bootparam 当中设置，我们的实现当中没有
   */
  // reverse_initrd();

  // vsmp_init();

  // io_delay_init();


  initmem_init(0, max_pfn);

  x86_init.paging.pagetable_setup_start(swapper_pg_dir);
  paging_init();
  x86_init.paging.pagetable_setup_done(swapper_pg_dir);

  /*
  e820_reserve_resources();
  e820_mark_nosave_regions(max_low_pfn);
  x86_init.resources.reserve_resources();
  e820_setup_gap();
  */
}

static struct resource standard_io_resources[] = {
  { .name = "dma1", .start = 0x00, .end = 0x1f,
    .flags = IORESOURCE_BUSY | IORESOURCE_IO },
  { .name = "pic1", .start = 0x20, .end = 0x21,
    .flags = IORESOURCE_BUSY | IORESOURCE_IO },
  { .name = "timer0", .start = 0x40, .end = 0x43,
    .flags = IORESOURCE_BUSY | IORESOURCE_IO },
  { .name = "timer1", .start = 0x50, .end = 0x53,
    .flags = IORESOURCE_BUSY | IORESOURCE_IO },
  { .name = "keyboard", .start = 0x60, .end = 0x60,
    .flags = IORESOURCE_BUSY | IORESOURCE_IO },
  { .name = "keyboard", .start = 0x64, .end = 0x64,
    .flags = IORESOURCE_BUSY | IORESOURCE_IO },
  { .name = "dma page reg", .start = 0x80, .end = 0x8f,
    .flags = IORESOURCE_BUSY | IORESOURCE_IO },
  { .name = "pic2", .start = 0xa0, .end = 0xa1,
    .flags = IORESOURCE_BUSY | IORESOURCE_IO },
  { .name = "dma2", .start = 0xc0, .end = 0xdf,
    .flags = IORESOURCE_BUSY | IORESOURCE_IO },
  { .name = "fpu", .start = 0xf0, .end = 0xff,
    .flags = IORESOURCE_BUSY | IORESOURCE_IO }
};

void __init reserve_standard_io_resources(void) {
  int i;
  for (i = 0; i < ARRAY_SIZE(standard_io_resources); i++) {
    request_resource(&ioport_resource, &standard_io_resources[i]);
  }
}

static struct resource video_ram_resource = {
  .name  = "Video RAM area",
  .start = 0xa0000,
  .end   = 0xb0000,
  .flags = IORESOURCE_BUSY | IORESOURCE_MEM,
};

void __init i386_reserve_resources(void) {
  request_resource(&iomem_resource, &video_ram_resource);
  reserve_standard_io_resources();
}
