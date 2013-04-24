#include "stdarg.h"
#include "stddef.h"
#include "kernel.h"
#include "bug.h"

struct slowpath_args {
  const char *fmt;
  va_list args;
};

void panic(const char * fmt, ...) {
  static char buf[1024];
  va_list args;
  // long i;

  /*
   * It's possible to come here directly from a panic-assertion and
   * not have preempt disabled. Some functions called from here want
   * preempt to be disabled. No point enabling it later though...
   */
  // preempt_disable();

  // bust_spinlocks(1);
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);
  printk(KERN_EMERG "Kernel panic - not syncing: %s\n",buf);
  /*
#ifdef CONFIG_DEBUG_BUGVERBOSE
  dump_stack();
#endif
  */
}


static void warn_slowpath_common(const char *file, int line, void *caller,
                                 struct slowpath_args *args)
{
  const char *board;

  printk(KERN_WARNING "------------[ cut here ]------------\n");
  printk(KERN_WARNING "WARNING: at %s:%d %pS()\n", file, line, caller);
  /*
  board = dmi_get_system_info(DMI_PRODUCT_NAME);
  if (board) {
    printk(KERN_WARNING "Hardware name: %s\n", board);
  }
  */

  if (args)
    vprintk(args->fmt, args->args);

  /*
  print_modules();
  dump_stack();
  print_oops_end_marker();
  add_taint(TAINT_WARN);
  */
}

void warn_slowpath_fmt(const char *file, int line, const char *fmt, ...)
{
  struct slowpath_args args;

  args.fmt = fmt;
  va_start(args.args, fmt);
  warn_slowpath_common(file, line, __builtin_return_address(0), &args);
  va_end(args.args);
}

void warn_slowpath_null(const char *file, int line)
{
  warn_slowpath_common(file, line, __builtin_return_address(0), NULL);
}
