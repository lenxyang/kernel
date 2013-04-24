#ifndef __PROTO_H__
#define __PROTO_H__
/*
 * This looks more complex than it should be. But we need to
 * get the type for the ~ right in round_down (it needs to be
 * as wide as the result!), and we want to evaluate the macro
 * arguments just once each.
 */
#define __round_mask(x,y) ((__typeof__(x))((y)-1))
#define round_up(x,y) ((((x)-1) | __round_mask(x,y))+1)
#define round_down(x,y) ((x) & ~__round_mask(x,y))


#endif  // __PROTO_H__
