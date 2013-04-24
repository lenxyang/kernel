
src := $(obj)

.PHONY: __clean
__clean:

include $(obj)/Makefile
include $(srctree)/scripts/lib.mk

# 判断生成的文件放在 lib.a 当中，还是放在built-in.o当中
ifneq ($(strip $(lib-y) $(lib-m) $(lib-n) $(lib-)),)
lib-target := $(obj)/lib.a
endif

ifneq ($(strip $(obj-y) $(obj-m) $(obj-n) $(obj-) $(lib-target)),)
builtin-target := $(obj)/built-in.o
endif

__clean: $(builtin-target) $(lib-target) $(extra-y) \
	$(subdir-ym) $(always)
	@:

ifdef builtin-target
$(builtin-target): $(obj-y) FORCE
	$rm $@
endif

ifdef lib-target
$(lib-target): $(lib-y) FORCE
	rm $@
endif

.PHONY: $(subdir-ym)
$(subdir-ym):
	$(Q)$(MAKE) $(build)=$@ clean

.PHONY: FORCE
FORCE:
