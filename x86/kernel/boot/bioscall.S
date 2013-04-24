
	.code16gcc
	.text
	.globl  intcall
	.type   intcall, @function

intcall:
	pushal
	pushl	%ecx
	movl	%esp, %ebp

	/*
	* 设置中断位
	*/
	movb	%al, (int)
	
	
	movl	%edx, %esi

	movl	0x0(%esi), %edi
	movl	0x10(%esi), %ebx
	movl	0x14(%esi), %edx
	movl	0x18(%esi), %ecx
	movl	0x1C(%esi), %eax
	movw	0x20(%esi), %gs
	movw	0x22(%esi), %fs
	movw	0x24(%esi), %es
	movw	0x26(%esi), %ds
	movl	0x04(%esi), %esi

	.byte	0xcd
int:	.byte	0

	pushl	%edi
	movl	(%ebp), %edi
	cmp	$0, %edi
	jnz	.fill
	addl	$4, %esp
	jmp	.exit
.fill:
	movl	%esi, 0x04(%edi)
	movl	%ebx, 0x10(%edi)
	movl	%edx, 0x14(%edi)
	movl	%ecx, 0x18(%edi)
	movl	%eax, 0x1C(%edi)

	/* 设置eflags */
	pushfl
	movl	(%esp), %eax
	movl	%eax, 0x28(%edi)
	addl	$4, %esp
	
	/* 设置 edi */
	popl	%eax
	movl	%eax, 0x0(%edi)

	
.exit:
	popl	%ecx
	popal
	retl
	.size   intcall, .-intcall
	
