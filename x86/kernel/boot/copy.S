#include "linkage.h"
	
	.code16gcc
	.text

GLOBAL(memcpy)
	pushl	%edi
	pushl	%esi

	movl	%edx, %esi
	movl	%eax, %edi
	pushl	%ecx
	shrl	$2, %ecx
	rep	stosl
	popl	%ecx
	andl	$3, %ecx
	rep	stosb

	pop	%esi
	pop	%edi
	ret
ENDPROC(memcpy)

GLOBAL(memset)
	pushl	%edi

	movl	%eax, %edi
	movl	%edx, %eax
	imull 	$0x01010101, %eax
	pushl	%ecx
	shrl	$2, %ecx
	rep	stosl
	popl	%ecx
	andl	$3, %ecx
	rep	stosb

	movl	%edi, %eax
	popl	%edi
	ret
ENDPROC(memset)


GLOBAL(copy_from_fs)
	pushw   %ds
	pushw   %fs
	popw    %ds
	call    memcpy
	popw    %ds
	ret
ENDPROC(copy_from_fs)
	
GLOBAL(copy_to_fs)
	pushw   %es
	pushw   %fs
	popw    %es
	call    memcpy
	popw    %es
	ret
ENDPROC(copy_to_fs)
