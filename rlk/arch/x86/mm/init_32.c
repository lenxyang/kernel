#include <linux/linkage.h>
#include <linux/types.h>
#include <asm/pgtable.h>

pteval_t __supported_pte_mask = ~(_PAGE_NX | _PAGE_GLOBAL | _PAGE_IOMAP);
