
LDFLAGS_bootsect := -Ttext 0x7c00 -s --oformat binary
LDFLAGS_bootsect_gdb  := -Ttext 0x7c00
LDFLAGS_setup    := -Ttext 0x0 -s --oformat binary -e begtext

targets   := bootsect.o

all: bootsect.bin

%(obj)/bootsect.o: $(obj)/bootsect.S FORCE

$(obj)/bootsect.bin: $(obj)/bootsect.o
	$(LD) $(LDFLAGS_bootsect) $? -o $@

$(obj)/bootsect.gdb: $(obj)/bootsect.o
	$(LD) -Ttext 0x7c00 $(LDFLAGS_bootsect_gdb) $? -o $@

image: $(obj)/bootsect.bin $(obj)/bootsect.gdb
	@echo "Build bootsect"
	dd if=$(obj)/bootsect.bin of=vm/a.img bs=512 count=1 conv=notrunc seek=0
	dd if=$(TOPDIR)/vmlinux of=vm/a.img bs=512 count=50 conv=notrunc seek=1

FORCE:

