#ifndef _LINUX_STRING_H_
#define _LINUX_STRING_H_

/* We don't want strings.h stuff being used by user stuff by accident */

#include "compiler.h"	/* for inline */
#include "types.h"	/* for size_t */
#include "stddef.h"	/* for NULL */
#include "stdarg.h"

extern char *strndup_user(const char __user *, long);
extern void *memdup_user(const void __user *, size_t);

/*
 * Include machine specific inline routines
 */

extern char * strcpy(char *,const char *);

extern char * strncpy(char *,const char *, __kernel_size_t);

size_t strlcpy(char *, const char *, size_t);

extern char * strcat(char *, const char *);

extern char * strncat(char *, const char *, __kernel_size_t);

extern size_t strlcat(char *, const char *, __kernel_size_t);

extern int strcmp(const char *,const char *);

extern int strncmp(const char *,const char *,__kernel_size_t);

extern int strnicmp(const char *, const char *, __kernel_size_t);

extern int strcasecmp(const char *s1, const char *s2);

extern int strncasecmp(const char *s1, const char *s2, size_t n);

extern char * strchr(const char *,int);

extern char * strnchr(const char *, size_t, int);

extern char * strrchr(const char *,int);

extern char * __must_check strstrip(char *);

extern char * strstr(const char *,const char *);

extern __kernel_size_t strlen(const char *);

extern __kernel_size_t strnlen(const char *,__kernel_size_t);

extern char * strpbrk(const char *,const char *);

extern char * strsep(char **,const char *);

extern __kernel_size_t strspn(const char *,const char *);

extern __kernel_size_t strcspn(const char *,const char *);



extern void * memset(void *,int,__kernel_size_t);

extern void * memcpy(void *,const void *,__kernel_size_t);

extern void * memmove(void *,const void *,__kernel_size_t);

extern void * memscan(void *,int,__kernel_size_t);

extern int memcmp(const void *,const void *,__kernel_size_t);

extern void * memchr(const void *,int,__kernel_size_t);


extern char *kstrdup(const char *s, gfp_t gfp);
extern char *kstrndup(const char *s, size_t len, gfp_t gfp);
extern void *kmemdup(const void *src, size_t len, gfp_t gfp);

extern char **argv_split(gfp_t gfp, const char *str, int *argcp);
extern void argv_free(char **argv);

extern bool sysfs_streq(const char *s1, const char *s2);

#ifdef CONFIG_BINARY_PRINTF
int vbin_printf(u32 *bin_buf, size_t size, const char *fmt, va_list args);
int bstr_printf(char *buf, size_t size, const char *fmt, const u32 *bin_buf);
int bprintf(u32 *bin_buf, size_t size, const char *fmt, ...) __printf(3, 4);
#endif

extern ssize_t memory_read_from_buffer(void *to, size_t count, loff_t *ppos,
                                       const void *from, size_t available);

/**
 * strstarts - does @str start with @prefix?
 * @str: string to examine
 * @prefix: prefix to look for.
 */
static inline bool strstarts(const char *str, const char *prefix)
{
	return strncmp(str, prefix, strlen(prefix)) == 0;
}

#endif /* _LINUX_STRING_H_ */
