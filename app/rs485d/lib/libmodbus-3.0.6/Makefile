# Copyright (C) 2008-2014 Marvell International Ltd.
# All Rights Reserved.


# Directory for output files (lst, obj, dep, elf, sym, map, hex, bin etc.).
OUTDIR = build
# define the source file directory
SRC_DIR = src
# define the current dir
CURRENT_DIR = .


# define the include dir
INC_DIR = -I$(SRC_DIR)
# define the lib include
LIB_INC_DIR = -I$(CURRENT_DIR)
# others include dir
EXTEND_CFLAGS =


# lib path
LINK_LIB_DIR =$(PRJROOT_PATH)/lib/lib
# lib LFLAGS
LINK_LIB =-llist,-lmessage_queue,-lbacnet,-lmodbus
# system lib flags
SYSTEM_LIB =-lc,-lgcc,-lrt,-lm,-lpthread


#define the target
TARGET = libmodbus.a



SRCS += $(SRC_DIR)/modbus-data.c \
		$(SRC_DIR)/modbus-rtu.c \
		$(SRC_DIR)/modbus-tcp.c \
		$(SRC_DIR)/modbus.c \


ifeq ($(NOISY),1)
AT=
else
AT=@
endif

# Debugging format.
#DEBUG = stabs
#DEBUG = dwarf-2
DEBUG= gdb

# Optimization level, can be [0, 1, 2, 3, s].
# 0 = turn off optimization. s = optimize for size.
# (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
#OPT = s
#OPT = 2
#OPT = 3
#OPT = 0
OPT = s


# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CSTANDARD = gnu99


# Compiler flags definition.
CFLAGS+=-g$(DEBUG) \
		-O$(OPT) \
		-std=$(CSTANDARD) \
		$(INC_DIR) \
		$(LIB_INC_DIR) \
		$(EXTEND_CFLAGS) \
		-D inline= \
		-MMD \
		-Wall -Wno-unused-function \


# Link flags definition
LFLAGS += -Wl,$(SYSTEM_LIB),-L$(LINK_LIB_DIR),$(LINK_LIB)

SOURCE+=$(SRCS)

# List of all source files without directory and file-extension.
ALLSRCBASE = $(notdir $(basename $(SOURCE)))


# List of all objects files.
OBJS = $(addprefix $(OUTDIR)/, $(addsuffix .o, $(ALLSRCBASE)))
D_LIST = $(wildcard $(addprefix $(PRJROOT_PATH)/$(OUTDIR)/, $(addsuffix .dep, $(ALLSRCBASE))))


# Define Messages.
# English
MSG_BEGIN = -------- begin --------
MSG_END = --------  end  --------

.PHONY : all clean begin gccversion end $(OUTDIR)/$(TARGET) install

# Rules definition. ***********************************************************
all: begin $(OUTDIR)/$(TARGET) end


#add the depened
ifneq ($(D_LIST),)
  include $(D_LIST)
endif



#get the obj file
SRC_BASE = $(notdir $(basename $(SRCS)))
OBJS = $(addprefix $(OUTDIR)/, $(addsuffix .o, $(SRC_BASE)))

$(OUTDIR)/$(TARGET): $(OBJS)
	@echo " [ar] $@"
	$(AT)$(AR) rcs $@ $^


# Compile: create object files from C source files.
define COMPILE_C_TEMPLATE
ifneq ($(findstring CYGWIN, ${OS}), )
$(OUTDIR)/$(notdir $(basename $(1))).o : $(1)
##	@echo
	@echo " [cc] $$@"
	$(AT)(`echo $(CC) -c  $$(CFLAGS) $$< -o $$@ | sed -e 's/\/cygdrive\/\([a-zA-Z]\)\//\1:\//g'`)
	$(AT)(sed -i -e 's/ \([a-zA-Z]\):/ \/cygdrive\/\1/g' $(OUTDIR)/$(notdir $(basename $(1))).d && mv $(OUTDIR)/$(notdir $(basename $(1))).d $(OUTDIR)/$(notdir $(basename $(1))).dep)
else
$(OUTDIR)/$(notdir $(basename $(1))).o : $(1)
##	@echo
	@echo " [cc] $$@"
	$(AT)($(CC) -c  $$(CFLAGS) $$< -o $$@)
	$(AT)(mv $(OUTDIR)/$(notdir $(basename $(1))).d $(OUTDIR)/$(notdir $(basename $(1))).dep)
endif
endef
$(foreach src, $(SOURCE), $(eval $(call COMPILE_C_TEMPLATE, $(src))))




clean :
	-@rm -f $(OBJS)
	-@rm -f $(D_LIST)
	-@rm -rf $(OUTDIR)


# Eye candy.
begin:
##	@echo
	@echo "build : [$(TARGET)]"
	@echo $(MSG_BEGIN)
	@if [ ! -d $(CURRENT_DIR)/$(OUTDIR) ];then \
		mkdir -p $(CURRENT_DIR)/$(OUTDIR); \
	fi

end:
	@echo $(MSG_END)
	@echo ""
##	@echo

# Display compiler version information.
gccversion :
	@$(CC) --version

install: all
	@mkdir -p $(PRJROOT_PATH)/lib/lib
	@cp -f $(OUTDIR)/$(TARGET) $(PRJROOT_PATH)/lib/lib
	@mkdir -p $(PRJROOT_PATH)/lib/include/modbus
	@cp -f src/*.h $(PRJROOT_PATH)/lib/include/modbus


