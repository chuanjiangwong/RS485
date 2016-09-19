

#bacnet := bacnet-0.8.3

libs-y += lib$(bacnet)

lib$(bacnet)-objs-y := \
	src/apdu.c \
	src/npdu.c \
	src/bacdcode.c \
	src/bacint.c \
	src/bacreal.c \
	src/bacstr.c \
	src/bacapp.c \
	src/bacprop.c \
	src/bactext.c \
	src/bactimevalue.c \
	src/datetime.c \
	src/indtext.c \
	src/key.c \
	src/keylist.c \
	src/proplist.c \
	src/debug.c \
	src/bigend.c \
	src/arf.c \
	src/awf.c \
	src/cov.c \
	src/dcc.c \
	src/iam.c \
	src/ihave.c \
	src/rd.c \
	src/rp.c \
	src/rpm.c \
	src/timesync.c \
	src/whohas.c \
	src/whois.c \
	src/wp.c \
	src/wpm.c \
	src/abort.c \
	src/reject.c \
	src/bacerror.c \
	src/ptransfer.c \
	src/memcopy.c \
	src/filename.c \
	src/tsm.c \
	src/bacaddr.c \
	src/address.c \
	src/bacdevobjpropref.c \
	src/bacpropstates.c \
	src/alarm_ack.c \
	src/event.c \
	src/getevent.c \
	src/get_alarm_sum.c \
	src/readrange.c \
	src/timestamp.c \
	src/version.c

lib$(bacnet)-objs-y += \
	demo/handler/dlenv.c \
	demo/handler/txbuf.c \
	demo/handler/noserv.c \
	demo/handler/h_npdu.c \
	demo/handler/h_whois.c \
	demo/handler/h_iam.c  \
	demo/handler/h_rp.c \
	demo/handler/h_rp_a.c \
	demo/handler/h_rpm.c \
	demo/handler/h_rpm_a.c \
	demo/handler/h_rr.c \
	demo/handler/h_wp.c  \
	demo/handler/h_wpm.c \
	demo/handler/h_alarm_ack.c  \
	demo/handler/h_arf.c  \
	demo/handler/h_arf_a.c  \
	demo/handler/h_awf.c  \
	demo/handler/h_rd.c  \
	demo/handler/h_dcc.c  \
	demo/handler/h_ts.c  \
	demo/handler/h_whohas.c  \
	demo/handler/h_ihave.c  \
	demo/handler/h_cov.c  \
	demo/handler/h_ccov.c  \
	demo/handler/h_ucov.c  \
	demo/handler/h_getevent.c  \
	demo/handler/h_gas_a.c  \
	demo/handler/h_get_alarm_sum.c  \
	demo/handler/h_pt.c  \
	demo/handler/h_pt_a.c  \
	demo/handler/h_upt.c  \
	demo/handler/s_arfs.c \
	demo/handler/s_awfs.c \
	demo/handler/s_dcc.c \
	demo/handler/s_ihave.c \
	demo/handler/s_iam.c  \
	demo/handler/s_cov.c  \
	demo/handler/s_ptransfer.c \
	demo/handler/s_rd.c \
	demo/handler/s_rp.c  \
	demo/handler/s_rpm.c  \
	demo/handler/s_ts.c \
	demo/handler/s_cevent.c  \
	demo/handler/s_router.c  \
	demo/handler/s_uevent.c  \
	demo/handler/s_whohas.c \
	demo/handler/s_whois.c  \
	demo/handler/s_wpm.c  \
	demo/handler/s_upt.c \
	demo/handler/s_wp.c

lib$(bacnet)-objs-y += \
	ports/linux/rs485.c \
	ports/linux/dlmstp.c \
	ports/linux/timer.c \
	src/ringbuf.c \
	src/fifo.c \
	src/mstp.c \
	src/mstptext.c \
	src/crc.c \

lib$(bacnet)-cflags-y := -I$(d)/include -I$(d)/ports/linux -I$(d)/demo/object

lib$(bacnet)-cflags-y += -DBACDL_MSTP=1


#########################################
#add the lflags
lib$(bacnet)-lflags-y := lm lpthread lc

#lib$(bacnet)-cflags-y += $(d)/include -DBACDL_MSTP=1


