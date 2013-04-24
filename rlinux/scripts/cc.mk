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


# read all saved command lines
targets := $(wildcard $(sort $(targets)))
cmd_files := $(wildcard .*.cmd $(foreach f,$(targets),$(dir $(f)).$(notdir $(f)).cmd))
