/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_sysctl.c,v $ $Revision: 1.96 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_sysctl.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
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
 **************************************************************************** */

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_heap_sort.h>
#include <ipcom_route.h>
#include <ipcom_sysctl.h>
#include <ipcom_type.h>

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_route.h"
#include "ipnet_routesock.h"
#include "ipnet_signal.h"
#include "ipnet_usr_sock.h"



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

/* Used when dumping routes to keep the state and buffers */
typedef struct Ipnet_sysctl_route_data_struct
{
    void        *buf;
    Ip_size_t   buf_len;
    Ip_size_t   bytes_written;
    Ip_u32      flags;
    Ip_u16      vr;
    Ip_u32      table;
    int         soerrno;
    int         domain;
}
Ipnet_sysctl_route_data;


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
 *                    ipnet_sysctl_route_dump_elem_len
 *===========================================================================
 * Description: Calculates the length needed to write down address information
 *              for the specified route entry.
 * Parameters:  rt - a route entry.
 * Returns:     Length in bytes.
 *
 */
IP_STATIC Ip_u16
ipnet_sysctl_route_dump_elem_len(Ipnet_route_entry *rt)
{
    Ip_u16 len;
    Ip_u16 addr_len;

    len = sizeof(struct Ipnet_rt_msghdr);
    if (IPNET_ROUTE_GET_FAMILY(rt->head) == IP_AF_INET)
        addr_len = sizeof(struct Ip_sockaddr_in);
    else
        addr_len = sizeof(struct Ip_sockaddr_in6);

    /* Length of destination address */
    len += addr_len;

    /* Length of destination mask */
    if (rt->hdr.mask)
        len += addr_len;

    /* Length of gateway address */
    if (rt->gateway)
        len += IPCOM_SA_LEN_GET(rt->gateway);
    else if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_CLONING))
        len += sizeof(struct Ip_sockaddr_dl);

    return len;
}


/*
 *===========================================================================
 *                    ipnet_sysctl_route_add_addr
 *===========================================================================
 * Description: Adds an address in route table format.
 * Parameters:  buf - the buffer to where the address should be written.
 *              domain - the domain this address is part of.
 *              rt_addr - a address or mask from the route table.
 * Returns:     The address to where the next data should be written in
 *              the buffer.
 *
 */
IP_STATIC void *
ipnet_sysctl_route_add_addr(void *buf, int domain, void *rt_addr)
{
    union Ip_sockaddr_union *addr;

    addr = (union Ip_sockaddr_union *) buf;
#ifdef IPCOM_USE_INET
    if (domain == IP_AF_INET)
    {
        struct Ipnet_ipv4_key *key = (struct Ipnet_ipv4_key *) rt_addr;

        ipcom_memset(addr, 0, sizeof(struct Ip_sockaddr_in));
        addr->sin.sin_family = IP_AF_INET;
        IPCOM_SA_LEN_SET(&addr->sin, sizeof(struct Ip_sockaddr_in));
        ipcom_memcpy(&addr->sin.sin_addr, &key->addr, sizeof(addr->sin.sin_addr));
    }
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    if (domain == IP_AF_INET6)
    {
        struct Ipnet_ipv6_key *key = (struct Ipnet_ipv6_key *) rt_addr;

        ipcom_memset(addr, 0, sizeof(struct Ip_sockaddr_in6));
        addr->sin6.sin6_family = IP_AF_INET6;
        IPCOM_SA_LEN_SET(&addr->sin6, sizeof(struct Ip_sockaddr_in6));
        ipcom_memcpy(&addr->sin6.sin6_scope_id, &key->scope_id, sizeof(addr->sin6.sin6_scope_id));
        ipcom_memcpy(&addr->sin6.sin6_addr, &key->addr, sizeof(addr->sin6.sin6_addr));
    }
#endif /* IPCOM_USE_INET6 */

    return (Ip_u8 *) buf + IPCOM_SA_LEN_GET(&addr->sa);
}


/*
 *===========================================================================
 *                    ipnet_sysctl_route_dump_cb
 *===========================================================================
 * Description: Checks if this entry should be added and adds it if it should.
 * Parameters:  rt - the route entry to add
 *              data - callback data.
 * Returns:     IP_FALSE (the entry should never be deleted).
 *
 */
IP_STATIC Ip_bool
ipnet_sysctl_route_dump_cb(Ipnet_route_entry *rt, Ipnet_sysctl_route_data *data)
{
    struct Ipnet_rt_msghdr *rt_msg;
    Ip_u16                  len;

    if (rt->next)
        (void)ipnet_sysctl_route_dump_cb(rt->next, data);

    if (rt->narrow)
        (void) ipnet_sysctl_route_dump_cb(rt->narrow, data);

    if ((rt->hdr.flags & data->flags) != data->flags)
        return IP_FALSE;

    /* Do not list the 255.255.255.255, 224.0.0.0/4 or IPv6 multicast routes. */
    if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_X_MCAST_RO | IPNET_RTF_X_BCAST_RO | IPNET_RTF_X_HIDDEN))
          return IP_FALSE;

    len = ipnet_sysctl_route_dump_elem_len(rt);
    data->bytes_written += len;

    if (data->bytes_written > data->buf_len)
    {
        /* Buffer to small */
        if (data->buf != IP_NULL)
            data->soerrno = -IP_ERRNO_ENOMEM;
        return IP_FALSE;
    }

    rt_msg = (struct Ipnet_rt_msghdr *) data->buf;
    data->buf = (Ip_u8 *) data->buf + sizeof(struct Ipnet_rt_msghdr);

    ipcom_memset(rt_msg, 0, sizeof(struct Ipnet_rt_msghdr));
    rt_msg->rtm_msglen  = len;
    rt_msg->rtm_version = IPNET_RTM_VERSION;
    rt_msg->rtm_type    = IPNET_RTM_GET;
    rt_msg->rtm_index   = rt->netif ? rt->netif->ipcom.ifindex : 0;
    rt_msg->rtm_table   = data->table;
    rt_msg->rtm_flags   = rt->hdr.flags;
    rt_msg->rtm_use     = rt->use;
    rt_msg->rtm_rmx     = rt->metrics;

    /* add destination address */
    IP_BIT_SET(rt_msg->rtm_addrs, IPNET_RTA_DST);
    data->buf = ipnet_sysctl_route_add_addr(data->buf, IPNET_ROUTE_GET_FAMILY(rt->head), rt->hdr.key);

    if (rt->gateway)
    {
        IP_BIT_SET(rt_msg->rtm_addrs, IPNET_RTA_GATEWAY);
        ipcom_memcpy(data->buf, rt->gateway, IPCOM_SA_LEN_GET(rt->gateway));
        data->buf = (Ip_u8 *) data->buf + IPCOM_SA_LEN_GET(rt->gateway);
    }
    else if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_CLONING))
    {
        struct Ip_sockaddr_dl  *dl = data->buf;
        IP_BIT_SET(rt_msg->rtm_addrs, IPNET_RTA_GATEWAY);
        ipcom_memset(data->buf, 0, sizeof(struct Ip_sockaddr_dl));
        IPCOM_SA_LEN_SET(dl, sizeof(struct Ip_sockaddr_dl));
        dl->sdl_family = IP_AF_LINK;
        ip_assert(rt->netif != IP_NULL);
        if (rt->netif != IP_NULL)
        {
            dl->sdl_index  = (Ip_u16)rt->netif->ipcom.ifindex;
            dl->sdl_type   = (Ip_u8)rt->netif->ipcom.type;
        }
        data->buf = (Ip_u8 *) data->buf + sizeof(struct Ip_sockaddr_dl);
    }

    if (rt->hdr.mask)
    {
        /* add destination mask */
        IP_BIT_SET(rt_msg->rtm_addrs, IPNET_RTA_NETMASK);
        data->buf = ipnet_sysctl_route_add_addr(data->buf, IPNET_ROUTE_GET_FAMILY(rt->head), rt->hdr.mask);
    }

    ip_assert((Ip_u16)((Ip_ptrdiff_t) data->buf - (Ip_ptrdiff_t) rt_msg) == len);

    return IP_FALSE;
}


/*
 *===========================================================================
 *                    ipnet_sysctl_for_each_rtab_cb
 *===========================================================================
 * Description: Dumps all routes entries for every route table.
 * Parameters:  rtab - The route table to dump.
 *              d - The route dump parameters.
 * Returns:
 *
 */
IP_STATIC void
ipnet_sysctl_for_each_rtab_cb(Ipcom_route *rtab, Ipnet_sysctl_route_data *d)
{
    if (d->vr != IPNET_ROUTE_GET_VR(rtab))
        return;
    if (d->domain != 0 && d->domain != IPNET_ROUTE_GET_FAMILY(rtab))
        return;

    d->table = IPNET_ROUTE_GET_TABLE(rtab);
    ipcom_route_walk_tree(rtab,
                          (Ipcom_route_walk_cb) ipnet_sysctl_route_dump_cb,
                          d);
}


/*
 *===========================================================================
 *                    ipnet_sysctl_route_dump
 *===========================================================================
 * Description: Dumps all routes entries.
 * Parameters:  oldp - buffer to where the route entries will be copied
 *              oldlenp - [in] the length of 'oldp' buffer
 *                        [out] the number of bytes copied to 'oldp' buffer or
 *                        the number of bytes needed (if 'oldp' is IP_NULL).
 *              domain - the domain to return routes for (0=all domains)
 *              flags - the IPNET_RTF_xxx flags that must be set in the returned
 *              entries.
 * Returns:     0 == success, <0 == error code.
 *
 */
IP_STATIC int
ipnet_sysctl_route_dump(Ip_u16 vr, void *oldp, Ip_size_t *oldlenp, int domain, Ip_u32 flags)
{
    Ipnet_sysctl_route_data  data;

    data.buf = oldp;
    if (oldp == IP_NULL)
        data.buf_len = 0;
    else
        data.buf_len = *oldlenp;
    data.bytes_written = 0;
    data.flags         = flags;
    data.soerrno       = 0;
    data.domain        = domain;
    data.vr            = vr;

    if (ipnet->rtabs != IP_NULL)
        ipcom_hash_for_each(ipnet->rtabs,
                            (Ipcom_hash_foreach_cb_func) ipnet_sysctl_for_each_rtab_cb,
                            &data);

    if (oldlenp)
        *oldlenp = data.bytes_written;
    return data.soerrno;
}


/*
 *===========================================================================
 *                    ipnet_sysctl_if_add_ip4_addr
 *===========================================================================
 * Description: Adds a IPv4 socket address to the buffer.
 * Parameters:  buf - the buffer where the socket address should be written.
 *              addr_n - the address in network byte order.
 * Returns:     Pointer to where next address should be written.
 *
 */
#ifdef IPCOM_USE_INET
IP_STATIC void *
ipnet_sysctl_if_add_ip4_addr(void *buf, Ip_u32 addr_n)
{
    struct Ip_sockaddr_in *in = (struct Ip_sockaddr_in *) buf;

    ipcom_memset(in, 0, sizeof(struct Ip_sockaddr_in));
    in->sin_family = IP_AF_INET;
    IPCOM_SA_LEN_SET(in, sizeof(struct Ip_sockaddr_in));
    ipcom_memcpy(&in->sin_addr, &addr_n, sizeof(in->sin_addr));

    return in + 1;
}
#endif /* IPCOM_USE_INET */


#ifdef IPCOM_USE_INET6
/*
 *===========================================================================
 *                    ipnet_sysctl_if_add_ip6_addr
 *===========================================================================
 * Description: Adds a IPv6 socket address to the buffer.
 * Parameters:  buf - the buffer where the socket address should be written.
 *              addr - the address in network byte order.
 *              is_scoped - IP_TRUE if the address or mask is scoped.
 *              scope_id - The scope_id to use if the mask/address is scoped.
 * Returns:     Pointer to where next address should be written.
 *
 */
IP_STATIC void *
ipnet_sysctl_if_add_ip6_addr(void *buf,
                             IP_CONST struct Ip_in6_addr *addr,
                             Ip_bool is_scoped,
                             Ip_u32 scope_id)
{
    struct Ip_sockaddr_in6 *in6 = (struct Ip_sockaddr_in6 *) buf;

    ipcom_memset(in6, 0, sizeof(struct Ip_sockaddr_in6));
    in6->sin6_family   = IP_AF_INET6;
    IPCOM_SA_LEN_SET(in6, sizeof(struct Ip_sockaddr_in6));
    in6->sin6_scope_id = (is_scoped ? scope_id : 0);
    IPNET_IP6_SET_ADDR(&in6->sin6_addr, addr);

    return in6 + 1;
}
#endif /* IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                    ipnet_sysctl_if_add_addrs
 *===========================================================================
 * Description: Adds a Ipnet_ifa_msghdr structure for each address matching
 *              the domain assigned to the specified interface.
 * Parameters:  buf - buffer to where the addresses will be copied.
 *              netif - interface for which all addresses should be added.
 *              domain - the domain to return addresses for (0=all domains)
 * Returns:     Number of bytes added or bytes needed if buf == IP_NULL
 *
 */
IP_STATIC Ip_u16
ipnet_sysctl_if_add_addrs(void *buf, Ipnet_netif *netif, int domain)
{
    Ip_size_t len = 0;

#ifdef IPCOM_USE_INET
    if (domain == 0 || domain == IP_AF_INET)
    {
        Ipnet_ip4_addr_entry *addr4;

        addr4 = netif->inet4_addr_list;
        if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_POINTOPOINT) && addr4 == IP_NULL)
            buf = ipnet_sysctl_if_add_addrs_one(buf, netif, IP_AF_INET, IP_NULL, &len);
        else
            for (; addr4 != IP_NULL; addr4 = addr4->next)
            {
#if defined (IPNGC) || defined (IPQUAGGA)
                /*!! show them or not? */
                if (addr4->type == IPNET_ADDR_TYPE_MULTICAST)
                    continue;
#endif
                if (addr4->type == IPNET_ADDR_TYPE_NETBROADCAST)
                    continue;

                buf = ipnet_sysctl_if_add_addrs_one(buf, netif, IP_AF_INET, addr4, &len);
            }
    }
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    if (domain == 0 || domain == IP_AF_INET6)
    {
        Ipnet_ip6_addr_entry *addr6;

        for (addr6 = netif->inet6_addr_list; addr6 != IP_NULL; addr6 = addr6->next)
        {
#if defined (IPNGC) || defined (IPQUAGGA)
            /*!! show them or not? */
            if (addr6->type == IPNET_ADDR_TYPE_MULTICAST)
              continue;
#endif
            buf = ipnet_sysctl_if_add_addrs_one(buf, netif, IP_AF_INET6, addr6, &len);
        }
    }
#endif /* IPCOM_USE_INET6 */

    return (Ip_u16)len;
}


/*
 *===========================================================================
 *                    ipnet_sysctl_if_dump
 *===========================================================================
 * Description: Dumps interface entries.
 * Parameters:  oldp - buffer to where the interface entries will be copied
 *              oldlenp - [in] the length of 'oldp' buffer
 *                        [out] the number of bytes copied to 'oldp' buffer or
 *                        the number of bytes needed (if 'oldp' is IP_NULL).
 *              domain - the domain to return addresses for (0=all domains)
 *              ifindex - the interface to return informtaion for (0=interfaces)
 * Returns:     0 == success, <0 == error code.
 *
 */
IP_STATIC int
ipnet_sysctl_if_dump(Ip_u16 vr, void *oldp, Ip_size_t *oldlenp, int domain, Ip_u32 ifindex)
{
    Ip_size_t    total_bytes = 0;
    Ip_size_t    written = 0;
    Ipnet_netif *netif;
    Ip_u32      *ifindexes = IP_NULL;
    int          ret = 0;
    unsigned     num_elem;
    Ip_u32       i;

    IPNET_NETIF_FOR_EACH_OR_SPECIFIC_ON_VR(netif, vr, ifindex, i)
    {
        total_bytes += ipnet_sysctl_create_ifinfo(IP_NULL, netif);
        total_bytes += ipnet_sysctl_if_add_addrs(IP_NULL, netif, domain);
    }

    if (total_bytes == 0)
    {
        /* Invalid ifindex */
        ret = -IP_ERRNO_ENXIO;
        goto cleanup;
    }

    if (oldp == IP_NULL)
    {
        *oldlenp = total_bytes;
        goto cleanup;
    }

    if (total_bytes > *oldlenp)
    {
        ret = -IP_ERRNO_ENOMEM;
        goto cleanup;
    }

    ifindexes = ipnet_if_get_index_array(vr, ifindex, &num_elem);
    if (ifindexes == IP_NULL)
        return -IP_ERRNO_ENOMEM;

    for (i = 0; i < num_elem; i++)
    {
        netif = ipnet_if_indextonetif(vr, ifindexes[i]);
        written += ipnet_sysctl_create_ifinfo((Ip_u8 *) oldp + written, netif);
        written += ipnet_sysctl_if_add_addrs((Ip_u8 *) oldp + written, netif, domain);
    }

    ip_assert(written == total_bytes);
    *oldlenp = written;

 cleanup:
    ipcom_free(ifindexes);
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_sysctl_inet4
 *===========================================================================
 * Description: System control for IPv4.
 * Parameters:  name - the hierarchical sysctl name.
 *              namelen - the size of the 'name' array.
 *              oldp - buffer to where the route entries will be copied
 *              oldlen - [in] the length of 'oldp' buffer
 *                        [out] the number of bytes copied to 'oldp' buffer or
 *                        the number of bytes needed (if 'oldp' is IP_NULL).
 *              newp - the new value for the variable
 *              newlen - the length of the 'newp' buffer.
 * Returns:     0 == success, <0 == error code.
 *
 */
#ifdef IPCOM_USE_INET
IP_STATIC int
ipnet_sysctl_inet4(Ip_u16 vr,
                   int *name, unsigned int namelen,
                   void *oldp, Ip_size_t *oldlen,
                   void *newp, Ip_size_t newlen)
{
    if (namelen < 4)
        return -IP_ERRNO_ENOTDIR;

    switch (name[2])
    {
    case IP_IPPROTO_IP:
        /* Size check */
        switch (name[3])
        {
        case IP_IPCTL_FORWARDING:
#ifdef IP_IPCTL_SENDREDIRECTS
        case IP_IPCTL_SENDREDIRECTS:
#endif
#ifdef IP_IPCTL_MTUDISC
        case IP_IPCTL_MTUDISC:
#endif
            if (oldp && *oldlen < sizeof(int))
                return -IP_ERRNO_ENOMEM;
            if (newp && newlen != sizeof(int))
                return -IP_ERRNO_EINVAL;
            break;

        default:
            break;
        }

        /* Action */
        switch (name[3])
        {
        case IP_IPCTL_FORWARDING:
            if (namelen > 4)
            {
                Ipnet_netif *netif = ipnet_if_indextonetif(vr, name[4]);
                if (netif == IP_NULL)
                    return -IP_ERRNO_ENXIO;


                if (oldp)
                {
                    *((int *) oldp) = IP_BIT_ISFALSE(netif->flags2, IPNET_IFF2_NO_IPV4_FORWARD);
                    *oldlen = sizeof(int);
                }

                if (newp)
                {
                    if (*(int*) newp)
                        IP_BIT_CLR(netif->flags2, IPNET_IFF2_NO_IPV4_FORWARD);
                    else
                        IP_BIT_SET(netif->flags2, IPNET_IFF2_NO_IPV4_FORWARD);

#ifdef IPNET_USE_RFC1256
                    ipnet_ip4_rfc1256_mode_update(netif->vr_index, netif);
#endif
                }
            }
            else
            {
                if (oldp)
                {
                    *((int *) oldp) = (IP_BIT_ISSET(ipnet_flags, IPNET_FLAG_IPV4_FORWARD_PKT) ? 1 : 0);
                    *oldlen = sizeof(int);
                }

                if (newp)
                {
                    if (*(int*) newp)
                        IP_BIT_SET(ipnet_flags, IPNET_FLAG_IPV4_FORWARD_PKT);
                    else
                        IP_BIT_CLR(ipnet_flags, IPNET_FLAG_IPV4_FORWARD_PKT);

#ifdef IPNET_USE_RFC1256
                    ipnet_ip4_rfc1256_mode_update(vr, IP_NULL);
#endif
                }
            }
            break;

#ifdef IP_IPCTL_SENDREDIRECTS
        case IP_IPCTL_SENDREDIRECTS:
            if (oldp)
            {
                int val = ipcom_sysvar_get_as_int("ipnet.inet.IcmpRedirectSend", 2);
                *((int *) oldp) = val > 0 ? 1 : 0;
                *oldlen = sizeof(int);
            }

            if (newp)
            {
                if (*(int*) newp)
                    /*  2 if a redirect should be sent and the original message should be forwarded */
                    (void)ipcom_sysvar_set("ipnet.inet.IcmpRedirectSend", "2", IPCOM_SYSVAR_FLAG_OVERWRITE);
                else
                    /* 0 if no redirect should be sent but the original message should be forwarded */
                    (void)ipcom_sysvar_set("ipnet.inet.IcmpRedirectSend", "0", IPCOM_SYSVAR_FLAG_OVERWRITE);
            }
            break;
#endif /* IP_IPCTL_SENDREDIRECTS */

#ifdef IP_IPCTL_MTUDISC
        case IP_IPCTL_MTUDISC:
            if (oldp)
            {
                int val = ipcom_sysvar_get_as_int0("ipnet.inet.EnablePathMtuDiscovery", 1);
                *((int *) oldp) = val > 0 ? 1 : 0;
                *oldlen = sizeof(int);
            }

            if (newp)
                (void)ipcom_sysvar_set("ipnet.inet.EnablePathMtuDiscovery",
                                       *(int*) newp ? "1" : "0",
                                       IPCOM_SYSVAR_FLAG_OVERWRITE);
            break;

#endif /* IP_IPCTL_MTUDISC */
        default:
            return -IP_ERRNO_EINVAL;
        }

        return 0;

    case IP_IPPROTO_UDP:
        switch (name[3])
        {
#ifdef IP_UDPCTL_CHECKSUM
        case IP_UDPCTL_CHECKSUM:
            if (oldp && *oldlen < sizeof(int))
                return -IP_ERRNO_ENOMEM;
            if (newp && newlen != sizeof(int))
                return -IP_ERRNO_EINVAL;

            if (oldp)
            {
                int val = ipcom_sysvar_get_as_int("ipnet.inet.UdpChecksum", 1);
                *((int *) oldp) = val > 0 ? 1 : 0;
                *oldlen = sizeof(int);
            }

            if (newp)
                (void)ipcom_sysvar_set("ipnet.inet.UdpChecksum",
                                       *(int*) newp ? "1" : "0",
                                       IPCOM_SYSVAR_FLAG_OVERWRITE);
            return 0;
#endif /* IP_UDPCTL_CHECKSUM */
        default:
            return -IP_ERRNO_EINVAL;
        }


    default:
        break;
    }

    return -IP_ERRNO_EINVAL;
}
#endif /* IPCOM_USE_INET */


/*
 *===========================================================================
 *                    ipnet_sysctl_ip6
 *===========================================================================
 * Description: System control for IPv4.
 * Parameters:  name - the hierarchical sysctl name.
 *              namelen - the size of the 'name' array.
 *              oldp - buffer to where the route entries will be copied
 *              oldlen - [in] the length of 'oldp' buffer
 *                        [out] the number of bytes copied to 'oldp' buffer or
 *                        the number of bytes needed (if 'oldp' is IP_NULL).
 *              newp - the new value for the variable
 *              newlen - the length of the 'newp' buffer.
 * Returns:     0 == success, <0 == error code.
 *
 */
#ifdef IPCOM_USE_INET6
IP_STATIC int
ipnet_sysctl_inet6(Ip_u16 vr,
                   int *name, unsigned int namelen,
                   void *oldp, Ip_size_t *oldlen,
                   void *newp, Ip_size_t newlen)
{
    Ipnet_netif *netif = IP_NULL;

    if (namelen < 4)
        return -IP_ERRNO_ENOTDIR;

    if (name[2] != IP_IPPROTO_IPV6)
        return -IP_ERRNO_EINVAL;

    switch (name[3])
    {
    case IP_IPV6CTL_ACCEPT_RTADV:
#ifdef IP_IPV6CTL_ENABLED
    case IP_IPV6CTL_ENABLED:
#endif
        if (namelen < 5)
            return -IP_ERRNO_ENOTDIR;

        netif = ipnet_if_indextonetif(vr, name[4]);
        if (netif == IP_NULL)
            return -IP_ERRNO_ENXIO;
    default:
        break;
    }

    switch (name[3])
    {
    case IP_IPV6CTL_FORWARDING:
        if (namelen > 4)
        {
            netif = ipnet_if_indextonetif(vr, name[4]);
            if (netif == IP_NULL)
                return -IP_ERRNO_ENXIO;

            if (oldp)
            {
                if (*oldlen < sizeof(int))
                    return -IP_ERRNO_ENOMEM;
                *((int *) oldp) = IP_BIT_ISFALSE(netif->flags2, IPNET_IFF2_NO_IPV6_FORWARD);
                *oldlen = sizeof(int);
            }

            if (newp)
            {
                if (newlen != sizeof(int))
                    return -IP_ERRNO_EINVAL;
                if (*(int*) newp)
                    IP_BIT_CLR(netif->flags2, IPNET_IFF2_NO_IPV6_FORWARD);
                else
                    IP_BIT_SET(netif->flags2, IPNET_IFF2_NO_IPV6_FORWARD);
            }
        }
        else
        {
            if (oldp)
            {
                if (*oldlen < sizeof(int))
                    return -IP_ERRNO_ENOMEM;
                *((int *) oldp) = (IP_BIT_ISSET(ipnet_flags, IPNET_FLAG_IPV6_FORWARD_PKT) ? 1 : 0);
                *oldlen = sizeof(int);
            }

            if (newp)
            {
                if (newlen != sizeof(int))
                    return -IP_ERRNO_EINVAL;
                if (*(int*) newp)
                    IP_BIT_SET(ipnet_flags, IPNET_FLAG_IPV6_FORWARD_PKT);
                else
                    IP_BIT_CLR(ipnet_flags, IPNET_FLAG_IPV6_FORWARD_PKT);
            }
        }
        break;

    case IP_IPV6CTL_ACCEPT_RTADV:
        if (oldp)
        {
            if (*oldlen < sizeof(int))
                return -IP_ERRNO_ENOMEM;
            *((int *) oldp) = (IP_BIT_ISSET(netif->flags2, IPNET_IFF2_ADV_ACCEPT) ? 1 : 0);
            *oldlen = sizeof(int);
        }

        if (newp)
        {
            if (newlen != sizeof(int))
                return -IP_ERRNO_EINVAL;

            if (*(int*) newp)
                IP_BIT_SET(netif->flags2, IPNET_IFF2_ADV_ACCEPT);
            else
                IP_BIT_CLR(netif->flags2, IPNET_IFF2_ADV_ACCEPT);
        }
        break;

    case IP_IPV6CTL_ENABLED:
        if (oldp)
        {
            if (*oldlen < sizeof(int))
                return -IP_ERRNO_ENOMEM;
            *((int *) oldp) = IP_BIT_ISFALSE(netif->flags2, IPNET_IFF2_IPV6_DISABLED);
            *oldlen = sizeof(int);
        }
        if (newp)
        {
            if (newlen != sizeof(int))
                return -IP_ERRNO_EINVAL;

            if (*(int*) newp)
                IP_BIT_CLR(netif->flags2, IPNET_IFF2_IPV6_DISABLED);
            else
            {
                IP_BIT_SET(netif->flags2, IPNET_IFF2_IPV6_DISABLED);
                ipnet_ip6_kioevent(netif, IP_EIOXDOWN);
            }
        }
        break;

    default:
        return -IP_ERRNO_EINVAL;
    }
    return 0;
}
#endif /* IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                    ipnet_sysctl_route
 *===========================================================================
 * Description: System control for IPNET_AF_INET domain.
 * Parameters:  name - the hierarchical sysctl name.
 *              namelen - the size of the 'name' array.
 *              oldp - buffer to where the route entries will be copied
 *              oldlenp - [in] the length of 'oldp' buffer
 *                        [out] the number of bytes copied to 'oldp' buffer or
 *                        the number of bytes needed (if 'oldp' is IP_NULL).
 * Returns:     0 == success, <0 == error code.
 *
 */
IP_STATIC int
ipnet_sysctl_route(Ip_u16 vr, int *name, unsigned int namelen, void *oldp, Ip_size_t *oldlenp)
{
    if (namelen < 6)
        return -IP_ERRNO_ENOTDIR;

    if (name[2] != 0)
        return -IP_ERRNO_EINVAL;

    switch (name[4])
    {
    case IP_NET_RT_DUMP:
    case IP_NET_RT_FLAGS:
        return ipnet_sysctl_route_dump(vr, oldp, oldlenp, name[3], name[5]);
    case IP_NET_RT_IFLIST:
        return ipnet_sysctl_if_dump(vr, oldp, oldlenp, name[3], name[5]);
    default:
        break;
    }

    return -IP_ERRNO_ENOTDIR;
}


/*
 *===========================================================================
 *                    ipnet_sysctl_net
 *===========================================================================
 * Description: System control for IPNET_CTL_NET.
 * Parameters:  name - the hierarchical sysctl name.
 *              namelen - the size of the 'name' array.
 *              oldp - buffer to where the route entries will be copied
 *              oldlen - [in] the length of 'oldp' buffer
 *                        [out] the number of bytes copied to 'oldp' buffer or
 *                        the number of bytes needed (if 'oldp' is IP_NULL).
 *              newp - the new value for the variable
 *              newlen - the length of the 'newp' buffer.
 * Returns:     0 == success, <0 == error code.
 *
 */
IP_STATIC int
ipnet_sysctl_net(Ip_u16 vr,
                 int *name, unsigned int namelen,
                 void *oldp, Ip_size_t *oldlen,
                 void *newp, Ip_size_t newlen)
{
    switch (name[1])
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET :
        return ipnet_sysctl_inet4(vr, name, namelen, oldp, oldlen, newp, newlen);
#endif
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6 :
        return ipnet_sysctl_inet6(vr, name, namelen, oldp, oldlen, newp, newlen);
#endif
    case IP_AF_ROUTE :
        if (newp != IP_NULL)
            /* All route values are read-only */
            return -IP_ERRNO_EPERM;
        return ipnet_sysctl_route(vr, name, namelen, oldp, oldlen);
    default:
        return -IP_ERRNO_ENOTDIR;
    }
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_sysctl_if_add_addrs_one
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void *
ipnet_sysctl_if_add_addrs_one(void *buf, Ipnet_netif *netif, int domain, void *addrx, Ip_size_t *lenp)
{
    Ip_size_t                elem_len;
    Ip_size_t                dl_len;
    struct Ipnet_ifa_msghdr *ifa = IP_NULL;

    elem_len = sizeof(struct Ipnet_ifa_msghdr);
    if (buf != IP_NULL)
    {
        ifa = (struct Ipnet_ifa_msghdr *) buf;
        ifa->ifam_version = IPNET_RTM_VERSION;
        ifa->ifam_type    = IPNET_RTM_NEWADDR;
        ifa->ifam_addrs   = IPNET_RTA_NETMASK | IPNET_RTA_IFA | IPNET_RTA_IFP;
        ifa->ifam_flags   = IPNET_RTF_UP;
        if (IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_NOARP | IP_IFF_POINTOPOINT | IP_IFF_LOOPBACK))
            IP_BIT_SET(ifa->ifam_flags, IPNET_RTF_CLONING);
        ifa->ifam_index   = netif->ipcom.ifindex;
        ifa->ifam_metric  = netif->ipcom.metric;
        buf = ifa + 1;
    }

#ifdef IPCOM_USE_INET
    if (domain == IP_AF_INET)
    {
        Ipnet_ip4_addr_entry *addr4 = (Ipnet_ip4_addr_entry *)addrx;

        /* It is now the _third_ time I add the "addr4 &&" check. do NOT remove or crash on OSE5! */
        if (addr4 && IP_IN_CLASSD(addr4->ipaddr_n) && ifa != IP_NULL)
            IP_BIT_CLR(ifa->ifam_flags, IPNET_RTF_CLONING);

        if (addr4 == IP_NULL
            || (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_POINTOPOINT)
                && IP_IN_CLASSD(addr4->ipaddr_n) == IP_FALSE))
        {
            struct Ip_ifreq        ifreq;
            struct Ip_sockaddr_in *inaddr = (struct Ip_sockaddr_in *)&ifreq.ip_ifr_addr;

            /* IPNET_RTA_NETMASK */
            elem_len += sizeof(struct Ip_sockaddr_in);
            if (buf != IP_NULL)
            {
                if (addr4 != IP_NULL)
                    buf = ipnet_sysctl_if_add_ip4_addr(buf, addr4->netmask_n);
                else
                    buf = ipnet_sysctl_if_add_ip4_addr(buf, 0xffffffff);
            }
            /* IPNET_RTA_IFP */
            dl_len = ipnet_if_to_sockaddr_dl(netif, buf);
            elem_len += dl_len;
            if (buf != IP_NULL)
                buf = (Ip_u8 *)buf + dl_len;
            /* IPNET_RTA_IFA */
            elem_len += sizeof(struct Ip_sockaddr_in);
            if (buf != IP_NULL)
            {
                if (addr4 != IP_NULL)
                    buf = ipnet_sysctl_if_add_ip4_addr(buf, addr4->ipaddr_n);
                else
                {
                    (void) ipnet_if_link_ioctl(netif, IP_SIOCGIFADDR, &ifreq);
                    buf = ipnet_sysctl_if_add_ip4_addr(buf, inaddr->sin_addr.s_addr);
                }
            }
            /* IPNET_RTA_BRD */
            elem_len += sizeof(struct Ip_sockaddr_in);
            if (buf != IP_NULL)
            {
                IP_BIT_SET(ifa->ifam_addrs, IPNET_RTA_BRD);
                (void) ipnet_if_link_ioctl(netif, IP_SIOCGIFDSTADDR, &ifreq);
                buf = ipnet_sysctl_if_add_ip4_addr(buf, inaddr->sin_addr.s_addr);
            }
        }
        else
        {
            ip_assert(addr4);
            /* IPNET_RTA_NETMASK */
            elem_len += sizeof(struct Ip_sockaddr_in);
            if (buf != IP_NULL)
                buf = ipnet_sysctl_if_add_ip4_addr(buf, addr4->netmask_n);
            /* IPNET_RTA_IFP */
            dl_len = ipnet_if_to_sockaddr_dl(netif, buf);
            elem_len += dl_len;
            if (buf != IP_NULL)
                buf = (Ip_u8 *)buf + dl_len;
            /* IPNET_RTA_IFA */
            elem_len += sizeof(struct Ip_sockaddr_in);
            if (buf != IP_NULL)
                buf = ipnet_sysctl_if_add_ip4_addr(buf, addr4->ipaddr_n);
            /* IPNET_RTA_BRD */
            if (IP_IN_CLASSD(addr4->ipaddr_n) == IP_FALSE && addr4->netmask_n != ~0u)
            {
                elem_len += sizeof(struct Ip_sockaddr_in);
                if (buf != IP_NULL)
                {
                    IP_BIT_SET(ifa->ifam_addrs, IPNET_RTA_BRD);
                    buf = ipnet_sysctl_if_add_ip4_addr(buf,
                                                       (addr4->netmask_n & addr4->ipaddr_n) | ~addr4->netmask_n);
                }
            }
        }
    }
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    if (domain == IP_AF_INET6)
    {
        Ipnet_ip6_addr_entry *addr6 = (Ipnet_ip6_addr_entry *)addrx;
        Ip_bool               is_scoped = ipnet_ip6_is_scoped_addr(&addr6->addr);
        Ip_u32                scope = netif->ipcom.ifindex;
        struct Ip_in6_addr    mask;

        if (IP_IN6_IS_ADDR_MULTICAST(&addr6->addr) && ifa != IP_NULL)
            IP_BIT_CLR(ifa->ifam_flags, IPNET_RTF_CLONING);

        /* IPNET_RTA_NETMASK */
        elem_len += sizeof(struct Ip_sockaddr_in6);
        if (buf != IP_NULL)
        {
            ipcom_memset(&mask, 0, sizeof(mask));
            ipnet_route_create_mask(&mask, addr6->prefixlen);
            buf = ipnet_sysctl_if_add_ip6_addr(buf, &mask, is_scoped, ~0u);
        }
        /* IPNET_RTA_IFP */
        dl_len = ipnet_if_to_sockaddr_dl(netif, buf);
        elem_len += dl_len;
        if (buf != IP_NULL)
            buf = (Ip_u8 *)buf + dl_len;
        /* IPNET_RTA_IFA */
        elem_len += sizeof(struct Ip_sockaddr_in6);
        if (buf != IP_NULL)
            buf = ipnet_sysctl_if_add_ip6_addr(buf, &addr6->addr, is_scoped, scope);

        /* IPNET_RTA_BRD */
        if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_POINTOPOINT)
            && IP_IN6_IS_ADDR_LINK_LOCAL(&addr6->addr))
        {
            struct Ip_in6_ifreq     ifreq6;
            struct Ip_sockaddr_in6 *sa6 = (struct Ip_sockaddr_in6 *)&ifreq6.ip_ifr_dstaddr;

            if (ipnet_if_link_ioctl(netif, IP_SIOCGIFDSTADDR_IN6, &ifreq6) == 0)
            {
                elem_len += sizeof(struct Ip_sockaddr_in6);
                if (buf != IP_NULL)
                {
                    IP_BIT_SET(ifa->ifam_addrs, IPNET_RTA_BRD);
#ifdef IPCOM_USE_SA_LEN
                    ip_assert(sa6->sin6_len == sizeof(struct Ip_sockaddr_in6));
#endif
                    buf = ipnet_sysctl_if_add_ip6_addr(buf, &sa6->sin6_addr, IP_TRUE, sa6->sin6_scope_id);
                }
            }
        }
    }
#endif /* IPCOM_USE_INET6 */
    if (ifa != IP_NULL)
        ifa->ifam_msglen = (Ip_u16)elem_len;
    *lenp += elem_len;
    return buf;
}


/*
 *===========================================================================
 *                    ipnet_sysctl_create_ifinfo
 *===========================================================================
 * Description: Creates a IPNET_RTM_IFINFO message for the specified interface.
 * Parameters:  buf - buffer where the interface information will be written
 *              buflen - the length of 'buf'
 *              netif - the interface to create information structure for.
 * Returns:     The length needed or copied into 'buf'
 *
 */
IP_GLOBAL Ip_u16
ipnet_sysctl_create_ifinfo(void *buf, Ipnet_netif *netif)
{
    struct Ipnet_if_msghdr *ifm;
    struct Ip_sockaddr_dl  *link_addr;
    Ip_u16 len;

    len = (Ip_u16) (sizeof(struct Ipnet_if_msghdr) + ipnet_if_to_sockaddr_dl(netif, IP_NULL));
    if (buf == IP_NULL)
        return len;

    ifm = (struct Ipnet_if_msghdr *) buf;
    link_addr = (struct Ip_sockaddr_dl *) (ifm + 1);

    ifm->ifm_msglen  = len;
    ifm->ifm_version = IPNET_RTM_VERSION;
    ifm->ifm_type    = IPNET_RTM_IFINFO;
    ifm->ifm_addrs   = IPNET_RTA_IFP;
    ifm->ifm_flags   = (int) netif->ipcom.flags;
    ifm->ifm_index   = netif->ipcom.ifindex;

    ifm->ifm_data.ifi_type               = (Ip_u8) netif->ipcom.type;
    ifm->ifm_data.ifi_addrlen            = (Ip_u8) netif->ipcom.link_addr_size;
    ifm->ifm_data.ifi_hdrlen             = (Ip_u8) netif->ipcom.link_hdr_size;
    ifm->ifm_data.ifi_link_state         = 0;
    ifm->ifm_data.ifi_mtu                = netif->ipcom.mtu;
    ifm->ifm_data.ifi_metric             = netif->ipcom.metric;
    ifm->ifm_data.ifi_baudrate           = 0;
    ifm->ifm_data.ifi_ipackets           = netif->ipackets;
    ifm->ifm_data.ifi_ierrors            = netif->ierrors;
    ifm->ifm_data.ifi_opackets           = netif->opackets;
    ifm->ifm_data.ifi_oerrors            = netif->oerrors;
    ifm->ifm_data.ifi_collisions         = netif->collisions;
    ifm->ifm_data.ifi_ibytes             = netif->ibytes;
    ifm->ifm_data.ifi_obytes             = netif->obytes;
    ifm->ifm_data.ifi_imcasts            = netif->imcasts;
    ifm->ifm_data.ifi_omcasts            = netif->omcasts;
    ifm->ifm_data.ifi_iqdrops            = netif->iqdrops;
    ifm->ifm_data.ifi_noproto            = netif->noproto;
    ifm->ifm_data.ifi_lastchange.tv_sec  = 0;
    ifm->ifm_data.ifi_lastchange.tv_usec = 0;

    (void)ipnet_if_to_sockaddr_dl(netif, link_addr);
    return len;
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_sys_sysctl
 *===========================================================================
 * Description: System control for IPNET.
 * Parameters:  name - the hierarchical sysctl name.
 *              namelen - the size of the 'name' array.
 *              oldp - buffer to where the route entries will be copied
 *              oldlenp - [in] the length of 'oldp' buffer
 *                        [out] the number of bytes copied to 'oldp' buffer or
 *                        the number of bytes needed (if 'oldp' is IP_NULL).
 *              newp - the new value for the variable
 *              newlen - the length of the 'newp' buffer.
 *              vr - The virtual router to do the syscontrol for.
 * Returns:     0 == success, <0 == error code.
 *
 */
IP_PUBLIC int
ipnet_sys_sysctl(struct Ipnet_sig_sysctl *sig)
{
    int    ret;

    if (sig->namelen < 2 || sig->namelen > IP_CTL_MAXNAME)
    {
        ret = -IP_ERRNO_EINVAL;
        goto leave;
    }

    if ((sig->oldp != IP_NULL && sig->oldlenp == IP_NULL)
        || (sig->newp != IP_NULL && sig->newlen == 0))
    {
        ret = -IP_ERRNO_EFAULT;
        goto leave;
    }

    switch (sig->name[0])
    {
    case IP_CTL_NET:
        ret = ipnet_sysctl_net(ipnet_get_effective_vr(),
                               sig->name,
                               sig->namelen,
                               sig->oldp,
                               sig->oldlenp,
                               sig->newp,
                               sig->newlen);
        break;
    default:
        ret = -IP_ERRNO_ENOTDIR;
        break;
    }

 leave:
    return ret;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

