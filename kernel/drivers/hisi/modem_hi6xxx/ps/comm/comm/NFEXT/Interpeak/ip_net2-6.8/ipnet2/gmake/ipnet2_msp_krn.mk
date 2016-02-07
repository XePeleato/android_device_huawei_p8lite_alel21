#############################################################################
#			      IPNET2.MK
#
#     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet2_msp_krn.mk,v $ $Revision: 1.7 $
#     $Source: /home/interpeak/CVSRoot/ipnet2/gmake/ipnet2_msp_krn.mk,v $
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

# Markets Specific Platform files
IPLIBOBJECTS += ipnet_ip4_mcast.o
IPLIBOBJECTS += ipnet_ip6_mcast.o
IPLIBOBJECTS += ipnet_pkt_queue_dpaf.o
IPLIBOBJECTS += ipnet_pkt_queue_htbc.o
IPLIBOBJECTS += ipnet_pkt_queue_mbc.o
IPLIBOBJECTS += ipnet_pkt_queue_netemu.o
IPLIBOBJECTS += ipnet_pkt_queue_null.o
IPLIBOBJECTS += ipnet_tunnel.o
IPLIBOBJECTS += ipnet_vlan.o
IPLIBOBJECTS += ipnet_vrrp.o

# NAT
IPLIBOBJECTS += ipnet_nat.o
IPLIBOBJECTS += ipnet_nat_pt.o
IPLIBOBJECTS += ipnet_nat_ftpproxy.o
IPLIBOBJECTS += ipnet_nat_h323proxy.o
IPLIBOBJECTS += ipnet_nat_dnsproxy.o
IPLIBOBJECTS += ipnet_nat_ipsecproxy.o
IPLIBOBJECTS += ipnet_nat_triggerproxy.o
IPLIBOBJECTS += ipnet_nat_pptpproxy.o
IPLIBOBJECTS += ipnet_nat_sipproxy.o


###########################################################################
# END OF IPNET2_MSP_KRN.MK
###########################################################################
