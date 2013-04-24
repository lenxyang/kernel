#ifndef __SETUP_H__
#define __SETUP_H__

#define PFN_UP(x)   (((x) + PAGE_SIZE - 1) >> PAGE_SHIFT)
#define PFN_DOWN(x) ((x) >> PAGE_SHIFT)
#define PFN_PHYS(x) ((x) << PAGE_SHIFT)

#endif  // _SETUP_H__
