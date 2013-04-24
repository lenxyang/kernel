#include <asm/boot.h>

void cleanscreen(void) {
  struct biosregs ireg;
  initregs(&ireg);
  ireg.bh = 0x0f;
  ireg.cx = 0x0;
  ireg.dh = 24;
  ireg.dl = 79;
  ireg.ax = 0x0600;
  intcall(0x10, &ireg, NULL);

  initregs(&ireg);
  ireg.ah = 0x2;
  ireg.bx = 0;
  ireg.dx = 0;
  intcall(0x10, &ireg, NULL);
}

void putchar(int ch) {
  struct biosregs ireg;

  if (ch == '\n')
    putchar('\r');  /* \n -> \r\n */

  initregs(&ireg);
  ireg.bx = 0x0007;
  ireg.cx = 0x0001;
  ireg.ah = 0x0e;
  ireg.al = ch;
  intcall(0x10, &ireg, NULL);
}

void puts(const char *str) {
  while (*str)
    putchar(*str++);
}
