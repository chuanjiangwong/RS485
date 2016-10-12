#Tha application makefile


#######################################################
#add the target subdir
ifeq ($(CONFIG_ENABLE_APP_RS485D_MASTER),y)
subdir-y += rs485d-master
endif

ifeq ($(CONFIG_ENABLE_APP_TEST),y)
subdir-y += test
endif

subdir-y += rs485c
subdir-y += rs485d
