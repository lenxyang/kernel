#ifndef _LINUX_KERNEL_H
#define _LINUX_KERNEL_H


#ifdef __KERNEL__
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

#define STACK_MAGIC     0xdeadbeef

#define ALIGN(x,a)              __ALIGN_MASK(x,(typeof(x))(a)-1)
#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))
#define PTR_ALIGN(p, a)         ((typeof(p))ALIGN((unsigned long)(p), (a)))
#define IS_ALIGNED(x, a)                (((x) & ((typeof(x))(a) - 1)) == 0)

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr))

#define FIELD_SIZEOF(t, f) (sizeof(((t*)0)->f))
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))
#define roundup(x, y) ((((x) + ((y) - 1)) / (y)) * (y))
#define DIV_ROUND_CLOSEST(x, divisor)(                  \
    {                                                   \
      typeof(divisor) __divisor = divisor;              \
      (((x) + ((__divisor) / 2)) / (__divisor));        \
    }                                                   \
    )

/*
 * Multiplies an integer by a fraction, while avoiding unnecessary
 * overflow or loss of precision.
 */
#define mult_frac(x, numer, denom)(                     \
    {                                                   \
      typeof(x) quot = (x) / (denom);                   \
      typeof(x) rem  = (x) % (denom);                   \
      (quot * (numer)) + ((rem * (numer)) / (denom));   \
    }                                                   \
    )

#define _RET_IP_                (unsigned long)__builtin_return_address(0)
#define _THIS_IP_  ({ __label__ __here; __here: (unsigned long)&&__here; })

#ifdef CONFIG_LBDAF
# include <asm/div64.h>
# define sector_div(a, b) do_div(a, b)
#else
# define sector_div(n, b)(                      \
    {                                           \
      int _res;                                 \
      _res = (n) % (b);                         \
      (n) /= (b);                               \
      _res;                                     \
    }                                           \
    )
#endif

/**
 * upper_32_bits - return bits 32-63 of a number
 * @n: the number we're accessing
 *
 * A basic shift-right of a 64- or 32-bit quantity.  Use this to suppress
 * the "right shift count >= width of type" warning when that quantity is
 * 32-bits.
 */
#define upper_32_bits(n) ((u32)(((n) >> 16) >> 16))

/**
 * lower_32_bits - return bits 0-31 of a number
 * @n: the number we're accessing
 */
#define lower_32_bits(n) ((u32)(n))

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

struct sysinfo;
extern int do_sysinfo(struct sysinfo *info);

#endif // __KERNEL__

#ifndef __EXPORTED_HEADERS__
#ifndef __KERNEL__
#warning Attempt to use kernel headers from user space, see http://kernelnewbies.org/KernelHeaders
#endif /* __KERNEL__ */
#endif /* __EXPORTED_HEADERS__ */

#define SI_LOAD_SHIFT   16
struct sysinfo {
  long uptime;                    /* Seconds since boot */
  unsigned long loads[3];         /* 1, 5, and 15 minute load averages */
  unsigned long totalram;         /* Total usable main memory size */
  unsigned long freeram;          /* Available memory size */
  unsigned long sharedram;        /* Amount of shared memory */
  unsigned long bufferram;        /* Memory used by buffers */
  unsigned long totalswap;        /* Total swap space size */
  unsigned long freeswap;         /* swap space still available */
  unsigned short procs;           /* Number of current processes */
  unsigned short pad;             /* explicit padding for m68k */
  unsigned long totalhigh;        /* Total high memory size */
  unsigned long freehigh;         /* Available high memory size */
  unsigned int mem_unit;          /* Memory unit size in bytes */
  char _f[20-2*sizeof(long)-sizeof(int)]; /* Padding: libc5 uses this.. */
};

/* Force a compilation error if condition is true */
#define BUILD_BUG_ON(condition) ((void)BUILD_BUG_ON_ZERO(condition))

/* Force a compilation error if condition is constant and true */
#define MAYBE_BUILD_BUG_ON(cond) ((void)sizeof(char[1 - 2 * !!(cond)]))

/* Force a compilation error if condition is true, but also produce a
   result (of value 0 and type size_t), so the expression can be used
   e.g. in a structure initializer (or where-ever else comma expressions
   aren't permitted). */
#define BUILD_BUG_ON_ZERO(e) (sizeof(struct { int:-!!(e); }))
#define BUILD_BUG_ON_NULL(e) ((void *)sizeof(struct { int:-!!(e); }))

/* Trap pasters of __FUNCTION__ at compile-time */
#define __FUNCTION__ (__func__)

/* This helps us to avoid #ifdef CONFIG_NUMA */
#ifdef CONFIG_NUMA
#define NUMA_BUILD 1
#else
#define NUMA_BUILD 0
#endif

/* Rebuild everything on CONFIG_FTRACE_MCOUNT_RECORD */
#ifdef CONFIG_FTRACE_MCOUNT_RECORD
# define REBUILD_DUE_TO_FTRACE_MCOUNT_RECORD
#endif


#endif  // _LINUX_KERNEL_H
