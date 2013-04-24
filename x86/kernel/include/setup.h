#ifndef __SETUP_H__
#define __SETUP_H__

#include "pfn.h"

/*
 * Do NOT EVER look at the BIOS memory size location.
 * It does not work on many machines.
 */
#define LOWMEMSIZE()    (0x9f000)

#define MAXMEM_PFN      PFN_DOWN(MAXMEM)
#define MAX_NONPAE_PFN  (1 << 20)

extern unsigned long max_low_pfn_mapped;
extern unsigned long max_pfn_mapped;

extern struct boot_params boot_params;

#endif  // __SETUP_H__
