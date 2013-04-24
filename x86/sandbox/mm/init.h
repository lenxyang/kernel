#ifndef __INIT_H__
#define __INIT_H__

#include "basictypes.h"

#ifndef __ASSEMBLY__
extern unsigned int swapper_pg_dir[1024];

extern unsigned int init_pg_tables_end;
#endif

#endif  /* ifndef __INIT_H__ */
