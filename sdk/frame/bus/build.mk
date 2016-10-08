

frame_bus := frame_bus

libs-y += lib$(frame_bus)

lib$(frame_bus)-objs-y := \
	bus.c \
	device_driver.c \
	device.c


lib$(frame_bus)-cflags-y += -I$(d)

