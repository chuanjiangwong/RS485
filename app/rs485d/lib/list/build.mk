
#list := list

libs-y += lib$(list)

lib$(list)-objs-y := \
	src/list_iterator.c \
	src/list_node.c \
	src/list.c


lib$(list)-cflags-y += -I$(d)/src
