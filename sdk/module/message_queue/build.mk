
message_queue := message_queue

libs-y += lib$(message_queue)

lib$(message_queue)-objs-y := message_queue.c

ifneq ($(CC),gcc)
	lib$(message_queue)-objs-y += linux-atomic.c
endif



lib$(message_queue)-cflags-y += -I$(d)
