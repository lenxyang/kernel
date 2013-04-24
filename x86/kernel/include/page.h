#ifndef __PAGE_H__
#define __PAGE_H__

#include "pgtable.h"

#ifndef __ASSEMBLY__
void setup_bootmem_allocator(void);
void find_low_pfn_range(void);
unsigned long init_memory_mapping(unsigned long start,
                                  unsigned long end);
#endif  // __ASSEMBLY__

// arch/x86/include/asm/sparsemem.h
#define SECTION_SIZE_BITS     26
#define MAX_PHYSADDR_BITS     32
#define MAX_PHYSMEM_BITS      32


// arch/x86/include/asm/pgtable_32_types.h:

/* Just any arbitrary offset to the start of the vmalloc VM area: the
 * current 8MB value just means that there will be a 8MB "hole" after the
 * physical memory until the kernel virtual memory starts.  That means that
 * any out-of-bounds memory accesses will hopefully be caught.
 * The vmalloc() routines leaves a hole of 4kB between each vmalloced
 * area for the same reason. ;)
 */
#define VMALLOC_OFFSET  (8 * 1024 * 1024)

#ifndef __ASSEMBLY__
extern bool __vmalloc_start_set; /* set once high_memory is set */
#endif

#define VMALLOC_START   ((unsigned long)high_memory + VMALLOC_OFFSET)

#define LAST_PKMAP 1024

#define PKMAP_BASE ((FIXADDR_BOOT_START - PAGE_SIZE * (LAST_PKMAP + 1)) \
                    & PMD_MASK)

#define VMALLOC_END    (PKMAP_BASE - 2 * PAGE_SIZE)

#define MAXMEM  (VMALLOC_END - PAGE_OFFSET - __VMALLOC_RESERVE)

#ifndef __ASSEMBLY__

#endif  // __ASSEMBLY__

#endif  // __PAGE_H__
