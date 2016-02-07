/* ipcom_netif.h - IPCOM network interface library */

/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_netif.h,v $ $Revision: 1.98.12.1.4.2 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_netif.h,v $
 *   $Author: ulf $ $Date: 2010-07-02 13:20:55 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_NETIF_H
#define IPCOM_NETIF_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
DESCRIPTION
This library contains the API for the IPCOM network interface.
INCLUDE FILES: ipcom_type.h, ipcom_pkt.h, ipcom_sock.h, ipcom_auth.h
*/

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"
#include "ipcom_mib2.h"

/* Maximum size of link address (e.g. Ethernet MAC) in bytes. */
#define IP_LINKNAMSIZ    8


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_auth.h>
#include <ipcom_pkt.h>
#include <ipcom_sock.h>
#include <ipcom_type.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         constants
 *===========================================================================
 */

/* Interface types: */
#define IP_IFT_OTHER       0x1    /* None of the following: */
#define IP_IFT_ETHER       0x6    /* Ethernet CSMACD */
#define IP_IFT_PPP         0x17   /* RFC 1331 */
#define IP_IFT_LOOP        0x18   /* Loopback */
#define IP_IFT_IEEE80211   0x47   /* Radio spread spectrum */
#define IP_IFT_MPLS_TUNNEL 0x83   /* MPLS tunnel interface */
#define IP_IFT_L2VLAN      0x87   /* Layer 2 Virtual LAN using 802.1Q */
#define IP_IFT_PPPOE       0xf0   /* PPPoE */
#define IP_IFT_TUNNEL      0xf1   /* IP over IP tunnel */
#define IP_IFT_SOCKDEV     0xf2   /* Device that has a 1-to-1 mapping to a
                                     socket in the IP_AF_SOCKDEV domain */
#define IP_IFT_SIT         0xf3
#define IP_IFT_IPGRE       0xf4
#define IP_IFT_TUNNEL6     0xf5
#define IP_IFT_LINK_STUB   0xf6   /* Custom Link layer */
#define IP_IFT_PPPL2TP     0xf7   /* L2TPv2 PPP interface */

/* Interface default MTUs: */
#define IP_ETH_MTU         1500
#define IP_PPP_MTU         1500

/* Link header sizes: */
#define IP_ETH_HDR_SIZE    14
#define IP_PPP_HDR_SIZE    (4 + 2)  /* PPP header + 2 bytes FCS */

/*
 *===========================================================================
 *                         Generic interface specific ioctls
 *===========================================================================
 * Generic non-public Interface IO control commands.
 */

/* Link layer and driver layer IO control commands */
#define IP_SIOCXOPEN          IP_IO(NETIF, 100, R, struct Ip_ifreq)     /* Open lower layer. argument not used. */
#define IP_SIOCXCLOSE         IP_IO(NETIF, 101, R, struct Ip_ifreq)     /* Close lower layer. argument not used. */
#define IP_SIOCXPROMISC       IP_IOW(NETIF, 102, W, struct Ip_ifreq) /* Activate/deactivate promiscuous mode 'Ip_bool' */
#define IP_SIOCXDHCPUP        IP_IOWR(NETIF, 103, W, struct Ip_ifreq) /* DHCP enabled on link. IPLITE ONLY!  */
#define IP_SIOCXDHCPDOWN      IP_IOWR(NETIF, 104, W, struct Ip_ifreq) /* DHCP disabled on link.IPLITE ONLY!  */
#define IP_SIOCXCHGFLAGS      IP_IOWR(NETIF, 105, W, struct Ip_ifreq)   /* Notify the driver about network interface flag(s) change */

/* Link-layer join/leave multicast address (note: 'R' flagged cause ok to change while interface UP). */
#define IP_SIOCXADDMULTI_IN   IP_IOW(INET , 100, R, struct Ip_in_addr)  /* Join the corresponding link addr */
#define IP_SIOCXDELMULTI_IN   IP_IOW(INET , 101, R, struct Ip_in_addr)  /* Leave the corresponding link addr */
#define IP_SIOCXADDMULTI_IN6  IP_IOW(INET6, 100, R, struct Ip_in6_addr) /* Join the corresponding link addr */
#define IP_SIOCXDELMULTI_IN6  IP_IOW(INET6, 101, R, struct Ip_in6_addr) /* Leave the corresponding link addr */


/*
 *===========================================================================
 *                         Ethernet specific ioctls
 *===========================================================================
 * IO control commands specific to the Ethernet driver.
 */

/* Add and delete multicast group. argp is a pointer to Ethernet hardware addr. */
typedef struct Ip_hwaddr_struct { Ip_u8 hwaddr[6]; } Ip_hwaddr; /*!!*/
#define IP_SIOCXADDMULTI   IP_IOW(ETH, 1, W, struct Ip_ethreq) /* Join a multicast group. */
#define IP_SIOCXDELMULTI   IP_IOW(ETH, 2, W, struct Ip_ethreq) /* Leave a multicast group. */

/* Get/set IPv4 address/mask/gateway from/to driver. */
#define IP_SIOCXETHGINET   IP_IOWR(ETH, 12, R, struct Ip_ethreq) /* Get IPv4 addr&mask&gateway. */
#define IP_SIOCXETHSINET   IP_IOWR(ETH, 13, W, struct Ip_ethreq) /* Set IPv4 addr&mask&gateway. */

#define IP_SIOCXETHGINET6  IP_IOWR(ETH, 20, R, struct Ip_ethreq) /* Get IPv6 addr&prefixlen. */

/* Enable/disable RARP (IPNET only). */
#define IP_SIOCXETHSRARP   IP_IOWR(ETH, 14, W, struct Ip_ethreq) /* Enable/disable RARP. */

#ifdef IPCOM_VXWORKS_USE_MUX_L2
/* proprietary ioctls: get vlan information from layer 2 */
#define IP_SIOCXETHGVLAN    IP_IOWR(ETH, 50, R, struct Ip_ethreq)

/* proprietary ioctls: join a port to a vlan membership */
#define IP_SIOCXETHSVLAN    IP_IOWR(ETH, 51, W, struct Ip_ethreq)

/* proprietary ioctls: enable a port for layer 2 vlan */
#define IP_SIOCXETHSPORT    IP_IOWR(ETH, 52, W, struct Ip_ethreq)
#endif /* IPCOM_VXWORKS_USE_MUX_L2 */

/*
 *===========================================================================
 *                         io events
 *===========================================================================
 * IO events are passed from driver -> link layer -> IPNET/IPLITE.
 * data = IP_NULL on all current events.
 */

#define IP_EIOXUP        0x1001   /* Interface is up. */
#define IP_EIOXDOWN      0x1002   /* Interface is down. */
#define IP_EIOXRUNNING   0x1003   /* Driver is running. */
#define IP_EIOXSTOP      0x1004   /* Driver is stopped. */
#define IP_EIOXCHANGE    0x1005   /* Interface flags (other than up/running) changed. */
#define IP_EIOXNEWADDR   0x1006   /* Address being added to interface. */
#define IP_EIOXDELADDR   0x1007   /* Address being deleted to interface. */
#ifdef IPCOM_VXWORKS_USE_MUX_L2
#define IP_EIOXVLAN      0x1008   /* vlan egress frame type changes */
#endif /* IPCOM_VXWORKS_USE_MUX_L2 */


/* Driver can calculate and store the transport layer checksum (UDP and TCP)
   on IPv4 packets while transmitting.
   The the stack has calculated the pseudo-header checksum and stored it in
   the transport layer checksum field. */
#define IPCOM_IF_DRV_CAP_IP4_TRANSPORT_CSUM_TX (1 << 0)
/* Driver can calculate and store the transport layer checksum (UDP and TCP)
   on IPv6 packets while transmitting.
   The the stack has calculated the pseudo-header checksum and stored it in
   the transport layer checksum field. */
#define IPCOM_IF_DRV_CAP_IP6_TRANSPORT_CSUM_TX (1 << 1)
/* Driver can create IPv4 TCP segments that is small enough to fit the MTU
   in hardware. TSO = Tcp Segmentation Offload */
#define IPCOM_IF_DRV_CAP_IP4_TSO                (1 << 2)
/* Driver can create IPv6 TCP segments that is small enough to fit the MTU
   in hardware. TSO = Tcp Segmentation Offload */
#define IPCOM_IF_DRV_CAP_IP6_TSO                (1 << 3)
/* Driver can handle slightly larger than normal frames (e.g. frames with
   vlan tag)
 */
#define IPCOM_IF_DRV_CAP_VLAN_MTU               (1 << 4)
/* Driver can insert the vlan tag to a frame on trasnmit */
#define IPCOM_IF_DRV_CAP_VLAN_HWTAGGING_TX      (1 << 5)
/* Driver can strip the vlan tag from the frame on receive */
#define IPCOM_IF_DRV_CAP_VLAN_HWTAGGING_RX      (1 << 6)

/*
 ****************************************************************************
 * 5                    PORT OVERRIDE INCLUDE
 ****************************************************************************
 */

#if defined(IPCOM_USE_NETIF) && IPCOM_USE_NETIF == IPCOM_NETIF_NATIVE
#include "ipcom_pnetif.h"
#endif

#include "ipcom_arphrd.h"

/*
 ****************************************************************************p
 * 6                    TYPES
 ****************************************************************************
 */

/* Forward declaration */
struct Ipcom_netif_struct;


/*
 *===========================================================================
 *                         Ipcom_eth_inet
 *===========================================================================
 * ioctl structure for IP_SIOCXETHGINET option.
 */
typedef struct Ipcom_eth_inet_struct
{
    struct Ip_in_addr   addr;    /* IPv4 address in network endian. */
    struct Ip_in_addr   mask;    /* IPv4 mask in network endian. */
    struct Ip_in_addr   gateway; /* IPv4 gateway in network endian (0 if none). */
}
Ipcom_eth_inet;

#ifdef IPCOM_USE_INET6
/*
 *===========================================================================
 *                         Ipcom_eth_inet6
 *===========================================================================
 * ioctl structure for IP_SIOCXETHGINET6 option.
 */
typedef struct Ipcom_eth_inet6_struct
{
    struct Ip_in6_addr   addr;      /* IPv6 address in network endian. */
    Ip_u8                prefixlen; /* IPv6 prefixlen */
}
Ipcom_eth_inet6;
#endif /* IPCOM_USE_INET6 */

#ifdef IPCOM_VXWORKS_USE_MUX_L2
/*
 *===========================================================================
 *                         Ipcom_eth_vlan
 *===========================================================================
 * ioctl structure for IP_SIOCXETHGVLAN, IP_SIOCXETHSVLAN and
 * IP_SIOCXETHSPORT options used exclusively by vlan pseudo-interface
 * to communicate with layer 2.
 */
typedef struct Ipcom_eth_vlan_struct
{
    Ip_u16    vid;         /* vlan id */
    Ip_bool   tagged;      /* send vlan tagged frame */
    Ip_bool   is_member;   /* port is a member of the vlan */
}
Ipcom_eth_vlan;
#endif /* IPCOM_VXWORKS_USE_MUX_L2 */

/*
 *===========================================================================
 *                         Ip_ethreq
 *===========================================================================
 * Interface request structure used for Ethernet ioctl's.  All interface
 * ioctl's must have parameter definitions which begin with ifr_name and
 * link_index. The remainder may be option specific.
 */
struct Ip_ethreq
{
    char    ethr_name[IP_IFNAMSIZ];    /* if name, e.g. "eth0" */

    union
    {
        int              rarp;   /* 0 = disable else enable. */
        Ipcom_eth_inet   inet;   /* IPv4 address & mask. */
#ifdef IPCOM_USE_INET6
        Ipcom_eth_inet6  inet6;  /* IPv6 address & prefixlen. */
#endif
#ifdef IPCOM_VXWORKS_USE_MUX_L2
        Ipcom_eth_vlan   vlan;
#endif
    }
    ethru;
};
/*
 *===========================================================================
 *                         Ipcom_eth_vlanreq
 *===========================================================================
 * Layer 2 link layer vlan notification structure used for IP_EIOXVLAN io
 * event
 */
#ifdef IP_EIOXVLAN
typedef struct Ipcom_eth_vlanreq
{
    Ip_u16   vid;    /* VLAN identifier */
    Ip_u8    pri;    /* 3-bit VLAN priority, for port-based only */

    Ip_bool  vid_is_pvid;   /* this is a port-based vlan */
    Ip_bool  vlan_tag_required; /* link layer indicates vlan tag is required */
}
Ipcom_eth_vlanreq;
#endif /* IP_EIOXVLAN */


/*******************************************************************************
*
* ipcom_link_ioevent - communicate IO events from the driver to link layer
*
* Communicates IO events from the driver to the link layer. The driver should
* use this function hook to signal events to the link layer. The supported
* events are:
*
* \ts
* IP_EIOXUP      |interface is up   |
* IP_EIOXDOWN    |interface is down |
* IP_EIOXRUNNING |driver is running |
* IP_EIOXSTOP    |driver is stopped |
* IP_EIOXVLAN    | vlan egress frame type changes |
* \te
*
* Note: data is 'IP_NULL' on all current events.
*
* RETURNS: Returns 0 on success, -1 otherwise.
*
* ERRNO:
*/
typedef int (
    *Ipcom_link_ioevent
    )
(
    struct Ipcom_netif_struct *netif,
    int command,
    void *data,
    int fcflags
);



/*******************************************************************************
*
* ipcom_link_tap - call a driver-specific 'rx/tx' tap handler
*
* This routine calls a driver-specific 'rx/tx' tap handler. The tap handler
* makes a copy of the packet that can be used for packet sniffing or other
* purposes.
*
* RETURNS: \&N/A
*
* ERRNO:
*/
typedef void (
    *Ipcom_link_tap
    )
(
    struct Ipcom_netif_struct   *netif,
    Ipcom_pkt                   *pkt,
    int                         pcap_dir,
    Ip_u16                      frame_type,
    int                         mac,
    int                         payload
);


/*
 *===========================================================================
 *                      Ipcom_drv_output
 *===========================================================================
 * Description: Output the packet to the driver.
 * Parameters:  netif - A pointer to the network interface for this driver.
 *              pkt   - The packet to send out on the link. pkt->start is the
 *                      offset to the link layer header.
 * Returns:     0 = success, < 0 = error code (-IP_ERRNO_xxx)
 *
 */
typedef int (*Ipcom_drv_output)(struct Ipcom_netif_struct *netif, Ipcom_pkt *pkt);


/*
 *===========================================================================
 *                      Ipcom_drv_ioctl
 *===========================================================================
 * Description: IO control for the driver.
 * Parameters:  netif - A pointer to the network interface structure for this
 *              driver.
 *              commmand - The IP_SIOCXxxx to perform (see above).
 *              data - Data for to command.
 * Returns:     0 = success, < 0 = error code (-IP_ERRNO_xxx)
 *
 */
typedef int (*Ipcom_drv_ioctl)(struct Ipcom_netif_struct *netif, Ip_u32 command, void *data);



#ifdef IPCOM_USE_MUX_DRV
/*
 *===========================================================================
 *                      Ipcom_drv_ip_resolve
 *===========================================================================
 * Description: To resolve an ipv4 address to link address
 * Parameters:  mux_cookie: pointer to END object
 *              m_blk: IPv4 packet which needs to be sent
 *              ip4_addr: the destination ipv4 address which needs to be
 *              resolved (in a sockaddr_in struct).
 *              pkt_type: 0 = UNICAST,1 = MULTICAST, 2 = BROADCAST
 *              frame_type: network service type ( ipv4 or ipv6)
 *              route_info: pointer to IPNET route entry ( can be used
 *              to cache already resolved addresses to avoid duplicate lookup)
 * Returns:     0 = immediate success with result in dst_m_blkid,
 *              < 0 = Delayed resolution with ownership of m_blk taken
 *              by the driver.
 *
 */
#ifdef IPCOM_USE_INET
typedef int (*Ipcom_drv_ip_resolve)(void *mux_cookie,void *m_blk,
				    struct Ip_sockaddr_in *ip4_addr,void *dst_m_blkid,
				    int pkt_type, int frame_type, void *route_info);
#endif

#ifdef IPCOM_USE_INET6
/*
 *===========================================================================
 *                      Ipcom_drv_ip6_resolve
 *===========================================================================
 * Description: To resolve an ipv4 address to link address
 * Parameters:  mux_cookie: pointer to END object
 *              m_blk: IPv6 packet which needs to be sent
 *              ip6_addr: the destination ipv6 address which needs to be
 *              resolved (in a sockaddr_in6 struct).
 *              pkt_type: 0 = UNICAST,1 = MULTICAST, 2 = BROADCAST
 *              frame_type: network service type ( ipv4 or ipv6)
 *              route_info: pointer to IPNET route entry ( can be used
 *              to cache already resolved addresses to avoid duplicate lookup)
 * Returns:     0 = immediate success with result in dst_m_blkid,
 *              < 0 = Delayed resolution with ownership of m_blk taken
 *              by the driver.
 *
 */
typedef int (*Ipcom_drv_ip6_resolve)(void *mux_cookie,void *m_blk,
				     struct Ip_sockaddr_in6 *ip6_addr,void *dst_m_blkid,
				     int pkt_type, int frame_type, void *route_info);

/*
 *===========================================================================
 *                      Ipcom_drv_ip6_create_addre
 *===========================================================================
 * Description: To create an ipv6 address from the link addr and
 * network prefix.
 * Parameters:  prefix: ipv6 network prefix in a struct Ip_in6_addr
 *              ip6_addr: returns the created ipv6 address in a struct
 *              Ip_in6_addr
 * Returns:     void
 */
typedef void (*Ipcom_drv_ip6_create_addr)(void *prefix, void *ip6_addr);

#endif /* IPCOM_USE_INET6 */
#endif /* IPCOM_USE_MUX_DRV */



/*
 *===========================================================================
 *                         Ipcom_netif
 *===========================================================================
 */
typedef struct Ipcom_netif_struct
{
    char           name[IP_IFNAMSIZ];  /* interface name (e.g. eth0, ppp1) */
    Ip_u32         flags;              /* IP_IFF_xxx flags */
    Ip_u32         ifindex;            /* Interface index. */
    Ip_u32         link_index;         /* Link index for MPPP. Currently 0. */
    Ip_u32         mtu;                /* The interface maximum transfer unit */
    Ip_u32         metric;             /* The interface metric */
    Ip_u16         type;               /* Interface type, one of IP_IFT_XXX */
    Ip_u16         arphrd_type;
    Ipcom_atomic_t ref_count;          /* Reference counter */


    /****** LINK VARIABLES ******/
    void     *plink;              /* Link port data, e.g. PPP specific info. */
    Ip_u16    link_hdr_size;      /* Size of the link header in bytes */
    Ip_u16    link_addr_size;     /* Size of the link address in bytes */
    Ip_u8     link_addr[IP_LINKNAMSIZ];

    /* Link function called by driver layer */
    Ipcom_link_ioevent  link_ioevent;

    /* Link tap function called by link layer */
    Ipcom_link_tap      link_tap;

    /****** DRIVER VARIABLES ******/
    void               *pdrv;      /* Driver port data, IPCOM/IPNET/IPLITE does not use this info. */

    /* Driver functions called by link layer */
    Ipcom_drv_output    drv_output;
    Ipcom_drv_ioctl     drv_ioctl;
    Ip_u32              drv_capabilities;  /* IPCOM_IF_DRV_CAP_xxx */

#ifdef IPCOM_USE_MUX_DRV
#ifdef IPCOM_USE_INET
    Ipcom_drv_ip_resolve            drv_ip_resolve;
#endif
#ifdef IPCOM_USE_INET6
    Ipcom_drv_ip6_resolve           drv_ip6_resolve;
    Ipcom_drv_ip6_create_addr       drv_ip6_create_addr;
#endif
#endif /* IPCOM_USE_MUX_DRV */

#ifdef IPCOM_USE_MIB2
    Ipcom_mib2_stats_netif mib2;
#endif

#ifdef IPWLAN
    int (*wlan_ioctl)(void *, Ip_u32, void *);
    Ip_u8 macaddr[6];
#endif

#ifdef IPCOM_USE_FORWARDER
    int  hw_ix; /* forwarder port index to use for outgoing traffic */
#endif
}
Ipcom_netif;

#define IPCOM_ARPHRD_TYPE(netif)    ((netif)->arphrd_type? (netif)->arphrd_type : (netif)->type)

/*
 ****************************************************************************
 * 7                    FUNCTIONS
 ****************************************************************************
 */

/*******************************************************************************
*
* ipcom_if_malloc - allocate a new 'ipcom_netif' structure
*
* This routine allocates a new 'ipcom_netif' structure with enough space to hold
* the link-layer address and data. All fields are initialized, except
* 'drv_ioctl', 'drv_output', 'pdrv' and 'link_addr', which must be initialized
* before calling ipcom_if_attach().
*
* If an ipnet_config.c ipnet_conf_link_layer[x].init_func(netif) initialization
* routine has been registered for this interface type, it is now called to
* initialize the interface.
*
* The ipcom_if_malloc() routine sets a default interface name. The name can be
* changed before attaching the interface.
*
* Parameter:
* \is
* \i <type>
* Interface type, see 'ipcom_netif.h' for 'IP_IFT_'<xxx> types. Usually, either
* 'IP_IFT_ETHER' or 'IP_IFT_PPP'.
* \ie
*
* RETURNS: Pointer to a new 'ipcom_netif' structure on success, or 'IP_NULL' if
* not enough memory could be allocated.
*
* ERRNO:
*/
IP_PUBLIC Ipcom_netif *ipcom_if_malloc
(
    Ip_u16 type
);


/*******************************************************************************
*
* ipcom_if_free - free all resources allocated by ipcom_if_malloc()
*
* This routine frees all resources allocated by ipcom_if_malloc() for the
* specified network interface.
*
* Parameter:
* \is
* \i <netif>
* The interface to free.
* \ie
*
* RETURNS: 0 on success, otherwise a negative ERRNO.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_if_free
(
    Ipcom_netif *netif
);


/*******************************************************************************
*
* ipcom_if_attach - add a specified network interface
*
* This routine adds a specified network interface to the list of available
* interfaces.
*
* The TCP/IP stack configures the route entries and addresses for the added
* interface using the configuration information in the TCP/IP stack 'config'
* directory, if this function is called at boot. If the function is called at a
* later time, interface addresses and routes must be added using socket system
* calls, such as ioctl() calls, for example.
*
* Parameter:
* \is
* \i <netif>
* The network interface to add.
* \ie
*
* RETURNS: The interface index number on success, or 'IP_ERRNO_EINVAL' if some
* of the fields in the 'ipcom_netif' structure are invalid.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_if_attach
(
    Ipcom_netif *netif
);


/*******************************************************************************
*
* ipcom_if_attach_and_lock - add a specified network interface
*
* This routine adds a specified network interface to the list of available
* interfaces. The interface is locked by ipnet when attached. The caller must
* unlock it (i.e decrease reference count) when finished with it
*
* The TCP/IP stack configures the route entries and addresses for the added
* interface using the configuration information in the TCP/IP stack 'config'
* directory, if this function is called at boot. If the function is called at a
* later time, interface addresses and routes must be added using socket system
* calls, such as ioctl() calls, for example.
*
* Parameter:
* \is
* \i <netif>
* The network interface to add.
* \ie
*
* RETURNS: The interface index number on success, or 'IP_ERRNO_EINVAL' if some
* of the fields in the 'ipcom_netif' structure are invalid.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_if_attach_and_lock
(
    Ipcom_netif *netif
);


/*******************************************************************************
*
* ipcom_if_detach - detach a previously attached network interface
*
* This routine detaches a previously attached network interface. The interface
* must be completely stopped (neither 'IFF_UP' or 'IFF_RUNNING' can be set)
* before being detached. An interface is brought down using
* 'ioctl(\'SIOCSIFLAGS\')' with 'IFF_UP' not set.
*
* The routine cleans the interface output queue and removes all IPv4 and IPv6
* addresses and routes on the interface. Processes waiting on sockets with any
* of the removed addresses are woken up, and the system call returns an error
* code.
*
* Once an interface has been detached, the interface memory must be released
* using the 'ipcom_if_free(netif)' routine.
*
* Parameter:
* \is
* \i <netif>
* The network interface to detach.
* \ie
*
* RETURNS: The interface index number on success, otherwise one of the
* following negative ERRNO codes:
* \is
* \i -'IP_ERRNO_EBUSY'
* The specified interface is still 'UP' or 'RUNNING'.
* \i -'IP_ERRNO_EINVAL'
* The specified interface <netif> pointer is invalid.
* \ie
*
* ERRNO:
*/
IP_PUBLIC int ipcom_if_detach
(
    Ipcom_netif *netif
);


/*******************************************************************************
*
* ipcom_if_changename - change the name of a network interface
*
* This routine changes the name of an attached interface.
*
*
* Parameters:
* \is
* \i <ipcomif>
* The network interface whose name is to change.
* \i <new_name>
* The new name for the interface.
* \ie
*
* RETURNS: \&N/A
*
* ERRNO:
*/
IP_PUBLIC void ipcom_if_changename
(
    Ipcom_netif *ipcomif,
    const char *new_name
);


/*******************************************************************************
*
* ipcom_if_changemtu - change the mtu of a network interface
*
* This routine changes the mtu of an attached interface.
*
*
* Parameters:
* \is
* \i <ipcomif>
* The network interface whose name is to change.
* \i <new_mtu>
* The new mtu for the interface.
* \ie
*
* RETURNS: \&N/A
*
* ERRNO:
*/
IP_PUBLIC void ipcom_if_changemtu
(
    Ipcom_netif *ipcomif,
    Ip_u32 new_mtu
);


/*******************************************************************************
*
* ipcom_if_changelinkaddr - change the link layer addr of a network interface
*
* This routine changes the link layer address of an attached interface.
*
*
* Parameters:
* \is
* \i <ipcomif>
* The network interface whose name is to change.
* \i <lladdr>
* The new link layer address for the interface (byte array).
* \i <size>
* The size of the new link layer address.
* \ie
*
* RETURNS: \&N/A
*
* ERRNO:
*/
IP_PUBLIC void ipcom_if_changelinkaddr
(
    Ipcom_netif *ipcomif,
    Ip_u8  *lladdr,
    Ip_u16 size
);


/*******************************************************************************
*
* ipcom_flow_fingerprint - create a flow fingerprint
*
* This routine calculates a fingerprint of a packet. Two packets that belongs
* to the same flow will have the same fingerprint.
* The 'pkt-\>start must' be the offset into 'pkt-\>data' where the link layer
* starts.
*
* Parameters:
* \is
* \i <ipcomif>
* The network interface that received the packet.
* \i <pkt>
* The packet for which a fingerprint should be calculated.
* \ie
*
* RETURNS: A flow fingerprint for the packet.
*
* ERRNO:
*/
IP_PUBLIC unsigned ipcom_flow_fingerprint(
    Ipcom_netif *ipcomif,
    Ipcom_pkt *pkt);


/*
 *===========================================================================
 *                    ipcom_drv_xxx_create
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err ipcom_drv_eth_create(void);
IP_PUBLIC Ip_err ipcom_drv_ppp_create(void);
IP_PUBLIC Ip_err ipcom_drv_wlan_create(void);

#ifdef IPCOM_USE_LBP
IP_PUBLIC Ip_err ipcom_drv_lbp_create(void);
#endif


/*
 *===========================================================================
 *                    ipcom_drv_ppp_if_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_drv_ppp_if_init(const char *ifname, const char *devname, int unit);


/*
 *===========================================================================
 *                    ipcom_if_nametonetif
 *===========================================================================
 * Description: Returns a pointer to the interface descriptor identified by
 *              its name.
 *              This function ensures that the interface is locked (reference counter
 *              has been increased).
 *              The user MUST self unlock the interface when finished with it.
 *
 * Parameters:  vr     - Actual VR to search for interface within.
 *              ifname - A buffer to hold the name, must be at least
 *              IP_IFNAMSIZ characters long.
 * Returns:     A pointer to the interface descriptor if found
 *              IP_NULL otherwise.
 *
 */
IP_PUBLIC Ipcom_netif *ipcom_if_nametonetif
(
   Ip_u16         vr,
   IP_CONST char *ifname
);


/*
 *===========================================================================
 *                    ipcom_if_xxx LKM
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IP_PORT_LKM
IP_PUBLIC void ipcom_netif_assign_vr(Ipcom_netif* netif, Ip_u16 vr);
IP_GLOBAL void ipcom_if_lock(Ipcom_netif *netif);
IP_GLOBAL void ipcom_if_unlock(Ipcom_netif *netif);
#endif


/*
 ****************************************************************************
 * 8                    VARIABLES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         ipcom_conf_max_link_hdr_size
 *===========================================================================
 * The largest link header size of all network interfaces the system have.
 * Ex: 14 for a system with only Ethernet network interfaces. The value is
 * calculated automatically by IPNET/IPLITE depending on supported interfaces.
 *
 * NOTE: An additional 4 bytes must be allocated due to driver 32-bit cookie!
 */
IP_EXTERN int ipcom_conf_max_link_hdr_size;


#ifdef __cplusplus
}
#endif

#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
