ARCH=i386
AS=/cross/i386-linux-elf/gcc-core-static/bin/i386-lenxyang-elf-as
AR=/cross/i386-linux-elf/gcc-core-static/bin/i386-lenxyang-elf-ar
CC=/cross/i386-linux-elf/gcc-core-static/bin/i386-lenxyang-elf-gcc
CPP=$(CC) -E
LD=/cross/i386-linux-elf/gcc-core-static/bin/i386-lenxyang-elf-ld

NOSTDINC_FLAGS  = -nostdinc -isystem $(shell $(CC) -print-file-name=include)
CHECKFLAGS     := -D__linux__ -Dlinux -D__STDC__ -Dunix -D__unix__
CHECKFLAGS     += $(NOSTDINC_FLAGS)
MODFLAGS        = -DMODULE
CFLAGS_MODULE   = $(MODFLAGS)
AFLAGS_MODULE   = $(MODFLAGS)
LDFLAGS_MODULE  = -r
CFLAGS_KERNEL   =
AFLAGS_KERNEL   =

LINUXINCLUDE    := -Iinclude \
                   $(if $(KBUILD_SRC),-Iinclude2 -I$(srctree)/include)

CPPFLAGS        := -D__KERNEL__ $(LINUXINCLUDE)

CFLAGS          := -Wall -Wstrict-prototypes -Wno-trigraphs \
                   -fno-strict-aliasing -fno-common \
                   -ffreestanding -Wno-pointer-sign \
	           -Wdeclaration-after-statement \
                   -g
AFLAGS          := -g -D__ASSEMBLY__

export AS AR CC CPP LD
export CPPFLAGS NOSTDINC_FLAGS LINUXINCLUDE OBJCOPYFLAGS LDFLAGS
export CFLAGS CFLAGS_KERNEL CFLAGS_MODULE
export AFLAGS AFLAGS_KERNEL AFLAGS_MODULE


export CPPFLAGS_vmlinux.lds += -P -C -U$(ARCH)
