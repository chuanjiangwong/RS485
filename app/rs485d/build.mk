


PROJECT_MAJOR_NUM := 0
PROJECT_MINOR_NUM := 33
PROJECT_VERSION := $(PROJECT_MAJOR_NUM).$(PROJECT_MINOR_NUM)



############################################################
# Revision version number in software revision
ifneq ($(PROJECT_REV_NUM),)
	PROJECT_VERSION := $(PROJECT_VERSION).$(PROJECT_REV_NUM)
endif

############################################################
#add the exec target
prj_name := enno_rs485d

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
bacnet := bacnet-stack-0.8.3
modbus := libmodbus-3.0.6
list := list
message_queue := message_queue

subdir-y += lib/$(bacnet)
subdir-y += lib/$(modbus)
subdir-y += lib/$(list)
#subdir-y += lib/$(message_queue)


###############################################################
#project srouce file
$(prj_name)-objs-y := \
	src/main.c \
	src/adapter.c \
	src/service.c \
	src/timer_task.c \
	src/device.c \
	src/object.c \
	src/support.c \
	src/read_config.c \
	src/enumtxt.c \
	src/item_config.c

#for general
$(prj_name)-objs-y += \
	src/protocol/general/general.c \
	src/protocol/general/rs485.c


#for bacnet
$(prj_name)-objs-y += \
	src/protocol/bacnet/bacnet.c \
	src/protocol/bacnet/device-client.c \
	src/protocol/bacnet/handle_property.c \
	src/protocol/bacnet/read_property.c \
	src/protocol/bacnet/write_property.c


#for modbus
$(prj_name)-objs-y+= \
	src/protocol/modbus/modbus.c


#for device air condition
$(prj_name)-objs-y += \
	src/device/airCondition/york/york.c \
	src/device/airCondition/york/johnson_controls.c \
	src/device/airCondition/panasonnic/panasonnic.c \
	src/device/airCondition/daikin/DTA116A621.c \
	src/device/airCondition/hitachi/vrv_manager.c \
	src/device/airCondition/mistsubishi/mac_ccs_01m.c

#for device curtain
$(prj_name)-objs-y += \
	src/device/curtain/aoke/aoke.c \
	src/device/curtain/doya/doya.c

#for device fresh air
$(prj_name)-objs-y += \
	src/device/freshAir/loreley/loreley.c


###############################################################
#add the cflags

include_flags := \
	-I$(d)/lib/$(bacnet)/include \
	-I$(d)/lib/$(modbus)/src \
	-I$(d)/lib/$(list)/src \
	-I$(d)/lib/$(message_queue)


$(prj_name)-cflags-y := -DAPP_PROJECT_VERSION=$(PROJECT_VERSION) -I$(d)/include $(include_flags)



##############################################################
#add the lflags
$(prj_name)-lflags-y += -lc -lgcc -lpthread -lm


##############################################################
#add the debug cflags
ifeq ($(BUILD_MODE),debug)
$(prj_name)-cflags-y += -DAPPCONFIG_DEBUG_ENABLE
endif



