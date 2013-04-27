#include <linux/mm.h>

#include <asm/e820.h>
#include <asm/processor.h>
#include <asm/sections.h>

static unsigned long direct_pages_count[PG_LEVEL_NUM];

void update_page_count(int level, unsigned long pages) {
  spin_lock(&pgd_lock);
  direct_pages_count[level] += pages;
  spin_unlock(&pgd_lock);
}
