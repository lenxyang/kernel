
	.code16
	.text

.equ	__BOOT_CS,      0x08
.equ	__BOOT_DS,      0x10
.equ 	__CODE16_CS,	0x0
.equ    __DATA16_DS,    0x20
.equ	__CODE32_CS,    0x28
.equ    __DATA32_DS,    0x30
.equ	__ELF_RAW,      0x800000
.equ	__SECTORS_TO_RD, 0x30

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
	mov	%eax, %ds
	mov	$__DATA16_DS, %eax
	mov	%eax, %fs
	
	/* 关闭保护模式 */
	mov	%cr0, %eax
	and	$0xFFFFFFFE, %eax
	mov	%eax, %cr0
	
	.byte 	0x66, 0xea
	.long	real
	.word	__CODE16_CS
	
	.code16
real:
	mov	%cs, %ax
	mov	%ax, %ds
	mov	%ax, %es
	mov	%ax, %ss
	movl	$0x7c00, %esp

	pushl	$__SECTORS_TO_RD
	pushl	$0x1
	pushl	$__ELF_RAW
	call	__load_sectors
	addl	$0xC, %esp
	
	call	__load_system

	lgdt	gdtdesc
	mov	%cr0, %eax
	or	$0x1, %eax
	mov	%eax, %cr0

	mov	$__BOOT_DS, %eax
	mov	%eax, %ds

	.byte 	0x66, 0xea
	.long	0x100000
	.word	__BOOT_CS

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
	push	%esi
	push	%edi
	push	%ebx
	push	%ecx

	/* elf 文件基地址*/
	mov	$__ELF_RAW, %ebx
	
	xor	%ecx, %ecx
	movw	%fs:0x2C(%ebx), %cx  /* program 个数 */
	movl	%fs:0x1C(%ebx), %esi /* program head 的起始地址 */
	add	%ebx, %esi
.L1:
	call	__load_program
	xor	%eax, %eax
	movw	%fs:0x2a(%ebx), %ax
	addl	%eax, %esi /* 下一个program head的起始地址 */
	loop	.L1

	/* 保存入口地址 */
	movl	%fs:0x18(%ebx), %eax
	pop	%ecx
	pop	%ebx
	pop	%edi
	pop	%esi
	ret

/*
* __load_program
* 加载一个program, program header 地址 %edx
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
	/* type */
	movl	%fs:0(%esi), %eax
	cmp	$0, %eax
	jz	.LP1

	/* size to eax */
	movl	%fs:0x10(%esi), %eax	
	push	%eax

	/* dst */
	movl	%fs:0x08(%esi), %eax
	pushl	%eax

	/* src */
	movl	%fs:0x4(%esi), %eax      
	add	$__ELF_RAW, %eax
	pushl	%eax
	
	call	__memcpy
	add	$12, %sp
.LP1:
	popl	%eax
	ret


/*
* 加载多个扇区到指定区域
* __load_sectors(int32 memory, int16 sector_index, int16 sector_num)
*/
__load_sectors:
	mov	%esp, %ebp
	add	$2, %ebp
	push	%ebx
	push	%ecx
	push	%edi
	push	%edx
	/*
	* 每次读取一个
	*/
	movl	(%ebp), %edi
	movw	$0x9000, %bx
	movw	4(%ebp), %dx
	movw	8(%ebp), %cx
.nextsector:
	mov	%dx, %ax
	call	__read_sector

	pushl	$0x200
	pushl	%edi
	pushl	$0x9000
	call	__memcpy
	add	$0xC, %esp
	add	$0x200, %edi
	inc	%dx
	loop	.nextsector

	pop	%edx
	pop	%edi
	pop	%ecx
	pop	%ebx
	ret
/*
* void memcpy(u32 src, u32 dst, u32 size)
*/
__memcpy:
	mov	%esp, %ebp
	add	$2, %ebp
	push	%ecx
	push	%edi
	push	%esi

	mov	0x0(%ebp), %esi
	mov	0x4(%ebp), %edi
	mov	0x8(%ebp), %ecx
	cmp	$0, %ecx
	jz	.nodata
.cpy:
	movb	%fs:(%esi), %al
	movb	%al, %fs:(%edi)
	inc	%edi
	inc	%esi
	loop	.cpy
.nodata:
	pop	%esi
	pop	%edi
	pop	%ecx
	ret

/*
 function __read_sector
 read sector #ax to address bx
*/
__read_sector:
	pusha
	
	xor	%edx, %edx
	call    LBA_to_CHS

	mov     $0, %dl
	movb    $0x02, %ah
	movb 	$0x01, %al
	int     $0x13

	popa
	ret

/*
input:	 ax - LBA sector
output:
        ch - cylinder
        cl - sector (1-63)
        dh - head
*/
LBA_to_CHS:
	movb $18, %cl   /* SPT = 10 */
	div %cl         /* al = LBA / SPT, ah = LBA % SPT */
	
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

	.align	4
gdt:
	.quad	0x0000000000000000 /* NULL selector */
	.quad	0x00cf9a000000ffff
	.quad	0x00cf92000000ffff
	.quad 	0x008f92000000ffff
	.quad 	0x008f92000000ffff
	.quad	0x00bf9a000000ffff
	.quad 	0x00bf92000000ffff
	
gdtdesc:
	.word	gdtdesc - gdt
	.long	gdt
	.word	0
	
.org	510
	.word 	0xAA55
