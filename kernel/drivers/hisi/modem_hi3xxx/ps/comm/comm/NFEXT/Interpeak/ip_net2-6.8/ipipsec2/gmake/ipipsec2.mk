#############################################################################
#			      IPIPSEC2.MK
#
#     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipipsec2.mk,v $ $Revision: 1.14 $
#     $Source: /home/interpeak/CVSRoot/ipipsec2/gmake/ipipsec2.mk,v $
#     $Author: ulf $ $Date: 2009-06-08 11:05:17 $
#     $State: Exp $ $Locker:  $
#
#     INTERPEAK_COPYRIGHT_STRING
#
#############################################################################

ifeq ($(IPIPSEC2_MK_INCLUDED),)

IPIPSEC2_MK_INCLUDED := yes

#############################################################################
# PRODUCT
###########################################################################

IPPROD ?= ipipsec2


#############################################################################
# DEFINE
###########################################################################

IPDEFINE += -DIPIPSEC2

ifeq ($(IPPORT),lkm)
IPIPSEC2=y
endif

#############################################################################
# INCLUDE
###########################################################################

IPINCLUDE += -I$(IPIPSEC_ROOT)/config
IPINCLUDE += -I$(IPIPSEC_ROOT)/include
IPINCLUDE += -I$(IPIPSEC_ROOT)/src


#############################################################################
# OBJECTS
###########################################################################

# Configuration
ifneq ($(IPPORT),las)
IPLIBOBJECTS_C 	+= ipipsec_config.o
ifneq ($(IPPORT),lkm)
IPLIBOBJECTS	+= ipipsec_example.o
endif

# Main
IPLIBOBJECTS	+= ipipsec.o
IPLIBOBJECTS	+= ipipsec_crypt.o
IPLIBOBJECTS	+= ipipsec_debug.o
IPLIBOBJECTS	+= ipipsec_pfkeyv2.o
endif # !LAS

IPLIBOBJECTS	+= ipipsec_ctrl.o

ifneq ($(IPPORT),lkm)
IPLIBOBJECTS	+= ipipsec_cmd_keyadm.o
IPLIBOBJECTS	+= ipipsec_cmd_ipsecctrl.o
endif

# Compiles the xxx_config.o if the $SKIP_CONFIG macro is either not defined
# or set to anything other than true.
ifneq ($(SKIP_CONFIG),true)
IPLIBOBJECTS    += $(IPLIBOBJECTS_C)
endif


#############################################################################
# SOURCE
###########################################################################

IPSRCDIRS += $(IPIPSEC_ROOT)/src $(IPIPSEC_ROOT)/config


#############################################################################
# LIB
###########################################################################

IPLIBS += $(IPLIBROOT)/libipipsec2.a


#############################################################################
# BINARIES
#############################################################################

ifeq ($(IPPORT),las)
# Path to binaries
IPCMDDIR 	:= $(IPOBJDIR)/bin

# List of shell commands to build
IPIPSEC2SHELLCMDS	= keyadm ipsecctrl
_IPIPSEC2SHELLCMDS	= $(addprefix $(IPCMDDIR)/,$(IPIPSEC2SHELLCMDS))

bin:	$(_IPIPSEC2SHELLCMDS)

$(IPOBJDIR)/ipipsec_cmd_%.o:	$(IPIPSEC_ROOT)/src/ipipsec_cmd_%.c
	@$(IPECHO) "+++ Compiling IPIPSEC shell command $<"
	$(IPVERB)$(IPCC) -Dipipsec_cmd_$*=ipcom_cmd_entry_point $(IPCFLAGS) -o $@ $<

$(IPCMDDIR)/keyadm:	$(IPOBJDIR)/ipipsec_cmd_keyadm.o $(IPOBJDIR)/ipcom_shell_cmd_stub.o
	@$(IPECHO) "+++ Linking IPIPSEC shell command $@"
	$(IPVERB)$(IPLD) -o $@ $(IPLDFLAGS) $^ $(IPLIBS) $(IPLIBS) $(IPLIBS) $(IPLASTLIBS)

$(IPCMDDIR)/ipsecctrl:	$(IPOBJDIR)/ipipsec_cmd_ipsecctrl.o $(IPOBJDIR)/ipcom_shell_cmd_stub.o
	@$(IPECHO) "+++ Linking IPIPSEC shell command $@"
	$(IPVERB)$(IPLD) -o $@ $(IPLDFLAGS) $^ $(IPLIBS) $(IPLIBS) $(IPLIBS) $(IPLASTLIBS)

endif

endif # IPIPSEC2_MK_INCLUDED

###########################################################################
# END OF IPIPSEC2.MK
###########################################################################
