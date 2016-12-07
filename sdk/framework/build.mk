#Tha sdk module makefile


#######################################################
#add the target subdir

ifeq ($(CONFIG_ENABLE_FRAME_BUS), y)
subdir-y += bus
endif
