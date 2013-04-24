	
	.code16
	.text

.equ	__CODE32_CS,    0x08
.equ    __DATA32_DS,    0x10
.equ	__VIDEO_BASE,	0x0B8000

	.global _start
_start:
	mov 	%cs, %ax
	mov	%ax, %ds
	mov	%ax, %es
	mov	%ax, %ss
	mov	$0x7c00, %sp

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
	movl	%eax, %fs
	movl	%eax, %gs

	lidt	idtptr

	ud2
1:
	jmp	1b
	
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
	
invalid_op:
	call	__clean_screen
	xor	%eax,%eax
	movl	$err_msg, %esi
	call	__puts
.no:
	jmp	.no
	
	.align	16
gdt:
	.quad	0x0000000000000000 /* NULL selector */
	.quad	0x00df9a000000ffff
	.quad 	0x00df92000000ffff
	
gdtdesc:
	.word	gdtdesc - gdt
	.long	gdt
	.word	0
idt:
	.fill	6,8,0
	/* 此处为invalid_op的trap gate描述符 */
	.word	invalid_op
	.word	__CODE32_CS
	.word	0x8F00
	.word	0x0  /* 高16位地址，在此处可以保证为0*/
	.fill	23,8,0
idtptr:
	.word	idtptr - idt
	.long	idt
	.word	0
err_msg:
	.asciz	"Invalid operator"
	
.org	510
	.word 	0xAA55
