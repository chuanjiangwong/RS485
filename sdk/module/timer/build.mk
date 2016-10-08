

timer := wtimer

libs-y += lib$(timer)

lib$(timer)-objs-y := \
	timer.c

