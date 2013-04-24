#ifndef __IO_H__
#define __IO_H__

#include "compiler.h"
#include "pgtable.h"
#include "list.h"

#define build_mmio_read(name, size, type, reg, barrier)                 \
  static inline type name(const volatile void __iomem *addr)            \
  { type ret; asm volatile("mov" size " %1,%0":reg (ret)                \
                           :"m" (*(volatile type __force *)addr) barrier); \
    return ret;                                                         \
  }

#define build_mmio_write(name, size, type, reg, barrier)                \
  static inline void name(type val, volatile void __iomem *addr)        \
  { asm volatile("mov" size " %0,%1": :reg (val),                       \
                 "m" (*(volatile type __force *)addr) barrier);         \
  }

build_mmio_read(readb, "b", unsigned char, "=q", :"memory")
build_mmio_read(readw, "w", unsigned short, "=r", :"memory")
build_mmio_read(readl, "l", unsigned int, "=r", :"memory")

build_mmio_read(__readb, "b", unsigned char, "=q", )
build_mmio_read(__readw, "w", unsigned short, "=r", )
build_mmio_read(__readl, "l", unsigned int, "=r", )

build_mmio_write(writeb, "b", unsigned char, "q", :"memory")
build_mmio_write(writew, "w", unsigned short, "r", :"memory")
build_mmio_write(writel, "l", unsigned int, "r", :"memory")

build_mmio_write(__writeb, "b", unsigned char, "q", )
build_mmio_write(__writew, "w", unsigned short, "r", )
build_mmio_write(__writel, "l", unsigned int, "r", )

extern void native_io_delay(void);

// extern int io_delay_type;
// extern void io_delay_init(void);

static inline void slow_down_io(void) {
  native_io_delay();
#ifdef REALLY_SLOW_IO
  native_io_delay();
  native_io_delay();
  native_io_delay();
#endif
}


#define __BUILDIO(bwl, bw, type)                                \
  static inline void out##bwl(unsigned type value, int port)    \
  {                                                             \
    out##bwl##_local(value, port);                              \
  }                                                             \
                                                                \
  static inline unsigned type in##bwl(int port)                 \
  {                                                             \
    return in##bwl##_local(port);                               \
  }

#define BUILDIO(bwl, bw, type)                                          \
  static inline void out##bwl##_local(unsigned type value, int port)    \
  {                                                                     \
    asm volatile("out" #bwl " %" #bw "0, %w1"                           \
                 : : "a"(value), "Nd"(port));                           \
  }                                                                     \
                                                                        \
  static inline unsigned type in##bwl##_local(int port)                 \
  {                                                                     \
    unsigned type value;                                                \
    asm volatile("in" #bwl " %w1, %" #bw "0"                            \
                 : "=a"(value) : "Nd"(port));                           \
    return value;                                                       \
  }                                                                     \
                                                                        \
  static inline void out##bwl##_local_p(unsigned type value, int port)  \
  {                                                                     \
    out##bwl##_local(value, port);                                      \
        slow_down_io();                                                 \
  }                                                                     \
                                                                        \
  static inline unsigned type in##bwl##_local_p(int port)               \
  {                                                                     \
    unsigned type value = in##bwl##_local(port);                        \
        slow_down_io();                                                 \
        return value;                                                   \
  }                                                                     \
                                                                        \
  __BUILDIO(bwl, bw, type)                                              \
                                                                        \
  static inline void out##bwl##_p(unsigned type value, int port)        \
  {                                                                     \
    out##bwl(value, port);                                              \
        slow_down_io();                                                 \
  }                                                                     \
  static inline unsigned type in##bwl##_p(int port)                     \
  {                                                                     \
    unsigned type value = in##bwl(port);                                \
        slow_down_io();                                                 \
        return value;                                                   \
  }                                                                     \
                                                                        \
  static inline void outs##bwl(int port, const void *addr, unsigned long count) \
  {                                                                     \
    asm volatile("rep; outs" #bwl                                       \
                 : "+S"(addr), "+c"(count) : "d"(port));                \
  }                                                                     \
                                                                        \
  static inline void ins##bwl(int port, void *addr, unsigned long count) \
  {                                                                     \
    asm volatile("rep; ins" #bwl                                        \
                 : "+D"(addr), "+c"(count) : "d"(port));                \
  }

BUILDIO(b, b, char)
BUILDIO(w, w, short)
BUILDIO(l, , int)

#define __ISA_IO_base ((char __iomem *)(PAGE_OFFSET))

#ifndef __ASSEMBLY__

/**
 *      virt_to_phys    -       map virtual addresses to physical
 *      @address: address to remap
 *
 *      The returned physical address is the physical (CPU) mapping for
 *      the memory address given. It is only valid to use this function on
 *      addresses directly mapped or allocated via kmalloc.
 *
 *      This function does not give bus mappings for DMA transfers. In
 *      almost all conceivable cases a device driver should not be using
 *      this function
 */

 static inline phys_addr_t virt_to_phys(volatile void *address)
{
  return __pa(address);
}

/**
 *      phys_to_virt    -       map physical address to virtual
 *      @address: address to remap
 *
 *      The returned virtual address is a current CPU mapping for
 *      the memory address given. It is only valid to use this function on
 *      addresses that have a kernel mapping
 *
 *      This function does not handle bus mappings for DMA transfers. In
 *      almost all conceivable cases a device driver should not be using
 *      this function
 */

static inline void *phys_to_virt(phys_addr_t address)
{
  return __va(address);
}


#endif  // __ASSEMBLY__

#endif  // __IO_H__
