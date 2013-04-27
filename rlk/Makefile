AS   = /cross/i386-linux-elf/gcc-core-static/bin/i386-lenxyang-elf-as
AR   = /cross/i386-linux-elf/gcc-core-static/bin/i386-lenxyang-elf-ar
CC   = /cross/i386-linux-elf/gcc-core-static/bin/i386-lenxyang-elf-gcc
CPP  = $(CC) -E
LD   = /cross/i386-linux-elf/gcc-core-static/bin/i386-lenxyang-elf-ld
MAKE = make

ARCH=x86
arch_setup=arch/$(ARCH)/boot
arch_kernel=arch/$(ARCH)/kernel
arch_lib=arch/$(ARCH)/lib
arch_mm=arch/$(ARCH)/mm

CONFIG=  -include include/linux/autoconf.h

CFLAGS = -c -g  -I. -Iinclude -Iarch/$(ARCH)/include \
	-Wall -Wstrict-prototypes \
	-march=i386 -mregparm=3 \
	-ffreestanding \
	-fno-toplevel-reorder \
	-fno-stack-protector \
	-mpreferred-stack-boundary=2 \
	-mno-sse -mno-mmx -mno-sse2 -mno-3dnow \
	-m32 -D__KERNEL__ $(CONFIG) \
	 -fno-builtin -nostdinc

AFLAGS = $(CFLAGS) -D__ASSEMBLY__ -fomit-frame-pointer \


BOOTCFLAGS = $(CFLAGS) -include linux/gcc16code.h -I$(arch_setup)
BOOTAFLAGS = $(BOOTCFLAGS) -D__ASSEMBLY__
MMCFLAGS = $(CFLAGS) -Imm

ARCH_KERNEL_CFLAGS = $(CFLAGS)
ARCH_KERNEL_AFLAGS = $(AFLAGS)

ARCH_MM_CFLAGS = $(CFLAGS)
ARCH_MM_AFLAGS = $(AFLAGS)

ARCH_LIB_CFLAGS = $(CFLAGS)
ARCH_LIB_AFLAGS = $(AFLAGS)

KERNEL_CFLAGS      = $(CFLAGS)
MM_CFLAGS          = $(CFLAGS) -Imm
INIT_CFLAGS        = $(CFLAGS)

all: install

setup_objs = $(arch_setup)/main.c.o $(arch_setup)/memory.c.o \
	$(arch_setup)/pmjump.S.o $(arch_setup)/pm.c.o \
	$(arch_setup)/bioscall.S.o $(arch_setup)/regs.c.o \
	$(arch_setup)/copy.S.o $(arch_setup)/header.S.o \
	$(arch_setup)/printf.c.o $(arch_setup)/string.c.o $(arch_setup)/tty.c.o

arch_kernel_objs = $(arch_kernel)/head.S.o $(arch_kernel)/setup.c.o  \
		$(arch_kernel)/e820.c.o $(arch_kernel)/entry.S.o \
		$(arch_kernel)/io_delay.c.o $(arch_kernel)/rtc.c.o  \
		$(arch_kernel)/head.c.o	$(arch_kernel)/trampoline.c.o \
		$(arch_kernel)/x86_init.c.o $(arch_kernel)/probe_roms.c.o

arch_mm_objs     = $(arch_mm)/highmem.c.o $(arch_mm)/pgtable.c.o \
		$(arch_mm)/pgtable_32.c.o $(arch_mm)/ioremap.c.o \
		$(arch_mm)/init.c.o $(arch_mm)/init_32.c.o \
		$(arch_mm)/ioremap.c.o $(arch_mm)/pageattr.c.o

arch_lib_objs    = $(arch_lib)/delay.c.o $(arch_lib)/checksum_32.S.o \
		$(arch_lib)/memcpy_32.c.o $(arch_lib)/strstr_32.c.o \
		$(arch_lib)/string_32.c.o
kernel_objs      = kernel/printk.c.o kernel/panic.c.o kernel/resource.c.o 
mm_objs          = mm/highmem.c.o  mm/init-mm.c.o mm/bootmem.c.o
lib_objs         = 
init_objs        = init/main.c.o

binary := setup.bin vmlinux.bin
debugs := setup.gdb vmlinux.gdb

vmlinux_size =`du -h vmlinux.bin | cut -f1`
install: $(binary) $(debugs)
	cp vm/bak.img vm/a.img
	dd if=setup.bin    of=vm/a.img bs=512 count=19 conv=notrunc seek=1
	dd if=vmlinux.bin  of=vm/a.img bs=512 count=1000 conv=notrunc seek=20

###################################################
# setup
vmlinux.bin: $(arch_kernel_objs) $(arch_mm_objs)  $(arch_lib_objs) $(kernel_objs) $(lib_objs) $(mm_objs) $(init_objs) $(arch_kernel)/vmlinux.lds
	$(LD) -s -T $(arch_kernel)/vmlinux.lds $? -o $@

vmlinux.gdb: $(arch_kernel_objs)  $(arch_mm_objs) $(arch_lib_objs) $(kernel_objs) $(lib_objs) $(mm_objs) $(init_objs) $(arch_kernel)/vmlinux.lds
	$(LD) -T $(arch_kernel)/vmlinux.lds $? -o $@

# arch/x86/kernel
$(arch_kernel)/head.S.o: $(arch_kernel)/head.S
	$(CC) $(ARCH_KERNEL_AFLAGS) $? -o $@
$(arch_kernel)/entry.S.o: $(arch_kernel)/entry.S
	$(CC) $(ARCH_KERNEL_AFLAGS) $? -o $@
$(arch_kernel)/setup.c.o: $(arch_kernel)/setup.c
	$(CC) $(ARCH_KERNEL_CFLAGS) $? -o $@
$(arch_kernel)/e820.c.o: $(arch_kernel)/e820.c
	$(CC) $(ARCH_KERNEL_CFLAGS) $? -o $@
$(arch_kernel)/vmlinux.lds: $(arch_kernel)/vmlinux.lds.S
	$(CPP) $(CFLAGS) -P -C  -D__ASSEMBLY__  -o $@ $<
$(arch_kernel)/io_delay.c.o: $(arch_kernel)/io_delay.c
	$(CC) $(ARCH_KERNEL_CFLAGS) $? -o $@
$(arch_kernel)/rtc.c.o: $(arch_kernel)/rtc.c
	$(CC) $(ARCH_KERNEL_CFLAGS) $? -o $@
$(arch_kernel)/head.c.o: $(arch_kernel)/head.c
	$(CC) $(ARCH_KERNEL_CFLAGS) $? -o $@
$(arch_kernel)/trampoline.c.o: $(arch_kernel)/trampoline.c
	$(CC) $(ARCH_KERNEL_CFLAGS) $? -o $@
$(arch_kernel)/x86_init.c.o: $(arch_kernel)/x86_init.c
	$(CC) $(ARCH_KERNEL_CFLAGS) $? -o $@
$(arch_kernel)/probe_roms.c.o: $(arch_kernel)/probe_roms.c
	$(CC) $(ARCH_KERNEL_CFLAGS) $? -o $@

$(arch_mm)/highmem.c.o: $(arch_mm)/highmem.c
	$(CC) $(ARCH_MM_CFLAGS) $? -o $@
$(arch_mm)/pgtable.c.o: $(arch_mm)/pgtable.c
	$(CC) $(ARCH_MM_CFLAGS) $? -o $@
$(arch_mm)/pgtable_32.c.o: $(arch_mm)/pgtable_32.c
	$(CC) $(ARCH_MM_CFLAGS) $? -o $@
$(arch_mm)/init_32.c.o: $(arch_mm)/init_32.c
	$(CC) $(ARCH_MM_CFLAGS) $? -o $@
$(arch_mm)/ioremap.c.o: $(arch_mm)/ioremap.c
	$(CC) $(ARCH_MM_CFLAGS) $? -o $@
$(arch_mm)/init.c.o: $(arch_mm)/init.c
	$(CC) $(ARCH_MM_CFLAGS) $? -o $@
$(arch_mm)/ioremap.c.o: $(arch_mm)/ioremap.c
	$(CC) $(ARCH_MM_CFLAGS) $? -o $@
$(arch_mm)/pageattr.c.o: $(arch_mm)/pageattr.c
	$(CC) $(ARCH_MM_CFLAGS) $? -o $@

$(arch_lib)/delay.c.o: $(arch_lib)/delay.c
	$(CC) $(ARCH_LIB_CFLAGS) $? -o $@
$(arch_lib)/checksum_32.S.o: $(arch_lib)/checksum_32.S
	$(CC) $(ARCH_LIB_AFLAGS) $? -o $@
$(arch_lib)/memcpy_32.c.o: $(arch_lib)/memcpy_32.c
	$(CC) $(ARCH_LIB_CFLAGS) $? -o $@
$(arch_lib)/strstr_32.c.o: $(arch_lib)/strstr_32.c
	$(CC) $(ARCH_LIB_CFLAGS) $? -o $@
$(arch_lib)/string_32.c.o: $(arch_lib)/string_32.c
	$(CC) $(ARCH_LIB_CFLAGS) $? -o $@

# kernel
kernel/printk.c.o: kernel/printk.c
	$(CC) $(KERNEL_CFLAGS) -o $@ $<
kernel/panic.c.o: kernel/panic.c
	$(CC) $(KERNEL_CFLAGS) -o $@ $<
kernel/resource.c.o: kernel/resource.c
	$(CC) $(KERNEL_CFLAGS) -o $@ $<

# init
init/main.c.o: init/main.c
	$(CC) $(INIT_CFLAGS) -o $@ $<

# mm
mm/highmem.c.o: mm/highmem.c
	$(CC) $(MM_CFLAGS)  -o $@ $<
mm/init-mm.c.o: mm/init-mm.c
	$(CC) $(ARCH_MM_CFLAGS) $? -o $@
mm/bootmem.c.o: mm/bootmem.c
	$(CC) $(ARCH_MM_CFLAGS) $? -o $@

###################################################
# setup
setup.bin: $(setup_objs)
	$(LD) -s -T $(arch_setup)/setup.ld $? -o $@

setup.gdb: $(setup_objs)
	$(LD) -T $(arch_setup)/setup.ld $? -o $@

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

run:
	./tools/run.sh

brun:
	./tools/bochs_run.sh

gdb:
	./tools/gdb.sh

bgdb:
	./tools/bochs_gdb.sh

binternaldbg:
	./tools/bochs_internal_gdb.sh

clean:
	rm -f $(arch_setup)/*.o
	rm -f $(arch_kernel)/*.o
	rm -f $(arch_kernel)/vmlinux.lds
	rm -f $(arch_lib)/*.o
	rm -f $(arch_mm)/*.o
	rm -f kernel/*.o
	rm -f mm/*.o
	rm -f init/*.o
	rm -f setup.gdb setup.bin


# 废弃的目标(暂时)
paravirt_objs = $(arch_kernel)/paravirt-spinlocks.c.o \
		$(arch_kernel)/paravirt.c.o $(arch_kernel)/paravirt_patch_32.c.o
$(arch_kernel)/paravirt-spinlocks.c.o :$(arch_kernel)/paravirt-spinlocks.c
	$(CC) $(ARCH_KERNEL_CFLAGS) $? -o $@
$(arch_kernel)/paravirt.c.o: $(arch_kernel)/paravirt.c
	$(CC) -fno-stack-protector $(ARCH_KERNEL_CFLAGS) $? -o $@
$(arch_kernel)/paravirt_patch_32.c.o: $(arch_kernel)/paravirt_patch_32.c
	$(CC) $(ARCH_KERNEL_CFLAGS) $? -o $@
