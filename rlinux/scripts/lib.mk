comma   := ,
empty   :=
space   := $(empty) $(empty)

obj-m := $(filter-out $(obj-y),$(obj-m))
lib-y := $(filter-out $(obj-y), $(sort $(lib-y) $(lib-m)))

# 根据当前Makefile当中指定的子目录，生成subdir-y
__subdir-y      := $(patsubst %/,%,$(filter %/, $(obj-y)))
subdir-y        += $(__subdir-y)
obj-y           := $(patsubst %/, %/built-in.o, $(obj-y))


subdir-ym       := $(sort $(subdir-y) $(subdir-m))

# 为所有的文件增加目录，否则会发生找不到文件的错误
extra-y         := $(addprefix $(obj)/,$(extra-y))
obj-y           := $(addprefix $(obj)/,$(obj-y))
lib-y           := $(addprefix $(obj)/,$(lib-y))


# 根据当前的文件名和文件类型确定编译及选项
depfile = $(subst $(comma),_,$(@D)/.$(@F).d)
_c_flags       = $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$(*F).o)
_a_flags       = $(AFLAGS) $(EXTRA_AFLAGS) $(AFLAGS_$(*F).o)
_cpp_flags     = $(CPPFLAGS) $(EXTRA_CPPFLAGS) $(CPPFLAGS_$(@F))

__c_flags       = $(_c_flags)
__a_flags       = $(_a_flags)
__cpp_flags     = $(_cpp_flags)

c_flags        = -Wp,-MD,$(depfile) $(NOSTDINC_FLAGS) $(CPPFLAGS) \
                 $(__c_flags) $(modkern_cflags) \
                 $(basename_flags) $(modname_flags)

a_flags        = -Wp,-MD,$(depfile) $(NOSTDINC_FLAGS) $(CPPFLAGS) \
                 $(__a_flags) $(modkern_aflags)

cpp_flags      = -Wp,-MD,$(depfile) $(NOSTDINC_FLAGS) $(__cpp_flags)

# 链接选型
ld_flags       = $(LDFLAGS) $(EXTRA_LDFLAGS)
