#ifndef __PAGE_H__
#define __PAGE_H__

/*
 * 这些变量在 bootmem当中初始化
 *
 */
extern unsigned long max_low_pfn;
extern unsigned long min_low_pfn;
extern unsigned long max_pfn;

#define PAGE_SHIFT 12
#define PAGE_SIZE  (1Ul << PAGE_SHIFT)
#define PAGE_MASK  (~(PAGE_SIZE - 1))

#ifndef __ASSEMBLY__

typedef struct {unsigned long pte_low;} pte_t;
typedef struct {unsigned long pud;}     pud_t;
typedef struct {unsigned long pmd;}     pmd_t;
typedef struct {unsigned long pgd;}     pgd_t;
typedef struct {unsigned long pgprot;}  pgprot_t;

#define pte_val(x) ((x).pte_low)
#define pmd_val(x) ((x).pmd)
#define pgd_val(x) ((x).pgd)
#define pgprot_val(x)  ((x).pgprot)

#define __pte(x) ((pte_t) {(x)})
#define __pmd(x) ((pmd_t) {(x)})
#define __pgd(x) ((pgd_t) {(x)})
#define __pgprot(x) ((pgprot_t) {(x)})

#endif

#ifdef __ASSEMBLY__
#define PAGE_OFFSET (0xC0000000)
#else
#define PAGE_OFFSET (0xC0000000UL)
#endif

#define __PAGE_OFFSET PAGE_OFFSET

#ifndef __ASSEMBLY__
#define __pa(x)            ((unsigned long)(x) - __PAGE_OFFSET)
#define __va(x)            ((unsigned long)(x) + __PAGE_OFFSET)
#define pfn_to_page(pfn)   (mem_map + (pfn))
#define page_to_pfn(page)  ((unsigned long)((page) - mem_page))
#define pfn_valid(pfn)     ((pfn) < max_maphr)

#endif  // __ASSEMBLY__

#endif  // __PAGE_H__
