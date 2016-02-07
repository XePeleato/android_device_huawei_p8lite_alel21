#############################################################################
#			      IPNET2.MK
#
#     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet2.mk,v $ $Revision: 1.29 $
#     $Source: /home/interpeak/CVSRoot/ipnet2/gmake/ipnet2.mk,v $
#     $Author: ulf $ $Date: 2009-06-08 11:06:01 $
#     $State: Exp $ $Locker:  $
#
#     INTERPEAK_COPYRIGHT_STRING
#
#############################################################################

ifeq ($(IPNET2_MK_INCLUDED),)
IPNET2_MK_INCLUDED := yes

#############################################################################
# PRODUCT
###########################################################################

IPPROD ?= ipnet


#############################################################################
# DEFINE
###########################################################################

IPDEFINE += -DIPNET


#############################################################################
# INCLUDE
###########################################################################

IPINCLUDE += -I$(IPNET_ROOT)/config
IPINCLUDE += -I$(IPNET_ROOT)/include
IPINCLUDE += -I$(IPNET_ROOT)/src


#############################################################################
# OBJECTS
###########################################################################

# Functions for the userland library
IPLIBOBJECTS += ipnet_cmd.o

ifneq ($(IPPORT),lkm)
IPLIBOBJECTS += ipnet_cmd_netstat.o

ifneq ($(IPPORT),las)
# Shell commands needed for boot
IPLIBOBJECTS += ipnet_cmd_ifconfig.o
IPLIBOBJECTS += ipnet_cmd_qc.o
IPLIBOBJECTS += ipnet_cmd_route.o
IPLIBOBJECTS += ipnet_cmd_sysctl.o

# Additional shell commands
IPLIBOBJECTS += ipnet_cmd_arp.o
IPLIBOBJECTS += ipnet_cmd_ctrl.o
IPLIBOBJECTS += ipnet_cmd_pcap.o
IPLIBOBJECTS += ipnet_cmd_ping.o
IPLIBOBJECTS += ipnet_cmd_qos.o
IPLIBOBJECTS += ipnet_cmd_scaletest.o

# Config daemon
IPLIBOBJECTS += ipnet_configd.o

endif
endif

#############################################################################
# VPATH
###########################################################################

IPSRCDIRS += $(IPNET_ROOT)/src
IPSRCDIRS += $(IPNET_ROOT)/config


#############################################################################
# LIB
###########################################################################

IPLIBS += $(IPLIBROOT)/libipnet.a
ifneq ($(IPPORT),las)
IPLIBS += $(IPLIBROOT)/libipnet_krn.a
endif

#############################################################################
# IPv6
###########################################################################

ifneq ($(IPCOM_USE_INET6),no)
-include $(IPNET_ROOT)/gmake/ipnet26.mk
endif


#############################################################################
# Markets Specific Platform
###########################################################################

ifneq ($(FEATURE_SET),)
ifneq ($(FEATURE_SET),gpp)
-include $(IPNET_ROOT)/gmake/ipnet2_msp.mk
endif
endif

#############################################################################
# BINARIES
#############################################################################

ifeq ($(IPPORT),las)
# Path to binaries
IPCMDDIR 	:= $(IPOBJDIR)/bin
IPDAEMONDIR 	:= $(IPOBJDIR)/sbin

# List of shell commands to build
IPNETSHELLCMDS	= ping route qc qos nat ifconfig
ifneq ($(IPCOM_USE_INET6),no)
IPNETSHELLCMDS	+= ping6
endif
_IPNETSHELLCMDS	= $(addprefix $(IPCMDDIR)/,$(IPNETSHELLCMDS))

# List of daemons to build
IPNETDAEMONS	= vrrpd
ifneq ($(IPCOM_USE_INET6),no)
IPNETDAEMONS	+= radvd
endif
_IPNETDAEMONS	= $(addprefix $(IPDAEMONDIR)/,$(IPNETDAEMONS))

bin:	$(_IPNETSHELLCMDS) $(_IPNETDAEMONS)


$(IPOBJDIR)/ipnet_cmd_%.o:	$(IPNET_ROOT)/src/ipnet_cmd_%.c
	@$(IPECHO) "+++ Compiling IPNET shell command $<"
	$(IPVERB)$(IPCC) -Dipnet_cmd_$*=ipcom_cmd_entry_point $(IPCFLAGS) -o $@ $<

$(IPCMDDIR)/ping:	$(IPOBJDIR)/ipnet_cmd_ping.o $(IPOBJDIR)/ipcom_shell_cmd_stub.o
	@$(IPECHO) "+++ Linking IPNET shell command $@"
	$(IPVERB)$(IPLD) -o $@ $(IPLDFLAGS) $^ $(IPLIBS) $(IPLIBS) $(IPLIBS) $(IPLASTLIBS)

$(IPCMDDIR)/route:	$(IPOBJDIR)/ipnet_cmd_route.o $(IPOBJDIR)/ipcom_shell_cmd_stub.o
	@$(IPECHO) "+++ Linking IPNET shell command $@"
	$(IPVERB)$(IPLD) -o $@ $(IPLDFLAGS) $^ $(IPLIBS) $(IPLIBS) $(IPLIBS) $(IPLASTLIBS)

$(IPCMDDIR)/qc:	$(IPOBJDIR)/ipnet_cmd_qc.o $(IPOBJDIR)/ipcom_shell_cmd_stub.o
	@$(IPECHO) "+++ Linking IPNET shell command $@"
	$(IPVERB)$(IPLD) -o $@ $(IPLDFLAGS) $^ $(IPLIBS) $(IPLIBS) $(IPLIBS) $(IPLASTLIBS)

$(IPCMDDIR)/qos:	$(IPOBJDIR)/ipnet_cmd_qos.o $(IPOBJDIR)/ipcom_shell_cmd_stub.o
	@$(IPECHO) "+++ Linking IPNET shell command $@"
	$(IPVERB)$(IPLD) -o $@ $(IPLDFLAGS) $^ $(IPLIBS) $(IPLIBS) $(IPLIBS) $(IPLASTLIBS)

$(IPCMDDIR)/nat:	$(IPOBJDIR)/ipnet_cmd_nat.o $(IPOBJDIR)/ipcom_shell_cmd_stub.o
	@$(IPECHO) "+++ Linking IPNET shell command $@"
	$(IPVERB)$(IPLD) -o $@ $(IPLDFLAGS) $^ $(IPLIBS) $(IPLIBS) $(IPLIBS) $(IPLASTLIBS)

$(IPCMDDIR)/ifconfig:	$(IPOBJDIR)/ipnet_cmd_ifconfig.o $(IPOBJDIR)/ipcom_shell_cmd_stub.o
	@$(IPECHO) "+++ Linking IPNET shell command $@"
	$(IPVERB)$(IPLD) -o $@ $(IPLDFLAGS) $^ $(IPLIBS) $(IPLIBS) $(IPLIBS) $(IPLASTLIBS)

$(IPCMDDIR)/ping6:	$(IPOBJDIR)/ipnet_cmd_ping6.o $(IPOBJDIR)/ipcom_shell_cmd_stub.o
	@$(IPECHO) "+++ Linking IPNET shell command $@"
	$(IPVERB)$(IPLD) -o $@ $(IPLDFLAGS) $^ $(IPLIBS) $(IPLIBS) $(IPLIBS) $(IPLASTLIBS)


$(IPOBJDIR)/ipnet_%.o:	$(IPNET_ROOT)/src/ipnet_%.c
	@$(IPECHO) "+++ Compiling IPNET deamon $<"
	$(IPVERB)$(IPCC) -Dipnet_$*_proc=ipcom_daemon_entry_point -Dipnet_$*_create=ipcom_daemon_create $(IPCFLAGS) -o $@ $<

$(IPOBJDIR)/ipnet_%_config.o:	$(IPNET_ROOT)/config/ipnet_%_config.c
	@$(IPECHO) "+++ Compiling IPNET deamon $<"
	$(IPVERB)$(IPCC) -Dipnet_$*_configure=ipcom_daemon_configure $(IPCFLAGS) -o $@ $<

$(IPDAEMONDIR)/vrrpd:	$(IPOBJDIR)/ipnet_vrrpd.o $(IPOBJDIR)/ipnet_vrrpd_config.o $(IPOBJDIR)/ipcom_daemon_stub.o
	@$(IPECHO) "+++ Linking IPNET daemon $@"
	$(IPVERB)$(IPLD) -o $@ $(IPLDFLAGS) $^ $(IPLIBS) $(IPLIBS) $(IPLIBS) $(IPLASTLIBS)

$(IPDAEMONDIR)/radvd:	$(IPOBJDIR)/ipnet_radvd.o $(IPOBJDIR)/ipnet_radvd_config.o $(IPOBJDIR)/ipcom_daemon_stub.o
	@$(IPECHO) "+++ Linking IPNET daemon $@"
	$(IPVERB)$(IPLD) -o $@ $(IPLDFLAGS) $^ $(IPLIBS) $(IPLIBS) $(IPLIBS) $(IPLASTLIBS)

endif

endif # IPNET2_MK_INCLUDED

###########################################################################
# END OF IPNET2.MK
###########################################################################
