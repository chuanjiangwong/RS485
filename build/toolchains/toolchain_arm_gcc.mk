

#CROSS_COMPILE :=

AS    := $(CROSS_COMPILE)gcc
CC    := $(CROSS_COMPILE)gcc
CPP   := $(CROSS_COMPILE)g++
LD    := $(CROSS_COMPILE)gcc
AR    := $(CROSS_COMPILE)ar
OBJCOPY := $(CROSS_COMPILE)objcopy
STRIP := $(CROSS_COMPILE)strip
HOST_CC := gcc


######### Common Linker File Handling
# This can be overriden from the apps


# Toolchain specific global cflags-y
global-cflags-y :=



compiler-version := $(shell $(CC) -dumpversion)


# FORCE option for execution
tc-force-opt := FORCE
# Compiler environment variable
tc-env := arm_gcc
tc-arm_gcc-env-y := y

tc-install-dir-y := for_arm_gcc
tc-install-dir-$(USE_EXTD_LIBC) := for_extd


# file include option
tc-include-opt := -include

# library strip options
tc-strip-opt := --strip-debug

## gcc specific extra linker flags
tc-lflags-y :=


global-cflags-y += \
		-g -Os \
		-fdata-sections \
		-ffunction-sections \
		-ffreestanding \
		-MMD -Wall \
		-fno-strict-aliasing \
		-fno-common


# cpp specific compiler and linker flags
ifeq ($(CONFIG_ENABLE_CPP_SUPPORT),y)
global-cpp-cflags-y := \
		-D_Bool=bool \
		-std=c++1y \
		--specs=nosys.specs

tc-cpp-lflags-y := \
		-Wl,--wrap,malloc -Wl,--wrap,free -Wl,--wrap,calloc \
		-Wl,--wrap,realloc -Wl,--wrap,printf
endif


global-c-cflags-y := -fgnu89-inline

##############################################
## ARM_GCC Tololchain specific rules

# The command for converting a .c/.cc/.cpp/.S/.s to .o
# arg1 the .c/.cc/.cpp/.S/.s filename
# arg2 the object filename
#
# This file has the default rule that maps an object file from the standard
# build output directory to the corresponding .c/.cc/.cpp/.S/.s file in the src directory
#
define b-cmd-c-to-o
$(CC) $(b-trgt-cflags-y) $(tc-include-opt) $(global-preinclude-y) $(b-trgt-c-cflags-y) $(global-cflags-y) $(global-c-cflags-y) -o $(2) -c $(1) -MMD
endef

define b-cmd-s-to-o
$(AS) $(b-trgt-cflags-y) $(tc-include-opt) $(global-preinclude-y) $(b-trgt-c-cflags-y) $(global-cflags-y) $(global-c-cflags-y) -o $(2) -c $(1) -MMD
endef

ifneq ($(CONFIG_ENABLE_CPP_SUPPORT),)
define b-cmd-cpp-to-o
$(CPP) $(b-trgt-cflags-y) $(tc-include-opt) $(global-preinclude-y) $(b-trgt-cpp-cflags-y) $(global-cflags-y) $(global-cpp-cflags-y) -o $(2) -c $(1) -MMD
endef
endif


#define b-cmd-axf
#$($(1)-LD) -o $(2) $($(1)-objs-y) $($(1)-lflags-y) -Xlinker --start-group $($(1)-prebuilt-libs-y) $($(1)-libs-paths-y) $(global-prebuilt-libs-y) -Xlinker --end-group -T $($(1)-linkerscript-y) -Xlinker -M -Xlinker -Map -Xlinker $(2:%.axf=%.map) $(tc-lflags-y) $(global-cflags-y)
#endef

define b-cmd-bin
$($(1)-LD) -o $(2) $($(1)-objs-y) $($(1)-lflags-y) $($(1)-prebuilt-libs-y) $($(1)-libs-paths-y) $(global-prebuilt-libs-y) $(global-cflags-y) -Wl,-Map,$(2).map
endef

define b-cmd-archive
$(AR) cru $(2) $($(1)-objs-y)
endef


