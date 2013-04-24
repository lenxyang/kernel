
	.equ	__BOOT_CS, 0x8
	.equ	__BOOT_DS, 0x10
	.equ	__VIDEO_BASE, 0x0B8000

	.global	startup_32
	.code32
	.text
startup_32:
	cld
	movl	$__BOOT_DS, %eax
	movl	%eax, %ds
	movl	%eax, %es
	movl	%eax, %fs
	movl	%eax, %gs
	movl	%eax, %ss
	lgdt	boot_gdt_descr
	ljmp 	$__BOOT_CS,$1f

1:	
	call	__clean_screen
	movl	$msg, %esi
	movl	$0, %eax
	call	__puts

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
	.asciz	"We are in protected model"

boot_gdt_table:	
        .quad 	0x0000000000000000
        .quad 	0x00cf9a000000ffff
        .quad 	0x00cf92000000ffff
	.quad	0x0000000000000000
boot_gdt_descr:
	.word 	boot_gdt_descr - boot_gdt_table
	.long 	boot_gdt_table
	.word	0
	.long	0
