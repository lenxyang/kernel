#include <asm/asm-offsets.h>
#include <asm/page.h>
#include <asm-generic/vmlinux.lds.h>	

#define LOAD_OFFSET __PAGE_OFFSET

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
        } = 0x9090
  _etext = .; /* End of text section */

  RO_DATA(PAGE_SIZE)

  .data : AT(ADDR(.data) - LOAD_OFFSET) { /* Data */
	_sdata = .;
        *(.init.data)
        *(.data)
        }

  .text32 ALIGN(4096) : {
          *(.text32)
          }

  __bss_start = .; /* BSS */
  .bss : {
       *(.bss.page_aligned)
       *(.bss)
       }
  __bss_stop = .;
  __end = .;

  /* This is where the kernel creates the early boot page tables */
  . = ALIGN(4096);
  pg0 = .;

  /* Stabs debugging sections.  */
  .stab 0 : { *(.stab) }
  .stabstr 0 : { *(.stabstr) }
  .stab.excl 0 : { *(.stab.excl) }
  .stab.exclstr 0 : { *(.stab.exclstr) }
  .stab.index 0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment 0 : { *(.comment) }
}
