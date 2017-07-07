
#
# Description:
# -------------
# This file, rules.mk contains rules/functions for:
#
#	Object file creation from source files, where source
#	files are of following types:
#
# 	.c/.S/.s file 	--> .o file 	(b-cmd-c-to-o)
# 	.cc/.cpp file 	--> .o file 	(b-cmd-cpp-to-o)
#
# 	Directory creation 		(create_dir)
# 	Subdirectory inclusions 	(inc_mak)
# 	Library creation 		(create_lib)
# 	bin creation			(create_bin)
# 	Obj creation 			(create_obj)

##################### Operating Variables
# The list of all the libraries in the system
b-libs-y :=
b-libs-paths-y :=
# The list of all the programs in the system
b-exec-y :=
b-exec-cpp-y :=
b-exec-apps-y :=
# The list of dependencies
b-deps-y :=
# The output directory for this configuration, this is deferred on-purpose
b-output-dir-y ?=
# The list of object directories that should be created
b-object-dir-y :=
# The list of libraries that are pre-built somewhere outside
global-prebuilt-libs-y :=
# This is path of output directory for libraries
b-libs-output-dir-y ?= $(b-output-dir-y)/libs
# This is path of output directory for intermediate files (.d,.o,.cmd)
b-objs-output-dir-y ?= $(b-output-dir-y)/objs

# This creates a rule for directory creation
# Use directories as order-only (|) prerequisites
define create_dir
$(1):
	$$(AT)$$(t_mkdir) -p $$@
endef

#------------------------------------------------#
# Check if both arguments has same arguments. Result is empty string if equal.
arg-check = $(strip $(filter-out $(1),$(subst ",\",$(cmd_$@))) \
                    $(filter-out $(subst ",\",$(cmd_$@)),$(1)))

# Find any prerequisites that is newer than target or that does not exist.
# PHONY targets skipped in both cases.
any-prereq = $(filter-out $(PHONY),$?) $(filter-out $(PHONY) $(wildcard $^),$^)

# execute command and store the command line in $@.cmd file
define cmd_save
  @printf "cmd_$@ := %s\n" "$(1)"  > $@.cmd
endef

# if_changed - execute command $(2) if any prerequisite is newer than
#              target, or command line $(1) has changed and save command in
#              command file

if_changed_0 = $(if $(strip $(any-prereq) $(call arg-check,$(call $(1),$<,$@))),$(call $(2),$<,$@),)

if_changed_1 = $(if $(strip $(any-prereq) $(call arg-check,$(call $(1),$(3),$@))), $(call $(2),$(3),$@),)

if_changed_3 = $(if $(strip $(any-prereq) $(call arg-check,$(call $(1),$(3),$(4),$(5)))),$(call $(2),$(3),$(4),$(5)),)

PHONY += $(tc-force-opt)
#------------------------------------------------#
##################### Subdirectory Inclusions
# The wildcard, let's users pass options like
# subdir-y += mydir/Makefile.ext
#    If the Makefile has a different name, that is facilitated by the 'if block'.
#    The standard case of handling 'directory' names is facilitated by the 'else block'.
#
real-subdir-y :=
define inc_mak
 ifeq ($(wildcard $(1)/build.mk),)
     d := $(patsubst %/,%,$(dir $(1)))
     include $(1)
 else
     d := $(patsubst %/,%,$(1))
     include $(1)/build.mk
 endif
 # For recursive subdir-y, append $(d)
 $$(foreach t,$$(subdir-y),$$(eval real-subdir-y += $$(patsubst %/,%,$$(d)/$$(t))))
 subdir-y :=
 include build/post-subdir.mk
endef

# Include rules.mk from all the subdirectories and process them
ifneq ($(subdir-y),)

# Default level subdir-y inclusion (level 1)
b-subdir-y := $(subdir-y)
subdir-y :=

define rm_dups
unique :=
$(foreach s,$($(1)),$(if $(filter $(s),$(unique)),,$(eval unique += $(s))))
$(1) := $(unique)
endef

$(eval $(call rm_dups,b-subdir-y))
$(foreach dir,$(b-subdir-y),$(eval $(call inc_mak,$(dir))))

# inclusion (level 2)
b-subdir-y := $(real-subdir-y)
real-subdir-y :=
$(eval $(call rm_dups,b-subdir-y))
$(foreach dir,$(b-subdir-y),$(eval $(call inc_mak,$(dir))))

# inclusion (level 3)
b-subdir-y := $(real-subdir-y)
real-subdir-y :=
$(eval $(call rm_dups,b-subdir-y))
$(foreach dir,$(b-subdir-y),$(eval $(call inc_mak,$(dir))))
endif

# Give warning if level of subdir-recursion is greater than level 3
ifneq ($(subdir-y),)
$(warning "[Warning] You have more levels of subdir-y recursion.")
endif

include build/refine.mk

.SUFFIXES:

##################### Object (.o) Creation

# All the prog-objs-y have prog-cflags-y as the b-trgt-cflags-y variable
$(foreach l,$(b-exec-y),$(eval $($(l)-objs-y): b-trgt-cflags-y := $($(l)-cflags-y)))

# exec specific  C flags
$(foreach l,$(b-exec-y),$(eval $($(l)-objs-y): b-trgt-c-cflags-y := $($(l)-c-cflags-y)))

# exec specific  CPP flags
$(foreach l,$(b-exec-y),$(eval $($(l)-objs-y): b-trgt-cpp-cflags-y := $($(l)-cpp-cflags-y)))
# All the lib-objs-y have lib-cflags-y as the b-trgt-cflags-y
# variable. This allows configuration flags specific only to certain
# libraries/programs. Only that may be dangerous.
$(foreach l,$(b-libs-y),$(eval $($(l)-objs-y): b-trgt-cflags-y := $($(l)-cflags-y)))

# library specific  C flags
$(foreach l,$(b-libs-y),$(eval $($(l)-objs-y): b-trgt-c-cflags-y := $($(l)-c-cflags-y)))

# library specific CPP flags
$(foreach l,$(b-libs-y),$(eval $($(l)-objs-y): b-trgt-cpp-cflags-y := $($(l)-cpp-cflags-y)))

# Rules for output directory creation for all the objects
$(foreach d,$(sort $(b-object-dir-y)),$(eval $(call create_dir,$(d))))


# Rule for creating an object file
# strip off the $(b-output-dir-y) from the .c/.cc/.cpp/.S/.s files pathname

# Following dependency rule only checks existence of object file directory, not its timestamp
$(foreach l,$(b-libs-y) $(b-exec-y),$(foreach o,$($(l)-objs-y),$(eval $(o): | $(dir $(o)))))

define create_obj
$$(b-objs-output-dir-y)/$(1)%.o: $(2)%.c $$(b-autoconf-file) $(tc-force-opt)
	$$(call if_changed_0,b-cmd-c-to-o,b-gen-c-to-o)

$$(b-objs-output-dir-y)/$(1)%.o: $(2)%.cc $$(b-autoconf-file) $(tc-force-opt)
	$$(call if_changed_0,b-cmd-cpp-to-o,b-gen-cpp-to-o)

$$(b-objs-output-dir-y)/$(1)%.o: $(2)%.cpp $$(b-autoconf-file) $(tc-force-opt)
	$$(call if_changed_0,b-cmd-cpp-to-o,b-gen-cpp-to-o)

$$(b-objs-output-dir-y)/$(1)%.o: $(2)%.S $$(b-autoconf-file) $(tc-force-opt)
	$$(call if_changed_0,b-cmd-s-to-o,b-gen-s-to-o)

$$(b-objs-output-dir-y)/$(1)%.o: $(2)%.s $$(b-autoconf-file) $(tc-force-opt)
	$$(call if_changed_0,b-cmd-s-to-o,b-gen-s-to-o)
endef

$(eval $(call create_obj))

ifdef drive-list-y
$(foreach drv,$(drive-list-y),$(eval $(call create_obj,$(drv)$(escape_dir_name)/,$(drv)$(escape_let)/)))
endif

-include $(b-deps-y)

##################### Library (.a) Creation

# Rule for creating a library
# Given liba
#  - create $(b-output-dir-y)/liba.a
#  - from $(liba-objs-y)
#  - create a target liba.a that can be used to build the lib

$(eval $(call create_dir,$(b-libs-output-dir-y)))

define create_lib
  $(1).a: $(b-libs-output-dir-y)/$(1).a

  # Following dependency rule only checks existence of $(b-libs-output-dir-y), not its timestamp
  $(b-libs-output-dir-y)/$(1).a: | $(b-libs-output-dir-y)

  $(b-libs-output-dir-y)/$(1).a: $$($(1)-objs-y) $(tc-force-opt)
	$$(call if_changed_1,b-cmd-archive,b-gen-archive,$(1))

  .PHONY: $(1).a.clean
  clean: $(1).a.clean
  $(1).a.clean:
	@echo " [clean] $(1)"
	$$(AT) $(t_rm) -f $(b-libs-output-dir-y)/$(1).a $$($(1)-objs-y) $$($(1)-objs-y:.o=.d) $$($(1)-objs-y:.o=.o.cmd) $(b-libs-output-dir-y)/$(1).a.cmd
endef

$(foreach l,$(b-libs-y), $(eval $(call create_lib,$(l))))

.PHONY: all-libs
all-libs: $(foreach l,$(b-libs-y),$(l).a)

##################### Program (.axf) Creation
# Rule for creating a program
# Given myprog
#  - create $(b-output-dir-y)/myprog
#  - from $(myprog-objs-y)
#  - add dependency on all the libraries, linker scripts
#  - create a target app_name.app which can be used to build the app

$(foreach d,$(sort $(foreach p,$(b-exec-y),$($(p)-output-dir-y))),$(eval $(call create_dir,$(d))))

define b-gen-bin
  @echo "\033[0;31;1m [bin] $(call b-abspath,$(2)) \033[0m"
  $(AT)$(call b-cmd-bin,$(1),$(2))
  @echo " [map] $(call b-abspath,$(2:%.bin=%.map))"
  $(call cmd_save,$(call b-cmd-bin,$(1),$(2)))
endef

define b-gen-archive
  $(AT)$(t_rm) -f $(2)
  @echo " [ar] $(call b-abspath,$(2))"
  $(AT)$(call b-cmd-archive,$(1),$(2))
  $(call cmd_save,$(call b-cmd-archive,$(1),$(2)))
endef

define b-gen-c-to-o
  @echo " [cc] $(1)"
  $(AT)$(call b-cmd-c-to-o,$(1),$(2))
  $(call cmd_save,$(call b-cmd-c-to-o,$(1),$(2)))
endef

define b-gen-s-to-o
  @echo " [cc] $(1)"
  $(AT)$(call b-cmd-s-to-o,$(1),$(2))
  $(call cmd_save,$(call b-cmd-s-to-o,$(1),$(2)))
endef

define b-gen-cpp-to-o
  @echo " [cpp] $(1)"
  $(AT)$(call b-cmd-cpp-to-o,$(1),$(2))
  $(call cmd_save,$(call b-cmd-cpp-to-o,$(1),$(2)))
endef

define create_bin
  $(1).app: $(b-output-dir-y)/$(1).bin
# Following dependency rule only checks existence of $(1)-output-dir-y, not its timestamp
  $(b-output-dir-y)/$(1).bin: | $(b-output-dir-y)

  $(b-output-dir-y)/$(1).bin: $$($(1)-objs-y) $$($(1)-libs-paths-y) $$(global-prebuilt-libs-y) $(tc-force-opt)
	$$(call if_changed_1,b-cmd-bin,b-gen-bin,$(1))

  .PHONY: $(1).app.clean
  clean: $(1).app.clean
  $(1).app.clean:
	@echo " [clean] $(1)"
	$$(AT)$(t_rm) -f $($(1)-output-dir-y)/$(1).bin $($(1)-output-dir-y)/$(1).map $$($(1)-objs-y) $$($(1)-objs-y:.o=.d)  $$($(1)-objs-y:.o=.o.cmd) $($(1)-output-dir-y)/$(1).cmd
endef

$(foreach p,$(b-exec-y), $(eval $(call create_bin,$(p))))

# Include app_postprocess function from app makefile
$(foreach p,$(b-exec-y), $(eval $(call $(p)_postprocess)))

##################### Miscellaneous handling

# Rule for clean
#
clean:

# Rule for NOISY Output
ifneq ($(NOISY),1)
  AT := @
endif

FORCE:
