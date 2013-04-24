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

	/*
	* 由于 idt 占用的空间太大，而绝大多数内容为0
	* 我们将将它放置在 0x9000 处
	*/
	mov	$0x20, %edi
	leal	IDT_ADDR(,%edi,8), %esi
	movl	%esi, %edi
	movl	$item,%esi
	movl	$4, %ecx
	rep	movsd

	lidt	idtptr

	mov	$0xff, %al
	outb 	%al, $PIC_MASTER_IMR
	outb	%al, $PIC_SLAVE_IMR
	call	delay
	/* 初始化 "主8259A" */
	movb	$0x11, %al
	outb	%al, $PIC_MASTER_CMD
	call	delay
	movb	$I8253_MASTER_INT, %al
	outb	%al, $PIC_MASTER_IMR /* first of interrupt */
	call	delay
	movb	$(1 << PIC_CASCADE_IR), %al
	outb	%al, $PIC_MASTER_IMR
	call	delay
	movb 	$(MASTER_ICW4_DEFAULT | PIC_ICW4_AEOI), %al
	outb	%al, $PIC_MASTER_IMR
	call	delay

	/* 初始化 "从8259A" */
	movb	$0x11, %al
	outb	%al, $PIC_SLAVE_CMD
	call	delay
	movb	$I8253_SLAVE_INT, %al
	outb	%al, $PIC_SLAVE_IMR
	call	delay
	movb	$PIC_CASCADE_IR, %al
	outb	%al, $PIC_SLAVE_IMR
	call	delay
	movb	$SLAVE_ICW4_DEFAULT, %al
	outb	%al, $PIC_SLAVE_IMR
	call	delay

	/*
	* 初始化 8253
	*/
	movb	$0x34, %al
	outb	%al, $PIT_MODE
	call	delay
	movl	$1193182, %eax
	outb	%al, $PIT_CH0
	call	delay
	movb	%ah, %al
	outb	%al, $PIT_CH0
	call	delay

	/* 打开时钟中断 */
	movb	$0xfe, %al
	outb	%al, $PIC_MASTER_IMR
	call	delay
	sti

1:
	jmp	1b

delay:
	push	%eax
	xor	%eax,%eax
	outb	%al, $0x80
	pop	%eax
	ret
/* 打印一个字符串*/
/* esi 为字符串地址 */
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
	xor	%eax, %eax
	mov	$(80 * 25), %ecx
	movl	$__VIDEO_BASE, %edi
	rep	stosw
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

item:	
	.word	interval
	.word	__CODE32_CS
	.word	0x8E00
	.word	0x0  /* 高16位地址，在此处可以保证为0*/
idtptr:

	.word	0x8 * IDT_VEC_ITEM
	.long	IDT_ADDR
	.word	0
err_msg:
	.ascii	"interrupt:"
num:	.ascii	"0"
	.byte	0
	/* 保存中断发生的次数 */	
.org	510
	.word 	0xAA55
