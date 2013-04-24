
	.extern main
	
	.code16

	.section ".header", "a"
	.globl  hdr
hdr:
	.global _start
_start:
	.byte	0xeb
	.byte	start_of_setup	- 1f
1:
code32_start:
	.long	0x100000
	
	.section ".entrytext", "ax"
start_of_setup:
	movw    $__bss_start, %di
	movw    $_end+3, %cx
	xorl    %eax, %eax
	subw    %di, %cx
	shrw    $2, %cx
	rep; stosl

	calll	main

	
