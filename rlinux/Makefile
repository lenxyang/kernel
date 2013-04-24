

srctree         := $(CURDIR)
TOPDIR          := $(srctree)
objtree         := $(CURDIR)
src             := $(srctree)
obj             := $(objtree)
VPATH           := $(srctree)
export srctree objtree VPATH TOPDIR

# 定义了 CC, AR等工具的路径及默认的编译选项
include $(srctree)/scripts/const.mk

# init-y          := init/
# drivers-y       := drivers/ sound/
# net-y           := net/
# libs-y          := lib/
# core-y          := usr/

# 架构相关的Makefile, 此处与kbuild功能相同
include $(srctree)/arch/$(ARCH)/Makefile

all: vmlinux bzImage

vmlinux-dirs    := $(patsubst %/,%,$(filter %/, $(core-y) \
		$(libs-y)	\
		))

# 在此将所有的路径改变增加一个/built-in作为目标
init-y          := $(patsubst %/, %/built-in.o, $(init-y))
core-y          := $(patsubst %/, %/built-in.o, $(core-y))
drivers-y       := $(patsubst %/, %/built-in.o, $(drivers-y))
net-y           := $(patsubst %/, %/built-in.o, $(net-y))
libs-y1         := $(patsubst %/, %/lib.a, $(libs-y))
libs-y2         := $(patsubst %/, %/built-in.o, $(libs-y))
libs-y          := $(libs-y1) $(libs-y2)

vmlinux-init    := $(head-y) $(init-y)
vmlinux-main    := $(core-y) $(libs-y)
vmlinux-all     := $(vmlinux-init) $(vmlinux-main)
vmlinux-lds     := $(srctree)/arch/$(ARCH)/kernel/vmlinux.lds


.PHONY: $(vmlinux-dirs)
$(vmlinux-dirs): scripts
	@echo "vmlinux-dirs: $(vmlinux-dirs)"
	$(MAKE) $(build)=$@

vmlinux : $(vmlinux-lds) $(vmlinux-init) $(vmlinux-main)
	$(LD) $(LDFLAGS) $(LDFLAGS_vmlinux) -o $@ 	\
	-T $(vmlinux-lds) $(vmlinux-init) 		\
	--start-group $(vmlinux-main) --end-group	\
	$(filter-out $(vmlinux-lds) $(vmlinux-init) $(vmlinux-main) FORCE ,$^)

$(sort $(vmlinux-init) $(vmlinux-main)) $(vmlinux-lds): $(vmlinux-dirs) ;

gdb: 
	tools/gdb.sh

run: 
	tools/run.sh

bgdb:
	tools/bochs_gdb.sh


# 定义了cc-option等函数，主要是用来测试编译器的选项
# include $(srctree)/scripts/cc.mk

# 定义了build命令的格式
build := -f ${srctree}/scripts/build.mk obj

descend = $(MAKE) -f $(srctree)/scripts/build.mk obj=$(1) $(2)

FORCE:
