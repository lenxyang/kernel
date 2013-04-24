	#include "init.h"
	#include "segment.h"

#define INIT_MAP_BEYOND_END       (128*1024)

	.extern kernel_main
	.global	startup_32
	.text
startup_32:
	cld
	lgdt	boot_gdt_descr - __PAGE_OFFSET
	
	movl	$(__BOOT_DS), %eax
	movl	%eax, %ds
	movl	%eax, %es
	movl	%eax, %fs
	movl	%eax, %gs
	movl	%eax, %ss

	/* we are in pm model now */
	/*
	call	__clean_screen - __PAGE_OFFSET
	movl	$(msg - __PAGE_OFFSET), %esi
	movl	$0, %eax
	call	__puts - __PAGE_OFFSET
	*/

page_pde_offset = (__PAGE_OFFSET >> 20)
	/* pg0 保存了vmlinux结束的位置, 通过寄存%edi得到了vmlinux的大小
	* 同时 pg0 将作为pte的其实地址
	*/
	movl	$(pg0	- __PAGE_OFFSET), %edi
	movl	$(swapper_pg_dir - __PAGE_OFFSET), %edx
	movl	$0x007, %eax /* attributes */

10:
	/* edi 为物理内存，+0x007 编程带有属性的pde item*/
	leal	0x007(%edi), %ecx
	movl	%ecx, (%edx)
	/*对应另外一个PDE表，他的地址在0xC00处，在后面它将建立完整的PDE*/
	movl	%ecx, page_pde_offset(%edx)
	addl	$4,%edx

	/* pte 1024 项 */
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
	* 128 * 1024 表示， pg0 处包含 128个 pde项
	*/
	leal	(INIT_MAP_BEYOND_END+0x007)(%edi), %ebp
	cmpl	%ebp, %eax
	jb	10b
	
	/* 将PDE的尾部保存在符号 init_pg_tables_end 变量当中 */
	movl	%edi, (init_pg_tables_end - __PAGE_OFFSET)

	
	/* 开启paging */
	movl	$(swapper_pg_dir - __PAGE_OFFSET), %eax
	movl	%eax, %cr3
	movl	%cr0, %eax
	orl	$0x80000000, %eax
	movl	%eax, %cr0

	call	__clean_screen
	movl	$(msg), %esi
	movl	$0, %eax
	call	__puts

	call	start_kernel
.loop:
	jmp	.loop

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

boot_gdt_table:	
        .quad 	0x0000000000000000
        .quad 	0x00cf9a000000ffff
        .quad 	0x00cf92000000ffff
	.quad	0x0000000000000000
boot_gdt_descr:
	.word 	boot_gdt_descr - boot_gdt_table
	.long 	boot_gdt_table - __PAGE_OFFSET
	.word	0
	.long	0

	.section ".bss"
ENTRY(swapper_pg_dir)
	.fill	1024,4,0
ENTRY(empty_zero_page)
	.fill	4096,1,0
ENTRY(kernel_stack)
	.fill	4096,1,0
