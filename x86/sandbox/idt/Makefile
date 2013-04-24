
ROOTDIR=$(CURDIR)/../../

include $(ROOTDIR)/scripts/env.mk

CFLAGS = -c -g  -I. -Wall -Wstrict-prototypes \
	-fomit-frame-pointer \
	-mno-sse -mno-mmx -mno-sse2 -mno-3dnow \
	-std=gnu99	\
	-fno-builtin -nostdinc

AFLAGS = $(CFLAGS) -D__ASSEMBLY__

all: install bootsect.bin

install: bootsect.bin bootsect.gdb 
	cp $(ROOTDIR)/vm/bak.img $(ROOTDIR)/vm/a.img
	dd if=bootsect.bin of=$(ROOTDIR)/vm/a.img bs=512 count=1 conv=notrunc seek=0

bootsect.bin: bootsect.S.o
	$(LD) -Ttext 0x7c00 -s --oformat binary $? -o $@

bootsect.gdb: bootsect.S.o
	$(LD) -Ttext 0x7c00 $? -o $@

bootsect.S.o: bootsect.S
	$(CC) $(AFLAGS) $? -o $@

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
