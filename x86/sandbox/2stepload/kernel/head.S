

	.equ	__VIDEO_BASE, 0x0B8000

	.global	startup_32
	.code32
	.text
startup_32:
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
