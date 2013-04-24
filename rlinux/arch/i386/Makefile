# i386/Makefile
#
# This file is included by the global makefile so that you can add your own
# architecture-specific flags and dependencies. Remember to do have actions
# for "archclean" cleanup up for this architecture.

LDFLAGS         := -m elf_i386
OBJCOPYFLAGS    := -O binary -R .note -R .comment -S
LDFLAGS_vmlinux := 
CHECKFLAGS      += -D__i386__

CFLAGS += -pipe -msoft-float

CFLAGS += -mpreferred-stack-boundary=2
align  := $(cc-option-align)

CFLAGS += -fno-unit-at-a-time
CFLAGS += $(cflags-y)

mcore-y   := mach-default

head-y := arch/i386/kernel/head.o

libs-y                                  += arch/i386/lib/
core-y                                  += arch/i386/kernel/

KBUILD_IMAGE=image
boot := arch/i386/boot
image: vmlinux FORCE
	$(MAKE) $(build)=$(boot) $(KBUILD_IMAGE)
