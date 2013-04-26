#include <linux/mm.h>
#include <asm/pgtable.h>

unsigned int __VMALLOC_RESERVE = 128 << 20;

unsigned long __FIXADDR_TOP = 0xfffff000;

void set_pte_vaddr(unsigned long vaddr, pte_t pteval) {
}


