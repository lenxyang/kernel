#ifndef __PGTABLE_H__
#define __PGTABLE_H__

#include "const.h"

#ifndef __ASSEMBLY__

typedef unsigned long   pteval_t;
typedef unsigned long   pmdval_t;
typedef unsigned long   pudval_t;
typedef unsigned long   pgdval_t;
typedef unsigned long   pgprotval_t;

typedef union {
pteval_t pte;
pteval_t pte_low;
} pte_t;

typedef struct pgprot { pgprotval_t pgprot; } pgprot_t;
typedef struct { pgdval_t pgd; } pgd_t;


/*
 * Having the pud type consist of a pgd gets the size right, and allows
 * us to conceptually access the pgd entry that this pud is folded into
 * without casting.
 */
typedef struct { pgd_t pgd; } pud_t;

#define PUD_SHIFT       PGDIR_SHIFT
#define PTRS_PER_PUD    1
#define PUD_SIZE        (1UL << PUD_SHIFT)
#define PUD_MASK        (~(PUD_SIZE-1))

static inline int pgd_none(pgd_t pgd)           { return 0; }
static inline int pgd_bad(pgd_t pgd)            { return 0; }
static inline int pgd_present(pgd_t pgd)        { return 1; }
static inline void pgd_clear(pgd_t *pgd)        { }


/*
 * Having the pmd type consist of a pud gets the size right, and allows
 * us to conceptually access the pud entry that this pmd is folded into
 * without casting.
 */
typedef struct { pud_t pud; } pmd_t;

#define PMD_SHIFT       PUD_SHIFT
#define PTRS_PER_PMD    1
#define PMD_SIZE        (1UL << PMD_SHIFT)
#define PMD_MASK        (~(PMD_SIZE-1))

/*
 * The "pud_xxx()" functions here are trivial for a folded two-level
 * setup: the pmd is never bad, and a pmd always exists (as it's folded
 * into the pud entry)
 */
static inline int pud_none(pud_t pud)           { return 0; }
static inline int pud_bad(pud_t pud)            { return 0; }
static inline int pud_present(pud_t pud)        { return 1; }
static inline void pud_clear(pud_t *pud)        { }

static inline pud_t* pud_offset(pgd_t *pgd, unsigned long address) {
  return (pud_t *)pgd;
}

static inline pmd_t* pmd_offset(pud_t *pud, unsigned long address) {
  return (pmd_t *)pud;
}

#define pud_val(x)                              (pgd_val((x).pgd))
#define __pud(x)                                ((pud_t) { __pgd(x) } )
#define pte_val(x)      ((x).pte)
#define pmd_val(x)      ((&x)->pud)
#define pgd_val(x)      ((x).pgd)
#define pgprot_val(x)   ((x).pgprot)
#define __pgprot(x)     ((pgprot_t) { (x) } )

#endif // __ASSEMBLY__

#define __PAGE_OFFSET 0xC0000000
#define PAGE_OFFSET __PAGE_OFFSET

#define PAGE_SHIFT 12
#define PAGE_SIZE  (1UL << PAGE_SHIFT)
#define PAGE_MASK  (~(PAGE_SIZE-1))

// include/asm/pgtable_2level-types.h
#define PTRS_PER_PTE   1024

#define __PHYSICAL_MASK_SHIFT   32
#define __VIRTUAL_MASK_SHIFT    32

#define __pa(x) ((x) - PAGE_OFFSET)
#define __va(x) ((x) + PAGE_OFFSET)

#define pa(x) __pa(x)
#define va(x) __va(x)


#include "pgtable_types.h"

#ifndef __ASSEMBLY__
unsigned long pages_to_mb(unsigned long npg);

/*
 * Mask out unsupported bits in a present pgprot.  Non-present pgprots
 * can use those bits for other purposes, so leave them be.
 */
static inline pgprotval_t massage_pgprot(pgprot_t pgprot)
{
  pgprotval_t protval = pgprot_val(pgprot);

  if (protval & _PAGE_PRESENT)
    protval &= __supported_pte_mask;

  return protval;
}

static inline void set_pte(pte_t *ptep, pte_t pte) {
*ptep = pte;
}

static inline pte_t __pte(pteval_t val) {
return (pte_t)val;
}

static inline unsigned long pte_index(unsigned long address)
{
return (address >> PAGE_SHIFT) & (PTRS_PER_PTE - 1);
}

static inline pte_t pfn_pte(unsigned long page_nr, pgprot_t pgprot)
{
return __pte(((phys_addr_t)page_nr << PAGE_SHIFT) | massage_pgprot(pgprot));
}

#endif  // __ASSEMBLY__

#endif   // __PGTABLE_H__
