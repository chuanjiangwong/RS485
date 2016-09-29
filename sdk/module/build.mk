#Tha sdk module makefile


#######################################################
#add the target subdir
ifeq ($(CONFIG_ENABLE_MODULE_DAEMON),y)
subdir-y += daemon
endif

ifeq ($(CONFIG_ENABLE_MODULE_MESSAGE_QUEUE),y)
subdir-y += message_queue
endif

#subdir-y += server

ifeq ($(CONFIG_ENABLE_MODULE_TIMER),y)
subdir-y += timer
endif

ifeq ($(CONFIG_ENABLE_MODULE_RINGBUFFER),y)
subdir-y += ringbuffer
endif

ifeq ($(CONFIG_ENABLE_MODULE_WLIST), y)
subdir-y += list
endif