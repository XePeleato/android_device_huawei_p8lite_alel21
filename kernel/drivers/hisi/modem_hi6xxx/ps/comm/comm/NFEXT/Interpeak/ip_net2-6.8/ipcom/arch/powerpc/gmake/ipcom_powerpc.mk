#############################################################################
#			INTERPEAK MAKEFILE
#
#     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_powerpc.mk,v $ $Revision: 1.3 $
#     $Source: /home/interpeak/CVSRoot/ipcom/arch/powerpc/gmake/ipcom_powerpc.mk,v $
#     $Author: ulf $
#     $State: Exp $ $Locker:  $
#
#     INTERPEAK_COPYRIGHT_STRING
#
#############################################################################
IPCOM_ARCH := yes

#############################################################################
# DESCRIPTION
#############################################################################
# IPCOM powerpc arch GNU makefile template.


#############################################################################
# IPDEFINE
#############################################################################

IPDEFINE	+= -DIPCOM_USE_ASM


#############################################################################
# FILE
#############################################################################

ifeq ($(IPPORT),vxworks)
IPLIBOBJECTS	+= ipcom_powerpc_vxworks.o
else
ifeq ($(IPCOMPILER),ghs)
IPLIBOBJECTS	+= ipcom_powerpc_ghs.o
else
IPLIBOBJECTS	+= ipcom_powerpc_gcc.o
endif
endif


#############################################################################
# DEPENDENCIES
#############################################################################

IPASDIRS += $(IPCOM_ROOT)/arch/powerpc/src


#############################################################################
# END OF IPCOM_POWERPC.MK
#############################################################################

