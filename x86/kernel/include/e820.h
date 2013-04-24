#ifndef __E820_H__
#define __E820_H__

#define E820MAP 0x2d0           /* our map */
#define E820MAX 128             /* number of entries in E820MAP */

#define E820_X_MAX E820MAX

#define E820NR  0x1e8           /* # entries in E820MAP */
#define E820_RAM        1
#define E820_RESERVED   2
#define E820_ACPI       3
#define E820_NVS        4
#define E820_UNUSABLE   5

/* reserved RAM used by kernel itself */
#define E820_RESERVED_KERN        128

#ifndef __ASSEMBLY__
#include <types.h>
struct e820entry {
  __u64 addr;     /* start of memory segment */
  __u64 size;     /* size of memory segment */
  __u32 type;     /* type of memory segment */
} __attribute__((packed));

struct e820map {
  __u32 nr_map;
  struct e820entry map[E820_X_MAX];
};

extern struct e820map e820;
extern struct e820map e820_saved;

#define BIOS_BEGIN              0x000a0000
#define BIOS_END                0x00100000
#define HIGH_MEMORY     (1024*1024)

char* default_machine_specific_memory_setup(void);
unsigned long e820_end_of_ram_pfn(void);
u64 find_e820_area(u64 start, u64 end, u64 size, u64 align);
int sanitize_e820_map(struct e820entry *biosmap, int max_nr_map,
                      u32 *pnr_map);
int e820_find_active_region(const struct e820entry* ei,
                            unsigned long start_pfn,
                            unsigned long last_pfn,
                            unsigned long* ei_startpfn,
                            unsigned long* ei_endpfn);
void e820_register_active_regions(int nid, unsigned long start_pfn,
                                  unsigned long last_pfn);
void finish_e820_parsing(void);
void reserve_early(u64 start, u64 end, char *name);
void reserve_early_overlap_ok(u64 start, u64 end, char *name);
#endif  // __ASSEMBLY__

#endif  // __E820_H__
