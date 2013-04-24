#include "x86_init.h"
#include "init.h"
#include "e820.h"

struct x86_init_ops x86_init __initdata = {
  .resources = {
    .memory_setup = default_machine_specific_memory_setup,
  },
};
