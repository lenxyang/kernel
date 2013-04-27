#include <linux/kernel.h>
#include <linux/pfn.h>
#include <linux/ioport.h>

#include <asm/io.h>

static DEFINE_RWLOCK(resource_lock);

/**
 * struct resource 定义在文件 linux/ioport.h 当中
 */
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

static struct resource* __request_resource(struct resource *root,
                                           struct resource *new) {
  resource_size_t start = new->start;
  resource_size_t end   = new->end;
  struct resource *tmp, **p;

  if (end < start)
    return root;
  if (start < root->start)
    return root;
  if (end > root->end)
    return root;
  p = &root->child;
  
  for (;;) {
    tmp = *p;
    if (!tmp || tmp->start > end) {
      new->sibling = tmp;
      *p = new;
      new->parent = root;
      return NULL;
    }

    p = &tmp->sibling;
    if (tmp->end < start)
      continue;
    return tmp;
  }
}

int request_resource(struct resource *root, struct resource *new) {
  struct resource *conflict;
  
  write_lock(&resource_lock);
  conflict = __request_resource(root, new);
  write_unlock(&resource_lock);

  return conflict ? -EBUSY : 0;
}
