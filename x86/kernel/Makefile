
ROOTDIR=$(CURDIR)/../

include $(ROOTDIR)/scripts/env.mk


CFLAGS = -c -g  -I. -Iinclude -Wall \
	-Wstrict-prototypes \
	-march=i386 -mregparm=3 \
	-ffreestanding \
	-fno-toplevel-reorder \
	-fno-stack-protector \
	-mpreferred-stack-boundary=2 \
	-m32 \
	-mno-sse -mno-mmx -mno-sse2 -mno-3dnow \
	-std=gnu99 -fno-builtin -nostdinc

AFLAGS = $(CFLAGS) -D__ASSEMBLY__ -fomit-frame-pointer \

BOOTCFLAGS = $(CFLAGS) -include boot/gcc16code.h
BOOTAFLAGS = $(BOOTCFLAGS) -D__ASSEMBLY__
MMCFLAGS = $(CFLAGS) -Imm

all: install

boot_objs = boot/main.c.o boot/memory.c.o boot/pmjump.S.o boot/pm.c.o \
	boot/bioscall.S.o boot/regs.c.o boot/copy.S.o boot/header.S.o \
	boot/printf.c.o boot/string.c.o boot/tty.c.o
vmlinux_objs = kernel/head.S.o kernel/init.c.o init/main.c.o \
	kernel/io_delay.c.o kernel/early_printk.c.o \
	kernel/setup.c.o kernel/printk.c.o \
	kernel/panic.c.o kernel/e820.c.o	\
	kernel/x86_init.c.o \
	mm/highmem.c.o 	mm/page_alloc.c.o mm/init.c.o mm/bootmem.c.o \
	mm/sparse.c.o mm/pgtable.c.o mm/memtest.c.o mm/pageattr.c.o \
	mm/numa.c.o \
	lib/string.c.o lib/vsprintf.c.o lib/ctype.c.o \
	lib/hexdump.c.o lib/hweight.c.o

binary := bootsect.bin setup.bin vmlinux.bin
debugs := bootsect.gdb setup.gdb vmlinux.gdb

vmlinux_size =`du -h vmlinux.bin | cut -f1`
install: $(binary) $(debugs)
	cp $(ROOTDIR)/vm/bak.img $(ROOTDIR)/vm/a.img
	dd if=bootsect.bin of=$(ROOTDIR)/vm/a.img bs=512 count=1  conv=notrunc seek=0
	dd if=setup.bin    of=$(ROOTDIR)/vm/a.img bs=512 count=19 conv=notrunc seek=1
	dd if=vmlinux.bin  of=$(ROOTDIR)/vm/a.img bs=512 count=1000 conv=notrunc seek=20

bootsect.bin: bootsect/bootsect.S.o
	$(LD) -Ttext 0x7c00 -s --oformat binary $? -o $@

bootsect.gdb: bootsect/bootsect.S.o
	$(LD) -Ttext 0x7c00 $? -o $@

setup.bin: $(boot_objs)
	$(LD) -s -T boot/setup.ld $? -o $@

setup.gdb: $(boot_objs)
	$(LD) -T boot/setup.ld $? -o $@

vmlinux.bin: $(vmlinux_objs)
	$(LD) -s -T kernel/vmlinux.lds $? -o $@

vmlinux.gdb: $(vmlinux_objs)
	$(LD) -T kernel/vmlinux.lds $? -o $@

bootsect/bootsect.S.o: bootsect/bootsect.S
	$(CC) $(AFLAGS) $? -o $@

# setup
boot/main.c.o: boot/main.c
	$(CC) $(BOOTCFLAGS) $? -o $@

boot/memory.c.o: boot/memory.c
	$(CC) $(BOOTCFLAGS) $? -o $@

boot/pm.c.o: boot/pm.c
	$(CC) $(BOOTCFLAGS) $? -o $@

boot/printf.c.o: boot/printf.c
	$(CC) $(BOOTCFLAGS) $? -o $@

boot/string.c.o: boot/string.c
	$(CC) $(BOOTCFLAGS) $? -o $@

boot/tty.c.o: boot/tty.c
	$(CC) $(BOOTCFLAGS) $? -o $@

boot/regs.c.o: boot/regs.c
	$(CC) $(BOOTCFLAGS) $? -o $@

boot/pmjump.S.o: boot/pmjump.S
	$(CC) $(BOOTAFLAGS) $? -o $@

boot/bioscall.S.o: boot/bioscall.S
	$(CC) $(BOOTAFLAGS) $? -o $@

boot/copy.S.o: boot/copy.S
	$(CC) $(BOOTAFLAGS) $? -o $@

boot/header.S.o: boot/header.S
	$(CC) $(BOOTAFLAGS) $? -o $@

# kernel
kernel/head.S.o: kernel/head.S
	$(CC) $(AFLAGS) $? -o $@

kernel/init.c.o: kernel/init.c
	$(CC) $(CFLAGS) $? -o $@

init/main.c.o: init/main.c
	$(CC) $(CFLAGS) $? -o $@

kernel/setup.c.o: kernel/setup.c
	$(CC) $(CFLAGS) $? -o $@

kernel/printk.c.o: kernel/printk.c
	$(CC) $(CFLAGS) $? -o $@

kernel/io_delay.c.o: kernel/io_delay.c
	$(CC) $(CFLAGS) $? -o $@

kernel/panic.c.o: kernel/panic.c
	$(CC) $(CFLAGS) $? -o $@

kernel/early_printk.c.o: kernel/early_printk.c
	$(CC) $(CFLAGS) $? -o $@

kernel/e820.c.o: kernel/e820.c
	$(CC) $(CFLAGS) $? -o $@

kernel/x86_init.c.o: kernel/x86_init.c
	$(CC) $(CFLAGS) $? -o $@

mm/highmem.c.o: mm/highmem.c
	$(CC) $(MMCFLAGS) $? -o $@

mm/memtest.c.o: mm/memtest.c
	$(CC) $(MMCFLAGS) $? -o $@

mm/init.c.o: mm/init.c
	$(CC) $(MMCFLAGS) $? -o $@

mm/bootmem.c.o: mm/bootmem.c
	$(CC) $(MMCFLAGS) $? -o $@

mm/page_alloc.c.o: mm/page_alloc.c
	$(CC) $(MMCFLAGS) $? -o $@

mm/sparse.c.o: mm/sparse.c
	$(CC) $(MMCFLAGS) $? -o $@

mm/pgtable.c.o: mm/pgtable.c
	$(CC) $(MMCFLAGS) $? -o $@

mm/pageattr.c.o: mm/pageattr.c
	$(CC) $(MMCFLAGS) $? -o $@

mm/numa.c.o: mm/numa.c
	$(CC) $(MMCFLAGS) $? -o $@

lib/string.c.o: lib/string.c
	$(CC) $(CFLAGS) $? -o $@

lib/vsprintf.c.o: lib/vsprintf.c
	$(CC) $(CFLAGS) $? -o $@

lib/ctype.c.o: lib/ctype.c
	$(CC) $(CFLAGS) $? -o $@

lib/hexdump.c.o: lib/hexdump.c
	$(CC) $(CFLAGS) $? -o $@

lib/hweight.c.o: lib/hweight.c
	$(CC) $(CFLAGS) $? -o $@

gdb:
	$(MAKE) -C $(ROOTDIR) -f $(ROOTDIR)/$(GDBMK) gdb

run:
	$(MAKE) -C $(ROOTDIR) -f $(ROOTDIR)/$(GDBMK) run

bgdb:
	$(MAKE) -C $(ROOTDIR) -f $(ROOTDIR)/$(GDBMK) bgdb

clean:
	rm -f boot/*.o
	rm -f kernel/*.o
	rm -f bootsect/*.o
	rm -f mm/*.o
	rm -f init/*.o
	rm -f *.bin
	rm -f *.gdb


