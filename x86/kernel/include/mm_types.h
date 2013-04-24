#ifndef __MM_TYPES_H__
#define __MM_TYPES_H__

#include "types.h"

struct page {
  unsigned long flags;
  atomic_t _count;
  union {
    atomic_t _mapcount;
    struct {
      u16 inuse;
      u16 objects;
    };
  };

  union {
    /*
    struct {
      unsigned long private;
      struct address_space *mapping;
    };
    */

    struct page* first_page;
  };

  void* virtual;
  void* shadow;
};

#endif  // __MM_TYPES_H__
