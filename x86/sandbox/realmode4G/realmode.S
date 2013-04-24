
	.code16
	.text

.equ	__CODE32_CS,    0x8
.equ    __DATA32_DS,    0x10
.equ    __DATA16_CS,    0x18
.equ    __DATA16_DS,    0x20

	.global _start
_start:
	mov 	%cs, %ax
	mov	%ax, %ds
	mov	%ax, %ss
	mov	$0x7c00, %sp

	lgdt	gdtptr

	/* 打开保护模式 */
	mov	%cr0, %eax
	or	$0x1, %eax
	mov	%eax, %cr0

	/* 进入保护模式 */
	.byte	0x66, 0xea
	.long	pm
	.word	0x8

	.code32
pm:
	mov	$__DATA32_DS, %eax
	mov	%eax, %ds
	mov	%eax, %ss
	mov	$__DATA16_DS, %eax
	mov	%eax, %fs
	
	
	/* 关闭保护模式 */
	mov	%cr0, %eax
	and	$0xFFFFFFFE, %eax
	mov	%eax, %cr0
	
	.byte 	0x66, 0xea
	.long	real
	.word	0
	
	.code16
real:
	mov	%cs, %ax
	mov	%ax, %ss
	mov	%ax, %ds
	mov	%ax, %es
	movl	$0x7c00, %esp

	/*
	将 $1234 吸入到 8M物理内存出
	而后通过bochs内置调试器验证
	*/
	movl	$1234, %eax

	movl	$0x100000, %edi
	movl	%eax, %fs:(%edi)

	.loop:
	jmp	.loop
gdt:
	.quad	0x0000000000000000 /* NULL selector */

	.quad	0x00bf9a000000ffff
	.quad 	0x00bf92000000ffff
	.quad	0x008f9a000000ffff
	.quad 	0x008f92000000ffff
gdtptr:
	.word	gdtptr - gdt
	.long	gdt
	.word	0

.org	510
	.word 	0xAA55
