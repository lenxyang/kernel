
ROOTDIR=$(CURDIR)/../../

include $(ROOTDIR)/scripts/env.mk


CFLAGS = -c -g  -I. -Iinclude -Wall -Wstrict-prototypes \
	-fomit-frame-pointer \
	-mno-sse -mno-mmx -mno-sse2 -mno-3dnow \
	-std=gnu99	\
	-fno-builtin -nostdinc

BOOTFLAGS = $(CFLAGS) -include boot/gcc16code.h

AFLAGS = $(CFLAGS) -D__ASSEMBLY__

all: install

boot_objs = boot/main.c.o boot/pmjump.S.o boot/pm.c.o
vmlinux_objs = kernel/head.S.o

binary := bootsect.bin setup.bin vmlinux.bin
debugs := bootsect.gdb setup.gdb vmlinux.gdb

install: $(binary) $(debugs)
	cp $(ROOTDIR)/vm/bak.img $(ROOTDIR)/vm/a.img
	dd if=bootsect.bin of=$(ROOTDIR)/vm/a.img bs=512 count=1  conv=notrunc seek=0
	dd if=setup.bin    of=$(ROOTDIR)/vm/a.img bs=512 count=19 conv=notrunc seek=1
	dd if=vmlinux.bin  of=$(ROOTDIR)/vm/a.img bs=512 count=80 conv=notrunc seek=20

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
	$(CC) $(BOOTFLAGS) $? -o $@

boot/pm.c.o: boot/pm.c
	$(CC) $(BOOTFLAGS) $? -o $@

boot/pmjump.S.o: boot/pmjump.S
	$(CC) $(AFLAGS) $? -o $@

# kernel
kernel/head.S.o: kernel/head.S
	$(CC) $(AFLAGS) $? -o $@

gdb:
	$(MAKE) -C $(ROOTDIR) -f $(ROOTDIR)/$(GDBMK) gdb

run:
	$(MAKE) -C $(ROOTDIR) -f $(ROOTDIR)/$(GDBMK) run

bgdb:
	$(MAKE) -C $(ROOTDIR) -f $(ROOTDIR)/$(GDBMK) bgdb

clean:
	rm boot/*.o
	rm kernel/*.o
	rm bootsect/*.o
	rm *.bin
	rm *.gdb

