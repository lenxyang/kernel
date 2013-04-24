#include "io.h"

// int iodelay_type = CONFIG_IO_DELAY_TYPE_0X80;

void native_io_delay(void) {
  /*
  switch(io_delay_type) {
    case CONFIG_IO_DELAY_TYPE_0X80:
      asm volatile ("outb %al, $0x80");
      break;
    case CONFIG_IO_DELAY_TYPE_NONE:
      break;
  }
  */
  asm volatile ("outb %al, $0x80");
}
