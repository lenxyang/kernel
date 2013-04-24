#ifndef __X86_INIT_H__
#define __X86_INIT_H__

struct x86_init_resources {
  char *(*memory_setup)(void);
};

struct x86_init_ops {
  struct x86_init_resources resources;
};

extern struct x86_init_ops x86_init;

#endif // __X86_INIT_H__
