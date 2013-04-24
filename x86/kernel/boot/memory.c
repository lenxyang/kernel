#include "boot/boot.h"
#include "types.h"
#include "processor-flags.h"
#include "kernel.h"

#define SMAP  0x534d4150

static int detect_memory_e820(void) {

  int count = 0;
  struct biosregs ireg, oreg;
  struct e820entry *desc = boot_params.e820_map;
  static struct e820entry buf;

  initregs(&ireg);
  ireg.eax = 0xe820;
  ireg.ecx = sizeof(buf);
  ireg.edx = SMAP;
  ireg.edi = (__u16)&buf;

  do {
    intcall(0x15, &ireg, &oreg);
    ireg.ebx = oreg.ebx; /* next iteration */

    if (oreg.eflags & X86_EFLAGS_OF) {
      break;
    }

    if (oreg.eax != SMAP) {
      count = 0;
      break;
    }

    *desc++ = buf;
    count++;
  } while (ireg.ebx && count < ARRAY_SIZE(boot_params.e820_map));

  return boot_params.e820_entries  = count;
}

char* memory_type(int type) {
  switch (type) {
    case 1:
      return "KER";
    case 2:
      return "REV";
    default:
      return "UNK";
  }
}

void dump_memory() {
  cleanscreen();
  int i = 0;
  printf("  begin   \t\t   end   \t\t   size   \t\t  type  \n");
  for (i = 0; i < boot_params.e820_entries; ++i) {
    unsigned int begin = (unsigned int)boot_params.e820_map[i].addr;
    unsigned int size = (unsigned int)boot_params.e820_map[i].size;
    unsigned int end = begin + size - 1;
    char* typestr = memory_type(boot_params.e820_map[i].type);
    printf("0x%08x\t\t0x%08x\t\t0x%08x\t\t%s\n",
           begin, end, size, typestr);
  }
}

static int detect_memory_e801(void) {
  return 0;
}

static int detect_memory_88(void) {
  return 0;
}

int detect_memory(void) {
  int err = -1;
  if (detect_memory_e820() > 0) {
    err = 0;
  }

  if (!detect_memory_e801()) {
    err = 0;
  }
  
  if (!detect_memory_88()) {
    err = 0;
  }

  dump_memory();
  return err;
}

