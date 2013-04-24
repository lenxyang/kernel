#ifndef __PGTABLE_H__
#define __PGTABLE_H__

#include "pgtable_2level.h"
#include "page.h"

#define _PAGE_PRESENT   0x001
#define _PAGE_RW        0x002
#define _PAGE_USER      0x004
#define _PAGE_PWT       0x008
#define _PAGE_PCD       0x010
#define _PAGE_ACCESSED  0x020
#define _PAGE_DIRTY     0x040
#define _PAGE_PSE       0x080   /* 2MB page */
#define _PAGE_FILE      0x040   /* set:pagecache, unset:swap */
#define _PAGE_GLOBAL    0x100   /* Global TLB entry */

#define _PAGE_PROTNONE  0x080   /* If not present */

#define _PAGE_TABLE     (_PAGE_PRESENT | _PAGE_RW | _PAGE_USER | _PAGE_ACCESSED | _PAGE_DIRTY)
#define _KERNPG_TABLE   (_PAGE_PRESENT | _PAGE_RW | _PAGE_ACCESSED | _PAGE_DIRTY)
#define _PAGE_CHG_MASK  (PTE_MASK | _PAGE_ACCESSED | _PAGE_DIRTY)

#define PAGE_NONE \
  __pgprot(_PAGE_PROTNONE | _PAGE_ACCESSED)
#define PAGE_SHARED \
  __pgprot(_PAGE_PRESENT | _PAGE_RW | _PAGE_USER | _PAGE_ACCESSED)

#define PAGE_SHARED_EXEC \
  __pgprot(_PAGE_PRESENT | _PAGE_RW | _PAGE_USER | _PAGE_ACCESSED)
#define PAGE_COPY_NOEXEC \
  __pgprot(_PAGE_PRESENT | _PAGE_USER | _PAGE_ACCESSED | _PAGE_NX)
#define PAGE_COPY_EXEC \
  __pgprot(_PAGE_PRESENT | _PAGE_USER | _PAGE_ACCESSED)
#define PAGE_COPY \
        PAGE_COPY_NOEXEC
#define PAGE_READONLY \
  __pgprot(_PAGE_PRESENT | _PAGE_USER | _PAGE_ACCESSED | _PAGE_NX)
#define PAGE_READONLY_EXEC \
  __pgprot(_PAGE_PRESENT | _PAGE_USER | _PAGE_ACCESSED)

extern unsigned long long __PAGE_KERNEL, __PAGE_KERNEL_EXEC;
#define __PAGE_KERNEL_RO                (__PAGE_KERNEL & ~_PAGE_RW)
#define __PAGE_KERNEL_NOCACHE           (__PAGE_KERNEL | _PAGE_PCD)
#define __PAGE_KERNEL_LARGE             (__PAGE_KERNEL | _PAGE_PSE)
#define __PAGE_KERNEL_LARGE_EXEC        (__PAGE_KERNEL_EXEC | _PAGE_PSE

#define PAGE_KERNEL             __pgprot(__PAGE_KERNEL)
#define PAGE_KERNEL_RO          __pgprot(__PAGE_KERNEL_RO)
#define PAGE_KERNEL_EXEC        __pgprot(__PAGE_KERNEL_EXEC)
#define PAGE_KERNEL_NOCACHE     __pgprot(__PAGE_KERNEL_NOCACHE)
#define PAGE_KERNEL_LARGE       __pgprot(__PAGE_KERNEL_LARGE)
#define PAGE_KERNEL_LARGE_EXEC  __pgprot(__PAGE_KERNEL_LARGE_EXEC)



#define pte_present(x)  ((x).pte_low & (_PAGE_PRESENT | _PAGE_PROTNONE))
#define pte_clear(xp)    do {set_pte(xp, __pte(0));} while (0)

#define pmd_none(x) (!pmd_val(x))
#define pmd_present(x) (pmd_val(x) & _PAGE_PRESENT)
#define pmd_clear(xp) do {set_pmd(xp, __pmd(0);} while (0)

#define pages_to_mb(x) ((x) >> (20 - PAGE_SHIFT))

static inline int pte_user(pte_t pte) { return pte.pte_low & _PAGE_USER;}
static inline int pte_read(pte_t pte) { return pte.pte_low & _PAGE_USER;}
static inline int page_dirty(pte_t pte) {return pte.pte_low & _PAGE_DIRTY;}
static inline int page_young(pte_t pte) {return pte.pte_low & _PAGE_ACCESSED;}
static inline int page_write(pte_t pte) {return pte.pte_low & _PAGE_RW;}

#define pgd_index(address) ((address >> PGDIR_SHIFT) & (PTRS_PER_PGD-1))
#define pgd_index_k(addr) pgd_index(addr)

#define pgd_offset(mm, address) ((mm)->pgd + pgd_index(address))
#define pgd_offset_k(address)  pgd_offset(&init_mm, address)

#define pmd_index(address)                      \
  (((address) >> PMD_SHIFT) & (PTRS_PER_PMD - 1))

#define pte_index(address)                      \
  (((address) >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))
#define pte_offset_kernel(dir, address)         \
  ((pte_t*)pmd_page_kernel(*(dir)) + pte_index(address))

#endif  // __PGTABLE_H__
