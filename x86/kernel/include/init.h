#ifndef __INIT_H__
#define __INIT_H__

#include "compiler.h"

// arch/x86/include/asm/setup.h
#define COMMAND_LINE_SIZE 2048

#ifndef __ASSEMBLY__
extern unsigned int swapper_pg_dir[1024];

extern unsigned int init_pg_tables_end;

/*
 * Only for really core code.  See moduleparam.h for the normal way.
 *
 * Force the alignment so the compiler doesn't space elements of the
 * obs_kernel_param "array" too far apart in .init.setup.
 */
#define __setup_param(str, unique_id, fn, early)                        \
  static const char __setup_str_##unique_id[]                           \
  __aligned(1) = str;                                                   \
  static struct obs_kernel_param __setup_##unique_id                    \
  __used __section(.init.setup)                                         \
  __attribute__((aligned((sizeof(long)))))                              \
  = { __setup_str_##unique_id, fn, early }

#define __setup(str, fn)                        \
  __setup_param(str, fn, fn, 0)

/* NOTE: fn is as per module_param, not __setup!  Emits warning if fn
 * returns non-zero. */
#define early_param(str, fn)                    \
  __setup_param(str, fn, fn, 1)

/* Relies on boot_command_line being set */
void parse_early_param(void);
void parse_early_options(char *cmdline);

#define __initdata  __section(.init.data)
#define __initconst __section(.init.rodata)

#endif  /* __ASSEMBLY__ */

#endif  /* __INIT_H__ */
