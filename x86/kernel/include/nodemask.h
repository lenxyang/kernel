#ifndef __NODEMASK_H__
#define __NODEMASK_H__

#include "bitmap.h"
#include "numa.h"

typedef struct { DECLARE_BITMAP(bits, MAX_NUMNODES); } nodemask_t;

/*
 * Bitmasks that are kept for all the nodes.
 */
enum node_states {
  N_POSSIBLE,             /* The node could become online at some point */
  N_ONLINE,               /* The node is online */
  N_NORMAL_MEMORY,        /* The node has regular memory */
  N_HIGH_MEMORY,
  N_CPU,          /* The node has one or more cpus */
  NR_NODE_STATES
};

/*
 * The following particular system nodemasks and operations
 * on them manage all possible and online nodes.
 */

extern nodemask_t node_states[NR_NODE_STATES];


#define NODE_MASK_LAST_WORD BITMAP_LAST_WORD_MASK(MAX_NUMNODES)

#define NODE_MASK_ALL                                                   \
  ((nodemask_t) { {                                                     \
      [BITS_TO_LONGS(MAX_NUMNODES)-1] = NODE_MASK_LAST_WORD             \
          } })

#define NODE_MASK_NONE                                                  \
  ((nodemask_t) { {                                                     \
      [0 ... BITS_TO_LONGS(MAX_NUMNODES)-1] =  0UL                      \
            } })

#define for_each_node_state(node, __state) \
  for ( (node) = 0; (node) == 0; (node) = 1)

#define for_each_node(node)        for_each_node_state(node, N_POSSIBLE)
#define for_each_online_node(node) for_each_node_state(node, N_ONLINE)

#endif  // __NODEMASK_H__
