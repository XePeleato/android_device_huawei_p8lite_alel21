/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_netif.h,v $ $Revision: 1.64.22.1 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_netif.h,v $
 *     $Author: kenneth $ $Date: 2010-09-10 15:58:42 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *      Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPNET_NETIF_H
#define IPNET_NETIF_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/* Types and function for network interface support in IPNET */

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

#include <ipcom_netif.h>
#include <ipcom_os.h>
#include <ipcom_wait_queue.h>

#include "ipnet.h"
#ifdef IPCOM_USE_INET
#include "ipnet_ip4.h"
#endif
#ifdef IPCOM_USE_INET6
#include "ipnet_ip6.h"
#endif
#include "ipnet_pkt_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_IF_UNSPECIFIED_INDEX     0
#define IPNET_IF_UNSPECIFIED_NAME      IP_NULL
#define IPNET_IF_END                   ((IP_CONST char *) -1)

/* ipnet_pif_notify(netif, event) values: */
#define IPNET_IF_NOTIFY_UP    1       /* Interface was brough up */
#define IPNET_IF_NOTIFY_DOWN  2       /* Interface was taken down */

/* Return the number of interfaces currectly attached */
#define IPNET_NETIF_NUM_ATTACHED (ipnet->netifs->elem)

/* Iterate over all attached interfaces, the netif parameter will be IP_NULL
 * if all elements has been visited
 */
#define IPNET_NETIF_FOR_EACH(netif, i) \
    for (i = 0; i < ipnet->netifs->size || IP_NULL != ((netif) = IP_NULL); i++) \
        if (((netif) = (Ipnet_netif *) ipnet->netifs->table[i]) != IP_NULL)

#define IPNET_NETIF_FOR_EACH_ON_VR(netif, vr, i) \
    for (i = 0; i < ipnet->netifs->size || IP_NULL != ((netif) = IP_NULL); i++) \
        if (((netif) = (Ipnet_netif *) ipnet->netifs->table[i]) != IP_NULL \
            && ((vr) == (netif)->vr_index || (vr) == IPCOM_VR_ANY))

/* Same as above but will only use the one matching "ifindex" if that variable is != 0 */
#define IPNET_NETIF_FOR_EACH_OR_SPECIFIC_ON_VR(netif, vr, ifindex, i) \
    for (i = 0; \
         i < ipnet->netifs->size || IP_NULL != ((netif) = IP_NULL);\
         (ifindex) ? (i = ipnet->netifs->size) : i++) \
        if (((netif) = (ifindex) \
                       ? ipnet_if_indextonetif((vr), (ifindex)) \
                       : (Ipnet_netif *) ipnet->netifs->table[i]) != IP_NULL \
            && ((vr) == (netif)->vr_index || (vr) == IPCOM_VR_ANY))


/* Reusing the next_fragment member since it will always be IP_NULL
   where this macro is used */
#define IPNET_NETIF_SND_Q_NEXT(netif) (netif)->next_fragment

/*
 *===========================================================================
 *                         IPNET_IF_LOCK/UNLOCK
 *===========================================================================
 */
#ifdef IP_PORT_LKM
# define IPCOM_PORT_IF_LOCK(netif)   ipcom_if_lock(&(netif)->ipcom)
# define IPCOM_PORT_IF_UNLOCK(netif) ipcom_if_unlock(&(netif)->ipcom)
#else
# define IPCOM_PORT_IF_LOCK(netif)   IP_NOOP
# define IPCOM_PORT_IF_UNLOCK(netif) IP_NOOP
#endif

#define IPNET_IF_LOCK(netif) \
    do { \
        ip_assert(netif); \
        ipcom_atomic_inc(&(netif)->ipcom.ref_count);  \
        IPCOM_PORT_IF_LOCK(netif); \
    } \
    while ((0))

#define IPNET_IF_UNLOCK(netif) \
    do { \
        ip_assert(netif); \
        IPCOM_PORT_IF_UNLOCK(netif); \
        if (IP_UNLIKELY(ipcom_atomic_sub_and_return(&(netif)->ipcom.ref_count, 1) < 0)) \
            ipnet_if_free(netif); \
    } \
    while ((0))


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/* Forward declaration */
struct Ipnet_netif_struct;
struct Ipnet_netif_eth_struct;
struct Ipnet_netif_wlan_struct;
struct Ipnet_netif_ppp_struct;
struct Ipnet_ip6_nc_entry_struct;
struct Ipnet_ipsec_tdb_struct;
struct Ipnet_ip4_data_struct;
struct Ipnet_route_entry_struct;
struct Ipnet_ip6_dr_entry_struct;

/*
 *===========================================================================
 *                      Ipnet_link_input
 *===========================================================================
 * Description: Process the link header and dispatch IPv4 packets to
 *              ipnet_ip4_input() and IPv6 packets to ipnet_ip6_input().
 *              The pkt->start must be changed to be the offset to the
 *              IPv4 or IPv6 header for IPv4 and IPv6 packets.
 * Parameters:  netif - A pointer to the network interface structure for this
 *              driver.
 *              pkt - The received packet. pkt->start is the offset to the
 *              link header.
 * Returns:     0 = success, <0 = error code (-IPNET_ERRNO_xxx)
 *
 */
typedef int (*Ipnet_link_input)(struct Ipnet_netif_struct *netif, Ipcom_pkt *pkt);

/*
 *===========================================================================
 *                      Ipnet_link_ip4_output
 *===========================================================================
 * Description: Adds the link header to a IPv4 packet and calls the
 *              Ipnet_drv_output function for the interface.
 * Parameters:  netif - A pointer to the network interface structure for this
 *              driver.
 *              rt - The route to the next hop. Link specific data can be
 *              stored at rt->data. The 'data' buffer is
 *              IPNET_ROUTE_ENTRY_DATA_LEN octets long (see <IPNET>/src/ipnet_route.c)
 *              pkt - The packet to send. pkt->start is the offset to the
 *              IP header. pkt->start must be updated to be the offset to
 *              the link layer before calling netif->drv_output().
 * Returns:     0 = success, <0 = error code (-IPNET_ERRNO_xxx)
 *
 */
typedef int (*Ipnet_link_ip4_output)(struct Ipnet_netif_struct *netif,
                                     struct Ipnet_route_entry_struct *rt,
                                     Ipcom_pkt *pkt);

/*
 *===========================================================================
 *                      Ipnet_link_ip6_output
 *===========================================================================
 * Description: Adds the link header to a IPv6 packet and calls the
 *              Ipnet_drv_output function for the interface.
 * Parameters:  netif - A pointer to the network interface structure for this
 *              driver.
 *              rt - The route entry for the first hop (contains IPv6 to link
 *              addr information).
 *              pkt - The packet to send. pkt->start is the offset to the
 *              IP header. pkt->start must be updated to be the offset to
 *              the link layer before calling netif->drv_output().
 * Returns:     0 = success, <0 = error code (-IPNET_ERRNO_xxx)
 *
 */
typedef int (*Ipnet_link_ip6_output)(struct Ipnet_netif_struct *netif,
                                     struct Ipnet_route_entry_struct *rt,
                                     Ipcom_pkt *pkt);

/*
 *===========================================================================
 *                      Ipnet_link_ip6_create_addr
 *===========================================================================
 * Description: Creates a
 * Parameters:  netif - A pointer to the network interface structure for this
 *              driver.
 *              prefix - The prefix to use (the topmost
 *              128 - netif->inet6_if_id_bits is used).
 *              in6_addr - Will contain the auto address if the call is
 *              successful.
 * Returns:     0 = success, <0 = error code (-IPNET_ERRNO_xxx)
 *
 */
typedef int (*Ipnet_link_ip6_create_addr)(struct Ipnet_netif_struct *netif,
                                          IP_CONST struct Ip_in6_addr* prefix,
                                          struct Ip_in6_addr* in6_addr);

/*
 *===========================================================================
 *                      Ipnet_link_ioctl
 *===========================================================================
 * Description: IO control function for the link layer.
 * Parameters:  netif - A pointer to the network interface structure for this
 *              driver.
 *              command - IPNET_IP4_SIOCxxx or IPNET_IP6_SIOCxxx style command.
 *              data - Depends on command.
 * Returns:     0 = success, <0 = error code (-IPNET_ERRNO_xxx)
 *
 */
typedef int (*Ipnet_link_ioctl)(struct Ipnet_netif_struct *netif,
                                Ip_u32 command,
                                void *data);


/*
 *===========================================================================
 *                      Ipnet_link_raw_output
 *===========================================================================
 * Description: Adds a link layer header and outputs the frame to the driver
 *              ARP (or similar) is bypassed.
 * Parameters:  netif - A pointer to the network interface structure for this
 *              driver.
 *              to - The link layer address of the recipient of this frame.
 *              pkt - The packet which should have a link layer header appended.
 * Returns:     0 = success, <0 = error code (-IPNET_ERRNO_xxx)
 *
 */
typedef int (*Ipnet_link_raw_output)(struct Ipnet_netif_struct *netif,
                                     IP_CONST struct Ip_sockaddr_ll *to,
                                     Ipcom_pkt *pkt);

/*
 *===========================================================================
 *                      Ipnet_link_get_frame_info
 *===========================================================================
 * Description: Gets header information from the frame header.
 * Parameters:  netif - A pointer to the network interface structure for this
 *              driver.
 *              frame - A complete link layer frame. pkt->start is the offset
 *              to the link layer header.
 *              pkt_type - Will be set to one of the IP_PACKET_xxx type,
 *              can be IP_NULL if this value is not needed.
 *              frame_type - Will be set to the frame type (i.e. one of the
 *              IPNET_ETH_P_xxx constants for Ethernet). can be IP_NULL if
 *              this information is not needed.
 *              src - Points to a buffer to where the source link address
 *              will be copied. Can be IP_NULL if not needed.
 *              dst - Points to a buffer to where the destination link address
 *              will be copied. Can be IP_NULL if not needed.
 * Returns:     0 = success, <0 = error code (-IPNET_ERRNO_xxx)
 *
 */
typedef int (*Ipnet_link_get_frame_info)(struct Ipnet_netif_struct *netif,
                                         Ipcom_pkt *frame,
                                         Ip_u8 *pkt_type,
                                         Ip_u16 *frame_type,
                                         Ip_u8 *src,
                                         Ip_u8 *dst);


#ifdef IPNET_USE_NETLINKSOCK
struct Ipnet_netlink_mem;
struct Ip_rtnl_link_ops
{
    struct Ip_rtnl_link_ops *next;

    const char              *kind;

    int                     maxtype;
    Ip_u16                  ift_type;
    void                    (*setup)(struct Ipnet_netif_struct *netif);
    int                     (*validate)(struct Ip_nlattr *tb[],
                                        struct Ip_nlattr *data[]);

    int                     (*newlink)(struct Ipnet_netif_struct *netif,
                                       struct Ip_nlattr *tb[],
                                       struct Ip_nlattr *data[]);

    int                     (*changelink)(struct Ipnet_netif_struct *netif,
                                          struct Ip_nlattr *tb[],
                                          struct Ip_nlattr *data[]);

    void                    (*dellink)(struct Ipnet_netif_struct *netif);

    Ip_size_t               (*get_size)(struct Ipnet_netif_struct *netif);

    int                     (*fill_info)(struct Ipnet_netlink_mem   *mem,
                                         struct Ipnet_netif_struct *netif);

    Ip_size_t               (*get_xstats_size)(struct Ipnet_netif_struct *netif);

    int                     (*fill_xstats)(struct Ipnet_netlink_mem   *mem,
                                           struct Ipnet_netif_struct *netif);

    int                     (*fill_link)(struct Ipnet_netlink_mem   *mem,
                                         struct Ipnet_netif_struct *netif);
};
#endif

/*
 *===========================================================================
 *                   Ipnet_in_addr_to_type_cache
 *===========================================================================
 * Cache for an IPv4 address to its address type.
 */
#ifdef IPCOM_USE_INET
struct Ipnet_in_addr_to_type_cache
{
    int                  rt_id; /* The cache is only valid if this
                                   value is equal to
                                   ipnet->route_cache_id */
    struct Ip_in_addr    addr;  /* An IPv4 address */
    enum Ipnet_addr_type type;  /* Type of address */

};
#endif /* IPCOM_USE_INET */

/*
 *===========================================================================
 *                   Ipnet_in6_addr_to_type_cache
 *===========================================================================
 * Cache for an IPv6 address to its address type.
 */
#ifdef IPCOM_USE_INET6
struct Ipnet_in6_addr_to_type_cache
{
    int                  rt_id;       /* The cache is only valid if
                                         this value is equal to
                                         ipnet->route_cache_id */
    struct Ip_in6_addr   addr;        /* An IPv4 address */
    enum Ipnet_addr_type type;        /* Type of address */
    Ipnet_ip6_addr_entry *addr_entry; /* Address entry for 'addr' if
                                         it is one of the local
                                         addresses. IP_NULL
                                         otherwise */
};
#endif /* IPCOM_USE_INET6 */

/*
 *===========================================================================
 *                         Ipnet_netif
 *===========================================================================
 */
typedef struct Ipnet_netif_struct
{
    Ipcom_netif     ipcom;        /* IPCOM Ipcom_netif structure.
                                     See IPCOM/include/ipcom_netif.h */
    Ip_u16        vr_index;       /* The ID of the virtual router this
                                     interface is assigned to */
    Ip_u32        flags2;         /* IPNET_FLAG_IFF2_xxx flags */

    /* Additional interface data, can only be used by link layers
       implemented inside IPNET. External link layer protocols (like
       PPP) must use the "plink" member in Ipcom_netif. */
    void                           *private_data;
    /* Pointer to Ethernet info. IP_NULL if not Eth if */
    struct Ipnet_netif_eth_struct  *eth;
#ifdef IPNET_USE_WLAN
    struct Ipnet_netif_wlan_struct *wlan;  /* Pointer to WLAN. */
#endif

#ifdef IPCOM_USE_INET
    struct Ipnet_ip4_addr_entry_struct *inet4_addr_list;    /* List of IPv4 addresses */
    struct Ipnet_in_addr_to_type_cache  inet4_addr_type_cache;
#ifdef IPNET_FASTPATH
    Ipnet_ip4_rt_cache *inet4_last_forward_cache; /* Last used Ethernet or normal forwarding path entry */
#endif

    /* Function that knows how to initialize neighbor entries for this
       interface. May be IP_NULL if not needed. */
    void (*inet4_neigh_init)(Ipnet_route_entry *rt);

    Ipnet_timeout    *igmpv1_querier_present_tmo; /* Timer for IGMPv1 mode */
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    Ipnet_timeout    *igmpv2_querier_present_tmo; /* Timer for IGMPv2 mode */
#endif
    Ip_u16            igmp_query_interval;        /* The last seen query interval */
    Ip_u8             igmp_robustness_variable;   /* The last seen robustness value */

#ifdef IPNET_USE_RFC1256
    Ipnet_timeout    *inet4_rfc1256_tmo;      /* Timeout structure for RFC1256 RT ADV/SOL timeout */
    Ip_u8             inet4_rfc1256_flags;    /* State for RFC1256                                */
    Ip_u8             inet4_rfc1256_num;      /* Number of advertisements; solicitations left     */
    Ip_u8             inet4_rfc1256_state;    /* Current state                                    */
    Ip_u8             inet4_rfc1256_mode;     /* Mode of operation                                */
#endif

#ifdef IPNET_USE_RFC3927
    Ip_u8             inet4_lladdr_state;    /* IPNET_IP4_LLADDR_STATE_xxx */
    Ip_u8             inet4_lladdr_num;      /* Number of probes or announcement left */
    Ip_u8             inet4_lladdr_conflics; /* Number of address conflicts seen */
    struct Ip_in_addr inet4_lladdr;          /* The current link local address */
    Ipnet_timeout    *inet4_lladdr_tmo;      /* Timeout structure for link-local IPv4 address */
#endif
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    struct Ipnet_ip6_addr_entry_struct *inet6_addr_list; /* All addresses on this interface */
    struct Ipnet_in6_addr_to_type_cache inet6_addr_type_cache;
#ifdef IPNET_FASTPATH
    Ipnet_ip6_rt_cache *inet6_last_forward_cache; /* Last used Ethernet or normal forwarding path entry */
#endif

    Ip_u32         inet6_nd_reachable_time;  /* 0 = use default, else the neighbor reachable time (msec) */
    Ip_u32         inet6_nd_retransmit_time; /* 0 = use default, else the ND retransmit time (msec) */
    Ip_u8          inet6_nd_curhoplimit;     /* default hop limit on packages */
    Ip_u8          inet6_rs_retrans_count;   /* Number of route solicitations left to send */
    Ip_u8          inet6_if_id_bits;   /* Number of bits that should be used for host id
                                          when creating a IPv6 from the link address */
    Ip_u8          mld_robustness_variable;   /* The last seen robustness value */
    Ip_u16         mld_query_interval;        /* The last seen query interval */
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    Ipnet_timeout *mldv1_querier_present_tmo; /* Timer for MLDv1 mode */
#endif
    Ipnet_timeout *inet6_rs_tmo;
#endif /* IPCOM_USE_INET6 */

    /***** IPNET to link layer functions (downcall) *****/
    Ipnet_link_input      link_input;     /* Parse packet in link layer. */
    Ipnet_link_ioctl      link_ioctl;
#ifdef IPCOM_USE_INET
    Ipnet_link_ip4_output link_ip4_output;    /* Called from ipnet_ip4_output() for IPv4 packets */
#endif
#ifdef IPCOM_USE_INET6
    Ipnet_link_ip6_output      link_ip6_output;    /* Called from ipnet_ip6_output() for IPv6 packets */
    Ipnet_link_ip6_create_addr link_ip6_create_addr;
#endif /* IPCOM_USE_INET6 */
    /* IPNET to link layer packet functions. */
#ifdef IPCOM_USE_PACKET
    Ipnet_link_raw_output       link_raw_output;
#endif
    Ipnet_link_get_frame_info   link_get_frame_info;

#ifdef IPNET_USE_NETLINKSOCK
    struct Ip_rtnl_link_ops     *rtnetlink_ops;
    Ip_u32                      rtnetlink_last_iflags;
#endif

    /***** Interface output queue *****/
    Ipcom_pkt            *snd_head;  /* Head of output queue */
    Ipcom_pkt            *snd_tail;  /* Tail of output queue */
    Ipnet_pkt_queue      *snd_queue; /* QoS queue */

#ifndef IPCOM_USE_JOB_QUEUE
    /* Used to detected fast driver restart, i.e. that the driver calls output_done before the
       send call returns */
    Ipcom_atomic_t        snd_id;
#endif

    /* Statistics, do not change the order since IP_SIOCXRESETSTAT make assumptions on order */
    Ip_u32     ipackets;    /* Packets received on interface */
    Ip_u32     ierrors;     /* Input errors on interface */
    Ip_u32     opackets;    /* Packets sent on interface */
    Ip_u32     oerrors;     /* Output errors on interface */
    Ip_u32     collisions;  /* Collisions on csma interfaces */
    Ip_u32     ibytes;      /* Total number of octets received */
    Ip_u32     obytes;      /* Total number of octets sent */
    Ip_u32     imcasts;     /* Packets received via multicast */
    Ip_u32     omcasts;     /* Packets sent via multicast */
    Ip_u32     iqdrops;     /* Dropped on input, this interface */
    Ip_u32     noproto;     /* Destined for unsupported protocol */
}
Ipnet_netif;

/* Packets received on this interface will not be forwarded if set */
#define IPNET_IFF2_NO_IPV4_FORWARD    (1 << 0)
/* Packets received on this interface will not be forwarded if set */
#define IPNET_IFF2_NO_IPV6_FORWARD    (1 << 1)
/* Set if this interface should process incoming router advertisments */
#define IPNET_IFF2_ADV_ACCEPT         (1 << 2)
/* Set if this interface cannot handle IPv4 traffic */
#define IPNET_IFF2_NO_IPV4_SUPPORT    (1 << 3)
/* Included in random seed */
#define IPNET_IFF2_RANDOM_SEED        (1 << 4)
/* Disabled all IPv6 processing since duplicate address detection
   failed for the automatically assigned link local address */
#define IPNET_IFF2_IPV6_DISABLED      (1 << 5)
/* Do not select global addresses */
#define IPNET_IFF2_NO_GLOBAL_SRC      (1 << 6)
/* No DAD has been sent since this link was bought up and running */
#define IPNET_IFF2_FIRST_NS           (1 << 7)

/*
 *===========================================================================
 *                        Ipnet_ppp_peer
 *===========================================================================
 * Keeps track of the peer address on a PPP interface.
 */
typedef struct Ipnet_ppp_peer_struct
{
    Ipnet_netif       *netif;
#ifdef IPCOM_USE_INET
    /* The peer address of an PPP interface */
    struct Ip_in_addr  peer4;
#endif
#ifdef IPCOM_USE_INET6
    /* The peer address of an PPP interface */
    struct Ip_in6_addr peer6;
#endif
}
Ipnet_ppp_peer;


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         internal funcs
 *===========================================================================
 */

IP_GLOBAL Ip_u32 * ipnet_if_get_index_array(Ip_u16 vr, Ip_u32 ifindex, unsigned *array_len);

IP_GLOBAL void ipnet_if_free(Ipnet_netif *netif);
IP_GLOBAL void ipnet_if_clean(Ipnet_netif *netif, Ip_bool clean_ip4, Ip_bool clean_ip6);
IP_GLOBAL int ipnet_if_detach(Ipnet_netif *netif);
IP_GLOBAL Ip_bool ipnet_if_can_detach(Ipcom_netif *netif);

IP_FASTTEXT IP_GLOBAL int ipnet_if_output(Ipnet_netif *netif, Ipcom_pkt *pkt);
IP_FASTTEXT IP_GLOBAL int ipnet_if_drv_output(Ipnet_netif *netif, Ipcom_pkt *pkt);

IP_GLOBAL void ipnet_if_clean_snd_queue(Ipnet_netif *netif);

IP_GLOBAL int ipnet_if_drv_ioctl(Ipnet_netif *netif, Ip_u32 command, void *data);
IP_GLOBAL int ipnet_if_link_ioctl(Ipnet_netif *netif, Ip_u32 command, void *data);
IP_GLOBAL int ipnet_if_link_lock_ioctl_unlock(Ipnet_netif *netif, Ip_u32 command, void *data);
IP_GLOBAL Ip_size_t ipnet_if_to_sockaddr_dl(Ipnet_netif *netif, struct Ip_sockaddr_dl *dl);
IP_GLOBAL Ipnet_netif *ipnet_if_indextonetif(Ip_u16 vr, Ip_u32 ifindex);
IP_GLOBAL Ipnet_netif *ipnet_if_nametonetif(Ip_u16 vr, IP_CONST char *ifname);

IP_GLOBAL int ipnet_netif_change_flags(Ipnet_netif **netif, Ip_u32 flags, Ip_u32 change);

#ifdef IPPPP
IP_PUBLIC int ipnet_ppp_if_init(Ipnet_netif *netif);
#endif

#ifdef IPMPLS
IP_PUBLIC int ipnet_mpls_if_init(Ipnet_netif *netif);
#endif

IP_GLOBAL void ipnet_if_init_ppp_peer(Ipnet_netif *netif, Ipnet_ppp_peer *p);
IP_GLOBAL void ipnet_if_clean_ppp_peer(Ipnet_ppp_peer *p);
#ifdef IPCOM_USE_INET
IP_GLOBAL int ipnet_if_set_ipv4_ppp_peer(Ipnet_ppp_peer *p, IP_CONST struct Ip_in_addr *peer4);
#endif
#ifdef IPCOM_USE_INET6
IP_GLOBAL int ipnet_if_set_ipv6_ppp_peer(Ipnet_ppp_peer *p, IP_CONST struct Ip_in6_addr *peer6);
IP_GLOBAL Ip_bool ipnet_netif_has_valid_inet6_unicast_addr(Ipnet_netif* netif);
#endif

IP_GLOBAL int ipnet_if_init(void);

#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
