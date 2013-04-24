#ifndef __LIST_H__
#define __LIST_H__

#define LIST_POSITION1 ((void*)0x00100100)
#define LIST_POSITION2 ((void*)0x00200200)

struct list_head {
  struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) {&(name), &(name)}

#define LIST_HEAD(name) \
  struct list_head name = LIST_HEAD_INIT(name);

#define INIT_LIST_HEAD(ptr) do {                \
    (ptr)->next = (ptr); (ptr)->prev = (ptr);   \
  } while (0)

static inline void __list_add(struct list_head *new,
                              struct list_head *prev,
                              struct list_head *next) {
  next->prev = new;
  new->next = next;
  new->prev = prev;
  prev->next = new;
}

static inline void list_add(struct list_head* new, struct list_head *head) {
  __list_add(new, head, head->next);
}

#endif  // __LIST_H__
