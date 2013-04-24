#include "types.h"
#include "segment.h"
#include "boot.h"

extern void protected_mode_jump(void);

/*
 * Set up the GDT
 */

struct gdt_ptr {
  u16 len;
  u32 ptr;
} __attribute__((packed));


static void setup_gdt(void)
{
  /* There are machines which are known to not boot with the GDT
     being 8-byte unaligned.  Intel recommends 16 byte alignment. */
  static const u64 boot_gdt[] __attribute__((aligned(16))) = {
    /* CS: code, read/execute, 4 GB, base 0 */
    [GDT_ENTRY_BOOT_CS] = GDT_ENTRY(0xc09b, 0, 0xfffff),
    /* DS: data, read/write, 4 GB, base 0 */
    [GDT_ENTRY_BOOT_DS] = GDT_ENTRY(0xc093, 0, 0xfffff),
    /* TSS: 32-bit tss, 104 bytes, base 4096 */
    /* We only have a TSS here to keep Intel VT happy;
       we don't actually use it for anything. */
    [GDT_ENTRY_BOOT_TSS] = GDT_ENTRY(0x0089, 4096, 103),
  };
  /* Xen HVM incorrectly stores a pointer to the gdt_ptr, instead
           of the gdt_ptr contents.  Thus, make it static so it will
           stay in memory, at least long enough that we switch to the
           proper kernel GDT. */
  static struct gdt_ptr gdt;

  gdt.len = sizeof(boot_gdt)-1;
  gdt.ptr = (u32)&boot_gdt + (ds() << 4);

  asm volatile("lgdtl %0" : : "m" (gdt));
}

void go_to_protected_mode(void) {
  setup_gdt();
  
  protected_mode_jump();
}
