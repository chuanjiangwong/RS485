
#
# Description:
# ------------
# This file, config.mk contains rules/functions for:
#
# 	defconfig handling

################# Build Configuration

-include .b_config_name

#b-output-dir-y  := bin/$(basename $(b-conf-name))
b-output-dir-y := bin

#################################################
# BIN_DIR Handling

ifdef BIN_DIR
  override BIN_DIR := $(call b-abspath,$(BIN_DIR))
endif

BIN_DIR ?= $(b-output-dir-y)
#################################################

b-autoconf-file := $(b-output-dir-y)/autoconf.h
global-preinclude-y := $(b-autoconf-file)

# copy file1 to file2 if they are different
# arg1 : file1, arg2: file2
define cp
  $(AT)$(t_cp) -a $(1) $(2)
  @echo Updating configuration $(2)
endef
# if file2 exists, compare and copy if different. if file2 does not exist, copy.
define cp_if_changed
  $(if $(wildcard $(2)),$(if $(shell $(t_cmp) $(1) $(2)),$(call cp,$(1),$(2)),),$(call cp,$(1),$(2)))
endef

# cmd for make %_defconfig
# arg1 : defconfig name or file
define defconfig_cmd
  $(if $(wildcard $(1)),$(eval conf-file:=$(call b-abspath,$(1))),$(eval conf-file:=$(b-sdkconfig-dir-y)/$(1)))
  $(AT)$(t_kconf) -D $(conf-file) $(b-sdkconfig-dir-y)/Kconfig
  @echo "Configured using $(conf-file)"
  $(AT)$(t_printf) "b-conf-name := $(notdir $(1))" > .b_config_name
  $(eval b-conf-name := $(1))
endef
menuconfig_cmd = $(AT)$(t_mconf) $(b-sdkconfig-dir-y)/Kconfig
config_cmd = $(AT)$(t_kconf) $(b-sdkconfig-dir-y)/Kconfig
oldconfig_cmd = $(if $(wildcard .config),$(AT)$(t_kconf) -o $(b-sdkconfig-dir-y)/Kconfig,)

config_goals := config oldconfig menuconfig %_defconfig
no_config_goals := $(words $(filter $(config_goals),$(MAKECMDGOALS)))

ifeq ($(no_config_goals),2)
  $(error only one of $(config_goals) is allowed)
endif

# There are four types of targets (config oldconfig menuconfig %_defconfig) which
# can update .config file. In that case, we have to force the re-read of .config file.
# This is possible only if there is a recipe for .config file. Since all four targets
# make .config in different ways, we define .config recipe as per make config goal.
# Also the recipes must be executed irrespective of existence of .config file and hence
# .config is defined as a .PHONY target.

# .config recipe if no config goal is given
# It merely creates .config if not already present. Hence it is not a .PHONY target.
ifeq ($(no_config_goals),0)
.config: $(t_kconf)
	$(call defconfig_cmd,$(b-defconfig-y))
endif

# .config recipe for %_defconfig
ifneq ($(filter %_defconfig,$(MAKECMDGOALS)),)
%_defconfig: ;
.PHONY: .config
.config: $(t_kconf)
	$(call defconfig_cmd,$(filter %_defconfig,$(MAKECMDGOALS)))
endif

# .config recipe for menuconfig
ifneq ($(filter menuconfig,$(MAKECMDGOALS)),)
menuconfig: ;
.PHONY: .config
.config: $(t_mconf)
	$(menuconfig_cmd)
endif

# .config recipe for config
ifneq ($(filter config,$(MAKECMDGOALS)),)
config: ;
.PHONY: .config
.config: $(t_kconf)
	$(config_cmd)
endif

# .config recipe for oldconfig
ifneq ($(filter oldconfig,$(MAKECMDGOALS)),)
oldconfig: ;
.PHONY: .config
.config: $(t_kconf)
	$(oldconfig_cmd)
endif

$(b-autoconf-file): sdk/include/autoconf.h
	$(AT)$(t_mkdir) -p $(dir $@)
	$(call cp_if_changed,$<,$@)

sdk/include/autoconf.h: .config
	$(oldconfig_cmd)

pre-build-1: .config

#################
################# Tools Build

# Use recursive make for the tools for now
$(t_kconf):
	$(AT)$(t_mkdir) -p $(@D)
	$(AT)$(MAKE) -s -C sdk/config/kconfig CC=$(HOST_CC) TARGET=$(notdir $@) file_ext=$(file_ext)
	$(AT)$(t_cp) -a sdk/config/kconfig/$(@F) $(@D)
	$(AT)$(MAKE) -s -C sdk/config/kconfig CC=$(HOST_CC) TARGET=$(notdir $@) file_ext=$(file_ext) clean

$(t_mconf):
	$(AT)$(t_mkdir) -p $(@D)
	$(AT)$(MAKE) -s -C sdk/config/kconfig CC=$(HOST_CC) TARGET=$(notdir $@) file_ext=$(file_ext) all
	$(AT)$(t_cp) -a sdk/config/kconfig/$(@F) $(@D)
	$(AT)$(t_cp) -a sdk/config/kconfig/lxdialog/lxdialog $(@D)
	$(AT)$(MAKE) -s -C sdk/config/kconfig CC=$(HOST_CC) TARGET=$(notdir $@) file_ext=$(file_ext) clean



help: config-help

config-help:
	@echo ""
	@echo "Select SDK configuration"
	@echo "------------------------"
	@echo "   $$ make <defconfig>"
	@echo "   where defconfig can be any of the following:"
	@echo "      other deconfigs can be found at $(b-sdkconfig-dir-y)"
	@echo ""




-include .kconfig.d
