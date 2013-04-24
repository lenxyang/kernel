
ROOTDIR=$(CURDIR)/../../

include $(ROOTDIR)/scripts/env.mk

CFLAGS = -c -g  -I. -Wall -Wstrict-prototypes \
	-fomit-frame-pointer \
	-mno-sse -mno-mmx -mno-sse2 -mno-3dnow \
	-std=gnu99	\
	-fno-builtin -nostdinc

AFLAGS = $(CFLAGS) -D__ASSEMBLY__

all: install bootsect.bin vmlinux vmlinux.gdb

install: bootsect.bin bootsect.gdb vmlinux.bin vmlinux.gdb
	cp $(ROOTDIR)/vm/bak.img $(ROOTDIR)/vm/a.img
	dd if=bootsect.bin of=$(ROOTDIR)/vm/a.img bs=512 count=1 conv=notrunc seek=0
	dd if=vmlinux.bin of=$(ROOTDIR)/vm/a.img bs=512 count=100 conv=notrunc seek=1

bootsect.bin: bootsect.S.o
	$(LD) -Ttext 0x7c00 -s --oformat binary $? -o $@

bootsect.gdb: bootsect.S.o
	$(LD) -Ttext 0x7c00 $? -o $@

bootsect.S.o: bootsect.S
	$(CC) $(AFLAGS) $? -o $@

vmlinux_objs = head.S.o kernel_main.c.o screen.c.o init.c.o
vmlinux.bin: $(vmlinux_objs)
	$(LD) -s -T vmlinux.lds $? -o $@

vmlinux.gdb: $(vmlinux_objs)
	$(LD) -T vmlinux.lds $? -o $@

head.S.o: head.S
	$(CC) $(AFLAGS) $? -o $@

screen.c.o: screen.c
	$(CC) $(CFLAGS) $? -o $@

kernel_main.c.o: kernel_main.c
	$(CC) $(CFLAGS) $? -o $@

init.c.o: init.c	
	$(CC) $(CFLAGS) $? -o $@

gdb:
	$(MAKE) -C $(ROOTDIR) -f $(ROOTDIR)/$(GDBMK) gdb

run:
	$(MAKE) -C $(ROOTDIR) -f $(ROOTDIR)/$(GDBMK) run

bgdb:
	$(MAKE) -C $(ROOTDIR) -f $(ROOTDIR)/$(GDBMK) bgdb

clean:
	rm -f *.gdb
	rm -f *.o
	rm -f *.bin
	rm -f *.elf
