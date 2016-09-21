


PROJECT_MAJOR_NUM := 1
PROJECT_MINOR_NUM := 0
PROJECT_VERSION := $(PROJECT_MAJOR_NUM).$(PROJECT_MINOR_NUM)




# Revision version number in software revision
ifneq ($(PROJECT_REV_NUM),)
	PROJECT_VERSION := $(PROJECT_VERSION).$(PROJECT_REV_NUM)
endif

prj_name := test-$(PROJECT_VERSION)

exec-y += $(prj_name)

# Info in software revision
#SW_VER_INFO :=
ifneq ($(PROJECT_VER_INFO),)
	PROJECT_VERSION := $(PROJECT_VERSION)\;$(PROJECT_VER_INFO)
endif

PROJECT_VERSION := \"$(PROJECT_VERSION)\"

$(prj_name)-objs-y := \
		test.c

$(prj_name)-cflags-y := -DAPP_PROJECT_VERSION=$(PROJECT_VERSION) #-I$(d)/src

ifeq ($(BUILD_MODE),debug)
$(prj_name)-cflags-y += -DAPPCONFIG_DEBUG_ENABLE
endif


#add the lflags
$(prj_name)-lflags-y += -lc -lgcc -lpthread -lm