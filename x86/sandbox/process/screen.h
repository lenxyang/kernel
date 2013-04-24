#ifndef __SCREEN_H__
#define __SCREEN_H__

#include "basictypes.h"

#define SCREEN_DEF_COLUMN 80
#define SCREEN_DEF_ROW    25

#define SCREEN_COL_RED      0x0C
#define SCREEN_COL_WHITE    0x0F

#ifndef __ASSEMBLY__

void init_screen(void);

/*
 * void
 */
void set_position(uint32 row, uint32 col);

/*
 * clean_screen 清屏函数
 */
void clean_screen(void);

/*
 * function puts
 * 此函数通过直接写显存来完成
 */
void puts(char* str);
void putc(char c);

/*
 * 打印带有颜色的文字
 */
void put_color_char(char c, uint8 colorindex);

#endif  /* __ASSEMBLY__ */

#endif /* __SCREEN_H__ */
