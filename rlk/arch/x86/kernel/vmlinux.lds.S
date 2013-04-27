
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

  /* Init code and data - will be freed after init */
  . = ALIGN(PAGE_SIZE) ;
	.init.begin : AT(ADDR(.init.begin) - LOAD_OFFSET) {
	__init_begin = . ; /* paired with __init_end */
	}

  INIT_DATA_SECTION(16)

  .x86_cpu_dev.init : AT(ADDR(.x86_cpu_dev.init) - LOAD_OFFSET) {
	__x86_cpu_dev_start = . ;
	*(.x86_cpu_dev.init)
	__x86_cpu_dev_end = . ;
	}

  . = ALIGN(8)	;
  .parainstructions : AT(ADDR(.parainstructions) - LOAD_OFFSET) {
	__parainstructions = . ;
	*(.parainstructions)
	__parainstructions_end = . ;
	}

  . = ALIGN(8)	;
	.altinstructions : AT(ADDR(.altinstructions) - LOAD_OFFSET) {
	__alt_instructions = . ;
	*(.altinstructions)
	__alt_instructions_end = . ;
	}

  .altinstr_replacement : AT(ADDR(.altinstr_replacement) - LOAD_OFFSET) {
	*(.altinstr_replacement)
	}

  /*
  * .exit.text is discard at runtime, not link time, to deal with
  *  references from .altinstructions and .eh_frame
  */
  .exit.text : AT(ADDR(.exit.text) - LOAD_OFFSET) {
        EXIT_TEXT
	}

  .exit.data : AT(ADDR(.exit.data) - LOAD_OFFSET) {
	EXIT_DATA
	}

  . = ALIGN(PAGE_SIZE);
	
  /* freed after init ends here */
  .init.end : AT(ADDR(.init.end) - LOAD_OFFSET) {
	__init_end = . ;
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
