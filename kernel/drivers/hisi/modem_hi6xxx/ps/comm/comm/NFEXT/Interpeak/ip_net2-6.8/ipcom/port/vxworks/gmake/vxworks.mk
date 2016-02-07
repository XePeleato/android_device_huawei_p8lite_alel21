#############################################################################
#			INTERPEAK MAKEFILE
#
#     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: vxworks.mk,v $ $Revision: 1.22.24.1 $
#     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/gmake/vxworks.mk,v $
#     $Author: ulf $
#     $State: Exp $ $Locker:  $
#
#     INTERPEAK_COPYRIGHT_STRING
#
#############################################################################

#############################################################################
# PRODUCT
###########################################################################

IPPROD ?= ipcom_vxworks
include $(IPCOM_ROOT)/port/gmake/ipcom_port.mk


#############################################################################
# INCLUDE
#############################################################################

IPLIBINCLUDE	+= -I$(WIND_BASE)/target/config/$(IPBOARD)
IPLIBINCLUDE	+= -I$(WIND_BASE)/target/config/all
IPLIBINCLUDE	+= -I$(WIND_BASE)/target/h
IPLIBINCLUDE	+= -I$(WIND_BASE)/target/h
IPLIBINCLUDE	+= -I$(WIND_BASE)/target/h/wrn/coreip
IPLIBINCLUDE	+= -I$(WIND_BASE)/target/src/config


ifneq ($(BSPNAME),)
-include $(WIND_BASE)/target/config/$(BSPNAME)/sdk_include.mk
IPLIBINCLUDE += $(SDK_INCLUDE)
endif


#############################################################################
# OBJECTS
#############################################################################

IPLIBOBJECTS	+= ipcom_atomic.o
IPLIBOBJECTS	+= ipcom_ccivalidate.o
IPLIBOBJECTS	+= ipcom_cmd_vxshell.o
IPLIBOBJECTS	+= ipcom_job_queue.o
IPLIBOBJECTS	+= ipcom_pfile.o
IPLIBOBJECTS	+= ipcom_drv_mux.o
IPLIBOBJECTS	+= ipcom_pshell.o
IPLIBOBJECTS	+= ipcom_psock.o
IPLIBOBJECTS	+= ipcom_rwsem.o
IPLIBOBJECTS	+= ipcom_shellcmd_vxworks.o
IPLIBOBJECTS	+= ipcom_spinlock.o
IPLIBOBJECTS	+= ipcom_windnet_compat.o
IPLIBOBJECTS	+= ipcom_windview.o
IPLIBOBJECTS    += zbufLib.o
IPLIBOBJECTS    += zbufSockLib.o

ifeq ($(IPBOARD),)
IPLIBOBJECTS	+= ipcom_vxworks.o
endif

ifneq ($(WIND_IPNET),)
IPLIBOBJECTS	+= ipcom_mcmd_ifconfig.o
IPLIBOBJECTS	+= ipcom_mcmd_route.o
IPLIBOBJECTS	+= ipcom_ramdisk.o
endif

#############################################################################
# Market Specific Platform
###########################################################################

ifneq ($(FEATURE_SET),)
ifneq ($(FEATURE_SET),gpp)
IPLIBOBJECTS	+= ipcom_vxworks_forwarder.o
IPLIBOBJECTS    += ipcom_vxworks_nehalem_forwarder_common.o
IPLIBOBJECTS    += ipcom_vxworks_nehalem_forwarder_ctrl.o
IPLIBOBJECTS    += ipcom_vxworks_nehalem_forwarder_data.o
IPLIBOBJECTS    += ipcom_vxworks_nehalem_forwarder_mipc.o
IPLIBOBJECTS	+= ipcom_ipc_spinlock.o
IPLIBOBJECTS	+= ipcom_shared_mem.o
endif
endif

#############################################################################
# END OF VXWORKS.MK
#############################################################################
