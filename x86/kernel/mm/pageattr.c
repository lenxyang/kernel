#include "init.h"
#include "page.h"

static unsigned long direct_pages_count[PG_LEVEL_NUM];
void update_page_count(int level, unsigned long pages) {
  direct_pages_count[level] = pages;
}
