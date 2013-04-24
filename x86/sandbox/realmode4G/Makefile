
ROOTDIR=$(CURDIR)/../../

include $(ROOTDIR)/scripts/env.mk

CFLAGS = -nostdinc
AFLAGS = -c -g $(CFLAGS)

all: install realmode.gdb

install: realmode.bin 
	dd if=$? of=$(ROOTDIR)/vm/a.img bs=512 count=1 conv=notrunc seek=0

realmode.bin: realmode.S.o
	$(LD) -Ttext 0x7c00 -s --oformat binary $? -o $@

realmode.gdb: realmode.S.o
	$(LD) -Ttext 0x7c00 $? -o $@

realmode.S.o: realmode.S
	$(CC) $(AFLAGS) $? -o $@

clean:
	rm -f *.gdb
	rm -f *.o
	rm -f *.bin
