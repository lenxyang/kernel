#ifndef __PROCESSOR_FLAGS_H__
#define __PROCESSOR_FLAGS_H__

/*
 * EFLAGS bits
 */
#define X86_EFLAGS_CF   0x00000001 /* Carry Flag */
#define X86_EFLAGS_PF   0x00000004 /* Parity Flag */
#define X86_EFLAGS_AF   0x00000010 /* Auxillary carry Flag */
#define X86_EFLAGS_ZF   0x00000040 /* Zero Flag */
#define X86_EFLAGS_SF   0x00000080 /* Sign Flag */
#define X86_EFLAGS_TF   0x00000100 /* Trap Flag */
#define X86_EFLAGS_IF   0x00000200 /* Interrupt Flag */
#define X86_EFLAGS_DF   0x00000400 /* Direction Flag */
#define X86_EFLAGS_OF   0x00000800 /* Overflow Flag */
#define X86_EFLAGS_IOPL 0x00003000 /* IOPL mask */
#define X86_EFLAGS_NT   0x00004000 /* Nested Task */
#define X86_EFLAGS_RF   0x00010000 /* Resume Flag */
#define X86_EFLAGS_VM   0x00020000 /* Virtual Mode */
#define X86_EFLAGS_AC   0x00040000 /* Alignment Check */
#define X86_EFLAGS_VIF  0x00080000 /* Virtual Interrupt Flag */
#define X86_EFLAGS_VIP  0x00100000 /* Virtual Interrupt Pending */
#define X86_EFLAGS_ID   0x00200000 /* CPUID detection flag */

/*
 * Basic CPU control in CR0
 */
#define X86_CR0_PE      0x00000001 /* Protection Enable */
#define X86_CR0_MP      0x00000002 /* Monitor Coprocessor */
#define X86_CR0_EM      0x00000004 /* Emulation */
#define X86_CR0_TS      0x00000008 /* Task Switched */
#define X86_CR0_ET      0x00000010 /* Extension Type */
#define X86_CR0_NE      0x00000020 /* Numeric Error */
#define X86_CR0_WP      0x00010000 /* Write Protect */
#define X86_CR0_AM      0x00040000 /* Alignment Mask */
#define X86_CR0_NW      0x20000000 /* Not Write-through */
#define X86_CR0_CD      0x40000000 /* Cache Disable */
#define X86_CR0_PG      0x80000000 /* Paging */


#endif  // __PROCESSOR_FLAGS_H__
