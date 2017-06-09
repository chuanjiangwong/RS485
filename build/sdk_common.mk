
# Description:
# ------------
# This file, sdk_common.mk contains following things:
#
# 	toolchain variables
# 	command variables used by build system

# .config should already have been included before this

# Step 0: Default target is all
all:

# Variables handled by specific toolchain file.

######## Default variables


####### Misc Handling ###########
# Following apps will be built only until axf
b-axf-only :=

# Step 1: Initialize the toolchain
include build/toolchains/toolchain.mk

# Step 2: Setup our own variables

global-cflags-y += \
	 -Isdk/src/incl/sdk                             \


######### Tools
# Step 3: Handle development host specific options
# devhost is towards the end, so it can override stuff defined from above
include build/host/devhost.mk

t_which ?= which
t_cp    ?= $(shell $(t_which) cp | tail -1)
t_mv    ?= $(shell $(t_which) mv | tail -1)
t_cmp   ?= $(shell $(t_which) cmp | tail -1)
t_mkdir ?= $(shell $(t_which) mkdir | tail -1)
t_cat   ?= $(shell $(t_which) cat | tail -1)
t_rm    ?= $(shell $(t_which) rm | tail -1)
t_printf ?= $(shell $(t_which) printf | tail -1)
t_python ?= $(shell $(t_which) python | tail -1)

##################################
#t_kconf  := sdk/tools/bin/$(os_dir)/conf$(file_ext)
#t_mconf  := sdk/tools/bin/$(os_dir)/mconf$(file_ext)

t_kconf  := bin/config/conf$(file_ext)
t_mconf  := bin/config/mconf$(file_ext)


