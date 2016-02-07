
/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_rtnetlink_ip6.c,v $ $Revision: 1.17.18.2.2.3 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_rtnetlink_ip6.c,v $
 *   $Author: jhorteli $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Joel Hortelius <joel.hortelis@interpeak.se>
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
#if defined(IPCOM_USE_INET6) && defined(IPNET_USE_NETLINK)

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_clib.h>
#include <ipcom_sysvar.h>
#include <ipcom_inet.h>
#include <ipcom_syslog.h>
#include <ipcom_sock2.h>

#include "ipnet_h.h"
#include "ipnet.h"
#include "ipnet_loopback.h"
#include "ipnet_icmp6.h"

#include "ipnet_eth.h"

#include "ipnet_netlink_h.h"
#include "ipnet_rtnetlink_h.h"

#ifdef IP_PORT_LKM
#include "ipcom_lkm.h"
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

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

IP_GLOBAL int
ipnet_rtnetlink_neigh_newsend_internal(Ipnet_socket         *sock,
                                        struct Ip_nlmsghdr   *nlmsg,
                                        Ip_bool              non_blocking,
                                        struct Ip_rtattr     **rta);

IP_GLOBAL int
ipnet_rtnetlink_neigh_send_sign_req_internal(Ipnet_socket         *sock,
                                             struct Ip_nlmsghdr   *nlmsg,
                                             Ip_bool              non_blocking,
                                             struct Ip_rtattr     **rta);

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

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
IP_STATIC int
ipnet_rtnetlink_ip6_addr_fillinfo(Ipnet_netlink_mem_t     *mem,
                                  Ipnet_ip6_addr_entry    *ifa,
                                  struct Ip_in6_addr      *peer,
                                  Ipnet_netif             *netif,
                                  int                     seq,
                                  Ip_u8                   event,
                                  Ip_u32                  eflags)
{
    struct Ip_ifa_cacheinfo ci;
    struct Ip_ifaddrmsg    *ifm;
    struct Ip_nlmsghdr     *nlh;
    Ip_u32                 tmp32;


    nlh = ipnet_netlink_nlmsg_sz_begin(mem, seq, event, eflags);
    if (!nlh)
        return -1;

    ifm = ipnet_netlink_sz_put(mem, IP_NULL, sizeof (*ifm));
    if (!ifm)
        return -1;

    ifm->ifa_family    = IP_AF_INET6;
    ifm->ifa_prefixlen = (Ip_u8)ifa->prefixlen;
    ifm->ifa_flags     = ipnet_rtnetlink_ip6_get_addr_flags(ifa);
    ifm->ifa_index     = netif->ipcom.ifindex;

    if (peer && !IPNET_IP6_IS_ADDR_UNSPECIFIED(peer))
    {
        /* Peer-to-peer address */
        ifm->ifa_scope     = IP_RT_SCOPE_UNIVERSE;
        ifm->ifa_prefixlen = 128;

        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_IFA_ADDRESS, sizeof(struct Ip_in6_addr), peer))
            return -1;
    }
    else
    {
        /* Normal address */
        ifm->ifa_prefixlen = (Ip_u8)ifa->prefixlen;

        if (IP_IN6_IS_ADDR_LOOPBACK(&ifa->addr))
            ifm->ifa_scope = IP_RT_SCOPE_HOST;
        else if (IP_IN6_IS_ADDR_SITE_LOCAL(&ifa->addr))
            ifm->ifa_scope = IP_RT_SCOPE_SITE;
        else if (IP_IN6_IS_ADDR_LINK_LOCAL(&ifa->addr))
            ifm->ifa_scope = IP_RT_SCOPE_LINK;
        else
            ifm->ifa_scope = IP_RT_SCOPE_UNIVERSE;
    }

    switch (ifa->type)
    {
    case IPNET_ADDR_TYPE_UNICAST:
        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_IFA_LOCAL, sizeof(struct Ip_in6_addr), &ifa->addr))
            return -1;
        break;
    case IPNET_ADDR_TYPE_ANYCAST:
        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_IFA_ANYCAST, sizeof(struct Ip_in6_addr), &ifa->addr))
            return -1;
        break;
    case IPNET_ADDR_TYPE_MULTICAST:
        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_IFA_MULTICAST, sizeof(struct Ip_in6_addr), &ifa->addr))
            return -1;
        break;
    default:
        /* Ignore all other type */
        break;
    }

    if (ifa->preferred_sec < IPCOM_ADDR_INFINITE)
        ci.ifa_prefered = ifa->preferred_sec - IPNET_SECONDS_ABS;
    else
        ci.ifa_prefered = IPCOM_ADDR_INFINITE;

    if (ifa->expires_sec < IPCOM_ADDR_INFINITE)
        ci.ifa_valid = ifa->expires_sec - IPNET_SECONDS_ABS;
    else
        ci.ifa_valid = IPCOM_ADDR_INFINITE;

    if (!ipnet_rtnetlink_rta_sz_put(mem, IP_IFA_CACHEINFO, sizeof(ci), &ci))
        return -1;

    tmp32 = netif->vr_index;
    if (!ipnet_rtnetlink_rta_sz_put(mem, IP_IFA_VR, sizeof(tmp32), &tmp32))
        return -1;

    if (!ipnet_rtnetlink_rta_sz_put(mem, IP_IFA_LABEL, IP_IFNAMSIZ, netif->ipcom.name))
        return -1;

    /**/
    return ipnet_netlink_nlmsg_sz_end(mem, nlh);
}

/*
 *===========================================================================
 *                    ipnet_rtnetlink_ip6_addr_event
 *===========================================================================
 * Description: Status of a network interface has changed, create a IFINFO
 *              event on NETLNK..
 * Parameters:  netif - The interface.
 * Returns:     -
 *
 */
IP_STATIC void
ipnet_rtnetlink_ip6_addr_event(Ipnet_netif *netif, struct Ipnet_ip6_addr_entry_struct *addr, int event)
{
    Ipcom_pkt               *pkt    = IP_NULL;
    Ipnet_netlink_mem_t     mem;
    int                     ret;
    Ipnet_ppp_peer         *p       = netif->private_data;

    if (!ipnet_netlink_pkt_alloc(&pkt, IP_NULL, IP_TRUE, &mem, 512))
        return;

    /* Must explicitly switch to the correct VR here */
    pkt->vr_index = mem.vr = netif->vr_index;

    ret = ipnet_rtnetlink_ip6_addr_fillinfo(&mem,
                                            addr,
                                            p && addr == netif->inet6_addr_list ? &p->peer6 : IP_NULL,
                                            netif,
                                            0,
                                            event,
                                            0);
    if (ret < 0)
        ipcom_pkt_free(pkt);
    else
    {
        pkt->end += ret;

        /* We're done; do report this pkt */
        ipnet_rtnetlink_broadcast(pkt, IP_RTNLGRP_IPV6_IFADDR);
    }
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                      ipnet_rtnetlink_ip6_get_addr_x_flags
 *===========================================================================
 * Description: Fetches additional flag information about an IPv6 address
 *              entry.
 * Parameters:  addr - Pointer to IPv6 address.
 * Returns:     address flags
 *
 */
IP_GLOBAL Ip_u32
ipnet_rtnetlink_ip6_get_addr_x_flags(Ipnet_ip6_addr_entry *addr)
{
    Ip_u8                  out_flags = 0;
    struct Ipnet_ipv6_key  key;
    struct Ipnet_ipv6_key  mask;
    Ipnet_route_entry      *rt;

    if (IP_BIT_ISSET(addr->flags,IPNET_IP6_ADDR_FLAG_AUTONOMOUS))
        IP_BIT_SET(out_flags, IP_IFA_F_X_AUTONOM);

    if (!IP_BIT_ISSET(addr->flags, IPNET_IP6_ADDR_FLAG_DEPRECATED) &&
        !IP_BIT_ISSET(addr->flags, IPNET_IP6_ADDR_FLAG_TENTATIVE))
        IP_BIT_SET(out_flags, IP_IFA_F_X_PREFERRED);

    if (IP_BIT_ISSET(addr->flags, IPNET_IP6_ADDR_FLAG_AUTOMATIC))
        IP_BIT_SET(out_flags, IP_IFA_F_X_AUTOMATIC);

    if (!addr->netif)
        goto out;

    if (IP_BIT_ISFALSE(addr->netif->ipcom.flags, IP_IFF_UP))
        IP_BIT_SET(out_flags, IP_IFA_F_X_INACCESSIBLE);

    ipcom_memset(&mask, 0 , sizeof(mask));
    ipcom_memcpy(&key.addr, &addr->addr, sizeof(addr->addr));

    ipnet_route_create_mask(&mask.addr, addr->prefixlen);
    ipnet_route_apply_mask(&key, &mask, sizeof(key) * 8);

    if (ipnet_route_lookup(IP_AF_INET6,
                           addr->netif->vr_index,
                           IPCOM_ROUTE_TABLE_DEFAULT,
                           IPNET_RTL_FLAG_DONTCLONE,
                           &key.addr,
                           addr->netif->ipcom.ifindex,
                           0,
                           &rt) != IPNET_ROUTE_PERFECT_MATCH)
        goto out;

    if (IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_GATEWAY))
        IP_BIT_SET(out_flags, IP_IFA_F_X_ONLINK);

 out:
    return out_flags;
}

/*
 *===========================================================================
 *                      ipnet_rtnetlink_ip6_get_addr_flags
 *===========================================================================
 * Description: Converts IPNET internal IPv6 address flags to netlink
 *              equivalents.
 * Parameters:  addr - Pointer to IPv6 address.
 * Returns:     address flags
 *
 */
IP_GLOBAL Ip_u8
ipnet_rtnetlink_ip6_get_addr_flags(Ipnet_ip6_addr_entry *addr)
{
    Ip_u8 out_flags = 0;

    if (IP_BIT_ISSET(addr->flags, IPNET_IP6_ADDR_FLAG_DEPRECATED))
        IP_BIT_SET(out_flags, IP_IFA_F_DEPRECATED);
    else if (IP_BIT_ISSET(addr->flags, IPNET_IP6_ADDR_FLAG_TENTATIVE))
        IP_BIT_SET(out_flags, IP_IFA_F_TENTATIVE);

    if (addr->expires_sec == IPCOM_ADDR_INFINITE)
        IP_BIT_SET(out_flags, IP_IFA_F_PERMANENT);

    if (IP_BIT_ISSET(addr->flags, IPNET_IP6_ADDR_FLAG_HOMEADDRESS))
        IP_BIT_SET(out_flags, IP_IFA_F_HOMEADDRESS);

#ifdef IPNET_USE_RFC3971
    if (IP_BIT_ISSET(addr->flags, IPNET_IP6_ADDR_FLAG_CGA))
        IP_BIT_SET(out_flags, IP_IFA_F_CGA);
#endif

    return out_flags;
}


/*
 *===========================================================================
 *                    ipnet_rtnetlink_ip6_scope_to_rtscope
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_u32
ipnet_rtnetlink_ip6_scope_to_rtscope(Ip_u32 scope)
{
    switch (scope)
    {
    default:
    case IPNET_IP6_SCOPE_GLOBAL:
        return IP_RT_SCOPE_UNIVERSE;
    case IPNET_IP6_SCOPE_SITE_LOCAL:
        return IP_RT_SCOPE_SITE;
    case IPNET_IP6_SCOPE_LINK_LOCAL:
        return IP_RT_SCOPE_LINK;
    case IPNET_IP6_SCOPE_NODE_LOCAL:
        return IP_RT_SCOPE_HOST;
    }
}

/*
 *===========================================================================
 *                    ipnet_rtnetlink_ip6_scope_to_rtscope
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_u32
ipnet_rtnetlink_ip6_rtscope_to_scope(Ip_u32 scope)
{
    switch (scope)
    {
    case IP_RT_SCOPE_UNIVERSE:
        return IPNET_IP6_SCOPE_GLOBAL;
    case IP_RT_SCOPE_SITE:
        return IPNET_IP6_SCOPE_SITE_LOCAL;
    case IP_RT_SCOPE_LINK:
        return IPNET_IP6_SCOPE_LINK_LOCAL;
    case IP_RT_SCOPE_HOST:
        return IPNET_IP6_SCOPE_NODE_LOCAL;
    default:
        return IPNET_IP6_SCOPE_GLOBAL;
    }
}


IP_GLOBAL void
ipnet_rtnetlink_ip6_init(void)
{
    ipnet_rtnetlink_register_family_event(IP_AF_INET6, IP_RTM_GETADDR,
                                          IP_IFA_MAX, sizeof(struct Ip_ifaddrmsg),
                                          IP_NULL,
                                          ipnet_rtnetlink_addr_dump_family);
    ipnet_rtnetlink_register_family_event(IP_AF_INET6, IP_RTM_GETMULTICAST,
                                          IP_IFA_MAX, sizeof(struct Ip_ifaddrmsg),
                                          IP_NULL,
                                          ipnet_rtnetlink_addr_dump_family);
    ipnet_rtnetlink_register_family_event(IP_AF_INET6, IP_RTM_GETANYCAST,
                                          IP_IFA_MAX, sizeof(struct Ip_ifaddrmsg),
                                          IP_NULL,
                                          ipnet_rtnetlink_addr_dump_family);
    ipnet_rtnetlink_register_family_event(IP_AF_INET6, IP_RTM_NEWADDR,
                                          IP_IFA_MAX, sizeof(struct Ip_ifaddrmsg),
                                          ipnet_rtnetlink_addr_newaddr_family,
                                          IP_NULL);
    ipnet_rtnetlink_register_family_event(IP_AF_INET6, IP_RTM_DELADDR,
                                          IP_IFA_MAX, sizeof(struct Ip_ifaddrmsg),
                                          ipnet_rtnetlink_addr_deladdr_family,
                                          IP_NULL);
    ipnet_rtnetlink_register_family_event(IP_AF_INET6, IP_RTM_GETROUTE,
                                          IP_RTA_MAX, sizeof(struct Ip_rtmsg),
                                          ipnet_rtnetlink_route_getroute_family,
                                          ipnet_rtnetlink_route_dump_family);
    ipnet_rtnetlink_register_family_event(IP_AF_INET6, IP_RTM_NEWROUTE,
                                          IP_RTA_MAX, sizeof(struct Ip_rtmsg),
                                          ipnet_rtnetlink_route_newroute_family,
                                          IP_NULL);
    ipnet_rtnetlink_register_family_event(IP_AF_INET6, IP_RTM_DELROUTE,
                                          IP_RTA_MAX, sizeof(struct Ip_rtmsg),
                                          ipnet_rtnetlink_route_delroute_family,
                                          IP_NULL);
    ipnet_rtnetlink_register_family_event(IP_AF_INET6, IP_RTM_GETRULE,
                                          IP_FRA_MAX, sizeof(struct Ip_fib_rule_hdr),
                                          IP_NULL,
                                          ipnet_rtnetlink_rule_dump_family);
    ipnet_rtnetlink_register_family_event(IP_AF_INET6, IP_RTM_NEWRULE,
                                          IP_FRA_MAX, sizeof(struct Ip_fib_rule_hdr),
                                          ipnet_rtnetlink_rule_newrule_family,
                                          IP_NULL);
    ipnet_rtnetlink_register_family_event(IP_AF_INET6, IP_RTM_DELRULE,
                                          IP_FRA_MAX, sizeof(struct Ip_fib_rule_hdr),
                                          ipnet_rtnetlink_rule_delrule_family,
                                          IP_NULL);
    ipnet_rtnetlink_register_family_event(IP_AF_INET6, IP_RTM_GETNEIGH,
                                          IP_NDA_MAX, sizeof(struct Ip_ndmsg),
                                          IP_NULL,
                                          ipnet_rtnetlink_neigh_dump_family);
    ipnet_rtnetlink_register_family_event(IP_AF_INET6, IP_RTM_DELNEIGH,
                                          IP_NDA_MAX, sizeof(struct Ip_ndmsg),
                                          ipnet_rtnetlink_neigh_delneigh_family,
                                          IP_NULL);
    ipnet_rtnetlink_register_family_event(IP_AF_INET6, IP_RTM_NEWNEIGH,
                                          IP_NDA_MAX, sizeof(struct Ip_ndmsg),
                                          ipnet_rtnetlink_neigh_newneigh_family,
                                          IP_NULL);
#ifdef IPNET_USE_RFC3971
    ipnet_rtnetlink_register_family_event(IP_AF_UNSPEC, IP_RTM_NEWSEND,
                                          IP_NDA_MAX, sizeof(struct Ip_ndmsg),
                                          ipnet_rtnetlink_neigh_newsend_family,
                                          IP_NULL);
    ipnet_rtnetlink_register_family_event(IP_AF_UNSPEC, IP_RTM_SEND_SIGN_REQ,
                                          IP_NDA_MAX, sizeof(struct Ip_ndmsg),
                                          ipnet_rtnetlink_neigh_send_sign_req_family,
                                          IP_NULL);
#endif
}


/*
 *===========================================================================
 *                    ipnet_rtnetlink_ip6_getroute
 *===========================================================================
 * Description: Get route info via NETLINK.
 * Parameters:  data    - Message payload.
 *              len     - Message length
 *              nlh     - NETLINK message header.
 *              arg     - Reference to route attributes
 *              out_msg - Reference to NETLINK reply message
 *
 * Returns:    0 - Success
 *            <0 - Failure
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_ip6_getroute(Ipnet_netlink_mem_t *mem, struct Ip_nlmsghdr *nlh, struct Ip_rtattr **rta)
{
    Ip_u16              vr;
    Ip_u32              table   = IPCOM_ROUTE_TABLE_DEFAULT;
    struct Ip_rtmsg     *rtm    = IP_NLMSG_DATA(nlh);
    struct Ip_in6_addr  addr;
    unsigned            scope_id = 0;
    int                 ret = -1;
    Ipnet_route_entry   *rt_entry;

#ifdef IPNET_USE_ROUTE_TABLE_NAMES
    if (rta[IP_RTA_TABLE_NAME -1])
    {
        if (ipnet_route_vr_and_table_from_name(IP_RTA_DATA(rta[IP_RTA_TABLE_NAME - 1]),
                                               &vr,
                                               &table) < 0)
            return -IP_ERRNO_ESRCH;
    }
    else
#endif /* IPNET_USE_ROUTE_TABLE_NAMES */
    {
        if (mem->vr == IPCOM_VR_ANY)
            return -IP_ERRNO_EINVAL;

        vr = ipnet_rtnetlink_vr(rta[IP_RTA_VR - 1], mem->vr);
        if (rta[IP_RTA_TABLE - 1])
            table = IP_GET_32ON8(IP_RTA_DATA(rta[IP_RTA_TABLE - 1]));
    }

    if (rta[IP_RTA_DST-1])
        ipcom_memcpy(&addr, IP_RTA_DATA(rta[IP_RTA_DST - 1]), sizeof(addr));
    else
        ipcom_memset(&addr, 0, sizeof(addr));

    if (rta[IP_RTA_OIF - 1])
        scope_id = IP_GET_32ON8(IP_RTA_DATA(rta[IP_RTA_OIF - 1]));

    /* Lookup the route */
    ret = ipnet_route_raw_lookup(rtm->rtm_family,
                                 vr,
                                 table,
                                 0,
                                 &addr,
                                 scope_id,
                                 0,
                                 &rt_entry);

    if (ret < 0)
        return ret;

    if (scope_id != 0)
    {
        /* Find a route that matches specified outgoing interface */
        while ((rt_entry != IP_NULL)  && (rt_entry->netif->ipcom.ifindex != scope_id))
            rt_entry = rt_entry->next;

        if (rt_entry == IP_NULL)
            return -IP_ERRNO_ENETUNREACH;
    }

    ret = ipnet_rtnetlink_ip6_route_fill_info(mem,
                                              rt_entry,
                                              nlh->nlmsg_seq,
                                              IP_RTM_NEWROUTE,
                                              0);

    return ret < 0? -IP_ERRNO_ENOMEM : ret;
}

/*
 *===========================================================================
 *                    ipnet_rtnetlink_ip6_deladdr
 *===========================================================================
 * Description: Delete an interface address via NETLINK
 * Parameters:  data - NETLINK message payload area
 *              len  - Length of message
 *              nlh  - NETLINK message header.
 *              arg  - Reference to attributes.
 *
 * Returns:     0 - Success
 *             <0 - Failure
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_ip6_deladdr(Ipnet_netlink_mem_t *mem, struct Ip_nlmsghdr *nlh, struct Ip_rtattr **rta)
{
    Ipnet_netif             *netif;
    Ip_u16                  vr;
    int                     ret = -IP_ERRNO_EINVAL;
    Ipnet_ppp_peer         *p;
    struct Ip_ifaddrmsg *ifm = IP_NLMSG_DATA(nlh);

#ifdef IPNET_USE_ROUTE_TABLE_NAMES
    if (rta[IP_IFA_TABLE_NAME -1])
    {
        Ip_u32 table;

        /* Extract router and table from name */
        if (ipnet_route_vr_and_table_from_name(IP_RTA_DATA(rta[IP_IFA_TABLE_NAME - 1]),
                                               &vr,
                                               &table) < 0)
            return -IP_ERRNO_ESRCH;
    }
    else
#endif
    {
        if (mem->vr == IPCOM_VR_ANY)
            return -IP_ERRNO_EINVAL;

        vr = ipnet_rtnetlink_vr(rta[IP_IFA_VR - 1], mem->vr);
    }

    netif = ipnet_if_indextonetif(vr, ifm->ifa_index);

    /* Interface required */
    if (netif == IP_NULL)
        return -IP_ERRNO_EINVAL;

    /* Caller must be in the same vr as the interface */
    if (netif->vr_index != vr)
        return -IP_ERRNO_EINVAL;

    p = netif->private_data;

    {
        struct Ip_in6_addr *addr = IP_NULL;
        struct Ip_in6_addr *addr_anycast = IP_NULL;
        struct Ip_in6_addr *peer = IP_NULL;

        if (rta[IP_IFA_ADDRESS-1])
        {
            if (IP_RTA_PAYLOAD(rta[IP_IFA_ADDRESS-1]) < sizeof(*peer))
                return -IP_ERRNO_EINVAL;

            peer = IP_RTA_DATA(rta[IP_IFA_ADDRESS-1]);
        }

        if (rta[IP_IFA_LOCAL-1])
        {
            if (IP_RTA_PAYLOAD(rta[IP_IFA_LOCAL-1]) < sizeof(*addr))
                return -IP_ERRNO_EINVAL;

            addr = IP_RTA_DATA(rta[IP_IFA_LOCAL-1]);
        }

        if (rta[IP_IFA_ANYCAST-1])
        {
            if (IP_RTA_PAYLOAD(rta[IP_IFA_ANYCAST-1]) < sizeof(*addr_anycast))
                return -IP_ERRNO_EINVAL;

            addr_anycast = IP_RTA_DATA(rta[IP_IFA_ANYCAST-1]);
        }

        addr = addr == IP_NULL ? peer : addr;
        if (addr == IP_NULL)
            return -IP_ERRNO_EINVAL;

        /* If peer address is specified, check that interface supports it */
        if (peer && peer != addr && !IP_IN6_ARE_ADDR_EQUAL(peer,addr) && p == IP_NULL)
            return -IP_ERRNO_EINVAL;

        ret = ipnet_ip6_remove_addr(netif, addr);
        if (ret == 0 && peer && peer != addr && !IP_IN6_ARE_ADDR_EQUAL(peer,addr))
        {
            ipcom_memset(peer, 0, sizeof(struct Ip_in6_addr));
            ret = ipnet_if_set_ipv6_ppp_peer(p, peer);
        }

        if (ret == 0 && addr_anycast)
            ret = ipnet_ip6_remove_addr(netif, addr_anycast);
    }

    return ret;
}

/*
 *===========================================================================
 *                    ipnet_rtnetlink_ip6_newaddr
 *===========================================================================
 * Description: Add a new interface address via NETLINK
 * Parameters:  data - NETLINK message payload area
 *              len  - Length of message
 *              nlh  - NETLINK message header
 *              arg  - Reference to attributes
 *
 * Returns:     0 - Success
 *             <0 - Failure
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_ip6_newaddr(Ipnet_netlink_mem_t *mem, struct Ip_nlmsghdr *nlh, struct Ip_rtattr **rta)
{
    Ip_u16                     vr;
    Ip_u32                     flags = 0;
    Ipnet_netif                *netif;
    Ipnet_ppp_peer             *p;
    int                        ret = -1;
    struct Ip_ifa_cacheinfo *ifc = IP_NULL;
    struct Ip_ifaddrmsg     *ifm = IP_NLMSG_DATA(nlh);


#ifdef IPNET_USE_ROUTE_TABLE_NAMES
    if (rta[IP_IFA_TABLE_NAME -1])
    {
        Ip_u32  table;

        /* Extract vr from name */
        if (ipnet_route_vr_and_table_from_name(IP_RTA_DATA(rta[IP_IFA_TABLE_NAME - 1]),
                                               &vr,
                                               &table) < 0)
            return -IP_ERRNO_ESRCH;
    }
    else
#endif
    {
        if (mem->vr == IPCOM_VR_ANY)
            return -IP_ERRNO_EINVAL;

        vr = ipnet_rtnetlink_vr(rta[IP_IFA_VR - 1], mem->vr);
    }

    netif = ipnet_if_indextonetif(vr, ifm->ifa_index);

    /* Interface required */
    if (netif == IP_NULL)
        return -IP_ERRNO_EINVAL;

    /* Caller must be in the same vr as the interface */
    if (netif->vr_index != vr)
        return -IP_ERRNO_EINVAL;

    p = netif->private_data;

    if (rta[IP_IFA_CACHEINFO -1])
    {
        if (IP_RTA_PAYLOAD(rta[IP_IFA_CACHEINFO-1]) < sizeof(*ifc))
            return -IP_ERRNO_EINVAL;
        ifc = IP_RTA_DATA(rta[IP_IFA_CACHEINFO-1]);
    }

    {
        struct Ip_in6_addr *addr = IP_NULL;
        struct Ip_in6_addr *addr_anycast = IP_NULL;
        struct Ip_in6_addr *peer = IP_NULL;

        if (rta[IP_IFA_ADDRESS-1])
        {
            if (IP_RTA_PAYLOAD(rta[IP_IFA_ADDRESS-1]) < sizeof(*peer))
                return -IP_ERRNO_EINVAL;
            peer = IP_RTA_DATA(rta[IP_IFA_ADDRESS-1]);
        }

        if (rta[IP_IFA_LOCAL-1])
        {
            if (IP_RTA_PAYLOAD(rta[IP_IFA_LOCAL-1]) < sizeof(*addr))
                return -IP_ERRNO_EINVAL;
            addr = IP_RTA_DATA(rta[IP_IFA_LOCAL-1]);
        }

        if (rta[IP_IFA_ANYCAST-1])
        {
            if (IP_RTA_PAYLOAD(rta[IP_IFA_ANYCAST-1]) < sizeof(*addr_anycast))
                return -IP_ERRNO_EINVAL;
            addr_anycast = IP_RTA_DATA(rta[IP_IFA_ANYCAST-1]);
        }

        if (rta[IP_IFA_X_INFO-1])
        {
            Ip_u32 xflags = IP_GET_32ON8(IP_RTA_DATA(rta[IP_IFA_X_INFO-1]));

            if (IP_BIT_ISSET(xflags, IP_IFA_F_X_AUTOMATIC))
                IP_BIT_SET(flags, IPNET_IP6_ADDR_FLAG_AUTOMATIC);
        }

        if (IP_BIT_ISSET(ifm->ifa_flags, IP_IFA_F_TENTATIVE))
            IP_BIT_SET(flags, IPNET_IP6_ADDR_FLAG_TENTATIVE);

        if (IP_BIT_ISSET(ifm->ifa_flags, IP_IFA_F_HOMEADDRESS))
            IP_BIT_SET(flags, IPNET_IP6_ADDR_FLAG_HOMEADDRESS);
#ifdef IPNET_USE_RFC3971
        if (IP_BIT_ISSET(ifm->ifa_flags, IP_IFA_F_CGA))
            IP_BIT_SET(flags, IPNET_IP6_ADDR_FLAG_CGA);
#endif
        addr = addr == IP_NULL ? peer : addr;
        if (addr == IP_NULL)
            return -IP_ERRNO_EINVAL;

        /* If peer address is specified, check that interface supports it */
        if (peer && peer != addr && !IP_IN6_ARE_ADDR_EQUAL(peer,addr) && p == IP_NULL)
            return -IP_ERRNO_EINVAL;

        ret = ipnet_ip6_add_addr(netif,
                                 addr,
                                 flags,
                                 ifc ? ifc->ifa_prefered : IPCOM_ADDR_INFINITE,
                                 ifc ? ifc->ifa_valid : IPCOM_ADDR_INFINITE,
                                 ifm->ifa_prefixlen);

        if (ret == 0 && peer && peer != addr && !IP_IN6_ARE_ADDR_EQUAL(peer,addr))
            ret = ipnet_if_set_ipv6_ppp_peer(p, peer);
        else if (ret == 0 && addr_anycast != IP_NULL)
            ret = ipnet_ip6_add_addr(netif,
                                     addr_anycast,
                                     IPNET_IP6_ADDR_FLAG_DONT_ADD_NETWORK_RT | IPNET_IP6_ADDR_FLAG_ANYCAST,
                                     ifc ? ifc->ifa_prefered : IPCOM_ADDR_INFINITE,
                                     ifc ? ifc->ifa_valid : IPCOM_ADDR_INFINITE,
                                     ifm->ifa_prefixlen);

        if (ret < 0)
            (void)ipnet_ip6_remove_addr(netif, addr);

    }

    /*
    ipnet_softirq_wait_for_pending();
    */
    return ret;
}

/*
 *===========================================================================
 *                    ipnet_rtnetlink_ip6_neigh_fill_info
 *===========================================================================
 * Description: Fill a NETLINK message with info about a route.
 * Parameters:  data - Message buffer.
 *              rt   - IPNET route entry.
 *              pid  - NETLINK caller pid.
 *              len  - Available buffer length
 *
 * Returns:     0 - Success
 *             <0 - Failure
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_ip6_neigh_fill_info(Ipnet_netlink_mem_t *mem,
                                    Ipnet_route_entry   *rt,
                                    int                 seq,
                                    Ip_u8               event,
                                    Ip_u32              eflags)
{
    struct Ip_nlmsghdr        *nlh;
    struct Ip_ndmsg           *ndm;
    Ip_u32                    tmp32;
#ifdef IPNET_USE_ROUTE_TABLE_NAMES
    char                       name[IPCOM_RTNAMSIZ];
#endif /* IPNET_USE_ROUTE_TABLE_NAMES */
    Ipnet_neigh_entry         *ne = IPNET_NEIGH_FROM_RT(rt);
    int                       type = ipnet_ip6_get_addr_type2((struct Ip_in6_addr*)rt->hdr.key,
                                                              rt->netif);

    if (!ne || (type != IPNET_ADDR_TYPE_NOT_LOCAL))
        return (0);

    nlh = ipnet_netlink_nlmsg_sz_begin(mem, seq, event, eflags);
    if (!nlh)
        return -1;

    ndm = ipnet_netlink_sz_put(mem, IP_NULL, sizeof (*ndm));
    if (!ndm)
        return -1;

    /* Setup the neigh header */
    ndm->ndm_family  = IP_AF_INET6;
    ndm->ndm_type    = IP_RTN_UNICAST;
    ndm->ndm_state   = ipnet_rtnetlink_neigh_nc2nud(ne->state, rt->metrics.rmx_expire);
    ndm->ndm_ifindex = rt->netif ? rt->netif->ipcom.ifindex : 0;
    ndm->ndm_flags   = IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_PROTO2) ? IP_NTF_PROXY : 0;

    /* Put destination address */
    if (!ipnet_rtnetlink_rta_sz_put(mem, IP_NDA_DST, sizeof(struct Ip_in6_addr), rt->hdr.key))
        return -1;

    /* Put route table index */
    tmp32 = IPNET_ROUTE_GET_TABLE(rt->head);
    if (!ipnet_rtnetlink_rta_sz_put(mem, IP_NDA_TABLE, sizeof(tmp32), &tmp32))
        return -1;

    /* Put virtual router index */
    tmp32 = IPNET_ROUTE_GET_VR(rt->head);
    if (!ipnet_rtnetlink_rta_sz_put(mem, IP_NDA_VR, sizeof(tmp32), &tmp32))
        return -1;

#ifdef IPNET_USE_ROUTE_TABLE_NAMES
    /* Put table name */
    if (ipnet_route_get_name(IPNET_ROUTE_GET_VR(rt->head), IPNET_ROUTE_GET_TABLE(rt->head), name))
    {
        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_NDA_TABLE_NAME, ipcom_strlen(name) + 1, name))
            return -1;
    }
#endif /* IPNET_USE_ROUTE_TABLE_NAMES */
#ifdef IPCOM_STORE_PHYSICAL_IF
    if (ne->physical_ifindex)
    {
        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_NDA_REALIFINDEX, sizeof(ne->physical_ifindex), &ne->physical_ifindex))
            return -1;
    }
#endif /* IPCOM_STORE_PHYSICAL_IF */

    /* Put link layer address */
    if (rt->gateway)
    {
        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_NDA_LLADDR, ((struct Ip_sockaddr_dl *)rt->gateway)->sdl_alen, IP_SOCKADDR_DL_LLADDR(rt->gateway)))
            return -1;
    }

    return ipnet_netlink_nlmsg_sz_end(mem, nlh);
}


/*
 *===========================================================================
 *                    ipnet_rtnetlink_ip6_route_fill_info
 *===========================================================================
 * Description: Fill a NETLINK message with info about a route.
 * Parameters:  data - Message buffer.
 *              rt   - IPNET route entry.
 *              pid  - NETLINK caller pid.
 *              len  - Available buffer length
 *
 * Returns:     0 - Success
 *             <0 - Failure
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_ip6_route_fill_info(Ipnet_netlink_mem_t   *mem,
                                  Ipnet_route_entry     *rt,
                                  int                   seq,
                                  Ip_u8                 event,
                                  Ip_u32                eflags)
{
    struct Ip_nlmsghdr        *nlh;
    struct Ip_rtattr          *rta;
    Ip_u32                    tmp32;

#ifdef IPNET_USE_ROUTE_TABLE_NAMES
    char                       name[IPCOM_RTNAMSIZ];
#endif /* IPNET_USE_ROUTE_TABLE_NAMES */
#ifdef IPNET_USE_ROUTE_COOKIES
    Ipnet_rt_cookie           zcookie;
#endif /* IPNET_USE_ROPUTE_COOKIES */

    struct Ip_rtmsg            *rtm;
    const struct Ip_in6_addr    *addr = (struct Ip_in6_addr*)rt->hdr.key;


    nlh = ipnet_netlink_nlmsg_sz_begin(mem, seq, event, eflags);
    if (!nlh)
        return -1;

    rtm = ipnet_netlink_sz_put(mem, IP_NULL, sizeof (*rtm));
    if (!rtm)
        return -1;

    /* Add route message header */
    rtm->rtm_family   = (Ip_u8)IPNET_ROUTE_GET_FAMILY(rt->head);
    rtm->rtm_table    = (Ip_u8)IPNET_ROUTE_GET_TABLE(rt->head);
    rtm->rtm_protocol = 0;
    rtm->rtm_scope    = IP_RT_SCOPE_UNIVERSE;
    rtm->rtm_flags    = 0;
    rtm->rtm_src_len  = 0;
    rtm->rtm_tos      = 0;

    /* Calculate prefixlen */
    if (rt->hdr.mask)
        rtm->rtm_dst_len  = (Ip_u8)ipcom_mask_to_prefixlen(rt->hdr.mask, rt->head->bit_count);
    else
        rtm->rtm_dst_len  = (Ip_u8)(sizeof(*addr) * 8);

    /* Set route type */
    if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_DYNAMIC))
        rtm->rtm_protocol = IP_RTPROT_REDIRECT;
    else
        rtm->rtm_protocol = IP_RTPROT_KERNEL;

    if (IP_IN6_IS_ADDR_MULTICAST(addr))
        rtm->rtm_type = IP_RTN_MULTICAST;
    else if (IP_BIT_ISSET(rt->hdr.flags,IPNET_RTF_REJECT))
        rtm->rtm_type = IP_RTN_PROHIBIT;
    else if (IP_BIT_ISSET(rt->hdr.flags,IPNET_RTF_SKIP))
        rtm->rtm_type = IP_RTN_THROW;
    else if (IP_BIT_ISFALSE(rt->hdr.flags,IPNET_RTF_UP))
        rtm->rtm_type = IP_RTN_UNREACHABLE;
    else if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_CLONING))
        rtm->rtm_type = IP_RTN_CLONE;
    else if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_BLACKHOLE))
        rtm->rtm_type = IP_RTN_BLACKHOLE;
    else if (IP_BIT_ISSET(rt->hdr.flags,IPNET_RTF_PROTO2))
        rtm->rtm_type = IP_RTN_PROXY;
    else if ((rt->netif != IP_NULL) && (IP_BIT_ISSET(rt->netif->ipcom.flags, IP_IFF_LOOPBACK)))
        rtm->rtm_type = IP_RTN_LOCAL;
    else
        rtm->rtm_type = IP_RTN_UNICAST;


    /* Put destination address */
    if (!ipnet_rtnetlink_rta_sz_put(mem, IP_RTA_DST, sizeof(*addr), addr))
        return -1;

    /* Put route metrics */
    if (ipnet_rtnetlink_route_sz_fill_metrics(mem,
                                              &rt->metrics,
                                              rt->netif ? rt->netif->ipcom.mtu : 0) < 0)
        return -1;


    /* Multipath */
    if (rt->next)
    {
        struct Ip_rtnexthop *nh;
        Ipnet_route_entry   *mrt = rt;

        rta = ipnet_rtnetlink_rta_sz_nested_begin(mem, IP_RTA_MULTIPATH);
        if (!rta)
            return -1;

        while (mrt)
        {
            nh = ipnet_netlink_sz_put(mem, IP_NULL, sizeof(*nh));
            if (!nh)
                return -1;

            nh->rtnh_flags   = 0;
            nh->rtnh_ifindex = mrt->netif ? mrt->netif->ipcom.ifindex : 0;
            nh->rtnh_hops    = mrt->metrics.rmx_hopcount;

            if (IP_BIT_ISSET(mrt->hdr.flags, IPNET_RTF_GATEWAY) && mrt->gateway)
            {
                struct Ip_in6_addr *gw = &((struct Ip_sockaddr_in6 *)mrt->gateway)->sin6_addr;

                if (!ipnet_rtnetlink_rta_sz_put(mem, IP_RTA_GATEWAY, sizeof(*gw), gw))
                    return -1;
            }

            ipnet_rtnetlink_rta_sz_nested_end(mem, nh);
            mrt = mrt->next;
        }

        ipnet_rtnetlink_rta_sz_nested_end(mem, rta);
    }
    else
    {
        if ((IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_GATEWAY)
             && rt->gateway
             && (rt->gateway->sa_family == IP_AF_INET6)))
        {
            addr = &((struct Ip_sockaddr_in6 *)rt->gateway)->sin6_addr;

            if (!ipnet_rtnetlink_rta_sz_put(mem, IP_RTA_GATEWAY, sizeof(*addr), addr))
                return -1;
        }

        if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_PROTO2)
            && IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_LLINFO)
            && (rt->gateway->sa_family == IP_AF_LINK))
        {
            /* Network proxy arp route with specific lladdr set */
            if (!ipnet_rtnetlink_rta_sz_put(mem, IP_RTA_PROXY_ARP_LLADDR, IPNET_ETH_ADDR_SIZE, IP_SOCKADDR_DL_LLADDR(rt->gateway)))
                return -1;
        }

#ifdef IPMPLS
        if ((IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_LLINFO) && (rt->gateway->sa_family == IP_AF_MPLS)))
        {
            Ip_u32  nh_type = IPNET_ETH_P_MPLS_UNICAST;

            /* MPLS Pseudo-Wire route */
            if (!ipnet_rtnetlink_rta_sz_put(mem, IP_RTA_NH_PROTO, sizeof(Ip_u32), &nh_type))
                return -1;

            if (!ipnet_rtnetlink_rta_sz_put(mem, IP_RTA_NH_PROTO_DATA, sizeof(Ip_u32), &((struct Ip_sockaddr_mpls*)rt->gateway)->smpls_key))
                return -1;
        }
        else
#endif
        {
            /* Add interface */
            if (rt->netif)
                if (!ipnet_rtnetlink_rta_sz_put(mem, IP_RTA_OIF, sizeof(int), &rt->netif->ipcom.ifindex))
                    return -1;
        }
    }

    rta = ipnet_rtnetlink_rta_sz_put(mem, IP_RTA_CACHEINFO, sizeof(struct Ip_rta_cacheinfo), IP_NULL);
    if (!rta)
        return -1;
    else
    {
        struct Ip_rta_cacheinfo   *ci = IP_RTA_DATA(rta);

        ipcom_memset(ci, 0, sizeof(*ci));
        if (rt->metrics.rmx_expire != 0xffffffff)
            ci->rta_expires = rt->metrics.rmx_expire;
        ci->rta_used    = rt->use;
    }

    /* Add route table index */
    tmp32 = IPNET_ROUTE_GET_TABLE(rt->head);
    if (!ipnet_rtnetlink_rta_sz_put(mem, IP_RTA_TABLE, sizeof(tmp32), &tmp32))
        return -1;


    /* Add virtual router index */
    tmp32 = IPNET_ROUTE_GET_VR(rt->head);
    if (!ipnet_rtnetlink_rta_sz_put(mem, IP_RTA_VR, sizeof(tmp32), &tmp32))
        return -1;

#ifdef IPNET_USE_ROUTE_TABLE_NAMES
    if (ipnet_route_get_name(IPNET_ROUTE_GET_VR(rt->head), IPNET_ROUTE_GET_TABLE(rt->head), name))
    {
        /* Add table name */
        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_RTA_TABLE_NAME, ipcom_strlen(name) + 1, name))
            return -1;
    }
#endif /* IPNET_USE_ROUTE_TABLE_NAMES */

#ifdef IPNET_USE_ROUTE_COOKIES
    ipcom_memset(&zcookie, 0, sizeof(Ipnet_rt_cookie));
    if (ipcom_memcmp(&zcookie, &rt->cookie, sizeof(Ipnet_rt_cookie)) != 0)
    {
        /* Add route cookie */
        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_RTA_COOKIE, sizeof(rt->cookie), &rt->cookie))
            return -1;
    }
#endif /* IPNET_USE_ROUTE_COOKIES */

    /* Add source address that will be used to the destination on unbound sends*/
    addr = ipnet_ip6_get_src_addr(IPNET_ROUTE_GET_VR(rt->head),
                                  &((struct Ipnet_ipv6_key *)rt->hdr.key)->addr,
                                  ((struct Ipnet_ipv6_key *)rt->hdr.key)->scope_id,
                                  rt,
                                  IP_NULL,
                                  IP_NULL,
                                  0);
    if (addr)
    {
        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_RTA_PREFSRC, sizeof(*addr), addr))
            return -1;
    }

    /* Done */
    return ipnet_netlink_nlmsg_sz_end(mem, nlh);
}


/*
 *==========================================================================
 *                    ipnet_rtnetlink_ip6_addr_dump
 *===========================================================================
 * Description: Get interface info via NETLINK
 * Parameters:  data - Message payload.
 *              len  - Length of message.
 *              cb   - NETLINK callback entry
 *
 * Returns:     0 - Success
 *             <0 - Failure
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_ip6_addr_dump(Ipnet_netlink_mem_t *mem,
                              struct Ip_nlmsghdr  *nlmsg,
                              int                 req_addr_type,
                              Ip_ptrdiff_t        *args)
{
    Ip_ptrdiff_t              *done         = &args[0];
    Ip_ptrdiff_t              *s_addr_ix    = &args[1];
    Ip_ptrdiff_t              *s_if_ix      = &args[2];
    Ip_ptrdiff_t              addr_ix       = 0;
    Ip_u32                    if_ix = 0;
    Ipnet_netif               *netif;
    Ipnet_ip6_addr_entry      *addr_it;
    Ipnet_ppp_peer            *p;
    int                       ret = 0;
    unsigned                  i;
    struct Ip_in6_addr        *peer;
    unsigned                  sifindex      = *s_if_ix;

    if (*done)
        goto exit_out;

    IPNET_NETIF_FOR_EACH(netif, i)
    {
        addr_ix     = 0;
        if_ix       = netif->ipcom.ifindex;

        if (IP_BIT_ISFALSE(nlmsg->nlmsg_flags, IP_NLM_F_VR_UNSPEC) && (mem->vr != IPCOM_VR_ANY) && (netif->vr_index != mem->vr))
            continue;

        if (sifindex != 0 && sifindex != netif->ipcom.ifindex)
            continue;

        /* We've found the start index */
        sifindex    = 0;
        /* Store the current interface index */
        *s_if_ix   = netif->ipcom.ifindex;

        IPNET_IF_LOCK(netif);

        p = netif->private_data;

        addr_it = netif->inet6_addr_list;
        peer    = p ? &p->peer6 : IP_NULL;

        while (addr_it)
        {
            if (addr_ix >= *s_addr_ix)
            {
                if (addr_it->type == req_addr_type)
                {
                    ret = ipnet_rtnetlink_ip6_addr_fillinfo(mem,
                                                            addr_it,
                                                            peer,
                                                            netif,
                                                            nlmsg->nlmsg_seq,
                                                            IP_RTM_NEWADDR,
                                                            IP_NLM_F_MULTI);

                    if (ret < 0)
                    {
                        IPNET_IF_UNLOCK(netif);
                        goto exit_out;
                    }
                }
            }

            peer = IP_NULL;
            addr_it = addr_it->next;
            addr_ix++;
        }
        IPNET_IF_UNLOCK(netif);
        addr_ix = 0;
    }

    *done = 1;

exit_out:
    *s_addr_ix = addr_ix;
    return mem->used;
}


/*
 *===========================================================================
 *                    ipnet_rtnetlink_ip6_route_key_setup
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_ip6_route_key_setup(Ipnet_rtnetlink_key_t *ukey,
                                    Ip_u32                ifindex,
                                    void                  **maskp,
                                    int                   mask_len,
                                    struct Ip_rtattr      *dst)
{
    Ip_bool have_mask = IP_FALSE;

    ipcom_memset(ukey, 0, sizeof(*ukey));

    if (mask_len != -1 && mask_len < (signed) (sizeof(ukey->v6.key.addr) * 8))
    {
        /* Mask valid */
        ipnet_route_create_mask(&ukey->v6.mask.addr, mask_len);
        have_mask = IP_TRUE;
    }

    if (dst)
    {
        if (IP_RTA_PAYLOAD(dst) < sizeof(struct Ip_in6_addr))
            return -IP_ERRNO_EINVAL;

        ipcom_memcpy(&ukey->v6.key.addr, IP_RTA_DATA(dst), sizeof(struct Ip_in6_addr));

        if (ifindex && ipnet_ip6_is_scoped_addr(&ukey->v6.key.addr))
        {
            ukey->v6.key.scope_id  = ipnet_ip6_get_zone(ifindex, &ukey->v6.key.addr);
            ukey->v6.mask.scope_id = 0xffffffff;
        }

        if (have_mask)
            ipnet_route_apply_mask(&ukey->v6.key.addr, &ukey->v6.mask.addr, sizeof(struct Ip_in6_addr) * 8);
    }

    /* Store mask */
    if (maskp != IP_NULL && have_mask)
        *maskp = &ukey->v6.mask;
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_rtnetlink_ip6_route_gw_setup
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_ip6_route_gw_setup(Ip_u32                     ifindex,
                                   union Ip_sockaddr_union    *ugw,
                                   struct Ip_sockaddr         **gw,
                                   struct Ip_rtattr           *rta)
{
    struct Ip_sockaddr         *gw6 = IP_NULL;

    ipcom_memset(ugw, 0, sizeof(*ugw));

    ugw->sin6.sin6_family   = IP_AF_INET6;
    IPCOM_SA_LEN_SET(&ugw->sin6, sizeof(struct Ip_sockaddr_in6));

    if (rta)
    {
        if (IP_RTA_PAYLOAD(rta) < sizeof(struct Ip_in6_addr))
            return -IP_ERRNO_EINVAL;

        ipcom_memcpy(&ugw->sin6.sin6_addr, IP_RTA_DATA(rta), sizeof(struct Ip_in6_addr));

        if (ifindex && ipnet_ip6_is_scoped_addr(&ugw->sin6.sin6_addr))
            ugw->sin6.sin6_scope_id = ipnet_ip6_get_zone(ifindex, &ugw->sin6.sin6_addr);

        gw6 = &ugw->sa;
    }

    if (gw)
        *gw = gw6;

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_rtnetlink_ip6_neigh_setup
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_ip6_neigh_setup(Ipnet_rtnetlink_key_t *ukey,
                                struct Ip_ndmsg       *ndm,
                                Ipnet_route_entry     *rt,
                                Ipnet_nd_state_t      nd_state,
                                void                  *link_addr,
                                struct Ip_rtattr      **rta)
{
    Ipnet_ip6_nc_entry           *nc = IPNET_IP6_NC(rt);
#ifdef IPCOM_STORE_PHYSICAL_IF
    if (rta[IP_NDA_REALIFINDEX - 1])
        nc->ne.physical_ifindex = IP_GET_32ON8(IP_RTA_DATA(rta[IP_NDA_REALIFINDEX - 1]));
#else
    (void)rta;
#endif
    (void)ndm;
    (void)ukey;
    ipnet_ip6_neighbor_cache_update(nc, nd_state, link_addr, IP_TRUE);

    return 0;
}


IP_GLOBAL void
ipnet_rtnetlink_ip6_addr_add(Ipnet_netif                        *netif,
                             struct Ipnet_ip6_addr_entry_struct *addr)
{

#ifdef IP_PORT_LKM
    ipcom_lkm_notify_address_event(netif, IP_AF_INET6, addr, IPCOM_LKM_ADDR_ADD);
#endif
    ipnet_rtnetlink_ip6_addr_event(netif, addr, IP_RTM_NEWADDR);
}

IP_GLOBAL void
ipnet_rtnetlink_ip6_addr_del(Ipnet_netif                        *netif,
                             struct Ipnet_ip6_addr_entry_struct *addr)
{

#ifdef IP_PORT_LKM
    ipcom_lkm_notify_address_event(netif, IP_AF_INET6, addr, IPCOM_LKM_ADDR_DEL);
#endif
    ipnet_rtnetlink_ip6_addr_event(netif, addr, IP_RTM_DELADDR);
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */


#endif
/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
