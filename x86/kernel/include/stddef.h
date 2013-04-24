#ifndef __STDDEF_H__
#define __STDDEF_H__

enum {
  false   = 0,
  true    = 1
};

#undef NULL
#if defined(__cplusplus)
#define NULL 0
#else
#define NULL ((void *)0)
#endif

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#endif  // __STDDEF_H__
