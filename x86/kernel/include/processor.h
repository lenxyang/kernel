#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__


#include "pgtable.h"
#include "nops.h"
#include "cpufeature.h"
#include "alternative.h"
#include "stringify.h"

#ifndef __ASSEMBLY__

/* REP NOP (PAUSE) is a good thing to insert into busy-wait loops. */
static inline void rep_nop(void)
{
  asm volatile("rep; nop" ::: "memory");
}

static inline void cpu_relax(void)
{
  rep_nop();
}


static inline void load_cr3(pgd_t *pgdir)
{
  __asm__ __volatile__("movl %0, %%cr3  \n\t"
                       :
                       :"r" (pa(pgdir))
                       :"eax"
                       );
}



#define BASE_PREFETCH          ASM_NOP4
#define ARCH_HAS_PREFETCH


/*
 * Prefetch instructions for Pentium III (+) and AMD Athlon (+)
 *
 * It's not worth to care about 3dnow prefetches for the K6
 * because they are microcoded there and very slow.
 */
static inline void prefetch(const void *x)
{
  alternative_input(BASE_PREFETCH,
                    "prefetchnta (%1)",
                    X86_FEATURE_XMM,
                    "r" (x));
}

/*
 * 3dnow prefetch to get an exclusive cache line.
 * Useful for spinlocks to avoid one state transition in the
 * cache coherency protocol:
 */
static inline void prefetchw(const void *x)
{
  alternative_input(BASE_PREFETCH,
                    "prefetchw (%1)",
                    X86_FEATURE_3DNOW,
                    "r" (x));
}

static inline void spin_lock_prefetch(const void *x)
{
  prefetchw(x);
}


#endif  // __ASSEMBLY__

#endif  // __PROCESSOR_H__
