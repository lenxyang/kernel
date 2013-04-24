

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(main)

SECTIONS
{
  . = 0x8000;
  __text_start = .;
  .text : { *(.text) }

  .text32 ALIGN(0x10) : {
    __text32_start = .;
    *(.text32)
    __text_end = .;
  }

  .rodata ALIGN(0x10) : { *(.rodata*) }

  .bss ALIGN(0x10) :{
      __bss_start = .;
      *(.bss)
      __bss_end = .;
  }

  _end = .;
}


