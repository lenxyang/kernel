#include "segment.h"
#include "processor-flags.h"

	.text
	.code16
	
.global	protected_mode_jump
protected_mode_jump:
	xor	%ecx, %ecx
	xor	%edi, %edi
	movw	$__BOOT_DS, %cx
	movw	$__BOOT_TSS, %di

	movl	%cr0, %edx
	orl	$X86_CR0_PE, %edx
	movl	%edx, %cr0

	.byte	0x66, 0xea
	.long	in_pm32
	.word	__BOOT_CS

	.code32
	.section ".text32", "ax"

.global in_pm32
in_pm32:
	mov	%ecx, %ds
	mov	%ecx, %es
	mov	%ecx, %fs
	mov	%ecx, %gs
	mov	%ecx, %ss
	
	xorl    %ecx, %ecx
	xorl    %edx, %edx
	xorl    %ebx, %ebx
	xorl    %ebp, %ebp
	xorl    %edi, %edi
	
	movl	$0x100000, %eax
	jmpl	*%eax
