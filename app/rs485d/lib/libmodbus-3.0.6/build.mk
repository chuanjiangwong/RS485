

#modbus := modbus-3.0.6

libs-y += lib$(modbus)

lib$(modbus)-objs-y := \
	src/modbus.c \
	src/modbus-data.c \
	src/modbus-rtu.c \
	src/modbus-tcp.c


lib$(modbus)-cflags-y := -I$(d)/src -I$(d)
