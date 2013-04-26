#include <linux/kernel.h>
#include <linux/pfn.h>
#include <linux/ioport.h>

#include <asm/io.h>

struct resource ioport_resource = {
  .name   = "PCI IO",
  .start  = 0,
  .end    = IO_SPACE_LIMIT,
  .flags  = IORESOURCE_IO,
};
//EXPORT_SYMBOL(ioport_resource);

struct resource iomem_resource = {
  .name   = "PCI mem",
  .start  = 0,
  .end    = -1,
  .flags  = IORESOURCE_MEM,
};
// EXPORT_SYMBOL(iomem_resource);
