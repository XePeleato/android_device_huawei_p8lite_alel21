/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_eth.h,v $ $Revision: 1.30 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_eth.h,v $
 *     $Author: jonas $ $Date: 2009-03-12 11:57:47 $
 *     $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *      Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */
#ifndef IPNET_ETH_H
#define IPNET_ETH_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipnet_config.h"

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_hash.h>
#include "ipnet_h.h"
#include "ipnet_neigh.h"
#include "ipnet_timeout.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */
#ifdef IPNET_USE_VLAN
#define IPNET_VLAN_TAG_SIZE  4u  /* The length of the vlan tag */
#endif /* IPNET_USE_VLAN */


/*
 *===========================================================================
 *                         Frame Values
 *===========================================================================
 * Note: Also used for 3:rd argument to when creating packet sockets.
 */
#define IPNET_ETH_P_ALL            0
#define IPNET_ETH_P_IP             0x0800
#define IPNET_ETH_P_ARP            0x0806
#define IPNET_ETH_P_RARP           0x8035
#define IPNET_ETH_P_IPV6           0x86DD
#define IPNET_ETH_P_PPPOE_DIS      0x8863
#define IPNET_ETH_P_PPPOE_SES      0x8864
#define IPNET_ETH_P_8021Q_VLAN     0x8100
#define IPNET_ETH_P_MPLS_UNICAST   0x8847
#define IPNET_ETH_P_MPLS_MULTICAST 0x8848


/* Ethernet types: */
#define IPNET_ETHFRAME_IP              ip_htons(IPNET_ETH_P_IP)
#define IPNET_ETHFRAME_IPV6            ip_htons(IPNET_ETH_P_IPV6)
#define IPNET_ETHFRAME_ARP             ip_htons(IPNET_ETH_P_ARP)
#define IPNET_ETHFRAME_RARP            ip_htons(IPNET_ETH_P_RARP)
#define IPNET_ETHFRAME_PPPOE_DIS       ip_htons(IPNET_ETH_P_PPPOE_DIS)
#define IPNET_ETHFRAME_PPPOE_SES       ip_htons(IPNET_ETH_P_PPPOE_SES)
#define IPNET_ETHFRAME_8021Q_VLAN      ip_htons(IPNET_ETH_P_8021Q_VLAN)
#define IPNET_ETHFRAME_MPLS_UNICAST    ip_htons(IPNET_ETH_P_MPLS_UNICAST)
#define IPNET_ETHFRAME_MPLS_MULTICAST  ip_htons(IPNET_ETH_P_MPLS_MULTICAST)

/* Ethernet MTU/MRU. */
#define IPNET_ETH_MTU          1500

/* Size of Ethernet address */
#define IPNET_ETH_ADDR_SIZE    6

/* Size of Ethernet link header. */
#define IPNET_ETH_HDR_SIZE     14

/* Size of Ethernet link trailer (not used by IP layers) */
#define IPNET_ETH_TRL_SIZE     6

/* Number of bits in the host part (EUI-64) when using the
   ethernet MAC to create a IPv6 address (RFC 2464) */
#define IPNET_ETH_IP6_ID_BITS  64


#define IPNET_ARP(rt)  ((Ipnet_arp_entry *) IPNET_NEIGH_FROM_RT(rt))


#ifdef IPCOM_32ON16_OK
#define IPNET_SET_ETHADDR(eth1,eth2)   do { *(Ip_u32 *)(eth1) = *(Ip_u32 *)(eth2); \
                                             ((Ip_u16*)(eth1))[2] = ((Ip_u16*)(eth2))[2]; } while((0))
#define IPNET_CLR_ETHADDR(eth1)        do { *(Ip_u32 *)(eth1) = 0; \
                                             ((Ip_u16*)(eth1))[2] = 0; } while((0))
#else
#define IPNET_SET_ETHADDR(eth1,eth2)   do { *(Ip_u16 *)(eth1) = *(Ip_u16 *)(eth2); \
											 ((Ip_u16*)(eth1))[1] = ((Ip_u16*)(eth2))[1]; \
											 ((Ip_u16*)(eth1))[2] = ((Ip_u16*)(eth2))[2]; } while((0))

#define IPNET_CLR_ETHADDR(eth1)        do { ((Ip_u16*)(eth1))[0] = 0; \
                                             ((Ip_u16*)(eth1))[1] = 0; \
                                             ((Ip_u16*)(eth1))[2] = 0; } while((0))
#endif

#define IPNET_SET_ETHADDR_BROADCAST(eth)   \
    do { ((Ip_u16*)eth)[0] = 0xffff; \
         ((Ip_u16*)eth)[1] = 0xffff; \
         ((Ip_u16*)eth)[2] = 0xffff; } while((0))

#define IPNET_SET_ETHADDR_IP6_MULTICAST(eth, ip6_addr)   \
    do { ((Ip_u16*)eth)[0] = 0x3333; \
         ((Ip_u16*)eth)[1] = (ip6_addr)->in6.addr16[6]; \
         ((Ip_u16*)eth)[2] = (ip6_addr)->in6.addr16[7]; } while((0))

#define IPNET_SET_ETHADDR_NONE(eth)   \
    do { ((Ip_u16*)eth)[0] = 0; \
         ((Ip_u16*)eth)[1] = 0; \
         ((Ip_u16*)eth)[2] = 0; } while((0))

#define IPNET_ARE_ETHADDR_EQUAL(addr1, addr2) (   ((Ip_u16*)addr1)[0] == ((Ip_u16*)addr2)[0] \
                                               && ((Ip_u16*)addr1)[1] == ((Ip_u16*)addr2)[1] \
                                               && ((Ip_u16*)addr1)[2] == ((Ip_u16*)addr2)[2] )

/* ARP pkt header values: */
#define IPNET_ARP_HRD_ETHER   ip_htons(0x0001)
#define IPNET_ARP_PRO_IP      IPNET_ETHFRAME_IP
#define IPNET_ARP_HLN_PLN     ip_htons(0x0604)

#define IPNET_ARP_OP_REQUEST  ip_htons(0x0001)
#define IPNET_ARP_OP_REPLY    ip_htons(0x0002)
#define IPNET_RARP_OP_REQUEST ip_htons(0x0003)
#define IPNET_RARP_OP_REPLY   ip_htons(0x0004)

/* Per packet information */
#define IPNET_ETH_PKT_MAGIC 0xD5A7

#define IPNET_ETH_PKT_SET_VLAN_TAG(pkt, tag) \
    (pkt)->link_cookie = IPNET_ETH_PKT_MAGIC + ((tag) << 16)

#define IPNET_ETH_PKT_GET_VLAN_TAG(pkt) \
    (Ip_u16) (((pkt)->link_cookie & 0xffff) != IPNET_ETH_PKT_MAGIC ? 0 : ((pkt)->link_cookie >> 16))

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/* Forward declarations */
struct Ipnet_route_entry_struct;
struct Ipnet_netif_struct;


/*
 *===========================================================================
 *                         Ipnet_pkt_eth
 *===========================================================================
 */

#include "ipcom_align16.h"
#ifdef IPNET_USE_VLAN
typedef IP_PACKED1 struct Ipnet_pkt_eth_vlan_struct
{
    Ip_u16   tag_type;                    /* Must be IPNET_ETHFRAME_8021Q_VLAN */
    Ip_u16   pri_cif_vid;                 /* 3 bits priority,
                                             1 bit canonical format indicator,
                                             12 bits VLAN identifier */
    Ip_u16   type;                        /* Ethernet frame type */
}
IP_PACKED2 Ipnet_pkt_eth_vlan;

#define IPNET_ETH_VLAN_GET_VID(pri_cif_vid)      (ip_ntohs(pri_cif_vid) & 0x0fff)
#define IPNET_ETH_VLAN_SET_VID(pri_cif_vid, vid) ((pri_cif_vid) = (pri_cif_vid) & ip_htons(0x0fff) | ip_htons(vid))
#endif /* IPNET_USE_VLAN */


typedef IP_PACKED1 struct Ipnet_pkt_eth_struct
{
    Ip_u8    dst[IPNET_ETH_ADDR_SIZE];    /* Destination address of the frame */
    Ip_u8    src[IPNET_ETH_ADDR_SIZE];    /* Source address of the fram */
    union {
        Ip_u16             type;
#ifdef IPNET_USE_VLAN
        Ipnet_pkt_eth_vlan vlan;
#endif
    } d;
}
IP_PACKED2 Ipnet_pkt_eth;


/*
 *===========================================================================
 *                         Ipnet_pkt_arp
 *===========================================================================
 */
typedef IP_PACKED1 struct Ipnet_pkt_arp_struct
{
    /* ARP header. */
    Ip_u16    hrd;                          /* Format of hardware address. */
    Ip_u16    pro;                          /* Format of protocol address. */
    Ip_u16    hln_pln;                      /* Length of hardware and protocol address. */
    Ip_u16    op;                           /* Code */

    Ip_u8     eth_src[IPNET_ETH_ADDR_SIZE];
    Ip_u16    ip_src[2];

    Ip_u8     eth_dst[IPNET_ETH_ADDR_SIZE];
    Ip_u16    ip_dst[2];
}
IP_PACKED2 Ipnet_pkt_arp;
#include "ipcom_align16.h"


/*
 *===========================================================================
 *                         Ipnet_netif_eth_vlan
 *===========================================================================
 */
typedef struct Ipnet_netif_eth_vlan_struct
{
    Ip_bool                    tag_required; /* tagged egress frames */
    int                        id;     /* Virtual LAN ID 1-4095 */
    int                        pri;     /* 3 bit vlan priority, 0-7 */
    int                        promisc_count;
}
Ipnet_netif_eth_vlan;


/*
 *===========================================================================
 *                         Ipnet_netif_eth
 *===========================================================================
 * Ethernet specific data
 */
typedef struct Ipnet_netif_eth_struct
{
#ifdef IPNET_USE_VLAN
    union
    {
        Ipcom_hash           *ifs;     /* All virtual interfaces that has this interface as parent */
        struct Ipnet_netif_struct *parent;   /* Parent Ethernet interface */
    } vlan;
    Ipnet_netif_eth_vlan     vlan_param;  /* Virtual LAN parameters */
#endif
#ifdef IPPPP_USE_PPPOE
    int            pppoe_num_sessions;  /* Used by ethX */
    void          *pppoe_tmo_handle;    /* Used by ethX  (tmo_handle or pppif) */
    Ip_u32         pppoe_ppp_ifindex;   /* Interface index. */
#endif
#ifdef IPNET_USE_RARP
    Ip_u32         rarp_interval;   /* RARP interval, 2,4,8, etc. */
    Ipnet_timeout *rarp_tmo;
#endif
#ifdef IPNET_USE_VRRP
    Ip_u32 vrids[8];
#endif
#if !(defined(IPNET_USE_RARP) || defined(IPNET_USE_VLAN) \
   || defined(IPPPP_USE_PPPOE) || defined(IPNET_USR_VRRP))
    int   dummy;
#endif
}
Ipnet_netif_eth;


/*
 *===========================================================================
 *                         Ipnet_arp_entry
 *===========================================================================
 */
typedef struct Ipnet_arp_entry_struct
{
    Ipnet_neigh_entry ne;
}
Ipnet_arp_entry;


/*
 ****************************************************************************
 * 6                    PROTOTYPES
 ****************************************************************************
 */
#ifdef IPNET_USE_VLAN
IP_GLOBAL void ipnet_set_eth_type(struct Ipnet_netif_struct *netif,
                                  Ipnet_pkt_eth *eth,  Ip_u16 eth_type,
                                  Ipcom_pkt *pkt);
#define IPNET_SET_ETHTYPE(netif, eth, eth_type) \
    ipnet_set_eth_type(netif, eth, eth_type, pkt)
#else
#define IPNET_SET_ETHTYPE(netif, eth, eth_type) (eth)->d.type = eth_type
#endif


IP_PUBLIC int ipnet_eth_if_init(struct Ipnet_netif_struct *netif);

#ifdef IPNET_USE_VLAN
IP_PUBLIC int ipnet_vlan_if_init(struct Ipnet_netif_struct *netif);
IP_GLOBAL int ipnet_vlan_create(struct Ipnet_socket_struct *sock, struct Ip_ifreq *ifreq);
IP_GLOBAL int ipnet_vlan_compact_create(Ipnet_socket *sock, struct Ip_ifreq *ifreq);
#ifdef IPNET_USE_PER_SOCKET_VLAN_TAG
IP_GLOBAL Ipnet_pkt_eth * ipnet_vlan_add_frame_header(Ipcom_pkt *pkt, Ip_u16 frame_type_n, struct Ipnet_netif_struct *netif);
#endif
#ifdef IPNET_USE_NETLINKSOCK
IP_GLOBAL void ipnet_vlan_rtnetlink_init(void);
#endif
#endif /* IPNET_USE_VLAN */

IP_GLOBAL int ipnet_eth_add_hdr(struct Ipnet_netif_struct *netif, Ipcom_pkt *pkt,
                                struct Ip_sockaddr *dst, Ip_u16 type);

#ifdef IPCOM_USE_INET
IP_GLOBAL void ipnet_arp_set_state(struct Ipnet_route_entry_struct *rt,
                                   Ipnet_nd_state_t state,
                                   IP_CONST void *link_addr);
#endif /* IPCOM_USE_INET */
#ifdef __cplusplus
}
#endif

#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
