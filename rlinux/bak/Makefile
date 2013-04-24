ARCH=i386

# 在Makefile当中有一个小技巧，如果在shell命令前加上@, 则这条shell
# 命令不会被打印到终端。
# Makefile通过对于Q的定义来指定用户是否能看到这些命令
ifeq ($(KBUILD_VERBOSE),1)
  quiet =
  Q =
else
  quiet=quiet_
  Q = @
endif

srctree         := $(CURDIR)
TOPDIR          := $(srctree)
objtree         := $(CURDIR)
src             := $(srctree)
obj             := $(objtree)

VPATH           := $(srctree)
export srctree objtree VPATH TOPDIR

######
# cc support functions to be used (only) in arch/$(ARCH)/Makefile
# See documentation in Documentation/kbuild/makefiles.txt

# cc-option
# Usage: cflags-y += $(call gcc-option, -march=winchip-c6, -march=i586)

cc-option = $(shell if $(CC) $(CFLAGS) $(1) -S -o /dev/null -xc /dev/null \
             > /dev/null 2>&1; then echo "$(1)"; else echo "$(2)"; fi ;)

# For backward compatibility
check_gcc = $(warning check_gcc is deprecated - use cc-option) \
            $(call cc-option, $(1),$(2))

# cc-option-yn
# Usage: flag := $(call cc-option-yn, -march=winchip-c6)
cc-option-yn = $(shell if $(CC) $(CFLAGS) $(1) -S -o /dev/null -xc /dev/null \
                > /dev/null 2>&1; then echo "y"; else echo "n"; fi;)

# cc-option-align
# Prefix align with either -falign or -malign
cc-option-align = $(subst -functions=0,,\
        $(call cc-option,-falign-functions=0,-malign-functions=0))

# cc-version
# Usage gcc-ver := $(call cc-version $(CC))
cc-version = $(shell $(CONFIG_SHELL) $(srctree)/scripts/gcc-version.sh \
              $(if $(1), $(1), $(CC)))

# Look for make include files relative to root of kernel src
MAKEFLAGS += --include-dir=$(srctree)


AS=/cross/i386-linux-elf/gcc-core-static/bin/i386-lenxyang-elf-as
AR=/cross/i386-linux-elf/gcc-core-static/bin/i386-lenxyang-elf-ar
CC=/cross/i386-linux-elf/gcc-core-static/bin/i386-lenxyang-elf-gcc
CPP=$(CC) -E
LD=/cross/i386-linux-elf/gcc-core-static/bin/i386-lenxyang-elf-ld
AWK=awk
PERL=perl

NOSTDINC_FLAGS  = -nostdinc -isystem $(shell $(CC) -print-file-name=include)
CHECKFLAGS     := -D__linux__ -Dlinux -D__STDC__ -Dunix -D__unix__
CHECKFLAGS     += $(NOSTDINC_FLAGS)
MODFLAGS        = -DMODULE
CFLAGS_MODULE   = $(MODFLAGS)
AFLAGS_MODULE   = $(MODFLAGS)
LDFLAGS_MODULE  = -r
CFLAGS_KERNEL   =
AFLAGS_KERNEL   =

LINUXINCLUDE    := -Iinclude \
                   $(if $(KBUILD_SRC),-Iinclude2 -I$(srctree)/include)

CPPFLAGS        := -D__KERNEL__ $(LINUXINCLUDE)

CFLAGS          := -Wall -Wstrict-prototypes -Wno-trigraphs \
                   -fno-strict-aliasing -fno-common \
                   -ffreestanding -,-Wno-pointer-sign \
	           -Wdeclaration-after-statement
AFLAGS          := -D__ASSEMBLY__

export AS AR CC CPP LD AWK PERL
export CPPFLAGS NOSTDINC_FLAGS LINUXINCLUDE OBJCOPYFLAGS LDFLAGS
export CFLAGS CFLAGS_KERNEL CFLAGS_MODULE
export AFLAGS AFLAGS_KERNEL AFLAGS_MODULE

# 需要访问的目录, 用来build vmlinux
init-y          := init/
drivers-y       := drivers/ sound/
net-y           := net/
libs-y          := lib/
core-y          := usr/


include $(srctree)/arch/$(ARCH)/Makefile

# vmlinux image - including updated kernel symbols
vmlinux: $(vmlinux-lds) $(vmlinux-init) $(vmlinux-main) $(kallsyms.o) FORCE
        $(call if_changed_rule,vmlinux__)

include/asm:
	@echo '  SYMLINK $@ -> include/asm-$(ARCH)'
	$(Q)if [ ! -d include ]; then mkdir -p include; fi;
	@ln -fsn asm-$(ARCH) $@	

core-y          += kernel/ mm/ fs/ ipc/ security/ crypto/

vmlinux-dirs    := $(patsubst %/,%,$(filter %/, $(init-y) $(init-m) \
                     $(core-y) $(core-m) $(drivers-y) $(drivers-m) \
                     $(net-y) $(net-m) $(libs-y) $(libs-m)))

vmlinux-alldirs := $(sort $(vmlinux-dirs) $(patsubst %/,%,$(filter %/, \
                     $(init-n) $(init-) \
                     $(core-n) $(core-) $(drivers-n) $(drivers-) \
                     $(net-n)  $(net-)  $(libs-n)    $(libs-))))

init-y          := $(patsubst %/, %/built-in.o, $(init-y))
core-y          := $(patsubst %/, %/built-in.o, $(core-y))
drivers-y       := $(patsubst %/, %/built-in.o, $(drivers-y))
net-y           := $(patsubst %/, %/built-in.o, $(net-y))
libs-y1         := $(patsubst %/, %/lib.a, $(libs-y))
libs-y2         := $(patsubst %/, %/built-in.o, $(libs-y))
libs-y          := $(libs-y1) $(libs-y2)

vmlinux-init := $(head-y) $(init-y)
vmlinux-main := $(core-y) $(libs-y) $(drivers-y) $(net-y)
vmlinux-all  := $(vmlinux-init) $(vmlinux-main)
vmlinux-lds  := arch/$(ARCH)/kernel/vmlinux.lds

# 链接 vmlinux的命令
# Rule to link vmlinux - also used during CONFIG_KALLSYMS
# May be overridden by arch/$(ARCH)/Makefile
quiet_cmd_vmlinux__ ?= LD      $@
      cmd_vmlinux__ ?= $(LD) $(LDFLAGS) $(LDFLAGS_vmlinux) -o $@ \
      -T $(vmlinux-lds) $(vmlinux-init)                          \
      --start-group $(vmlinux-main) --end-group                  \
      $(filter-out $(vmlinux-lds) $(vmlinux-init) $(vmlinux-main) FORCE ,$^)

define rule_vmlinux__
        $(if $(CONFIG_KALLSYMS),,+$(call cmd,vmlinux_version))

        $(call cmd,vmlinux__)
        $(Q)echo 'cmd_$@ := $(cmd_vmlinux__)' > $(@D)/.$(@F).cmd

        $(Q)$(if $($(quiet)cmd_sysmap),                 \
          echo '  $($(quiet)cmd_sysmap) System.map' &&) \
        $(cmd_sysmap) $@ System.map;                    \
        if [ $$? -ne 0 ]; then                          \
                rm -f $@;                               \
                /bin/false;                             \
        fi;
        $(verify_kallsyms)
endef


all: vmlinux

vmlinux: $(vmlinux-lds) $(vmlinux-init) $(vmlinux-main) $(kallsyms.o) FORCE
        $(call if_changed_rule,vmlinux__)

# Execute command and generate cmd file
if_changed = $(if $(strip $? \
                          $(filter-out $(cmd_$(1)),$(cmd_$@))\
                          $(filter-out $(cmd_$@),$(cmd_$(1)))),\
        @set -e; \
        $(if $($(quiet)cmd_$(1)),echo '  $(subst ','\'',$($(quiet)cmd_$(1)))';) \
        $(cmd_$(1)); \
        echo 'cmd_$@ := $(subst $$,$$$$,$(subst ','\'',$(cmd_$(1))))' > $(@D)/.$(@F).cmd)

# Usage: $(call if_changed_rule,foo)
# will check if $(cmd_foo) changed, or any of the prequisites changed,
# and if so will execute $(rule_foo)

if_changed_rule = $(if $(strip $? \
                               $(filter-out $(cmd_$(1)),$(cmd_$(@F)))\
                               $(filter-out $(cmd_$(@F)),$(cmd_$(1)))),\
                       $(Q)$(rule_$(1)))

# If quiet is set, only print short version of command
cmd = @$(if $($(quiet)cmd_$(1)),echo '  $($(quiet)cmd_$(1))' &&) $(cmd_$(1))

FORCE:
