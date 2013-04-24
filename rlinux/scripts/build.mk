#=============================================
# Building
#=============================================


src := $(obj)

.PHONY: __build
__build:

include $(obj)/Makefile
include $(srctree)/scripts/lib.mk

# 判断生成的文件放在 lib.a 当中，还是放在built-in.o当中
ifneq ($(strip $(lib-y) $(lib-m) $(lib-n) $(lib-)),)
lib-target := $(obj)/lib.a
endif

ifneq ($(strip $(obj-y) $(obj-m) $(obj-n) $(obj-) $(lib-target)),)
builtin-target := $(obj)/built-in.o
endif

# __build: $(if $(KBUILD_BUILTIN),$(builtin-target) $(lib-target) $(extra-y)) \
#         $(if $(KBUILD_MODULES),$(obj-m)) \
#         $(subdir-ym) $(always)
#         @:

# 暂时仅仅支持 built-in，不支持module的Makefile
__build: $(builtin-target) $(lib-target) $(extra-y) \
         $(subdir-ym) $(always)
         @:


# 将生成的 .o 文件连接成一个built-in.o文件
# 这个文件在每一个目录下都有(前提是它有需要生成的代码文件)
ifdef builtin-target
# 如果obj-y为空，那么创建空的built-in
cmd_link_o_target = $(if $(strip $(obj-y)),\
                      $(LD) $(ld_flags) -r -o $@ $(filter $(obj-y), $^),\
                      rm -f $@; $(AR) rcs $@)
$(builtin-target): $(obj-y) FORCE
	$(cmd_link_o_target)

targets += $(builtin-target)
endif

# 如果生成的是lib.a
ifdef lib-target
$(lib-target): $(lib-y) FORCE
	rm -f $@
	$(AR) $(EXTRA_ARFLAGS) rcs $@ $(lib-y)

targets += $(lib-target)
endif

targets += $(builtin-target)

# 隐式规则列表
%.o: %.c FORCE
	$(CC) $(c_flags) -c -o $@ $<

%.s: %.S FORCE
	$(CPP) $(a_flags)   -o $@ $<

%.o: %.S FORCE
	$(CC) -Da $(a_flags) -c -o $@ $<

%.lds: %.lds.S FORCE
	$(CPP) $(cpp_flags) -D__ASSEMBLY__ -o $@ $<

# 从当前的$(obj)展开继续向下make
.PHONY: $(subdir-ym)
$(subdir-ym):
	$(Q)$(MAKE) $(build)=$@

.PHONY: FORCE
FORCE:
