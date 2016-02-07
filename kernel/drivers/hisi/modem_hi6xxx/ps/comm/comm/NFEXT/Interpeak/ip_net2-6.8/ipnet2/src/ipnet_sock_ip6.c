/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_sock_ip6.c,v $ $Revision: 1.45.22.2 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_sock_ip6.c,v $
 *   $Author: kenneth $
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
 * IPv6 specific socket operations
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
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_syslog.h>
#include <ipcom_type.h>

#include "ipnet_h.h"

#ifdef IPCOM_USE_INET6

#include "ipnet_icmp6.h"
#include "ipnet_usr_sock.h"
#ifdef IPSCTP
#include "ipsctp.h"
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
 *                    ipnet_sock_ip6_init
 *===========================================================================
 * Description: IPv4 specific initialization.
 * Parameters:  sock - The socket that is beeing created.
 * Returns:     0 for success else negative error code.
 *
 */
IP_STATIC int
ipnet_sock_ip6_init(Ipnet_socket *sock, Ip_bool user_context)
{
    IPCOM_UNUSED_ARG(user_context);

    sock->uni_hop_limit = (Ip_u8) ipcom_sysvar_get_as_int0("ipnet.inet6.BaseHopLimit", 64);

    sock->ip6 = ipnet_sock_alloc_private_data(sock, sizeof(Ipnet_ip6_socket));
#ifdef IPSCTP
    if (IP_NULL != sock->sctp_inst)
    {
#ifdef IPCOM_USE_INET /* for IPV4&IPV6 */
        IP_BIT_SET(sock->sctp_inst->inst_flags, IPSCTP_SOCKET_BOUND_V4);
        sock->ip4 = (Ipnet_ip4_socket *)ipcom_calloc(1, sizeof(Ipnet_ip4_socket));
#endif
        IP_BIT_SET(sock->sctp_inst->inst_flags, IPSCTP_SOCKET_BOUND_V6);
    }
#endif

    switch (sock->ipcom.type)
    {
    case IP_SOCK_RAW:
        if (sock->proto == IP_IPPROTO_ICMPV6)
        {
            sock->ip6->icmp6_filter = ipnet_sock_alloc_private_data(sock, sizeof(struct Ip_icmp6_filter));
            IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_ADDCHECKSUM);
            sock->chk_offset = 2;
            IP_ICMP6_FILTER_SETPASSALL(sock->ip6->icmp6_filter);
        }
        break;
    case IP_SOCK_SEQPACKET:
    case IP_SOCK_STREAM:
    case IP_SOCK_DGRAM:
        return 0;
    default:
        IP_PANIC2();
        return -IP_ERRNO_EINVAL;
    }

    return ipnet_sock_set_reachable(sock);
}


/*
 *===========================================================================
 *                    ipnet_sock_ip6_close
 *===========================================================================
 * Description: IPv4 specific cleanup during socket close.
 * Parameters:  sock - The socket that is beeing closed.
 * Returns:
 *
 */
#ifdef IPNET_USE_MCAST_ROUTING
IP_STATIC int
ipnet_sock_ip6_close(Ipnet_socket *sock)
{
    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_MROUTE_CONTROL))
        ipnet_ip6_mcast_mrt_done(sock);
    return 0;
}
#endif


/*
 *===========================================================================
 *                    ipnet_sock_ip6_connect
 *===========================================================================
 * Description: The IPv6 network layer connect.
 * Parameters:  sock - The socket to connect.
 *              to - The address to connect to.
 *              tolen - The size of the 'to' buffer.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_STATIC int
ipnet_sock_ip6_connect(Ipnet_socket *sock, IP_CONST struct Ip_sockaddr *to, Ip_socklen_t tolen)
{
    struct Ip_sockaddr_in6       in6_name;
    struct Ip_sockaddr_in6      *in6_to;
    IP_CONST struct Ip_in6_addr *src_addr;
    Ip_u16                       sport = 0;

    ipnet_sock_set_unreachable(sock);

    sock->ip6->daddr    = ip_in6addr_any;
    sock->ip6->flowinfo = 0;

    switch (to->sa_family)
    {
    case IP_AF_INET6:
        /* Setup the IPv6 destination (peer) address */

        if (tolen < sizeof(struct Ip_sockaddr_in6))
            return -IP_ERRNO_EINVAL;

        in6_to = (struct Ip_sockaddr_in6*) to;
        if (IP_IN6_IS_ADDR_UNSPECIFIED(&in6_to->sin6_addr))
            return ipnet_sock_unconnect(sock);

        if (sock->bind_to_ifindex > 0 && IPNET_IP6_IS_SCOPE_LINK_LOCAL(&in6_to->sin6_addr))
            /* Socket is bound to an interface.*/
            in6_to->sin6_scope_id = sock->bind_to_ifindex;

        if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_BOUND))
        {
            Ipnet_netif *netif = IP_NULL;

            if (sock->ip6->iscope_id != in6_to->sin6_scope_id
                && IP_IN6_IS_ADDR_UNSPECIFIED(&sock->ip6->saddr) == IP_FALSE)
                /* The scope ID differes between source and destination,
                 * it will not be possible to communicate */
                return -IP_ERRNO_EHOSTUNREACH;

            if (ipnet_ip6_is_link_local_addr(&sock->ip6->saddr))
                netif = ipnet_if_indextonetif(sock->vr_index, sock->ip6->iscope_id);

            if (ipnet_ip6_get_addr_type(&sock->ip6->saddr, sock->vr_index, netif)
                != IPNET_ADDR_TYPE_UNICAST)
            {
                /* No address specified when the socket was bound, need to rebind */
                sport = sock->sport;
                ipnet_sock_unbind(sock);
            }
        }

        /* Copy the peer information */
        sock->ip6->daddr     = in6_to->sin6_addr;
        sock->ip6->flowinfo  = ip_ntohl(in6_to->sin6_flowinfo);
        sock->dport          = ip_ntohs(in6_to->sin6_port);
        sock->ip6->iscope_id = in6_to->sin6_scope_id;

        if (IP_BIT_ISFALSE(sock->flags, IPNET_SOCKET_FLAG_BOUND))
        {
            int                         ret;
            Ipnet_netif                *netif  = IP_NULL;
            Ipnet_ip6_output_param      param;
            Ipnet_route_policy_selector rps;

            /* Convert the socket to a parameter object */
            if ((ret = ipnet_ip6_socket2param(sock, IP_NULL, in6_to, &param)) < 0)
                return ret;

            if (IP_IN6_IS_ADDR_MULTICAST(&in6_to->sin6_addr))
            {
                if (IPNET_IP6_MC_SCOPE(&in6_to->sin6_addr) != IPNET_IP6_SCOPE_LINK_LOCAL)
                    netif = ipnet_if_indextonetif(sock->vr_index, sock->ip6->multicast_if);
            }
            else
            {
                /* If it's forced */
                netif = param.netif;
            }

            /* Convert it to policy */
            ipnet_ip6_param2policy(&param, IP_NULL, &rps, IP_NULL);

            in6_name.sin6_family = IP_AF_INET6;
            IPCOM_SA_LEN_SET(&in6_name, sizeof(in6_name));
            src_addr = ipnet_ip6_get_src_addr(sock->vr_index,
                                              &sock->ip6->daddr,
                                              sock->ip6->iscope_id,
                                              IP_NULL,
                                              &rps,
                                              netif,
                                              sock->ip6->prefer_src_flags);
            if (src_addr == IP_NULL)
                return -IP_ERRNO_EHOSTUNREACH;

            IPNET_IP6_SET_ADDR(&in6_name.sin6_addr, src_addr);
            in6_name.sin6_port     = ip_htons(sport);
            in6_name.sin6_flowinfo = in6_to->sin6_flowinfo;
            in6_name.sin6_scope_id = in6_to->sin6_scope_id;
            return sock->ops->bind(sock, (struct Ip_sockaddr *) &in6_name, IPCOM_SA_LEN_GET(&in6_name), IP_TRUE);
        }
        /* else: already bound to an unicast address */
        return ipnet_sock_set_reachable(sock);

    case IP_AF_UNSPEC:
        return ipnet_sock_unconnect(sock);

    default:
        return -IP_ERRNO_EAFNOSUPPORT;
    }
}


/*
 *===========================================================================
 *                    ipnet_sock_ip6_raw_connect
 *===========================================================================
 * Description: Connects a RAW IPv6 socket.
 * Parameters:  sock - The socket to connect.
 *              to - The address to connect to.
 *              tolen - The size of the 'to' buffer.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_STATIC int
ipnet_sock_ip6_raw_connect(Ipnet_socket *sock, IP_CONST struct Ip_sockaddr *to, Ip_socklen_t tolen)
{
    int ret;

    ret = ipnet_sock_ip6_connect(sock, to, tolen);
    if (ret < 0)
        return ret;

    IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_CONNECTED);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_sock_ip6_bind
 *===========================================================================
 * Description: The IPv4 network layer bind.
 * Parameters:  sock - The socket to bind.
 *              name - The socket name.
 *              namelen - The length of the name variable.
 *              check_reuse - Set to IP_TRUE if the name should be checked
 *              for address reuse.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_STATIC int
ipnet_sock_ip6_bind(struct Ipnet_socket_struct *sock,
                    IP_CONST struct Ip_sockaddr *name,
                    Ip_socklen_t namelen,
                    Ip_bool check_reuse)
{
    struct Ip_sockaddr_in6 *name_in6;
    enum Ipnet_addr_type    addr_type = IPNET_ADDR_TYPE_NOT_LOCAL;
    Ipnet_ip6_addr_entry   *addr_entry = IP_NULL;
    Ipnet_netif            *netif;

    IPCOM_UNUSED_ARG(check_reuse);

    if (namelen < sizeof(struct Ip_sockaddr_in6))
    {
        IPCOM_LOG3(WARNING, "socket %d: namelen too small in bind is %d must be at least %d",
                   sock->ipcom.fd, namelen, sizeof(struct Ip_sockaddr_in6));
        return -IP_ERRNO_EINVAL;
    }

    name_in6 = (struct Ip_sockaddr_in6*) name;

    if (ipnet_ip6_is_link_local_addr(&name_in6->sin6_addr))
    {
        netif = ipnet_if_indextonetif(sock->vr_index, name_in6->sin6_scope_id);
        if (netif == IP_NULL)
            return -IP_ERRNO_EADDRNOTAVAIL;
    }
    else
    {
        if (name_in6->sin6_scope_id != 0)
        {
            IPCOM_LOG3(WARNING, "scope %d was specified on unscoped address %s when bidning socket %d",
                       name_in6->sin6_scope_id,
                       ipcom_inet_ntop(IP_AF_INET6, &name_in6->sin6_addr, ipnet->log_buf, sizeof(ipnet->log_buf)),
                       sock->ipcom.fd);
            return -IP_ERRNO_EINVAL;
        }
        netif = IP_NULL;
    }

    /* Verify that IP address is either ipnet_in6addr_any or one of my addresses.
     * Note: it is ok to bind to any multicast address.
     */
    if (IP_IN6_IS_ADDR_MULTICAST(&name_in6->sin6_addr))
        addr_type = IPNET_ADDR_TYPE_MULTICAST;
    else
    {
        Ipnet_ip6_addr_entry *addr6_entry;

        addr6_entry = ipnet_ip6_get_addr_entry(&name_in6->sin6_addr, sock->vr_index, netif);
        if (addr6_entry != IP_NULL)
        {
            while (IP_BIT_ISSET(addr6_entry->flags, IPNET_IP6_ADDR_FLAG_DEPRECATED)
                   && addr6_entry->next_dup_addr != IP_NULL)
                addr6_entry = addr6_entry->next_dup_addr;

            if (IP_BIT_ISSET(addr6_entry->flags, IPNET_IP6_ADDR_FLAG_DEPRECATED))
            {
                IPCOM_LOG1(WARNING, "sock bind: cannot bind since address %s is deprecated",
                           ipcom_inet_ntop(IP_AF_INET6, &addr6_entry->addr, ipnet->log_buf, sizeof(ipnet->log_buf)));
                return -IP_ERRNO_EADDRNOTAVAIL;
            }
        }
        addr_entry = addr6_entry;
        addr_type = ipnet_ip6_get_addr_type(&name_in6->sin6_addr, sock->vr_index,  netif);
    }

    /* Not my address. */
    if (addr_type == IPNET_ADDR_TYPE_NOT_LOCAL)
        return -IP_ERRNO_EADDRNOTAVAIL;

    if (sock->ipcom.type == IP_SOCK_STREAM
        && addr_type != IPNET_ADDR_TYPE_UNICAST
        && addr_type != IPNET_ADDR_TYPE_TENTATIVE
        && addr_type != IPNET_ADDR_TYPE_ANY)
    {
        /* Can't bind stream socket to multicast address. */
        return -IP_ERRNO_EINVAL;
    }

    sock->ip6->saddr     = name_in6->sin6_addr;
    sock->ip6->iscope_id = name_in6->sin6_scope_id;
    /* Invalidate the cache pseudo header checksum since bind
       can change the source address */
    sock->ip6->cache.pseudo_hdr_chksum = 0;

#ifdef IPSCTP
    if (IP_IPPROTO_SCTP == sock->proto)
        goto set_invalid_out;
#endif

    if (addr_entry != IP_NULL)
    {
        sock->addr_next = addr_entry->socket_list;
        addr_entry->socket_list = sock;
        if (sock->addr_next != IP_NULL)
            sock->addr_next->addr_prev = sock;
        sock->addr_prev  = IP_NULL;
        sock->addr_entry = addr_entry;
    }
#ifdef IPCOM_USE_INET
    else if (IP_IN6_IS_ADDR_V4MAPPED(&name_in6->sin6_addr))
    {
        Ipnet_ip4_addr_entry *ae4;
        Ip_u32                addr4;

        addr4 = IPNET_IP4_GET_IPADDR(&name_in6->sin6_addr.in6.addr32[3]);
        ae4 = ipnet_ip4_get_addr_entry(addr4, sock->vr_index, netif);
        if (ae4 != IP_NULL)
        {
            /* Add this socket to list of sockets using the address */
            sock->addr_next = ae4->socket_list;
            ae4->socket_list = sock;
            if (sock->addr_next != IP_NULL)
                sock->addr_next->addr_prev = sock;
            sock->addr_prev  = IP_NULL;
            sock->addr_entry = ae4;
        }
    }
#endif /* IPCOM_USE_INET */

#ifdef IPSCTP
set_invalid_out:
#endif
    ipnet_sock_invalidate_ip6_cache(sock);
    return 0;
}


/*
 *===========================================================================
 *                      ipnet_sock_ip6_getname
 *===========================================================================
 * Description: Returns the local name (or address) of the socket.
 * Parameters:  sock - The socket to return the name for.
 *              name - Buffer where the name will be stored.
 *              namelen - The length of the 'name' buffer.
 *              is_local_name - Is set to IP_TRUE of the local name of the
 *              socket should be returned, IP_FALSE means that the peer name
 *              should be returned.
 * Returns:     0 = success, <0 = error code (-IPNET_ERRNO_xxx)
 *
 */
IP_STATIC int ipnet_sock_ip6_getname(struct Ipnet_socket_struct *sock,
                                     struct Ip_sockaddr *name,
                                     Ip_socklen_t  *namelen,
                                     Ip_bool is_local_name)
{
    struct Ip_sockaddr_in6 *name_in6;

    if (*namelen < sizeof(struct Ip_sockaddr_in6))
        return -IP_ERRNO_ENOBUFS;

    *namelen = sizeof(struct Ip_sockaddr_in6);
    name_in6 = (struct Ip_sockaddr_in6 *) name;
    IPCOM_SA_LEN_SET(name_in6, sizeof(struct Ip_sockaddr_in6));
    name_in6->sin6_family   = IP_AF_INET6;
    name_in6->sin6_scope_id = sock->ip6->iscope_id;
    name_in6->sin6_flowinfo = ip_htonl(sock->ip6->flowinfo);
    if (is_local_name)
    {
        name_in6->sin6_addr = sock->ip6->saddr;
        name_in6->sin6_port = ip_htons(sock->sport);
    }
    else
    {
        if (IP_IN6_IS_ADDR_UNSPECIFIED(&sock->ip6->daddr))
            return -IP_ERRNO_ENOTCONN;

        name_in6->sin6_addr = sock->ip6->daddr;
        name_in6->sin6_port = ip_htons(sock->dport);
    }
    return 0;
}


/*
 *===========================================================================
 *                   ipnet_sock_ip6_mcast_remove_source
 *===========================================================================
 * Description: Callback to send a report for every source address
 *              blocked/allowed.
 * Parameters:  source - The source address to remove.
 *              md - The multicast address control block.
 * Returns:
 */
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
IP_STATIC void
ipnet_sock_ip6_mcast_remove_source(struct Ip_in6_addr *source,
                                   Ipnet_ip6_sock_mcast_data *md)
{
    (void)ipcom_set_remove(md->sources, source);
    ipnet_mldv2_report_change(md->netif, &md->maddr);
}
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                   ipnet_sock_ip6_get_mcast_data
 *===========================================================================
 * Description: Returns the pointer to where the multicast data for the
 *              specified multicast address is stored.
 * Parameters:  sock - The controlling socket for the multicast address.
 *              netif - The interface the multicast address will be assigned to.
 *              maddr - An multicast address.
 * Returns:     Always != IP_NULL, but the storage might hold IP_NULL which
 *              means that there was not match.
 */
IP_GLOBAL Ipnet_ip6_sock_mcast_data **
ipnet_sock_ip6_get_mcast_data(Ipnet_socket *sock,
                              Ipnet_netif *netif,
                              IP_CONST struct Ip_in6_addr *maddr)
{
    Ipnet_ip6_sock_mcast_data **mlt_it;

    /* Try to find the multicast address in the socket multicast address list */
    for (mlt_it = &sock->ip6->multicast_addr_list;
         *mlt_it != IP_NULL;
         mlt_it = &(*mlt_it)->next)
    {
        if (IP_IN6_ARE_ADDR_EQUAL(maddr, &(*mlt_it)->maddr)
            && (*mlt_it)->netif == netif)
            break;
    }
    return mlt_it;
}


/*
 *===========================================================================
 *               ipnet_sock_ip6_mcast_delete_data
 *===========================================================================
 * Description: Deletes a multicast data block for a specific address.
 * Parameters:  sock - The controlling socket for the multicast address.
 *              netif - The interface the multicast address will removed from.
 *              maddr - An multicast address.
 * Returns:     >=0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_sock_ip6_mcast_delete_data(Ipnet_socket *sock,
                                 Ipnet_netif *netif,
                                 struct Ip_in6_addr *maddr)
{
    Ipnet_ip6_sock_mcast_data  *mcast_data;
    Ipnet_ip6_sock_mcast_data **pmcd;
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    Ipnet_ip6_addr_entry       *addr_entry;
    int                         filter_mode;
#endif

    pmcd = ipnet_sock_ip6_get_mcast_data(sock, netif, maddr);
    mcast_data = *pmcd;

    if (mcast_data == IP_NULL)
        /* This address has not been joined */
        return -IP_ERRNO_EADDRNOTAVAIL;

#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    /* Send IGMPv3 report if the interface filter changes */
    ipcom_set_for_each(mcast_data->sources,
                       (Ipcom_set_foreach_cb_func) ipnet_sock_ip6_mcast_remove_source,
                       mcast_data);
    filter_mode = mcast_data->sources->user;
    ipcom_set_delete(mcast_data->sources);

    /* This socket does not control this multicast address */
    addr_entry = ipnet_ip6_get_addr_entry(maddr, sock->vr_index, netif);
    if (addr_entry != IP_NULL)
        (void)ipcom_set_remove(addr_entry->mcast.sockets, &sock->ipcom.fd);

    if (filter_mode == IPCOM_SET_IS_EXCLUDE)
        ipnet_mldv2_report_change(mcast_data->netif, &mcast_data->maddr);
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */

    /* Unhook 'mcast_data' from the list */
    *pmcd = mcast_data->next;
    ipcom_free(mcast_data);
    return ipnet_ip6_remove_addr(netif, maddr);
}


/*
 *===========================================================================
 *                    ipnet_sock_ip6_lookup
 *===========================================================================
 * Description: Returns the IPv6 socket matching the parameters. Source is
 *              always this host and the destination is the peer.
 * Parameters:  vr - The virtual router the socket must be using.
 *              proto - The protocol the returned socket should be using.
 *              src_addr - The IPv6 source address in NETWORK byte order or
 *              the undefined address (all zeros) if all addresses should
 *              match. IP_NULL is will also be interpreted as the undefined
 *              address.
 *              local_port - The source port in HOST byte order or 0 if any
 *              source port should match.
 *              dst_addr - The IPv6 destination address in NETWORK byte order
 *              or the undefined address (all zeros) if all addresses should
 *              match. IP_NULL is will also be interpreted as the undefined
 *              address.
 *              peer_port - The destination port in HOST byte order or 0 if any
 *              destination port should match.
 * Returns:     A pointer to the matching socket or IP_NULL if no socket match.
 *
 */
IP_GLOBAL Ipnet_socket *
ipnet_sock_ip6_lookup(Ip_u16 vr, int proto,
                      IP_CONST struct Ip_in6_addr *local_addr, Ip_u16 local_port,
                      IP_CONST struct Ip_in6_addr *peer_addr, Ip_u16 peer_port)
{
    Ipnet_socket_lookup_key key;
    Ipnet_socket           *sock;

    key.domain     = IP_AF_INET6;
    key.proto      = (Ip_u16) proto;
    key.vr         = vr;
    key.local_port = local_port;
    key.peer_port  = peer_port;
    if (local_addr)
        IPNET_IP6_SET_ADDR(&key.local_addr.in6, local_addr);
    else
        ipcom_memset(&key.local_addr.in6, 0, sizeof(struct Ip_in6_addr));
    if (peer_addr)
        IPNET_IP6_SET_ADDR(&key.peer_addr.in6, peer_addr);
    else
        ipcom_memset(&key.peer_addr.in6, 0, sizeof(struct Ip_in6_addr));

    sock = ipcom_hash_get(ipnet->reachable_sockets, &key);
    if (sock == IP_NULL)
    {
        key.peer_port = 0;
        key.peer_addr.in6.in6.addr32[0] = 0;
        key.peer_addr.in6.in6.addr32[1] = 0;
        key.peer_addr.in6.in6.addr32[2] = 0;
        key.peer_addr.in6.in6.addr32[3] = 0;
        sock = ipcom_hash_get(ipnet->reachable_sockets, &key);
        if (sock == IP_NULL)
        {
            key.local_addr.in6.in6.addr32[0] = 0;
            key.local_addr.in6.in6.addr32[1] = 0;
            key.local_addr.in6.in6.addr32[2] = 0;
            key.local_addr.in6.in6.addr32[3] = 0;
            sock = ipcom_hash_get(ipnet->reachable_sockets, &key);
        }
    }
    return sock;
}


/*
 *===========================================================================
 *                    ipnet_sock_ip6_get_ops
 *===========================================================================
 * Description: Sets all members that are valid for IPv6.
 * Parameters:  ops - Socket operation structure.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_sock_ip6_get_ops(Ipnet_sock_inet_ops *ops)
{
    ipnet_usr_sock_get_ops(&ops->sock);
    ops->sock.domain           = IP_AF_INET6;
    ops->sock.connect          = ipnet_sock_ip6_connect;
    ops->sock.hdr_space        = IPNET_IP6_HDR_SIZE;
    ops->sock.extra_sock_space = sizeof(Ipnet_ip6_socket);
    ops->sock.init             = ipnet_sock_ip6_init;
    ops->sock.destroy          = ipnet_sock_unbind;
    ops->sock.bind             = ipnet_sock_bind;
    ops->sock.connect          = ipnet_sock_ip6_connect;
    ops->sock.lookup           = (Ipnet_sock_lookup_f) ipnet_sock_ip6_lookup;
    ops->sock.send             = ipnet_ip6_sendto;
    ops->sock.getname          = ipnet_sock_ip6_getname;
#ifdef IPNET_USE_MCAST_ROUTING
    ops->sock.close            = ipnet_sock_ip6_close;
#endif
    ops->network_bind          = ipnet_sock_ip6_bind;
    ops->network_send          = ipnet_ip6_sendto;
}


/*
 *===========================================================================
 *                    ipnet_sock_ip6_register
 *===========================================================================
 * Description: Registers the IP_SOCK_RAW type for the IP_AF_INET6 domain.
 * Parameters:
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_sock_ip6_register(void)
{
    Ipnet_sock_inet_ops *ops;

    ops = ipcom_calloc(1, sizeof(*ops));
    if (ops == IP_NULL)
    {
        IPCOM_LOG0(CRIT, "Could not register the IP_SOCK_RAW in IP_AF_INET6 domain, out of memory");
        IP_PANIC();
        return -IP_ERRNO_ENOMEM;
    }

    ipnet_sock_ip6_get_ops(ops);
    ops->sock.type              = IP_SOCK_RAW;
    ops->sock.proto             = -1;
    ops->sock.connect           = ipnet_sock_ip6_raw_connect;
    ops->sock.extra_sock_space += sizeof(struct Ip_icmp6_filter);
    return ipnet_sock_register_ops(&ops->sock);
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#endif /* IPCOM_USE_INET6 */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

