#include "kernel.h"

int parse_args(const char* name,
               char* args,
               struct kernel_param* param,
               unsigned num,
               int (*unknown)(char* param, char* val)) {
  char *param, *val;

  while (isspace(*args)) {
    args++;
  }

  while (*args) {
    int ret;
    int irq_was_disabled;

    args = next_arg(args, &param, &val);
  }
}
