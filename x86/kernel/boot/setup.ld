

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)
SECTIONS
{
  . = 0x8000;
  .header         : { *(.header) }
  .entrytext      : { *(.entrytext) }
  __end_init = .;

  
  .text ALIGN(0x10): {
    __text_start = .;
    *(.text)
    __text_end = .;
  }

  .text32 ALIGN(0x10) : {
    __text32_start = .;
    *(.text32)
    __text32_end = .;
  }

  .rodata ALIGN(0x10) : { *(.rodata*) }
  .data ALIGN(0x10) : { *(.data*) }

  .bss ALIGN(0x10) :{
      __bss_start = .;
      *(.bss)
      __bss_end = .;
  }

  _end = .;
}


