#Tha application makefile


#######################################################
#add the target subdir

ifeq ($(CONFIG_ENABLE_APP_RS485C),y)
subdir-y += rs485c
endif

ifeq ($(CONFIG_ENABLE_APP_RS485D),y)
subdir-y += rs485d
endif
