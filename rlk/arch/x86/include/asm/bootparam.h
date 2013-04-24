#ifndef __BOOTPARAM_H__
#define __BOOTPARAM_H__

#include <linux/types.h>
#include <asm/e820.h>

#ifndef __ASSEMBLY__
struct setup_header {
  __u16   jump;
  __u32   code32_start;
} __attribute__((packed));

struct boot_params {
  /* struct screen_info screen_info; */
  __u32 alt_mem_k;
  __u8 e820_entries;
  __u8 eddbuf_entries;
  struct setup_header hdr;
  struct e820entry e820_map[E820MAX];
  /*
   * 保证结构体至少为4096字节大小
   */
  char buffer[4096];
} __attribute__((packed));

#endif  // __ASSEMBLY__

#define BOOTPARAM_SIZE 4096         /* sizeof(struct boot_params) */

#endif  // __BOOTPARAM_H__
