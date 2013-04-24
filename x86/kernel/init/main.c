#include "init.h"
#include "stdarg.h"
#include "stddef.h"
#include "highmem.h"
#include "linkage.h"
#include "processor.h"

/*
 * 内核启动，此函数有arch/x86/kernel/head_32.S调用
 */
void start_kernel(void) {
  page_address_init();

  /*
   * setup_arch定义在 arch/x86/kernel/setup.c当中
   * 此处放在kernel/setup.c
   */
  setup_arch();

  while (true) {
    
  }
}
