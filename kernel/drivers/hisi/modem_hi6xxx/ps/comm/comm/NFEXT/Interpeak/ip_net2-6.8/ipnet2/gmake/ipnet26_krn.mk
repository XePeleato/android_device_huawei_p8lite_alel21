#############################################################################
#			      IPNET26.MK
#
#     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet26_krn.mk,v $ $Revision: 1.10 $
#     $Source: /home/interpeak/CVSRoot/ipnet2/gmake/ipnet26_krn.mk,v $
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

# IPv6
IPLIBOBJECTS += ipnet_icmp6.o
IPLIBOBJECTS += ipnet_ip6.o
ifneq ($(WIND_IPNET),true)
IPLIBOBJECTS += ipnet_ipv6_icmp_mib.o
IPLIBOBJECTS += ipnet_ipv6_mib.o
IPLIBOBJECTS += ipnet_ipv6_udp_mib.o
endif
IPLIBOBJECTS += ipnet_sock_ip6.o
#IPLIBOBJECTS += ipnet_mip6_kern.o
IPLIBOBJECTS += ipnet_rtnetlink_ip6.o

#############################################################################
# Markets Specific Platform
###########################################################################

ifneq ($(FEATURE_SET),)
ifneq ($(FEATURE_SET),gpp)
-include $(IPNET_ROOT)/gmake/ipnet26_msp_krn.mk
endif
endif

###########################################################################
# END OF IPNET26.MK
###########################################################################
