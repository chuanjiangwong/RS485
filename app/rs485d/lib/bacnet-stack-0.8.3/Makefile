# Copyright (C) 2008-2014 Marvell International Ltd.
# All Rights Reserved.


# Directory for output files (lst, obj, dep, elf, sym, map, hex, bin etc.).
OUTDIR = build
# define the source file directory
SRC_DIR = src
# define the current dir
CURRENT_DIR = .


# define the include dir
INC_DIR = 
# define the lib include
LIB_INC_DIR =
# others include dir
EXTEND_CFLAGS =

# configuration
# If BACNET_DEFINES has not already been set, configure to your needs here
#MY_BACNET_DEFINES = -DPRINT_ENABLED=1
#MY_BACNET_DEFINES += -DBACAPP_ALL
MY_BACNET_DEFINES += -DBACFILE
MY_BACNET_DEFINES += -DINTRINSIC_REPORTING
MY_BACNET_DEFINES += -DBACNET_PROPERTY_LISTS=1
BACNET_DEFINES ?= $(MY_BACNET_DEFINES)

#BACDL_DEFINE=-DBACDL_ETHERNET=1
#BACDL_DEFINE=-DBACDL_ARCNET=1
BACDL_DEFINE=-DBACDL_MSTP=1
#BACDL_DEFINE?=-DBACDL_BIP=1

# Declare your level of BBMD support
#BBMD_DEFINE ?=-DBBMD_ENABLED=1
#BBMD_DEFINE ?= -DBBMD_ENABLED=0
BBMD_DEFINE ?= -DBBMD_CLIENT_ENABLED

# Passing parameters via command line
MAKE_DEFINE ?=

# Define WEAK_FUNC for [...somebody help here; I can't find any uses of it]
DEFINES = $(BACNET_DEFINES) $(BACDL_DEFINE) $(BBMD_DEFINE) -DWEAK_FUNC=
DEFINES += $(MAKE_DEFINE)

# BACnet Ports Directory
BACNET_PORT ?= linux

# Default compiler settings
OPTIMIZATION = -Os
DEBUGGING =
WARNINGS = -Wall -Wmissing-prototypes
ifeq (${BUILD},debug)
OPTIMIZATION = -O0
DEBUGGING = -g -DDEBUG_ENABLED=1
ifeq (${BACDL_DEFINE},-DBACDL_BIP=1)
DEFINES += -DBIP_DEBUG
endif
endif


# lib path
LINK_LIB_DIR =$(PRJROOT_PATH)/lib/lib
# lib LFLAGS
LINK_LIB =-llist,-lmessage_queue,-lbacnet,-lmodbus
# system lib flags
SYSTEM_LIB =-lc,-lgcc,-lrt,-lm,-lpthread

#define the target
TARGET = libbacnet.a


BACNET_PORT_DIR = ports/${BACNET_PORT}
BACNET_OBJECT = demo/object
BACNET_HANDLER = demo/handler
BACNET_CORE = src
BACNET_INCLUDE = include
# compiler configuration
#STANDARDS = -std=c99
INCLUDE1 = -I$(BACNET_PORT_DIR) -I$(BACNET_OBJECT) -I$(BACNET_HANDLER)
INCLUDE2 = -I$(BACNET_INCLUDE)
INC_DIR = $(INCLUDE1) $(INCLUDE2)


SRCS += \
	$(BACNET_CORE)/apdu.c \
	$(BACNET_CORE)/npdu.c \
	$(BACNET_CORE)/bacdcode.c \
	$(BACNET_CORE)/bacint.c \
	$(BACNET_CORE)/bacreal.c \
	$(BACNET_CORE)/bacstr.c \
	$(BACNET_CORE)/bacapp.c \
	$(BACNET_CORE)/bacprop.c \
	$(BACNET_CORE)/bactext.c \
	$(BACNET_CORE)/bactimevalue.c \
	$(BACNET_CORE)/datetime.c \
	$(BACNET_CORE)/indtext.c \
	$(BACNET_CORE)/key.c \
	$(BACNET_CORE)/keylist.c \
	$(BACNET_CORE)/proplist.c \
	$(BACNET_CORE)/debug.c \
	$(BACNET_CORE)/bigend.c \
	$(BACNET_CORE)/arf.c \
	$(BACNET_CORE)/awf.c \
	$(BACNET_CORE)/cov.c \
	$(BACNET_CORE)/dcc.c \
	$(BACNET_CORE)/iam.c \
	$(BACNET_CORE)/ihave.c \
	$(BACNET_CORE)/rd.c \
	$(BACNET_CORE)/rp.c \
	$(BACNET_CORE)/rpm.c \
	$(BACNET_CORE)/timesync.c \
	$(BACNET_CORE)/whohas.c \
	$(BACNET_CORE)/whois.c \
	$(BACNET_CORE)/wp.c \
	$(BACNET_CORE)/wpm.c \
	$(BACNET_CORE)/abort.c \
	$(BACNET_CORE)/reject.c \
	$(BACNET_CORE)/bacerror.c \
	$(BACNET_CORE)/ptransfer.c \
	$(BACNET_CORE)/memcopy.c \
	$(BACNET_CORE)/filename.c \
	$(BACNET_CORE)/tsm.c \
	$(BACNET_CORE)/bacaddr.c \
	$(BACNET_CORE)/address.c \
	$(BACNET_CORE)/bacdevobjpropref.c \
	$(BACNET_CORE)/bacpropstates.c \
	$(BACNET_CORE)/alarm_ack.c \
	$(BACNET_CORE)/event.c \
	$(BACNET_CORE)/getevent.c \
	$(BACNET_CORE)/get_alarm_sum.c \
	$(BACNET_CORE)/readrange.c \
	$(BACNET_CORE)/timestamp.c \
	$(BACNET_CORE)/version.c

SRCS += \
	$(BACNET_HANDLER)/dlenv.c \
	$(BACNET_HANDLER)/txbuf.c \
	$(BACNET_HANDLER)/noserv.c \
	$(BACNET_HANDLER)/h_npdu.c \
	$(BACNET_HANDLER)/h_whois.c \
	$(BACNET_HANDLER)/h_iam.c  \
	$(BACNET_HANDLER)/h_rp.c \
	$(BACNET_HANDLER)/h_rp_a.c \
	$(BACNET_HANDLER)/h_rpm.c \
	$(BACNET_HANDLER)/h_rpm_a.c \
	$(BACNET_HANDLER)/h_rr.c \
	$(BACNET_HANDLER)/h_wp.c  \
	$(BACNET_HANDLER)/h_wpm.c \
	$(BACNET_HANDLER)/h_alarm_ack.c  \
	$(BACNET_HANDLER)/h_arf.c  \
	$(BACNET_HANDLER)/h_arf_a.c  \
	$(BACNET_HANDLER)/h_awf.c  \
	$(BACNET_HANDLER)/h_rd.c  \
	$(BACNET_HANDLER)/h_dcc.c  \
	$(BACNET_HANDLER)/h_ts.c  \
	$(BACNET_HANDLER)/h_whohas.c  \
	$(BACNET_HANDLER)/h_ihave.c  \
	$(BACNET_HANDLER)/h_cov.c  \
	$(BACNET_HANDLER)/h_ccov.c  \
	$(BACNET_HANDLER)/h_ucov.c  \
	$(BACNET_HANDLER)/h_getevent.c  \
	$(BACNET_HANDLER)/h_gas_a.c  \
	$(BACNET_HANDLER)/h_get_alarm_sum.c  \
	$(BACNET_HANDLER)/h_pt.c  \
	$(BACNET_HANDLER)/h_pt_a.c  \
	$(BACNET_HANDLER)/h_upt.c  \
	$(BACNET_HANDLER)/s_arfs.c \
	$(BACNET_HANDLER)/s_awfs.c \
	$(BACNET_HANDLER)/s_dcc.c \
	$(BACNET_HANDLER)/s_ihave.c \
	$(BACNET_HANDLER)/s_iam.c  \
	$(BACNET_HANDLER)/s_cov.c  \
	$(BACNET_HANDLER)/s_ptransfer.c \
	$(BACNET_HANDLER)/s_rd.c \
	$(BACNET_HANDLER)/s_rp.c  \
	$(BACNET_HANDLER)/s_rpm.c  \
	$(BACNET_HANDLER)/s_ts.c \
	$(BACNET_HANDLER)/s_cevent.c  \
	$(BACNET_HANDLER)/s_router.c  \
	$(BACNET_HANDLER)/s_uevent.c  \
	$(BACNET_HANDLER)/s_whohas.c \
	$(BACNET_HANDLER)/s_whois.c  \
	$(BACNET_HANDLER)/s_wpm.c  \
	$(BACNET_HANDLER)/s_upt.c \
	$(BACNET_HANDLER)/s_wp.c

PORT_ARCNET_SRC = \
	$(BACNET_PORT_DIR)/arcnet.c

SRCS += \
	$(BACNET_PORT_DIR)/rs485.c \
	$(BACNET_PORT_DIR)/dlmstp.c \
	$(BACNET_PORT_DIR)/timer.c \
	$(BACNET_CORE)/ringbuf.c \
	$(BACNET_CORE)/fifo.c \
	$(BACNET_CORE)/mstp.c \
	$(BACNET_CORE)/mstptext.c \
	$(BACNET_CORE)/crc.c \

PORT_ETHERNET_SRC = \
	$(BACNET_PORT_DIR)/ethernet.c

PORT_BIP_SRC = \
	$(BACNET_PORT_DIR)/bip-init.c \
	$(BACNET_CORE)/bvlc.c \
	$(BACNET_CORE)/bip.c

PORT_ALL_SRC = \
	$(BACNET_PORT_DIR)/arcnet.c \
	$(BACNET_PORT_DIR)/dlmstp.c \
	$(BACNET_PORT_DIR)/rs485.c \
	$(BACNET_PORT_DIR)/timer.c \
	$(BACNET_CORE)/ringbuf.c \
	$(BACNET_CORE)/fifo.c \
	$(BACNET_CORE)/mstp.c \
	$(BACNET_CORE)/crc.c \
	$(BACNET_PORT_DIR)/ethernet.c \
	$(BACNET_PORT_DIR)/bip-init.c \
	$(BACNET_CORE)/bvlc.c \
	$(BACNET_CORE)/bip.c

ifeq (${BACDL_DEFINE},-DBACDL_BIP=1)
PORT_SRC = ${PORT_BIP_SRC}
endif
ifeq (${BACDL_DEFINE},-DBACDL_MSTP=1)
PORT_SRC = ${PORT_MSTP_SRC}
endif
ifeq (${BACDL_DEFINE},-DBACDL_ARCNET=1)
PORT_SRC = ${PORT_ARCNET_SRC}
endif
ifeq (${BACDL_DEFINE},-DBACDL_ETHERNET=1)
PORT_SRC = ${PORT_ETHERNET_SRC}
endif
ifdef BACDL_ALL
PORT_SRC = ${PORT_ALL_SRC}
endif




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
	$(AT)$(AR) cru $@ $^


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
	$(AT)($(CC) -c  $$(CFLAGS) $$(DEFINES) $$< -o $$@)
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
	@mkdir -p $(PRJROOT_PATH)/lib/include/bacnet
	@cp -f include/*.h $(PRJROOT_PATH)/lib/include/bacnet



