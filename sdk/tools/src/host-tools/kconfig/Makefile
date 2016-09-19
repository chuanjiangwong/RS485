# Copyright (C) 2008-2015, Marvell International Ltd.
# All Rights Reserved.

ifneq ($(NOISY),1)
	AT := @
else
	AT :=
endif

libncurses = -lncurses
OS := $(shell uname)

ifneq ($(OS), Darwin)
	LDFLAGS := -static -s
	libncurses := -lncursesw
endif

export libncurses
CFLAGS := -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer -DKBUILD_NO_NLS

ifneq ($(or $(findstring MINGW,$(OS)), $(findstring windows,$(OS))),)
CFLAGS += -I ./WinRegex/include
LDFLAGS += -L ./WinRegex/lib -lregex
endif

export CFLAGS

file_ext ?=

TARGET ?= conf$(file_ext) mconf$(file_ext)
CC ?= gcc

.PHONY: $(TARGET)

all: $(TARGET)

conf$(file_ext): conf.o kxgettext.o zconf.tab.o
	$(AT)$(CC) -o $@ conf.o zconf.tab.o $(LDFLAGS)

mconf$(file_ext): mconf.o kxgettext.o zconf.tab.o
	$(AT)$(CC) $(STATIC) -o $@ mconf.o zconf.tab.o
	$(AT)$(MAKE) -s -C lxdialog/ all CC=$(CC)

zconf.tab.o: zconf.tab.c lex.zconf.c zconf.hash.c

zconf.tab.c:
	$(AT)cat zconf.tab.c_shipped > zconf.tab.c

lex.zconf.c:
	$(AT)cat lex.zconf.c_shipped > lex.zconf.c

zconf.hash.c:
	$(AT)cat zconf.hash.c_shipped > zconf.hash.c

clean:
	$(AT)rm -f zconf.tab.c lex.zconf.c zconf.hash.c conf$(file_ext) mconf$(file_ext) $(wildcard *.o)
	$(AT)$(MAKE) -s -C lxdialog/ clean CC=$(CC)
