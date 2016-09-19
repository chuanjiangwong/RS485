# Copyright (C) 2008-2016, Marvell International Ltd.
# All Rights Reserved.

os_dir := Windows
file_ext := .exe

make_version_check-$(tc-arm_gcc-env-y) := $(shell expr $(MAKE_VERSION) \< 4.1)

ifeq ($(make_version_check-y),1)
  $(error "Please use make version 4.1 or higher")
endif

######################################
# Cygwin python check
cygwin_python_used := $(shell which python | tail -1)

define cygwin_python_error

*** It seems that you are using Cygwin Python. Please install Python separately for Windows.
*** For more details please refer to developement host setup.

endef

ifeq ($(cygwin_python_used),/usr/bin/python)
  $(error $(cygwin_python_error))
endif
######################################

$(t_mconf): mconf_not_on_cygwin
mconf_not_on_cygwin:
	@echo ""
	@echo "The 'menuconfig' option is not supported in Cygwin"
	@echo "Please use 'make config' instead"
	@echo ""
	@false

# This is used to replace ":" in drive letter
# This will be handy in resolving issues in rules/targets
escape_dir_name := _wmdrive

# Alphabet to be escaped
escape_let := :

# List of  Drive letters
drive-list-y := C D E F

# Function to resolve input path
define b-abspath
$(shell cygpath -m $(1))
endef
