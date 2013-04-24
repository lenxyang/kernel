#include "list.h"
#include "mm_types.h"
#include "highmem.h"
#include "spinlock.h"
#include "kernel.h"


struct page_address_slot {
  struct list_head lh;
  spinlock_t lock;
};

#define PA_HASH_ORDER 7
static struct page_address_slot page_address_htable[(1 << PA_HASH_ORDER)];

struct page_address_map {
  struct page* page;
  void *virtual;
  struct list_head list;
};

static struct list_head page_address_pool;
static struct page_address_map page_address_maps[LAST_PKMAP];

void page_address_init(void) {
  int i;
  for (i = 0; i < ARRAY_SIZE(page_address_maps); ++i) {
    list_add(&page_address_maps[i].list, &page_address_pool);
  }

  for (i = 0; i < ARRAY_SIZE(page_address_htable); ++i) {
    INIT_LIST_HEAD(&page_address_htable[i].lh);
  }
}

