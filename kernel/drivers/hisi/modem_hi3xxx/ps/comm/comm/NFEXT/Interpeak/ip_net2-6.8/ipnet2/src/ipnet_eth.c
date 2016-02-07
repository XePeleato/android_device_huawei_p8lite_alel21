/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_eth.c,v $ $Revision: 1.317.18.2 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_eth.c,v $
 *     $Author: ulf $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Lennart Bang <lob@interpeak.se>
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

#ifdef IPCOM_USE_ETHERNET

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_clib.h>
#include <ipcom_syslog.h>

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_eth.h"
#include "ipnet_pkt_queue.h"
#include "../../InterpeakMntn/include/InterpeakMntn.h"


#ifdef IPMPLS
#include "ipnet_mpls.h"
#endif

#ifdef IPNET_USE_VRRP
#include "ipnet_vrrp.h"
#endif

#ifdef IPCOM_WV_INSTRUMENTATION
#include <ipcom_windview.h>
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Minimum length, in octets, of a valid Ethernet frame */
#define IPNET_ETH_MIN_FRAME_LEN  60

/* Returns if the specified MAC address is multicast or not */
#define IPNET_ETH_IS_MCAST_MAC(mac) (0x01 & mac[0])

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

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

#ifdef IPNET_USE_RARP
IP_STATIC void ipnet_eth_rarp_request(Ipnet_netif *netif);
#endif
IP_FASTTEXT IP_STATIC int ipnet_eth_input(Ipnet_netif *netif, Ipcom_pkt *pkt);
IP_STATIC int ipnet_eth_ioevent(Ipcom_netif *ipcomif, int command, void *data, int fcflags);

#ifdef IPCOM_USE_INET
IP_FASTTEXT IP_STATIC int ipnet_eth_ip4_output(Ipnet_netif *netif,
                                               Ipnet_route_entry *rt,
                                               Ipcom_pkt *pkt);
IP_STATIC int ipnet_arp_try_resolve(Ip_u16 vr,
                                    struct Ip_in_addr *target,
                                    Ipnet_netif *netif);
IP_STATIC void ipnet_arp_try_resolve_on_all_links(Ipnet_route_entry *rt,
                                                  struct Ip_in_addr *target);
#endif

#ifdef IPCOM_USE_INET6
IP_STATIC int ipnet_eth_ip6_output(Ipnet_netif *netif,
    Ipnet_route_entry *re, Ipcom_pkt *pkt);

IP_STATIC int ipnet_eth_ip6_create_addr(Ipnet_netif *netif,
    IP_CONST struct Ip_in6_addr* prefix, struct Ip_in6_addr* in6_addr);
#endif /* IPCOM_USE_INET6 */


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                      ipnet_eth_is_valid_node_mac
 *===========================================================================
 * Description: Retruns if the specified mac address can be considered a
 *              valid MAC address of a node.
 * Parameters:  netif - Interface to consider
 *              mac - a MAC address
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_eth_is_valid_node_mac(Ipnet_netif *netif, Ip_u8 *mac)
{
    if (!IPNET_ETH_IS_MCAST_MAC(mac))
        return IP_TRUE;

    return ipnet_sysvar_netif_get_as_int(IP_AF_INET,
                                         netif,
                                         "AllowMcastSrcMac",
                                         IP_FALSE);
}


/*
 *===========================================================================
 *                    ipnet_eth_rarp_request
 *===========================================================================
 * Description: Send a RARP request.
 * Parameters:  [in,out] netif - The netif entry where RARP should be sent.
 * Returns:
 *
 */
#ifdef IPNET_USE_RARP
IP_STATIC void
ipnet_eth_rarp_request(Ipnet_netif *netif)
{
    Ipcom_pkt       *pkt;
    Ipnet_pkt_eth   *eth;
    Ipnet_pkt_arp   *rarp;

    /*IPNET_STATS(eth_output_arp_req++);*/

    pkt = ipcom_pkt_malloc(sizeof(Ipnet_pkt_arp),
                           IP_FLAG_FC_STACKCONTEXT);
    if (pkt == IP_NULL)
        return;

    /* Packet header. */
    pkt->ifindex = netif->ipcom.ifindex;
    pkt->start   = ipcom_conf_max_link_hdr_size - netif->ipcom.link_hdr_size;
    pkt->end     = pkt->start + sizeof(Ipnet_pkt_arp) + netif->ipcom.link_hdr_size;

#if defined(IP_PORT_LKM) || (defined(IP_PORT_VXWORKS) && IP_PORT_VXWORKS >= 67)
    pkt->ipstart = pkt->start + netif->ipcom.link_hdr_size;
#endif

    /* Ethernet header. */
    eth = (Ipnet_pkt_eth *) &pkt->data[pkt->start];
    IPNET_SET_ETHADDR(eth->src, netif->ipcom.link_addr);
    IPNET_SET_ETHADDR_BROADCAST(eth->dst);
    IPNET_SET_ETHTYPE(netif, eth, IPNET_ETHFRAME_RARP);

    /* RARP header */
    rarp = (Ipnet_pkt_arp *) &pkt->data[pkt->start + netif->ipcom.link_hdr_size];
    rarp->hrd       = IPNET_ARP_HRD_ETHER;
    rarp->pro       = IPNET_ARP_PRO_IP;
    rarp->hln_pln   = IPNET_ARP_HLN_PLN;
    rarp->op        = IPNET_RARP_OP_REQUEST;
    /*
      rarp->ip_src[0] = 0;    rarp->ip_src[1] = 0;
      rarp->ip_dst[0] = 0;    rarp->ip_dst[1] = 0;
    */
    IPNET_SET_ETHADDR(rarp->eth_src, netif->ipcom.link_addr);
    IPNET_SET_ETHADDR(rarp->eth_dst, netif->ipcom.link_addr);

    /* Output RARP request. */
    (void)ipnet_if_output(netif, pkt);
}
#endif /* IPNET_USE_RARP */


/*
 *===========================================================================
 *                    ipnet_eth_rarp_input
 *===========================================================================
 * Description: Input handler for ARP frames. pkt->start is the offset
 *              to the RARP header.
 * Parameters:  pkt - The ethernet frame.
 * Returns:
 *
 */
#ifdef IPNET_USE_RARP
IP_STATIC void
ipnet_eth_rarp_input(Ipnet_netif *netif, Ipcom_pkt *pkt)
{
    Ipnet_pkt_arp *rarp;
    Ip_u32         ip_addr_n;

    rarp = (Ipnet_pkt_arp *)&pkt->data[pkt->start];
    if (rarp->hrd != IPNET_ARP_HRD_ETHER
        || rarp->pro != IPNET_ARP_PRO_IP
        || rarp->hln_pln != IPNET_ARP_HLN_PLN
        || rarp->op != IPNET_RARP_OP_REPLY
        || !ipnet_eth_is_valid_node_mac(netif, rarp->eth_src))
        return;

    ipnet_timeout_cancel(netif->eth->rarp_tmo);
    /* Valid RARP reply, add my IPv4 address. */
    ip_addr_n = IPNET_IP4_GET_IPADDR(rarp->ip_dst);
    if (ipnet_ip4_add_addr(netif, ip_addr_n, 0, IP_TRUE, IPNET_ADDR_TYPE_UNICAST) < 0)
    {
        IP_PANIC2();
        return;
    }
}
#endif /* IPNET_USE_RARP */


/*
 *===========================================================================
 *                     ipnet_eth_rarp_timeout
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPNET_USE_RARP
IP_STATIC void
ipnet_eth_rarp_timeout(Ipnet_netif *netif)
{
    /* Send RARP request. */
    ipnet_eth_rarp_request(netif);

    /* schedule next RARP timeout. */
    if (ipnet_timeout_schedule(netif->eth->rarp_interval * 1000,
                               (Ipnet_timeout_handler) ipnet_eth_rarp_timeout,
                               netif,
                               &netif->eth->rarp_tmo) == 0)
    {
        netif->eth->rarp_interval <<= 1;
        if (netif->eth->rarp_interval > 60)
            netif->eth->rarp_interval = 2;
    }
}
#endif /* IPNET_USE_RARP */


/*
 *===========================================================================
 *                         ipnet_eth_ioevent
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_eth_ioevent(Ipcom_netif *ipcomif, int command, void *data, int fcflags)
{
    int ret = 0;

    switch (command)
    {
    case IP_EIOXUP :
    case IP_EIOXDOWN :
    case IP_EIOXRUNNING :
    case IP_EIOXSTOP :
        ipnet_kioevent((Ipnet_netif *) ipcomif, command, data, fcflags);
        break;

#ifdef IP_EIOXVLAN
    /*
     * IP_EIOXVLAN is a link layer notification associated with 802.1Q vlan.
     */
    case IP_EIOXVLAN:
        {
            Ipnet_netif  *netif = (Ipnet_netif *)ipcomif;
            int vid = ((Ipcom_eth_vlanreq *)data)->vid;

            if (((Ipcom_eth_vlanreq *)data)->vid_is_pvid)
            {
                /* override the port-based vid and priority unconditionally */
                netif->eth->vlan_param.id = vid;
                netif->eth->vlan_param.pri = ((Ipcom_eth_vlanreq *)data)->pri;
            }
            else
            {
                /*
                 * 'ifs' would be NULL if the specified vid is either a
                 * socket-based vlan or the vlan pseudo-interface associated
                 * with the vid has not been created.
                 */
                if (netif->eth->vlan.ifs == IP_NULL)
                    return 0;

                netif = ipcom_hash_get(netif->eth->vlan.ifs, &vid);
                if (netif == IP_NULL)
                    return 0;  /* this could be socket-based vlan, ignore it */
            }

            /*
             * The following link_hdr_size adjustment should only be made
             * if the state of egress frame type for the vid is indeed
             * changed (e.g. from tagged to untagged or vice versa).
             */
            if (netif->eth->vlan_param.tag_required !=
                        ((Ipcom_eth_vlanreq *)data)->vlan_tag_required)
            {
                netif->eth->vlan_param.tag_required =
                        ((Ipcom_eth_vlanreq *)data)->vlan_tag_required;

                /*
                 * Avoid the link_hdr_size adjustment if hardware supports
                 * vlan tag insertion on transmit. Untagged frame will be
                 * sent.
                 */
#ifdef IPCOM_USE_HW_VLAN_TAGGING
                if (IP_BIT_ISSET(netif->ipcom.drv_capabilities,
                                 IPCOM_IF_DRV_CAP_VLAN_HWTAGGING_TX))
                    return 0;
#endif /* IPCOM_USE_HW_VLAN_TAGGING */

                if (((Ipcom_eth_vlanreq *)data)->vlan_tag_required)
                    netif->ipcom.link_hdr_size += IPNET_VLAN_TAG_SIZE;
                else
                    netif->ipcom.link_hdr_size -= IPNET_VLAN_TAG_SIZE;
            }
        }
        break;
#endif /* IP_EIOXVLAN */

    default:
        ret = -1;
        break;
    }

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_eth_ip4_multicast_to_link_addr
 *===========================================================================
 * Description: Calculates the ethernet IPv4 address for the specified class D
 *              IPv4 multicast address.
 * Parameters:
 * Returns:
 *
 */
#ifdef IPCOM_USE_INET
IP_STATIC void
ipnet_eth_ip4_multicast_to_link_addr(IP_CONST Ip_u8 *ipaddr, Ip_u8 *mlt_eth_addr)
{
    mlt_eth_addr[0] = 0x01;
    mlt_eth_addr[1] = 0x00;
    mlt_eth_addr[2] = 0x5e;
    mlt_eth_addr[3] = (Ip_u8) (ipaddr[1] & 0x7f);
    mlt_eth_addr[4] = ipaddr[2];
    mlt_eth_addr[5] = ipaddr[3];
}
#endif /* IPCOM_USE_INET */


/*
 *===========================================================================
 *                    ipnet_eth_ip6_create_addr
 *===========================================================================
 * Description: Creates an unique address based on the passed prefix and the
 *              address of the ethernet card in the machine.
 * Parameters:  netif - The interface to which this address will be bound.
 *              prefix - The network identifier part of the address.
 *              in6_addr - Will hold a unique address if the function is
 *              successful.
 * Returns:     0 = succeess. <0 = error code.
 *
 */
#ifdef IPCOM_USE_INET6
IP_STATIC int
ipnet_eth_ip6_create_addr(Ipnet_netif *netif, IP_CONST struct Ip_in6_addr *prefix,
    struct Ip_in6_addr *in6_addr)
{
    /* Create the EUI-64 identifier from the 48-bit IEEE 802 address, RFC 2464 */
    ip_assert(netif->ipcom.link_addr_size == IPNET_ETH_ADDR_SIZE);
    ip_assert(netif->inet6_if_id_bits == 64);

    in6_addr->in6.addr16[0] = prefix->in6.addr16[0];
    in6_addr->in6.addr16[1] = prefix->in6.addr16[1];
    in6_addr->in6.addr16[2] = prefix->in6.addr16[2];
    in6_addr->in6.addr16[3] = prefix->in6.addr16[3];
    in6_addr->in6.addr16[4] = ((Ip_u16 *)netif->ipcom.link_addr)[0] ^ ip_htons(0x0200);
    in6_addr->in6.addr16[5] = ip_htons(0x00ff) | ((Ip_u16 *)netif->ipcom.link_addr)[1];
    in6_addr->in6.addr16[6] = ip_htons(0xfe00) | (((Ip_u16 *)netif->ipcom.link_addr)[1] & ip_htons(0x00ff));
    in6_addr->in6.addr16[7] = ((Ip_u16 *)netif->ipcom.link_addr)[2];
    return 0;
}
#endif /* IPCOM_USE_INET6 */


#ifdef IPCOM_USE_INET
/*
 *===========================================================================
 *                    ipnet_arp_probe
 *===========================================================================
 * Description: Send an ARP request.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_arp_probe(Ipnet_neigh_entry *neigh, Ip_bool unicast_probe)
{
    Ipnet_route_entry *rt = neigh->rt;

    ipnet_arp_request(rt->netif,
                      *(Ip_u32*)rt->hdr.key,
                      IP_FALSE,
                      IP_NULL,
                      (unicast_probe
                       ? (Ip_u8 *) IP_SOCKADDR_DL_LLADDR(rt->gateway)
                       : IP_NULL));
}


/*
 *===========================================================================
 *                    ipnet_arp_timeout
 *===========================================================================
 * Description: Handles ARP entry timeouts
 * Parameters:  rt - The route entry that holds the IP to MAC information
 *                   (IPNET_RTF_LLINFO flag set)
 *              is_timeout - IP_TRUE if this function is called due to a timeout
 *                           IP_FALSE if the entry is about to be deleted.
 * Returns:
 *
 */
IP_STATIC void
ipnet_arp_timeout(Ipnet_route_entry *rt, Ip_bool is_timeout)
{
    ipnet_neigh_timeout(IP_AF_INET, IPNET_NEIGH_FROM_RT(rt), is_timeout);
}


#ifdef IPNET_USE_VRRP
/*
 *===========================================================================
 *                    ipnet_vrrp_timeout
 *===========================================================================
 * Description: Handles VRRP proxy ARP entry timeouts.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_vrrp_timeout(Ipnet_route_entry *rt, Ip_bool is_timeout)
{
    Ip_u8             *mac = (Ip_u8 *) IP_SOCKADDR_DL_LLADDR(rt->gateway);
    Ipnet_netif       *netif = rt->netif;
    struct Ip_in_addr  key;

    ipcom_memcpy(&key, rt->hdr.key, sizeof(key));
    ipnet_arp_timeout(rt, is_timeout);
    (void)ipnet_vrrp_del_addr(netif,
                              mac[5],
                              key);
    if (ipnet_vrrp_get_addr_entry(netif, mac[5]))
        IP_BIT_CLR(netif->eth->vrids[mac[5] >> 5], 1 << (mac[5] & 0x1f));

    if (ipnet_ip4_get_addr_type2(key.s_addr, netif) == IPNET_ADDR_TYPE_UNICAST)
        /* Owner address, inform link local nodes about the MAC change */
        ipnet_arp_request(netif, key.s_addr, IP_TRUE, IP_NULL, IP_NULL);
}


#define IPNET_ETH_IS_VRRP_ADDR(mac) \
    (((Ip_u16 *) (mac))[0] == 0 \
     && ((Ip_u16 *) (mac))[1] == ip_htons(0x5e00) \
     && (mac)[4] == 0x01)

/*
 *===========================================================================
 *                    ipnet_eth_vrid_from_mac
 *===========================================================================
 * Description: Returns the VRID matching the address.
 * Parameters:  vrids - The head of the VRID hash table.
 *              mac - A MAC address
 * Returns:     IP_TRUE if the the key matches the configuration entry.
 *
 */
IP_STATIC Ip_u8
ipnet_eth_vrid_from_mac(Ipnet_netif *netif, Ip_u8 *mac)
{
    if (IPNET_ETH_IS_VRRP_ADDR(mac)
        && IP_BIT_ISSET(netif->eth->vrids[mac[5] >> 5], 1 << (mac[5] & 0x1f)))
        return mac[5];
    return 0;
}


/*
 *===========================================================================
 *                      ipnet_eth_try_find_vrrp_rt
 *===========================================================================
 * Description: Tries to find a VRRP route entry in the specified network.
 * Parameters:  net_rt - The route entry for the network to search.
 * Returns:     The passed network route if no VRRP route was found,
 *              a VRRP route otherwise.
 *
 */
IP_STATIC Ipnet_route_entry *
ipnet_eth_try_find_vrrp_rt(Ipnet_netif *netif, struct Ip_in_addr *src, Ipnet_route_entry *original_rt)
{
    Ipnet_route_entry    *rt;
    Ipnet_ip4_addr_entry *addr;
    struct Ip_in_addr     net;
    int                   ret;
    Ipnet_route_entry    *net_rt;

    /* Check if the requester located on one of the (sub)networks is assigned to this interface */
    for (addr = netif->inet4_addr_list; addr != IP_NULL; addr = addr->next)
    {
        if (addr->type != IPNET_ADDR_TYPE_UNICAST)
            return original_rt;

        if (ipcom_route_key_cmp(original_rt->head->bit_count,
                                src,
                                &addr->ipaddr_n,
                                &addr->netmask_n) != IPCOM_ROUTE_KEY_CMP_NO_MATCH)
            break;
    }

    if (addr == IP_NULL)
        /* The requester was not located on of (sub)nets on the incoming interface */
        return original_rt;

    net.s_addr = addr->ipaddr_n;
    ipnet_route_apply_mask(&net, &addr->netmask_n, original_rt->head->bit_count);
    ret = ipnet_route_raw_lookup(IP_AF_INET,
                                 netif->vr_index,
                                 IPCOM_ROUTE_TABLE_DEFAULT,
                                 IPNET_RTL_FLAG_DONTCLONE,
                                 &net,
                                 0,
                                 0,
                                 &net_rt);
    if (ret < 0)
        return original_rt;

    for (rt = ipnet_route_next_entry(net_rt);
         ipcom_route_key_cmp(rt->head->bit_count,
                             rt->hdr.key,
                             net_rt->hdr.key,
                             net_rt->hdr.mask) != IPCOM_ROUTE_KEY_CMP_NO_MATCH;
         rt = ipnet_route_next_entry(rt))
    {
        if (netif != rt->netif
            || IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_PROTO2 | IPNET_RTF_LLINFO))
            /* Wrong interface and/or not a proxy ARP route with a specific link address */
            continue;

        if (IPNET_ETH_IS_VRRP_ADDR(IP_SOCKADDR_DL_LLADDR(rt->gateway)) == IP_FALSE)
            /* Not a VRRP proxy ARP entry */
            continue;

        return rt;
    }
    return original_rt;
}
#endif /* IPNET_USE_VRRP */


/*
 *===========================================================================
 *                    ipnet_eth_ip4_output
 *===========================================================================
 * Description: Outputs a IPv4 packet on the Ethernet.
 * Parameters:  netif - The network interface on which the packet should
 *              be sent.
 *              next_hop - Next hop IP address (should be on link).
 *              rtindex - Last arp cache location for the 'next hop'.
 *              pkt - The packet to send.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_FASTTEXT IP_STATIC int
ipnet_eth_ip4_output(Ipnet_netif *netif, Ipnet_route_entry *rt, Ipcom_pkt *pkt)
{
    Ipnet_pkt_eth   *eth;
    Ipnet_arp_entry *arp_entry = IPNET_ARP(rt);
    int              ret;

    IPCOM_WV_MARKER_1 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_VERBOSE, 1, 1, IPCOM_WV_NETDEVENT_START,
                       ipnet_eth_ip4_output, IPCOM_WV_IPNET_ETH_MODULE, IPCOM_WV_NETD_ARP);
    IPNET_STATS(eth_output_ip4++);
    ip_assert((pkt->end - pkt->start) >= IPNET_IP_HDR_SIZE);
    ip_assert(pkt->start >= netif->ipcom.link_hdr_size);
    ip_assert(rt != IP_NULL);

    if (IP_UNLIKELY(IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_UP)))
    {
        ret = -IP_ERRNO_ENETDOWN;
        goto badout;
    }

#ifdef IPNET_USE_PER_SOCKET_VLAN_TAG
    if (IPNET_ETH_PKT_GET_VLAN_TAG(pkt) & 0xffff)
        eth = ipnet_vlan_add_frame_header(pkt, IPNET_ETHFRAME_IP, netif);
    else
#endif
    {
        pkt->start -= netif->ipcom.link_hdr_size;
        /* Set Ethernet type to IP frame */
        eth = (Ipnet_pkt_eth *) &pkt->data[pkt->start];
        IPNET_SET_ETHTYPE(netif, eth, IPNET_ETHFRAME_IP);
    }

    /* Broadcasting */
    if (IP_UNLIKELY(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_BROADCAST)))
    {
        IPNET_SET_ETHADDR_BROADCAST(eth->dst);
        if (rt->gateway != IP_NULL)
            ipnet_arp_set_state(rt, IPNET_ND_REACHABLE, eth->dst);
    }

    /* NOARP interface */
    else if (IP_UNLIKELY(IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_NOARP)))
    {
        IPNET_SET_ETHADDR_NONE(eth->dst);
    }

    /* Multicasting */
    else if (IP_UNLIKELY(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_MULTICAST)))
    {
#ifndef IPNET_USE_MULTICAST_BROADCAST
#ifndef IPROHC
        ipnet_eth_ip4_multicast_to_link_addr((Ip_u8 *)((Ipnet_pkt_ip *)&pkt->data[pkt->ipstart])->dst, eth->dst);
#else
	if (pkt->flag_rohc)
	    ipnet_eth_ip4_multicast_to_link_addr((Ip_u8 *)pkt->dst, eth->dst);
	else
            ipnet_eth_ip4_multicast_to_link_addr((Ip_u8 *)((Ipnet_pkt_ip *)&pkt->data[pkt->ipstart])->dst, eth->dst);
#endif /* IPROHC */
#else
        IPNET_SET_ETHADDR_BROADCAST(eth->dst);
#endif
    }

    else
    {
        /* Unicast packet. */
        if (IP_UNLIKELY(arp_entry == IP_NULL) || IP_UNLIKELY(rt->gateway == IP_NULL))
        {
            /* Invalid route, it was not directly reachable. */
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                              1, 2, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_SEND,
                              ipnet_eth_ip4_output, IPCOM_WV_NETD_BADROUTE,
                              IPCOM_WV_IPNET_ETH_MODULE, IPCOM_WV_NETD_ARP);
            IPNET_STATS(eth_output_ip4_err++);
            ret = -IP_ERRNO_ENETUNREACH;
            goto badout;
        }

        ip_assert(rt->gateway->sa_family == IP_AF_LINK);

#ifdef IPCOM_STORE_PHYSICAL_IF
        pkt->physical_ifindex = arp_entry->ne.physical_ifindex;
#endif

        if (IP_LIKELY(arp_entry->ne.state == IPNET_ND_REACHABLE)
            && IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_PROGRESS))
            arp_entry->ne.is_making_forward_progress = IP_TRUE;

        switch (arp_entry->ne.state)
        {
        case IPNET_ND_STALE:
            ipnet_arp_set_state(rt, IPNET_ND_DELAY, IP_NULL);
            /* Fall through */
        case IPNET_ND_REACHABLE:
        case IPNET_ND_DELAY:
        case IPNET_ND_PROBE:
            /* The ARP entry is valid */
            IPNET_SET_ETHADDR(eth->dst, IP_SOCKADDR_DL_LLADDR(rt->gateway));
            break;

        case IPNET_ND_UNINITIALIZED:
        case IPNET_ND_INCOMPLETE:
            ipnet_arp_set_state(rt, IPNET_ND_INCOMPLETE, IP_NULL);
            ipnet_neigh_queue_until_resolved(&arp_entry->ne.pending, pkt);

            if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_X_DONT_ROUTE)
                || rt->rt_template == IP_NULL)
                (void)ipnet_arp_try_resolve(netif->vr_index,
                                            (struct Ip_in_addr *) rt->hdr.key,
                                            netif);
            else
                ipnet_arp_try_resolve_on_all_links(rt->rt_template,
                                                   (struct Ip_in_addr *) rt->hdr.key);
            /* Return success even if state is unknown. */
            return 0;

        case IPNET_ND_PROXY:
            /* Can't output datagram to proxy ARP entry */
            IPCOM_LOG1(WARNING, "Eth IPv4: trying to send to %s using a proxy ARP entry",
                       ipcom_inet_ntop(IP_AF_INET, rt->hdr.key, ipnet->log_buf, sizeof(ipnet->log_buf)));
            ret = -IP_ERRNO_EHOSTUNREACH;
            goto badout;

        default:
            IP_PANIC();
            ret = -IP_ERRNO_EINVAL;
            goto badout;
        }
    }

#ifdef IPNET_USE_VRRP
    if (IPCOM_PKT_GET_VRID(pkt))
    {
        eth->src[0] = 0x00;
        eth->src[1] = 0x00;
        eth->src[2] = 0x5e;
        eth->src[3] = 0x00;
        eth->src[4] = 0x01;
        eth->src[5] = IPCOM_PKT_GET_VRID(pkt);
    }
    else
#endif
        IPNET_SET_ETHADDR(eth->src, netif->ipcom.link_addr);

#ifdef IPNET_COMPENSATE_UNRELIABLE_NETLINK
    if ((IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_X_NEIGH_ACK)
         && (arp_entry != IP_NULL)
         && (arp_entry->app_retries > 0)))
        /* Compensate for unreliable netlink implementations */
        IPNET_NETLINKSOCK(ipnet_rtnetlink_route_change(rt));
#endif

    ret = ipnet_if_output(netif, pkt);
    if (IP_UNLIKELY(ret != 0))
    {
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                          1, 3, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_SEND,
                          ipnet_eth_ip4_output, IPCOM_WV_NETD_NETDOWN,
                          IPCOM_WV_IPNET_ETH_MODULE, IPCOM_WV_NETD_ARP);
        IPNET_STATS(eth_output_ip4_drverr++);
    }
    return ret;

 badout:
    IPCOM_MIB2_NETIF(ifOutDiscards++);
    ip_assert(ret < 0);
    ipcom_pkt_free(pkt);
    return ret;
}
#endif


/*
 *===========================================================================
 *                    ipnet_eth_ip6_output
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPCOM_USE_INET6
IP_STATIC int
ipnet_eth_ip6_output(Ipnet_netif *netif, Ipnet_route_entry *rt, Ipcom_pkt *pkt)
{
    Ipnet_pkt_eth *eth;
    int            ret;

    IPCOM_WV_MARKER_1 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_VERBOSE, 1, 4, IPCOM_WV_NETDEVENT_START,
                       ipnet_eth_ip6_output, IPCOM_WV_IPNET_ETH_MODULE, IPCOM_WV_NETD_ARP);
    IPNET_STATS(eth_output_ip6++);
    ip_assert((pkt->end - pkt->start) >= IPNET_IP6_HDR_SIZE);
    ip_assert(pkt->start >= netif->ipcom.link_hdr_size);

    if (IP_UNLIKELY(IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_UP)))
    {
        ipcom_pkt_free(pkt);
        ret = -IP_ERRNO_ENETDOWN;
        goto badout;
    }

#ifdef IPNET_USE_PER_SOCKET_VLAN_TAG
    if (IPNET_ETH_PKT_GET_VLAN_TAG(pkt) & 0xffff)
        eth = ipnet_vlan_add_frame_header(pkt, IPNET_ETHFRAME_IPV6, netif);
    else
#endif
    {
        pkt->start -= netif->ipcom.link_hdr_size;
        eth = (Ipnet_pkt_eth *)&pkt->data[pkt->start];
        if (pkt->force_frame_type != 0)
            IPNET_SET_ETHTYPE(netif, eth, pkt->force_frame_type);
        else
            IPNET_SET_ETHTYPE(netif, eth, IPNET_ETHFRAME_IPV6);
    }

    IPNET_SET_ETHADDR(eth->src, netif->ipcom.link_addr);

    if (IP_UNLIKELY(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_MULTICAST)))
    {
        Ipnet_pkt_ip6 *ip6;
        ip6 = (Ipnet_pkt_ip6*) ((Ip_u8 *) eth + netif->ipcom.link_hdr_size);
#ifndef IPNET_USE_MULTICAST_BROADCAST
#ifndef IPROHC
        IPNET_SET_ETHADDR_IP6_MULTICAST(eth->dst, &ip6->dst);
#else
        if (pkt->flag_rohc)
            IPNET_SET_ETHADDR_IP6_MULTICAST(eth->dst, (struct Ip_in6_addr *)pkt->dst);
        else
            IPNET_SET_ETHADDR_IP6_MULTICAST(eth->dst, &ip6->dst);
#endif /* IPROHC */
#else
        IPNET_SET_ETHADDR_BROADCAST(eth->dst);
#endif
    }
    else if (IP_UNLIKELY(IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_NOARP)))
    {
        /* NOARP interface */
        IPNET_SET_ETHADDR_NONE(eth->dst);
    }
    else
    {
        /* Unicast packet. */
        if (IP_UNLIKELY(IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_LLINFO))
            || IP_UNLIKELY(rt->gateway == IP_NULL))
        {
            /* Invalid route, it was not directly reachable. */
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                              1, 5, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_SEND,
                              ipnet_eth_ip6_output, IPCOM_WV_NETD_BADROUTE,
                              IPCOM_WV_IPNET_ETH_MODULE, IPCOM_WV_NETD_ARP);
            IPNET_STATS(eth_output_ip6_err++);
            ipcom_pkt_free(pkt);
            ret = -IP_ERRNO_ENETUNREACH;
            goto badout;
        }
        ip_assert(rt->gateway->sa_family == IP_AF_LINK);
        ip_assert(((struct Ip_sockaddr_dl *)rt->gateway)->sdl_alen == IPNET_ETH_ADDR_SIZE);
        IPNET_SET_ETHADDR(eth->dst, IP_SOCKADDR_DL_LLADDR(rt->gateway));

#ifdef IPCOM_STORE_PHYSICAL_IF
        pkt->physical_ifindex = IPNET_IP6_NC(rt)->ne.physical_ifindex;
#endif

#ifdef IPNET_COMPENSATE_UNRELIABLE_NETLINK
        if (IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_X_NEIGH_ACK)
            && nc_entry->remaining_app_probes > 0)
            /* Compensate for unreliable netlink implementations */
            IPNET_NETLINKSOCK(ipnet_rtnetlink_route_change(rt));
#endif
    }

    ret = ipnet_if_output(netif, pkt);
    if (IP_UNLIKELY(ret != 0))
    {
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                          1, 6, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_SEND,
                          ipnet_eth_ip6_output, IPCOM_WV_NETD_NETDOWN,
                          IPCOM_WV_IPNET_ETH_MODULE, IPCOM_WV_NETD_ARP);
        IPNET_STATS(eth_output_ip6_drverr++);
    }
    return ret;

 badout:
    IPCOM_MIB2_NETIF(ifOutDiscards++);
    return ret;

}
#endif /* IPCOM_USE_INET6 */


#ifdef IPCOM_USE_INET
/*
 *===========================================================================
 *                    ipnet_eth_send_pending_pkt
 *===========================================================================
 * Description: Sends a pending frame since ARP resolution is now finished.
 * Parameters:  neigh - Neighbor entry that has received its link layer address.
 *              pkt - The frame to send, pkt->start is the offset to the
 *                    unfinshed Ethernet header.
 * Returns:
 *
 */
IP_STATIC void
ipnet_eth_send_pending_pkt(Ipnet_neigh_entry *neigh,
                           Ipcom_pkt *pkt)
{
    Ipnet_pkt_eth         *eth;
    Ipnet_route_entry     *rt = neigh->rt;
    Ipnet_netif           *netif = rt->netif;
    struct Ip_sockaddr_dl *gw;

    eth = (Ipnet_pkt_eth *) &pkt->data[pkt->start];
    gw = (struct Ip_sockaddr_dl *) rt->gateway;
    IPNET_SET_ETHADDR(eth->dst, IP_SOCKADDR_DL_LLADDR(gw));
    IPNET_SET_ETHADDR(eth->src, netif->ipcom.link_addr);
    (void)ipnet_if_output(netif, pkt);
}


/*
 *===========================================================================
 *                  ipnet_eth_update_arp_entry_from_msg
 *===========================================================================
 * Description: Updates or creates an IPv4 to MAC mapping using the
 *              information from an incoming ARP request/response.
 * Parameters:  netif - The interface/link the mapping is for.
 *              arp_ip_src_addr - The IP source address in the ARP message.
 *              arp_hw_src_addr - The MAC source address in the ARP message.
 *              can_create - IP_TRUE if a new message should be created
 *                           if not already exist.
 *                           IP_FALSE if already present entry should be
 *                           updated, but not created if not present.
 * Returns:
 *
 */
IP_STATIC void
ipnet_eth_update_arp_entry_from_msg(Ipnet_netif *netif,
                                    IP_CONST struct Ip_in_addr *arp_ip_src_addr,
                                    Ip_u8 *arp_hw_src_addr,
#ifdef IPCOM_STORE_PHYSICAL_IF
                                    Ip_u32 physical_ifindex,
#endif
                                    Ip_bool can_create)
{
    int                ret;
    Ipnet_route_entry *requester_rt;
#ifdef IPCOM_STORE_PHYSICAL_IF
    Ipnet_arp_entry   *arp_entry;
#endif

    /* Cache the IP to MAC address mapping of the requester */
    if (ipnet_route_lookup_ip4_dont_route(IP_TRUE,
                                          netif->vr_index,
                                          arp_ip_src_addr,
                                          netif->ipcom.ifindex,
                                          &requester_rt) == 0)
    {
        ipnet_arp_set_state(requester_rt, IPNET_ND_REACHABLE, arp_hw_src_addr);
    }

    ret = ipnet_route_raw_lookup(IP_AF_INET,
                                 netif->vr_index,
                                 IPCOM_ROUTE_TABLE_DEFAULT,
                                 (can_create ? 0 : IPNET_RTL_FLAG_DONTCLONE) | IPNET_RTL_FLAG_LINK_LOCAL,
                                 arp_ip_src_addr,
                                 0,
                                 netif->ipcom.ifindex,
                                 &requester_rt);
    if (ret != IPNET_ROUTE_PERFECT_MATCH
        || IPNET_ARP(requester_rt) == IP_NULL)
    {
        /* No ARP request has been issued to the sender of the message */
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                          1, 7, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                          ipnet_eth_update_arp_entry_from_msg, IPCOM_WV_NETD_INVAL,
                          IPCOM_WV_IPNET_ETH_MODULE, IPCOM_WV_NETD_ARP);
        IPNET_STATS(eth_input_arp_reperr++);
        return;
    }

    /* Update our ARP cache */
#ifdef IPCOM_STORE_PHYSICAL_IF
    arp_entry = IPNET_ARP(requester_rt);

    if (arp_entry->ne.physical_ifindex == 0)
        arp_entry->ne.physical_ifindex = physical_ifindex;
    else if (arp_entry->ne.physical_ifindex != physical_ifindex)
    {
        arp_entry->ne.physical_ifindex = physical_ifindex;
        IPNET_NETLINKSOCK(ipnet_rtnetlink_route_change(requester_rt));
    }
#endif

    if (IP_BIT_ISFALSE(requester_rt->hdr.flags, IPNET_RTF_PROTO2)
        && requester_rt->metrics.rmx_expire != IPCOM_ADDR_INFINITE)
    {
        Ip_bool has_mac_addr_changed;

        has_mac_addr_changed = (ipcom_memcmp(arp_hw_src_addr,
                                             IP_SOCKADDR_DL_LLADDR(requester_rt->gateway),
                                             IPNET_ETH_ADDR_SIZE) != 0);
        ipnet_arp_set_state(requester_rt,
                            (has_mac_addr_changed
                             ? IPNET_ND_STALE
                             : IPNET_ND_REACHABLE),
                            arp_hw_src_addr);
    }
}


/*
 *===========================================================================
 *                    ipnet_eth_arp_input
 *===========================================================================
 * Description: Input handler for ARP frames. pkt->start is the offset
 *              to the ARP header.
 * Parameters:  pkt - The ethernet frame.
 * Returns:     IP_TRUE if the packet ownership was removed from the caller.
 *
 */
IP_STATIC Ip_bool
ipnet_eth_arp_input(Ipnet_netif *netif, Ipcom_pkt *pkt, Ipnet_pkt_eth *eth, Ip_u16 rcv_link_hdr_size)
{
    Ipnet_pkt_arp     *arp;
    int                ret;
    Ip_u32             arp_src_addr_n;
    Ip_u32             arp_dst_addr_n;
    int                arp_dst_addr_type;
    Ip_bool            create_arp = IP_FALSE;
    Ipnet_route_entry *rt;
    IPS_MNTN_CCORE_CAPTURE_CB            pCaptureCb;
    IPS_MNTN_CCORE_CAPTURE_PARA_STRU     stIpsMntnCapturePara;

    arp = (Ipnet_pkt_arp *)&pkt->data[pkt->start];

    pCaptureCb  = g_astCallbackTbl[IPS_MNTN_CCORE_RECV_ARP_CB];

    stIpsMntnCapturePara.paucNetIfName    = (unsigned char *)(netif->ipcom.name);
    stIpsMntnCapturePara.pucData          = &(pkt->data[pkt->start]);
    stIpsMntnCapturePara.usLen            = (unsigned short)(pkt->end - pkt->start);

    if ( IP_NULL != pCaptureCb )
    {
        pCaptureCb(&stIpsMntnCapturePara);
    }

    IPCOM_WV_MARKER_1 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_VERBOSE, 1, 8, IPCOM_WV_NETDEVENT_START,
                       ipnet_eth_arp_input, IPCOM_WV_IPNET_ETH_MODULE, IPCOM_WV_NETD_ARP);
    IPNET_STATS(eth_input_arp++);
    if (arp->hrd != IPNET_ARP_HRD_ETHER
        || arp->pro != IPNET_ARP_PRO_IP
        || arp->hln_pln != IPNET_ARP_HLN_PLN
        || !ipnet_eth_is_valid_node_mac(netif, arp->eth_src))
    {
        /* Last check verifies that chapter 3.3.2 of RFC 1812 is not
           violated
           ...
           A router MUST not believe any ARP reply that claims that
           the Link Layer address of another host or router is a
           broadcast or multicast address.
           ...
         */
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                          1, 9, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_SEND,
                          ipnet_eth_arp_input, IPCOM_WV_NETD_BADOPT,
                          IPCOM_WV_IPNET_ETH_MODULE, IPCOM_WV_NETD_ARP);
        IPNET_STATS(eth_input_arp_bad++);
        return IP_FALSE;
    }

    if (IPNET_ARE_ETHADDR_EQUAL(eth->src, netif->ipcom.link_addr)
        || (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_LINK_OTHER)
#ifdef IPNET_USE_VRRP
            && !IPNET_ETH_IS_VRRP_ADDR(arp->eth_dst)
#endif
            ))
        /* This frame was sent by this node or it is a response sent
           to a unicast MAC address that is not ours, ignore */
        return IP_FALSE;

    arp_src_addr_n = IPNET_IP4_GET_IPADDR(arp->ip_src);
    arp_dst_addr_n = IPNET_IP4_GET_IPADDR(arp->ip_dst);

    if (rcv_link_hdr_size == netif->ipcom.link_hdr_size
        && arp->op == IPNET_ARP_OP_REQUEST
        && !IPNET_IP4_IS_LINK_LOCAL(&arp_dst_addr_n)
        && netif->inet4_addr_list != IP_NULL
        && arp_dst_addr_n == netif->inet4_addr_list->ipaddr_n)
    {
        /*
         * Request for the primary IPv4 address on this node. Generate
         * a quick response.
         */
        pkt->start -= rcv_link_hdr_size;

        /* actually eth_hdr_len+28 but 60 is min length. */
        pkt->end = pkt->start + IPNET_ETH_MIN_FRAME_LEN;
        /* Some OS:es can handle frames smaller than 60 bytes and
           the request can be less than 60 byte in size then  */
        pkt->end = IP_MIN(pkt->end, pkt->maxlen);

        /*
         * Create Ethernet header
         */
        IPNET_SET_ETHADDR(eth->dst, arp->eth_src);
        IPNET_SET_ETHADDR(eth->src, netif->ipcom.link_addr);
        IPNET_SET_ETHTYPE(netif, eth, IPNET_ETHFRAME_ARP);

        /*
         * Compose the ARP header
         */

        /* Change OP code to ARP reply. */
        arp->op = IPNET_ARP_OP_REPLY;

        /* ARP target. */
        IPNET_SET_ETHADDR(arp->eth_dst, arp->eth_src);
        IPNET_IP4_SET_IPADDR(arp->ip_dst, arp_src_addr_n);

        /* ARP sender. */
        IPNET_SET_ETHADDR(arp->eth_src, netif->ipcom.link_addr);
        IPNET_IP4_SET_IPADDR(arp->ip_src, arp_dst_addr_n);

        /* Output ARP reply, packet consumed */
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                          1, 111, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                          ipnet_eth_arp_input, IPCOM_WV_NETD_INFO_RECEIVE,
                          IPCOM_WV_IPNET_ETH_MODULE, IPCOM_WV_NETD_ARP);
        IPNET_STATS(eth_output_arp_rep++);
        (void)ipnet_if_output(netif, pkt);
        return IP_TRUE;
    }

    arp_dst_addr_type = ipnet_ip4_get_addr_type2(arp_dst_addr_n, netif);
#ifdef IPNET_USE_RFC3927
    if (netif->inet4_lladdr.s_addr != IP_INADDR_ANY
        && (netif->inet4_lladdr.s_addr == arp_src_addr_n
            || (IP_INADDR_ANY == arp_src_addr_n
                && netif->inet4_lladdr.s_addr == arp_dst_addr_n)))
    {
        ipnet_ip4_lladdr_conflict(netif);
        return IP_FALSE;
    }
#endif /* IPNET_USE_RFC3927 */

    /* Check for static proxy ARP entries, always use the proxy ARP MAC
       address even if the address is local to this host */
    ret = ipnet_route_raw_lookup2(IP_AF_INET,
                                  netif->vr_index,
                                  IPCOM_ROUTE_TABLE_DEFAULT,
                                  (IPNET_RTL_FLAG_DONTCLONE
                                   | IPNET_RTL_FLAG_LINK_LOCAL),
                                  &arp_dst_addr_n,
                                  0,
                                  IP_NULL,
                                  IP_NULL,
                                  netif->ipcom.ifindex,
                                  &rt);

    if (arp_dst_addr_type == IPNET_ADDR_TYPE_UNICAST
#ifdef IPNET_USE_VRRP
        || (ret >= 0
            && IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_LLINFO)
            && IPNET_ETH_IS_VRRP_ADDR(IP_SOCKADDR_DL_LLADDR(rt->gateway)))
#endif
        )
    {
        create_arp = IP_TRUE;
    }

    ipnet_eth_update_arp_entry_from_msg(netif,
                                        (struct Ip_in_addr *) &arp_src_addr_n,
                                        arp->eth_src,
#ifdef IPCOM_STORE_PHYSICAL_IF
                                        pkt->physical_ifindex,
#endif
                                        create_arp);

    if (arp->op == IPNET_ARP_OP_REQUEST)
    {
        Ip_bool is_proxy_arp = IP_FALSE;

        if (ret == IPNET_ROUTE_PERFECT_MATCH
            || arp_dst_addr_type != IPNET_ADDR_TYPE_UNICAST)
        {
            Ipnet_route_entry *original_rt;

            if (ret != IPNET_ROUTE_PERFECT_MATCH)
                /* Check for PPP peer host routes or network routes flagged "proxyarp" */
                ret = ipnet_route_lookup(IP_AF_INET,
                                         netif->vr_index,
                                         IPCOM_ROUTE_TABLE_DEFAULT,
                                         IPNET_RTL_FLAG_DONTCLONE,
                                         &arp_dst_addr_n,
                                         0,
                                         0,
                                         &rt);

            if (ret != IPNET_ROUTE_PERFECT_MATCH
                && ret != IPNET_ROUTE_MASK_MATCH)
            {
                /* ARP request for another host on a network we do not know about */
                IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                                  1, 10, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                                  ipnet_eth_arp_input, IPCOM_WV_NETD_INFO_RECEIVE,
                                  IPCOM_WV_IPNET_ETH_MODULE, IPCOM_WV_NETD_ARP);
                IPNET_STATS(eth_input_arp_reqother++);
                return IP_FALSE;
            }

            original_rt = rt;
            if (rt->netif != netif && rt->rt_template != IP_NULL)
            {
                /* The IPNET_RTF_PROTO2 flags is only set on the template
                   route if this node should proxy ARP */
                rt = rt->rt_template;
            }

            if (IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_PROTO2))
                /* Not acting as proxy */
                return IP_FALSE;

            if (IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_LLINFO))
            {
                /* The host will respond with its own MAC address */

                if (rt->netif == netif
                    && (rt->next != IP_NULL || rt->prev != IP_NULL))
                {
                    /* The current found route entry points to the interface
                       where the ARP request was received, no response will be
                       created in that case. Check if there are routes that
                       are identical in all but the interface */
                    Ipnet_route_entry *tmp_rt = rt;

                    while (tmp_rt->prev != IP_NULL)
                        tmp_rt = tmp_rt->prev;

                    do
                    {
                        if (netif != tmp_rt->netif
                           && IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_PROTO2))
                        {
                            rt = tmp_rt;
                            break;
                        }
                    } while (IP_NULL != (tmp_rt = tmp_rt->next));
                }

                if (netif == rt->netif)
                    /* Rx and Tx interface must not be the same when using
                       the physical MAC address */
                    return IP_FALSE;

                if (ipnet_sysvar_netif_get_as_int_ex(IP_AF_INET,
                                                     netif,
                                                     "EnableNetworkProxyArp",
                                                     0,
                                                     ipnet_bool_map) == 0)
                    /* Network proxy ARP:ing is not enabled on the Rx
                       interface */
                    return IP_FALSE;

                if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_CLONING))
                {
                    Ipnet_arp_entry *arp_entry = IPNET_ARP(original_rt);

                    if (arp_entry == IP_NULL
                        || arp_entry->ne.state == IPNET_ND_UNINITIALIZED
                        || arp_entry->ne.state == IPNET_ND_INCOMPLETE)
                    {
                        ipnet_arp_try_resolve_on_all_links(rt, (struct Ip_in_addr *) &arp_dst_addr_n);
                        return IP_FALSE;
                    }
                }

#ifdef IPNET_USE_VRRP
                /* Try to found a matching VRRP route */
                rt = ipnet_eth_try_find_vrrp_rt(netif,
                                                (struct Ip_in_addr *) &arp_src_addr_n,
                                                rt);
#endif
            }

            if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_LLINFO))
                /* Proxy ARP using a specific MAC address */
                is_proxy_arp = IP_TRUE;
            /* else: use our own physical MAC address */

            rt->use++;
        }

        /* Compose the response. */

        /*
         * re-adjust pkt->start based on the actual link header size
         * of the received frame. This is necessary because it is possible
         * that untagged arp request is received but the netif is configured
         * to send a tagged response.
         */
        pkt->start -= rcv_link_hdr_size;

        /* actually eth_hdr_len+28 but 60 is min length. */
        pkt->end = pkt->start + IPNET_ETH_MIN_FRAME_LEN;
        /* Some OS:es can handle frames smaller than 60 bytes and
           the request can be less than 60 byte in size then  */
        pkt->end = IP_MIN(pkt->end, pkt->maxlen);

        /* Ethernet destination <= ARP source. */
        if (IPNET_IP4_IS_LINK_LOCAL(&arp_dst_addr_n))
            /* All ARP replies for link local IPv4 address
               must be sent to the Ethernet broadcast address */
            IPNET_SET_ETHADDR_BROADCAST(eth->dst);
        else
            IPNET_SET_ETHADDR(eth->dst, arp->eth_src);

        IPNET_SET_ETHADDR(eth->src, netif->ipcom.link_addr);
#ifdef IPNET_USE_VRRP
        if (IPNET_ETH_IS_VRRP_ADDR(arp->eth_src))
            IPNET_SET_ETHADDR(eth->src, arp->eth_src);
#endif

#if defined(IPNET_USE_VLAN) && defined(IPCOM_VXWORKS_USE_MUX_L2)
        /*
         * It is possible that a tagged frame is received but an untagged frame
         * must be sent for the given vlan (or vice versa). Even though this
         * scenario is somewhat unusual, it is an acceptable configuration as
         * per 802.1Q. If that happens, the entire arp response must be
         * adjusted so that the vlan tag can be removed from (or inserted to)
         * the ethernet header for the response.
         */
        if (rcv_link_hdr_size != netif->ipcom.link_hdr_size)
        {
            Ipnet_pkt_arp  temp_arp = *arp; /* copy the received arp contents */

            /*
             * re-initalize the arp pointer since pkt->start has changed.
             * netif->ipcom.link_hdr_size reflects the actual link header
             * size that will be used for the arp response.
             */
            arp = (Ipnet_pkt_arp *)&pkt->data[pkt->start+netif->ipcom.link_hdr_size];

            /* restore the Ipnet_pkt_arp that is previously saved */
            *arp = temp_arp;
        }
#endif
        /* IPNET_USE_VLAN && IPCOM_VXWORKS_USE_MUX_L2 */
        /* configure the frame ether type */
        IPNET_SET_ETHTYPE(netif, eth, IPNET_ETHFRAME_ARP);

        /* Change OP code to ARP reply. */
        arp->op = IPNET_ARP_OP_REPLY;

        /* ARP target. */
        IPNET_SET_ETHADDR(arp->eth_dst, arp->eth_src);
        IPNET_IP4_SET_IPADDR(arp->ip_dst, arp_src_addr_n);

        /* ARP sender. */
        if (is_proxy_arp)
            IPNET_SET_ETHADDR(arp->eth_src, IP_SOCKADDR_DL_LLADDR(rt->gateway));
        else
            IPNET_SET_ETHADDR(arp->eth_src, netif->ipcom.link_addr);
        IPNET_IP4_SET_IPADDR(arp->ip_src, arp_dst_addr_n);

        /* Output ARP reply, packet consumed */
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                          1, 11, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                          ipnet_eth_arp_input, IPCOM_WV_NETD_INFO_RECEIVE,
                          IPCOM_WV_IPNET_ETH_MODULE, IPCOM_WV_NETD_ARP);
        IPNET_STATS(eth_output_arp_rep++);
        (void)ipnet_if_output(netif, pkt);
        return IP_TRUE;
    }

    if (arp->op == IPNET_ARP_OP_REPLY)
    {
        Ip_bool check_for_dup = IP_TRUE;

#ifdef IPNET_USE_VRRP
        if (IPNET_ETH_IS_VRRP_ADDR(arp->eth_src)
            && ipnet_vrrp_owns_addr(netif,
                                    ipnet_eth_vrid_from_mac(netif, arp->eth_src),
                                    *(struct Ip_in_addr*) &arp_src_addr_n))
        {
            check_for_dup = IP_FALSE;
        }
#endif

        /* Check for ARP reply from our own IP address
           (gratuitous ARP detected duplicate address) */
        if (check_for_dup
            && ipnet_ip4_get_addr_type2(arp_src_addr_n, netif)
            == IPNET_ADDR_TYPE_UNICAST)
        {
            Ipnet_ip4_addr_entry *addr;

            IPCOM_LOG1(WARNING, "IPv4: duplicate address detected: %s",
                       ipcom_inet_ntop(IP_AF_INET,
                                       &arp_src_addr_n,
                                       ipnet->log_buf,
                                       sizeof(ipnet->log_buf)));

            addr = ipnet_ip4_get_addr_entry(arp_src_addr_n,
                                            netif->vr_index,
                                            netif);
            ip_assert(addr != IP_NULL);

            if (addr && IP_BIT_ISSET(addr->flags, IPNET_IP4_ADDR_FLAG_TENTATIVE)
                && ipnet_sysvar_netif_get_as_int_ex(IP_AF_INET,
                                                    netif,
                                                    "DeleteAddressOnDuplicateDetect",
                                                    0,
                                                    ipnet_bool_map))
            {
                IPCOM_LOG1(WARNING, "IPv4: deleting duplicate address: %s.",
                           ipcom_inet_ntop(IP_AF_INET,
                                           &arp_src_addr_n,
                                           ipnet->log_buf,
                                           sizeof(ipnet->log_buf)));

                (void) ipnet_ip4_remove_addr(netif, arp_src_addr_n);
            }
        }

        return IP_FALSE;
    }

    IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                      1, 12, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                      ipnet_eth_arp_input, IPCOM_WV_NETD_INFO_DROPPED,
                      IPCOM_WV_IPNET_ETH_MODULE, IPCOM_WV_NETD_ARP);
    IPNET_STATS(eth_input_arp_unknown++);
    return IP_FALSE;
}


/*
 *===========================================================================
 *                       ipnet_arp_init
 *===========================================================================
 * Description: Initialize the route entry to be used in ARP.
 * Parameters:  rt - The route table entry to initialize as an ARP entry.
 * Returns:
 *
 */
IP_STATIC void
ipnet_arp_init(Ipnet_route_entry *rt)
{
    ipnet_neigh_init(rt,
                     ipnet_arp_timeout,
                     ipnet_arp_probe,
                     ipnet_ip4_schedule_host_unreachable,
                     ipnet_eth_send_pending_pkt);
}


/*
 *===========================================================================
 *                    ipnet_arp_try_resolve
 *===========================================================================
 * Description: Tries to learn the Ethernet address of the specified target.
 * Parameters:  vr - virtual router index (0=default VR)
 *              target - IPv4 address of target for which the link layer
 *                       address should be learned.
 *              netif - The interface to send the ARP request or IP_NULL
 *                      if the stack should pick the interface.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_arp_try_resolve(Ip_u16 vr, struct Ip_in_addr *target, Ipnet_netif *netif)
{
    Ipnet_route_entry  *rt;
    Ipnet_arp_entry    *arp_entry;
    int                 ret;

    ret = ipnet_route_raw_lookup(IP_AF_INET,
                                 vr,
                                 IPCOM_ROUTE_TABLE_DEFAULT,
                                 IPNET_RTL_FLAG_LINK_LOCAL,
                                 target,
                                 0,
                                 netif ? netif->ipcom.ifindex : 0,
                                 &rt);

    if ((ret != 0) || IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_LLINFO))
        return -IP_ERRNO_ENETUNREACH;

    arp_entry = IPNET_ARP(rt);

    switch (arp_entry->ne.state)
    {
    case IPNET_ND_UNINITIALIZED:
        ipnet_arp_set_state(rt, IPNET_ND_INCOMPLETE, IP_NULL);
        break;
    case IPNET_ND_REACHABLE:
    case IPNET_ND_STALE:
    case IPNET_ND_DELAY:
        ipnet_arp_set_state(rt, IPNET_ND_PROBE, IP_NULL);
        break;
    default:
        break;
    }

    ip_assert(rt->timedout_cb);

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_arp_try_resolve_on_all_links
 *===========================================================================
 * Description: Tries to learn the Ethernet address of the specified target.
 * Parameters:  rt - Network route for the network to resolve the address on.
 *              target - IPv4 address of the target.
 * Returns:
 *
 */
IP_STATIC void
ipnet_arp_try_resolve_on_all_links(Ipnet_route_entry *rt, struct Ip_in_addr *target)
{
    ip_assert(rt != IP_NULL);
    ip_assert(IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_CLONING));

    if (rt == IP_NULL)
        return;

    while (rt->prev != IP_NULL)
        rt = rt->prev;

    while (rt != IP_NULL)
    {
        if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_CLONING))
            (void)ipnet_arp_try_resolve(IPNET_ROUTE_GET_VR(rt->head),
                                        target,
                                        rt->netif);
        rt = rt->next;
    }
}
#endif /* IPCOM_USE_INET */


/*
 *===========================================================================
 *                    ipnet_eth_input
 *===========================================================================
 * Description: Input handler for ethernet frames. pkt->start is the offset
 *              to the ethernet header.
 * Parameters:  pkt - The ethernet frame.
 * Returns:     0 = success, <0 error code
 *
 */
IP_FASTTEXT IP_STATIC int
ipnet_eth_input(Ipnet_netif *netif, Ipcom_pkt *pkt)
{
    Ipnet_pkt_eth *eth;
    int            frame_type;
    Ip_u16         link_hdr_size;

    IPS_MNTN_CCORE_CAPTURE_CB            pCaptureCb;
    IPS_MNTN_CCORE_CAPTURE_PARA_STRU     stIpsMntnCapturePara;

#ifdef IPNET_USE_VLAN
    int vid = 0;
#endif /* IPNET_USE_VLAN */

    ip_assert(netif->ipcom.link_addr_size == IPNET_ETH_ADDR_SIZE);
    ip_assert(netif->ipcom.type == IP_IFT_ETHER || netif->ipcom.type == IP_IFT_IEEE80211);

    IPCOM_WV_MARKER_1 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_VERBOSE, 1, 13, IPCOM_WV_NETDEVENT_START,
                       ipnet_eth_input, IPCOM_WV_IPNET_ETH_MODULE, IPCOM_WV_NETD_ARP);
    IPNET_STATS(eth_input++);

    if (IP_UNLIKELY(IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_UP)))
    {
        IPNET_STATS(eth_input_down++);
        IPCOM_MIB2_NETIF(ifInDiscards++);
        goto free_pkt;
    }

    eth = (Ipnet_pkt_eth *)&pkt->data[pkt->start];

    pCaptureCb  = g_astCallbackTbl[IPS_MNTN_CCORE_INPUT_DATA_CB];

    stIpsMntnCapturePara.paucNetIfName    = (unsigned char *)(netif->ipcom.name);
    stIpsMntnCapturePara.pucData          = &(pkt->data[pkt->start]);
    stIpsMntnCapturePara.usLen            = (unsigned short)(pkt->end - pkt->start);

    if ( IP_NULL != pCaptureCb )
    {
        pCaptureCb(&stIpsMntnCapturePara);
    }

    /* Setup the LLA offsets */
    pkt->lla_off    = ip_offsetof(Ipnet_pkt_eth, src) + pkt->start;
    pkt->lla_size   = sizeof(eth->src);

    frame_type = eth->d.type;

    /*
     * rather than assuming the receiving link header size to be the value that
     * is configured the netif (e.g. netif->ipcom.link_hdr_size), determine the
     * size based on what is being received. This is necessary because it is
     * possible that untagged frame is received even though the underlying port
     * is configured to send tagged frame for a given vlan (or vice versa).
     */
    link_hdr_size  = IPNET_ETH_HDR_SIZE;

#ifdef IPNET_USE_VLAN

#ifdef IPCOM_USE_HW_VLAN_TAGGING
    /*
     * If hardware supports extracting vlan tag from the received frame, the
     * vlan information will be stored in pkt->link_cookie (in host byte order)
     * and the flag IPCOM_PKT_FLAG_VLAN_TAG is set. The ethernet header will
     * be untagged. If IPCOM_PKT_FLAG_VLAN_TAG packet flag is not set, attempt
     * to extract the vlan id from the ethernet header.
     */
    if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_VLAN_TAG))
    {
        vid = IPNET_ETH_PKT_GET_VLAN_TAG(pkt) & 0x0fff;

        /*
         * Explicitly clear the IPCOM_PKT_FLAG_VLAN_TAG and pkt->link_cookie
         * so that the info will not be incorrectly interpreted during
         * forwarding. Outbound interface will set this flag again if necessary.
         */
        IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_VLAN_TAG);
        pkt->link_cookie = 0;
    }
    else
#endif /* IPCOM_USE_HW_VLAN_TAGGING */
        if (frame_type == IPNET_ETHFRAME_8021Q_VLAN)
        {
            vid = IPNET_ETH_VLAN_GET_VID(eth->d.vlan.pri_cif_vid);

            /*
             * featch the real frame_type from the vlan tag regardless of the vid
             * value. That way, priority-tagged frame (vid = 0) can still be
             * handled.
             */
            frame_type = eth->d.vlan.type;

            /* adjust the link header size to include the vlan tag */
            link_hdr_size += IPNET_VLAN_TAG_SIZE;
        }

    if (vid)
    {
#ifdef IPCOM_VXWORKS_USE_MUX_L2
        /*
         * netif still points to the underlying physical interface. Check
         * for matching port-based vlan configuration.
         */
        if (vid == netif->eth->vlan_param.id)
            goto found_if;
#endif /* IPCOM_VXWORKS_USE_MUX_L2 */

        /* Find the correct VLAN interface */
        if (netif->eth->vlan.ifs == IP_NULL)
        {
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                              1, 14, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                              ipnet_eth_input, IPCOM_WV_NETD_INFO_DROPPED,
                              IPCOM_WV_IPNET_ETH_MODULE, IPCOM_WV_NETD_ARP);
            IPNET_STATS(vlan_input_no_match++);
            IPCOM_MIB2_NETIF(ifInDiscards++);
            goto free_pkt;
        }

        netif = ipcom_hash_get(netif->eth->vlan.ifs, &vid);
        if (netif == IP_NULL)
        {
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                              1, 15, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                              ipnet_eth_input, IPCOM_WV_NETD_INFO_DROPPED,
                              IPCOM_WV_IPNET_ETH_MODULE, IPCOM_WV_NETD_ARP);
            IPNET_STATS(vlan_input_no_match++);
            goto free_pkt;
        }

#ifdef IPCOM_VXWORKS_USE_MUX_L2
    found_if:
#endif /* IPCOM_VXWORKS_USE_MUX_L2 */

        if (IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_UP))
        {
            ++netif->iqdrops;
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                              1, 16, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                              ipnet_eth_input, IPCOM_WV_NETD_PKTLOST,
                              IPCOM_WV_IPNET_ETH_MODULE, IPCOM_WV_NETD_ARP);
            IPNET_STATS(vlan_input_down++);
            IPCOM_MIB2_NETIF(ifInDiscards++);
            goto free_pkt;
        }

        pkt->ifindex  = netif->ipcom.ifindex;
        pkt->vr_index = netif->vr_index;
        ++netif->ipackets;
        netif->ibytes += pkt->end - pkt->start;

        /* */
        if (netif->ipcom.link_tap != IP_NULL)
            /* Let the tap handler copy the packet */
            netif->ipcom.link_tap(&netif->ipcom, pkt, IP_PACKET_HOST, frame_type, pkt->start, pkt->start + link_hdr_size);

#ifdef IP_PORT_LKM
        /* VLAN interfaces MAY have different MAC addresses */
        if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_LINK_OTHER))
        {
            if (IP_LIKELY(IPNET_ARE_ETHADDR_EQUAL(eth->dst, netif->ipcom.link_addr)))
                IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_LINK_OTHER);
        }
#endif
    }
#endif /* IPNET_USE_VLAN */

#ifdef IPCOM_USE_PACKET
    /* Give a copy of packet to all matching packet sockets. */
    if (ipnet->num_packet_sock > 0 && ipnet_packet_input(netif, pkt) == IP_FALSE)
        return 0;
#endif

    IPCOM_MIB2_NETIF(ifInOctets += pkt->end - pkt->start);
    pkt->start += link_hdr_size;
    IP_PREPARE_FOR_READ(&pkt->data[pkt->start]);

#ifndef IP_PORT_LKM
    /* The link layer dst address type has already been set in the LKM port */
    if (IP_UNLIKELY(eth->dst[0] & 0x01))
        IP_BIT_SET(pkt->flags,
                   (eth->dst[0] == 0xff
                    ? IPCOM_PKT_FLAG_LINK_BROADCAST
                    : IPCOM_PKT_FLAG_LINK_MULTICAST));
    else if (IP_UNLIKELY(IPNET_ARE_ETHADDR_EQUAL(eth->dst, netif->ipcom.link_addr) == IP_FALSE))
        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_LINK_OTHER);
#endif
    if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_LINK_BROADCAST))
        IPCOM_MIB2_NETIF(ifInBroadcastPkts++);
    else if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_LINK_MULTICAST))
        IPCOM_MIB2_NETIF(ifInMulticastPkts++);
    else
        IPCOM_MIB2_NETIF(ifInUcastPkts++);

    if (IPNET_ARE_ETHADDR_EQUAL(eth->src, netif->ipcom.link_addr))
        /* This packet was sent by this interface */
        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_LOOPED_BACK);

    /* Process frame */
    switch (frame_type)
    {
#ifdef IPCOM_USE_INET
    case IPNET_ETHFRAME_IP:
        {
            int ret;

            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                              1, 17, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                              ipnet_eth_input, IPCOM_WV_NETD_INFO_RECEIVE,
                              IPCOM_WV_IPNET_ETH_MODULE, IPCOM_WV_NETD_ARP);
            IPNET_STATS(eth_input_ip4++);
#ifdef IPNET_USE_VRRP
            IPCOM_PKT_SET_VRID(pkt, ipnet_eth_vrid_from_mac(netif, eth->dst));
#endif
            ret = ipnet_ip4_input(netif, pkt);
#ifdef IPCOM_USE_MIB2
            if (ret == -IP_ERRNO_ENAT || ret == -IP_ERRNO_EFIREWALL)
                IPCOM_MIB2_NETIF(ifInDiscards++);
#endif
            return ret;
        }

    case IPNET_ETHFRAME_ARP:
        if (ipnet_eth_arp_input(netif, pkt, eth, link_hdr_size))
            return 0;
        break;

#ifdef IPNET_USE_RARP
    case IPNET_ETHFRAME_RARP:
        ipnet_eth_rarp_input(netif, pkt);
        break;
#endif

#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    case IPNET_ETHFRAME_IPV6:
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                          1, 18, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                          ipnet_eth_input, IPCOM_WV_NETD_INFO_RECEIVE,
                          IPCOM_WV_IPNET_ETH_MODULE, IPCOM_WV_NETD_ARP);
        IPNET_STATS(eth_input_ip6++);
        return ipnet_ip6_input(netif, pkt);
#endif /* IPCOM_USE_INET6 */

#ifdef IPMPLS
    case IPNET_ETHFRAME_MPLS_UNICAST:
    case IPNET_ETHFRAME_MPLS_MULTICAST:
        return ipnet_mpls_input(netif, pkt);
#endif /* IPMPLS */

#ifdef IPPPP_USE_PPPOE
    case IPNET_ETHFRAME_PPPOE_DIS:
    case IPNET_ETHFRAME_PPPOE_SES:
        if (ipnet->ppp.pppoe_input != IP_NULL)
        {
#ifdef IPNET_USE_VLAN
            if (eth->d.type == IPNET_ETH_P_8021Q_VLAN)
            {
                /* Remove the VLAN tag (note: the order of the memmoves is important!) */
                ipcom_memmove(&eth->src[4], eth->src, IPNET_ETH_ADDR_SIZE); /*lint !e419 */
                ipcom_memmove(&eth->dst[4], eth->dst, IPNET_ETH_ADDR_SIZE); /*lint !e419 */
                eth->d.type = (Ip_u16) frame_type;
            }
#endif /* IPNET_USE_VLAN */
            pkt->start -= link_hdr_size;
            return ipnet->ppp.pppoe_input(&netif->ipcom, pkt);
        }
        /* fall through */
#endif /* IPPPP_USE_PPPOE */

    default:
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                          1, 19, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                          ipnet_eth_input, IPCOM_WV_NETD_INFO_DROPPED,
                          IPCOM_WV_IPNET_ETH_MODULE, IPCOM_WV_NETD_ARP);
        IPNET_STATS(eth_input_other++);
        IPCOM_MIB2_NETIF(ifInUnknownProtos++);
        netif->noproto++;
        break;
    }

 free_pkt:

    ipcom_pkt_free(pkt);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_eth_set_mac_addr
 *===========================================================================
 * Description: Sets the MAC address and regenerates all IPv6 addresses
 *              created by automatic address configuration.
 * Parameters:  netif - The interface on which the multicast address will
 *              be used.
 *              ifreq - Contains the new
 * Returns:     0 = success, < 0 = error code
 *
 */
IP_STATIC int
ipnet_eth_set_mac_addr(Ipnet_netif *netif, struct Ip_ifreq *ifreq)
{
#ifdef IPCOM_USE_INET
    Ipnet_ip4_addr_entry *ae;
#endif
    int retval;

    retval = ipnet_if_drv_ioctl(netif, IP_SIOCSIFLLADDR, ifreq);
    if (retval < 0)
        return retval;

    IPNET_SET_ETHADDR(netif->ipcom.link_addr, ifreq->ip_ifr_addr.sa_data);
#ifdef IPCOM_USE_FORWARDER
    if (netif->ipcom.hw_ix >= 0)
    {
        extern void ipcom_forwarder_port_mac_update(int port_index,
                                                    Ip_u8 *hwaddr);
        ipcom_forwarder_port_mac_update(netif->ipcom.hw_ix,
                                        netif->ipcom.link_addr);
    }
#endif

#ifdef IPCOM_USE_INET
    for (ae = netif->inet4_addr_list;
         ae != IP_NULL;
         ae = ae->next)
    {
        if (ae->type == IPNET_ADDR_TYPE_UNICAST)
            ipnet_arp_request(netif, ae->ipaddr_n, IP_TRUE, IP_NULL, IP_NULL);

    }
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_UP))
    {
        /* Unconfigure/configure cycle will make sure all addresses
           based on the MAC address are regenerated */
        ipnet_ip6_if_unconfigure(netif);
        ipnet_ip6_if_configure(netif);
    }
    /* else: automatic addresses based on MAC is calculated when the
       interface comes up */
#endif /* IPCOM_USE_INET6 */
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_eth_ioctl
 *===========================================================================
 * Description: Ethernet IO control. Support joining and leaving IPv4 and
 *              IPv6 multicast groups.
 * Parameters:  netif - The network interface to operate on.
 *              command - IPNET_IP4_SIOCxxx or IPNET_IP6_SIOCxxx style command.
 *              data - Depends on command.
 * Returns:     0 = success, < 0 = error code
 *
 */
IP_STATIC int
ipnet_eth_ioctl(Ipnet_netif *netif, Ip_u32 command, void *data)
{
    int retval = 0;
    Ip_u8 link_addr[IPNET_ETH_ADDR_SIZE];
    struct Ip_ifreq *ifreq = data;

    ip_assert(netif->ipcom.link_addr_size == IPNET_ETH_ADDR_SIZE);

    switch (command)
    {
    case IP_SIOCXOPEN:
        retval = ipnet_if_drv_ioctl(netif, IP_SIOCXOPEN, IP_NULL);
        if (retval == 0)
            ipnet_kioevent(netif, IP_EIOXUP, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        break;

    case IP_SIOCXCLOSE:
        retval = ipnet_if_drv_ioctl(netif, IP_SIOCXCLOSE, IP_NULL);
        if (retval == 0)
            ipnet_kioevent(netif, IP_EIOXDOWN, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        break;

    case IP_SIOCGIFHWADDR:
        /**/
        ifreq->ip_ifr_addr.sa_family = IPCOM_ARPHRD_TYPE(&netif->ipcom);
        IPNET_SET_ETHADDR(ifreq->ip_ifr_addr.sa_data, netif->ipcom.link_addr);
        break;

    case IP_SIOCGIFLLADDR:
        ifreq->ip_ifr_addr.sa_family = IP_AF_LINK;
        IPCOM_SA_LEN_SET(&ifreq->ip_ifr_addr, IPNET_ETH_ADDR_SIZE);
        IPNET_SET_ETHADDR(ifreq->ip_ifr_addr.sa_data, netif->ipcom.link_addr);
        break;

    case IP_SIOCSIFLLADDR:
        retval = ipnet_eth_set_mac_addr(netif, ifreq);
        break;

#ifdef IPCOM_USE_INET
    case IP_SIOCXADDMULTI_IN:
    case IP_SIOCXDELMULTI_IN:
        {
            struct Ip_in_addr *inaddr = (struct Ip_in_addr *) data;

            if (inaddr->s_addr == IP_INADDR_ANY)
            {
                /* Enable/disable promiscuous mode */
                Ip_bool promisc = (command == IP_SIOCXADDMULTI_IN);
                retval = ipnet_if_drv_ioctl(netif, IP_SIOCXPROMISC, &promisc);
            }
            else
            {
                ipnet_eth_ip4_multicast_to_link_addr(data, link_addr);
                if (command == IP_SIOCXADDMULTI_IN)
                    retval = ipnet_if_drv_ioctl(netif, IP_SIOCXADDMULTI, link_addr);
                else
                    retval = ipnet_if_drv_ioctl(netif, IP_SIOCXDELMULTI, link_addr);
            }
        }
        break;
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    case IP_SIOCXADDMULTI_IN6:
    case IP_SIOCXDELMULTI_IN6:
        {
            struct Ip_in6_addr *addr6 = (struct Ip_in6_addr *) data;

            IPNET_SET_ETHADDR_IP6_MULTICAST(link_addr, addr6);
            if (command == IP_SIOCXADDMULTI_IN6)
                retval = ipnet_if_drv_ioctl(netif, IP_SIOCXADDMULTI, link_addr);
            else
                retval = ipnet_if_drv_ioctl(netif, IP_SIOCXDELMULTI, link_addr);
        }
        break;
#endif /* IPCOM_USE_INET6 */
    case IP_SIOCADDMULTI:
    case IP_SIOCDELMULTI:
        {
            struct Ip_sockaddr *sa = (struct Ip_sockaddr *) data;
            ipcom_memcpy(link_addr, sa->sa_data, sizeof(link_addr));
            if (command == IP_SIOCADDMULTI)
                retval = ipnet_if_drv_ioctl(netif, IP_SIOCXADDMULTI, link_addr);
            else
                retval = ipnet_if_drv_ioctl(netif, IP_SIOCXDELMULTI, link_addr);
        }
        break;

#ifdef IPNET_USE_RARP
    case IP_SIOCXETHSRARP:
        {
            netif->eth->rarp_interval = 1;
            retval = ipnet_timeout_schedule(0,
                                            (Ipnet_timeout_handler) ipnet_eth_rarp_timeout,
                                            netif,
                                            &netif->eth->rarp_tmo);
        }
        break;
#endif /* IPNET_USE_RARP */

    case IP_SIOCXPROMISC:
        {
            /* Enable/disable promiscuous mode */
            Ip_bool promisc = ifreq->ifr_ifru.ifru_opt;
            retval = ipnet_if_drv_ioctl(netif, IP_SIOCXPROMISC, &promisc);
        }
        break;

#ifdef IPCOM_USE_FORWARDER
    case IP_SIOCSIFMTU:
        retval = ipnet_if_drv_ioctl(netif, command, data);
        if (netif->ipcom.hw_ix >= 0 && retval == 0)
        {
            extern void ipcom_forwarder_port_mtu_update(int port, Ip_u32 mtu);
            ipcom_forwarder_port_mtu_update(netif->ipcom.hw_ix,
                                            ifreq->ifr_ifru.ifru_mtu);
        }
        break;
#endif

    default:
        retval = ipnet_if_drv_ioctl(netif, command, data);
        break;
    }

    return retval;
}


#ifdef IPCOM_USE_PACKET
/*
 *===========================================================================
 *                      ipnet_eth_raw_output
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
IP_STATIC int
ipnet_eth_raw_output(Ipnet_netif *netif,
                     IP_CONST struct Ip_sockaddr_ll *to,
                     Ipcom_pkt *pkt)
{
    Ipnet_pkt_eth *eth;

    if (IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_UP))
    {
        ipcom_pkt_free(pkt);
        return -IP_ERRNO_ENETDOWN;
    }

    if (to->sll_halen != IPNET_ETH_ADDR_SIZE)
    {
        IPCOM_LOG2(DEBUG, "ETH: the address length is invalid, is %d, must be %d",
                   to->sll_halen, IPNET_ETH_ADDR_SIZE);
        ipcom_pkt_free(pkt);
        return -IP_ERRNO_EFAULT;
    }

    ip_assert(netif->ipcom.link_hdr_size < pkt->start);
    ip_assert(pkt->start < pkt->end);
    ip_assert(pkt->start >= netif->ipcom.link_hdr_size);

    /* Set Ethernet header. */
    pkt->start -= netif->ipcom.link_hdr_size;
    eth = (Ipnet_pkt_eth *) &pkt->data[pkt->start];
    IPNET_SET_ETHADDR(eth->dst, to->sll_addr);
    IPNET_SET_ETHADDR(eth->src, netif->ipcom.link_addr);
    IPNET_SET_ETHTYPE(netif, eth, to->sll_protocol);

    return ipnet_if_output(netif, pkt);
}


/*
 *===========================================================================
 *                      ipnet_eth_get_frame_info
 *===========================================================================
 * Description: Gets header information from the frame header.
 * Parameters:  [in] netif - A pointer to the network interface structure for this
 *              driver.
 *              [in] pkt - A compleate link layer fram. pkt->start is the offset
 *              to the link layer header.
 *              [out] pkt_type - Will be set to one of the IP_PACKET_xxx type,
 *              can be IP_NULL if this value is not needed.
 *              [out] frame_type - Will be set to the frame type (i.e. one of the
 *              IPNET_ETH_P_xxx constants). can be IP_NULL if this value is not needed.
 *              [out] src - Will be set to the source address of the frame, can
 *              be IP_NULL.
 *              [out] dst - Will be set to the destination address of the frame,
 *              can be IP_NULL.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_eth_get_frame_info(Ipnet_netif *netif,
                         Ipcom_pkt *pkt,
                         Ip_u8 *pkt_type,
                         Ip_u16 *frame_type,
                         Ip_u8 *src,
                         Ip_u8 *dst)
{
    Ipnet_pkt_eth   *eth;
    static Ip_u16 macbrdcast[3] = { 0xffff, 0xffff, 0xffff };

    ip_assert(pkt);

    eth = (Ipnet_pkt_eth *)&pkt->data[pkt->start];

    if (pkt_type != IP_NULL)
    {
        if (IPNET_ARE_ETHADDR_EQUAL(eth->dst, macbrdcast))
            *pkt_type = IP_PACKET_BROADCAST;
        else if (IPNET_ARE_ETHADDR_EQUAL(eth->dst, netif->ipcom.link_addr))
            *pkt_type = IP_PACKET_HOST;
        else if (IPNET_ARE_ETHADDR_EQUAL(eth->src, netif->ipcom.link_addr))
            *pkt_type = IP_PACKET_OUTGOING;
        else if (IP_BIT_ISSET(1, ip_ntohs(eth->dst[0])))
            *pkt_type = IP_PACKET_MULTICAST;
        else
            *pkt_type = IP_PACKET_OTHERHOST;
    }

    if (frame_type != IP_NULL)
    {
#ifdef IPNET_USE_VLAN
        if (eth->d.type == IPNET_ETHFRAME_8021Q_VLAN)
            *frame_type = eth->d.vlan.type;
        else
#endif /* IPNET_USE_VLAN */
            *frame_type = eth->d.type;
    }
    if (src != IP_NULL)
        IPNET_SET_ETHADDR(src, eth->src);
    if (dst != IP_NULL)
        IPNET_SET_ETHADDR(dst, eth->dst);

    return 0;
}

#endif /* IPCOM_USE_PACKET */

/*
 ****************************************************************************
 * 10                    GLOBAL FUNCTIONS
 ****************************************************************************
 */
/*
 *===========================================================================
 *                      ipnet_set_eth_type
 *===========================================================================
 * Description: Set the Ethernet frame type
 * Parameters: netif - The network interface on which the packet should
 *             be sent.
 *             eth - Pointer to the Ethernet frame header
 *             eth_type - The type of data this ethernet frame contains.
 *             pkt - The ethernet frame.
 * Returns:    none.
 *
 */
#ifdef IPNET_USE_VLAN
IP_GLOBAL void
ipnet_set_eth_type(struct Ipnet_netif_struct *netif,
                   Ipnet_pkt_eth *eth,
                   Ip_u16 eth_type,
                   Ipcom_pkt *pkt)
{
    Ip_u16 tag;

    if (netif->eth->vlan_param.tag_required == IP_FALSE)
    {
        eth->d.type = eth_type;
        return;
    }

    tag = ((netif->eth->vlan_param.id & 0x0fff)
           | ((netif->eth->vlan_param.pri << 13) & 0xe000));

#ifdef IPCOM_USE_HW_VLAN_TAGGING
    if (IP_BIT_ISSET(netif->ipcom.drv_capabilities,
                     IPCOM_IF_DRV_CAP_VLAN_HWTAGGING_TX))
    {
        /*
         * hardware shall take care inserting the vlan tag to the
         * ethernet header. Simply create an untagged ethernet header
         */
        IPNET_ETH_PKT_SET_VLAN_TAG(pkt, tag);
        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_VLAN_TAG);
        eth->d.type = eth_type;
        return;
    }
    IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_VLAN_TAG);
#else
    IPCOM_UNUSED_ARG(pkt);
#endif /* IPCOM_USE_HW_VLAN_TAGGING */

    eth->d.vlan.tag_type    = IPNET_ETHFRAME_8021Q_VLAN;
    eth->d.vlan.pri_cif_vid = ip_htons(tag);
    eth->d.vlan.type        = eth_type;
}
#endif /* IPNET_USE_VLAN */


/*
 *===========================================================================
 *                    ipnet_eth_update_mib2_lastchange
 *===========================================================================
 * Description: Update ARP/NDP entry last change member
 * Parameters:  rt - pointer to the route entry.
 * Returns:     Nothing.
 */
#ifdef IPCOM_USE_MIB2_NEW
IP_GLOBAL void
ipnet_eth_update_mib2_lastchange(Ipnet_route_entry *rt)
{
    Ipcom_mib2_TimeStamp timestamp;

    if (ipnet->mib2.sysUpTimeFp != IP_NULL)
    {
        timestamp = ipnet->mib2.sysUpTimeFp();
    }
    else
    {
        timestamp = (Ipcom_mib2_TimeStamp)ipnet_msec_now();
        timestamp /= 10; /* Convert to hundreds of seconds */
    }
    rt->lastchange = (Ip_u32)timestamp;
}
#endif /* #ifdef IPCOM_USE_MIB2_NEW */


/*
 *===========================================================================
 *                    ipnet_eth_if_init
 *===========================================================================
 * Description: Initializes the Ethernet specific part of an interface.
 * Parameters:  netif - The generic interface structure.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_PUBLIC int
ipnet_eth_if_init(Ipnet_netif *netif)
{
    int i;

    netif->eth = ipcom_calloc(1, sizeof(Ipnet_netif_eth));
    if (netif->eth == IP_NULL)
        return -IP_ERRNO_ENOMEM;
    i = 0;
    do
    {
        ipcom_sprintf(netif->ipcom.name, "eth%d", i++);
    } while (ipnet_if_nametonetif(netif->vr_index, netif->ipcom.name) != IP_NULL);

    netif->ipcom.flags          = IP_IFF_BROADCAST | IP_IFF_MULTICAST;
    netif->ipcom.mtu            = IPNET_ETH_MTU;
    netif->ipcom.link_hdr_size  = IPNET_ETH_HDR_SIZE;
    netif->ipcom.link_addr_size = IPNET_ETH_ADDR_SIZE;

    /* Setup link functions for Ethernet use. */
    netif->link_input           = ipnet_eth_input;
    netif->link_ioctl           = ipnet_eth_ioctl;
    netif->ipcom.link_ioevent   = ipnet_eth_ioevent;

#ifdef IPCOM_USE_INET
    netif->link_ip4_output      = ipnet_eth_ip4_output;
    netif->inet4_neigh_init     = ipnet_arp_init;
#endif

#ifdef IPCOM_USE_INET6
    netif->inet6_if_id_bits     = IPNET_ETH_IP6_ID_BITS;
    netif->link_ip6_output      = ipnet_eth_ip6_output;
    netif->link_ip6_create_addr = ipnet_eth_ip6_create_addr;
#endif /* IPCOM_USE_INET6 */

#ifdef IPCOM_USE_PACKET
    netif->link_raw_output      = ipnet_eth_raw_output;
    netif->link_get_frame_info  = ipnet_eth_get_frame_info;
#endif /* IPCOM_USE_PACKET */

    return 0;
}


#ifdef IPCOM_USE_INET
/*
 *===========================================================================
 *                    ipnet_arp_set_state
 *===========================================================================
 * Description: Sets the state of an ARP entry.
 * Parameters:  rt - The route containing the ARP entry to set state for.
 *              state - The new state.
 *              link_addr - new link address or IP_NULL to keep current
 *                          link address.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_arp_set_state(Ipnet_route_entry *rt,
                    Ipnet_nd_state_t state,
                    IP_CONST void *link_addr)
{
    Ipnet_neigh_entry *ne = IPNET_NEIGH_FROM_RT(rt);

#ifdef IPNET_USE_VRRP
    if (state == IPNET_ND_PROXY
        && IPNET_ETH_IS_VRRP_ADDR(IP_SOCKADDR_DL_LLADDR(rt->gateway)))
    {
        Ip_u8 *mac = (Ip_u8 *) IP_SOCKADDR_DL_LLADDR(rt->gateway);

        IP_BIT_SET(rt->netif->eth->vrids[mac[5] >> 5], 1 << (mac[5] & 0x1f));
        rt->timedout_cb = ipnet_vrrp_timeout;
        (void)ipnet_vrrp_add_addr(rt->netif,
                                  mac[5],
                                  *(struct Ip_in_addr*) rt->hdr.key);
    }
#endif /* IPNET_USE_VRRP */

    ipnet_neigh_set_state(IP_AF_INET, ne, state, link_addr);

    if (IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_X_DONT_ROUTE)
        && (ne->state == IPNET_ND_REACHABLE
            || ne->state == IPNET_ND_PROXY))
    {
        Ipnet_route_entry *cur_rt = rt;
        Ipnet_route_entry *next_rt;

        while (cur_rt->prev != IP_NULL)
            cur_rt = cur_rt->prev;

        do {
            Ipnet_arp_entry *cur_arp_entry = IPNET_ARP(cur_rt);

            next_rt = cur_rt->next;
            if (cur_rt != rt
                && cur_rt->rt_func_info == IP_NULL
                && cur_arp_entry != IP_NULL)
            {
                /* Make sure that no ICMP unreachable is sent */
                cur_arp_entry->ne.pending.dropcb = IP_NULL;
                (void) ipnet_route_delete2(IP_AF_INET,
                                           IPNET_ROUTE_GET_VR(cur_rt->head),
                                           IPNET_ROUTE_GET_TABLE(cur_rt->head),
                                           cur_rt->hdr.key,
                                           IP_NULL,
                                           IP_NULL,
                                           cur_rt->netif->ipcom.ifindex,
                                           0,
                                           0,
                                           IP_FALSE);
            }
        } while (IP_NULL != (cur_rt = next_rt));
    }
}


/*
 *===========================================================================
 *                    ipnet_arp_request
 *===========================================================================
 * Description: Send an ARP request.
 * Parameters:  netif - The ethernet interface to send to ARP request from.
 *              dstaddr_n - IP address for which we want to learn the MAC address.
 *              gratuitous - 0 = normal ARP, >0 = gratuitous, <0 = RFC3927 probe.
 *              src_mac - IP_NULL if the MAC address of the outgoing interface
 *                        should be used, the MAC address for the host
 *                        this host is acting as proxy for otherwise.
 *              dst_mac - IP_NULL if the MAC address should be the broadcast
 *                        address.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL void
ipnet_arp_request(Ipnet_netif *netif,
                  Ip_u32 dstaddr_n,
                  int gratuitous,
                  Ip_u8 *src_mac,
                  Ip_u8 *dst_mac)
{
    Ipcom_pkt         *pkt;
    Ipnet_pkt_eth     *eth;
    Ipnet_pkt_arp     *arp;
    Ipnet_route_entry *rt;
    IPS_MNTN_CCORE_CAPTURE_CB            pCaptureCb;
    IPS_MNTN_CCORE_CAPTURE_PARA_STRU     stIpsMntnCapturePara;

    pkt = ipcom_pkt_malloc(sizeof(Ipnet_pkt_arp),
                           IP_FLAG_FC_STACKCONTEXT);
    if (pkt == IP_NULL)
    {
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_CRITICAL,
                          1, 20, IPCOM_WV_NETDEVENT_CRITICAL, IPCOM_WV_NETD_SEND,
                          ipnet_arp_request, IPCOM_WV_NETD_NOMEM,
                          IPCOM_WV_IPNET_ETH_MODULE, IPCOM_WV_NETD_ARP);
        IPNET_STATS(eth_output_arp_req_nomem++);
        return;
    }

    if (gratuitous)
        /* Not critical that these got sent and gratuitous ARP is sent from
           contexts that do not expect the IPNET lock to be unlocked */
        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_NONBLOCKING);

    /* Packet header. */
    pkt->ifindex = netif->ipcom.ifindex;
    pkt->start   = ipcom_conf_max_link_hdr_size - netif->ipcom.link_hdr_size;
    pkt->end     = pkt->start + sizeof(Ipnet_pkt_arp) + netif->ipcom.link_hdr_size;

#if defined(IP_PORT_LKM) || (defined(IP_PORT_VXWORKS) && IP_PORT_VXWORKS >= 67)
    pkt->ipstart = pkt->start + netif->ipcom.link_hdr_size;
#endif

    /* Ethernet header. */
    eth = (Ipnet_pkt_eth *) &pkt->data[pkt->start];

    /* ARP header */
    arp = (Ipnet_pkt_arp *) &pkt->data[pkt->start + netif->ipcom.link_hdr_size];

    if (src_mac == IP_NULL)
        src_mac = netif->ipcom.link_addr;

    if (dst_mac != IP_NULL)
        IPNET_SET_ETHADDR(eth->dst, dst_mac);
    else
        IPNET_SET_ETHADDR_BROADCAST(eth->dst);
    IPNET_SET_ETHADDR(eth->src, src_mac);
    IPNET_SET_ETHTYPE(netif, eth, IPNET_ETHFRAME_ARP);

    /* ARP header. */
    arp->hrd     = IPNET_ARP_HRD_ETHER;
    arp->pro     = IPNET_ARP_PRO_IP;
    arp->hln_pln = IPNET_ARP_HLN_PLN;
    arp->op      = IPNET_ARP_OP_REQUEST;
    IPNET_CLR_ETHADDR(arp->eth_dst);
    IPNET_SET_ETHADDR(arp->eth_src, src_mac);
    IPNET_IP4_SET_IPADDR(arp->ip_dst, dstaddr_n);
    if (gratuitous > 0)
        IPNET_IP4_SET_IPADDR(arp->ip_src, dstaddr_n);
    else if (gratuitous < 0)
        IPNET_IP4_SET_IPADDR(arp->ip_src, IP_INADDR_ANY);
    else
    {
        IP_CONST struct Ip_in_addr *insrc;
        insrc = ipnet_ip4_get_src_addr(netif->vr_index,
                                       (IP_CONST struct Ip_in_addr *)arp->ip_dst,
                                       IP_NULL,
                                       netif);
        if (insrc != IP_NULL)
            IPNET_IP4_SET_IPADDR(arp->ip_src, insrc->s_addr);
        else
        {
            IP_PANIC2();
            IPNET_IP4_SET_IPADDR(arp->ip_src, 0);
        }
    }

    if (ipnet_route_raw_lookup2(IP_AF_INET,
                                netif->vr_index,
                                IPCOM_ROUTE_TABLE_DEFAULT,
                                IPNET_RTL_FLAG_DONTCLONE | IPNET_RTL_FLAG_LINK_LOCAL,
                                arp->ip_src,
                                0,
                                IP_NULL,
                                IP_NULL,
                                netif->ipcom.ifindex,
                                &rt) == IPNET_ROUTE_PERFECT_MATCH)
    {
        if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_PROTO2))
            /* We have a proxy ARP entry for our source address,
               use that MAC address in the ARP message */
            IPNET_SET_ETHADDR(arp->eth_src, IP_SOCKADDR_DL_LLADDR(rt->gateway));
    }

    /* Output ARP request. */
    IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                      1, 21, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_SEND,
                      ipnet_arp_request, IPCOM_WV_NETD_INFO_SEND,
                      IPCOM_WV_IPNET_ETH_MODULE, IPCOM_WV_NETD_ARP);
    IPNET_STATS(eth_output_arp_req++);

    pCaptureCb  = g_astCallbackTbl[IPS_MNTN_CCORE_SEND_ARP_CB];

    stIpsMntnCapturePara.paucNetIfName    = (unsigned char *)(netif->ipcom.name);
    stIpsMntnCapturePara.pucData          = &(pkt->data[pkt->start]);
    stIpsMntnCapturePara.usLen            = (unsigned short)(pkt->end - pkt->start);

    if ( IP_NULL != pCaptureCb )
    {
        pCaptureCb(&stIpsMntnCapturePara);
    }

    (void)ipnet_if_output(netif, pkt);
}


/*
 *===========================================================================
 *                    ipnet_arp_ioctl
 *===========================================================================
 * Description: Create a multicast ethernet address corresponding to the
 *              to the IPv6 multicast address.
 * Parameters:  request - The ARP IO ctl request.
 *              req - The request parameters.
 * Returns:     0 = success, < 0 = error code
 *
 */
IP_GLOBAL int
ipnet_arp_ioctl(Ipnet_socket *sock, unsigned long request, struct Ip_arpreq *req)
{
    struct Ip_sockaddr_in *addr_in = (struct Ip_sockaddr_in *)&req->arp_pa;
    struct Ip_arpreqn     *reqn = (struct Ip_arpreqn*)req;
    Ipnet_route_entry     *rt = IP_NULL;
    Ipnet_netif           *netif = IP_NULL;
    int                    rt_flags = IPNET_RTL_FLAG_LINK_LOCAL;
    Ip_u8                 *lladdr;
    Ipnet_arp_entry       *arp_entry;

    ip_assert(req != IP_NULL);

    /* Socket is used to get VR */
    if (sock == IP_NULL)
        return -IP_ERRNO_EBADF;

    if (addr_in->sin_family != IP_AF_INET)
    {
        IPCOM_LOG1(DEBUG, "ETH: invalid address domain for ARP IO control (%d)",
                   addr_in->sin_family);
        return -IP_ERRNO_EINVAL;
    }

#ifdef IPCOM_USE_SA_LEN
    if (req->arp_pa.sa_len == 0)
        return -IP_ERRNO_EADDRNOTAVAIL;
#endif

    if (req->arp_ha.sdl_index != 0)
        netif = ipnet_if_indextonetif(sock->vr_index, req->arp_ha.sdl_index);

    if (netif == IP_NULL
        && (request == IP_SIOCSARPN
            || request == IP_SIOCDARPN
            || request == IP_SIOCGARPN))
        netif = ipnet_if_nametonetif(sock->vr_index, reqn->arp_dev);

    if (request == IP_SIOCPARP)
        /* Force ARP request to the node at address 'addr_in' */
        return ipnet_arp_try_resolve(sock->vr_index,
                                     &addr_in->sin_addr,
                                     netif);

    /* Is there an entry for this address already? */
    if (request != IP_SIOCSARP && request != IP_SIOCSARPN)
        rt_flags |= IPNET_RTL_FLAG_DONTCLONE;
    else if (netif != IP_NULL)
    {
        int ret;
        ret = ipnet_route_create_llinfo_entry(IP_AF_INET,
                                              sock->vr_index,
                                              &addr_in->sin_addr,
                                              IP_NULL,
                                              netif);
        if (ret < 0)
            return ret;
    }

    if (ipnet_route_raw_lookup(IP_AF_INET,
                               sock->vr_index,
                               IPCOM_ROUTE_TABLE_DEFAULT,
                               rt_flags,
                               &addr_in->sin_addr,
                               0,
                               netif == IP_NULL ? 0 : netif->ipcom.ifindex,
                               &rt) != IPNET_ROUTE_PERFECT_MATCH)
        /* Not found (The protocol address is not valid) */
        return -IP_ERRNO_EINVAL;

    netif = rt->netif;

    switch (request)
    {
    case IP_SIOCSARP:
    case IP_SIOCSARPN:
        /* Change/Add ARP entry */
        /* Verify interface. */
        if (netif == IP_NULL
            || (netif->ipcom.type != IP_IFT_ETHER && netif->ipcom.type != IP_IFT_L2VLAN && netif->ipcom.type != IP_IFT_IEEE80211))
            return -IP_ERRNO_ENXIO;

        rt->netif = netif;

        if (request == IP_SIOCSARP)
        {
            ipcom_memcpy(rt->gateway, &req->arp_ha, IPCOM_SA_LEN_GET(&req->arp_ha));
            lladdr = (Ip_u8 *) IP_SOCKADDR_DL_LLADDR(&req->arp_ha);
        }
        else
        {
            /* IP_SIOCSARPN */
            struct Ip_sockaddr_dl *entry = (struct Ip_sockaddr_dl*)&rt->gateway;

            entry->sdl_family  = IP_AF_LINK;
            entry->sdl_type    = IP_IFT_ETHER;
            entry->sdl_index = (Ip_u16)netif->ipcom.ifindex;
            entry->sdl_alen    = IPNET_ETH_ADDR_SIZE;
            entry->sdl_nlen    = 0;
            entry->sdl_slen    = 0;

            lladdr = (Ip_u8*)&reqn->arp_ha.sa_data;
            ipcom_memcpy(IP_SOCKADDR_DL_LLADDR(entry), lladdr, IPNET_ETH_ADDR_SIZE);
        }

#ifdef IPNET_COMPENSATE_UNRELIABLE_NETLINK
        /* Set acknowledge bit */
        IP_BIT_SET(rt->hdr.flags, IPNET_RTF_X_NEIGH_ACK);
#endif
        IP_BIT_SET(rt->hdr.flags, IPNET_RTF_DONE | IPNET_RTF_STATIC);

        if (IP_BIT_ISSET(req->arp_flags, IP_ATF_BLACKHOLE))
            IP_BIT_SET(rt->hdr.flags, IPNET_RTF_BLACKHOLE);

        if (IP_BIT_ISSET(req->arp_flags, IP_ATF_PERM))
            ipnet_route_set_lifetime(rt, IPCOM_ADDR_INFINITE);

        if (IP_BIT_ISFALSE(req->arp_flags, IP_ATF_PUBL))
            ipnet_arp_set_state(rt, IPNET_ND_REACHABLE, lladdr);
        else
        {
            IP_BIT_SET(rt->hdr.flags, IPNET_RTF_PROTO2);
            ipnet_arp_set_state(rt, IPNET_ND_PROXY, lladdr);
        }

        if (IP_BIT_ISSET(req->arp_flags, IP_ATF_GRAT))
            ipnet_arp_request(netif,
                              addr_in->sin_addr.s_addr,
                              IP_TRUE,
                              (Ip_u8 *) IP_SOCKADDR_DL_LLADDR(&req->arp_ha),
                              IP_NULL);
        return 0;

    case IP_SIOCGARP:
    case IP_SIOCGARPN:
        if (netif == IP_NULL
            || netif->ipcom.link_addr_size != IPNET_ETH_ADDR_SIZE
            || rt->gateway == IP_NULL
            || IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_LLINFO))
            return -IP_ERRNO_ESRCH;

        /* Get ARP entry */
        arp_entry = IPNET_ARP(rt);
        if (arp_entry == IP_NULL
            || arp_entry->ne.state == IPNET_ND_UNINITIALIZED
            || arp_entry->ne.state == IPNET_ND_INCOMPLETE)
            return -IP_ERRNO_ESRCH;

        ipcom_memset(req, 0, sizeof(struct Ip_arpreq)); /*lint !e668 Possibly passing a null pointer */

        /* Copy IP address */
        IPCOM_SA_LEN_SET(addr_in, sizeof(struct Ip_sockaddr_in));
        addr_in->sin_family = IP_AF_INET;
        ipcom_memcpy(&addr_in->sin_addr, rt->hdr.key, sizeof(struct Ip_in_addr));

        /* Copy link address */
        if (request == IP_SIOCGARP)
            ipcom_memcpy(&req->arp_ha, rt->gateway, IPCOM_SA_LEN_GET(rt->gateway));
        else
        {
            /* IP_SIOCGARPN */
            IPCOM_SA_LEN_SET(addr_in, sizeof(struct Ip_sockaddr));
            reqn->arp_ha.sa_family = netif->ipcom.type;
            ipcom_memcpy(&reqn->arp_ha.sa_data,
                         IP_SOCKADDR_DL_LLADDR(rt->gateway),
                         IPNET_ETH_ADDR_SIZE);

            ipcom_strcpy(reqn->arp_dev, netif->ipcom.name);
        }

        if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_DONE))
            IP_BIT_SET(req->arp_flags, IP_ATF_COM);

        if (rt->metrics.rmx_expire == IPCOM_ADDR_INFINITE)
            IP_BIT_SET(req->arp_flags, IP_ATF_PERM);

        if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_PROTO2))
            IP_BIT_SET(req->arp_flags, IP_ATF_PUBL);

        if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_BLACKHOLE))
            IP_BIT_SET(req->arp_flags, IP_ATF_BLACKHOLE);

        return 0;

    case IP_SIOCDARP:
    case IP_SIOCDARPN:
        /* Delete ARP entry */
        return ipnet_route_delete2(IP_AF_INET,
                                   sock->vr_index,
                                   IPNET_ROUTE_GET_TABLE(rt->head),
                                   rt->hdr.key,
                                   IP_NULL,
                                   rt->gateway,
                                   netif->ipcom.ifindex,
                                   0,
                                   0,
                                   IP_TRUE);

    default:
        break;
    }
    return -IP_ERRNO_EINVAL;
}
#endif /* IPCOM_USE_INET */

/*
 *===========================================================================
 *                    ipnet_eth_add_hdr
 *===========================================================================
 * Description:
 *
 * Parameters:
 *
 * Returns:     0 = success, < 0 = error code
 *
 */
IP_GLOBAL int
ipnet_eth_add_hdr(Ipnet_netif *netif, Ipcom_pkt *pkt,
                       struct Ip_sockaddr *addr, Ip_u16 type)
{
    Ipnet_pkt_eth  *eth;

    pkt->start -= netif->ipcom.link_hdr_size;
    if (pkt->start < 0)
    {
        /* Can not fit the ethernet header */
        /*IPNET_STATS(eth_output_mpls_err++);*/
        ipcom_pkt_free(pkt);
        return -IP_ERRNO_EFAULT;
    }

    eth = (Ipnet_pkt_eth *) &pkt->data[pkt->start];

    IPNET_SET_ETHADDR(eth->src, netif->ipcom.link_addr);
    IPNET_SET_ETHADDR(eth->dst, IP_SOCKADDR_DL_LLADDR(addr));
    IPNET_SET_ETHTYPE(netif, eth, type);

    return 0;
}


#endif /* IPCOM_USE_ETHERNET */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
