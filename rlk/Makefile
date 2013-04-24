AS   = /cross/i386-linux-elf/gcc-core-static/bin/i386-lenxyang-elf-as
AR   = /cross/i386-linux-elf/gcc-core-static/bin/i386-lenxyang-elf-ar
CC   = /cross/i386-linux-elf/gcc-core-static/bin/i386-lenxyang-elf-gcc
CPP  = $(CC) -E
LD   = /cross/i386-linux-elf/gcc-core-static/bin/i386-lenxyang-elf-ld
MAKE = make

arch=x86
arch_setup=arch/$(arch)/boot
arch_kernel=arch/$(arch)/kernel

CONFIG= -DCONFIG_X86_32 \
	-DCONFIG_X86_L1_CACHE_BYTES=64\
	-DCONFIG_X86_INTERNODE_CACHE_BYTES=64 \
	-DCONFIG_X86_L1_CACHE_SHIFT=5 \
	-DCONFIG_PAGE_OFFSET=0xC0000000 \

CFLAGS = -c -g  -I. -Iinclude -Iarch/$(arch)/include \
	-Wall -Wstrict-prototypes \
	-march=i386 -mregparm=3 \
	-ffreestanding \
	-fno-toplevel-reorder \
	-fno-stack-protector \
	-mpreferred-stack-boundary=2 \
	-mno-sse -mno-mmx -mno-sse2 -mno-3dnow \
	-m32 -D__KERNEL__ $(CONFIG) \
	-std=gnu99 -fno-builtin -nostdinc

AFLAGS = $(CFLAGS) -D__ASSEMBLY__ -fomit-frame-pointer \

BOOTCFLAGS = $(CFLAGS) -include linux/gcc16code.h
BOOTAFLAGS = $(BOOTCFLAGS) -D__ASSEMBLY__
MMCFLAGS = $(CFLAGS) -Imm

all: install

boot_objs = $(arch_setup)/main.c.o $(arch_setup)/memory.c.o \
	$(arch_setup)/pmjump.S.o $(arch_setup)/pm.c.o \
	$(arch_setup)/bioscall.S.o $(arch_setup)/regs.c.o \
	$(arch_setup)/copy.S.o $(arch_setup)/header.S.o \
	$(arch_setup)/printf.c.o $(arch_setup)/string.c.o $(arch_setup)/tty.c.o

binary := setup.bin
debugs := setup.gdb

vmlinux_size =`du -h vmlinux.bin | cut -f1`
install: $(binary) $(debugs)
	cp vm/bak.img vm/a.img
	dd if=setup.bin    of=vm/a.img bs=512 count=19 conv=notrunc seek=1
	dd if=vmlinux.bin  of=vm/a.img bs=512 count=1000 conv=notrunc seek=20

setup.bin: $(boot_objs)
	$(LD) -s -T $(arch_setup)/setup.ld $? -o $@

setup.gdb: $(boot_objs)
	$(LD) -T $(arch_setup)/setup.ld $? -o $@

# setup
$(arch_setup)/main.c.o: $(arch_setup)/main.c
	$(CC) $(BOOTCFLAGS) $? -o $@

$(arch_setup)/memory.c.o: $(arch_setup)/memory.c
	$(CC) $(BOOTCFLAGS) $? -o $@

$(arch_setup)/pm.c.o: $(arch_setup)/pm.c
	$(CC) $(BOOTCFLAGS) $? -o $@

$(arch_setup)/printf.c.o: $(arch_setup)/printf.c
	$(CC) $(BOOTCFLAGS) $? -o $@

$(arch_setup)/string.c.o: $(arch_setup)/string.c
	$(CC) $(BOOTCFLAGS) $? -o $@

$(arch_setup)/tty.c.o: $(arch_setup)/tty.c
	$(CC) $(BOOTCFLAGS) $? -o $@

$(arch_setup)/regs.c.o: $(arch_setup)/regs.c
	$(CC) $(BOOTCFLAGS) $? -o $@

$(arch_setup)/pmjump.S.o: $(arch_setup)/pmjump.S
	$(CC) $(BOOTAFLAGS) $? -o $@

$(arch_setup)/bioscall.S.o: $(arch_setup)/bioscall.S
	$(CC) $(BOOTAFLAGS) $? -o $@

$(arch_setup)/copy.S.o: $(arch_setup)/copy.S
	$(CC) $(BOOTAFLAGS) $? -o $@

$(arch_setup)/header.S.o: $(arch_setup)/header.S
	$(CC) $(BOOTAFLAGS) $? -o $@


gdb:
	$(MAKE) -C $(ROOTDIR) -f $(ROOTDIR)/$(GDBMK) gdb

run:
	$(MAKE) -C $(ROOTDIR) -f $(ROOTDIR)/$(GDBMK) run

bgdb:
	$(MAKE) -C $(ROOTDIR) -f $(ROOTDIR)/$(GDBMK) bgdb

clean:
	rm -f $(arch_setup)/*.o



