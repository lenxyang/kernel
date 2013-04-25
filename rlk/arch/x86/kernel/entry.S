#include <linux/linkage.h>


ENTRY(iret_exc)

#ifdef CONFIG_PARAVIRT
ENTRY(native_iret)
	iret
	.section __ex_table,"a"
	.align 4
	.long native_iret, iret_exc
	.previous
END(native_iret)

ENTRY(native_irq_enable_sysexit)
	sti
	sysexit
END(native_irq_enable_sysexit)
#endif
	
