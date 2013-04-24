OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(startup_32)

SECTIONS {
  . = 0x100000 ;
  /* read-only */
  _text = .; /* Text and read-only data */
  .text : {
  	*(.text)
	} = 0x9090
  _etext = .; /* End of text section */

  /* writeable */
  .rodata ALIGN(16) : {
  	  *(.rodata)
	  }

  .data : { /* Data */
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
