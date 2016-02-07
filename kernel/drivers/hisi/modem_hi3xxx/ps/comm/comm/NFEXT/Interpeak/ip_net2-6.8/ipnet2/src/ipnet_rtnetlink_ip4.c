
/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_rtnetlink_ip4.c,v $ $Revision: 1.15.8.2.2.2 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_rtnetlink_ip4.c,v $
 *   $Author: ulf $
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
 *===========================================================================
 *                         misc
 *===========================================================================
#define IPCOM_USE_PROFILING_ENABLE
 */

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#if defined(IPCOM_USE_INET) && defined(IPNET_USE_NETLINK)

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_inet.h>
#include <ipcom_prof.h>
#include <ipcom_syslog.h>
#include <ipcom_sysvar.h>
#include <ipcom_type.h>
#include <ipcom_sock.h>
#include <ipcom_sock2.h>

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_ip4.h"
#include "ipnet_loopback.h"

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
#define IS_LOOPBACK(x) (((x) & ip_htonl(0xff000000)) == ip_htonl(0x7f000000))

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
ipnet_rtnetlink_ip4_addr_fillinfo(Ipnet_netlink_mem_t     *mem,
                                  Ipnet_ip4_addr_entry    *ifa,
                                  struct Ip_in_addr       *peer,
                                  Ipnet_netif             *netif,
                                  Ip_bool                 primary,
                                  int                     seq,
                                  Ip_u8                   event,
                                  Ip_u32                  eflags)
{
    Ip_u32                  subbrd_n;
    Ip_u32                  tmp32;
    struct Ip_ifaddrmsg     *ifm;
    struct Ip_nlmsghdr      *nlh;

    if (ifa->ipaddr_n == IP_INADDR_ANY)/* Ignore the 0.0.0.0 */
        return 0;

    nlh = ipnet_netlink_nlmsg_sz_begin(mem, seq, event, eflags);
    if (!nlh)
        return -1;

    ifm = ipnet_netlink_sz_put(mem, IP_NULL, sizeof (*ifm));
    if (!ifm)
        return -1;

    ifm->ifa_family = IP_AF_INET;
    ifm->ifa_flags  = IP_IFA_F_PERMANENT | (primary ? 0 : IP_IFA_F_SECONDARY);
    ifm->ifa_index  = netif->ipcom.ifindex;

    if (IP_BIT_ISSET(ifa->flags, IPNET_IP4_ADDR_FLAG_HOMEADDRESS))
        IP_BIT_SET(ifm->ifa_flags, IP_IFA_F_HOMEADDRESS);

    if (peer && (peer->s_addr != IP_INADDR_ANY))
    {
        /* Peer-to-peer address */
        ifm->ifa_scope     = IP_RT_SCOPE_UNIVERSE;
        ifm->ifa_prefixlen = 32;

        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_IFA_ADDRESS, sizeof(struct Ip_in_addr), peer))
            return -1;
    }
    else
    {
        /* Normal address */
        ifm->ifa_prefixlen = (Ip_u8)ipcom_mask_to_prefixlen(&ifa->netmask_n, sizeof(struct Ip_in_addr) * 8);

        if (IS_LOOPBACK(ifa->ipaddr_n))
            ifm->ifa_scope = IP_RT_SCOPE_HOST;
        else
            ifm->ifa_scope = IP_RT_SCOPE_UNIVERSE;

        subbrd_n = (ifa->netmask_n & ifa->ipaddr_n) | ~ifa->netmask_n;

        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_IFA_BROADCAST, sizeof(struct Ip_in_addr), &subbrd_n))
            return -1;
    }

    if (!ipnet_rtnetlink_rta_sz_put(mem, IP_IFA_LOCAL, sizeof(struct Ip_in_addr), &ifa->ipaddr_n))
        return -1;


    tmp32 = netif->vr_index;
    if (!ipnet_rtnetlink_rta_sz_put(mem, IP_IFA_VR, sizeof(tmp32), &tmp32))
        return -1;

    if (!ipnet_rtnetlink_rta_sz_put(mem, IP_IFA_LABEL, IP_IFNAMSIZ, netif->ipcom.name))
        return -1;

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
ipnet_rtnetlink_ip4_addr_event(Ipnet_netif *netif, struct Ipnet_ip4_addr_entry_struct *addr, int event)
{
    Ipcom_pkt               *pkt    = IP_NULL;
    Ipnet_netlink_mem_t     mem;
    int                     ret;
    Ipnet_ppp_peer         *p       = netif->private_data;

    if (!ipnet_netlink_pkt_alloc(&pkt, IP_NULL, IP_TRUE, &mem, 512))
        return;

    /* Must explicitly switch to the correct VR here */
    pkt->vr_index = mem.vr = netif->vr_index;

    ret = ipnet_rtnetlink_ip4_addr_fillinfo(&mem,
                                            addr,
                                            p && addr == netif->inet4_addr_list ? &p->peer4 : IP_NULL,
                                            netif,
                                            addr == netif->inet4_addr_list,
                                            0,
                                            event,
                                            0);

    if (ret < 0)
        ipcom_pkt_free(pkt);
    else
    {
        pkt->end += ret;

        /* We're done; do report this pkt */
        ipnet_rtnetlink_broadcast(pkt, IP_RTNLGRP_IPV4_IFADDR);
    }
}

/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

IP_GLOBAL void
ipnet_rtnetlink_ip4_init(void)
{
    ipnet_rtnetlink_register_family_event(IP_AF_INET, IP_RTM_GETADDR,
                                          IP_IFA_MAX, sizeof(struct Ip_ifaddrmsg),
                                          IP_NULL,
                                          ipnet_rtnetlink_addr_dump_family);
    ipnet_rtnetlink_register_family_event(IP_AF_INET, IP_RTM_GETMULTICAST,
                                          IP_IFA_MAX, sizeof(struct Ip_ifaddrmsg),
                                          IP_NULL,
                                          ipnet_rtnetlink_addr_dump_family);
    ipnet_rtnetlink_register_family_event(IP_AF_INET, IP_RTM_GETANYCAST,
                                          IP_IFA_MAX, sizeof(struct Ip_ifaddrmsg),
                                          IP_NULL,
                                          ipnet_rtnetlink_addr_dump_family);
    ipnet_rtnetlink_register_family_event(IP_AF_INET, IP_RTM_NEWADDR,
                                          IP_IFA_MAX, sizeof(struct Ip_ifaddrmsg),
                                          ipnet_rtnetlink_addr_newaddr_family,
                                          IP_NULL);
    ipnet_rtnetlink_register_family_event(IP_AF_INET, IP_RTM_DELADDR,
                                          IP_IFA_MAX, sizeof(struct Ip_ifaddrmsg),
                                          ipnet_rtnetlink_addr_deladdr_family,
                                          IP_NULL);
    ipnet_rtnetlink_register_family_event(IP_AF_INET, IP_RTM_GETROUTE,
                                          IP_RTA_MAX, sizeof(struct Ip_rtmsg),
                                          ipnet_rtnetlink_route_getroute_family,
                                          ipnet_rtnetlink_route_dump_family);
    ipnet_rtnetlink_register_family_event(IP_AF_INET, IP_RTM_NEWROUTE,
                                          IP_RTA_MAX, sizeof(struct Ip_rtmsg),
                                          ipnet_rtnetlink_route_newroute_family,
                                          IP_NULL);
    ipnet_rtnetlink_register_family_event(IP_AF_INET, IP_RTM_DELROUTE,
                                          IP_RTA_MAX, sizeof(struct Ip_rtmsg),
                                          ipnet_rtnetlink_route_delroute_family,
                                          IP_NULL);
    ipnet_rtnetlink_register_family_event(IP_AF_INET, IP_RTM_GETRULE,
                                          IP_FRA_MAX, sizeof(struct Ip_fib_rule_hdr),
                                          IP_NULL,
                                          ipnet_rtnetlink_rule_dump_family);
    ipnet_rtnetlink_register_family_event(IP_AF_INET, IP_RTM_NEWRULE,
                                          IP_FRA_MAX, sizeof(struct Ip_fib_rule_hdr),
                                          ipnet_rtnetlink_rule_newrule_family,
                                          IP_NULL);
    ipnet_rtnetlink_register_family_event(IP_AF_INET, IP_RTM_DELRULE,
                                          IP_FRA_MAX, sizeof(struct Ip_fib_rule_hdr),
                                          ipnet_rtnetlink_rule_delrule_family,
                                          IP_NULL);
    ipnet_rtnetlink_register_family_event(IP_AF_INET, IP_RTM_GETNEIGH,
                                          IP_NDA_MAX, sizeof(struct Ip_ndmsg),
                                          IP_NULL,
                                          ipnet_rtnetlink_neigh_dump_family);
    ipnet_rtnetlink_register_family_event(IP_AF_INET, IP_RTM_DELNEIGH,
                                          IP_NDA_MAX, sizeof(struct Ip_ndmsg),
                                          ipnet_rtnetlink_neigh_delneigh_family,
                                          IP_NULL);
    ipnet_rtnetlink_register_family_event(IP_AF_INET, IP_RTM_NEWNEIGH,
                                          IP_NDA_MAX, sizeof(struct Ip_ndmsg),
                                          ipnet_rtnetlink_neigh_newneigh_family,
                                          IP_NULL);
}


/*
 *==========================================================================
 *                    ipnet_rtnetlink_ip4_addr_dump
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
ipnet_rtnetlink_ip4_addr_dump(Ipnet_netlink_mem_t *mem,
                              struct Ip_nlmsghdr  *nlmsg,
                              int                 req_addr_type,
                              Ip_ptrdiff_t        *args)
{
    Ip_ptrdiff_t              *done         = &args[0];
    Ip_ptrdiff_t              *s_addr_ix    = &args[1];
    Ip_ptrdiff_t              *s_if_ix      = &args[2];
    Ip_bool                   is_primary;
    Ip_ptrdiff_t              addr_ix = 0;
    Ip_u32                    if_ix = 0;
    Ipnet_netif               *netif;
    Ipnet_ip4_addr_entry      *addr_it;
    Ipnet_ppp_peer            *p;
    int                       ret = 0;
    unsigned                  i;
    struct Ip_in_addr         *peer;
    unsigned                  sifindex      = *s_if_ix;



    if (*done)
        goto exit_out;


    IPNET_NETIF_FOR_EACH(netif, i)
    {
        addr_ix     = 0;
        if_ix       = netif->ipcom.ifindex;
        is_primary  = IP_TRUE;

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

        addr_it = netif->inet4_addr_list;
        peer    = p ? &p->peer4 : IP_NULL;

        while (addr_it)
        {
            if (addr_ix >= *s_addr_ix)
            {
                if (addr_it->type == req_addr_type)
                {
                    ret = ipnet_rtnetlink_ip4_addr_fillinfo(mem,
                                                            addr_it,
                                                            peer,
                                                            netif,
                                                            is_primary,
                                                            nlmsg->nlmsg_seq,
                                                            IP_RTM_NEWADDR,
                                                            IP_NLM_F_MULTI);
                    is_primary = IP_FALSE;

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
 *                    ipnet_rtnetlink_ip4_neigh_fill_info
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
ipnet_rtnetlink_ip4_neigh_fill_info(Ipnet_netlink_mem_t *mem,
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
    int                       type = ipnet_ip4_get_addr_type2(*(Ip_u32*)rt->hdr.key,
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
    ndm->ndm_family  = IP_AF_INET;
    ndm->ndm_type    = IP_RTN_UNICAST;
    ndm->ndm_state   = ipnet_rtnetlink_neigh_nc2nud(ne->state, rt->metrics.rmx_expire);
    ndm->ndm_ifindex = rt->netif ? rt->netif->ipcom.ifindex : 0;
    ndm->ndm_flags   = IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_PROTO2) ? IP_NTF_PROXY : 0;


    /* Put destination address */
    if (!ipnet_rtnetlink_rta_sz_put(mem, IP_NDA_DST, sizeof(struct Ip_in_addr), rt->hdr.key))
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
 *                    ipnet_rtnetlink_ip4_route_fill_info
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
ipnet_rtnetlink_ip4_route_fill_info(Ipnet_netlink_mem_t *mem,
                                    Ipnet_route_entry   *rt,
                                    int                 seq,
                                    Ip_u8               event,
                                    Ip_u32              eflags)
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
    const struct Ip_in_addr    *addr = (struct Ip_in_addr*)rt->hdr.key;


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

    if (IP_IN_CLASSD(addr->s_addr))
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
                struct Ip_in_addr *gw = &((struct Ip_sockaddr_in *)mrt->gateway)->sin_addr;

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
             && (rt->gateway->sa_family == IP_AF_INET)))
        {
            addr = &((struct Ip_sockaddr_in *)rt->gateway)->sin_addr;

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
    addr = ipnet_ip4_get_src_addr(IPNET_ROUTE_GET_VR(rt->head), rt->hdr.key, rt, IP_NULL);
    if (addr)
    {
        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_RTA_PREFSRC, sizeof(*addr), addr))
            return -1;
    }

    /* Done */
    return ipnet_netlink_nlmsg_sz_end(mem, nlh);
}




/*
 *===========================================================================
 *                    ipnet_rtnetlink_ip4_getroute
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
ipnet_rtnetlink_ip4_getroute(Ipnet_netlink_mem_t *mem, struct Ip_nlmsghdr *nlh, struct Ip_rtattr **rta)
{
    Ip_u16              vr;
    Ip_u32              table   = IPCOM_ROUTE_TABLE_DEFAULT;
    struct Ip_rtmsg     *rtm    = IP_NLMSG_DATA(nlh);
    struct Ip_in_addr   addr;
    unsigned int        scope_id = 0;
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

    ret = ipnet_rtnetlink_ip4_route_fill_info(mem,
                                              rt_entry,
                                              nlh->nlmsg_seq,
                                              IP_RTM_NEWROUTE,
                                              0);

    return ret < 0? -IP_ERRNO_ENOMEM : ret;
}

/*
 *===========================================================================
 *                    ipnet_rtnetlink_ip4_newaddr
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
ipnet_rtnetlink_ip4_newaddr(Ipnet_netlink_mem_t *mem, struct Ip_nlmsghdr *nlh, struct Ip_rtattr **rta)
{
    Ip_u16                      vr;
    Ip_u32                      flags = 0;
    Ipnet_netif                 *netif;
    Ipnet_ppp_peer              *p;
    int                         ret = -1;
    struct Ip_ifa_cacheinfo     *ifc = IP_NULL;
    struct Ip_ifaddrmsg         *ifm = IP_NLMSG_DATA(nlh);


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
        Ip_u32 addr = 0;
        Ip_u32 addr_anycast = 0;
        Ip_u32 mask = 0;
        struct Ip_in_addr peer = {0};

        if (ifm->ifa_prefixlen > 32 || !rta[IP_IFA_LOCAL-1])
            return -IP_ERRNO_EINVAL;

        if (rta[IP_IFA_ADDRESS-1])
        {
            if (IP_RTA_PAYLOAD(rta[IP_IFA_ADDRESS-1]) < sizeof(peer))
                return -IP_ERRNO_EINVAL;

            ipcom_memcpy(&peer, IP_RTA_DATA(rta[IP_IFA_ADDRESS-1]), sizeof(peer));
        }

        if (rta[IP_IFA_LOCAL-1])
        {
            if (IP_RTA_PAYLOAD(rta[IP_IFA_LOCAL-1]) < sizeof(addr))
                return -IP_ERRNO_EINVAL;

            ipcom_memcpy(&addr, IP_RTA_DATA(rta[IP_IFA_LOCAL-1]), sizeof(addr));
        }

        if (rta[IP_IFA_ANYCAST-1])
        {
            if (IP_RTA_PAYLOAD(rta[IP_IFA_ANYCAST-1]) < sizeof(addr_anycast))
                return -IP_ERRNO_EINVAL;

            ipcom_memcpy(&addr_anycast, IP_RTA_DATA(rta[IP_IFA_ANYCAST-1]), sizeof(addr_anycast));
        }

        if (ifm->ifa_prefixlen)
            mask = ip_htonl(~((1u<<(32-ifm->ifa_prefixlen))-1));

        if (rta[IP_IFA_X_INFO - 1])
        {
            Ip_u32 xflags = IP_GET_32ON8(IP_RTA_DATA(rta[IP_IFA_X_INFO - 1]));

            if (IP_BIT_ISSET(xflags, IP_IFA_F_X_AUTOMATIC))
                IP_BIT_SET(flags, IPNET_IP4_ADDR_FLAG_AUTOMATIC);
            if (IP_BIT_ISSET(xflags, IP_IFA_F_X_PREFERRED))
                IP_BIT_SET(flags, IPNET_IP4_ADDR_FLAG_PREFERRED);
        }

        if (IP_BIT_ISSET(ifm->ifa_flags, IP_IFA_F_HOMEADDRESS))
            IP_BIT_SET(flags, IPNET_IP4_ADDR_FLAG_HOMEADDRESS);

        addr = addr ? addr : peer.s_addr;

        /* If peer address is specified, check that interface supports it */
        if (peer.s_addr != IP_INADDR_ANY && peer.s_addr != addr && p == IP_NULL)
            return -IP_ERRNO_EINVAL;

        ret = ipnet_ip4_add_addr2(netif,
                                  addr,
                                  mask,
                                  flags,
                                  IPNET_ADDR_TYPE_UNICAST);

        if (ret == 0 && peer.s_addr != addr && peer.s_addr != IP_INADDR_ANY)
            ret = ipnet_if_set_ipv4_ppp_peer(p, &peer);
        else if (ret == 0 && addr_anycast != IP_INADDR_ANY)
            ret = ipnet_ip4_add_addr2(netif,
                                      addr_anycast,
                                      mask,
                                      0,
                                      IPNET_ADDR_TYPE_ANYCAST);
        if (ret < 0)
            (void)ipnet_ip4_remove_addr(netif, addr);
    }

    /*
    ipnet_softirq_wait_for_pending();
    */
    return ret;
}

/*
 *===========================================================================
 *                    ipnet_rtnetlink_ip4_deladdr
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
ipnet_rtnetlink_ip4_deladdr(Ipnet_netlink_mem_t *mem, struct Ip_nlmsghdr *nlh, struct Ip_rtattr **rta)
{
    Ipnet_netif             *netif;
    Ip_u16                  vr;
    int                     ret = -IP_ERRNO_EINVAL;
    Ipnet_ppp_peer          *p;
    struct Ip_ifaddrmsg     *ifm = IP_NLMSG_DATA(nlh);

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
        Ip_u32 addr = 0;
        Ip_u32 addr_anycast = 0;
        struct Ip_in_addr peer = {0};

        if (rta[IP_IFA_ADDRESS-1])
        {
            if (IP_RTA_PAYLOAD(rta[IP_IFA_ADDRESS-1]) < sizeof(peer))
                return -IP_ERRNO_EINVAL;

            ipcom_memcpy(&peer,IP_RTA_DATA(rta[IP_IFA_ADDRESS-1]),sizeof(peer));
        }

        if (rta[IP_IFA_LOCAL-1])
        {
            if (IP_RTA_PAYLOAD(rta[IP_IFA_LOCAL-1]) < sizeof(addr))
                return -IP_ERRNO_EINVAL;

            ipcom_memcpy(&addr, IP_RTA_DATA(rta[IP_IFA_LOCAL-1]),sizeof(addr));
        }

        if (rta[IP_IFA_ANYCAST-1])
        {
            if (IP_RTA_PAYLOAD(rta[IP_IFA_ANYCAST-1]) < sizeof(addr_anycast))
                return -IP_ERRNO_EINVAL;

            ipcom_memcpy(&addr, IP_RTA_DATA(rta[IP_IFA_ANYCAST-1]),sizeof(addr_anycast));
        }

        addr = addr ? addr : peer.s_addr;

        /* If peer address is specified, check that interface supports it */
        if (peer.s_addr != IP_INADDR_ANY && peer.s_addr != addr && p == IP_NULL)
            return -IP_ERRNO_EINVAL;

        if (addr != IP_INADDR_ANY)
            ret = ipnet_ip4_remove_addr(netif, addr);

        if ((ret == 0)
            && (peer.s_addr != addr)
            && (peer.s_addr != IP_INADDR_ANY))
        {
            peer.s_addr = 0;
            ret = ipnet_if_set_ipv4_ppp_peer(p, &peer);
        }
#if 0 /* addr_anycast is always 0, thus, code will never be executed */
        if (ret == 0 && addr_anycast != IP_INADDR_ANY)
            (void)ipnet_ip4_remove_addr(netif, addr_anycast);
#endif

    }
    return ret;
}

/*
 *===========================================================================
 *                    ipnet_rtnetlink_ip4_route_key_setup
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_ip4_route_key_setup(Ipnet_rtnetlink_key_t *ukey,
                                    Ip_u32                ifindex,
                                    void                  **maskp,
                                    int                   mask_len,
                                    struct Ip_rtattr      *dst)
{
    Ip_bool have_mask = IP_FALSE;

    (void)ifindex;

    ipcom_memset(ukey, 0, sizeof(*ukey));

    if (mask_len != -1 && mask_len < (signed) (sizeof(ukey->v4.key.addr) * 8))
    {
        /* Mask valid */
        ipnet_route_create_mask(&ukey->v4.mask.addr, mask_len);
        have_mask = IP_TRUE;
    }

    if (dst)
    {
        if (IP_RTA_PAYLOAD(dst) < sizeof(struct Ip_in_addr))
            return -IP_ERRNO_EINVAL;

        ipcom_memcpy(&ukey->v4.key.addr, IP_RTA_DATA(dst), sizeof(struct Ip_in_addr));

        if (have_mask)
            ipnet_route_apply_mask(&ukey->v4.key.addr, &ukey->v4.mask.addr, sizeof(struct Ip_in_addr) * 8);
    }

    /* Store mask */
    if (maskp != IP_NULL && have_mask)
        *maskp = &ukey->v4.mask;
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_rtnetlink_ip4_route_gw_setup
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_ip4_route_gw_setup(Ip_u32                     ifindex,
                                   union Ip_sockaddr_union    *ugw,
                                   struct Ip_sockaddr         **gw,
                                   struct Ip_rtattr           *rta)
{
    struct Ip_sockaddr         *gw4 = IP_NULL;

    (void)ifindex;

    ipcom_memset(ugw, 0, sizeof(*ugw));

    ugw->sin.sin_family   = IP_AF_INET;
    IPCOM_SA_LEN_SET(&ugw->sin, sizeof(struct Ip_sockaddr_in));

    if (rta)
    {
        if (IP_RTA_PAYLOAD(rta) < sizeof(struct Ip_in_addr))
            return -IP_ERRNO_EINVAL;

        ipcom_memcpy(&ugw->sin.sin_addr.s_addr, IP_RTA_DATA(rta), sizeof(struct Ip_in_addr));

        gw4 = &ugw->sa;
    }

    if (gw)
        *gw = gw4;

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_rtnetlink_ip4_neigh_setup
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_ip4_neigh_setup(Ipnet_rtnetlink_key_t *ukey,
                                struct Ip_ndmsg       *ndm,
                                Ipnet_route_entry     *rt,
                                Ipnet_nd_state_t      nd_state,
                                void                  *link_addr,
                                struct Ip_rtattr      **rta)
{
#ifdef IPCOM_STORE_PHYSICAL_IF
    Ipnet_arp_entry *ac = IPNET_ARP(rt);
    if (rta[IP_NDA_REALIFINDEX - 1])
        ac->ne.physical_ifindex = IP_GET_32ON8(IP_RTA_DATA(rta[IP_NDA_REALIFINDEX - 1]));
#else
    (void)rta;
#endif

    ipnet_arp_set_state(rt, nd_state, link_addr);
    if (link_addr != IP_NULL)
    {
        if (IP_BIT_ISSET(ndm->ndm_flags, IP_NTF_GRAT))
            ipnet_arp_request(rt->netif,
                              ukey->v4.key.addr.s_addr,
                              IP_TRUE,
                              link_addr,
                              IP_NULL);
    }

    return 0;
}

IP_GLOBAL void
ipnet_rtnetlink_ip4_addr_add(struct Ipnet_netif_struct *netif,
                             struct Ipnet_ip4_addr_entry_struct *addr)
{
#ifdef IP_PORT_LKM
    ipcom_lkm_notify_address_event(netif, IP_AF_INET, addr, IPCOM_LKM_ADDR_ADD);
#endif
    ipnet_rtnetlink_ip4_addr_event(netif, addr, IP_RTM_NEWADDR);
}

IP_GLOBAL void
ipnet_rtnetlink_ip4_addr_del(struct Ipnet_netif_struct *netif,
                             struct Ipnet_ip4_addr_entry_struct *addr)
{
#ifdef IP_PORT_LKM
    ipcom_lkm_notify_address_event(netif, IP_AF_INET, addr, IPCOM_LKM_ADDR_DEL);
#endif
    ipnet_rtnetlink_ip4_addr_event(netif, addr, IP_RTM_DELADDR);
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */


#endif /* defined(IPCOM_USE_INET) && defined(IPNET_USE_NETLINKSOCK) */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
