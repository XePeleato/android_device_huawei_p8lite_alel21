#############################################################################
#			      VXNETAPPS.MK
#
#     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: vxbridge.mk,v $ $Revision: 1.1.14.1.2.2 $
#     $Source: /home/interpeak/CVSRoot/vxbridge/gmake/vxbridge.mk,v $
#     $Author: ulf $ $Date: 2010-07-02 13:21:21 $
#     $State: Exp $ $Locker:  $
#
#     INTERPEAK_COPYRIGHT_STRING
#
#############################################################################


#############################################################################
# PRODUCT
###########################################################################
IPPROD ?= vxbridge

#############################################################################
# DEFINE
###########################################################################
IPDEFINE += -DVXBRIDGE
ifdef _WRS_CONFIG_COMPONENT_IPFIREWALL_SUPPORT
IPDEFINE += -DINCLUDE_IPFIREWALL_SUPPORT
IPDEFINE += -DIPCOM_STORE_PHYSICAL_IF
IPDEFINE += -DIPFIREWALL_PHYSICAL_IF_SUPPORT
endif

#############################################################################
# INCLUDE
###########################################################################

IPINCLUDE += -I$(VXBRIDGE_ROOT)/config
IPINCLUDE += -I$(VXBRIDGE_ROOT)/include

#############################################################################
# OBJECTS
###########################################################################

# Configuration

# Main
IPLIBOBJECTS	+= bridge.o
IPLIBOBJECTS	+= mirrorEnd.o
IPLIBOBJECTS	+= mirrorUtils.o

#############################################################################
# VPATH
###########################################################################
IPSRCDIRS += $(VXBRIDGE_ROOT)/src

TGT_DIR=$(WIND_BASE)/target
#DOC_FILES=      bridge.c mirrorEnd.c mirrorUtils.c

ifeq ($(IP_PORT_VXWORKS),)
WR_IPCOM_NAME := vxworks
WR_IPCOM_VERSION := $(filter-out $(WR_IPCOM_NAME), $(subst -, ,$(WIND_PLATFORM)))
WR_IPCOM_VERSION := $(subst .,,$(WR_IPCOM_VERSION))
EXTRA_DEFINE += -DIP_PORT_VXWORKS=$(WR_IPCOM_VERSION)
endif

IPNET_ROOT_PATH := $(subst \,/,$(WRVX_COMPBASE))/$(subst \,/,$(COMP_IPNET2))
INCLUDE_CC += -I$(IPNET_ROOT_PATH)/ipnet2/include
INCLUDE_CC += -I$(IPNET_ROOT_PATH)/ipnet2/config
INCLUDE_CC += -I$(IPNET_ROOT_PATH)/ipcom/port/vxworks/include
INCLUDE_CC += -I$(IPNET_ROOT_PATH)/ipcom/port/vxworks/config
INCLUDE_CC += -I$(IPNET_ROOT_PATH)/ipcom/include
INCLUDE_CC += -I$(IPNET_ROOT_PATH)/ipcom/config

#############################################################################
# LIB
###########################################################################

IPLIBS += $(IPLIBROOT)/libvxbridge.a

###########################################################################
# END BRIDGE.MK
###########################################################################
