#ifndef __FIXMAP_H__
#define __FIXMAP_H__

#include "kmap_types.h"
#include "threads.h"

extern unsigned long __FIXADDR_TOP;
#define FIXADDR_TOP     ((unsigned long)__FIXADDR_TOP)
#define FIXADDR_USER_START     __fix_to_virt(FIX_VDSO)
#define FIXADDR_USER_END       __fix_to_virt(FIX_VDSO - 1)

#define __fix_to_virt(x)        (FIXADDR_TOP - ((x) << PAGE_SHIFT))
#define __virt_to_fix(x)        ((FIXADDR_TOP - ((x)&PAGE_MASK)) >> PAGE_SHIFT)

enum fixed_addresses {

  FIX_HOLE,
  FIX_VDSO,

  FIX_DBGP_BASE,
  FIX_EARLYCON_MEM_BASE,




  FIX_KMAP_BEGIN, /* reserved pte's for temporary kernel mappings */
  FIX_KMAP_END = FIX_KMAP_BEGIN+(KM_TYPE_NR * NR_CPUS)-1,

  FIX_PCIE_MCFG,


  FIX_PARAVIRT_BOOTMAP,

  FIX_TEXT_POKE1, /* reserve 2 pages for text_poke() */
  FIX_TEXT_POKE0, /* first page is last, because allocation is backward */
  __end_of_permanent_fixed_addresses,
  /*
   * 256 temporary boot-time mappings, used by early_ioremap(),
   * before ioremap() is functional.
   *
   * We round it up to the next 256 pages boundary so that we
   * can have a single pgd entry and a single pte table:
   */
#define NR_FIX_BTMAPS           64
#define FIX_BTMAPS_SLOTS        4
  FIX_BTMAP_END = __end_of_permanent_fixed_addresses + 256 -
  (__end_of_permanent_fixed_addresses & 255),
  FIX_BTMAP_BEGIN = FIX_BTMAP_END + NR_FIX_BTMAPS*FIX_BTMAPS_SLOTS - 1,

  FIX_WP_TEST,

  __end_of_fixed_addresses,
};


#define FIXADDR_SIZE    (__end_of_permanent_fixed_addresses << PAGE_SHIFT)
#define FIXADDR_BOOT_SIZE       (__end_of_fixed_addresses << PAGE_SHIFT)
#define FIXADDR_START           (FIXADDR_TOP - FIXADDR_SIZE)
#define FIXADDR_BOOT_START      (FIXADDR_TOP - FIXADDR_BOOT_SIZE)

#endif  // __FIXMAP_H__
