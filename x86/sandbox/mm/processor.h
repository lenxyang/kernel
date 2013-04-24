#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

#define load_cr3(pgdir)                         \
  asm volatile("movl %0,%%cr3": :"r" (__pa(pgdir)))

#endif  // __PROCESSOR_H__
