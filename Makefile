
# Note:
# In case of only "make" command, "all" will be triggered with BUILD_MODE=debug.

# Default BUILD_MODE is debug
BUILD_MODE ?= debug

# The gcc
CROSS_COMPILE ?= arm-vfp-linux-gnu-


-include .config
include build/hapsdk.mk



###################################
# SDK modules





###################################
# APP management


ifndef APP
  subdir-y += app
else
  override APP := $(call b-abspath,$(APP))
  ifeq ($(wildcard $(APP)),)
    $(error "Please specify a valid APP path")
  endif
  subdir-y += $(APP)
endif




#include build/hapsdk_rules.mk
include build/rules.mk

###################################
ifneq ($(NOISY),1)
silent := -s
endif

############################################
-include prebuild.mk
############################################

### And now the targets
.PHONY: all build do_the_build post-build-1 pre-build-1

# all: The build dependencies are:
#                all -> post-build-1 -> build -> pre-build-1
# This allows developers to attach their pre/post scripts by adding
# dependencies to pre-build-1 and post-build-1
all: post-build-1

post-build-1: do_the_build
#	$(AT)echo " [compiler] $(CC)--> $(compiler-version)"
	
do_the_build: pre-build-1
	$(AT)$(MAKE) $(silent) build

# The entire build system is dependency driven. By default, build
# all the apps and boot2. This will build the other dependencies as
# required
build: $(b-exec-apps-y)

debug release:
	$(error "This target is no longer supported. Please pass BUILD_MODE option as: make BUILD_MODE=$@")

clean: 

###################################
help: build-help

build-help:
	@echo ""
	@echo "Compile SDK and sample application"
	@echo "-----------------------------------"
	@echo "   $$ make BUILD_MODE=<debug/release> <defconfig> [APP=<path/to/accessory>] all"
	@echo "   APP is relative path with your compile project"
	@echo ""
	@echo "   Default configuration:"
	@echo "      BUILD_MODE: debug"
	@echo "      defconfig: defconfig"
	@echo "      APP: test"
	@echo ""
	@echo "   for e.g. to build test demo,"
	@echo "   $$ make BUILD_MODE=debug defconfig APP=./app/test all"
	@echo ""
###################################

