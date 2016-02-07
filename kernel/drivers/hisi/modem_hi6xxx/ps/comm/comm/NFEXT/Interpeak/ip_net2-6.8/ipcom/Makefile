#############################################################################
#			INTERPEAK MAKEFILE
#
#     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: Makefile,v $ $Revision: 1.59 $
#     $Source: /home/interpeak/CVSRoot/ipcom/Makefile,v $
#     $Author: dbush $
#     $State: Exp $ $Locker:  $
#
#     INTERPEAK_COPYRIGHT_STRING
#
#############################################################################

#############################################################################
# GENERAL
#############################################################################

# RTOS port
IPPORT	= vxworks

ifeq ($(COMP_IPNET_PATH),)
    IPGMAKE = yes
else
    # Building from within the VxWorks build system
    IPGMAKE = no
    IPPORT  = vxworks
endif

#############################################################################
# CONFIGURATION
#############################################################################
ifeq ($(IPGMAKE),yes)

# IPBUILD = debug | speed | debugspeed | size
IPBUILD	= debug

# IPVERBOSE = no | yes
IPVERBOSE = no

# IPCOM_USE_INET6 = yes | no
IPCOM_USE_INET6 = yes

# FEATURE_SET = msp | gpp (vxworks only)
FEATURE_SET = msp

endif
#############################################################################
# IPMAKEFILE
#############################################################################
# If you move/rename this Makefile, please specify full filename!

#export IPMAKEFILE := /my/new/location/Makefile


#############################################################################
# FEATURE SET BUILD
#############################################################################

ifeq ($(WIND_IPNET),true)
include $(BASE_ROOT)/config.mk
ifneq ($(FEATURE_SET),)
include $(WIND_BASE)/config/common/defs.component
endif
endif


#############################################################################
# PRODUCT PATHS
#############################################################################

BASE_ROOT 	?= $(CURDIR)/..

IPCOM_ROOT      ?= $(CURDIR)

ifeq ($(IPGMAKE),yes)
#PRODLIST_START

#IPNET_ROOT       = $(BASE_ROOT)/ipnet2-any-r6_7_1
#IPLITE_ROOT	  = $(BASE_ROOT)/iplite2-any-r6_7_1
#IPTCP_ROOT       = $(BASE_ROOT)/iptcp-any-r6_7_1

#IP8021X_ROOT	  = $(BASE_ROOT)/ip8021x-any-r6_7_1
#IPAPPL_ROOT	  = $(BASE_ROOT)/ipappl-any-r6_7_1
#IPDHCPR_ROOT	  = $(BASE_ROOT)/ipdhcpr-any-r6_7_1
#IPDHCPS6_ROOT	  = $(BASE_ROOT)/ipdhcps6-any-r6_7_1
#IPDHCPS_ROOT	  = $(BASE_ROOT)/ipdhcps-any-r6_7_1
#IPDIAMETER_ROOT  = $(BASE_ROOT)/ipdiameter-any-r6_7_1
#IPEAP_ROOT	  = $(BASE_ROOT)/ipeap-any-r6_7_1
#IPEMANATE_ROOT	  = $(BASE_ROOT)/ipemanate-any-r6_7_1
#IPFIREWALL_ROOT  = $(BASE_ROOT)/ipfirewall-any-r6_7_1
#IPFREESCALE_ROOT = $(BASE_ROOT)/ipfreescale-any-r6_7_1
#IPHWCRYPTO_ROOT  = $(BASE_ROOT)/iphwcrypto-any-r6_7_1
#IPIKE_ROOT	  = $(BASE_ROOT)/ipike-any-r6_7_1
#IPIPSEC_ROOT	  = $(BASE_ROOT)/ipipsec-$(IPPORT)-r6_7_1
#IPIPSEC_ROOT	  = $(BASE_ROOT)/ipipsec2-r6_7_1
#IPL2TP_ROOT	  = $(BASE_ROOT)/ipl2tp-any-r6_7_1
#IPLDAPC_ROOT	  = $(BASE_ROOT)/ipldapc-any-r6_7_1
#IPMCP_ROOT       = $(BASE_ROOT)/ipmcp-any-r6_7_1
#IPMIP_ROOT	  = $(BASE_ROOT)/ipmip-any-r6_7_1
#IPMIP4_ROOT	  = $(BASE_ROOT)/ipmip4-any-r6_7_1
#IPMIP6CN_ROOT	  = $(BASE_ROOT)/ipmip6cn-any-r6_7_1
#IPMIP6HA_ROOT	  = $(BASE_ROOT)/ipmip6ha-any-r6_7_1
#IPMIP6MN_ROOT	  = $(BASE_ROOT)/ipmip6mn-any-r6_7_1
#IPMIP6PM_ROOT	  = $(BASE_ROOT)/ipmip6pm-any-r6_7_1
#IPMIP6_ROOT	  = $(BASE_ROOT)/ipmip6-any-r6_7_1
#IPMIPFA_ROOT	  = $(BASE_ROOT)/ipmipfa-any-r6_7_1
#IPMIPHA_ROOT	  = $(BASE_ROOT)/ipmipha-any-r6_7_1
#IPMIPMN_ROOT	  = $(BASE_ROOT)/ipmipmn-any-r6_7_1
#IPMIPPM_ROOT	  = $(BASE_ROOT)/ipmippm-any-r6_7_1
#IPMPLS_ROOT	  = $(BASE_ROOT)/ipmpls-any-r6_7_1
#IPNETPERF_ROOT	  = $(BASE_ROOT)/ipnetperf-any-r6_7_1
#IPNGC_ROOT	  = $(BASE_ROOT)/ipngc-any-r6_7_1
#IPPPP_ROOT	  = $(BASE_ROOT)/ipppp-any-r6_7_1
#IPPRISM_ROOT     = $(BASE_ROOT)/ipprism-$(IPPORT)-r6_7_1
#IPRADIUS_ROOT	  = $(BASE_ROOT)/ipradius-any-r6_7_1
#IPRIPNG_ROOT	  = $(BASE_ROOT)/ipripng-any-r6_7_1
#IPRIP_ROOT	  = $(BASE_ROOT)/iprip-any-r6_7_1
#IPSCTP_ROOT	  = $(BASE_ROOT)/ipsctp-any-r6_7_1
#IPSNMP_ROOT	  = $(BASE_ROOT)/ipsnmp-any-r6_7_1
#IPSNTP_ROOT	  = $(BASE_ROOT)/ipsntp-any-r6_7_1
#IPSSH_ROOT	  = $(BASE_ROOT)/ipssh-any-r6_7_1
#IPSSLPROXY_ROOT  = $(BASE_ROOT)/ipsslproxy-any-r6_7_1
#IPSSL_ROOT	  = $(BASE_ROOT)/ipssl2-any-r6_7_1
#IPWEBS_ROOT	  = $(BASE_ROOT)/ipwebs-any-r6_7_1
#IPXINC_ROOT	  = $(BASE_ROOT)/ipxinc-any-r6_7_1
#IPROUTE2_ROOT	  = $(BASE_ROOT)/iproute2
#VXCOMPAT_ROOT	  = $(BASE_ROOT)/vxcompat
#VXMUX_ROOT	  = $(BASE_ROOT)/vxmux
#VXBRIDGE_ROOT    = $(BASE_ROOT)/vxbridge
#VXCOREIP_ROOT    = $(BASE_ROOT)/vxcoreip
#VXDOT1X_ROOT    = $(BASE_ROOT)/vxdot1x

# Choose one of ipcrypto or ipmcrypto:
#IPCRYPTO_ROOT	  = $(BASE_ROOT)/ipmcrypto-any-r6_7_1
#IPCRYPTO_ROOT	  = $(BASE_ROOT)/ipcrypto-any-r6_7_1

#PRODLIST_END
endif

#############################################################################
# IPOBJDIR
#############################################################################
# Object/library/binary directory.

ifeq ($(WIND_IPNET),true)
IPOBJDIR = $(WR_IPCOM_LIB_DIR)/objipcom$(OPT)
else
ifeq ($(IPPORT),lkm)
IPOBJDIR = $(IPCOM_ROOT)/obj-$(IPPORT)-$(IPTARGET)-$(IPVER)-gcc-$(IPBUILD)
else
ifeq ($(IPPORT),las)
IPOBJDIR = $(IPCOM_ROOT)/obj-$(IPPORT)-$(IPTARGET)-$(IPVER)-gcc-$(IPBUILD)
else
ifeq ($(IPPORT),rtcore)
IPOBJDIR = $(IPCOM_ROOT)/obj-$(IPPORT)-$(IPVER)-$(IPCOMPILER)-$(IPBUILD)
else
IPOBJDIR = $(IPCOM_ROOT)/obj-$(IPPORT)-$(IPTARGET)-$(IPVER)-$(IPCOMPILER)-$(IPBUILD)
endif
endif
endif
endif

#############################################################################
# DO NOT EDIT BELOW!
#############################################################################

include $(IPCOM_ROOT)/gmake/ipcom_defs.mk


#############################################################################
# END OF Makefile
#############################################################################
