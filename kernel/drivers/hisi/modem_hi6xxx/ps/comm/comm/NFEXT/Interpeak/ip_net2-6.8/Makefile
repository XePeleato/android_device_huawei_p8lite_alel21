# Makefile - Top-level comp_ip_net2 Makefile
#
# Copyright 2006-2009 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# modification history
# --------------------
# 03r,08jun10,spw  WIND00216460 Add ifproxy wrnad
# 03q,11feb10,uol  WIND00158454: Use rclean rule also for GPP.
# 03p,31jul09,ten  Add IPL2TP
# 03o,22jul09,uol  Add IPMIPPM
# 03n,14jul09,mdk  merge from networking branch
# 03m,18jun09,mze  adding changes to support /usr/lib in VSBs
# 03l,18jun09,mze  fixin WIND00153279 again, change lost after merge.
# 03k,11jun09,dwb  Adding vxdot1x
# 03j,15jun09,mze  fixin WIND00153279 again, change lost after merge.
# 03i,16dec08,mry  Add IPWPS
# 03h,11feb09,ggz  Move coreip and bridge into CVS.
# 03g,27jan09,pee  WIND00153279
# 03k,23apr09,uol  Add module ipmippm
# 03j,26mar09,uol  WIND00158454: Use rclean rule also for GPP.
# 03i,02mar09,uol  Add ipmip6hm and ipmip6pm
# 03h,09feb09,mca  add IPCAVIUM component
# 03g,03dec08,f_g  added IPSCTP component
# 03g,27jan09,pee  WIND00153279
# 03f,17nov08,mze  move export of FEATURE_SET below VSB eval
# 03e,04nov08,mze  test
# 03d,03nov08,mze  move eval out of conditional
# 03b,29oct08,mze  fix usr side tool reference
# 03a,02oct08,mze  Change VSB exports
# 02z,02sep08,mze  removal of ipcom_libconfig.mk reference
# 02y,31jul08,pee  exporting _WRS_CONFIG_ADDEDCFLAGS 
# 02x,08jul08,pee  adding IPDHCPS6 to VSB parameter translation list
# 02w,02jul08,mze  undo last change
# 02v,02jul08,mze  VSB changes
# 02u,01jul08,mze  adding new Components
# 02t,30jun08,mze  adding DIAMETERP
# 02s,30jun08,mze  VSB changes
# 02r,24jun08,mze  added SPACE
# 02q,23jun08,mze  remove IPVXCOMPAT
# 02p,05jun08,mze  VSB changes
# 02o,03jun08,pee  including VSB configuration.
# 02n,03jun08,mze  changing library location to be vsb based
# 02m,28may08,dlk  Added vxmux module.
# 02l,20feb08,tlu  Add build support for IPROHC
# 02k,27may08,uol  Move util_ipdef and util_ipinc to be visible in MSP builds.
# 02j,26may08,brg  Changing product name ipvxcompat to vxcompat
# 02i,01may08,dlk  Added util_ipdef, util_ipinc rules.
# 02h,14may08,brg  Adding ipvxcompat
# 02g,08may08,uol  Add module ipmip
# 02f,07aug07,mry  Add 802.1x supplicant support
# 02e,04sep07,kch  Stripped leading and trailing whitespace (WIND00046447).
# 02d,17jul07,spw  Added IPWLAN
# 02c,12jul07,nlu  add mip4
# 02b,29jun07,kch  Removed trailing space after the backslash immediately
#                  following "ipmip4".
# 02a,29jun07,mcarlste Add IPFREESCALE
# 01z,29jun07,???  build rules for MIP4
# 01y,29jun07,???  build rules for MIP4
# 01x,19jun07,tpw  Use WRVX_COMPBASE vs. WIND_COMPONENTS correctly.
# 01w,03apr07,tlu  Add build support for IPDIAMETER
# 01v,02may07,tkf  Update component library path for SMP.
# 01u,26apr07,tkf  Add IPv6-Only build support. Also fix CQ:WIND00092985.
# 01t,21feb07,kch  Added DIR_EXT for SMP build support.
# 01s,13dec06,tkf  Fix user side compiler error (WIND00083671).
# 01r,27oct06,tkf  Change library archive location to WIND_COMPONENTS/obj.
# 01q,26sep06,kch  Added IPRADIUS.
# 01p,14sep06,tkf  Replcaed target all with kernel. Added IPDHCPC6 and
#                  IPDHCPS6.
# 01o,12sep06,dlk  Replace 'make' with MAKE variable.
# 01n,09sep06,jbl  add WRSNMP to the mix
# 01m,08sep06,tkf  Add user side build support.
# 01l,06sep06,tkf  Changed the way IpComps is processed.
# 01k,07aug06,kch  Renamed IPDHCPDIR to IPDHCPSDIR and IPFWDIR to
#                  IPFIREWALLDIR. Also removed unsupported IPNet products.
# 01j,19jul06,tlu  Added support for IPRIPNG component
# 01i,19jul06,tkf  Include make.<CPU>.<TOOL> regardless of whether
#                  VX_CPU_FAMILY is defined or not.
# 01h,03jul06,tkf  Modifications for use with version-less component
#                  directories: define and export BASE_ROOT; changed rule to
#                  ignore version numbers in directory names.
# 01g,02jul06,tkf  Export WIND_IPNET=true by default.
# 01f,12jun06,dlk  Simplify component logic; handle missing components.
# 01e,08jun06,tkf  More dynamic directory checking on all components.
# 01d,25may06,tkf  Merge with Dan Kresja's and John True's changes.\
# 01c,15may06,tkf  Move MSP/GPP env vars to here.
# 01b,10may06,tkf  Extract VxWorks version from env vars. Add rclean target.
#                  Get CPU/TOOL info from env vars.
# 01a,09may06,tkf  Remove dependecy on build host and DCOM component.
#                  Modified to build the IPNET way temporarily.
# 00a,01may06,j_c  written.
#
# DESCRIPTION
# This is the top level Makefile for the comp_ip_net2 component
#

##
# Define and export current VxWorks version
#
WR_IPCOM_NAME := vxworks
WR_IPCOM_VERSION := $(filter-out $(WR_IPCOM_NAME), $(subst -, ,$(WIND_PLATFORM)))
export WR_IPCOM_VERSION := $(subst .,_,$(WR_IPCOM_VERSION))

export COMP_IPNET_PATH	= $(CURDIR)

export BASE_ROOT := $(COMP_IPNET_PATH)

TGT_DIR = $(WIND_BASE)/target


# must only bring in VSB if building kernel
ifeq ($(SPACE),user)
TMP_USR_BLD := 1
endif
 
ifeq ($(SPACE),USER)
TMP_USR_BLD := 1
endif

ifndef __DEFS_VSBVARS_MK_INCLUDED
include $(WIND_BASE)/target/h/make/defs.vsbvars.mk
endif

ifneq ($(wildcard $(VSB_MAKE_CONFIG_FILE)),)
include $(VSB_MAKE_CONFIG_FILE)
endif

ifeq ($(TMP_USR_BLD),)

# the following is a list of the IPNET configurations that have booleans associated with them

_VARLIST = \
	COMPONENT_IPNET \
	COMPONENT_IPLITE \
	WIND_IPNET \
	COMPONENT_COREIP \
	FEATURE_IPNET_VERBOSE \
	FEATURE_IPNET_GEN_DEPEND \
	FEATURE_IP_GPP_BUILD \
	MSP_FEATURE_PAD \
	MSP_FEATURE_PCD \
	MSP_FEATURE_PID \
	MSP_FEATURE_PNE \
	MSP_FEATURE_VIPER \
	MSP_KERNEL \
	MSP_RTP \
	MSP_BUILD_USER \
	COMPONENT_DOT1X \
	COMPONENT_XML \
	COMPONENT_IP8021X \
	COMPONENT_IPWPS \
	COMPONENT_IPAPPL \
	COMPONENT_IPCAVIUM \
	COMPONENT_IPCRYPTO \
	COMPONENT_IPXINC \
	COMPONENT_IPDHCPC6 \
	COMPONENT_IPDHCPR \
	COMPONENT_IPDHCPS \
	COMPONENT_IPDHCPS6 \
	COMPONENT_IPDIAMETERC \
	COMPONENT_IPDIAMETERP \
	COMPONENT_IPEAP \
	COMPONENT_IPFIREWALL \
	COMPONENT_IPFREESCALE \
	COMPONENT_IPHWCRYPTO \
	COMPONENT_IPIKE \
	COMPONENT_IPIPSEC \
	COMPONENT_IPL2TP \
	COMPONENT_IPMCP \
	COMPONENT_IPMCRYPTO \
	COMPONENT_IPMIP \
	COMPONENT_IPMIP4 \
	COMPONENT_IPMIP6 \
	COMPONENT_IPMIP6HA \
	COMPONENT_IPMIP6MN \
	COMPONENT_IPMIP6PM \
	COMPONENT_IPMIPFA \
	COMPONENT_IPMIPHA \
	COMPONENT_IPMIPMN \
	COMPONENT_IPMIPPM \
	COMPONENT_IPMPLS \
	COMPONENT_IPPPP \
	COMPONENT_IPRADIUS \
	COMPONENT_IPRIP \
	COMPONENT_IPRIPNG \
	COMPONENT_IPSNTP \
	COMPONENT_IPSSH \
	COMPONENT_IPSSL \
	COMPONENT_IPTCP \
	COMPONENT_IPSCTP \
	COMPONENT_WLAN \
	COMPONENT_SNMP \
	COMPONENT_DCOM \
	COMPONENT_OPC \
	COMPONENT_BRIDGE \
	COMPONENT_CAN \
	COMPONENT_SECURITY \
	COMPONENT_VIP \
	FEATURE_SNMP_V3 \
	FEATURE_SNMP_AGENTX \
	COMPONENT_IPROHC \
	COMPONENT_VXMUX \
	COMPONENT_VXCOMPAT \
	COMPONENT_VXCOREIP \
	COMPONENT_VXDOT1X \
        COMPONENT_WRNAD \
        COMPONENT_IFPROXY
else

_VARLIST = \
	COMPONENT_IPNET \
	WIND_IPNET \
	FEATURE_IPNET_VERBOSE \
	FEATURE_IPNET_GEN_DEPEND \
	MSP_FEATURE_PAD \
	MSP_FEATURE_PCD \
	MSP_FEATURE_PID \
	MSP_FEATURE_PNE
        
MSP_RTP= true
export MSP_RTP

COMPONENT_COREIP= true
export COMPONENT_COREIP

MSP_BUILD_USER= true
export MSP_BUILD_USER


endif

# straight export
export _WRS_CONFIG_ADDEDCFLAGS
export _WRS_CONFIG_ADDEDCFLAGS2
export _WRS_CONFIG_ADDEDCFLAGS_APP


# the following is a list of the IPNET configurations that have strings associated with them

_STRLIST = \
        FEATURE_IPNET_BUILD
		

define defvars

ifdef _WRS_CONFIG_$1
$1 = true
else
$1 = false
endif
export $1
endef

$(eval $(foreach var,$(_VARLIST),$(call defvars,$(var))))

ifeq ($(MSP_FEATURE_PAD),true)
export FEATURE_SET = pad
endif

ifeq ($(MSP_FEATURE_PCD),true)
export FEATURE_SET = pcd
endif

ifeq ($(MSP_FEATURE_PID),true)
export FEATURE_SET = pid
endif

ifeq ($(MSP_FEATURE_PNE),true)
export FEATURE_SET = pne
endif



define defStr

$1 = $(_WRS_CONFIG_$1)
export $1
endef

$(eval $(foreach var,$(_STRLIST),$(call defStr,$(var))))

ifeq ($(WIND_IPNET),)
export WIND_IPNET = true
endif

IP_DOT := .
IP_EMPTY :=
IP_SPACE := $(IP_EMPTY) $(IP_EMPTY)
##
# The available IP components are listed as <IP_MACRO>.<IP_NAME>
#
IpComps := \
	BRIDGEDIR.vxbridge \
	IP8021XDIR.ip8021x \
	IPAPPLDIR.ipappl \
	IPCAVIUMDIR.ipcavium \
	IPCOMDIR.ipcom \
	IPCRYPTODIR.ipcrypto \
	IPDHCPC6DIR.ipdhcpc6 \
	IPDHCPRDIR.ipdhcpr \
	IPDHCPS6DIR.ipdhcps6 \
	IPDHCPSDIR.ipdhcps \
	IPDIAMETERDIR.ipdiameter \
	IPEAPDIR.ipeap \
	IPFIREWALLDIR.ipfirewall \
	IPFREESCALEDIR.ipfreescale \
	IPHWCRYPTODIR.iphwcrypto \
	IPIKEDIR.ipike \
	IPIPSEC2DIR.ipipsec2 \
	IPL2TPDIR.ipl2tp \
	IPLDAPCDIR.ipldapc \
	IPLITEDIR.iplite2 \
	IPMCPDIR.ipmcp \
	IPMCRYPTODIR.ipmcrypto \
	IPMIP4DIR.ipmip4 \
	IPMIP6DIR.ipmip6 \
	IPMIP6HADIR.ipmip6ha \
	IPMIP6MNDIR.ipmip6mn \
	IPMIP6PMDIR.ipmip6pm \
	IPMIPDIR.ipmip \
	IPMIPFADIR.ipmipfa \
	IPMIPHADIR.ipmipha \
	IPMIPMNDIR.ipmipmn \
	IPMIPPMDIR.ipmippm \
	IPMPLSDIR.ipmpls \
	IPNET2DIR.ipnet2 \
	IPPPPDIR.ipppp \
	IPRADIUSDIR.ipradius \
	IPRIPDIR.iprip \
	IPRIPNGDIR.ipripng \
	IPROHCDIR.iprohc \
	IPSCTPDIR.ipsctp \
	IPSNTPDIR.ipsntp \
	IPSSHDIR.ipssh \
	IPSSL2DIR.ipssl2 \
	IPTCPDIR.iptcp \
	IPWLANDIR.ipwlan \
	IPWPSDIR.ipwps \
	IPXINCDIR.ipxinc \
	VXCOMPATDIR.vxcompat \
	VXCOREIPDIR.vxcoreip \
	VXDOT1XDIR.vxdot1x \
	VXMUXDIR.vxmux \
	WRSNMP.wrsnmp \
        WRNADDIR.wrnad \
        IFPROXYDIR.ifproxy

###
# By default we choose the lexically last IPNET component version in hopes
# that it is the most recent. The directory macros (e.g. IPCOMDIR) may of
# course be overriden on the command line.

define _DIR_DEF
ifeq ($$($(basename $(1))),)
TMP_DIRSU := $$(wildcard $(patsubst .%,%,$(suffix $(1)))*)
TMP_DIRS := $$(sort $$(TMP_DIRSU))
ifneq ($$(strip $$(TMP_DIRS)),)
export $(basename $(1)) := $$(word 1,$$(TMP_DIRS))
endif
endif
IP_BUILDABLE_COMPS := $$(IP_BUILDABLE_COMPS) $(basename $(1))=$$($(basename $(1)))
#$$(warning $(basename $(1)) = $$($(basename $(1))) )
endef

$(foreach IpComp,$(IpComps), $(eval $(call _DIR_DEF,$(IpComp))))

ifeq ($(IPCOMDIR),)
$(error IPCOMDIR is empty.)
endif

ifeq ($(TGT_DIR),)
ifeq ($(TMP_USR_BLD),)
TGT_DIR = $(WIND_BASE)/target
else
TGT_DIR = $(WIND_BASE)/target/usr
endif
endif

include $(WIND_BASE)/target/h/make/defs.options

##
# Ensure VX_CPU_FAMILY is defined
#
ifeq ($(TOOL_FAMILY),)
ifeq  ($(findstring gnu,$(TOOL)),gnu)
TOOL_FAMILY	= gnu
else
ifeq  ($(findstring diab,$(TOOL)),diab)
TOOL_FAMILY	= diab
endif
endif
endif

ifeq ($(TMP_USR_BLD),)
TMP_BUILD_SPACE := krnl

include $(TGT_DIR)/h/tool/$(TOOL_FAMILY)/make.$(CPU)$(TOOL)

ifneq ($(LOCAL_OBJS),)
ifeq ($(WR_IPCOM_LIB_DIR),)
export WR_IPCOM_LIB_DIR := $(CURDIR)/$(IPCOMDIR)/lib$(MAJOR_OPTION_SUFFIX)/$(VX_CPU_FAMILY)/$(CPU)/$(TOOL_COMMON_DIR)
endif
else
ifeq ($(WR_IPCOM_LIB_DIR),)
export WR_IPCOM_LIB_DIR := $(VSB_DIR)/$(VX_CPU_FAMILY)/$(CPU)$(CPU_OPTION_SUFFIX)/$(TOOL_COMMON_DIR)$(LIB_DIR_TAG)$(MINOR_OPTION_SUFFIX)
endif
endif

else
TMP_BUILD_SPACE := usr
include $(TGT_DIR)/usr/tool/$(TOOL_FAMILY)/make.$(CPU)$(TOOL)
ifneq ($(LOCAL_OBJS),)
ifeq ($(WR_IPCOM_LIB_DIR),)
export WR_IPCOM_LIB_DIR := $(CURDIR)/$(IPCOMDIR)/lib$(MAJOR_OPTION_SUFFIX)/usr/lib/$(VX_CPU_FAMILY)/$(CPU)/$(TOOL_COMMON_DIR)
endif
else
ifeq ($(WR_IPCOM_LIB_DIR),)
export WR_IPCOM_LIB_DIR := $(VSB_DIR)/usr/lib/$(VX_CPU_FAMILY)/$(CPU)/$(TOOL_COMMON_DIR)
endif
endif
endif

export WR_IPTARGET := $(VX_CPU_FAMILY)

ifneq ($(FEATURE_SET),)  # MSP Build

###
# This block is to be used during feature set build
#

include $(WIND_BASE)/config/common/defs.component
#-include config.mk

ifeq ($(strip $(COMPONENT_IPNET)),true)
kernel:
ifeq ($(TARGET),rclean)
	@echo "make: removing targets and objects of `pwd`"
	$(MAKE) -C $(IPCOMDIR) TOOL=$(TOOL) CPU=$(CPU) cleanall
else
ifeq ($(DEBUG),true)
	@echo "#"
	@echo "#"
	@echo "######### Build Config Info ########"
	@echo "#"
	@echo "# IP Component Dirs   : $(IP_BUILDABLE_COMPS)"
	@echo "#"
	@echo "# CPU                 = $(CPU)"
	@echo "# TOOL                = $(TOOL)"
	@echo "# VX_CPU_FAMILY       = $(VX_CPU_FAMILY)"
	@echo "# WR_IPCOM_LIB_DIR    = $(WR_IPCOM_LIB_DIR)"
	@echo "# WR_IPCOM_VERSION    = $(WR_IPCOM_VERSION)"
	@echo "# COMP_IPNET_PATH     = $(COMP_IPNET_PATH)"
	@echo "#"
	@echo "####################################"
	@echo "#"
	@echo "#"
else
	$(MAKE) -C $(IPCOMDIR) TOOL=$(TOOL) CPU=$(CPU) all
endif
endif
usr:
	$(warning USER build for IPNET is not yet supported.)
endif

include $(WIND_BASE)/config/common/rules.component
#
# End of feature set build block.
###

else # Not an MSP build

###
# This block is to be used during non-feature set build
#

kernel :
	@echo "make: building targets of $(COMP_IPNET_PATH)"
ifeq ($(DEBUG),true)
	@echo "#"
	@echo "#"
	@echo "######### Build Config Info ########"
	@echo "#"
	@echo "# IP Component Dirs   : $(IP_BUILDABLE_COMPS)"
	@echo "#"
	@echo "# CPU                 = $(CPU)"
	@echo "# TOOL                = $(TOOL)"
	@echo "# VX_CPU_FAMILY       = $(VX_CPU_FAMILY)"
	@echo "# WR_IPCOM_LIB_DIR    = $(WR_IPCOM_LIB_DIR)"
	@echo "# WR_IPCOM_VERSION    = $(WR_IPCOM_VERSION)"
	@echo "# COMP_IPNET_PATH     = $(COMP_IPNET_PATH)"
	@echo "#"
	@echo "####################################"
	@echo "#"
	@echo "#"
else
	$(MAKE) -C $(IPCOMDIR) TOOL=$(TOOL) CPU=$(CPU) all
endif
usr :
	$(warning GPP usr build not supported for IPNET)
	
endif # End MSP Build

rclean :
	@echo "make: removing targets and objects of `pwd`"
	$(MAKE) -C $(IPCOMDIR) TOOL=$(TOOL) CPU=$(CPU) cleanall

.PHONY : util_ipdef util_ipinc

util_ipdef:
	$(MAKE) -C $(IPCOMDIR) TOOL=$(TOOL) CPU=$(CPU) util_ipdef

util_ipinc:
	$(MAKE) -C $(IPCOMDIR) TOOL=$(TOOL) CPU=$(CPU) util_ipinc
