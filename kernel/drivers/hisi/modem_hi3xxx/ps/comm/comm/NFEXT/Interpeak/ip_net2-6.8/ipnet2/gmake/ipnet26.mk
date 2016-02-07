#############################################################################
#			      IPNET26.MK
#
#     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet26.mk,v $ $Revision: 1.16 $
#     $Source: /home/interpeak/CVSRoot/ipnet2/gmake/ipnet26.mk,v $
#     $Author: jonas $ $Date: 2009-03-05 12:52:20 $
#     $State: Exp $ $Locker:  $
#
#     INTERPEAK_COPYRIGHT_STRING
#
#############################################################################


#############################################################################
# DEFINE
###########################################################################

ifneq ($(IPPORT),lkm)
IPDEFINE 	+= -DIPCOM_USE_INET6
endif

#############################################################################
# OBJECTS
###########################################################################



ifneq ($(IPPORT),lkm)
IPLIBOBJECTS += ipnet_radvd_lib.o

ifneq ($(IPPORT),las)
# Config
IPLIBOBJECTS_C += ipnet_radvd_config.o

# IPv6 shell commands
IPLIBOBJECTS += ipnet_cmd_ndp.o
IPLIBOBJECTS += ipnet_cmd_ping6.o
IPLIBOBJECTS += ipnet_cmd_radvd.o
IPLIBOBJECTS += ipnet_radvd.o

# Compiles the xxx_config.o if the $SKIP_CONFIG macro is either not defined
# or set to anything other than true.
ifneq ($(SKIP_CONFIG),true)
IPLIBOBJECTS    += $(IPLIBOBJECTS_C)
endif

endif

endif

#############################################################################
# Markets Specific Platform
###########################################################################

ifneq ($(FEATURE_SET),)
ifneq ($(FEATURE_SET),gpp)
-include $(IPNET_ROOT)/gmake/ipnet26_msp.mk
endif
endif


###########################################################################
# END OF IPNET26.MK
###########################################################################
