
message_queue := message_queue

libs-y += lib$(message_queue)

lib$(message_queue)-objs-y := message_queue.c


lib$(message_queue)-cflags-y += -I$(d)
