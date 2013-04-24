
CROSS_AS   = /cross/i386-linux-elf/gcc-core-static/bin/i386-lenxyang-elf-as
CROSS_AR   = /cross/i386-linux-elf/gcc-core-static/bin/i386-lenxyang-elf-ar
CROSS_CC   = /cross/i386-linux-elf/gcc-core-static/bin/i386-lenxyang-elf-gcc
CROSS_CXX  = $(CROSS_CC) -E
CROSS_LD   = /cross/i386-linux-elf/gcc-core-static/bin/i386-lenxyang-elf-ld

HOST_AS=gas
HOST_AR=ar
HOST_CC=gcc
HOST_CXX=g++
HOST_LD=ld

MAKE = make

GDBMK=scripts/gdb.mk
RUNMK=scripts/run.mk
