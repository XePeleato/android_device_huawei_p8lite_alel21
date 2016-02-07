#############################################################################
#			INTERPEAK MAKEFILE
#
#     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom.mk,v $ $Revision: 1.47.24.1 $
#     $Source: /home/interpeak/CVSRoot/ipcom/gmake/ipcom.mk,v $
#     $Author: ulf $
#     $State: Exp $ $Locker:  $
#
#     INTERPEAK_COPYRIGHT_STRING
#
#############################################################################
ifeq ($(IPCOM),)
IPCOM	:= yes

#############################################################################
# PRODUCT
###########################################################################

IPPROD ?= ipcom
-include $(IPCOM_ROOT)/arch/gmake/ipcom_arch.mk


#############################################################################
# INCLUDE
#############################################################################

IPINCLUDE	+= -I$(IPCOM_ROOT)/config
IPINCLUDE	+= -I$(IPCOM_ROOT)/include
IPINCLUDE	+= -I$(IPCOM_ROOT)/src
IPINCLUDE   += -I$(IPCOM_ROOT)/port/include

#############################################################################
# OBJECTS
###########################################################################

# Configuration
ifneq ($(IPPORT),lkm)
ifneq ($(IPPORT),las)
IPLIBOBJECTS_C	+= ipcom_config.o
IPLIBOBJECTS_C	+= ipcom_telnets_config.o
endif
endif

# Main
IPLIBOBJECTS	+= ipcom_bitarray.o
IPLIBOBJECTS	+= ipcom_cache.o
IPLIBOBJECTS	+= ipcom_err_string.o
IPLIBOBJECTS	+= ipcom_flow_fingerprint.o
IPLIBOBJECTS	+= ipcom_getinaddrprefixbyaddr.o
IPLIBOBJECTS	+= ipcom_hash.o
IPLIBOBJECTS	+= ipcom_heap_sort.o
IPLIBOBJECTS	+= ipcom_inet.o
IPLIBOBJECTS	+= ipcom_inet6.o
IPLIBOBJECTS	+= ipcom_inet_ntop.o
IPLIBOBJECTS	+= ipcom_inet_pton.o
IPLIBOBJECTS	+= ipcom_init.o
IPLIBOBJECTS	+= ipcom_list.o
IPLIBOBJECTS	+= ipcom_mask.o
IPLIBOBJECTS	+= ipcom_once.o
IPLIBOBJECTS	+= ipcom_pkt_util.o
IPLIBOBJECTS	+= ipcom_pqueue.o
IPLIBOBJECTS	+= ipcom_route.o
IPLIBOBJECTS	+= ipcom_set.o
IPLIBOBJECTS	+= ipcom_sock2.o
IPLIBOBJECTS	+= ipcom_sysvar.o
IPLIBOBJECTS	+= ipcom_sysvar2.o
IPLIBOBJECTS	+= ipcom_sysvar_conf.o
IPLIBOBJECTS	+= ipcom_usr_init.o

ifneq ($(IPPORT),lkm)
# Files that should be in LAS but not in LKM
IPLIBOBJECTS	+= ipcom_syslog_data.o
IPLIBOBJECTS	+= ipcom_strlib.o
IPLIBOBJECTS	+= ipcom_buffer.o
IPLIBOBJECTS	+= ipcom_dmalloc.o
IPLIBOBJECTS	+= ipcom_getsockaddrbyaddr.o
IPLIBOBJECTS	+= ipcom_file.o
IPLIBOBJECTS	+= ipcom_func.o
IPLIBOBJECTS	+= ipcom_getline.o
IPLIBOBJECTS	+= ipcom_getopt.o
IPLIBOBJECTS	+= ipcom_tmo.o
IPLIBOBJECTS	+= ipcom_tmo2.o
ifneq ($(IPPORT),las)
IPLIBOBJECTS	+= ipcom_auth.o
IPLIBOBJECTS	+= ipcom_auth_login.o
IPLIBOBJECTS	+= ipcom_drv_eth_filter.o
IPLIBOBJECTS	+= ipcom_fmemcpy.o
IPLIBOBJECTS	+= ipcom_ipd.o
IPLIBOBJECTS	+= ipcom_prof.o
IPLIBOBJECTS	+= ipcom_stdio_syslog.o
IPLIBOBJECTS    += ipcom_mshell.o
IPLIBOBJECTS    += ipcom_shell.o
IPLIBOBJECTS    += ipcom_shellalias.o
IPLIBOBJECTS    += ipcom_shellcmd.o
IPLIBOBJECTS    += ipcom_shellfile.o
IPLIBOBJECTS    += ipcom_shellio.o
IPLIBOBJECTS    += ipcom_syslogd.o
IPLIBOBJECTS    += ipcom_telnet.o
IPLIBOBJECTS	+= ipcom_waitif.o
endif
endif

ifneq ($(IPPORT),las)
IPLIBOBJECTS	+= ipcom_pipe.o
endif

# Shell commands
IPLIBOBJECTS	+= ipcom_cmd.o
ifneq ($(IPPORT),lkm)
ifneq ($(IPPORT),las)
IPLIBOBJECTS	+= ipcom_cmd_echo.o
IPLIBOBJECTS	+= ipcom_cmd_ipd.o
IPLIBOBJECTS    += ipcom_cmd_shellcmds.o
IPLIBOBJECTS	+= ipcom_cmd_sockperf.o
IPLIBOBJECTS	+= ipcom_cmd_socktest.o
IPLIBOBJECTS	+= ipcom_cmd_syslog.o
IPLIBOBJECTS	+= ipcom_cmd_sysvar.o
IPLIBOBJECTS	+= ipcom_cmd_tracert.o
IPLIBOBJECTS	+= ipcom_cmd_ttcp.o
IPLIBOBJECTS	+= ipcom_cmd_user.o
IPLIBOBJECTS	+= ipcom_cmd_version.o
IPLIBOBJECTS	+= ipcom_cmd_mib2.o
IPLIBOBJECTS	+= ipcom_cmd_smptest.o
IPLIBOBJECTS	+= ipcom_cmd_cpu.o
#IPLIBOBJECTS	+= ipcom_cmd_wlan.o
endif
endif

###########################################################################
# Market Specific Platform
###########################################################################

ifneq ($(FEATURE_SET),)
ifneq ($(FEATURE_SET),gpp)

IPLIBOBJECTS	+= ipcom_key_db.o

ifneq ($(IPPORT),las)
IPLIBOBJECTS	+= ipcom_forwarder_common.o
IPLIBOBJECTS	+= ipcom_forwarder_ctrl.o
IPLIBOBJECTS	+= ipcom_forwarder_data.o
IPLIBOBJECTS	+= ipcom_forwarder_ipsec.o
ifneq ($(IPPORT),lkm)
IPLIBOBJECTS	+= ipcom_cmd_fwctrl.o
IPLIBOBJECTS	+= ipcom_cmd_keydb.o
endif
endif

endif
endif

###########################################################################


# Compiles the xxx_config.o if the $SKIP_CONFIG macro is either not defined
# or set to anything other than true.
ifneq ($(SKIP_CONFIG),true)
IPLIBOBJECTS    += $(IPLIBOBJECTS_C)
endif


#############################################################################
# VPATH
###########################################################################

IPSRCDIRS += $(IPCOM_ROOT)/src
IPSRCDIRS += $(IPCOM_ROOT)/config


#############################################################################
# LIB
###########################################################################

# Library
IPLIBS += $(IPLIBROOT)/libipcom.a


endif
#############################################################################
# END OF IPCOM.MK
#############################################################################

