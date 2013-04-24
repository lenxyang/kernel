#ifndef __BUG_H__
#define __BUG_H__

extern void warn_slowpath_fmt(const char *file, const int line,
                                const char *fmt, ...) __attribute__((format(printf, 3, 4)));
extern void warn_slowpath_null(const char *file, const int line);

#define __WARN()                warn_slowpath_null(__FILE__, __LINE__)
#define __WARN_printf(arg...)   warn_slowpath_fmt(__FILE__, __LINE__, arg)

#define WARN_ON(condition) ({                                           \
      int __ret_warn_on = !!(condition);                                \
      if (unlikely(__ret_warn_on))                                      \
        __WARN();                                                       \
      unlikely(__ret_warn_on);                                          \
    })

#define WARN(condition, format...) ({                             \
      int __ret_warn_on = !!(condition);                          \
      if (unlikely(__ret_warn_on))                                \
        __WARN_printf(format);                                    \
      unlikely(__ret_warn_on);                                    \
    })

#define WARN_ON_ONCE(condition) ({                              \
      static int __warned;                                      \
      int __ret_warn_once = !!(condition);                      \
                                                                \
      if (unlikely(__ret_warn_once))                            \
        if (WARN_ON(!__warned))                                 \
          __warned = 1;                                         \
      unlikely(__ret_warn_once);                                \
    })

#define WARN_ONCE(condition, format...) ({                      \
      static int __warned;                                      \
      int __ret_warn_once = !!(condition);                      \
                                                                \
      if (unlikely(__ret_warn_once))                            \
        if (WARN(!__warned, format))                            \
          __warned = 1;                                         \
      unlikely(__ret_warn_once);                                \
    })

#define WARN_ON_RATELIMIT(condition, state)     \
    WARN_ON((condition) && __ratelimit(state))

#define BUG()                                                     \
  do {                                                            \
    asm volatile("ud2");                                          \
    for (;;) ;                                                    \
  } while (0)

#define BUG_ON(condition) do { if (unlikely(condition)) BUG(); } while(0)

#endif  // __BUG_H__
