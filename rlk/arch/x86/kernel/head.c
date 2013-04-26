#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <asm/io_apic.h>
#include <asm/setup.h>
#include <asm/x86_init.h>
#include <asm/e820.h>
#include <asm/page.h>
#include <asm/sections.h>
#include <asm/trampoline.h>

static void __init i386_default_early_setup(void) {
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
