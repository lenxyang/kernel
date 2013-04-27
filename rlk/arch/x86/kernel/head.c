#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <asm/bios_ebda.h>
#include <asm/io_apic.h>
#include <asm/setup.h>
#include <asm/x86_init.h>
#include <asm/e820.h>
#include <asm/page.h>
#include <asm/sections.h>
#include <asm/paravirt.h>
#include <asm/processor.h>
#include <asm/trampoline.h>

static void __init i386_default_early_setup(void) {
/**
 * probe_roms 
 */
  x86_init.resources.probe_roms = probe_roms;
  x86_init.resources.reserve_resources = i386_reserve_resources;
  x86_init.mpparse.setup_ioapic_ids = setup_ioapic_ids_from_mpc;

  reserve_ebda_region();
}

void __init i386_start_kernel(void) {
  reserve_trampoline_memory();
  reserve_early(__pa_symbol(&_text), __pa_symbol(&__bss_stop),
                "TEXT DATA BSS");

  i386_default_early_setup();

  start_kernel();
}

#define BIOS_LOWMEM_KILOBYTES 0x413

/*
 * The BIOS places the EBDA/XBDA at the top of conventional
 * memory, and usually decreases the reported amount of
 * conventional memory (int 0x12) too. This also contains a
 * workaround for Dell systems that neglect to reserve EBDA.
 * The same workaround also avoids a problem with the AMD768MPX
 * chipset: reserve a page before VGA to prevent PCI prefetch
 * into it (errata #56). Usually the page is reserved anyways,
 * unless you have no PS/2 mouse plugged in.
 */
void __init reserve_ebda_region(void) {
  unsigned int lowmem, ebda_addr;

  /**
   * 在 paravirtual env 启动的使用， EBDA 区域失效。
   * 此时可以认为 paravirtual 能够正确处理相关的内存设置
   */
  /* To determine the position of the EBDA and the */
  /* end of conventional memory, we need to look at */
  /* the BIOS data area. In a paravirtual environment */
  /* that area is absent. We'll just have to assume */
  /* that the paravirt case can handle memory setup */
  /* correctly, without our help. */
  if (paravirt_enabled())
    return;

  /* end of low (conventional) memory */
  lowmem = *(unsigned short *)__va(BIOS_LOWMEM_KILOBYTES);
  lowmem <<= 10;

  /* start of EBDA area */
  ebda_addr = get_bios_ebda();

  /* Fixup: bios puts an EBDA in the top 64K segment */
  /* of conventional memory, but does not adjust lowmem. */
  if ((lowmem - ebda_addr) <= 0x10000)
    lowmem = ebda_addr;

  /* Fixup: bios does not report an EBDA at all. */
  /* Some old Dells seem to need 4k anyhow (bugzilla 2990) */
  if ((ebda_addr == 0) && (lowmem >= 0x9f000))
    lowmem = 0x9f000;

  /* Paranoia: should never happen, but... */
  if ((lowmem == 0) || (lowmem >= 0x100000))
    lowmem = 0x9f000;

  /* reserve all memory between lowmem and the 1MB mark */
  reserve_early_overlap_ok(lowmem, 0x100000, "BIOS reserved");
}
