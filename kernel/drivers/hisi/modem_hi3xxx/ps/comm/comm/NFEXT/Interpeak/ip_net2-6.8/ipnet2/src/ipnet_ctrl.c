/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_ctrl.c,v $ $Revision: 1.77.22.2 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_ctrl.c,v $
 *     $Author: kenneth $
 *     $State: Exp $ $Locker:  $o
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *      Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Functions for IP_PUBLIC int ipnet_ctrl(struct Ipnet_tag *tag) function.
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

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_err.h>
#include <ipcom_clib.h>

#include "ipnet_h.h"
#include "ipnet_ctrl.h"
#include "ipnet_cmd.h"

#ifdef IPTCP
#include <iptcp.h>
#endif

#ifdef IPSCTP
#include <ipsctp.h>
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

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_STATIC int ipnet_ctrl_sock(struct Ipnet_tag *tag);

#ifdef IPCOM_USE_INET6
IP_STATIC int ipnet_ctrl_ip6_nc(struct Ipnet_tag *tag);
IP_STATIC int ipnet_ctrl_ip6_prefix(struct Ipnet_tag *tag);
IP_STATIC int ipnet_ctrl_ip6_router(struct Ipnet_tag *tag);
#endif

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
 *                    ipnet_ctrl_sock
 *===========================================================================
 * Description: Get state of sockets.
 * Parameters:  tag - The tag structure fill in.
 * Returns:     0 >= success, <0 error code.
 *
 */
IP_STATIC int
ipnet_ctrl_sock(struct Ipnet_tag *tag)
{
    struct Ipnet_tag_sock_data *sock_tag;

    sock_tag = (struct Ipnet_tag_sock_data*) tag;

    if (tag->size < sizeof(*sock_tag))
    {
        tag->size = sizeof(*sock_tag);
        return -IP_ERRNO_ENOSPC;
    }

    if (tag->type == IPNET_CTRL_NEW_ENUM_SOCK_DATA)
        sock_tag->fd = 0;
    else if (tag->type == IPNET_CTRL_CONT_ENUM_SOCK_DATA)
    {
        if (sock_tag->fd < 0 || sock_tag->fd >= ipnet_conf_max_sockets)
            return -IP_ERRNO_EINVAL;
        sock_tag->fd++;
    }
    else
        return -IP_ERRNO_EINVAL;

    for (; sock_tag->fd < ipnet_conf_max_sockets; sock_tag->fd++)
    {
        Ipnet_socket *sock = ipnet->sockets[sock_tag->fd];
        if (sock != IP_NULL)
        {
            sock_tag->pid       = sock->ipcom.pid;
            sock_tag->domain    = sock->ipcom.domain;
            sock_tag->type      = sock->ipcom.type;
            sock_tag->protocol  = sock->proto;
            sock_tag->vr_index  = sock->vr_index;
            sock_tag->snd_bytes = ipcom_atomic_get(&sock->snd_bytes);
            sock_tag->rcv_bytes = sock->ipcom.rcv_bytes;
#ifdef IPSCTP
            /* if not SCTP set it with NULL */
            sock_tag->priv_data = IP_NULL;
            if (IP_IPPROTO_SCTP == sock->proto)
            {
                sock_tag->priv_data = ipsctp_get_netstat_info(sock);
                if (IP_NULL != sock_tag->priv_data)
                    return IPNET_CTRL_SUCCESS;
            }
#endif /* IPSCTP */

#ifdef IPTCP
            if (sock->proto == IP_IPPROTO_TCP)
                sock_tag->tcp_state = iptcp_get_state(sock);
#endif /* IPTCP */
            sock_tag->saddr.in_addr.sin_port = ip_htons(sock->sport);
            sock_tag->daddr.in_addr.sin_port = ip_htons(sock->dport);
            sock_tag->saddr.in_addr.sin_family = (Ip_u8) sock->ipcom.domain;
            sock_tag->daddr.in_addr.sin_family = (Ip_u8) sock->ipcom.domain;
#ifdef IPCOM_USE_INET
            if (sock->ipcom.domain == IP_AF_INET)
            {
                IPCOM_SA_LEN_SET(&sock_tag->saddr.in_addr, sizeof(sock_tag->saddr.in_addr));
                sock_tag->saddr.in_addr.sin_addr.s_addr = sock->ip4->saddr_n;
                IPCOM_SA_LEN_SET(&sock_tag->daddr.in_addr, sizeof(sock_tag->daddr.in_addr));
                sock_tag->daddr.in_addr.sin_addr.s_addr = sock->ip4->daddr_n;
            }
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
            if (sock->ipcom.domain == IP_AF_INET6)
            {
                Ip_u32 scope_id = sock->ip6->iscope_id;

                IPCOM_SA_LEN_SET(&sock_tag->saddr.in6_addr, sizeof(sock_tag->saddr.in6_addr));
                sock_tag->saddr.in6_addr.sin6_addr = sock->ip6->saddr;
                sock_tag->saddr.in6_addr.sin6_scope_id = scope_id;
                sock_tag->saddr.in6_addr.sin6_flowinfo = ip_htonl(sock->ip6->flowinfo);
                IPCOM_SA_LEN_SET(&sock_tag->daddr.in6_addr, sizeof(sock_tag->daddr.in6_addr));
                sock_tag->daddr.in6_addr.sin6_addr = sock->ip6->daddr;
                sock_tag->daddr.in6_addr.sin6_scope_id = scope_id;
                sock_tag->daddr.in6_addr.sin6_flowinfo = ip_htonl(sock->ip6->flowinfo);
            }
#endif /* IPCOM_USE_INET6 */
            return IPNET_CTRL_SUCCESS;
        }
    }
    return IPNET_CTRL_ENUM_DONE;
}


#ifdef IPCOM_USE_INET6
/*
 *===========================================================================
 *                    ipnet_ctrl_ip6_nc
 *===========================================================================
 * Description: Sets and gets IPv6 neighbor cache entries.
 * Parameters:  tag - Contains the information to set/add/delete.
 * Returns:     0 = success, <0 = error.
 *
 */
IP_STATIC int
ipnet_ctrl_ip6_nc(struct Ipnet_tag *tag)
{
    struct Ipnet_tag_nc_entry *nc_tag = (struct Ipnet_tag_nc_entry*) tag;
    Ipnet_ip6_nc_entry        *nc;
    Ipnet_netif               *netif;
    Ipnet_route_entry         *rt = IP_NULL;
    Ip_bool                    prev_entry = IP_TRUE;
    int                        ret;

    if (tag->size < sizeof(*nc_tag))
    {
        tag->size = sizeof(*nc_tag);
        return -IP_ERRNO_ENOSPC;
    }

    switch (tag->type)
    {
    case IPNET_CTRL_NEW_ENUM_NC_ENTRY :
        rt = ipnet_route_first_entry(IP_AF_INET6, nc_tag->vr_index, IPCOM_ROUTE_TABLE_DEFAULT);
        prev_entry = IP_FALSE;
        /* Fall through */
    case IPNET_CTRL_CONT_ENUM_NC_ENTRY :
        if (prev_entry)
        {
            /* The nc_tag->addr contains the previous entry */
            ret = ipnet_route_raw_lookup(IP_AF_INET6,
                                         nc_tag->vr_index,
                                         IPCOM_ROUTE_TABLE_DEFAULT,
                                         0,
                                         &nc_tag->addr.sin6_addr,
                                         nc_tag->addr.sin6_scope_id,
                                         0,
                                         &rt);
            if (ret < 0)
                return ret;
        }

        for (;;)
        {
            while (prev_entry || IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_LLINFO) == IP_FALSE)
            {
                rt = ipnet_route_next_entry(rt);
                if (rt == IP_NULL)
                    return IPNET_CTRL_ENUM_DONE;
                prev_entry = IP_FALSE;
            }
            nc = IPNET_IP6_NC(rt);
            if (nc->ne.state == IPNET_ND_STALE
                && rt->metrics.rmx_expire != IPCOM_ADDR_INFINITE
                && IPCOM_IS_LEQ(rt->metrics.rmx_expire, IPNET_SECONDS_ABS))
            {
                /* Scheduled for delete, move to the next one */
                prev_entry = IP_TRUE;
                continue;
            }
            break;
        }

        ipnet_route_key_to_sockaddr(IP_AF_INET6, rt->hdr.key,
            (struct Ip_sockaddr *) &nc_tag->addr);
        if (rt->metrics.rmx_expire == IPCOM_ADDR_INFINITE)
            nc_tag->expires_sec = IPCOM_ADDR_INFINITE;
        else if (IPCOM_IS_LT(rt->metrics.rmx_expire, IPNET_SECONDS_ABS))
            nc_tag->expires_sec = 0;
        else
            nc_tag->expires_sec = (rt->metrics.rmx_expire - IPNET_SECONDS_ABS);
        nc_tag->ifindex = rt->netif->ipcom.ifindex;
        nc_tag->is_router = nc->is_router;
        nc_tag->state = nc->ne.state;
        ipcom_memcpy(&nc_tag->link_addr, rt->gateway,
                     IPCOM_SA_LEN_GET(rt->gateway));
        return IPNET_CTRL_SUCCESS;

    case IPNET_CTRL_ADD_NC_ENTRY :
        if (nc_tag->ifindex != 0)
        {
            netif = ipnet_if_indextonetif(nc_tag->vr_index, nc_tag->ifindex);
            if (netif == IP_NULL)
                /* The specified interface does not exist */
                return -IP_ERRNO_ENXIO;
            nc_tag->vr_index = netif->vr_index;
        }
        else
        {
            /* Use the interface specified by the route table */
            ret = ipnet_route_raw_lookup(IP_AF_INET6,
                                         nc_tag->vr_index,
                                         IPCOM_ROUTE_TABLE_DEFAULT,
                                         0,
                                         &nc_tag->addr.sin6_addr,
                                         nc_tag->addr.sin6_scope_id,
                                         0,
                                         &rt);
            if (ret < 0)
                return ret;
            netif = rt->netif;
        }
        nc = ipnet_ip6_neighbor_cache_find(netif, &nc_tag->addr.sin6_addr, IP_TRUE);
        if (nc == IP_NULL)
            return -IP_ERRNO_ENOBUFS;
        /* Manually created NDP entries should have the RTF_STATIC
           flag set */
        IP_BIT_SET(nc->ne.rt->hdr.flags, IPNET_RTF_STATIC);
        ipnet_ip6_neighbor_cache_update(nc,
                                        IPNET_ND_REACHABLE,
                                        IP_SOCKADDR_DL_LLADDR(&nc_tag->link_addr),
                                        IP_TRUE);
        ipnet_route_set_lifetime(nc->ne.rt, nc_tag->expires_sec);
        return IPNET_CTRL_SUCCESS;

    case IPNET_CTRL_DEL_NC_ENTRY :
        ret = ipnet_route_lookup(IP_AF_INET6,
                                 nc_tag->vr_index,
                                 IPCOM_ROUTE_TABLE_DEFAULT,
                                 IPNET_RTL_FLAG_DONTCLONE | IPNET_RTL_FLAG_LINK_LOCAL,
                                 &nc_tag->addr.sin6_addr,
                                 nc_tag->addr.sin6_scope_id,
                                 nc_tag->ifindex,
                                 &rt);
        if (ret != 0)
            return -IP_ERRNO_ENOTFOUND;
        /* Schedule for deletion */
        nc = IPNET_IP6_NC(rt);
        nc->ne.state = IPNET_ND_STALE;
        ipnet_route_set_lifetime(rt, 0);
        return IPNET_CTRL_SUCCESS;

    default:
        break;
    }

    return -IP_ERRNO_EINVAL;
}


/*
 *===========================================================================
 *                    ipnet_ctrl_ip6_prefix
 *===========================================================================
 * Description: Sets and gets IPv6 address prefixes.
 * Parameters:  tag - Contains the information to set/add/delete.
 * Returns:     0 = success, <0 = error.
 *
 */
IP_STATIC int
ipnet_ctrl_ip6_prefix(struct Ipnet_tag *tag)
{
    struct Ipnet_tag_prefix_entry *prefix_tag = (struct Ipnet_tag_prefix_entry*) tag;
    Ipnet_netif                   *netif;
    Ip_bool                        has_moved_to_next = IP_FALSE;
    Ipnet_route_entry             *rt = IP_NULL;
    struct Ip_sockaddr_in6         sa_mask;
    int                            ret;

    if (tag->size < sizeof(*prefix_tag))
    {
        tag->size = sizeof(*prefix_tag);
        return -IP_ERRNO_ENOSPC;
    }

    if (prefix_tag->ifindex == 0
        && tag->type != IPNET_CTRL_NEW_ENUM_PREFIX_ENTRY
        && tag->type != IPNET_CTRL_CONT_ENUM_PREFIX_ENTRY)
        return -IP_ERRNO_EINVAL;

    if (prefix_tag->ifindex != 0)
    {
        netif = ipnet_if_indextonetif(prefix_tag->vr_index, prefix_tag->ifindex);
        if (netif == IP_NULL)
            /* The specified interface does not exist */
            return -IP_ERRNO_ENXIO;
    }

    switch (tag->type)
    {
    case IPNET_CTRL_CONT_ENUM_PREFIX_ENTRY :
        ret = ipnet_route_raw_lookup(IP_AF_INET6,
                                     prefix_tag->vr_index,
                                     IPCOM_ROUTE_TABLE_DEFAULT,
                                     0,
                                     &prefix_tag->prefix.sin6_addr,
                                     prefix_tag->prefix.sin6_scope_id,
                                     0,
                                     &rt);
        if (ret < 0)
            return ret;
        /* Fall through */
    case IPNET_CTRL_NEW_ENUM_PREFIX_ENTRY :
        if (rt == IP_NULL)
            rt = ipnet_route_first_entry(IP_AF_INET6, prefix_tag->vr_index, IPCOM_ROUTE_TABLE_DEFAULT);

        do {
            while (!has_moved_to_next
                   || IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_X_AUTO))
            {
                rt = ipnet_route_next_entry(rt);
                if (rt == IP_NULL)
                    return IPNET_CTRL_ENUM_DONE;
                has_moved_to_next = IP_TRUE;
            }
            /* Scheduled for delete if the while statements evaluates to true */
        } while (rt->metrics.rmx_expire != IPCOM_ADDR_INFINITE
                 && IPCOM_IS_LEQ(rt->metrics.rmx_expire, IPNET_SECONDS_ABS));

        ipnet_route_key_to_sockaddr(IP_AF_INET6, rt->hdr.key,
            (struct Ip_sockaddr *) &prefix_tag->prefix);
        ipnet_route_key_to_sockaddr(IP_AF_INET6, rt->hdr.mask,
            (struct Ip_sockaddr *) &sa_mask);
        prefix_tag->bits = ipcom_sockaddr_to_prefixlen((struct Ip_sockaddr *) &sa_mask);
        prefix_tag->ifindex = rt->netif->ipcom.ifindex;
        if (rt->metrics.rmx_expire == IPCOM_ADDR_INFINITE)
            prefix_tag->expires_sec = IPCOM_ADDR_INFINITE;
        else
            prefix_tag->expires_sec = (rt->metrics.rmx_expire - IPNET_SECONDS_ABS);
        return IPNET_CTRL_SUCCESS;

    default:
        break;
    }
    return -IP_ERRNO_EINVAL;
}


/*
 *===========================================================================
 *                    ipnet_ctrl_ip6_router
 *===========================================================================
 * Description: Sets and gets IPv6 default routers.
 * Parameters:  tag - Contains the information to set/add/delete.
 * Returns:     0 = success, <0 = error.
 *
 */
IP_STATIC int
ipnet_ctrl_ip6_router(struct Ipnet_tag *tag)
{
    struct Ipnet_ipv6_key          key;
    struct Ip_sockaddr_in6         gw;
    struct Ipnet_tag_router_entry *router_tag = (struct Ipnet_tag_router_entry*) tag;
    int                            ret = 0;
    Ipnet_route_entry             *rt = IP_NULL;
    Ip_bool                        force_next = IP_FALSE;
    Ipnet_netif                   *netif;

    if (tag->size < sizeof(*router_tag))
    {
        tag->size = sizeof(*router_tag);
        return -IP_ERRNO_ENOSPC;
    }

    switch (tag->type)
    {
    case IPNET_CTRL_NEW_ENUM_ROUTER_ENTRY :
        ret = ipnet_route_raw_lookup2(IP_AF_INET6,
                                      router_tag->vr_index,
                                      IPCOM_ROUTE_TABLE_DEFAULT,
                                      0,
                                      &ip_in6addr_any,
                                      0,
                                      &ip_in6addr_any,
                                      IP_NULL,
                                      0,
                                      &rt);
        /* Fall through */
    case IPNET_CTRL_CONT_ENUM_ROUTER_ENTRY :
        if (tag->type == IPNET_CTRL_CONT_ENUM_ROUTER_ENTRY)
        {
            force_next = IP_TRUE;
            netif = ipnet_if_indextonetif(router_tag->vr_index, router_tag->ifindex);
            if (netif == IP_NULL)
                /* The specified interface does not exist */
                return -IP_ERRNO_ENXIO;
            ipnet_ip6_create_sockaddr(&gw,
                                      &router_tag->addr,
                                      netif->ipcom.ifindex);
            ret = ipnet_route_raw_lookup2(IP_AF_INET6,
                                          router_tag->vr_index,
                                          IPCOM_ROUTE_TABLE_DEFAULT,
                                          0,
                                          &ip_in6addr_any,
                                          0,
                                          IP_NULL,
                                          (struct Ip_sockaddr *) &gw,
                                          router_tag->ifindex,
                                          &rt);
        }

        if (ret == IPNET_ROUTE_PERFECT_MATCH)
        {
            /* Select default routes that was dynamically added */
            while (rt
                   && (rt->gateway == IP_NULL
                       || IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_STATIC)
                       || force_next))
            {
                rt = rt->next;
                force_next = IP_FALSE;
            }
        }

        if (ret != IPNET_ROUTE_PERFECT_MATCH || rt == IP_NULL || rt->gateway == IP_NULL)
            return IPNET_CTRL_ENUM_DONE;

        IPNET_IP6_SET_ADDR(&router_tag->addr, &((struct Ip_sockaddr_in6 *) rt->gateway)->sin6_addr);
        if (rt->metrics.rmx_expire == IPCOM_ADDR_INFINITE)
            router_tag->expires_sec = IPCOM_ADDR_INFINITE;
        else
            router_tag->expires_sec = rt->metrics.rmx_expire - IPNET_SECONDS_ABS;
        router_tag->ifindex = rt->netif->ipcom.ifindex;
        return IPNET_CTRL_SUCCESS;

    case IPNET_CTRL_DEL_ROUTER_ENTRY :
        ipcom_memset(&key, 0, sizeof(key));
        ipnet_ip6_create_sockaddr(&gw,
                                  &router_tag->addr,
                                  router_tag->ifindex);

        if (ipnet_route_delete2(IP_AF_INET6,
                                router_tag->vr_index,
                                IPCOM_ROUTE_TABLE_DEFAULT,
                                &key,
                                &key,
                                (struct Ip_sockaddr *) &gw,
                                router_tag->ifindex,
                                ipnet_get_effective_pid(),
                                0,
                                IP_FALSE) < 0)
        {
            return -IP_ERRNO_ENOTFOUND;
        }
        return IPNET_CTRL_SUCCESS;

    default:
        break;
    }
    return -IP_ERRNO_EINVAL;
}
#endif /* IPCOM_USE_INET6 */


/*
 ****************************************************************************
 * 10                    GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_ctrl_islocked
 *===========================================================================
 * Description: SNMP helper function to access ndp tables
 *              Code semaphore must be locked.
 * Parameters:  tag - The data to set or get.
 * Returns:     0 = succcess, -1 = error
 *              (get error code from ipcom_errno)
 *
 */
#if defined(IPCOM_USE_MIB2) && defined(IPCOM_USE_INET6)
IP_GLOBAL int
ipnet_ctrl_islocked(struct Ipnet_tag *tag)
{
    int retval = -IP_ERRNO_EINVAL;
    ip_assert(tag != IP_NULL);

    if (IP_BIT_ISSET(tag->type, IPNET_CTRL_NC_ENTRY_BASE))
        retval = ipnet_ctrl_ip6_nc(tag);

    if (retval < 0)
        goto errout;
    return retval;

errout:
    return ipcom_errno_set(-retval);
}
#endif


/*
 ****************************************************************************
 * 11                    PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_ctrl
 *===========================================================================
 * Description: Sets/gets runtime configuration for IPNET.
 * Parameters:  tag - The data to set or get.
 * Returns:     0 = succcess, -1 = error
 *              (get error code from ipcom_errno)
 *
 */
IP_PUBLIC int
ipnet_ctrl(struct Ipnet_tag *tag)
{
    int   retval;
    void *suspend_handle;
    ip_assert(tag != IP_NULL);

    retval = IP_ERRNO_ENOMEM;
    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        goto errout;

#ifdef IPCOM_USE_INET6
    if (IP_BIT_ISSET(tag->type, IPNET_CTRL_NC_ENTRY_BASE))
        retval = ipnet_ctrl_ip6_nc(tag);
    else if (IP_BIT_ISSET(tag->type, IPNET_CTRL_PREFIX_ENTRY_BASE))
        retval = ipnet_ctrl_ip6_prefix(tag);
    else if (IP_BIT_ISSET(tag->type, IPNET_CTRL_ROUTER_ENTRY_BASE))
        retval = ipnet_ctrl_ip6_router(tag);
    else
#endif /* IPCOM_USE_INET6 */
    if (IP_BIT_ISSET(tag->type, IPNET_CTRL_SOCK_BASE))
        retval = ipnet_ctrl_sock(tag);
    else
        retval = -IP_ERRNO_EINVAL;

    ipnet_resume_stack(suspend_handle);

    if (retval < 0)
        goto errout;
    return retval;

errout:
    (void) ipcom_errno_set(-retval);
    return -1;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

