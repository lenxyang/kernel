#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/bitmap.h>

#include <asm/setup.h>
#include <asm/irq.h>
#include <asm/time.h>
#include <asm/sections.h>
#include <asm/mpspec.h>
#include <asm/apic.h>
#include <asm/paravirt.h>

void __cpuinit x86_init_noop(void) { }
void __init x86_init_uint_noop(unsigned int unused) { }
void __init x86_init_pgd_noop(pgd_t *unused) { }

/**
 * for standard PC hardware.
 */
 struct x86_init_ops x86_init __initdata = {

     .resources = {
       .probe_roms             = x86_init_noop,
       // .reserve_resources      = reserve_standard_io_resources,
       .memory_setup           = default_machine_specific_memory_setup,
     },

     .mpparse = {
       .mpc_record             = x86_init_uint_noop,
       .setup_ioapic_ids       = x86_init_noop,
       .mpc_apic_id            = default_mpc_apic_id,
       .smp_read_mpc_oem       = default_smp_read_mpc_oem,
       .mpc_oem_bus_info       = default_mpc_oem_bus_info,
       .find_smp_config        = default_find_smp_config,
       .get_smp_config         = default_get_smp_config,
     },

     .irqs = {
     // .pre_vector_init        = init_ISA_irqs,
     //  .intr_init              = native_init_IRQ,
     //  .trap_init              = x86_init_noop,
     },

     .oem = {
       .arch_setup             = x86_init_noop,
       .banner                 = default_banner,
     },

     .paging = {
     // .pagetable_setup_start  = native_pagetable_setup_start,
     //  .pagetable_setup_done   = native_pagetable_setup_done,
     },

     .timers = {
       .setup_percpu_clockev   = setup_boot_APIC_clock,
       .tsc_pre_init           = x86_init_noop,
       // .timer_init             = hpet_time_init,
     },
   };
