

ringbuffer := ringbuffer

libs-y += lib$(ringbuffer)

lib$(ringbuffer)-objs-y := \
	ringbuf.c

