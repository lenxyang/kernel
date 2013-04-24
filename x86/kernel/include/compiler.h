#ifndef __COMPILER_H__
#define __COMPILER_H__

#ifdef  __CHECKER__
#define __user         __attribute__((noderef, address_space(1)))
#define __kernel
#define __iomem        __attribute__((noderef, address_space(2)))
#define __force        __attribute__((force))
#else
#define __user
#define __kernel
#define __iomem
#define __force
#endif

#define noinline __attribute__((noinline))

#define __must_check


/*
 * Tell gcc if a function is cold. The compiler will assume any path
 * directly leading to the call is unlikely.
 */
#ifndef __cold
#define __cold
#endif


#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

#ifndef __section
#define __section(S) __attribute__ ((__section__(#S)))
#endif

/* Optimization barrier */
#ifndef barrier
#define barrier() __memory_barrier()
#endif

#define __pure                          __attribute__((pure))
#define __aligned(x)                    __attribute__((aligned(x)))
#define __printf(a,b)                   __attribute__((format(printf,a,b)))
#define  noinline                       __attribute__((noinline))
#define __attribute_const__             __attribute__((__const__))
#define __maybe_unused                  __attribute__((unused))



#endif  /* __COMPILER_H__ */
