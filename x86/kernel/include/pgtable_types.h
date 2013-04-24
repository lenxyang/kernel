#ifndef __PGTABLE_TYPES_H__
#define __PGTABLE_TYPES_H__

#include "types.h"

#define PTE_IDENT_ATTR   0x003          /* PRESENT+RW */
#define PDE_IDENT_ATTR   0x067          /* PRESENT+RW+USER+DIRTY+ACCESSED */
#define PGD_IDENT_ATTR   0x001          /* PRESENT (no other attributes) */


#define __PHYSICAL_MASK         ((phys_addr_t)(1ULL << __PHYSICAL_MASK_SHIFT) - 1)
#define PHYSICAL_PAGE_MASK      (((signed long)PAGE_MASK) & __PHYSICAL_MASK) 
#define PTE_PFN_MASK            ((pteval_t)PHYSICAL_PAGE_MASK)
#define PTE_FLAGS_MASK          (~PTE_PFN_MASK)

#define FIRST_USER_ADDRESS      0

#define _PAGE_BIT_PRESENT       0       /* is present */
#define _PAGE_BIT_RW            1       /* writeable */
#define _PAGE_BIT_USER          2       /* userspace addressable */
#define _PAGE_BIT_PWT           3       /* page write through */
#define _PAGE_BIT_PCD           4       /* page cache disabled */
#define _PAGE_BIT_ACCESSED      5       /* was accessed (raised by CPU) */
#define _PAGE_BIT_DIRTY         6       /* was written to (raised by CPU) */
#define _PAGE_BIT_PSE           7       /* 4 MB (or 2MB) page */
#define _PAGE_BIT_PAT           7       /* on 4KB pages */
#define _PAGE_BIT_GLOBAL        8       /* Global TLB entry PPro+ */
#define _PAGE_BIT_UNUSED1       9       /* available for programmer */
#define _PAGE_BIT_IOMAP         10      /* flag used to indicate IO mapping */
#define _PAGE_BIT_HIDDEN        11      /* hidden by kmemcheck */
#define _PAGE_BIT_PAT_LARGE     12      /* On 2MB or 1GB pages */
#define _PAGE_BIT_SPECIAL       _PAGE_BIT_UNUSED1
#define _PAGE_BIT_CPA_TEST      _PAGE_BIT_UNUSED1
#define _PAGE_BIT_NX           63       /* No execute: only valid after cpuid check */

/* If _PAGE_BIT_PRESENT is clear, we use these: */
/* - if the user mapped it with PROT_NONE; pte_present gives true */
#define _PAGE_BIT_PROTNONE      _PAGE_BIT_GLOBAL
/* - set: nonlinear file mapping, saved PTE; unset:swap */
#define _PAGE_BIT_FILE          _PAGE_BIT_DIRTY

#define _PAGE_PRESENT   (_AT(pteval_t, 1) << _PAGE_BIT_PRESENT)
#define _PAGE_RW        (_AT(pteval_t, 1) << _PAGE_BIT_RW)
#define _PAGE_USER      (_AT(pteval_t, 1) << _PAGE_BIT_USER)
#define _PAGE_PWT       (_AT(pteval_t, 1) << _PAGE_BIT_PWT)
#define _PAGE_PCD       (_AT(pteval_t, 1) << _PAGE_BIT_PCD)
#define _PAGE_ACCESSED  (_AT(pteval_t, 1) << _PAGE_BIT_ACCESSED)
#define _PAGE_DIRTY     (_AT(pteval_t, 1) << _PAGE_BIT_DIRTY)
#define _PAGE_PSE       (_AT(pteval_t, 1) << _PAGE_BIT_PSE)
#define _PAGE_GLOBAL    (_AT(pteval_t, 1) << _PAGE_BIT_GLOBAL)
#define _PAGE_UNUSED1   (_AT(pteval_t, 1) << _PAGE_BIT_UNUSED1)
#define _PAGE_IOMAP     (_AT(pteval_t, 1) << _PAGE_BIT_IOMAP)
#define _PAGE_PAT       (_AT(pteval_t, 1) << _PAGE_BIT_PAT)
#define _PAGE_PAT_LARGE (_AT(pteval_t, 1) << _PAGE_BIT_PAT_LARGE)
#define _PAGE_SPECIAL   (_AT(pteval_t, 1) << _PAGE_BIT_SPECIAL)
#define _PAGE_CPA_TEST  (_AT(pteval_t, 1) << _PAGE_BIT_CPA_TEST)
#define __HAVE_ARCH_PTE_SPECIAL

#define _PAGE_NX        (_AT(pteval_t, 0))

#define _PAGE_FILE      (_AT(pteval_t, 1) << _PAGE_BIT_FILE)
#define _PAGE_PROTNONE  (_AT(pteval_t, 1) << _PAGE_BIT_PROTNONE)

#define _PAGE_TABLE     (_PAGE_PRESENT | _PAGE_RW | _PAGE_USER |        \
                         _PAGE_ACCESSED | _PAGE_DIRTY)
#define _KERNPG_TABLE   (_PAGE_PRESENT | _PAGE_RW | _PAGE_ACCESSED |    \
                         _PAGE_DIRTY)

/* Set of bits not changed in pte_modify */
#define _PAGE_CHG_MASK  (PTE_PFN_MASK | _PAGE_PCD | _PAGE_PWT |         \
                         _PAGE_SPECIAL | _PAGE_ACCESSED | _PAGE_DIRTY)

#define _PAGE_CACHE_MASK        (_PAGE_PCD | _PAGE_PWT)
#define _PAGE_CACHE_WB          (0)
#define _PAGE_CACHE_WC          (_PAGE_PWT)
#define _PAGE_CACHE_UC_MINUS    (_PAGE_PCD)
#define _PAGE_CACHE_UC          (_PAGE_PCD | _PAGE_PWT)

#define PAGE_NONE       __pgprot(_PAGE_PROTNONE | _PAGE_ACCESSED)
#define PAGE_SHARED     __pgprot(_PAGE_PRESENT | _PAGE_RW | _PAGE_USER | \
                                 _PAGE_ACCESSED | _PAGE_NX)

#define PAGE_SHARED_EXEC        __pgprot(_PAGE_PRESENT | _PAGE_RW |     \
                                         _PAGE_USER | _PAGE_ACCESSED)
#define PAGE_COPY_NOEXEC        __pgprot(_PAGE_PRESENT | _PAGE_USER |   \
                                         _PAGE_ACCESSED | _PAGE_NX)
#define PAGE_COPY_EXEC          __pgprot(_PAGE_PRESENT | _PAGE_USER |   \
                                         _PAGE_ACCESSED)
#define PAGE_COPY               PAGE_COPY_NOEXEC
#define PAGE_READONLY           __pgprot(_PAGE_PRESENT | _PAGE_USER |   \
                                         _PAGE_ACCESSED | _PAGE_NX)
#define PAGE_READONLY_EXEC      __pgprot(_PAGE_PRESENT | _PAGE_USER |   \
                                         _PAGE_ACCESSED)

#define __PAGE_KERNEL_EXEC                                              \
  (_PAGE_PRESENT | _PAGE_RW | _PAGE_DIRTY | _PAGE_ACCESSED | _PAGE_GLOBAL)
#define __PAGE_KERNEL           (__PAGE_KERNEL_EXEC | _PAGE_NX)

#define __PAGE_KERNEL_RO                (__PAGE_KERNEL & ~_PAGE_RW)
#define __PAGE_KERNEL_RX                (__PAGE_KERNEL_EXEC & ~_PAGE_RW)
#define __PAGE_KERNEL_EXEC_NOCACHE      (__PAGE_KERNEL_EXEC | _PAGE_PCD | _PAGE_PWT)
#define __PAGE_KERNEL_WC                (__PAGE_KERNEL | _PAGE_CACHE_WC)
#define __PAGE_KERNEL_NOCACHE           (__PAGE_KERNEL | _PAGE_PCD | _PAGE_PWT)
#define __PAGE_KERNEL_UC_MINUS          (__PAGE_KERNEL | _PAGE_PCD)
#define __PAGE_KERNEL_VSYSCALL          (__PAGE_KERNEL_RX | _PAGE_USER)
#define __PAGE_KERNEL_VSYSCALL_NOCACHE  (__PAGE_KERNEL_VSYSCALL | _PAGE_PCD | _PAGE_PWT)
#define __PAGE_KERNEL_LARGE             (__PAGE_KERNEL | _PAGE_PSE)
#define __PAGE_KERNEL_LARGE_NOCACHE     (__PAGE_KERNEL | _PAGE_CACHE_UC | _PAGE_PSE)
#define __PAGE_KERNEL_LARGE_EXEC        (__PAGE_KERNEL_EXEC | _PAGE_PSE)

#define __PAGE_KERNEL_IO                (__PAGE_KERNEL | _PAGE_IOMAP)
#define __PAGE_KERNEL_IO_NOCACHE        (__PAGE_KERNEL_NOCACHE | _PAGE_IOMAP)
#define __PAGE_KERNEL_IO_UC_MINUS       (__PAGE_KERNEL_UC_MINUS | _PAGE_IOMAP)
#define __PAGE_KERNEL_IO_WC             (__PAGE_KERNEL_WC | _PAGE_IOMAP)

#define PAGE_KERNEL                     __pgprot(__PAGE_KERNEL)
#define PAGE_KERNEL_RO                  __pgprot(__PAGE_KERNEL_RO)
#define PAGE_KERNEL_EXEC                __pgprot(__PAGE_KERNEL_EXEC)
#define PAGE_KERNEL_RX                  __pgprot(__PAGE_KERNEL_RX)
#define PAGE_KERNEL_WC                  __pgprot(__PAGE_KERNEL_WC)
#define PAGE_KERNEL_NOCACHE             __pgprot(__PAGE_KERNEL_NOCACHE)
#define PAGE_KERNEL_UC_MINUS            __pgprot(__PAGE_KERNEL_UC_MINUS)
#define PAGE_KERNEL_EXEC_NOCACHE        __pgprot(__PAGE_KERNEL_EXEC_NOCACHE)
#define PAGE_KERNEL_LARGE               __pgprot(__PAGE_KERNEL_LARGE)
#define PAGE_KERNEL_LARGE_NOCACHE       __pgprot(__PAGE_KERNEL_LARGE_NOCACHE)
#define PAGE_KERNEL_LARGE_EXEC          __pgprot(__PAGE_KERNEL_LARGE_EXEC)
#define PAGE_KERNEL_VSYSCALL            __pgprot(__PAGE_KERNEL_VSYSCALL)
#define PAGE_KERNEL_VSYSCALL_NOCACHE    __pgprot(__PAGE_KERNEL_VSYSCALL_NOCACHE)

#define PGDIR_SHIFT     22
#define PTRS_PER_PGD    1024

/*
 * the i386 is two-level, so we don't really have any
 * PMD directory physically.
 */

#define PTRS_PER_PTE    1024

#ifndef __ASSEMBLY__
#define pgprot_val(x)   ((x).pgprot)
#define __pgprot(x)     ((pgprot_t) { (x) } )

// page_32_types.h
// 定义在文件pgtable_32.c
extern unsigned int __VMALLOC_RESERVE;

enum {
  PG_LEVEL_NONE,
  PG_LEVEL_4K,
  PG_LEVEL_2M,
  PG_LEVEL_1G,
  PG_LEVEL_NUM
};

#define pgd_index(address) (((address) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))
#define pgd_offset(mm, address) ((mm)->pgd + pgd_index((address)))
#define pgd_offset_k(address) pgd_offset(&init_mm, (address))

extern pteval_t __supported_pte_mask;
#endif 

#endif  // __PGTABLE_TYPES_H__