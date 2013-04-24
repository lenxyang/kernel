
ROOTDIR=$(CURDIR)/../../

include $(ROOTDIR)/scripts/env.mk


CFLAGS = -nostdinc
AFLAGS = -c -g $(CFLAGS)

all: install bootsect.bin head.elf

install: bootsect.bin head.elf bootsect.gdb
	cp $(ROOTDIR)/vm/bak.img $(ROOTDIR)/vm/a.img
	dd if=bootsect.bin of=$(ROOTDIR)/vm/a.img bs=512 count=1 conv=notrunc seek=0
	dd if=head.elf of=$(ROOTDIR)/vm/a.img bs=512 count=100 conv=notrunc seek=1

bootsect.bin: bootsect.S.o
	$(LD) -Ttext 0x7c00 -s --oformat binary $? -o $@

bootsect.gdb: bootsect.S.o
	$(LD) -Ttext 0x7c00 $? -o $@

bootsect.S.o: bootsect.S
	$(CC) $(AFLAGS) $? -o $@

head.elf: head.S.o
	$(LD) -s -e startup_32 -Ttext 0x100000 $? -o $@

head.S.o: head.S
	$(CC) $(AFLAGS) $? -o $@

gdb:
	$(MAKE) -C $(ROOTDIR) -f $(ROOTDIR)/$(GDBMK)

clean:
	rm -f *.gdb
	rm -f *.o
	rm -f *.bin
	rm -f *.elf
