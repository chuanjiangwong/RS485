

bitmap := wbitmap

libs-y += lib$(bitmap)

lib$(bitmap)-objs-y := \
	bitmap.c \
	it_table.c \


