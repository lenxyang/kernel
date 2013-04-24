#include <asm/segment.h>
#include <linux/linkage.h>
#include <asm/page.h>
	
	.code16
	.text

.equ	ELFDATA_SEG,  0x2000
.equ	ELFDATA_OFF,  0x0000
.equ	ELFDATA_ADDR, 0X20000

.equ	__CODE32_CS,    0x8
.equ    __DATA16_DS,    0x10

/* system 加载地址 */	
ENTRY(_start)	
	/*
	init segment regs
	*/
	movw	%cs, %ax
	movw	%ax, %ds
	movw	%ax, %ss

	movl	$0x7c00, %esp
	xor	%ebp, %ebp

	/*
	* 进入保护模式而后退出，启动实模式下的4G内存访问
	*/
	lgdt	gdtptr
	
	/* 进入保护模式 */
	.byte	0x66, 0xea
	.long	pm
	.word	0x8

	.code32
pm:
	mov	$__DATA16_DS, %eax
	mov	%eax, %ds
	mov	%eax, %fs
	mov	%eax, %gs
	mov	%eax, %ss
	
	/* 关闭保护模式 */
	mov	%cr0, %eax
	and	$0xFFFFFFFE, %eax
	mov	%eax, %cr0
	
	.byte 	0x66, 0xea
	.long	real
	.word	0x0

	.code16
real:
	movl	$0x7c00, %esp
	
	call	__clean_screen

	/* 从软盘读内核并加载 */
	mov	$msg, %si
	call	__puts

	/* 将kernel数据加载到内存当中*/
	mov	$ELFDATA_SEG, %ax
	mov	%ax, %es
	mov	$ELFDATA_OFF, %di   /* 设置加载地址 */
	mov	$1,  %ax            /* 从第一个扇区开始读 */
	mov	$50, %cx            /* 读10个扇区*/
	/* call	__read_multi_sector */

	/* 入口地址从 eax 返回*/
	call	__load_system
	jmp	*(%eax)
	
/*
function load_system
 load_system用于加载system到指定内存
 注意system.bin是一个elf格式的文件
 ELFDATA_SEG:ELFDATA_OFF 保存elf在文件当中的地址 
 %es:%di   目标地址
*/
/*
#define EI_NIDENT 16
typedef struct {
  unsigned char e_ident[EI_NIDENT];
  uint16_t      e_type;      off: 0x10
  uint16_t      e_machine;   off: 0x12
  uint32_t      e_version;   off: 0x14
  ElfN_Addr     e_entry;     off: 0x18
  ElfN_Off      e_phoff;     off: 0x1c
  ElfN_Off      e_shoff;     off: 0x20
  uint32_t      e_flags;     off: 0x24
  uint16_t      e_ehsize;    off: 0x28
  uint16_t      e_phentsize; off: 0x2a
  uint16_t      e_phnum;     off: 0x2c
  uint16_t      e_shentsize; off: 0x2e
  uint16_t      e_shnum;     off: 0x30
  uint16_t      e_shstrndx;  off: 0x32
} ElfN_Ehdr;
*/
__load_system:
	push	%ds
	push	%es
	push	%fs
	sub	$2, %bp
	push	%esi
	push	%edi
	push	%ebx
	push	%ecx
	push	%edx

	/* 保存 ELF 文件的地址 ds:bx */
	mov	$ELFDATA_ADDR, %eax
	call	addrtrans
	mov	%ax, %bx
	mov	%fs, %ax
	mov	%ax, %ds
	
	xor	%ecx, %ecx
	/*
	load program
        计算section的绝对地址并保存在es:si当中
	*/
	movw	%ds:0x2C(%bx), %cx
	movl	%ds:0x1C(%bx), %eax
	add	$ELFDATA_ADDR, %eax
	call	addrtrans
	mov	%ax, %si
	mov	%fs, %ax
	mov	%ax, %es 
.L1:
	call	__load_program
	addw	%ds:0x2a(%bx), %si
	loop	.L1

	/*
	load section
	*/
	/*
	xorl	%ecx, %ecx
	movw	%ds:0x30(%bx), %cx
	movl	%ds:0x20(%bx), %eax
	add	$ELFDATA_ADDR, %eax
	call	addrtrans
	mov	%ax, %si
	mov	%fs, %ax
	mov	%ax, %es

.L2:
	call	__load_section
	addw	%ds:0x2e(%bx), %si
	loop	.L2
	*/
	

	/* 保存入口地址 */
	movl	%ds:0x18(%bx), %eax
	pop	%edx
	pop	%ecx
	pop	%ebx
	pop	%edi
	pop	%esi
	add	$2, %bp
	pop	%fs
	pop	%es
	pop	%ds
	ret

/*
* __load_program
* 加载一个program, program header 地址 es:si
*/
/*
分析 program 的内容
typedef struct {
  uint32_t   p_type;   off: 0x00
  Elf32_Off  p_offset; off: 0x04
  Elf32_Addr p_vaddr;  off: 0x08
  Elf32_Addr p_paddr;  off: 0x0c
  uint32_t   p_filesz; off: 0x10
  uint32_t   p_memsz;  off: 0x14
  uint32_t   p_flags;  off: 0x18
  uint32_t   p_align;  off: 0x1c
} Elf32_Phdr;
*/
__load_program:
	pushl	%eax
	push	%fs
	/* type */
	movl	%es:0(%si), %eax
	cmp	$0, %eax
	jz	.LP1

	/* size to eax */
	movl	%es:0x10(%si), %eax	
	push	%eax

	/* Offset */
	movl	%es:0x4(%si), %eax      
	add	$ELFDATA_ADDR, %eax

	/* 源地址 */
	call	addrtrans
	push	%fs
	push	%ax

	/* 目的地址 */
	movl	%es:0x08(%si, 1), %eax
	call	addrtrans
	push	%fs
	push	%ax
	
	call	__memcpy16
	add	$12, %sp
.LP1:
	pop	%fs
	popl	%eax
	ret

/*
 macro addrtrans(%eax -> %fs:%ax)
 函数负责将32位地址转化为段地址加偏移地址
*/
addrtrans:
	push	%dx
	mov	%ax, %dx
	and	$0xFFFF0000, %eax
	shr	$4,  %eax
	mov	%ax, %fs
	mov	%dx, %ax
	pop	%dx
	ret


/*
  function __memcpy16 dst, src, count
  参数通过栈传递
  top->  ret_addr word
	 dst_off  word
	dst_seg  word
	 src_off  word
	 src_seg  word
         count    dword
*/
__memcpy16:
	sub	$2, %esp
	push	%ebp
	mov	%esp, %ebp
	push	%di
	push	%si
	push	%es
	push	%ds
	push	%ecx
	push	%eax
	
	movw	8(%ebp), %di
	movw	10(%ebp), %ax
	mov	%ax, %es
	
	movw	12(%ebp), %si
	movw	14(%ebp), %ax
	mov	%ax, %ds
	movl	16(%ebp), %ecx

	/* copy from ds:si -> es:di, cnt: ecx */
	rep	movsb
	
	pop	%eax
	pop	%ecx
	pop	%ds
	pop	%es
	pop	%si
	pop	%di
	
	pop	%ebp
	add	$2, %esp
	ret
/*
function __read_multi_sector
  read multi(cx) sector ax, to es:di
*/
__read_multi_sector:
	push	%ax
	push	%di
	push	%bx

	mov	%di, %bx
.Lnext:
	/* 将指定扇区读出到临时内存当中*/
	call	__read_sector
	add	$0x200, %bx
	inc	%ax
	loop	.Lnext

	pop	%bx
	pop	%di
	pop	%ax
	ret

/*
function __read_sector
  read sector #ax to address bx
*/
__read_sector:
	pusha
	call	LBA_to_CHS

	mov	$0, %dl
	movw	$0x00201, %ax
	int	$0x13
	popa
	ret

/*
input: ax - LBA sector
output:
	ch - cylinder
	cl - sector (1-63)
	dh - head
*/
LBA_to_CHS:
	movb $18, %cl   /* SPT = 10 */
	div %cl  	/* al = LBA / SPT, ah = LBA % SPT */

	/*  cylinder = LBA / SPT / HPC */
	mov %al, %ch 
	shr $1, %ch     /* ch = cylinder , HPC = 2  */
	/* head = (LBA / SPT ) % HPC */
	mov %al, %dh
	and $1, %dh     /* dh = head */
	/*  sector = LBA % SPT + 1 */
	mov %ah, %cl
	inc %cl         /* cl = sector */
	ret

/* 打印字符串 */
__puts:
	pusha
	xor	%bh, %bh
.Lputs1:
	lodsb
	test	%al, %al
	jz	.done
	movb	$0x0e, %ah
	int	$0x10
	jmp	.Lputs1
.done:	
	popa
	ret

/*
clean screen
*/
__clean_screen:
	pusha
	movw	$0x0600, %ax
	xor	%cx, %cx
	xor	$0x0f, %bh
	movb	$24, %dh
	movb	$79, %dl
	int 	$0x10

	/* set position */
	mov	$0x2, %ah
	mov	$0, %bh
	mov	$0, %dx
	int	$0x10

	popa
	ret

msg:
	.asciz	"Loading System...\r\n"
gdt:
	.quad	0x0000000000000000 /* NULL selector */
	.quad	0x008f9a000000ffff /* base=0, limit=0xffff, DPL=0 */
	.quad 	0x008f92000000ffff /* base=0, limit=0xffff, DPL=0 */
gdtptr:
	.word	gdtptr - gdt
	.long	gdt
	.word	0
.org 510
	.word	0xAA55	
