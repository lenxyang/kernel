#ifndef __HWEIGHTS_H__
#define __HWEIGHTS_H__

#include "types.h"

unsigned int hweight32(unsigned int w);
unsigned int hweight16(unsigned int w);
unsigned int hweight8(unsigned int w);
unsigned long hweight64(__u64 w);

#endif  // __HWEIGHTS_H__
