#ifndef __POSIX_TYPES_H__
#define __POSIX_TYPES_H__

#undef __NFDBITS
#define __NFDBITS       (8 * sizeof(unsigned long))

#undef __FD_SETSIZE
#define __FD_SETSIZE    1024

#undef __FDSET_LONGS
#define __FDSET_LONGS   (__FD_SETSIZE/__NFDBITS)

#undef __FDELT
#define __FDELT(d)      ((d) / __NFDBITS)

#undef __FDMASK
#define __FDMASK(d)     (1UL << ((d) % __NFDBITS))

#ifndef __ASSEMBLY__

typedef struct {
  unsigned long fds_bits [__FDSET_LONGS];
} __kernel_fd_set;

/* Type of a signal handler.  */
typedef void (*__kernel_sighandler_t)(int);

/* Type of a SYSV IPC key.  */
typedef int __kernel_key_t;
typedef int __kernel_mqd_t;

typedef unsigned long   __kernel_ino_t;

typedef unsigned int    __kernel_mode_t;

typedef unsigned long   __kernel_nlink_t;

typedef int             __kernel_pid_t;

typedef int             __kernel_ipc_pid_t;

typedef unsigned int    __kernel_uid_t;
typedef unsigned int    __kernel_gid_t;

typedef long            __kernel_suseconds_t;

typedef int             __kernel_daddr_t;

typedef __kernel_uid_t  __kernel_uid32_t;
typedef __kernel_gid_t  __kernel_gid32_t;

#if __BITS_PER_LONG != 64
typedef unsigned int    __kernel_size_t;
typedef int             __kernel_ssize_t;
typedef int             __kernel_ptrdiff_t;
#else
typedef unsigned long   __kernel_size_t;
typedef long            __kernel_ssize_t;
typedef long            __kernel_ptrdiff_t;
#endif

typedef long            __kernel_off_t;
typedef long long       __kernel_loff_t;
typedef long            __kernel_time_t;
typedef long            __kernel_clock_t;
typedef int             __kernel_timer_t;
typedef int             __kernel_clockid_t;
typedef char *          __kernel_caddr_t;
typedef unsigned short  __kernel_uid16_t;
typedef unsigned short  __kernel_gid16_t;

typedef struct {
  int     val[2];
} __kernel_fsid_t;

#endif  // __ASSEMBLY__

#endif  // __POSIX_TYPES_H__
