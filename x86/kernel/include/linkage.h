#ifndef __LINKAGE_H__
#define __LINKAGE_H__

#define __fastcall__ __attribute__((regparm(3)))
#define asmlinkage __attribute__((regparm(0)))

#ifdef __ASSEMBLY__

#define ALIGN  .align 4,0x90

#define ENTRY(name)                             \
  .globl name;                                  \
  ALIGN;                                        \
name:

#define GLOBAL(name)                            \
  .globl name;                                  \
name:

#define END(name)                               \
  .size name, .-name

#define ENDPROC(name)                           \
  .type name, @function;                        \
  END(name)

#endif  // __ASSEMBLY__

#define __ELF_RAW  0x800000

#define __VIDEO_BASE   0x0B8000
#define __PAGE_OFFSET  0xC0000000


#endif  // __LINKAGE_H__
