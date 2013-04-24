#ifndef __SEGMENT_H__
#define __SEGMENT_H__

#define __BOOT_CS 0x8
#define __BOOT_DS 0x10

#define __VIDEO_BASE   0x0B8000
#define __PAGE_OFFSET  0xC0000000

#define ALIGN  .align 4,0x90

#define ENTRY(name)                             \
  .globl name;                                  \
  ALIGN;                                        \
  name:

#define __ELF_RAW  0x800000

#endif  // __SEGMENT_H__
