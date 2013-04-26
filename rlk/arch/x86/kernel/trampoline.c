#include <asm/trampoline.h>
#include <asm/e820.h>
#include <asm/pgtable.h>

/*
void __init reserve_trampoline_memory(void) {
  reserve_early(PAGE_SIZE, PAGE_SIZE + PAGE_SIZE, "EX TRAMPOLINE");
  reserve_early(TRAMPOLINE_BASE, TRAMPOLINE_BASE + TRAMPOLINE_SIZE,
                "TRAMPOLINE");
}
*/
