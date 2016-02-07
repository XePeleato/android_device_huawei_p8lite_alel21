/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_rtnetlink_route.c,v $ $Revision: 1.8 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_rtnetlink_route.c,v $
 *   $Author: jonas $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation Jonas Rendel <Jonas.Rendel@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Contains netlink handlers for managing Ipv4 and Ipv6 routes as well as
 * MPLS shortcut routes.
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

#ifdef IPNET_USE_NETLINK
#define IPCOM_USE_CLIB_PROTO
#include <ipcom.h>
#include <ipcom_type.h>
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_sock.h>
#include <ipcom_sock6.h>
#include <ipcom_sock2.h>
#include <ipcom_route.h>
#include <ipcom_errno.h>

#include "ipnet_h.h"
#include "ipnet_ctrl.h"
#include "ipnet_route.h"
#include "ipnet_netlink_h.h"
#include "ipnet_rtnetlink_h.h"
#include "ipcom_sock.h"
#include "ipnet_loopback.h"

#ifdef IP_PORT_LKM
#include "ipcom_lkm.h" /* Needed for Linux PPP interface config */
#endif /* IP_PORT_LKM */

#include "ipnet_eth.h"

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */
#define IP_IN4_IS_ADDR_MULTICAST(x) (((x) & ip_htonl(0xf0000000)) == ip_ntohl (0xe0000000))

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

typedef struct Ipnet_rtnetlink_route_family
{
    struct
    {
        Ip_ptrdiff_t    *vr;
        Ip_ptrdiff_t    *table;
        Ip_ptrdiff_t    *rt;
        Ip_ptrdiff_t    *done;
        int             aborted;
        int             cont;
    } state;

    /* Requested */
    struct Ip_nlmsghdr  *nlmsg;
    int                 table;               /* Caller specified table    */
    Ip_u8               family;

    /* Buffering */
    Ipnet_netlink_mem_t *mem;
}
Ipnet_rtnetlink_route_family_t;

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
/*
 *===========================================================================
 *                    ipnet_rtnetlink_route_foreach_vr
 *===========================================================================
 * Description: Hash table foreach callback. Called once for each virtual
 *              router that exists in the system.
 * Parameters:  rtab  - route table
 *              param - callback parameters.
 * Returns:     -
 *
 */
IP_STATIC void
ipnet_rtnetlink_route_foreach_vr_family(Ipcom_route *rtab, Ipnet_rtnetlink_route_family_t *param)
{
    int                new_offs = 0;
    Ip_ptrdiff_t       table    = IPNET_ROUTE_GET_TABLE(rtab);
    Ip_u16             vr       = IPNET_ROUTE_GET_VR(rtab);
    Ipnet_route_entry  *rt      = IP_NULL;

    /* This traversal is already aborted */
    if (param->state.aborted)
        return;

    /* Only handle rtabs of selected address family */
    if (IPNET_ROUTE_GET_FAMILY(rtab) != param->family)
        return;

    /* Check if specific VR shall be dumped */
    if (IP_BIT_ISFALSE(param->nlmsg->nlmsg_flags, IP_NLM_F_VR_UNSPEC) && (param->mem->vr != IPCOM_VR_ANY) && (param->mem->vr != vr))
        return;

    /*
     * If this is a trailer message buffer, make sure it
     * starts dumping where the last message ended
     */
    if (param->state.cont)
    {
        Ipnet_route_entry  *rtt = (Ipnet_route_entry*)(*param->state.rt);

        /* Verify current state */
        if (*param->state.vr != vr || *param->state.table != table)
            return;

        if (rtt != IP_NULL)
        {
            rt = ipnet_route_first_entry(IPNET_ROUTE_GET_FAMILY(rtab),
                                         vr,
                                         table);
            while (rt != IP_NULL)
            {
                if (rt == rtt)
                    break;
                rt = ipnet_route_next_entry(rt);
            }
        }
    }


    /* Reset the continue variable; we're on the move again */
    param->state.cont = 0;

    /* Get first unless we've found one to continue from */
    if (rt == IP_NULL)
        rt = ipnet_route_first_entry(IPNET_ROUTE_GET_FAMILY(rtab),
                                     vr,
                                     table);

    /* */
    while (rt)
    {
        /* Do not dump the 255.255.255.255, 224.0.0.0/4 or IPv6 multicast routes. */
        /* Do not dump routes that are part of multipath routes either */
        if (IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_X_MCAST_RO | IPNET_RTF_X_BCAST_RO | IPNET_RTF_X_HIDDEN)
            && (rt->prev == IP_NULL))
        {
#ifdef IPCOM_USE_INET
            if (param->family == IP_AF_INET)
            {
                new_offs = ipnet_rtnetlink_ip4_route_fill_info (param->mem,
                                                                rt,
                                                                param->nlmsg->nlmsg_seq,
                                                                IP_RTM_NEWROUTE,
                                                                IP_NLM_F_MULTI);
            }
            else
#endif
#ifdef IPCOM_USE_INET6
            if (param->family == IP_AF_INET6)
            {
                new_offs = ipnet_rtnetlink_ip6_route_fill_info (param->mem,
                                                                rt,
                                                                param->nlmsg->nlmsg_seq,
                                                                IP_RTM_NEWROUTE,
                                                                IP_NLM_F_MULTI);
            }
            else
#endif
            {
                IP_PANIC();
            }

            if (new_offs < 0)
                goto aborted;
        }

        rt = ipnet_route_next_entry(rt);

        /* */
        if (rt && new_offs)
            goto aborted;
    }

    return;

aborted:
    param->state.aborted = 1;
    *param->state.rt     = (Ip_ptrdiff_t)rt;
    *param->state.table  = (Ip_ptrdiff_t)table;
    *param->state.vr     = (Ip_ptrdiff_t)vr;
}

/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_rtnetlink_route_newroute_family
 *===========================================================================
 * Description: Add a new rotue via NETLINK.
 * Parameters:  data - Message payload
 *              len  - Length of message.
 *              nlh  - NETLINK message header
 *              arg  - Reference to route attributes.
 *
 * Returns:     0 - Success
 *             <0 - Failure
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_route_newroute_family(Ipnet_netlink_mem_t   *mem,
                                      struct Ip_nlmsghdr    *nlh,
                                      struct Ip_rtattr      **rta,
                                      int                   family)
{
    Ipnet_netif                   *netif = IP_NULL;
    Ipnet_ppp_peer                *p = IP_NULL;
    int                           ret  = -1;
    Ip_u16                        vr;
    Ip_u32                        table;
    Ip_bool                       is_multipath_route = IP_FALSE;
    Ip_ssize_t                    mpa_size = 0;
    struct Ip_rtnexthop           *nh = IP_NULL;
    struct Ipnet_rt_metrics       metrics;
    struct Ip_sockaddr            *gw   = IP_NULL;
    void                          *mask = IP_NULL;
    struct Ip_sockaddr_dl         gw_dl;
    struct Ipnet_route_add_param  param;
    struct Ip_rtmsg               *rtm   = IP_NLMSG_DATA(nlh);

    union Ip_sockaddr_union       ugw;
    Ipnet_rtnetlink_key_t         ukey;

#ifdef IPMPLS
    struct Ip_sockaddr_mpls       gw_mpls;
#endif /* IPMPLS */
#ifdef IPNET_USE_ROUTE_COOKIES
    Ipnet_rt_cookie               cookie;
#endif /* IPNET_USE_ROUTE_COOKIES */

    table = IPCOM_ROUTE_TABLE_GET(rtm->rtm_table);

    /* Prepare route parameters */
    ipcom_memset(&param, 0, sizeof(param));
    ipcom_memset(&metrics,0,sizeof(metrics));
    param.flags = IPNET_RTF_STATIC | IPNET_RTF_UP | IPNET_RTF_DONE;

#ifdef IPNET_USE_ROUTE_TABLE_NAMES
    if (rta[IP_RTA_TABLE_NAME -1])
    {
        if (ipnet_route_vr_and_table_from_name(IP_RTA_DATA(rta[IP_RTA_TABLE_NAME - 1]),
                                               &vr,
                                               &table) < 0)
            return -IP_ERRNO_ESRCH;
    }
    else
#endif
    {
        if (mem->vr == IPCOM_VR_ANY)
            return -IP_ERRNO_EINVAL;

        vr = ipnet_rtnetlink_vr(rta[IP_RTA_VR - 1], mem->vr);

        if (rta[IP_RTA_TABLE - 1])
            table = IP_GET_32ON8(IP_RTA_DATA(rta[IP_RTA_TABLE - 1]));
    }

    if (rta[IP_RTA_OIF -1])
    {
        int ifindex = 0;

        if (rta[IP_RTA_OIF-1]->rta_len != IP_RTA_LENGTH(sizeof(int)))
			return -IP_ERRNO_EINVAL;

        ifindex = IP_GET_32ON8(IP_RTA_DATA(rta[IP_RTA_OIF-1]));
        netif = ipnet_if_indextonetif(vr, ifindex);
        if (netif)
            p = netif->private_data;
    }

    if (rta[IP_RTA_MULTIPATH -1])
    {
        is_multipath_route = IP_TRUE;
        nh = IP_RTA_DATA(rta[IP_RTA_MULTIPATH-1]);
        mpa_size = IP_RTA_PAYLOAD(rta[IP_RTA_MULTIPATH-1]);

        if ((nh == IP_NULL) || (mpa_size == 0))
            return -IP_ERRNO_EINVAL;
    }

    if ((ret = ipnet_rtnetlink_route_key_setup(family, &ukey, netif? netif->ipcom.ifindex : 0, &mask, rtm->rtm_dst_len, rta[IP_RTA_DST-1])) < 0)
        return ret;

    if ((ret = ipnet_rtnetlink_route_gw_setup(family, netif? netif->ipcom.ifindex : 0, &ugw, &gw, rta[IP_RTA_GATEWAY-1])) < 0)
        return ret;

#ifdef IPCOM_USE_INET
    if (family == IP_AF_INET)
    {
        if (gw != IP_NULL)
        {
            /* Check if it is a pure gateway or not */
            if (p == IP_NULL || p->peer4.s_addr != ugw.sin.sin_addr.s_addr)
                param.flags |= IPNET_RTF_GATEWAY;
        }
    }
    else
#endif
#ifdef IPCOM_USE_INET6
    if (family == IP_AF_INET6)
    {
        if (gw != IP_NULL)
        {
            /* Check if it is a pure gateway or not */
            if (p == IP_NULL || !IP_IN6_ARE_ADDR_EQUAL(&ugw.sin6.sin6_addr, &p->peer6))
                param.flags |= IPNET_RTF_GATEWAY;
        }
    }
    else
#endif
    {
        return -IP_ERRNO_EAFNOSUPPORT;
    }

    if (!mask)
        param.flags |= IPNET_RTF_HOST;

    if (rtm->rtm_type == IP_RTN_PROXY)
    {
        /* This is a proxy arp network route */
        IP_BIT_CLR(param.flags, IPNET_RTF_CLONING);
        IP_BIT_SET(param.flags, IPNET_RTF_PROTO2);

        if (rta[IP_RTA_PROXY_ARP_LLADDR - 1])
        {
            /* This is a network proxy arp with specific lladdr */
            if (netif == IP_NULL)
               return -IP_ERRNO_EINVAL;

            ipcom_memset(&gw_dl, 0, sizeof(struct Ip_sockaddr_dl));

            IPCOM_SA_LEN_SET(&gw_dl, sizeof(struct Ip_sockaddr_dl));
            gw_dl.sdl_family = IP_AF_LINK;
            gw_dl.sdl_index  = (Ip_u16)netif->ipcom.ifindex;
            gw_dl.sdl_alen   = (Ip_u8) netif->ipcom.link_addr_size;
            gw_dl.sdl_type   = (Ip_u8) netif->ipcom.type;

            ipcom_memcpy(IP_SOCKADDR_DL_LLADDR(&gw_dl),
                         IP_RTA_DATA(rta[IP_RTA_PROXY_ARP_LLADDR - 1]),
                         IPNET_ETH_ADDR_SIZE);

            gw = (struct Ip_sockaddr *) &gw_dl;
            IP_BIT_SET(param.flags, IPNET_RTF_LLINFO);
        }
    }
    else if (rtm->rtm_type == IP_RTN_PROHIBIT)
        IP_BIT_SET(param.flags, IPNET_RTF_REJECT);
    else if (rtm->rtm_type == IP_RTN_THROW)
        IP_BIT_SET(param.flags, IPNET_RTF_SKIP);
    else if (rtm->rtm_type == IP_RTN_UNREACHABLE)
    {
        if (netif == IP_NULL)
            netif = ipnet_loopback_get_netif(vr);
        IP_BIT_CLR(param.flags, IPNET_RTF_UP);
    }
    else if (rtm->rtm_type == IP_RTN_CLONE)
        IP_BIT_SET(param.flags, IPNET_RTF_CLONING);
    else if (rtm->rtm_type == IP_RTN_BLACKHOLE)
        IP_BIT_SET(param.flags, IPNET_RTF_BLACKHOLE);

#ifdef IPMPLS
    /* Check for IPNET MPLS pseudo-wire route */
    if (rta[IP_RTA_NH_PROTO-1])
    {
        Ip_u32 nh_type;

        if (rta[IP_RTA_NH_PROTO-1]->rta_len != IP_RTA_LENGTH(sizeof(Ip_u32)))
            return -IP_ERRNO_EINVAL;

        if (rta[IP_RTA_NH_PROTO_DATA-1]->rta_len != IP_RTA_LENGTH(sizeof(Ip_u32)))
            return -IP_ERRNO_EINVAL;

        ipcom_memcpy(&nh_type, IP_RTA_DATA(rta[IP_RTA_NH_PROTO-1]), sizeof(Ip_u32));

        if (nh_type !=  IPNET_ETH_P_MPLS_UNICAST)
            return -IP_ERRNO_EINVAL;

        ipcom_memset(&gw_mpls, 0, sizeof(gw_mpls));
        gw_mpls.smpls_family = IP_AF_MPLS;
        IPCOM_SA_LEN_SET(&gw_mpls, sizeof (struct Ip_sockaddr_mpls));

        ipcom_memcpy(&gw_mpls.smpls_key,
                     IP_RTA_DATA(rta[IP_RTA_NH_PROTO_DATA-1]),
                     sizeof(Ip_u32));

        IP_BIT_CLR(param.flags,IPNET_RTF_GATEWAY);
        gw = (struct Ip_sockaddr *)&gw_mpls;
    }
    else
    {
        /* Check if cloning flag shall be set */
        if (IP_BIT_ISFALSE(param.flags, IPNET_RTF_HOST | IPNET_RTF_GATEWAY | IPNET_RTF_REJECT | IPNET_RTF_BLACKHOLE))
            IP_BIT_SET(param.flags, IPNET_RTF_CLONING);
    }
#endif

    if (rta[IP_RTA_METRICS-1])
    {
        int rlen = (int)IP_RTA_PAYLOAD(rta[IP_RTA_METRICS-1]);
        Ip_u32 dummy = 0;
        struct Ip_rtattr *rtax = (struct Ip_rtattr*)IP_RTA_DATA(rta[IP_RTA_METRICS-1]);

        metrics.rmx_expire = IPCOM_ADDR_INFINITE;
        param.metrics      = &metrics;

        for(;rlen > 0; rlen -= (int)rtax->rta_len,rtax = IP_RTA_NEXT(rtax,dummy))
        {
            switch (rtax->rta_type)
            {
            case IP_RTAX_MTU:
                ipcom_memcpy(&metrics.rmx_mtu, IP_RTA_DATA(rtax), sizeof(Ip_u32));
                break;
            case IP_RTAX_RTT:
                ipcom_memcpy(&metrics.rmx_rtt, IP_RTA_DATA(rtax), sizeof(Ip_u32));
                break;
            case IP_RTAX_RTTVAR:
                ipcom_memcpy(&metrics.rmx_rttvar, IP_RTA_DATA(rtax), sizeof(Ip_u32));
                break;
            }
        }
    }

#ifdef IPNET_USE_ROUTE_COOKIES
    if (rta[IP_RTA_COOKIE-1])
    {
        if (IP_RTA_PAYLOAD(rta[IP_RTA_COOKIE-1]) > sizeof(cookie))
            return -IP_ERRNO_EINVAL;

        ipcom_memset(&cookie, 0, sizeof(cookie));
        ipcom_memcpy(&cookie,
                     IP_RTA_DATA(rta[IP_RTA_COOKIE-1]),
                     IP_RTA_PAYLOAD(rta[IP_RTA_COOKIE-1]));

        param.cookie = &cookie;
    }
#endif /* IPNET_USE_ROUTE_COOKIES */

    while (1)
    {
        if (is_multipath_route && (gw == IP_NULL))
        {
            netif                = ipnet_if_indextonetif(vr, nh->rtnh_ifindex);
            metrics.rmx_hopcount = nh->rtnh_hops;
            metrics.rmx_expire   = IPCOM_ADDR_INFINITE;
            param.metrics        = &metrics;

            if (nh->rtnh_len > sizeof(struct Ip_rtnexthop))
            {
                /* Multihop route has gateway */
                IP_BIT_CLR(param.flags, IPNET_RTF_CLONING);
                IP_BIT_SET(param.flags, IPNET_RTF_GATEWAY);

                if ((ret = ipnet_rtnetlink_route_gw_setup(family, netif? netif->ipcom.ifindex : 0, &ugw, &gw, IP_RTNH_DATA(nh))) < 0)
                    return ret;
            }
            mpa_size -= IP_RTNH_ALIGN(nh->rtnh_len);
            if (mpa_size > 0)
                nh = IP_RTNH_NEXT(nh);
        }

        param.domain       = family;
        param.vr           = vr;
        param.table        = table;
        param.netif        = netif;
        param.key          = &ukey;
        param.netmask      = mask;
        param.gateway      = gw;
        param.pid          = mem->pid;
        param.seq          = nlh->nlmsg_seq;
        param.no_ref_count = IP_TRUE;

        /* Try to add the route */
        if (netif)
            IPNET_IF_LOCK(netif);

        ret = ipnet_route_add(&param);

        if (netif != IP_NULL)
            IPNET_IF_UNLOCK(netif);

        if (is_multipath_route)
        {
            gw = IP_NULL;

            if (ret < 0 && ret != -IP_ERRNO_EEXIST)
                goto rollback;

            if (mpa_size == 0)
            {
                ret = 0;
                break;
            }
            else if (mpa_size < 0)
            {
                /* Malformed packet */
                ret = -IP_ERRNO_EINVAL;
                goto rollback;
            }
        }
        else
            break;
    }

    return ret;

rollback:
    (void)ipnet_rtnetlink_route_delroute_family(mem, nlh, rta, family);
    return ret;
}

/*
 *===========================================================================
 *                    ipnet_rtnetlink_route_delroute_family
 *===========================================================================
 * Description: Delete a route via NETLINK.
 * Parameters:  data - Message payload
 *              len  - Length of message.
 *              nlh  - NETLINK message header
 *              arg  - Reference to route attributes.
 *
 * Returns:     0 - Success
 *             <0 - Failure
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_route_delroute_family(Ipnet_netlink_mem_t   *mem,
                                      struct Ip_nlmsghdr    *nlh,
                                      struct Ip_rtattr      **rta,
                                      int                   family)
{
    int                      ret = -1;
    Ip_u16                   vr;
    Ip_u32                   table;
    Ip_u32                   ifindex = 0;
    Ip_bool                  is_multipath_route = IP_FALSE;
    Ip_size_t                mpa_size = 0;
    struct Ip_rtnexthop      *nh = IP_NULL;
    struct Ip_sockaddr       *gw = IP_NULL;
    void                     *mask = IP_NULL;
    struct Ip_rtmsg          *rtm = IP_NLMSG_DATA(nlh);

    union Ip_sockaddr_union  ugw;
    Ipnet_rtnetlink_key_t    ukey;

#ifdef IPMPLS
    struct Ip_sockaddr_mpls  gw_mpls;
#endif /* IPMPLS */

    table = IPCOM_ROUTE_TABLE_GET(rtm->rtm_table);

#ifdef IPNET_USE_ROUTE_TABLE_NAMES
    if (rta[IP_RTA_TABLE_NAME -1])
    {
        if (ipnet_route_vr_and_table_from_name(IP_RTA_DATA(rta[IP_RTA_TABLE_NAME - 1]),
                                               &vr,
                                               &table) < 0)
            return -IP_ERRNO_ESRCH;
    }
    else
#endif
    {
        if (mem->vr == IPCOM_VR_ANY)
            return -IP_ERRNO_EINVAL;

        vr = ipnet_rtnetlink_vr(rta[IP_RTA_VR - 1], mem->vr);

        if (rta[IP_RTA_TABLE - 1])
            table = IP_GET_32ON8(IP_RTA_DATA(rta[IP_RTA_TABLE - 1]));
    }

    if (rta[IP_RTA_OIF -1])
    {
        if (rta[IP_RTA_OIF-1]->rta_len != IP_RTA_LENGTH(sizeof(Ip_u32)))
			return -IP_ERRNO_EINVAL;

        ifindex = IP_GET_32ON8(IP_RTA_DATA(rta[IP_RTA_OIF-1]));
     }

    if (rta[IP_RTA_MULTIPATH -1])
    {
        is_multipath_route = IP_TRUE;
        nh = IP_RTA_DATA(rta[IP_RTA_MULTIPATH-1]);
        mpa_size = IP_RTA_PAYLOAD(rta[IP_RTA_MULTIPATH-1]);

        if ((nh == IP_NULL) || (mpa_size == 0))
            return -IP_ERRNO_EINVAL;
    }

    if ((ret = ipnet_rtnetlink_route_key_setup(family, &ukey, ifindex, &mask, rtm->rtm_dst_len, rta[IP_RTA_DST-1])) < 0)
        return ret;

    if ((ret = ipnet_rtnetlink_route_gw_setup(family, ifindex, &ugw, &gw, rta[IP_RTA_GATEWAY-1])) < 0)
        return ret;

    if (rta[IP_RTA_GATEWAY-1])
        gw = &ugw.sa;

#ifdef IPMPLS
    if (rta[IP_RTA_NH_PROTO-1])
    {
        Ip_u32 nh_type;

        /* IPNET MPLS pseudo-wire route */
        if (rta[IP_RTA_NH_PROTO-1]->rta_len != IP_RTA_LENGTH(sizeof(Ip_u32)))
            return -IP_ERRNO_EINVAL;

        if (rta[IP_RTA_NH_PROTO_DATA-1]->rta_len != IP_RTA_LENGTH(sizeof(Ip_u32)))
            return -IP_ERRNO_EINVAL;

        ipcom_memcpy(&nh_type, IP_RTA_DATA(rta[IP_RTA_NH_PROTO-1]), sizeof(Ip_u32));

        if (nh_type !=  IPNET_ETH_P_MPLS_UNICAST)
            return -IP_ERRNO_EINVAL;

        ipcom_memset(&gw_mpls, 0, sizeof(gw_mpls));
        gw_mpls.smpls_family = IP_AF_MPLS;
        IPCOM_SA_LEN_SET(&gw_mpls, sizeof (struct Ip_sockaddr_mpls));
        ipcom_memcpy(&gw_mpls.smpls_key, IP_RTA_DATA(rta[IP_RTA_NH_PROTO_DATA-1]), sizeof(Ip_u32));

        gw = (struct Ip_sockaddr *)&gw_mpls;
    }
#endif

    while(1)
    {
        if (is_multipath_route)
        {
            ifindex = nh->rtnh_ifindex;

            if (nh->rtnh_len > sizeof(struct Ip_rtnexthop))
            {
                /* Multihop route has gateway */
                if ((ret = ipnet_rtnetlink_route_gw_setup(family, ifindex, &ugw, &gw, IP_RTNH_DATA(nh))) < 0)
                    return ret;
            }
        }

        /* Delete the route */
        ret = ipnet_route_delete2(family,
                                  vr,
                                  table,
                                  &ukey,
                                  mask,
                                  gw,
                                  ifindex,
                                  0,
                                  0,
                                  0);

        if (is_multipath_route)
        {
            mpa_size -= IP_RTNH_ALIGN(nh->rtnh_len);
            gw = IP_NULL;

            if (mpa_size > 0)
                nh = IP_RTNH_NEXT(nh);
            else
                break;
        }
        else
            break;

    }

    return is_multipath_route ? 0 : ret;
}

/*
 *===========================================================================
 *                   ipnet_rtnetlink_route_sz_fill_metrics
 *===========================================================================
 * Description: Fill route metrics as attributes in a NETLINK route message.
 * Parameters:  data    - Message buffer
 *              metrics - Pointer to IPNET metrics pointer.
 * Returns:     Length of added attributes
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_route_sz_fill_metrics(Ipnet_netlink_mem_t     *mem,
                                      struct Ipnet_rt_metrics *metrics,
                                      Ip_u32                  mtu)
{
    struct Ip_rtattr *rta;


    rta = ipnet_rtnetlink_rta_sz_nested_begin(mem, IP_RTA_METRICS);
    if (!rta)
        return -1;

    if (metrics->rmx_mtu > 0)
        mtu = metrics->rmx_mtu;

    if (!ipnet_rtnetlink_rta_sz_put(mem, IP_RTAX_MTU, sizeof(Ip_u32), &mtu))
        return -1;

    /* Add Round Trip Time */
    if (metrics->rmx_rtt > 0)
        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_RTAX_RTT, sizeof(Ip_u32), &metrics->rmx_rtt))
            return -1;

    /* Add RTT Variance */
    if (metrics->rmx_rttvar > 0)
        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_RTAX_RTTVAR, sizeof(Ip_u32), &metrics->rmx_rttvar))
            return -1;

    /* Terminate the nested RTA */
    ipnet_rtnetlink_rta_sz_nested_end(mem, rta);

    return 0;
}


/*
 *==========================================================================
 *                    ipnet_rtnetlink_route_dump_family
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
ipnet_rtnetlink_route_dump_family(Ipnet_netlink_mem_t             *mem,
                                  struct Ip_nlmsghdr              *nlmsg,
                                  int                             family,
                                  Ip_ptrdiff_t                    *args)

{
    Ipnet_rtnetlink_route_family_t      param;
    Ip_ptrdiff_t                        *done       = &args[0];
    Ip_ptrdiff_t                        *s_vr       = &args[1];
    Ip_ptrdiff_t                        *s_table    = &args[2];
    Ip_ptrdiff_t                        *s_rtm      = &args[3];
    Ip_ptrdiff_t                        *s_continue = &args[4];

    param.nlmsg     = nlmsg;
    param.table     = 0;
    param.family    = family;
    param.mem       = mem;

    if (*done)
        goto exit_out;

    param.state.aborted = 0;
    param.state.done    = done;
    param.state.rt      = s_rtm;
    param.state.table   = s_table;
    param.state.vr      = s_vr;
    param.state.cont    = *s_continue;

    /* */
    ipcom_hash_for_each(ipnet->rtabs,
                        (Ipcom_hash_foreach_cb_func)ipnet_rtnetlink_route_foreach_vr_family,
                        &param);

    /* Are we aborted? */
    *s_continue = param.state.aborted;

    /* If not aborted; we're done */
    if (!param.state.aborted)
        *done = 1;

exit_out:
    return param.mem->used;
}


/*
 *==========================================================================
 *                    ipnet_rtnetlink_getroute_family
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
ipnet_rtnetlink_route_getroute_family(Ipnet_netlink_mem_t  *mem,
                                      struct Ip_nlmsghdr   *nlh,
                                      struct Ip_rtattr     **rta,
                                      int                  family)
{
    switch (family)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        return ipnet_rtnetlink_ip4_getroute(mem, nlh, rta);
#endif
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        return ipnet_rtnetlink_ip6_getroute(mem, nlh, rta);
#endif
    default:
        return -IP_ERRNO_EAFNOSUPPORT;;
    }

}

/*
 *===========================================================================
 *                    ipnet_rtnetlink_route_key_setup
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_route_key_setup(int                   family,
                                Ipnet_rtnetlink_key_t *ukey,
                                Ip_u32                ifindex,
                                void                  **maskp,
                                int                   mask_len,
                                struct Ip_rtattr      *dst)
{
    int ret;
#ifdef IPCOM_USE_INET
    if (family == IP_AF_INET)
    {
        if ((ret = ipnet_rtnetlink_ip4_route_key_setup(ukey, ifindex, maskp, mask_len, dst)) < 0)
            return ret;
    }
    else
#endif
#ifdef IPCOM_USE_INET6
    if (family == IP_AF_INET6)
    {
        if ((ret = ipnet_rtnetlink_ip6_route_key_setup(ukey, ifindex, maskp, mask_len, dst)) < 0)
            return ret;
    }
    else
#endif
    {
        return -IP_ERRNO_EAFNOSUPPORT;
    }

    return 0;
}

/*
 *===========================================================================
 *                    ipnet_rtnetlink_route_gw_setup
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_route_gw_setup(int                        family,
                               Ip_u32                     ifindex,
                               union Ip_sockaddr_union    *ugw,
                               struct Ip_sockaddr         **gw,
                               struct Ip_rtattr           *rta)
{
    int ret;

#ifdef IPCOM_USE_INET
    if (family == IP_AF_INET)
    {
        if ((ret = ipnet_rtnetlink_ip4_route_gw_setup(ifindex, ugw, gw, rta)) < 0)
            return ret;
    }
    else
#endif
#ifdef IPCOM_USE_INET6
    if (family == IP_AF_INET6)
    {
        if ((ret = ipnet_rtnetlink_ip6_route_gw_setup(ifindex, ugw, gw, rta)) < 0)
            return ret;
    }
    else
#endif
    {
        return -IP_ERRNO_EAFNOSUPPORT;
    }

    return 0;
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
ipnet_rtnetlink_route_event(struct Ipnet_route_entry_struct *rt, int event)
{
    Ipcom_pkt               *pkt    = IP_NULL;
    Ipnet_netlink_mem_t     mem;
    int                     ret     = -1;
    int                     group;

    if (!ipnet_netlink_pkt_alloc(&pkt, IP_NULL, IP_TRUE, &mem, 512))
        return;

    /* Must explicitly switch to the correct VR here */
    pkt->vr_index = mem.vr = IPNET_ROUTE_GET_VR(rt->head);

#ifdef IPCOM_USE_INET
    if (IPNET_ROUTE_GET_FAMILY(rt->head) == IP_AF_INET)
    {
        ret = ipnet_rtnetlink_ip4_route_fill_info (&mem,
                                                   rt,
                                                   0,
                                                   event,
                                                   0);
        group = IP_RTNLGRP_IPV4_ROUTE;
    }
#endif
#ifdef IPCOM_USE_INET6
    if (IPNET_ROUTE_GET_FAMILY(rt->head) == IP_AF_INET6)
    {
        ret = ipnet_rtnetlink_ip6_route_fill_info (&mem,
                                                   rt,
                                                   0,
                                                   event,
                                                   0);
        group = IP_RTNLGRP_IPV6_ROUTE;
    }
#endif

    if (ret < 0)
        ipcom_pkt_free(pkt);
    else
    {
        pkt->end += ret;

        /* We're done; do report this pkt */
        ipnet_rtnetlink_broadcast(pkt, group);
    }
}


IP_GLOBAL void
ipnet_rtnetlink_route_add(Ipnet_route_entry *rt)
{
    if (ipnet_rtnetlink_neigh_is(rt))
        ipnet_rtnetlink_neigh_add(rt);
    else
    {
        ipnet_rtnetlink_route_event(rt, IP_RTM_NEWROUTE);
    }
}

IP_GLOBAL void
ipnet_rtnetlink_route_change(Ipnet_route_entry *rt)
{
    if (ipnet_rtnetlink_neigh_is(rt))
        ipnet_rtnetlink_neigh_change(rt);
    else
    {
        ipnet_rtnetlink_route_event(rt, IP_RTM_NEWROUTE);
    }
}

IP_GLOBAL void
ipnet_rtnetlink_route_del(Ipnet_route_entry *rt)
{
    if (ipnet_rtnetlink_neigh_is(rt))
        ipnet_rtnetlink_neigh_del(rt);
    else
    {
        ipnet_rtnetlink_route_event(rt, IP_RTM_DELROUTE);
    }
}

IP_GLOBAL void
ipnet_rtnetlink_route_resolve(Ipnet_route_entry *rt)
{
    ipnet_rtnetlink_route_event(rt, IP_RTM_GETROUTE);
}


IP_GLOBAL void
ipnet_rtnetlink_route_miss(Ipcom_route *rtab, IP_CONST void *key)
{
    /*!! TODO */
    (void)rtab;
    (void)key;
}

IP_GLOBAL void
ipnet_rtnetlink_route_redirect(Ipcom_route *rtab, Ip_u32 ifindex,
                               IP_CONST void *dst, IP_CONST void *new_gw)
{
    /*!! TODO */
    (void)rtab;
    (void)ifindex;
    (void)dst;
    (void)new_gw;
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */


#endif /* IPNET_USE_NETLINK */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
