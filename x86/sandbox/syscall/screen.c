#include "screen.h"

struct screen_pos {
  int row;
  int col;
};

struct screen_pos g_pos = {
  .row = 0,
  .col = 0,
};

void clean_screen(void) {
  asm volatile(
      "xor  %eax, %eax        \n\t"
      "mov  $(80 * 25), %ecx  \n\t"
      "mov  $0x0B8000, %edi   \n\t"
      "rep  stosw             \n\t"
               );
}

void set_position(uint32 row, uint32 col) {
  g_pos.col = col;
  g_pos.row = row;
}

void puts(char* str) {
  while (*str != '\0') {
    putc(*str);
    str++;
  }
}

/*
 * function handle_control_char 
 * 打印控制字符，如果是控制字符返回 1
 * 否则返回 0
 */
static int handle_control_char(char c) {
  switch (c) {
    case '\n':
      g_pos.row++;
      return 1;
    case '\r':
      g_pos.col = 0;
      return 1;
    default:
      return 0;
  }
}

void putc(char c) {
  put_color_char(c, SCREEN_COL_WHITE);
}

void put_color_char(char c, uint8 colorindex) {
  if (handle_control_char(c)) {
    return;
  }
  
  int offset = (g_pos.row * 80 + g_pos.col) * 2;
  asm volatile(
      "movl   $0x0B8000, %%edi       \n\t"
      "addl   %%ecx, %%edi           \n\t"
      "movb   %%bl, %%ah            \n\t"
      "movw   %%ax, (%%edi)          \n\t"
      :
      : "aN"(c), "bN"(colorindex), "c"(offset)
               );
  g_pos.col++;
  if (g_pos.col % SCREEN_DEF_COLUMN == 0) {
    g_pos.row++;
    g_pos.col = 0;
  }
}
