# Copyright (C) 2008-2015, Marvell International Ltd.
# All Rights Reserved.

# Makefile to build lxdialog package
#

ifneq ($(NOISY),1)
	AT := @
else
	AT :=
endif

check-lxdialog  := $(srctree)/$(src)/check-lxdialog.sh

# Use reursively expanded variables so we do not call gcc unless
# we really need to do so. (Do not call gcc as part of make mrproper)
CFLAGS += -I/usr/include/ncurses -DCURSES_LOC="<ncurses.h>" -DLOCALE

all: lxdialog

lxdialog: checklist.o menubox.o textbox.o yesno.o inputbox.o \
		 util.o lxdialog.o msgbox.o
	$(AT)$(CC) -o $@ $^ $(libncurses)

clean:
	$(AT)rm -f lxdialog $(wildcard *.o)
