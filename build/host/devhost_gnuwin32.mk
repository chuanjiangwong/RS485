# Copyright (C) 2008-2016, Marvell International Ltd.
# All Rights Reserved.

os_dir := Windows
file_ext := .exe

t_mconf = mconf_not_on_gnuwin32
mconf_not_on_gnuwin32:
	@echo ""
	@echo "The 'menuconfig' option is not supported in GNUWin32"
	@echo "Please use 'make config' instead"
	@echo ""
	@false

# Function to resolve input path
define b-abspath
$(join $(filter %:,$(subst :,: ,$(1))),$(abspath $(filter-out %:,$(subst :,: ,$(subst \,/,$(1))))))
endef

t_bin_path := sdk/tools/bin/GnuWin32/bin

t_which := $(t_bin_path)/which.exe
t_uname	:= $(t_bin_path)/uname.exe

t_cp	:= $(t_bin_path)/cp.exe
t_mv	:= $(t_bin_path)/mv.exe
t_cmp	:= $(t_bin_path)/cmp.exe
t_mkdir := $(t_bin_path)/mkdir.exe
t_cat	:= $(t_bin_path)/cat.exe
t_rm	:= $(t_bin_path)/rm.exe
t_printf := $(t_bin_path)/printf.exe
t_python := $(shell $(t_which) python)
