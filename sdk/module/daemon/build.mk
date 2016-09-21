

daemon := daemon

libs-y += lib$(daemon)

lib$(daemon)-objs-y := \
	daemon.c \
	signal.c


#lib$(daemon)-cflags-y += -I$(d)
