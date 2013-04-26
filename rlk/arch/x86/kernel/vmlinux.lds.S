
#define LOAD_OFFSET __PAGE_OFFSET
	
#include <asm/asm-offsets.h>
#include <asm/page.h>
#include <asm/boot.h>
#include <asm-generic/vmlinux.lds.h>	



#undef i386
OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(startup_32)

SECTIONS {
  . = LOAD_OFFSET + LOAD_PHYSICAL_ADDR ;
  .phys_startup_32 = startup_32 - LOAD_OFFSET ;
	
  /* read-only */
  
  .text : AT(ADDR(.text) - LOAD_OFFSET) {
	_text = .; /* Text and read-only data */
        *(.text)
        *(.altinstructions)
        *(.altinstr_replacement)
        }
  _etext = .; /* End of text section */

  RO_DATA(PAGE_SIZE)

  .data : AT(ADDR(.data) - LOAD_OFFSET) { /* Data */
	_sdata = .;
        *(.init.data)
        *(.data)
        }
  _edata = .;

  .text32 ALIGN(4096) : {
          *(.text32)
          }

  __bss_start = .; /* BSS */
  .bss : {
       *(.bss.page_aligned)
       *(.bss)
       }
  __bss_stop = .;

  /* This is where the kernel creates the early boot page tables */
  . = ALIGN(PAGE_SIZE)		;
  .brk : AT(ADDR(.brk) - LOAD_OFFSET) {
	__brk_base = .;
	. += 64 * 1024;         /* 64k alignment slop space */
	*(.brk_reservation)     /* areas brk users have reserved */
	__brk_limit = .;
  }

  .end : AT(ADDR(.end) - LOAD_OFFSET) {
	_end = .;
  }

  /* Stabs debugging sections.  */
  .stab 0 : { *(.stab) }
  .stabstr 0 : { *(.stabstr) }
  .stab.excl 0 : { *(.stab.excl) }
  .stab.exclstr 0 : { *(.stab.exclstr) }
  .stab.index 0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment 0 : { *(.comment) }
}
