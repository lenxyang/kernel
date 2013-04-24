#include "consts.h"
	
	.code16
	.text

	.global _start
_start:
	mov 	%cs, %ax
	mov	%ax, %ds

	lgdt	gdtdesc

	/* 打开保护模式 */
	mov	%cr0, %eax
	or	$0x1, %eax
	mov	%eax, %cr0

	/* 进入保护模式 */
	.byte	0x66, 0xea
	.long	pm
	.word	__CODE32_CS

	.code32
pm:
	mov	$__DATA32_DS, %eax
	movl	%eax, %ds
	movl	%eax, %ss
	movl	%eax, %es
	lidt	idtptr

1:
	int 	$0x14
	mov	$0x1000000, %ecx
.loop:
	call	iodelay
	loop	.loop
	jmp	1b

iodelay:
	push	%eax
	xor	%eax,%eax
	outb	%al, $0x80
	pop	%eax
	ret
/* 打印一个字符串*/
/* esi 为字符串地址 */
/* eax 为初始位置 */
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
	
interval:
	call	__clean_screen
	movb	(num), %al
	inc	%al
	movb	%al, (num)
	xor	%eax, %eax
	movl	$err_msg, %esi
	call	__puts
	iret
gdt:
	.quad	0x0000000000000000 /* NULL selector */
	.quad	0x00df9a000000ffff
	.quad 	0x00df92000000ffff
gdtdesc:
	.word	gdtdesc - gdt
	.long	gdt
	.word	0
idt:
	.fill	20,8,0
	.word	interval
	.word	__CODE32_CS
	.word	0x8E00
	.word	0x0  /* 高16位地址，在此处可以保证为0*/
	.fill	10,8,0
idtptr:
	.word	idtptr - idt
	.long	idt
	.word	0
err_msg:
	.ascii	"soft_irq"
num:
	.byte	'0'
	.byte	0
	/* 保存中断发生的次数 */	
.org	510
	.word 	0xAA55
