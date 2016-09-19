# Copyright (C) 2014-2015 Marvell International Ltd.
# All Rights Reserved.


PROJECT_MAJOR_NUM := 1
PROJECT_MINOR_NUM := 0
PROJECT_VERSION := $(PROJECT_MAJOR_NUM).$(PROJECT_MINOR_NUM)



############################################################
# Revision version number in software revision
ifneq ($(PROJECT_REV_NUM),)
	PROJECT_VERSION := $(PROJECT_VERSION).$(PROJECT_REV_NUM)
endif

############################################################
#add the exec target
prj_name := enno_rs485c

exec-y += $(prj_name)

############################################################
# Info in software revision
#SW_VER_INFO :=
ifneq ($(PROJECT_VER_INFO),)
	PROJECT_VERSION := $(PROJECT_VERSION)\;$(PROJECT_VER_INFO)
endif

PROJECT_VERSION := \"$(PROJECT_VERSION)\"


###########################################################
#add the lib



###############################################################
#project srouce file
$(prj_name)-objs-y := \
	src/rs485_api.c \
	src/main.c

###############################################################
#add the cflags


$(prj_name)-cflags-y := -DAPP_PROJECT_VERSION=$(PROJECT_VERSION) -I$(d)/include



##############################################################
#add the lflags
$(prj_name)-lflags-y += -lc -lgcc -lpthread -lm


##############################################################
#add the debug cflags
ifeq ($(BUILD_MODE),debug)
$(prj_name)-cflags-y += -DAPPCONFIG_DEBUG_ENABLE
endif



