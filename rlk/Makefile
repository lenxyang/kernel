include scripts/env.mk

ARCH=x86
BUILD_UNITTEST=1

ifeq ($(BUILD_UNITTEST),0)
AR=$(CROSS_AR)
AS=$(CROSS_AS)
CC=$(CROSS_CC)
CXX=$(CROSS_CXX)
LD=$(CROSS_LD)
else
AR=$(HOST_AR)
AS=$(HOST_AS)
CC=$(HOST_CC)
CXX=$(HOST_CXX)
LD=$(HOST_CXX)
endif

ifeq ($(BUILD_UNITTEST),0)
CXXFLAGS = -c -g  -I. -Iinclude -Iarch/$(ARCH)/include -Wall \
	-march=i386 -mregparm=3 \
	-ffreestanding \
	-fno-toplevel-reorder \
	-fno-stack-protector \
	-mpreferred-stack-boundary=2 \
	-m32 

CFLAGS = $(CXXFLAGS) -std=gnu99 -nostdinc -fno-builtin \
	-mno-sse -mno-mmx -mno-sse2 -mno-3dnow \
	-DCONFIG_X86_32 \
	-Wstrict-prototypes
else
CXXFLAGS = -c -g  -I. -Iinclude -Iarch/$(ARCH)/include -Wall 

CFLAGS = $(CXXFLAGS) -DCONFIG_X86_32 -std=gnu99
endif

AFLAGS = $(CFLAGS) -D__ASSEMBLY__ -fomit-frame-pointer \

KERNEL_CFLAGS=$(CFLAGS) -D__KERNEL__


kernel_objs=out/arch/x86/kernel/e820.c.o

unittest_objs= out/unittest/unittests.cc.o 
#	out/unittest/arch/x86/kernel/e820.cc.o

GTEST_CXXFLAGS=$(CXXFLAGS) -Ithird_party/gtest-1.6.0/include \
	-Ithird_party/gmock-1.6.0/include
GTEST_LDFLAGS=-Lthird_party/gtest-1.6.0/lib/.libs/ \
	-Lthird_party/gmock-1.6.0/lib/.libs/\
	-lgtest -lgmock 

ifeq ($(BUILD_UNITTEST),0)
all: prepare vmlinux
else
all: prepare vmlinux test_cases
endif

vmlinux: ${kernel_objs}

test_cases: ${unittest_objs}
	$(LD) ${unittest_objs} $(GTEST_LDFLAGS) -o $@

out/arch/${ARCH}/kernel/e820.c.o: arch/${ARCH}/kernel/e820.c
	$(CC) $(KERNEL_CFLAGS) $? -o $@

out/unittest/arch/x86/kernel/e820.cc.o: unittest/arch/x86/kernel/e820.cc
	$(CXX) $(GTEST_CXXFLAGS) $? -o $@

out/unittest/unittests.cc.o: unittest/unittests.cc
	$(CXX) $(GTEST_CXXFLAGS) $? -o $@

prepare:
	mkdir -p out/arch/x86/kernel
	mkdir -p out/unittest/arch/x86/kernel
clean:
	rm -fr out
