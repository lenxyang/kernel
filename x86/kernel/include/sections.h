#ifndef __SECTIONS_H__
#define __SECTIONS_H__

/* function descriptor handling (if any).  Override
 * in asm/sections.h */
#ifndef dereference_function_descriptor
#define dereference_function_descriptor(p) (p)
#endif

#endif  // __SECTIONS_H__
