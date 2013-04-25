#ifndef __BOOT_H__
#define __BOOT_H__


#include <linux/types.h>
#include <linux/stdarg.h>

extern struct setup_header hdr;
extern struct boot_params boot_params;

static inline u16 ds(void) {
  u16 seg;
  asm("movw %%ds,%0" : "=rm" (seg));
  return seg;
}

static inline u16 fs(void)
{
  u16 seg;
  asm volatile("movw %%fs,%0" : "=rm" (seg));
  return seg;
}

static inline u16 gs(void)
{
  u16 seg;
  asm volatile("movw %%gs,%0" : "=rm" (seg));
  return seg;
}

static inline int isdigit(int ch)
{
  return (ch >= '0') && (ch <= '9');
}

/* bioscall.c */
struct biosregs {
  union {
    struct {
      u32 edi;
      u32 esi;
      u32 ebp;
      u32 _esp;
      u32 ebx;
      u32 edx;
      u32 ecx;
      u32 eax;
      u32 _fsgs;
      u32 _dses;
      u32 eflags;
    };
    struct {
      u16 di, hdi;
      u16 si, hsi;
      u16 bp, hbp;
      u16 _sp, _hsp;
      u16 bx, hbx;
      u16 dx, hdx;
      u16 cx, hcx;
      u16 ax, hax;
      u16 gs, fs;
      u16 es, ds;
      u16 flags, hflags;
    };
    struct {
      u8 dil, dih, edi2, edi3;
      u8 sil, sih, esi2, esi3;
      u8 bpl, bph, ebp2, ebp3;
      u8 _spl, _sph, _esp2, _esp3;
      u8 bl, bh, ebx2, ebx3;
      u8 dl, dh, edx2, edx3;
      u8 cl, ch, ecx2, ecx3;
      u8 al, ah, eax2, eax3;
    };
  };
};

/* regs.c */
void initregs(struct biosregs *regs);

/* copy.S */
typedef unsigned int addr_t;
void copy_to_fs(addr_t dst, void *src, size_t len);
void *copy_from_fs(void *dst, addr_t src, size_t len);
void *memcpy(void *dst, void *src, size_t len);
void *memset(void *dst, int c, size_t len);

/* 为何还要如此定义 */
#define memcpy(d,s,l) __builtin_memcpy(d,s,l)
#define memset(d,c,l) __builtin_memset(d,c,l)

/* printf.c */
int sprintf(char *buf, const char *fmt, ...);
int vsprintf(char *buf, const char *fmt, va_list args);
int printf(const char *fmt, ...);

/* string.c */
int strcmp(const char *str1, const char *str2);
size_t strnlen(const char *s, size_t maxlen);
unsigned int atou(const char *s);

/* tty.c */
void cleanscreen(void);
void puts(const char *);
void putchar(int);

void intcall(u8 int_no, const struct biosregs *ireg, struct biosregs *oreg);

int detect_memory(void);

void go_to_protected_mode(void);

#endif // __BOOT_H__
