#ifndef __BOOT_TYPES_H__
#define __BOOT_TYPES_H__

#include "posix_types.h"

#define BITS_PER_LONG 32

#define DECLARE_BITMAP(name,bits) \
    unsigned long name[BITS_TO_LONGS(bits)]

#ifndef __ASSEMBLY__

#ifdef __CHECKER__
#define __bitwise__ __attribute__((bitwise))
#else
#define __bitwise__
#endif

/*
 * __xx is ok: it doesn't pollute the POSIX namespace. Use these in the
 * header files exported to user space
 */

typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;

#ifdef __GNUC__
__extension__ typedef __signed__ long long __s64;
__extension__ typedef unsigned long long __u64;
#else
typedef __signed__ long long __s64;
typedef unsigned long long __u64;
#endif  /* __GNUC */

#endif /* __ASSEMBLY__ */

#ifndef __ASSEMBLY__

typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

typedef signed long long s64;
typedef unsigned long long u64;

typedef __u32 __kernel_dev_t;

typedef __kernel_fd_set         fd_set;
typedef __kernel_dev_t          dev_t;
typedef __kernel_ino_t          ino_t;
typedef __kernel_mode_t         mode_t;
typedef __kernel_nlink_t        nlink_t;
typedef __kernel_off_t          off_t;
typedef __kernel_pid_t          pid_t;
typedef __kernel_daddr_t        daddr_t;
typedef __kernel_key_t          key_t;
typedef __kernel_suseconds_t    suseconds_t;
typedef __kernel_timer_t        timer_t;
typedef __kernel_clockid_t      clockid_t;
typedef __kernel_mqd_t          mqd_t;

typedef __kernel_uid32_t        uid_t;
typedef __kernel_gid32_t        gid_t;
typedef __kernel_uid16_t        uid16_t;
typedef __kernel_gid16_t        gid16_t;

typedef unsigned long           uintptr_t;

#if defined(__GNUC__)
typedef __kernel_loff_t         loff_t;
#endif

typedef __kernel_size_t         size_t;
typedef __kernel_ssize_t        ssize_t;
typedef __kernel_ptrdiff_t      ptrdiff_t;
typedef __kernel_time_t         time_t;
typedef __kernel_clock_t        clock_t;
typedef __kernel_caddr_t        caddr_t;



#define S8_C(x)  x
#define U8_C(x)  x ## U
#define S16_C(x) x
#define U16_C(x) x ## U
#define S32_C(x) x
#define U32_C(x) x ## U
#define S64_C(x) x ## LL
#define U64_C(x) x ## ULL

#else /* __ASSEMBLY__ */

#define S8_C(x)  x
#define U8_C(x)  x
#define S16_C(x) x
#define U16_C(x) x
#define S32_C(x) x
#define U32_C(x) x
#define S64_C(x) x
#define U64_C(x) x

#endif /* __ASSEMBLY__ */


#ifndef __ASSEMBLY__

typedef int bool;
typedef unsigned __bitwise__ gfp_t;
typedef unsigned __bitwise__ fmode_t;

#ifdef CONFIG_PHYS_ADDR_T_64BIT
typedef u64 phys_addr_t;
#else
typedef u32 phys_addr_t;
#endif

typedef phys_addr_t resource_size_t;

typedef struct {
  volatile int counter;
} atomic_t;

typedef __u16 __bitwise__ __le16;
typedef __u16 __bitwise__ __be16;
typedef __u32 __bitwise__ __le32;
typedef __u32 __bitwise__ __be32;
typedef __u64 __bitwise__ __le64;
typedef __u64 __bitwise__ __be64;

#define min(x,y) ({ \
  typeof(x) _x = (x);     \
  typeof(y) _y = (y);     \
  (void) (&_x == &_y);    \
  _x < _y ? _x : _y; })

#define max(x,y) ({ \
    typeof(x) _x = (x);     \
    typeof(y) _y = (y);     \
    (void) (&_x == &_y);    \
    _x > _y ? _x : _y; })


#endif


#endif  // __BOOT_TYPES_H__
