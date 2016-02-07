/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_mpls.c,v $ $Revision: 1.63.28.1 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_mpls.c,v $
 *   $Author: jhorteli $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Jonas Rendel <Jonas Rendel@interpeak.se>
 ******************************************************************************
 */


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

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#include "ipnet_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef IPMPLS

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_hash.h>
#include <ipcom_type.h>

#include <ipcom_netif.h>

#include <ipmpls.h>
#include <ipmpls_ipstack.h>

#include "ipnet_h.h"
#include "ipnet_pkt_queue.h"

#include "ipnet_netif.h"
#include "ipnet_eth.h"

#include "ipnet_mpls.h"

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Number of bits in the host part (EUI-64) when using the
 * MPLS to create a IPv6 address (RFC 2464?)
 */
#define IPMPLS_IP6_ID_BITS     64


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */
typedef struct Ipnet_mpls_route_struct
{
    Ipcom_route_entry          hdr;   /* The header required by the radix package */
    Ipcom_route               *head;  /* Head of the route table */
    Ip_s32                     refcnt;/* Reference counter */
} Ipnet_mpls_route;


/*
 *===========================================================================
 *                    Ipnet_mpls_probecb
 *===========================================================================
 * Description: Link layer probe callback
 * Parameters:  netif - The interface that the pending packets should be sent to.
 *              pkt - One of the pending packets.
 *              dst - The link layer address of the destination
 * Returns:
 *
 */
typedef void (*Ipnet_mpls_probecb) (struct Ipnet_netif_struct *netif,
                                    struct Ipcom_pkt_struct *pkt,
                                    struct Ip_sockaddr *dst);

/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_STATIC int ipnet_mpls_link_ioctl(struct Ipnet_netif_struct *netif, Ip_u32 command, void *data);
IP_STATIC int ipnet_mpls_ioevent(Ipcom_netif *netif, int command, void *data, int fcflags);
IP_STATIC int ipnet_mpls_tunnel_output(struct Ipnet_netif_struct *netif, struct Ipnet_route_entry_struct *rt, Ipcom_pkt *pkt);
IP_STATIC int ipnet_mpls_pw_output(struct Ipnet_netif_struct *netif, struct Ipnet_route_entry_struct *rt, Ipcom_pkt *pkt);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_STATIC Ipcom_hash *ipnet_mpls_fibif = IP_NULL;

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                     ipnet_mpls_for_each_rtab_cb
 *===========================================================================
 * Description: Callback that is called for each route table in the system.
 * Parameters:  rtab - One of the route tables in the system.
 *              data - User supplied data
 * Returns:
 *
 */
IP_STATIC void
ipnet_mpls_for_each_rtab_cb(Ipcom_route *rtab, void *data)
{
    Ipnet_route_entry *rt;

    (void)data;

    rt = ipnet_route_first_entry(IPNET_ROUTE_GET_FAMILY(rtab),
                                 IPNET_ROUTE_GET_VR(rtab),
                                 IPNET_ROUTE_GET_TABLE(rtab));
    while (rt != IP_NULL)
    {
        ipnet_mpls_shortcut_route_clear(rt);
        rt = ipnet_route_next_entry(rt);
    }
}


/*
 *===========================================================================
 *                    ipnet_mpls_ioctl
 *===========================================================================
 * Description: MPLS IO control.
 * Parameters:  netif - The interface on which the multicast address will
 *              be used.
 *              command - ioctl command.
 *              data - Depends on command.
 * Returns:     0 = success, < 0 = error code
 *
 */
IP_STATIC int
ipnet_mpls_link_ioctl(Ipnet_netif *netif, Ip_u32 command, void *data)
{
    struct Ip_ifreq       *ifreq = data;
    struct Ip_sockaddr_in *in_addr;
    Ipnet_ppp_peer        *p = netif->private_data;

    switch (command)
    {
    case IPMPLS_SIOCMPLSTUNNELDEL:
        ipnet_if_clean_ppp_peer(netif->private_data);
        ipcom_free(netif->private_data);
        netif->private_data = IP_NULL;
        /* Use break here since this ioctl must be propagated to the MPLS module */
        break;
#ifdef IPCOM_USE_INET
    case IP_SIOCGIFDSTADDR:
        in_addr = (struct Ip_sockaddr_in *) &ifreq->ip_ifr_dstaddr;
        ipcom_memset(in_addr, 0, sizeof(struct Ip_sockaddr_in));
        in_addr->sin_family      = IP_AF_INET;
        IPCOM_SA_LEN_SET(in_addr, sizeof(struct Ip_sockaddr_in));
        in_addr->sin_addr.s_addr = p->peer4.s_addr;
        return 0;
    case IP_SIOCSIFDSTADDR:
        in_addr = (struct Ip_sockaddr_in *) &ifreq->ip_ifr_dstaddr;
        if (in_addr->sin_family != IP_AF_INET
            || IPCOM_SA_LEN_GET(in_addr) < sizeof(struct Ip_sockaddr_in))
            return -IP_ERRNO_EINVAL;
        return ipnet_if_set_ipv4_ppp_peer(p, &in_addr->sin_addr);
#endif /* IPCOM_USE_INET */
    default:
        break;
    }
    return ipmpls_tunnel_ioctl(&netif->ipcom, command, data);
}


/*
 *===========================================================================
 *                    ipnet_mpls_tunnel_output
 *===========================================================================
 * Description:  Outputs a packet to a MPLS tunnel interface.
 * Parameters:
 * Returns:      0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_mpls_tunnel_output(struct Ipnet_netif_struct *netif, struct Ipnet_route_entry_struct *rt, Ipcom_pkt *pkt)
{
    IPCOM_UNUSED_ARG(rt);

    netif->obytes += pkt->end - pkt->start;
    netif->opackets++;

    return ipmpls_tunnel_output(&netif->ipcom, pkt);
}


/*
 *===========================================================================
 *                    ipnet_mpls_pw_output
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_mpls_pw_output(struct Ipnet_netif_struct *netif, struct Ipnet_route_entry_struct *rt, Ipcom_pkt *pkt)
{
    (void)netif;

    if (rt->gateway)
        return ipmpls_pw_output((struct Ip_sockaddr_mpls*)rt->gateway, pkt);
    else
        return -IP_ERRNO_EHOSTUNREACH;
}



/*
 *===========================================================================
 *                         ipnet_mpls_ioevent
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_mpls_ioevent(Ipcom_netif *netif, int command, void *data, int fcflags)
{
    (void)netif;
    (void)command;
    (void)data;
    (void)fcflags;

    return 0;
}

/*
 *===========================================================================
 *                         ipnet_mpls_get_proto
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_mpls_get_proto(Ipcom_pkt *pkt)
{
    switch(pkt->data[pkt->ipstart] & 0xf0)
    {
#ifdef IPCOM_USE_INET
    case 0x40:
        return IPNET_ETHFRAME_IP;
#endif
#ifdef IPCOM_USE_INET6
    case 0x60:
        return IPNET_ETHFRAME_IPV6;
#endif
    default:
        return 0;
    }
}


/*
 *===========================================================================
 *                    ipnet_mpls_ifs_obj_hash
 *===========================================================================
 * Description: Creates a hash value from a network interface.
 * Parameters:  netif - A MPLS tunnel interface
 * Returns:     hash value
 *
 */
IP_STATIC unsigned
ipnet_mpls_ifs_obj_hash(Ipnet_netif *netif)
{
    return (unsigned) netif->vr_index;
}


/*
 *===========================================================================
 *                    ipnet_mpls_ifs_key_hash
 *===========================================================================
 * Description: Creates a hash value from a virtual router index.
 * Parameters:  vr_index - A pointer to a virtual router index.
 * Returns:     hash value
 *
 */
IP_STATIC unsigned
ipnet_mpls_ifs_key_hash(Ip_u16 *vr_index)
{
    return (unsigned) *vr_index;
}


/*
 *===========================================================================
 *                    ipnet_mpls_ifs_cmp
 *===========================================================================
 * Description: Checks if the MPLS tunnel interface is assigned to the
 *              specified virtual router.
 * Parameters:  netif - A MPLS tunnel interface
 *              vr_index - A pointer to a virtual router index.
 * Returns:     IP_TRUE if they matches.
 *
 */
IP_STATIC unsigned
ipnet_mpls_ifs_cmp(Ipnet_netif *netif, Ip_u16 *vr_index)
{
    return netif->vr_index == *vr_index;
}


/*
 *===========================================================================
 *
 *===========================================================================
 * Description:
 *
 * Parameters:
 *
 * Returns:
 *
 */
IP_STATIC void
ipnet_mpls_prepare_eth_hdr(Ipnet_netif *netif, Ipcom_pkt *pkt)
{
    Ipnet_pkt_eth      *eth;
    Ip_u16 type;

    if (pkt->force_frame_type == 0)
        type = ipnet_mpls_get_proto(pkt);
    else
        type = pkt->force_frame_type;

    pkt->start -= netif->ipcom.link_hdr_size;
    eth = (Ipnet_pkt_eth *) &pkt->data[pkt->start];
    IPNET_SET_ETHTYPE(netif, eth, type);

}

/*
 *===========================================================================
 *                    ipnet_mpls_arp_probe
 *===========================================================================
 * Description: Send an forced ARP request.
 * Parameters:  netif - The ethernet interface to send to ARP request from.
 *              pkt - The packet to send if the ARP finish successfully.
 *              sin - The IPv4 destination address for 'pkt'
 *              probecb - A function that will be called when/if the ARP
 *                        request is successful.
 *              cookie - Caller data that will be passed to the 'probecb'
 * Returns:     0 = success, <0 = error code.
 *
 */
#ifdef IPCOM_USE_INET
IP_GLOBAL int
ipnet_mpls_arp_probe(Ipnet_netif *netif,
                     Ipcom_pkt *pkt,
                     struct Ip_sockaddr_in *sin,
                     Ipnet_mpls_probecb probecb)
{
    Ipnet_route_entry  *rt;
    Ipnet_arp_entry    *arp_entry;
    int                 ret;

    if (probecb == IP_NULL)
        return -IP_ERRNO_EINVAL;

    ret = ipnet_route_raw_lookup(IP_AF_INET,
                                 netif->vr_index,
                                 IPCOM_ROUTE_TABLE_DEFAULT,
                                 IPNET_RTL_FLAG_LINK_LOCAL,
                                 &sin->sin_addr,
                                 0,
                                 netif->ipcom.ifindex,
                                 &rt);

    if ((ret != 0) || IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_LLINFO))
        return -IP_ERRNO_ENETUNREACH;

    arp_entry = IPNET_ARP(rt);
    switch (arp_entry->ne.state)
    {
    case IPNET_ND_STALE:
        ipnet_arp_set_state(rt, IPNET_ND_DELAY, IP_NULL);
        /* Fall through */
    case IPNET_ND_REACHABLE:
    case IPNET_ND_DELAY:
    case IPNET_ND_PROBE:
        probecb(netif, pkt, rt->gateway); /*lint !e522 */
        break;

    case IPNET_ND_UNINITIALIZED:
        ipnet_arp_set_state(rt, IPNET_ND_INCOMPLETE, IP_NULL);
        /* fall through */

    case IPNET_ND_INCOMPLETE:
        ipnet_mpls_prepare_eth_hdr(netif, pkt);
        ipnet_neigh_queue_until_resolved(&arp_entry->ne.pending, pkt);
        break;

    case IPNET_ND_PROXY:
        /* Can't output datagram to proxy ARP entry */
        ret = -IP_ERRNO_EHOSTUNREACH;
        goto badout;

    default:
        IP_PANIC();
        ret = -IP_ERRNO_EINVAL;
        goto badout;
    }

    return 0;

 badout:
    ip_assert(ret < 0);
    ipcom_pkt_free(pkt);
    return ret;
}
#endif /* IPNET_USE_INET */


/*
 *===========================================================================
 *                    ipnet_mpls_ip6_neighbor_cache_probe
 *===========================================================================
 * Description: Probes for a neighbor cache entry
 * Parameters:  netif - interface to send packet on
 *              pkt - packet to send
 *              probecb - Callback to be called when probe is completed
 * Returns:
 *
 */
#ifdef IPCOM_USE_INET6
IP_GLOBAL void
ipnet_mpls_ip6_neighbor_cache_probe(Ipnet_netif *netif,
                                    Ipcom_pkt *pkt,
                                    struct Ip_sockaddr_in6 *sin6,
                                    Ipnet_mpls_probecb probecb)
{
    Ipnet_ip6_nc_entry *nc;

    nc = ipnet_ip6_neighbor_cache_find(netif, &sin6->sin6_addr, IP_TRUE);
    if (nc == IP_NULL)
    {
        ipcom_pkt_free(pkt);
        return;
    }

    switch (nc->ne.state)
    {
    case IPNET_ND_STALE:
        ipnet_ip6_neighbor_cache_update(nc, IPNET_ND_DELAY, IP_NULL, IP_FALSE);
        /* Fall through */
    case IPNET_ND_REACHABLE:
    case IPNET_ND_DELAY:
    case IPNET_ND_PROBE:
        if (probecb)
            probecb(netif, pkt, nc->ne.rt->gateway); /*lint !e522 */
        break;

    case IPNET_ND_UNINITIALIZED:
        ipnet_ip6_neighbor_cache_update(nc, IPNET_ND_INCOMPLETE, IP_NULL, IP_FALSE);
        /* fall through */

    case IPNET_ND_INCOMPLETE:
        ipnet_neigh_queue_until_resolved(&nc->ne.pending, pkt);
        break;

    default:
        IP_PANIC();
        break;
    }
}
#endif /* IPNET_USE_INET6 */

/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_mpls_init
 *===========================================================================
 * Description: Initiates the IPNET_MPLS module.
 * Parameters:  -
 * Returns:     0 = success, <0 error code
 *
 */
IP_GLOBAL int
ipnet_mpls_init(void)
{
    ipnet_mpls_fibif = ipcom_hash_new((Ipcom_hash_obj_func) ipnet_mpls_ifs_obj_hash,
                                      (Ipcom_hash_key_func) ipnet_mpls_ifs_key_hash,
                                      (Ipcom_hash_cmp_func) ipnet_mpls_ifs_cmp);

    return ipnet_mpls_fibif != IP_NULL ? 0 : -1;
}


/*
 *===========================================================================
 *                    ipnet_mpls_input
 *===========================================================================
 * Description: Input handler for ethernet frames. pkt->start is the offset
 *              to the ethernet header.
 * Parameters:  pkt - The ethernet frame.
 * Returns:     0 = success, <0 error code
 *
 */
IP_GLOBAL int
ipnet_mpls_input(struct Ipnet_netif_struct *netif, Ipcom_pkt *pkt)
{
    return ipmpls_input(&netif->ipcom, pkt, IP_FALSE);
}


/*
 *===========================================================================
 *                    ipnet_mpls_route_table_notify
 *===========================================================================
 * Description: Called when a route table is created or deleted in IPNET
 * Parameters:  vr_index - Route table index.
 *              is_added    - Tells if table is added (or deleted)
 * Returns:     -
 *
 */
IP_GLOBAL void
ipnet_mpls_route_table_notify(Ip_u16 vr_index, Ip_bool is_added)
{
    Ipnet_netif *fibif;

    fibif = ipcom_hash_get(ipnet_mpls_fibif, &vr_index);

    if (!is_added && fibif != IP_NULL)
        /* Removed table. Delete corresponding MPLS FIB interface */
        (void)ipcom_hash_remove(ipnet_mpls_fibif, fibif);
    else if (is_added && fibif == IP_NULL)
    {
        /* New table. Create a new MPLS FIB interface to represent it */
        fibif = ipcom_calloc(1, sizeof(Ipnet_netif));

        if (fibif)
        {
            fibif->ipcom.metric = 1;
            fibif->ipcom.type   = IP_IFT_MPLS_FIBIF;
            fibif->vr_index     = (Ip_u16)vr_index;

            ipcom_snprintf(fibif->ipcom.name,
                           IP_IFNAMSIZ,
                           "mplsfib%d",
                           vr_index);


            (void)ipnet_mpls_if_init(fibif);

            IP_BIT_SET(fibif->ipcom.flags, IP_IFF_UP | IP_IFF_RUNNING | IP_IFF_NOARP);
            if (ipcom_hash_add(ipnet_mpls_fibif, fibif) != IPCOM_SUCCESS)
                IP_PANIC();
        }
    }
#if IPMPLS_MULTIPLE_ROUTE_TABLES
    ipmpls_route_table_notify(vr_index, is_added);
#endif /* IPMPLS_MULTIPLE_ROUTE_TABLES */
}

/*
 *===========================================================================
 *                    ipnet_mpls_get_src_addr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL IP_CONST void*
ipnet_mpls_get_src_addr(Ip_sa_family_t sa_family, Ipnet_netif *fibif, void* dst)
{
    Ip_u32 i;
    Ipnet_netif *this_netif;
    void* src = IP_NULL;

#ifdef IPNET_ASSERT
    ip_assert(fibif != IP_NULL);
#endif

    switch(sa_family)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        {
            IP_CONST struct Ip_in_addr *first_valid = IP_NULL;
            IP_CONST struct Ip_in_addr *selected    = IP_NULL;
            Ipnet_ip4_addr_entry *addr;
            Ip_u32 mask_n = 0;
            Ip_u32 dst_n;
            int longest_match_bits;

            dst_n = IP_GET_32ON16((struct Ip_in_addr*)dst);

            IPNET_NETIF_FOR_EACH(this_netif, i)
            {
                if (this_netif->ipcom.type == IP_IFT_LOOP
                    || this_netif->vr_index != fibif->vr_index)
                    continue;

                for (addr = this_netif->inet4_addr_list; addr != IP_NULL; addr = addr->next)
                {
                    if (addr->type != IPNET_ADDR_TYPE_UNICAST)
                        continue;

                    if (!first_valid)
                        first_valid =  (struct Ip_in_addr *) &addr->ipaddr_n;

                    mask_n             = 0;
                    longest_match_bits = IP_NETBITS(dst_n);
                    ipnet_route_create_mask(&mask_n, longest_match_bits);

                    while (longest_match_bits < 32 && ipcom_route_key_cmp(32, &addr->ipaddr_n, &dst_n, &mask_n))
                    {
                        ipnet_route_create_mask(&mask_n, ++longest_match_bits);
                        selected = (struct Ip_in_addr *) &addr->ipaddr_n;
                    }
                }

                if (selected)
                {
                    src = (void*)selected;
                    goto finish;
                }
            }

            src = (void*)first_valid;
        }
        break;
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        {
            IP_CONST struct Ip_in6_addr *first_valid = IP_NULL;
            IP_CONST struct Ip_in6_addr *selected    = IP_NULL;
            IP_CONST struct Ip_in6_addr *dst_addr;
            Ipnet_ip6_addr_entry        *addr;
            int                         matched_16bits = -1;

            dst_addr = (IP_CONST struct Ip_in6_addr *)dst;

            IPNET_NETIF_FOR_EACH(this_netif, i)
            {
                if (this_netif->ipcom.type == IP_IFT_LOOP
                    || this_netif->vr_index != fibif->vr_index)
                    continue;

                for (addr = this_netif->inet6_addr_list; addr != IP_NULL; addr = addr->next)
                {
                    if (IP_IN6_IS_ADDR_MULTICAST(&addr->addr)
                        || IP_IN6_IS_ADDR_LINK_LOCAL(&addr->addr)
                        || IP_IN6_IS_ADDR_SITE_LOCAL(&addr->addr)
                        || IP_BIT_ISSET(addr->flags, IPNET_IP6_ADDR_FLAG_DEPRECATED))
                        continue;

                    if (!first_valid)
                        src = &addr->addr;

                    if (IP_BIT_ISFALSE(addr->flags, IPNET_IP6_ADDR_FLAG_TENTATIVE))
                    {
                        unsigned b;

                        for (b = 0; b < (int) (sizeof(struct Ip_in6_addr) / sizeof(Ip_u16)); b++)
                            if (dst_addr->in6.addr16[b] != addr->addr.in6.addr16[b])
                                break;

                        if (matched_16bits < (int) b)
                        {
                            if ((b << 4) >= addr->prefixlen)
                            {
                                /* All of the prefix matched, use this address */
                                src = &addr->addr;
                                goto finish;
                            }

                            /* Use the address with the longest match */
                            matched_16bits = b;
                            selected = &addr->addr;
                        }
                    }
                }
                if (selected)
                {
                    src = (void*)selected;
                    goto finish;
                }
            }

            src = (void*)first_valid;
        }
        break;
#endif /* IPCOM_USE_INET6 */
    }

 finish:

    return src;
}


/*
 *===========================================================================
 *                    ipnet_mpls_shortcut_route_setup
 *===========================================================================
 * Description: Called when a new shortcut route is setup in IPNET.
 * Parameters:  vr     - Virtual router to use.
 *              rt     - Pointer to pseudo-wire route entry.
 *              pnetif - Location where the outgoing interface should be stored.
 * Returns:     0 = success, <0 error code
 *
 */
IP_GLOBAL int
ipnet_mpls_shortcut_route_setup(Ip_u16 vr, Ipnet_route_entry *rt, Ipnet_netif **pnetif)
{
    rt->netif = ipcom_hash_get(ipnet_mpls_fibif, &vr);
    if (pnetif != IP_NULL)
        *pnetif = rt->netif;
    IP_BIT_SET(rt->hdr.flags, IPNET_RTF_LLINFO);

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_mpls_shortcut_route_setup
 *===========================================================================
 * Description: Called when a new shortcut route is removed in IPNET.
 * Parameters:  rt     - Pointer to pseudo-wire route entry.
 *
 * Returns:     -
 *
 */
IP_GLOBAL void
ipnet_mpls_shortcut_route_clear(Ipnet_route_entry *rt)
{
    (void)rt;
}


/*
 *===========================================================================
 *                    ipnet_mpls_if_init
 *===========================================================================
 * Description: MPLS interface setup routine.
 * Parameters:  netif - pointer to interface descriptor.
 * Returns:     0 = success, <0 error code
 *
 */
IP_GLOBAL int
ipnet_mpls_if_init(Ipnet_netif *netif)
{
    /* Setup link functions for MPLS use. */
    netif->link_input         = ipnet_mpls_input;
    netif->link_ioctl         = ipnet_mpls_link_ioctl;
    netif->ipcom.link_ioevent = ipnet_mpls_ioevent;

    switch (netif->ipcom.type)
    {
    case IP_IFT_MPLS_TUNNEL:
#ifdef IPCOM_USE_INET
        netif->link_ip4_output    = ipnet_mpls_tunnel_output;
#endif
#ifdef IPCOM_USE_INET6
        netif->link_ip6_output    = ipnet_mpls_tunnel_output;
#endif
        break;
    case IP_IFT_MPLS_FIBIF:
#ifdef IPCOM_USE_INET
        netif->link_ip4_output    = ipnet_mpls_pw_output;
#endif
#ifdef IPCOM_USE_INET6
        netif->link_ip6_output    = ipnet_mpls_pw_output;
#endif
        break;
    default:
        return -IP_ERRNO_EINVAL;
    }

#ifdef IPCOM_USE_INET6
    netif->inet6_if_id_bits   = IPMPLS_IP6_ID_BITS;
#endif

#ifdef IPNET_FASTPATH
        /* Make sure that the one-behind forwarding cache points to
           valid memory. It does not matter if the actual thing it
           points to is stale or not. */
#ifdef IPCOM_USE_INET
        netif->inet4_last_forward_cache = ipnet->ip4.forward_cache;
#endif
#ifdef IPCOM_USE_INET6
        netif->inet6_last_forward_cache = ipnet->ip6.forward_cache;
#endif
#endif /* IPNET_FASTPATH */

    netif->private_data = ipcom_malloc(sizeof(Ipnet_ppp_peer));
    if (netif->private_data == IP_NULL)
        return -IP_ERRNO_ENOMEM;
    ipnet_if_init_ppp_peer(netif, netif->private_data);

    return ipmpls_if_init(&netif->ipcom);
}


IP_GLOBAL void
ipnet_mpls_probe_reply(Ipnet_netif *netif, Ipcom_pkt *pkt,
                       struct Ip_sockaddr *dst)
{
    /* Get frame type from PDU */
    Ip_u16 type;

    if (pkt->force_frame_type == 0)
        type = ipnet_mpls_get_proto(pkt);
    else
        type = pkt->force_frame_type;

    if (ipnet_eth_add_hdr(netif, pkt, dst, type) != 0)
        return;

    /* The interface is now known, what we should do is set the packet's
     * interface index so that it matches the output interface
     */
    pkt->ifindex = netif->ipcom.ifindex;
    (void) ipnet_if_output(netif, pkt);
}

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_mpls_ioevent
 *===========================================================================
 * Description: IPNET Protocol stack portation layer handler for the IPMPLS
 *              module.
 * Parameters:  netif   - Pointer to IPCOM interface descriptor.
 *              command - IPMPLS EIOX command.
 *              data    - Used for various purposes.
 *              more    - Used for various purposes.
 * Returns:     0 = success, <0 error code
 *
 */
IP_PUBLIC int
ipcom_mpls_ioevent(Ipcom_netif *netif, int command, void *data, void *more, void *cookie)
{
    Ipnet_netif                 *ipnet_netif = (Ipnet_netif *)netif;
    int                          ret = 0;
    Ipcom_pkt                   *pkt = (Ipcom_pkt*)data;
    Ip_bool                     *penultimate = (Ip_bool*)cookie;

    switch(command)
    {
    case IPMPLS_EIOXBLOCKOK:
        if (ipnet_is_stack_task() == IP_FALSE)
            ret = 1;      /* user process -> block is ok. */
        break;

    case IPMPLS_EIOXDRVIOCTL:
        {
            Ipmpls_eio_ioctl   *eio_ioctl = data;

            ip_assert(netif);
            ret = netif->drv_ioctl(netif, eio_ioctl->command, eio_ioctl->data);
            break;
        }

    case IPMPLS_EIOXGET_TTL:
        {
            Ip_u8 ttl;
            switch (ipnet_mpls_get_proto(pkt))
            {
#ifdef IPCOM_USE_INET
            case IPNET_ETHFRAME_IP:
                ttl = ((Ipnet_pkt_ip*)&pkt->data[pkt->ipstart])->ttl;
                break;
#endif
#ifdef IPCOM_USE_INET6
            case IPNET_ETHFRAME_IPV6:
                ttl = ((Ipnet_pkt_ip6*)&pkt->data[pkt->ipstart])->hlim;
                break;
#endif
            default:
                ttl = 255;
                break;
            }

            ret = (int)ttl;
        }
        break;

    case IPMPLS_EIOXGET_USER_VR:
        {
            Ip_u16 *vr = (Ip_u16*)data;

            if (vr == IP_NULL)
                return -IP_ERRNO_EINVAL;
#if IPCOM_VR_MAX > 1
            *vr = ipnet_get_effective_vr();
#else
            *vr = 0;
#endif
        }
        break;

    case IPMPLS_EIOXOUTPUT_IF:
        ip_assert(ipnet_netif);
        ret = ipnet_if_output(ipnet_netif, pkt);
        break;

    case IPMPLS_EIOXRTABTONETIF:
        {
            Ip_u32       *vr_index = (Ip_u32*)data;
            Ipnet_netif **ptr = (Ipnet_netif**)more;

            ip_assert(ptr);
            ip_assert(vr_index);

            *ptr = ipcom_hash_get(ipnet_mpls_fibif, vr_index);

            if (*ptr == IP_NULL)
                ret = 1;
            break;
        }

    case IPMPLS_EIOXINDEXTONETIF:
        {
            Ipnet_netif **ptr = (Ipnet_netif**)more;
            Ip_u32 *ifindex   = (Ip_u32*)data;

            ip_assert(ptr);
            ip_assert(ifindex);

            *ptr = ipnet_if_indextonetif(IPCOM_VR_ANY, *ifindex);

            if (ptr == IP_NULL)
                ret = 1;
            break;
        }
    case IPMPLS_EIOXNAMETONETIF:
        {
            char         *name = (char*)data;
            Ipnet_netif **ptr  = (Ipnet_netif**)more;
            Ip_u16       *vr   = (Ip_u16*)cookie;

            ip_assert(ptr);
            ip_assert(name);
            ip_assert(vr);

            *ptr = ipnet_if_nametonetif(*vr, name);

            if (ptr == IP_NULL)
                ret = 0;
            break;
        }
    case IPMPLS_EIOXFOREACHNETIF:
        {
            Ipmpls_netif_foreach_cb cb = (Ipmpls_netif_foreach_cb)data;
            Ip_u32 i;

            if (cb)
            {
#if IPMPLS_RELEASE >= 60101
                IPNET_NETIF_FOR_EACH(ipnet_netif, i)
                    cb(&ipnet_netif->ipcom, ipnet_netif->vr_index, more);
#else
                IPNET_NETIF_FOR_EACH(ipnet_netif, i)
                    cb(&ipnet_netif->ipcom, more);
#endif
            }
        }
        break;

    case IPMPLS_EIOXIF_LOCK:
        ip_assert(netif);
        IPNET_IF_LOCK(ipnet_netif);
        break;

    case IPMPLS_EIOXIF_UNLOCK:
        ip_assert(netif);
        IPNET_IF_UNLOCK(ipnet_netif);
        break;
    case IPMPLS_EIOXIF_OPEN:
        ip_assert(netif);
        ipnet_kioevent(ipnet_netif, IP_EIOXUP, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        break;
    case IPMPLS_EIOXIF_RUNNING:
        ip_assert(netif);
        ipnet_kioevent(ipnet_netif, IP_EIOXRUNNING, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        break;
    case IPMPLS_EIOXIF_CLOSE:
        ip_assert(netif);
        ipnet_kioevent(ipnet_netif, IP_EIOXDOWN, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        break;
    case IPMPLS_EIOXIF_STOP:
        ip_assert(netif);
        ipnet_kioevent(ipnet_netif, IP_EIOXSTOP, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        break;

#ifdef IPCOM_USE_INET
    case IPMPLS_EIOXINPUT_IN:
        ip_assert(ipnet_netif);

        ipnet_netif->ibytes += pkt->end - pkt->start;
        ipnet_netif->ipackets++;

        if (!more)
            ret = ipnet_ip4_input(ipnet_netif, pkt);
        else
        {
            /* Forward IPv4 packet via IPNET */
            Ipnet_ip4_output_param ip4_param;
            Ipnet_pkt_ip           *ip_pkt;
            struct Ip_in_addr      src;

            pkt->ipstart    = pkt->start;
            ip_pkt          = (Ipnet_pkt_ip *)&pkt->data[pkt->ipstart];

            src.s_addr = IPNET_IP4_GET_IPADDR(ip_pkt->src);

            ip4_param.flags      = IP_BIT_ISSET(ip_pkt->off, IPNET_OFF_DF) ? IPNET_IP4_OPF_DONT_FRAG : 0;
            ip4_param.from       = &src;
            ip4_param.to         = &((struct Ip_sockaddr_in*)more)->sin_addr;
            ip4_param.proto      = ip_pkt->p;
            ip4_param.ttl        = --ip_pkt->ttl ; /* ttl < 0 ??? */
            ip4_param.tos        = ip_pkt->tos;
            ip4_param.netif      = IP_NULL;
            ip4_param.chksum_ptr = IP_NULL;
            ip4_param.cache      = IP_NULL;
            ip4_param.nexthop    = IP_NULL;

            ip_pkt->sum = 0;
            ip_pkt->sum = ipcom_in_checksum(ip_pkt, (ip_pkt->v_hl & 0x0F) << 2);
            IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR | IPCOM_PKT_FLAG_FORWARDED);

            IPCOM_MIB2(ipForwDatagrams++);
            IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCInForwDatagrams, 1);
            IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCInForwDatagrams, 1, ipnet_netif);

            ret = ipnet_ip4_output(&ip4_param, pkt);
        }
        break;

    case IPMPLS_EIOXUP_IN:
        {

        }
        break;

    case IPMPLS_EIOXDOWN_IN:
        ip_assert(ipnet_netif);
        ipnet_if_clean(ipnet_netif, IP_TRUE, IP_FALSE);
        break;

    case IPMPLS_EIOXDSTADDR_IN:
        {
            Ip_u32                 dstaddr = ((struct Ip_in_addr *)data)->s_addr;
            Ip_u32                 i;
            Ipnet_netif           *tmpif;
            struct Ip_ifreq        ifreq;
            struct Ip_sockaddr_in *inaddr = (struct Ip_sockaddr_in *)&ifreq.ip_ifr_addr;

            dstaddr = ip_htonl(dstaddr);
            IPNET_NETIF_FOR_EACH(tmpif, i)
            {
                if (IP_BIT_ISFALSE(tmpif->ipcom.flags, IP_IFF_POINTOPOINT))
                    continue;
                (void)ipnet_if_link_ioctl(tmpif, IP_SIOCGIFDSTADDR, &ifreq);
                if (inaddr->sin_addr.s_addr == dstaddr)
                    return 1;
            }
        }
        return 0;

#endif /* ifdef IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    case IPMPLS_EIOXINPUT_IN6:
        ip_assert(ipnet_netif);

        ipnet_netif->ibytes += pkt->end - pkt->start;
        ipnet_netif->ipackets++;

        if (!more)
        {
            ret = ipnet_ip6_input(ipnet_netif, data);
        }
        else
        {
            Ipnet_ip6_output_param ip6_param;
            Ipnet_pkt_ip6          *ip6_hdr;

            pkt->ipstart = pkt->start;
            ip6_hdr = (Ipnet_pkt_ip6*) &pkt->data[pkt->ipstart];

            ipcom_memset(&ip6_param, 0, sizeof(ip6_param));

            ip6_param.from                = ip6_param.real_from     = &ip6_hdr->src;
            ip6_param.to                  = ip6_param.final_to      = &((struct Ip_sockaddr_in6*)more)->sin6_addr;
            ip6_param.scope_id            = ipnet_netif->ipcom.ifindex;
            ip6_param.has_exthdr          = IP_FALSE;
            ip6_param.send_icmp6_redirect = IP_FALSE;
            ip6_param.hlim                = --ip6_hdr->hlim;
            ip6_param.flowinfo            = IPNET_IP6_GET_FLOW(ip6_hdr);
            ip6_param.nxt_hdr             = ip6_hdr->nxt;
            ip6_param.tclass              = (Ip_u8)IPNET_IP6_GET_CLASS(ip6_hdr);
            ip6_param.nexthop             = IP_NULL;

            IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR | IPCOM_PKT_FLAG_FORWARDED);

            IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCInForwDatagrams, 1);
            IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCInForwDatagrams, 1, ipnet_netif);

            ret = ipnet_ip6_output(&ip6_param, pkt);
        }
        break;

    case IPMPLS_EIOXUP_IN6:
        {
            struct Ipnet_ipv6_key rt6[2];
            struct Ipnet_rt_metrics metrics;
            struct Ipnet_route_add_param param;
            Ipmpls_eio_in6  *eio_in6 = data;

            ip_assert(netif);

            /* Add IPv6 address. */
            ret = ipnet_ip6_add_addr(ipnet_netif, &eio_in6->local_addr, 0,
                                     IPCOM_ADDR_INFINITE, IPCOM_ADDR_INFINITE, 0);
            if (ret < 0)
            {
                IP_PANIC2();
                return ret;
            }

            /* Add cloning route for link local net. */
            ipcom_memset(&param, 0, sizeof(struct Ipnet_route_add_param));
            ipcom_memset(&metrics, 0, sizeof(struct Ipnet_rt_metrics));
            ipcom_memset(rt6, 0, sizeof(struct Ipnet_ipv6_key) * 2);
            rt6[0].scope_id = netif->ifindex;
            rt6[0].addr.in6.addr8[0] = 0xfe;
            rt6[0].addr.in6.addr8[1] = 0x80;
            rt6[1].scope_id = 0xffffffff;
            rt6[1].addr.in6.addr32[0] = 0xffffffff;
            rt6[1].addr.in6.addr32[1] = 0xffffffff;
            param.domain  = IP_AF_INET6;
            param.vr      = ipnet_netif->vr_index;
            param.table   = IPCOM_ROUTE_TABLE_DEFAULT;
            param.netif   = ipnet_netif;
            param.key     = &rt6[0];
            param.netmask = &rt6[1];
            param.flags   = IPNET_RTF_UP | IPNET_RTF_MASK | IPNET_RTF_CLONING | IPNET_RTF_DONE;
            param.metrics = &metrics;
            metrics.rmx_hopcount = netif->metric;
            metrics.rmx_expire   = IPCOM_ADDR_INFINITE;
            ret = ipnet_route_add(&param);
            if (ret < 0)
            {
                IP_PANIC2();
                return ret;
            }

            /* Add link local multicast route. */
            rt6[0].addr.in6.addr8[0] = 0xff;
            rt6[0].addr.in6.addr8[1] = 0x02;
            param.flags = IPNET_RTF_UP  | IPNET_RTF_MASK | IPNET_RTF_DONE;
            ret = ipnet_route_add(&param);
            if (ret < 0)
            {
                IP_PANIC2();
                return ret;
            }

            /* Add IPv6 host route to peer. */
            ipcom_memcpy(&rt6[0].addr, &eio_in6->peer_addr, sizeof(struct Ip_in6_addr));
            param.netmask = IP_NULL;
            param.flags = IPNET_RTF_UP | IPNET_RTF_HOST | IPNET_RTF_DONE;
            ret = ipnet_route_add(&param);
            if (ret < 0)
            {
                IP_PANIC2();
                return ret;
            }
        }
        break;

    case IPMPLS_EIOXDOWN_IN6:
        ip_assert(ipnet_netif);
        ipnet_if_clean(ipnet_netif, IP_FALSE, IP_TRUE);
        break;
#endif /* ifdef IPCOM_USE_INET6 */

    case IPMPLS_EIOXADDETHHDR:
        return ipnet_eth_add_hdr(ipnet_netif, (Ipcom_pkt *) data,
                                 (struct Ip_sockaddr*) more,
                                 IPNET_ETHFRAME_MPLS_UNICAST);


    case IPMPLS_EIOXFLUSHPWROUTES:
        ipcom_hash_for_each(ipnet->rtabs,
                            (Ipcom_hash_foreach_cb_func) ipnet_mpls_for_each_rtab_cb,
                            data);
        break;

#ifdef IPCOM_USE_INET
    case IPMPLS_EIOXARPPROBE:

        if (penultimate == IP_NULL || *penultimate == IP_FALSE)
            pkt->force_frame_type = IPNET_ETHFRAME_MPLS_UNICAST;

        ret = ipnet_mpls_arp_probe((Ipnet_netif *) netif,
                                   pkt,
                                   (struct Ip_sockaddr_in *) more,
                                   ipnet_mpls_probe_reply);

        break;
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    case IPMPLS_EIOXNDPROBE:


        if (penultimate == IP_NULL || *penultimate == IP_FALSE)
            pkt->force_frame_type = IPNET_ETHFRAME_MPLS_UNICAST;

        ipnet_mpls_ip6_neighbor_cache_probe((Ipnet_netif *) netif,
                                            pkt,
                                            (struct Ip_sockaddr_in6 *) more,
                                            ipnet_mpls_probe_reply);
        break;
#endif /* IPCOM_USE_INET6 */

    default:
        ip_assert(ipnet_netif);
        ipnet_kioevent(ipnet_netif, command, data, IP_FLAG_FC_STACKCONTEXT);
        break;
    }

    return ret;
}

#endif /* IPMPLS */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
