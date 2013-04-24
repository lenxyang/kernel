#ifndef __PGTABLE_2LEVEL_H__
#define __PGTABLE_2LEVEL_H__

#include "pgtable_2level_defs.h"

#define set_pte(pteptr, pteval) (*(pteptr) = pteval)
#define set_pte_atomic(pteptr, pteval) set_pte(pteptr, pteval)
#define set_pmd(pmdptr, pmdval) (*(pmdptr) = pmdval)


#define pte_page(x) pfn_to_page(pte_pfn(x))
#define pte_none(x) (!(x).pte_low)
#define pte_pfn(x) ((unsigned long)(((x).pte_low >> PAGE_SHIFT)))
#define pfn_pte(pfn, prot) __pte(((pfn) << PAGE_SHIFT) | pgprot_val(prot))
#define pfn_pmd(pfn, prot) __pmd(((pfn) << PAGE_SHIFT) | pgprot_val(prot))

#define pmd_page(pmd) (pfn_to_page(pmd_val(pmd) >> PAGE_SHIFT))
#define pmd_page_kernel(pmd)                    \
  ((unsigned long) __va(pmd_val(pmd) & PAGE_MASK))

#endif  // __PGTABLE_2LEVEL_H__
