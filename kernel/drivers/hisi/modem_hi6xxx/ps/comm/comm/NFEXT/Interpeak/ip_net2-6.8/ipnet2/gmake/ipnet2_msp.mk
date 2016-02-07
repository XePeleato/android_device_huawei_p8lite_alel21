#############################################################################
#			      IPNET26.MK
#
#     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet2_msp.mk,v $ $Revision: 1.6 $
#     $Source: /home/interpeak/CVSRoot/ipnet2/gmake/ipnet2_msp.mk,v $
#     $Author: rboden $ $Date: 2008-08-11 12:04:27 $
#     $State: Exp $ $Locker:  $
#
#     INTERPEAK_COPYRIGHT_STRING
#
#############################################################################

#############################################################################
# DEFINE
###########################################################################

#############################################################################
# OBJECTS
###########################################################################

ifneq ($(IPPORT),lkm)
ifneq ($(IPPORT),las)
# Vrrp daemon
IPLIBOBJECTS_C += ipnet_vrrpd_config.o
IPLIBOBJECTS += ipnet_vrrpd.o
IPLIBOBJECTS += ipnet_cmd_nat.o
endif
endif


# Compiles the xxx_config.o if the $SKIP_CONFIG macro is either not defined
# or set to anything other than true.
ifneq ($(SKIP_CONFIG),true)
IPLIBOBJECTS    += $(IPLIBOBJECTS_C)
endif

###########################################################################
# END OF IPNET2_MSP.MK
###########################################################################
