#ifndef __LIST_H__
#define __LIST_H__

#include "kernel.h"

struct list_head {
  struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name)                         \
  struct list_head name = LIST_HEAD_INIT(name);

static inline void INIT_LIST_HEAD(struct list_head* list) {
  list->next = list;
  list->prev = list;
}

static inline void __list_add(struct list_head *new,
                              struct list_head *prev,
                              struct list_head *next) {
  next->prev = new;
  new->prev = prev;
  
  prev->next = new;
  new->next = next;
}

static inline void list_add(struct list_head* new, struct list_head *head) {
  __list_add(new, head, head->next);
}

static inline void __list_add_tail(struct list_head *prev, struct list_head *next) {
  next->prev = prev;
  prev->next = next;
}

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
  __list_add(new, head->prev, head);
}


/**
 * list_entry - get the struct for this entry
 * @ptr:        the &struct list_head pointer.
 * @type:       the type of the struct this is embedded in.
 * @member:     the name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member) \
  container_of(ptr, type, member)

/**
 * list_first_entry - get the first element from a list
 * @ptr:        the list head to take the element from.
 * @type:       the type of the struct this is embedded in.
 * @member:     the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_first_entry(ptr, type, member) \
  list_entry((ptr)->next, type, member)

/**
 * list_for_each        -       iterate over a list
 * @pos:        the &struct list_head to use as a loop cursor.
 * @head:       the head for your list.
 */
#define list_for_each(pos, head) \
    for (pos = (head)->next; prefetch(pos->next), pos != (head); \
         pos = pos->next)

      /**
       * __list_for_each      -       iterate over a list
       * @pos:        the &struct list_head to use as a loop cursor.
       * @head:       the head for your list.
       *
       * This variant differs from list_for_each() in that it's the
       * simplest possible list iteration code, no prefetching is done.
       * Use this for code that knows the list to be very short (empty
       * or 1 entry) most of the time.
       */
#define __list_for_each(pos, head) \
      for (pos = (head)->next; pos != (head); pos = pos->next)

      /**
       * list_for_each_prev   -       iterate over a list backwards
       * @pos:        the &struct list_head to use as a loop cursor.
       * @head:       the head for your list.
       */
#define list_for_each_prev(pos, head) \
      for (pos = (head)->prev; prefetch(pos->prev), pos != (head); \
           pos = pos->prev)


/**
 * list_for_each_entry  -       iterate over list of given type
 * @pos:        the type * to use as a loop cursor.
 * @head:       the head for your list.
 * @member:     the name of the list_struct within the struct.
 */
#define list_for_each_entry(pos, head, member)                          \
    for (pos = list_entry((head)->next, typeof(*pos), member);      \
         prefetch(pos->member.next), &pos->member != (head);        \
         pos = list_entry(pos->member.next, typeof(*pos), member))

#endif  // __LIST_H__
