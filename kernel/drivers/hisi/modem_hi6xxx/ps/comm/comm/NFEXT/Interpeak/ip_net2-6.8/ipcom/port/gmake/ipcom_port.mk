#############################################################################
#			INTERPEAK MAKEFILE
#
#     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_port.mk,v $ $Revision: 1.37 $
#     $Source: /home/interpeak/CVSRoot/ipcom/port/gmake/ipcom_port.mk,v $
#     $Author: ulf $
#     $State: Exp $ $Locker:  $
#
#     INTERPEAK_COPYRIGHT_STRING
#
#############################################################################


#############################################################################
# OBJECTS
###########################################################################

# start
ifneq ($(IPPORT),lkm)
IPOBJECTS	+= ipcom_$(IPPORT).o

# port/<ipport>/src
IPLIBOBJECTS	+= ipcom_debug.o
IPLIBOBJECTS	+= ipcom_errno.o
IPLIBOBJECTS	+= ipcom_malloc.o
IPLIBOBJECTS	+= ipcom_os.o
IPLIBOBJECTS	+= ipcom_time.o

ifneq ($(IPPORT),las)
IPLIBOBJECTS	+= ipcom_pkt.o
IPLIBOBJECTS	+= ipcom_drv_eth.o
IPLIBOBJECTS	+= ipcom_drv_ppp.o
IPLIBOBJECTS	+= ipcom_random.o
endif
endif


# port/src
IPLIBOBJECTS	+= ipcom_checksum.o
IPLIBOBJECTS	+= ipcom_strerror.o
IPLIBOBJECTS	+= ipcom_strtok_r.o
IPLIBOBJECTS	+= ipcom_strdup.o

ifneq ($(IPPORT),lkm)
IPLIBOBJECTS	+= ipcom_access.o
IPLIBOBJECTS	+= ipcom_array.o
IPLIBOBJECTS	+= ipcom_ctime.o
IPLIBOBJECTS	+= ipcom_ctype.o
IPLIBOBJECTS	+= ipcom_dns.o
IPLIBOBJECTS	+= ipcom_exit.o
IPLIBOBJECTS	+= ipcom_ffs.o
IPLIBOBJECTS	+= ipcom_getenv.o
IPLIBOBJECTS	+= ipcom_gethostby.o
IPLIBOBJECTS	+= ipcom_getinfo.o
IPLIBOBJECTS	+= ipcom_getinfo2.o
IPLIBOBJECTS	+= ipcom_getput.o
IPLIBOBJECTS	+= ipcom_getservby.o
IPLIBOBJECTS	+= ipcom_getuid.o
IPLIBOBJECTS	+= ipcom_gmtime.o
IPLIBOBJECTS	+= ipcom_hostname.o
IPLIBOBJECTS	+= ipcom_ifaddrs.o
IPLIBOBJECTS	+= ipcom_job_queue_pipe.o
IPLIBOBJECTS	+= ipcom_mktime.o
IPLIBOBJECTS	+= ipcom_perror.o
IPLIBOBJECTS	+= ipcom_setvbuf.o
IPLIBOBJECTS	+= ipcom_signal.o
IPLIBOBJECTS	+= ipcom_snprintf.o
IPLIBOBJECTS	+= ipcom_strcasecmp.o
IPLIBOBJECTS	+= ipcom_strftime.o
IPLIBOBJECTS	+= ipcom_string_mem.o
IPLIBOBJECTS	+= ipcom_strl.o
IPLIBOBJECTS	+= ipcom_strsep.o
IPLIBOBJECTS	+= ipcom_strto.o
IPLIBOBJECTS	+= ipcom_strtok_r.o
IPLIBOBJECTS	+= ipcom_termios.o
IPLIBOBJECTS	+= ipcom_times.o
IPLIBOBJECTS	+= ipcom_uname.o
IPLIBOBJECTS	+= ipcom_vr.o
IPLIBOBJECTS	+= ipcom_xtime.o
IPLIBOBJECTS	+= ipcom_xtoyz.o
endif
ifneq ($(IPPORT),las)
ifneq ($(IPPORT),lkm)
IPLIBOBJECTS	+= ipcom_printf.o
IPLIBOBJECTS	+= ipcom_stdio.o
endif
endif


# Generic implementation of features that should be re-implemented
# OS specific for increased performance if possble
ifneq ($(IPPORT),las)
IPLIBOBJECTS	+= ipcom_generic_pipe.o
IPLIBOBJECTS	+= ipcom_generic_llpipe.o
ifneq ($(IPPORT),lkm)
IPLIBOBJECTS	+= ipcom_generic_slab.o
endif
endif

ifneq  ($(IPPORT),lkm)
IPLIBOBJECTS	+= ipcom_generic_wait_queue.o
ifneq ($(IPPORT),vxworks)
IPLIBOBJECTS	+= ipcom_generic_atomic.o
endif
endif


#############################################################################
# VPATH
###########################################################################

IPSRCDIRS += $(IPCOM_ROOT)/port/src $(IPCOM_ROOT)/port/$(IPPORT)/src


#############################################################################
# LIB
###########################################################################

# Library
IPLIBS += $(IPLIBROOT)/libipcom_$(IPPORT).a


#############################################################################
# END OF IPCOM_PORT.MK
#############################################################################

