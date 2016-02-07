#############################################################################
#			      IPNET2_KRN.MK
#
#     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet2_krn.mk,v $ $Revision: 1.33 $
#     $Source: /home/interpeak/CVSRoot/ipnet2/gmake/ipnet2_krn.mk,v $
#     $Author: n $ $Date: 2009-09-09 13:02:04 $
#     $State: Exp $ $Locker:  $
#
#     INTERPEAK_COPYRIGHT_STRING
#
#############################################################################


#############################################################################
# PRODUCT
###########################################################################

IPPROD ?= ipnet_krn


#############################################################################
# DEFINE
###########################################################################

#############################################################################
# INCLUDE
###########################################################################

IPINCLUDE += -I$(BASE_ROOT)/$(IPWLANDIR)/include

#############################################################################
# OBJECTS
###########################################################################

# Configuration
IPLIBOBJECTS_C += ipnet_config.o

# Main
IPLIBOBJECTS += ipnet.o
IPLIBOBJECTS += ipnet_classifier.o
IPLIBOBJECTS += ipnet_ctrl.o
IPLIBOBJECTS += ipnet_debug.o
IPLIBOBJECTS += ipnet_diffserv.o
IPLIBOBJECTS += ipnet_ds_sm.o
IPLIBOBJECTS += ipnet_ds_srtcm.o
IPLIBOBJECTS += ipnet_eth.o
IPLIBOBJECTS += ipnet_frag.o
IPLIBOBJECTS += ipnet_gre.o
IPLIBOBJECTS += ipnet_ioctl.o
IPLIBOBJECTS += ipnet_ip4.o
IPLIBOBJECTS += ipnet_loopback.o
IPLIBOBJECTS += ipnet_mcast.o
IPLIBOBJECTS += ipnet_mib2.o
IPLIBOBJECTS += ipnet_neigh.o
IPLIBOBJECTS += ipnet_netif.o
IPLIBOBJECTS += ipnet_netlink.o
IPLIBOBJECTS += ipnet_pipe.o
IPLIBOBJECTS += ipnet_pkt_queue.o
IPLIBOBJECTS += ipnet_pkt_queue_fifo.o
IPLIBOBJECTS += ipnet_raw.o
IPLIBOBJECTS += ipnet_route.o
IPLIBOBJECTS += ipnet_routesock.o
IPLIBOBJECTS += ipnet_rtnetlink.o
IPLIBOBJECTS += ipnet_rtnetlink_ip4.o
IPLIBOBJECTS += ipnet_rtnetlink_link.o
IPLIBOBJECTS += ipnet_rtnetlink_neigh.o
IPLIBOBJECTS += ipnet_rtnetlink_route.o
IPLIBOBJECTS += ipnet_rtnetlink_rule.o
IPLIBOBJECTS += ipnet_rtnetlink_vr.o
IPLIBOBJECTS += ipnet_sock.o
IPLIBOBJECTS += ipnet_sock_ip4.o
IPLIBOBJECTS += ipnet_sock_sctp.o
IPLIBOBJECTS += ipnet_sock_tcp.o
IPLIBOBJECTS += ipnet_sockdev.o
IPLIBOBJECTS += ipnet_sockopt.o
IPLIBOBJECTS += ipnet_sysctl.o
IPLIBOBJECTS += ipnet_udp.o
IPLIBOBJECTS += ipnet_usr_sock.o
IPLIBOBJECTS += ipnet_wlan.o

ifeq ($(IPPORT),vxworks)
IPLIBOBJECTS += ipnet_link_stub.o
endif


ifneq ($(IPPORT),lkm)
IPLIBOBJECTS += ipnet_pcap.o
IPLIBOBJECTS += ipnet_pkt_pool.o
IPLIBOBJECTS += ipnet_configd.o
endif

# PPP
IPLIBOBJECTS += ipnet_ppp.o

# MPLS
IPLIBOBJECTS += ipnet_mpls.o

# IPIPSEC
IPLIBOBJECTS += ipnet_ipsec.o

# SNMP
ifneq ($(WIND_IPNET),true)
IPLIBOBJECTS += ipnet_if_mib.o
IPLIBOBJECTS += ipnet_ip_forward_mib.o
IPLIBOBJECTS += ipnet_ip_mib.o
IPLIBOBJECTS += ipnet_udp_mib.o
endif

# Compiles the xxx_config.o if the $SKIP_CONFIG macro is either not defined
# or set to anything other than true.
ifneq ($(SKIP_CONFIG),true)
IPLIBOBJECTS    += $(IPLIBOBJECTS_C)
endif


#############################################################################
# VPATH
###########################################################################

IPSRCDIRS += $(IPNET_ROOT)/src
IPSRCDIRS +=  $(IPNET_ROOT)/config


#############################################################################
# IPv6
###########################################################################

ifneq ($(IPCOM_USE_INET6),no)
-include $(IPNET_ROOT)/gmake/ipnet26_krn.mk
endif


#############################################################################
# Markets Specific Platform
###########################################################################

ifneq ($(FEATURE_SET),)
ifneq ($(FEATURE_SET),gpp)
-include $(IPNET_ROOT)/gmake/ipnet2_msp_krn.mk
endif
endif

###########################################################################
# END OF IPNET2_KRN.MK
###########################################################################
