#include <linux/linkage.h>

#include <asm/segment.h>
#include <asm/page.h>
#include <asm/pgtable_types.h>
#include <asm/processor-flags.h>
#include <asm/setup.h>
#include <asm/e820.h>
#include <asm/asm-offsets.h>
#include <asm/cache.h>


/* Physical address */
#define pa(X) ((X) - __PAGE_OFFSET)
#define __VIDEO_BASE	0x0B8000

#define INIT_MAP_BEYOND_END       (128*1024)

	.extern __bss_start
	.extern __bss_stop
	.extern boot_params
	.extern start_kernel
	.global	startup_32
	.text
ENTRY(startup_32)
	cld
	lgdt	boot_gdt_descr - __PAGE_OFFSET
	
	movl	$(__BOOT_DS), %eax
	movl	%eax, %ds
	movl	%eax, %es
	movl	%eax, %fs
	movl	%eax, %gs
	movl	%eax, %ss

	/*
	* memoy
	*/
	xor	%eax, %eax
	movl	$pa(__bss_start), %edi
	movl	$pa(__bss_stop), %ecx
	subl	%edi, %ecx
	cld
	rep	stosb

	/*
``	* copy memory size
	*/
	movl	$pa(boot_params), %edi
	movl	$(PARAM_SIZE), %ecx
	cld
	rep	movsb	

	/**
	* 初始化页表
	*/
page_pde_offset = (__PAGE_OFFSET >> 20)
	/* pg0 保存了vmlinux结束的位置, 通过寄存%edi得到了vmlinux的大小
	* 同时 pg0 将作为pte的其实地址
	*/
	movl	$pa(__brk_base), %edi
	movl	$pa(swapper_pg_dir), %edx
	movl	$PTE_IDENT_ATTR, %eax /* attributes */

10:
	/* edi 为物理内存，+0x007 编程带有属性的pde item*/
	leal	0x007(%edi), %ecx
	movl	%ecx, (%edx)
	/*对应另外一个PDE表，他的地址在0xC00处，在后面它将建立完整的PDE*/
	movl	%ecx, page_pde_offset(%edx)
	addl	$4,%edx
	movl	$1024, %ecx
11:
	stosl
	addl	$0x1000, %eax /* 每次初始化 4K */
	loop	11b

	/*
	* 当大于当前128 * 1024 个表项时，停止初始化
	* 此处代码初始化
	* 也就是说初始化的表象需要覆盖整个vmlinux加载的地址
	* 及PTE所在的地址。覆盖完毕后结束初始化
	*/
	movl	$pa(_end) + (INIT_MAP_BEYOND_END+PTE_IDENT_ATTR), %ebp
	cmpl	%ebp, %eax
	jb	10b

	/**
	* edi 表示的 pgd 对应的地址
	* 下面的语句为 pgd 新增一项 
	*/
	addl	$__PAGE_OFFSET, %edi
	movl 	%edi, pa(_brk_end)
	
	/* max_pfn_mapped 完成映射的pf的个数 */
	shrl 	$12, %eax
	movl 	%eax, pa(max_pfn_mapped)

	/* Do early initialization of the fixmap area */
	movl 	$pa(swapper_pg_fixmap)+PDE_IDENT_ATTR,%eax
	movl 	%eax,pa(swapper_pg_dir+0xffc)
		
	/* 开启paging */
	movl	$(swapper_pg_dir - __PAGE_OFFSET), %eax
	movl	%eax, %cr3
	movl	%cr0, %eax
	orl	$X86_CR0_PG, %eax
	movl	%eax, %cr0

	
	ljmp	$(__BOOT_CS), $1f
1:

	movl	$kernel_stack_top, %esp
	call	__clean_screen
	movl	$(msg), %esi
	movl	$0, %eax
	call	__puts

	call	start_kernel


__clean_screen:
	push	%eax
	push	%ecx
	push	%edi
	xor	%eax, %eax
	mov	$(80 * 25), %ecx
	movl	$__VIDEO_BASE, %edi
	rep	stosw
	pop	%edi
	pop	%ecx
	pop	%eax
	ret

/*
* eax: position
* esi: string
*/
__puts:
	pusha
	movl	$__VIDEO_BASE, %edi
	addl	%eax, %edi
	xor	%eax, %eax
	mov	$0xf, %ah
.putc:
	lodsb
	cmp	$0, %al
	jz	.fin
	movw	%ax, (%edi)
	addl	$2, %edi
	jmp	.putc
.fin:
	popa
	ret
	
	.data
msg:
	.asciz	"We are in protected model, Now!"

/*
 * BSS section
 */
__PAGE_ALIGNED_BSS
        .align PAGE_SIZE_asm
ENTRY(swapper_pg_dir)
        .fill 1024,4,0
swapper_pg_fixmap:
        .fill 1024,4,0
#ifdef CONFIG_X86_TRAMPOLINE
ENTRY(trampoline_pg_dir)
        .fill 1024,4,0
#endif
ENTRY(empty_zero_page)
        .fill 4096,1,0
	
ENTRY(kernel_stack)
	.fill	4096,1,0
ENTRY(kernel_stack_top)

/*
 * only used by the lidt and lgdt instructions. They are not
 * like usual segment descriptors - they consist of a 16-bit
 * segment size, and 32-bit linear address value:
 */

.globl boot_gdt_descr
.globl idt_descr

	ALIGN
# early boot GDT descriptor (must use 1:1 address mapping)
	.word 0                         # 32 bit align gdt_desc.address
boot_gdt_descr:
	.word __BOOT_DS+7
	.long boot_gdt - __PAGE_OFFSET
	
	.word 0                         # 32-bit align idt_desc.address
idt_descr:
	.word IDT_ENTRIES*8-1           # idt contains 256 entries
	.long idt_table

# boot GDT descriptor (later on used by CPU#0):
	.word 0                         # 32 bit align gdt_desc.address
ENTRY(early_gdt_descr)
	.word GDT_ENTRIES*8-1
	.long per_cpu__gdt_page         /* Overwritten for secondary CPUs */

/*
 * The boot_gdt must mirror the equivalent in setup.S and is
 * used only for booting.
 */
	.align L1_CACHE_BYTES
ENTRY(boot_gdt)
	.fill GDT_ENTRY_BOOT_CS,8,0
	.quad 0x00cf9a000000ffff        /* kernel 4GB code at 0x00000000 */
	.quad 0x00cf92000000ffff        /* kernel 4GB data at 0x00000000 */



