
#include "page.h"
#include "pgtable.h"
#include "kernel.h"
#include "internal.h"
#include "bug.h"

/* Validate the physical addressing limitations of the model */
void mminit_validate_memmodel_limits(unsigned long *start_pfn,
                                     unsigned long *end_pfn) {
  unsigned long max_sparsemem_pfn = 1UL << (MAX_PHYSMEM_BITS-PAGE_SHIFT);

  /*
   * Sanity checks - do not allow an architecture to pass
   * in larger pfns than the maximum scope of sparsemem:
   */
  if (*start_pfn > max_sparsemem_pfn) {
    mminit_dprintk(MMINIT_WARNING, "pfnvalidation",
                   "Start of range %lu -> %lu exceeds SPARSEMEM max %lu\n",
                   *start_pfn, *end_pfn, max_sparsemem_pfn);
    WARN_ON_ONCE(1);
    *start_pfn = max_sparsemem_pfn;
    *end_pfn = max_sparsemem_pfn;
  } else if (*end_pfn > max_sparsemem_pfn) {
    mminit_dprintk(MMINIT_WARNING, "pfnvalidation",
                   "End of range %lu -> %lu exceeds SPARSEMEM max %lu\n",
                   *start_pfn, *end_pfn, max_sparsemem_pfn);
    WARN_ON_ONCE(1);
    *end_pfn = max_sparsemem_pfn;
  }
}
