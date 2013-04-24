#ifndef __KERNEL_H__
#define __KERNEL_H__

#include "stdarg.h"
#include "compiler.h"
#include "types.h"
#include "linkage.h"
#include "console.h"

#define KERN_EMERG      "<0>"   /* system is unusable                   */
#define KERN_ALERT      "<1>"   /* action must be taken immediately     */
#define KERN_CRIT       "<2>"   /* critical conditions                  */
#define KERN_ERR        "<3>"   /* error conditions                     */
#define KERN_WARNING    "<4>"   /* warning conditions                   */
#define KERN_NOTICE     "<5>"   /* normal but significant condition     */
#define KERN_INFO       "<6>"   /* informational                        */
#define KERN_DEBUG      "<7>"   /* debug-level messages                 */

/* Use the default kernel loglevel */
#define KERN_DEFAULT    "<d>"
/*
 * Annotation for a "continued" line of log printout (only done after a
 * line that had no enclosing \n). Only to be used by core/arch code
 * during early bootup (a continued line is not SMP-safe otherwise).
 */
#define KERN_CONT       "<c>"

extern int console_printk[];

#define console_loglevel (console_printk[0])
#define default_message_loglevel (console_printk[1])
#define minimum_console_loglevel (console_printk[2])
#define default_console_loglevel (console_printk[3])

asmlinkage int vprintk(const char *fmt, va_list args)
    __attribute__ ((format (printf, 1, 0)));
asmlinkage int printk(const char * fmt, ...)
    __attribute__ ((format (printf, 1, 2))) __cold;

#define USHORT_MAX      ((u16)(~0U))
#define SHORT_MAX       ((s16)(USHORT_MAX>>1))
#define SHORT_MIN       (-SHORT_MAX - 1)
#define INT_MAX         ((int)(~0U>>1))
#define INT_MIN         (-INT_MAX - 1)
#define UINT_MAX        (~0U)
#define LONG_MAX        ((long)(~0UL>>1))
#define LONG_MIN        (-LONG_MAX - 1)
#define ULONG_MAX       (~0UL)
#define LLONG_MAX       ((long long)(~0ULL>>1))
#define LLONG_MIN       (-LLONG_MAX - 1)
#define ULLONG_MAX      (~0ULL)

/*
 * Print a one-time message (analogous to WARN_ONCE() et al):
 */
#define printk_once(x...) ({                    \
      static bool __print_once = true;          \
                                                \
      if (__print_once) {                       \
        __print_once = false;                   \
        printk(x);                              \
      }                                         \
    })

extern void asmlinkage __attribute__((format(printf, 1, 2)))
    early_printk(const char *fmt, ...);

extern const char hex_asc[];
#define hex_asc_lo(x)   hex_asc[((x) & 0x0f)]
#define hex_asc_hi(x)   hex_asc[((x) & 0xf0) >> 4]
static inline char *pack_hex_byte(char *buf, u8 byte)
{
  *buf++ = hex_asc_hi(byte);
  *buf++ = hex_asc_lo(byte);
  return buf;
}

extern unsigned long simple_strtoul(const char *,char **,unsigned int);
extern long simple_strtol(const char *,char **,unsigned int);
extern unsigned long long simple_strtoull(const char *,char **,unsigned int);
extern long long simple_strtoll(const char *,char **,unsigned int);
extern int strict_strtoul(const char *, unsigned int, unsigned long *);
extern int strict_strtol(const char *, unsigned int, long *);
extern int strict_strtoull(const char *, unsigned int, unsigned long long *);
extern int strict_strtoll(const char *, unsigned int, long long *);
extern int sprintf(char * buf, const char * fmt, ...)
    __attribute__ ((format (printf, 2, 3)));
extern int vsprintf(char *buf, const char *, va_list)
    __attribute__ ((format (printf, 2, 0)));
extern int snprintf(char * buf, size_t size, const char * fmt, ...)
    __attribute__ ((format (printf, 3, 4)));
extern int vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
    __attribute__ ((format (printf, 3, 0)));
extern int scnprintf(char * buf, size_t size, const char * fmt, ...)
    __attribute__ ((format (printf, 3, 4)));
extern int vscnprintf(char *buf, size_t size, const char *fmt, va_list args)
    __attribute__ ((format (printf, 3, 0)));
extern char *kasprintf(gfp_t gfp, const char *fmt, ...)
    __attribute__ ((format (printf, 2, 3)));
extern char *kvasprintf(gfp_t gfp, const char *fmt, va_list args);

extern int sscanf(const char *, const char *, ...)
    __attribute__ ((format (scanf, 2, 3)));
extern int vsscanf(const char *, const char *, va_list)
    __attribute__ ((format (scanf, 2, 0)));

#define ALIGN(x,a)              __ALIGN_MASK(x,(typeof(x))(a)-1)
#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))
#define PTR_ALIGN(p, a)         ((typeof(p))ALIGN((unsigned long)(p), (a)))
#define IS_ALIGNED(x, a)                (((x) & ((typeof(x))(a) - 1)) == 0)

#define ARRAY_SIZE(x) sizeof((x)) / sizeof((x)[0])

#define FIELD_SIZEOF(t, f) (sizeof(((t*)0)->f))
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))
#define roundup(x, y) ((((x) + ((y) - 1)) / (y)) * (y))
#define DIV_ROUND_CLOSEST(x, divisor)(                  \
    {                                                       \
        typeof(divisor) __divisor = divisor;            \
        (((x) + ((__divisor) / 2)) / (__divisor));      \
        }                                                       \
      )


/*
 * swap - swap value of @a and @b
 */
#define swap(a, b) \
  do { typeof(a) __tmp = (a); (a) = (b); (b) = __tmp; } while (0)

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:        the pointer to the member.
 * @type:       the type of the container struct this is embedded in.
 * @member:     the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({                      \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );})


#endif // __KERNEL_H__
