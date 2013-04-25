#include <linux/mm.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/hash.h>
#include <linux/highmem.h>

#ifdef CONFIG_HIGHMEM

#define PA_HASH_ORDER   7

struct page_address_map {
  struct page* page;
  void *virtual;
  struct list_head list;
};

static struct list_head page_address_pool;
static spinlock_t pool_lock;

static struct page_address_slot {
  struct list_head lh;
  spinlock_t lock;
} page_address_htable[1 << PA_HASH_ORDER];


/* LAST_PKMAP 的值是 1024 */
static struct page_address_map page_address_maps[LAST_PKMAP];

void page_address_init(void) {
  int i;
  INIT_LIST_HEAD(&page_address_pool);
  for (i = 0; i < ARRAY_SIZE(page_address_maps); i++) {
    list_add(&page_address_maps[i].list, &page_address_pool);
  }

  for (i = 0; i < ARRAY_SIZE(page_address_htable); i++) {
    INIT_LIST_HEAD(&page_address_htable[i].lh);
    spin_lock_init(&page_address_htable[i].lock);
  }
  spin_lock_init(&pool_lock);
}

#endif /* CONFIG_HIGHMEM */
