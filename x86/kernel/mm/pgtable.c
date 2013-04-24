#include "page.h"

unsigned int __VMALLOC_RESERVE = 128 << 20;

unsigned long __FIXADDR_TOP = 0xfffff000;


unsigned long pages_to_mb(unsigned long npg) {
  return npg >> (20 - PAGE_SHIFT); 
}
