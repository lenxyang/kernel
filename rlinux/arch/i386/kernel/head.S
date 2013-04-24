
	
#include <asm/cache.h>
#include <asm/segment.h>
#include <asm/linkage.h>
#include <asm/page.h>
#include <asm/thread_info.h>
#include <linux/linkage.h>
#include <linux/threads.h>

	.text
ENTRY(startup_32)
	cld
	lgdt	boot_gdt_descr - __PAGE_OFFSET
	movl	$(__BOOT_DS), %eax
	movl	%eax, %ds
	movl	%eax, %es
	movl	%eax, %fs
	movl	%eax, %gs

	movl %cr0,%eax
	orl $0x80000000,%eax
	movl %eax,%cr0          /* ..and set paging (PG) bit */
	ljmp $__BOOT_CS,$1f
1:
	/* Set up the stack pointer */
	lss stack_start,%esp

	/*
	 * BSS section
	 */
	.section ".bss.page_aligned","w"
ENTRY(swapper_pg_dir)
        .fill 1024,4,0
ENTRY(empty_zero_page)
	.fill 4096,1,0

/*
 * Real beginning of normal "text" segment
 */
ENTRY(stext)
ENTRY(_stext)
	
	
	.data
ENTRY(stack_start)
        /* .long init_thread_union+THREAD_SIZE */
	.long __BOOT_DS

ready:	.byte 0

/*
* The IDT and GDT 'descriptors' are a strange 48-bit object
* only used by the lidt and lgdt instructions. They are not
* like usual segment descriptors - they consist of a 16-bit
* segment size, and 32-bit linear address value:
*/

	.globl boot_gdt_descr
	.globl idt_descr
	.globl cpu_gdt_descr
        ALIGN
# early boot GDT descriptor (must use 1:1 address mapping)
        .word 0                         # 32 bit align gdt_desc.address
boot_gdt_descr:
	.word __BOOT_DS+7
	.long boot_gdt_table - __PAGE_OFFSET
	
	.word 0                         # 32-bit align idt_desc.address
idt_descr:
	.word IDT_ENTRIES*8-1           # idt contains 256 entries
	/* .long idt_table*/
	
	# boot GDT descriptor (later on used by CPU#0):
	.word 0                         # 32 bit align gdt_desc.address
cpu_gdt_descr:
	.word GDT_ENTRIES*8-1
	.long cpu_gdt_table
	
	.fill NR_CPUS-1,8,0             # space for the other GDT descriptors
	

	/*
	 * The boot_gdt_table must mirror the equivalent in setup.S and is
	 * used only for booting.
	 */
        .align L1_CACHE_BYTES
ENTRY(boot_gdt_table)
        .fill GDT_ENTRY_BOOT_CS,8,0
        .quad 0x00cf9a000000ffff        /* kernel 4GB code at 0x00000000 */
        .quad 0x00cf92000000ffff        /* kernel 4GB data at 0x00000000 */

	/*
	 * The Global Descriptor Table contains 28 quadwords, per-CPU.
	 */
        /*.align PAGE_SIZE_asm*/
	.align 4096
ENTRY(cpu_gdt_table)
	.quad 0x0000000000000000        /* NULL descriptor */
	.quad 0x0000000000000000        /* 0x0b reserved */
	.quad 0x0000000000000000        /* 0x13 reserved */
	.quad 0x0000000000000000        /* 0x1b reserved */
	.quad 0x0000000000000000        /* 0x20 unused */
	.quad 0x0000000000000000        /* 0x28 unused */
	.quad 0x0000000000000000        /* 0x33 TLS entry 1 */
	.quad 0x0000000000000000        /* 0x3b TLS entry 2 */
	.quad 0x0000000000000000        /* 0x43 TLS entry 3 */
	.quad 0x0000000000000000        /* 0x4b reserved */
	.quad 0x0000000000000000        /* 0x53 reserved */
	.quad 0x0000000000000000        /* 0x5b reserved */
	
	.quad 0x00cf9a000000ffff        /* 0x60 kernel 4GB code at 0x00000000 */
	.quad 0x00cf92000000ffff        /* 0x68 kernel 4GB data at 0x00000000 */
	.quad 0x00cffa000000ffff        /* 0x73 user 4GB code at 0x00000000 */
	.quad 0x00cff2000000ffff        /* 0x7b user 4GB data at 0x00000000 */
	
	.quad 0x0000000000000000        /* 0x80 TSS descriptor */
	.quad 0x0000000000000000        /* 0x88 LDT descriptor */
	
	/* Segments used for calling PnP BIOS */
	.quad 0x00c09a0000000000        /* 0x90 32-bit code */
	.quad 0x00809a0000000000        /* 0x98 16-bit code */
	.quad 0x0080920000000000        /* 0xa0 16-bit data */
	.quad 0x0080920000000000        /* 0xa8 16-bit data */
	.quad 0x0080920000000000        /* 0xb0 16-bit data */
	/*
	* The APM segments have byte granularity and their bases
	* and limits are set at run time.
	*/
	.quad 0x00409a0000000000        /* 0xb8 APM CS    code */
	.quad 0x00009a0000000000        /* 0xc0 APM CS 16 code (16 bit) */
	.quad 0x0040920000000000        /* 0xc8 APM DS    data */
	
	.quad 0x0000000000000000        /* 0xd0 - unused */
	.quad 0x0000000000000000        /* 0xd8 - unused */
	.quad 0x0000000000000000        /* 0xe0 - unused */
	.quad 0x0000000000000000        /* 0xe8 - unused */
	.quad 0x0000000000000000        /* 0xf0 - unused */
	.quad 0x0000000000000000        /* 0xf8 - GDT entry 31: double-fault TSS */
	
