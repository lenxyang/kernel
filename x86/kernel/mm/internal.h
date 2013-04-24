#ifndef __MM_INTERNAL_H__
#define __MM_INTERNAL_H__

/* Memory initialisation debug and verification */
enum mminit_level {
  MMINIT_WARNING,
  MMINIT_VERIFY,
  MMINIT_TRACE
};


static inline void mminit_dprintk(enum mminit_level level,
                                  const char *prefix, const char *fmt, ...) {
}


static inline void mminit_validate_memmodel_limits(unsigned long *start_pfn,
                                                   unsigned long *end_pfn);

#endif // __MM_INTERNAL_H__
