#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__

// arch/x86/include/asm/spinlock_types.h
typedef struct raw_spinlock {
  unsigned int slock;
} raw_spinlock_t;

typedef struct {
  unsigned long lock;
} raw_rwlock_t;


// include/linux/spinlock_types.h"
typedef struct {
  raw_spinlock_t raw_lock;
  unsigned int magic, owner_cpu;
  void *owner;
}spinlock_t;



#endif  // __SPINLOCK_H__
