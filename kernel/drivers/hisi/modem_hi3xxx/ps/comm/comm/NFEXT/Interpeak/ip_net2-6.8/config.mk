# config.mk - ip_net2 specific configuration
#
# Copyright 2007-2009 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
#
# 
# modification history
# --------------------
# 02r,08jun10,spw  CQ:WIND00216460 Add ifproxy wrnad
# 02q,31jul09,ten  Adding IPL2TP
# 02p,14jul09,mdk  merge from networking branch
# 02o,11jun09,dwb  Adding vxdot1x
# 02n,16dec08,mry  adding IPWPS
# 02m,21apr09,pee  align with changes in default VSB configuration for default
#                  libraries.
# 02l,24mar09,mze  cleanup of INET6 INET6ONLY for WIND00160969
# 02k,11feb09,ggz  Move coreip and bridge into CVS.
# 02j,19aug08,mze  removing warnings for IPNET4
# 02i,12jun08,mze  changing default logic to account for VSB configuration 
# 02h,28may08,dlk  Added non-optional VXMUX.
# 02g,20feb08,tlu  Added IPROHC component
# 02f,14may08,brg  Added IPVXCOMPAT  component
# 02e,08nov07,tlu  Added IPDIAMETERP component
# 02d,07aug07,mry  Add 802.1x supplicant support
# 02c,11oct07,uol  Defect WIND00107491, add DHCP capabilities in bootrom.
# 02b,27sep07,kch  Stripped leading and trailing whitespace (WIND00046447).
# 02a,12sep07,spw  Changed IPWLAN to WLAN
# 01z,17jul07,spw  Added IPWLAN
# 01y,27jun07,mcarlste  Added IPFREESCALE component
# 01x,03apr07,tlu  Added IPDIAMETERC component
# 01x,25apr07,tkf  Added IPv6 Only Build Support.
# 01w,13dec06,tkf  Added OPT=-basic option support.
# 01v,08nov06,kch  Moved FEATURE_IPNET_GEN_DEPEND outside FS_IP_GPP_BUILD 
#                  block.
# 01u,06nov06,tkf  Add FEATURE_IPNET_GEN_DEPEND.
# 01t,03nov06,tkf  Included IPPPP in default build.
# 01s,18oct06,kch  Removed references to the discontinued ipldapc.
# 01r,22sep06,kch  Added IPMPLS, IPEAP, IPLDAPC and IPRADIUS.
# 01q,22sep06,dlk  Add option build support.
# 01p,21sep06,tkf  Added directory check for wrsnmp.
# 01o,18sep06,kch  Added support for COMPONENT_IPPPP. Also include 
#                  COMPONENT_IPDHCPS and COMPONENT_IPDHCPR for gpp default
#                  build.
# 01n,15sep06,tkf  Added IPDHCPC6 and IPDHCPS6. Added more robust directory
#                  checking.
# 01m,06sep06,tkf  Moved configurable macros from ipcom/Makefile so that
#                  they can be managed from platform config.mk. Also include
#                  IPRIPNG only if we are building for IPv6.
# 01l,06sep06,kch  Temporarily allow COMPONENT_IPRIPNG build for GPP.
# 01k,06sep06,kch  Remove COMPONENT_IPRIPNG from default GPP build.
# 01j,01sep06,tlu  Remove debugging message for RIPng build
# 01i,04sep06,kch  Added support for COMPONENT_IPDHCPS and COMPONENT_IPDHCPR.
# 01h,04sep06,tkf  Replace any backslash characters in BASE_ROOT with forward
#                  slash characters.
# 01g,07aug06,kch  Added support for COMPONENT_IPAPPL, COMPONENT_IPSNTP,
#                  COMPONENT_IPFIREWALL and COMPONENT_IPMCP.
# 01f,25jul06,kch  Added COMPONENT_IPRIP support.
# 01e,18jul06,tlu  Added COMPONENT_IPRIPNG support.
# 01d,02jun06,kch  Added SKIP_CONFIG make macro to support configuration
#                  files override for IPNet build.
# 01c,18jun06,tkf  IPNET component directories are determined dynamically.
# 01b,10may06,tkf  IPNET can now be built from both GPP and MSP.
# 01b,10may06,tkf  Add IPNET component configuration defines.
# 01a,01may06,j_c  Written.
#
# DESCRIPTION
# This file implements the IPNET component configuration as dictated by
# the current feature set (included before this fragment).
#

#ifeq ($(EXTRA_DEFINE),)
#export EXTRA_DEFINE =
#endif

######
# Default values
######



ifeq ($(FEATURE_IP_GPP_BUILD),true)
FS_IP_GPP_BUILD = true
else
FS_IP_GPP_BUILD = false
endif

ifeq ($(FS_IP_GPP_BUILD),true)
	ifeq ($(findstring -basic,$(OPT)),-basic)
	IP_GPP_BUILD_BASIC = true
	EXTRA_DEFINE				+= -DIPBASIC
	endif
endif

ifeq ($(TGT_DIR),)
TGT_DIR = $(WIND_BASE)/target
endif

include $(TGT_DIR)/h/make/defs.option



ifeq ($(findstring -inet6,$(OPT)),-inet6)
_INET6_BUILD = 1

endif

ifeq ($(findstring -inet6only,$(OPT)),-inet6only)
_INET6_BUILD_ONLY = 1
endif

###
# User configured macros
###
#
# NOTE:
#	These macros are now defined in the common.vxconfig file that 
# 	exists for each component, all MSP/GPP logic for these macros 
# 	should be part of the vxconfig file which is based on the 
#	Linux Kconfig 
#	
#

ifeq ($(_INET6_BUILD),)
export FEATURE_IPNET_INET6 = false
else
export FEATURE_IPNET_INET6 = true
endif

ifeq ($(_INET6_BUILD_ONLY),)
export FEATURE_IPNET_INET6_ONLY = false
else
export FEATURE_IPNET_INET6_ONLY = true
endif

#if this build is for IPV4 then disable the IPV6 and 6 only components that may have been set
ifeq ($(_INET6_BUILD)$(_INET6_BUILD_ONLY)$(IP_GPP_BUILD_BASIC),) 
#$(warning doing a INET4 build)
export COMPONENT_IPDHCPC6 	= false
export COMPONENT_IPDHCPS6 	= false
export COMPONENT_IPMIP6 	= false
export COMPONENT_IPMIP6HA	= false
export COMPONENT_IPMIP6MN 	= false
export COMPONENT_IPMIP6PM	= false
export COMPONENT_IPRIPNG 	= false
else

#override configuration to support old OPT build
ifneq ($(OPT),)
export COMPONENT_IPDHCPC6       = true
export COMPONENT_IPDHCPS6       = true
export COMPONENT_IPRIPNG        = true
endif

#$(warning Not doing a INET4 build)
endif

ifneq ($(IP_GPP_BUILD_BASIC),)
#$(warning building basic)
export COMPONENT_IPMIP6         = false
export COMPONENT_IPMIP6HA       = false
export COMPONENT_IPMIP6MN       = false
export COMPONENT_IPMIP6PM       = false
export COMPONENT_IPRIP	      	= false
export COMPONENT_IPSNTP		= false
export COMPONENT_IPDHCPS    	= false
export COMPONENT_IPDHCPR    	= false
export COMPONENT_IPPPP      	= false
export COMPONENT_IPRIPNG	= false
export COMPONENT_IPDHCPC6	= false
export COMPONENT_IPDHCPS6	= false
endif



# 
# Set $(SKIP_CONFIG) to true if the $(SKIP_CONFIG) is not yet defined at
# this point. This forces the IPNet not to compile the xxx_config.c
# when building the IPNet archives. This macro is defined here but its
# value is checked in the IPNet makefiles. Use the conditional variable
#
export SKIP_CONFIG ?= true


#
# Derived Macros
#
BASE_ROOT					?= $(COMP_IPNET_PATH)
BASE_ROOT                   := $(subst \,/,$(BASE_ROOT))

ifeq ($(strip $(COMPONENT_IPNET)),true)
ifeq ($(strip $(COMPONENT_IPLITE)),true)
$(error "Can only include COMPONENT_IPNET or COMPONENT_IPLITE, not both")
endif
IPNET_ROOT					= $(BASE_ROOT)/$(IPNET2DIR)
export WRS_IPNET			= true
ifneq ($(findstring -DWRS_IPNET,$(EXTRA_DEFINE)),-DWRS_IPNET)
EXTRA_DEFINE				+= -DWRS_IPNET
endif
endif

ifeq ($(strip $(COMPONENT_IPLITE)),true)
ifeq ($(strip $(COMPONENT_IPNET)),true)
$(error "Can only include COMPONENT_IPNET or COMPONENT_IPLITE, not both")
endif
IPLITE_ROOT	 = $(BASE_ ROOT)/$(IPLITEDIR)
endif

ifeq ($(strip $(COMPONENT_IPTCP)),true)
ifeq ($(IPTCPDIR),)
$(error IPTCP is not installed)
endif
IPTCP_ROOT	= $(BASE_ROOT)/$(IPTCPDIR)
endif

ifeq ($(strip $(COMPONENT_IPSCTP)),true)
ifeq ($(IPSCTPDIR),)
$(error IPSCTP is not installed)
endif
IPSCTP_ROOT	= $(BASE_ROOT)/$(IPSCTPDIR)
endif

ifeq ($(strip $(COMPONENT_IPSSH)),true)
ifeq ($(IPSSHDIR),)
$(error IPSSH is not installed)
endif
IPSSH_ROOT	= $(BASE_ROOT)/$(IPSSHDIR)
endif

ifeq ($(strip $(COMPONENT_IPIKE)),true)
ifeq ($(IPIKEDIR),)
$(error IPIKE is not installed)
endif
IPIKE_ROOT	= $(BASE_ROOT)/$(IPIKEDIR)
endif

ifeq ($(strip $(COMPONENT_IPIPSEC)),true)
ifeq ($(IPIPSEC2DIR),)
$(error IPIPSEC2 is not installed)
endif
IPIPSEC_ROOT	= $(BASE_ROOT)/$(IPIPSEC2DIR)
endif

ifeq ($(strip $(COMPONENT_IPHWCRYPTO)),true)
ifeq ($(IPHWCRYPTODIR),)
$(error IPHWCRYPTO is not installed)
endif
IPHWCRYPTO_ROOT	= $(BASE_ROOT)/$(IPHWCRYPTODIR)
endif

ifeq ($(strip $(COMPONENT_IPMCRYPTO)),true)
ifeq ($(IPMCRYPTODIR),)
$(error IPMCRYPTO is not installed)
endif
IPMCRYPTO_ROOT	= $(BASE_ROOT)/$(IPMCRYPTODIR)
endif


ifeq ($(strip $(COMPONENT_IPCRYPTO)),true)
ifeq ($(IPCRYPTODIR),)
$(error IPCRYPTO is not installed)
endif
IPCRYPTO_ROOT	= $(BASE_ROOT)/$(IPCRYPTODIR)
endif

ifeq ($(strip $(COMPONENT_IPSSL)),true)
ifeq ($(IPSSL2DIR),)
$(error IPSSL2 is not installed)
endif
IPSSL_ROOT	= $(BASE_ROOT)/$(IPSSL2DIR)
endif


ifeq ($(strip $(COMPONENT_IPXINC)),true)
ifeq ($(IPXINCDIR),)
$(error IPXINC is not installed)
endif
IPXINC_ROOT	= $(BASE_ROOT)/$(IPXINCDIR)
endif

ifeq ($(strip $(COMPONENT_IPCCI)),true)
ifeq ($(IPCCIDIR),)
$(error IPCCI is not installed)
endif
IPCCI_ROOT	= $(BASE_ROOT)/$(IPCCIDIR)
endif

ifeq ($(strip $(COMPONENT_IPMIP)),true)
ifeq ($(IPMIPDIR),)
$(error IPMIP is not installed)
endif
IPMIP_ROOT	 = $(BASE_ROOT)/$(IPMIPDIR)
endif

ifeq ($(strip $(COMPONENT_IPMIP4)),true)
ifeq ($(IPMIP4DIR),)
$(error IPMIP4 is not installed)
endif
IPMIP4_ROOT	 = $(BASE_ROOT)/$(IPMIP4DIR)
endif

ifeq ($(strip $(COMPONENT_WLAN)),true)
ifeq ($(IPWLANDIR),)
$(error IPWLAN is not installed)
endif
IPWLAN_ROOT	= $(BASE_ROOT)/$(IPWLANDIR)
endif

ifeq ($(strip $(COMPONENT_IPMIPFA)),true)
ifeq ($(IPMIPFADIR),)
$(error IPMIPFA is not installed)
endif
IPMIPFA_ROOT	 = $(BASE_ROOT)/$(IPMIPFADIR)
endif

ifeq ($(strip $(COMPONENT_IPMIPHA)),true)
ifeq ($(IPMIPHADIR),)
$(error IPMIPHA is not installed)
endif
IPMIPHA_ROOT	 = $(BASE_ROOT)/$(IPMIPHADIR)
endif

ifeq ($(strip $(COMPONENT_IPMIPMN)),true)
ifeq ($(IPMIPMNDIR),)
$(error IPMIPMN is not installed)
endif
IPMIPMN_ROOT	 = $(BASE_ROOT)/$(IPMIPMNDIR)
endif

ifeq ($(strip $(COMPONENT_IPMIPPM)),true)
ifeq ($(IPMIPPMDIR),)
$(error IPMIPPM is not installed)
endif
IPMIPPM_ROOT    = $(BASE_ROOT)/$(IPMIPPMDIR)
endif

ifeq ($(strip $(COMPONENT_IPMIP6)),true)
ifeq ($(IPMIP6DIR),)
$(error IPMIP6 is not installed)
endif
IPMIP6_ROOT		 = $(BASE_ROOT)/$(IPMIP6DIR)
endif

ifeq ($(strip $(COMPONENT_IPMIP6HA)),true)
ifeq ($(IPMIP6HADIR),)
$(error IPMIP6HA is not installed)
endif
IPMIP6HA_ROOT    = $(BASE_ROOT)/$(IPMIP6HADIR)
endif

ifeq ($(strip $(COMPONENT_IPMIP6MN)),true)
ifeq ($(IPMIP6MNDIR),)
$(error IPMIP6MN is not installed)
endif
IPMIP6MN_ROOT	 = $(BASE_ROOT)/$(IPMIP6MNDIR)
endif

ifeq ($(strip $(COMPONENT_IPMIP6PM)),true)
ifeq ($(IPMIP6PMDIR),)
$(error IPMIP6PM is not installed)
endif
IPMIP6PM_ROOT    = $(BASE_ROOT)/$(IPMIP6PMDIR)
endif

ifeq ($(strip $(COMPONENT_MPLS)),true)
ifeq ($(IPMPLSDIR),)
$(error IPMPLS is not installed)
endif
IPMPLS_ROOT		 = $(BASE_ROOT)/$(IPMPLSDIR)
endif

ifeq ($(strip $(COMPONENT_IPRIPNG)),true)
ifeq ($(IPRIPNGDIR),)
$(error IPRIPNG is not installed)
endif
IPRIPNG_ROOT      = $(BASE_ROOT)/$(IPRIPNGDIR)
endif

ifeq ($(strip $(COMPONENT_IPRIP)),true)
ifeq ($(IPRIPDIR),)
$(error IPRIP is not installed)
endif
IPRIP_ROOT		 = $(BASE_ROOT)/$(IPRIPDIR)
endif

ifeq ($(strip $(COMPONENT_IPAPPL)),true)
ifeq ($(IPAPPLDIR),)
$(error IPAPPL is not installed)
endif
IPAPPL_ROOT		 = $(BASE_ROOT)/$(IPAPPLDIR)
endif

ifeq ($(strip $(COMPONENT_IPSNTP)),true)
ifeq ($(IPSNTPDIR),)
$(error IPSNTP is not installed)
endif
IPSNTP_ROOT		 = $(BASE_ROOT)/$(IPSNTPDIR)
endif

ifeq ($(strip $(COMPONENT_IPMCP)),true)
ifeq ($(IPMCPDIR),)
$(error IPMCP is not installed)
endif
IPMCP_ROOT		 = $(BASE_ROOT)/$(IPMCPDIR)
endif

ifeq ($(strip $(COMPONENT_IPFIREWALL)),true)
ifeq ($(IPFIREWALLDIR),)
$(error IPFIREWALL is not installed)
endif
IPFIREWALL_ROOT		 = $(BASE_ROOT)/$(IPFIREWALLDIR)
endif

ifeq ($(strip $(COMPONENT_IPDHCPS)),true)
ifeq ($(IPDHCPSDIR),)
$(error IPDHCP is not installed)
endif
IPDHCPS_ROOT     = $(BASE_ROOT)/$(IPDHCPSDIR)
endif

ifeq ($(strip $(COMPONENT_IPDHCPR)),true)
ifeq ($(IPDHCPRDIR),)
$(error IPDHCPR is not installed)
endif
IPDHCPR_ROOT     = $(BASE_ROOT)/$(IPDHCPRDIR)
endif

ifeq ($(strip $(COMPONENT_IPDHCPC6)),true)
ifeq ($(IPDHCPC6DIR),)
$(error IPDHCPR is not installed)
endif
IPDHCPC6_ROOT     = $(BASE_ROOT)/$(IPDHCPC6DIR)
endif

ifeq ($(strip $(COMPONENT_IPDHCPS6)),true)
ifeq ($(IPDHCPS6DIR),)
$(error IPDHCPR is not installed)
endif
IPDHCPS6_ROOT     = $(BASE_ROOT)/$(IPDHCPS6DIR)
endif

ifeq ($(strip $(COMPONENT_IPL2TP)),true)
ifeq ($(IPL2TPDIR),)
$(error IPL2TPDIR is not installed)
endif
IPL2TP_ROOT     = $(BASE_ROOT)/$(IPL2TPDIR)
endif

ifeq ($(strip $(COMPONENT_IPPPP)),true)
ifeq ($(IPPPPDIR),)
$(error IPPPPDIR is not installed)
endif
IPPPP_ROOT     = $(BASE_ROOT)/$(IPPPPDIR)
endif

ifeq ($(strip $(COMPONENT_IPMPLS)),true)
ifeq ($(IPMPLSDIR),)
$(error IPMPLSDIR is not installed)
endif
IPMPLS_ROOT     = $(BASE_ROOT)/$(IPMPLSDIR)
endif

ifeq ($(strip $(COMPONENT_IPEAP)),true)
ifeq ($(IPEAPDIR),)
$(error IPEAPDIR is not installed)
endif
IPEAP_ROOT     = $(BASE_ROOT)/$(IPEAPDIR)
endif

ifeq ($(COMPONENT_IP8021X),true)
ifeq ($(IP8021XDIR),)
$(error IP8021XDIR is not installed)
endif
IP8021X_ROOT     = $(BASE_ROOT)/$(IP8021XDIR)
endif

ifeq ($(COMPONENT_IPWPS),true)
ifeq ($(IPWPSDIR),)
$(error IPWPSDIR is not installed)
endif
IPWPS_ROOT     = $(BASE_ROOT)/$(IPWPSDIR)
endif

ifeq ($(strip $(COMPONENT_IPRADIUS)),true)
ifeq ($(IPRADIUSDIR),)
$(error IPRADIUSDIR is not installed)
endif
IPRADIUS_ROOT     = $(BASE_ROOT)/$(IPRADIUSDIR)
endif

ifeq ($(strip $(COMPONENT_IPDIAMETERC)),true)
ifeq ($(IPDIAMETERDIR),)
$(error IPDIAMETERDIR is not installed)
endif
IPDIAMETER_ROOT     = $(BASE_ROOT)/$(IPDIAMETERDIR)
endif

ifeq ($(strip $(COMPONENT_IPDIAMETERP)),true)
ifeq ($(IPDIAMETERDIR),)
$(error IPDIAMETERDIR is not installed)
endif
IPDIAMETER_ROOT     = $(BASE_ROOT)/$(IPDIAMETERDIR)
endif

ifeq ($(strip $(COMPONENT_IPROHC)),true)
ifeq ($(IPROHCDIR),)
$(error IPROHCDIR is not installed)
endif
IPROHC_ROOT     = $(BASE_ROOT)/$(IPROHCDIR)
endif

ifeq ($(strip $(COMPONENT_IPFREESCALE)),true)
ifeq ($(IPFREESCALEDIR),)
$(error IPFREESCALE is not installed)
endif
IPFREESCALE_ROOT    = $(BASE_ROOT)/$(IPFREESCALEDIR)
endif

ifeq ($(strip $(COMPONENT_IPCAVIUM)),true)
ifeq ($(IPCAVIUMDIR),)
$(error IPCAVIUM is not installed)
endif
IPCAVIUM_ROOT    = $(BASE_ROOT)/$(IPCAVIUMDIR)
endif

ifeq ($(strip $(COMPONENT_VXCOMPAT)),true)
ifeq ($(VXCOMPATDIR),)
$(error VXCOMPAT is not installed)
endif
VXCOMPAT_ROOT    = $(BASE_ROOT)/$(VXCOMPATDIR)
endif

ifeq ($(strip $(COMPONENT_BRIDGE)),true)
ifeq ($(BRIDGEDIR),)
$(error BRIDGE is not installed)
endif
VXBRIDGE_ROOT    = $(BASE_ROOT)/$(BRIDGEDIR)
endif

ifeq ($(strip $(COMPONENT_DOT1X)),true)
ifeq ($(VXDOT1XDIR),)
$(error VXDOT1X is not installed)
endif
VXDOT1X_ROOT    = $(BASE_ROOT)/$(VXDOT1XDIR)
endif

ifeq ($(strip $(COMPONENT_WRNAD)),true)
ifeq ($(WRNADDIR),)
$(error WRNAD is not installed)
endif
WRNAD_ROOT    = $(BASE_ROOT)/$(WRNADDIR)
endif

ifeq ($(strip $(COMPONENT_IFPROXY)),true)
ifeq ($(IFPROXYDIR),)
$(error IFPROXY is not installed)
endif
IFPROXY_ROOT    = $(BASE_ROOT)/$(IFPROXYDIR)
endif

# VXMUX is not really optional.
ifeq ($(VXMUXDIR),)
$(error VXMUX is not installed)
endif
VXMUX_ROOT    = $(BASE_ROOT)/$(VXMUXDIR)

# VXCOREIP is not optional.
ifeq ($(VXCOREIPDIR),)
$(error VXCOREIP is not installed)
endif
VXCOREIP_ROOT    = $(BASE_ROOT)/$(VXCOREIPDIR)

####
# If user specified IPv6 only stack, we should exclude IPV4 support.
#

ifeq ($(strip $(FEATURE_IPNET_INET6_ONLY)),true)
IPDEFINE ?=
ifneq ($(findstring -DINET6,$(ADDED_CFLAGS) $(EXTRA_DEFINE)),-DINET6)
EXTRA_DEFINE += -DINET6
endif
export IPCOM_USE_INET6 = yes
IPDEFINE += -DIPCOM_EXCLUDE_INET
else
ifneq ($(findstring -DINET,$(ADDED_CFLAGS) $(EXTRA_DEFINE)),-DINET)
EXTRA_DEFINE += -DINET
endif
ifeq ($(strip $(FEATURE_IPNET_INET6)),true)
ifneq ($(findstring -DINET6,$(ADDED_CFLAGS) $(EXTRA_DEFINE)),-DINET6)
EXTRA_DEFINE += -DINET6
endif
export IPCOM_USE_INET6 = yes
else
export IPCOM_USE_INET6 = no
endif
endif

ifeq ($(strip $(COMPONENT_SNMP)),true)
ifeq ($(WRSNMP),)
$(error WRSNMP is not installed)
endif
WRSNMP_ROOT          = $(BASE_ROOT)/$(WRSNMP)
endif

export IPBUILD ?= $(strip $(FEATURE_IPNET_BUILD))

ifeq ($(IPGENDEPEND),)
ifeq ($(strip $(FEATURE_IPNET_GEN_DEPEND)),true)
export IPGENDEPEND = yes
else
export IPGENDEPEND = no
endif
endif

ifeq ($(IPVERBOSE),)
ifeq ($(strip $(FEATURE_IPNET_VERBOSE)),true)
export IPVERBOSE = yes
else
export IPVERBOSE = no
endif
endif
