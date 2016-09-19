

AS    := iasmarm.exe
CC    := iccarm.exe
CPP   := iccarm.exe
LD    := ilinkarm.exe
AR    := iarchive.exe
OBJCOPY :=
STRIP :=
HOST_CC := gcc

python_script_dir := sdk/tools/ide/iar
template_dir := $(python_script_dir)/templates
python_script_list := create_workspace update_ewp execute_command

template_exec_ewp := $(template_dir)/template_exec.ewp
template_lib_ewp := $(template_dir)/template_lib.ewp
template_workspace_eww := $(template_dir)/template_workspace.eww

define define_python_scripts
  t_$(1) := $(python_script_dir)/$(1).py
endef

$(foreach p,$(python_script_list),$(eval $(call define_python_scripts,$(p))))

######### Common Linker File Handling
# This can be overriden from the apps
global-linkerscript-y := build/toolchains/iar/$(arch_name-y).icf

# Toolchain specific global cflags-y
global-cflags-y :=

######### XIP Handling
ifeq ($(XIP), 1)
  global-linkerscript-y := build/toolchains/iar/$(arch_name-y)-xip.icf
endif

# Compiler environment variable
tc-env := iar
tc-iar-env-y := y

tc-install-dir-y := for_extd
USE_EXTD_LIBC := y

# =========================
# NOTE
# For IAR, LD is used to do AXF build
# Hence there are no equivalent flags in
# linking options in IAR as of now
# =========================

# file include option
tc-include-opt := --preinclude

# library strip options
tc-strip-opt :=

# iar specific extra linker flags
tc-lflags-y := \
		--redirect _Printf=_PrintfSmall \
		--redirect _Scanf=_ScanfSmall \
		--log libraries,initialization,modules,redirects,sections,veneers,unused_fragments \
		--entry Reset_IRQHandler

global-cflags-y += \
		-I. --endian little

tc-cflags-y := \
		--debug \
		-Ohz \
		--char_is_signed \
		-e \
		--dlib_config build/toolchains/iar/DLib_Config_Normal.h

# silent and no warnings flags added
# in toolchain linker and compiler flags
ifneq ($(NOISY),1)
  tc-lflags-y += \
	--silent \
	--no_warnings

  tc-cflags-y += \
	--silent \
	--no_warnings
endif

global-cflags-$(tc-cortex-m3-y) += --cpu Cortex-M3
global-cflags-$(tc-cortex-m4-y) += --cpu Cortex-M4

##############################################
## IAR Tololchain specific rules

# The command for converting a .c/.cc/.cpp/.S/.s to .o
# arg1 the .c/.cc/.cpp/.S/.s filename
# arg2 the object filename
#
# This file has the default rule that maps an object file from the standard
# build output directory to the corresponding .c/.cc/.cpp/.S/.s file in the src directory
#
define b-cmd-c-to-o
$(CC) $(b-trgt-cflags-y) $(tc-include-opt) $(global-preinclude-y) $(tc-cflags-y) $(global-cflags-y) $(global-c-cflags-y) -o $(2) $(1) --dependencies=m ${2:.o=.d}
endef

define b-cmd-s-to-o
$(AS) $(b-trgt-cflags-y) $(global-cflags-y) -S $(global-c-cflags-y) -o $(2) $(1)
endef

ifneq ($(CONFIG_ENABLE_CPP_SUPPORT),)
define b-cmd-cpp-to-o
$(CPP) $(b-trgt-cflags-y) $(tc-include-opt) $(global-preinclude-y) $(tc-cflags-y) $(global-cflags-y) $(global-cpp-cflags-y) -o $(2) $(1) --dependencies=m ${2:.o=.d}
endef
endif

define b-cmd-axf
$($(1)-LD) -o $(2) $($(1)-objs-y) $($(1)-lflags-y) $($(1)-prebuilt-libs-y) $($(1)-libs-paths-y) $(global-prebuilt-libs-y) --config $($(1)-linkerscript-y) --map $(2:%.axf=%.map) $(tc-lflags-y) --log_file $($(1)-output-dir-y)/$(1).iar.log
endef

define b-cmd-archive
$(AR) $(2) $($(1)-objs-y)
endef

define b-cmd-exec-ewp
  $(AT)$(t_python) $(t_create_workspace) \
  --template_file $(template_exec_ewp) \
  create_ewp \
  --exec_ewp \
  --src_file $($(1)-objs-all-y) \
  --lib_file $($(1)-libs-paths-y) \
  --prebuilt_lib_file $($(1)-prebuilt-libs-y) $(global-prebuilt-libs-y) \
  --config $($(1)-linkerscript-y) \
  --preinclude $(global-preinclude-y) \
  --output_bin_name $(1) \
  --output_ewp_name $($(1)-dir-y)/$(1).ewp \
  --output_bin_path $($(1)-output-dir-y) \
  $(global-cflags-y) $(global-c-cflags-y) $($(1)-cflags-y)

  $(AT)$(t_python) $(t_update_ewp) \
  --execution_script $(t_execute_command) \
  $(if $($(1)-ftfs-y), \
  --create_ftfs \
  --ftfs_api $(foreach p,$($(1)-ftfs-y),$($(basename $(p))-ftfs-api-y)) \
  --ftfs_output_file $(foreach p,$($(1)-ftfs-y),$($(1)-output-dir-y)/$(p)) \
  --ftfs_dir $(foreach p,$($(1)-ftfs-y),$($(basename $(p))-ftfs-dir-y)) \
  --ftfs_tool $(t_mkftfs) \
  ,) \
  --create_bin \
  --bin_tool $(t_axf2fw) \
  --bin_src_file $($(1)-output-dir-y)/$(1).axf \
  --bin_output_file $($(1)-output-dir-y)/$(1).bin \
  --postbuild \
  --ewp_src_file $($(1)-dir-y)/$(1).ewp
endef

define b-cmd-lib-ewp
  $(AT)$(t_python) $(t_create_workspace) \
  --template_file $(template_lib_ewp) \
  create_ewp \
  --lib_ewp \
  --src_file $($(1)-objs-all-y) \
  --preinclude $(global-preinclude-y) \
  --output_bin_name $(1) \
  --output_ewp_name $($(1)-dir-y)/$(1).ewp \
  --output_bin_path $(b-libs-output-dir-y) \
  $(global-cflags-y) $(global-c-cflags-y) $($(1)-cflags-y)
endef

define b-cmd-eww
  $(AT)$(t_python) $(t_create_workspace) \
  --template_file $(template_workspace_eww) \
  create_eww \
  --debug_config \
  --output_eww_name $(1) \
  --ewp_file $(foreach t,$($(2)),$($(t)-dir-y)/$(t).ewp)
endef

##############################################
subdir-y += sdk/src/iar

# Return error if CONFIG_ENABLE_CPP_SUPPORT is y with toolchain is iar.
ifeq ($(CONFIG_ENABLE_CPP_SUPPORT),y)
  $(error "We do not support IAR toolchain with CPP Support enabled")
endif

##############################################
# Set value for axf2firmware
t_axf2fw := sdk/tools/bin/Windows/iar-axf2firmware.exe
