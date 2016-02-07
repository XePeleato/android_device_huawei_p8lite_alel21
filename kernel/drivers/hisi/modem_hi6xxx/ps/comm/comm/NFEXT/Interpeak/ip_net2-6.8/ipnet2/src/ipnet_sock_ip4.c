/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_sock_ip4.c,v $ $Revision: 1.41.22.2 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_sock_ip4.c,v $
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
 * IPv4 specific socket operations.
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
#include "ipnet_usr_sock.h"
#ifdef IPSCTP
#include "ipsctp.h"
#endif

#ifdef IPCOM_USE_INET

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
 *                    ipnet_sock_ip4_init
 *===========================================================================
 * Description: IPv4 specific initialization.
 * Parameters:  sock - The socket that is beeing created.
 * Returns:     0 for success else negative error code.
 *
 */
IP_STATIC int
ipnet_sock_ip4_init(Ipnet_socket *sock, Ip_bool user_context)
{
    IPCOM_UNUSED_ARG(user_context);

    sock->uni_hop_limit = (Ip_u8) ipcom_sysvar_get_as_int0("ipnet.inet.BaseHopLimit", 64);

    sock->ip4 = ipnet_sock_alloc_private_data(sock, sizeof(Ipnet_ip4_socket));

#ifndef IPNET_USE_RESTRICTED_SOCKET_BROADCAST
    IP_BIT_SET(sock->ip4->flags, IPNET_SOCKET_FLAG_IP4_ALLOW_BROADCAST);
#endif

#ifdef IPSCTP
    if (IP_NULL != sock->sctp_inst)
        IP_BIT_SET(sock->sctp_inst->inst_flags, IPSCTP_SOCKET_BOUND_V4);
#endif

    switch (sock->ipcom.type)
    {
    case IP_SOCK_RAW:
        if (sock->proto == IP_IPPROTO_ICMP)
        {
            sock->ip4->icmp_filter = ipnet_sock_alloc_private_data(sock, sizeof(struct Ip_icmp_filter));
            IP_ICMP_FILTER_SETPASSALL(sock->ip4->icmp_filter);
        }
        break;
    case IP_SOCK_STREAM:
    case IP_SOCK_SEQPACKET:
        sock->ip4->dont_frag = (Ip_u8) ipcom_sysvar_get_as_int0("ipnet.inet.StreamSockDontFrag", 1);
        return 0;
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
 *                    ipnet_sock_ip4_close
 *===========================================================================
 * Description: IPv4 specific cleanup during socket close.
 * Parameters:  sock - The socket that is beeing closed.
 * Returns:
 *
 */
IP_STATIC int
ipnet_sock_ip4_close(Ipnet_socket *sock)
{
#ifdef IPNET_USE_MCAST_ROUTING
    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_MROUTE_CONTROL))
        ipnet_ip4_mcast_mrt_done(sock);
#endif
    ipcom_free(sock->ip4->opts);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_sock_ip4_connect
 *===========================================================================
 * Description: The IPv4 network layer connect.
 * Parameters:  sock - The socket to connect.
 *              to - The address to connect to.
 *              tolen - The size of the 'to' buffer.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_STATIC int
ipnet_sock_ip4_connect(Ipnet_socket *sock, IP_CONST struct Ip_sockaddr *to, Ip_socklen_t tolen)
{
    struct Ip_sockaddr_in       in_name;
    struct Ip_sockaddr_in      *in_to;
    IP_CONST struct Ip_in_addr *src_addr;
    Ip_u16                      sport = 0;

    ipnet_sock_set_unreachable(sock);

    sock->ip4->daddr_n = 0;

    switch (to->sa_family)
    {
    case IP_AF_INET:
        /* Setup the IPv4 destination (peer) address */
        if (tolen < sizeof(struct Ip_sockaddr_in))
            return -IP_ERRNO_EINVAL;

        in_to = (struct Ip_sockaddr_in*) to;
        if (in_to->sin_addr.s_addr == IP_INADDR_ANY)
            return ipnet_sock_unconnect(sock);

        sock->ip4->daddr_n = in_to->sin_addr.s_addr;
        sock->dport        = ip_ntohs(in_to->sin_port);

        if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_BOUND)
            && ipnet_ip4_get_addr_type(sock->ip4->saddr_n, sock->vr_index, IP_NULL)
            != IPNET_ADDR_TYPE_UNICAST)
        {
            /* No unicast address specified when the socket was bound, need to rebind */
            sport = sock->sport;
            ipnet_sock_unbind(sock);
        }

        if (IP_BIT_ISFALSE(sock->flags, IPNET_SOCKET_FLAG_BOUND))
        {
            Ipnet_netif *netif = IP_NULL;

            if (IP_IN_CLASSD(in_to->sin_addr.s_addr))
                netif = ipnet_if_indextonetif(sock->vr_index, sock->ip4->multicast_if);

            in_name.sin_family = IP_AF_INET;
            IPCOM_SA_LEN_SET(&in_name, sizeof(in_name));
            src_addr = ipnet_ip4_get_src_addr(sock->vr_index, &in_to->sin_addr, IP_NULL, netif);
            if (src_addr == IP_NULL)
            {
                IPCOM_LOG2(WARNING, "connect failed on socket %d, %s is unreachable",
                           sock->ipcom.fd,
                           ipcom_inet_ntop(IP_AF_INET, &in_to->sin_addr, ipnet->log_buf, sizeof(ipnet->log_buf)));
                return -IP_ERRNO_EHOSTUNREACH;
            }
            in_name.sin_addr.s_addr = src_addr->s_addr;
            in_name.sin_port        = ip_htons(sport);
            return sock->ops->bind(sock, (struct Ip_sockaddr *) &in_name, IPCOM_SA_LEN_GET(&in_name), IP_TRUE);
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
 *                    ipnet_sock_ip4_raw_connect
 *===========================================================================
 * Description: Connects a RAW IPv4 socket.
 * Parameters:  sock - The socket to connect.
 *              to - The address to connect to.
 *              tolen - The size of the 'to' buffer.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_STATIC int
ipnet_sock_ip4_raw_connect(Ipnet_socket *sock, IP_CONST struct Ip_sockaddr *to, Ip_socklen_t tolen)
{
    int ret;

    ret = ipnet_sock_ip4_connect(sock, to, tolen);
    if (ret < 0)
        return ret;

    IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_CONNECTED);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_sock_ip4_bind
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
ipnet_sock_ip4_bind(struct Ipnet_socket_struct *sock,
                    IP_CONST struct Ip_sockaddr *name,
                    Ip_socklen_t namelen,
                    Ip_bool check_reuse)
{
    struct Ip_sockaddr_in *name_in;
    enum Ipnet_addr_type   addr_type = IPNET_ADDR_TYPE_NOT_LOCAL;
    Ipnet_ip4_addr_entry  *addr_entry = IP_NULL;

    IPCOM_UNUSED_ARG(check_reuse);

    if (namelen < sizeof(struct Ip_sockaddr_in))
    {
        IPCOM_LOG3(WARNING, "socket %d: namelen too small in bind is %d must be at least %d",
                   sock->ipcom.fd, namelen, sizeof(struct Ip_sockaddr_in));
        return -IP_ERRNO_EINVAL;
    }
    name_in = (struct Ip_sockaddr_in*)name;

    /* Verify that IP address is either ANY (0) or one of my addresses.
     * Note: it is ok to bind to any multicast address
     * or the broadcast address.
     */
    if (name_in->sin_addr.s_addr == IP_INADDR_ANY)
        addr_type = IPNET_ADDR_TYPE_ANY;
    else if (name_in->sin_addr.s_addr == IP_INADDR_BROADCAST)
        addr_type = IPNET_ADDR_TYPE_BROADCAST;
    else if (IP_IN_CLASSD(name_in->sin_addr.s_addr))
        addr_type = IPNET_ADDR_TYPE_MULTICAST;
    else
    {
        /* Check for matching unicast address */
        addr_entry = ipnet_ip4_get_addr_entry(name_in->sin_addr.s_addr, sock->vr_index, IP_NULL);
        if (addr_entry == IP_NULL)
        {
            /* Check all network broadcast addresses */
            Ipnet_netif *netif;
            Ip_u32       i;
            IPNET_NETIF_FOR_EACH_ON_VR(netif, sock->vr_index, i)
            {
                addr_entry = ipnet_ip4_get_addr_entry(name_in->sin_addr.s_addr, sock->vr_index, netif);
                if (addr_entry != IP_NULL)
                    break;
            }
        }
        if (addr_entry != IP_NULL)
            addr_type = addr_entry->type;
    }

    /* Not my address. */
    if (addr_type == IPNET_ADDR_TYPE_NOT_LOCAL)
        return -IP_ERRNO_EADDRNOTAVAIL;

    if (sock->ipcom.type == IP_SOCK_STREAM
        && addr_type != IPNET_ADDR_TYPE_UNICAST
        && addr_type != IPNET_ADDR_TYPE_ANY)
    {
        /* Can't bind stream socket to broadcast/multicast address. */
        return -IP_ERRNO_EINVAL;
    }

    sock->ip4->saddr_n = name_in->sin_addr.s_addr;
    /* SCTP can have many addresses, one is not enough, unbind need not modify */
    if ((addr_entry != IP_NULL) && (IP_IPPROTO_SCTP != sock->proto))
    {
        ip_assert(sock->addr_next == IP_NULL);
        ip_assert(sock->addr_prev == IP_NULL);

        /* Add this socket to list of sockets using the address */
        sock->addr_next = addr_entry->socket_list;
        addr_entry->socket_list = sock;
        if (sock->addr_next != IP_NULL)
            sock->addr_next->addr_prev = sock;
        sock->addr_prev  = IP_NULL;
        sock->addr_entry = addr_entry;
    }

    ipnet_sock_invalidate_ip4_cache(sock);
    return 0;
}


/*
 *===========================================================================
 *                      ipnet_sock_ip4_getname
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
IP_STATIC int
ipnet_sock_ip4_getname(struct Ipnet_socket_struct *sock,
                       struct Ip_sockaddr *name,
                       Ip_socklen_t  *namelen,
                       Ip_bool is_local_name)
{
    struct Ip_sockaddr_in *name_in;

    if (*namelen < sizeof(struct Ip_sockaddr_in))
        return -IP_ERRNO_ENOBUFS;

    *namelen = sizeof(struct Ip_sockaddr_in);
    name_in = (struct Ip_sockaddr_in *)name;
    ipcom_memset(name_in->sin_zero, 0, sizeof(name_in->sin_zero));
    IPCOM_SA_LEN_SET(name_in, sizeof(struct Ip_sockaddr_in));
    name_in->sin_family = IP_AF_INET;
    if (is_local_name)
    {
        name_in->sin_addr.s_addr = sock->ip4->saddr_n;
        name_in->sin_port        = ip_htons(sock->sport);
    }
    else
    {
        if (sock->ip4->daddr_n == IP_INADDR_ANY)
            return -IP_ERRNO_ENOTCONN;

        name_in->sin_addr.s_addr = sock->ip4->daddr_n;
        name_in->sin_port        = ip_htons(sock->dport);
    }
    return 0;
}


/*
 *===========================================================================
 *                   ipnet_sock_ip4_mcast_remove_source
 *===========================================================================
 * Description: Callback to send a report for every source address
 *              blocked/allowed.
 * Parameters:  source - The source address to remove.
 *              md - The multicast address control block.
 * Returns:
 */
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
IP_STATIC void
ipnet_sock_ip4_mcast_remove_source(struct Ip_in_addr *source,
                                   Ipnet_ip4_sock_mcast_data *md)
{
    (void)ipcom_set_remove(md->sources, source);
    ipnet_igmpv3_report_change(md->netif, &md->maddr);
}
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */

/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                   ipnet_sock_ip4_get_mcast_data
 *===========================================================================
 * Description: Returns the pointer to where the multicast data for the
 *              specified multicast address is stored.
 * Parameters:  sock - The controlling socket for the multicast address.
 *              netif - The interface the multicast address will be assigned to.
 *              maddr - An multicast address.
 * Returns:     Always != IP_NULL, but the storage might hold IP_NULL which
 *              means that there was not match.
 */
IP_GLOBAL Ipnet_ip4_sock_mcast_data **
ipnet_sock_ip4_get_mcast_data(Ipnet_socket *sock,
                              Ipnet_netif *netif,
                              IP_CONST struct Ip_in_addr *maddr)
{
    Ipnet_ip4_sock_mcast_data **mlt_it;

    /* Try to find the multicast address in the socket multicast address list */
    for (mlt_it = &sock->ip4->multicast_addr_list;
         *mlt_it != IP_NULL;
         mlt_it = &(*mlt_it)->next)
    {
        if (maddr->s_addr == (*mlt_it)->maddr.s_addr
            && netif == (*mlt_it)->netif)
            break;
    }
    return mlt_it;
}


/*
 *===========================================================================
 *               ipnet_sock_ip4_mcast_delete_data
 *===========================================================================
 * Description: Deletes a multicast data block for a specific address.
 * Parameters:  sock - The controlling socket for the multicast address.
 *              netif - The interface the multicast address will removed from.
 *              maddr - An multicast address.
 * Returns:     >=0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_sock_ip4_mcast_delete_data(Ipnet_socket *sock,
                                 Ipnet_netif *netif,
                                 struct Ip_in_addr *maddr)
{
    Ipnet_ip4_sock_mcast_data  *mcast_data;
    Ipnet_ip4_sock_mcast_data **pmcd;
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    Ipnet_ip4_addr_entry       *addr_entry;
    int                         filter_mode;
#endif

    pmcd = ipnet_sock_ip4_get_mcast_data(sock, netif, maddr);
    mcast_data = *pmcd;

    if (mcast_data == IP_NULL)
        /* This address has not been joined */
        return -IP_ERRNO_EADDRNOTAVAIL;

#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    /* Send IGMPv3 report if the interface filter changes */
    ipcom_set_for_each(mcast_data->sources,
                       (Ipcom_set_foreach_cb_func) ipnet_sock_ip4_mcast_remove_source,
                       mcast_data);
    filter_mode = mcast_data->sources->user;
    ipcom_set_delete(mcast_data->sources);

    addr_entry = ipnet_ip4_get_addr_entry(maddr->s_addr, sock->vr_index, netif);
    if (addr_entry != IP_NULL)
        (void)ipcom_set_remove(addr_entry->mcast.sockets, &sock->ipcom.fd);
    /* else: This socket does not control this multicast address */

    if (filter_mode == IPCOM_SET_IS_EXCLUDE)
        ipnet_igmpv3_report_change(mcast_data->netif, &mcast_data->maddr);
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */

    /* Unhook 'mcast_data' from the list */
    *pmcd = mcast_data->next;
    ipcom_free(mcast_data);
    return ipnet_ip4_remove_addr(netif, maddr->s_addr);
}


/*
 *===========================================================================
 *                    ipnet_sock_ip4_lookup
 *===========================================================================
 * Description: Returns the IPv4 socket matching the parameters.
 * Parameters:  vr - The virtual router the socket must be bound to.
 *              proto - The protocol the returned socket should be using.
 *              local_addr_ptr - The IPv4 address in network order of the
 *              local address to match or IP_NULL if any address should match.
 *              local_port - The source port in HOST byte order or 0 if any
 *              source port should match.
 *              peer_addr_n - The IPv4 address in network order of the
 *              peer address to match or IP_NULL if any address should match.
 *              peer_port - The destination port in HOST byte order or 0 if any
 *              destination port should match.
 *              pfirst - [in] the socket descriptor to from where the scan should start
 *                       [out] the socket descriptor to start the next search from.
 *                       set to IP_NULL to start from 0.
 * Returns:     A pointer to the matching socket or IP_NULL if no socket match.
 *
 */
IP_GLOBAL Ipnet_socket *
ipnet_sock_ip4_lookup(Ip_u16 vr, int proto,
                      IP_CONST Ip_u32 *local_addr_ptr, Ip_u16 local_port,
                      IP_CONST Ip_u32 *peer_addr_ptr, Ip_u16 peer_port)
{
    Ipnet_socket           *sock;
    Ipnet_socket_lookup_key key;

    key.domain     = IP_AF_INET;
    key.proto      = (Ip_u16) proto;
    key.vr         = vr;
    key.local_port = local_port;
    key.peer_port  = peer_port;
    if (local_addr_ptr)
        key.local_addr.in.s_addr = IP_GET_32ON16(local_addr_ptr);
    else
        key.local_addr.in.s_addr = 0;
    if (peer_addr_ptr)
        key.peer_addr.in.s_addr = IP_GET_32ON16(peer_addr_ptr);
    else
        key.peer_addr.in.s_addr = 0;

    sock = ipcom_hash_get(ipnet->reachable_sockets, &key);
    if (sock == IP_NULL)
    {
        key.peer_port = 0;
        key.peer_addr.in.s_addr = 0;
        sock = ipcom_hash_get(ipnet->reachable_sockets, &key);
        if (sock == IP_NULL)
        {
            key.local_addr.in.s_addr = 0;
            sock = ipcom_hash_get(ipnet->reachable_sockets, &key);
        }
    }

#ifdef IPCOM_USE_INET6
    if (sock == IP_NULL)
    {
        struct Ip_in6_addr local_in6;
        struct Ip_in6_addr peer_in6;

        if (local_addr_ptr)
            key.local_addr.in.s_addr = IP_GET_32ON16(local_addr_ptr);
        if (peer_addr_ptr)
            key.peer_addr.in.s_addr = IP_GET_32ON16(peer_addr_ptr);
        ipnet_ip6_create_mapped_addr(&key.local_addr.in, &local_in6);
        ipnet_ip6_create_mapped_addr(&key.peer_addr.in, &peer_in6);
        sock = ipnet_sock_ip6_lookup(vr, proto,
                                     &local_in6, local_port,
                                     &peer_in6, peer_port);
        if (sock == IP_NULL || IP_BIT_ISSET(sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_V6ONLY))
            return IP_NULL;
    }
#endif /* IPCOM_USE_INET6 */
    return sock;
}


/*
 *===========================================================================
 *                    ipnet_sock_ip4_get_ops
 *===========================================================================
 * Description: Sets all members that are valid for IPv4.
 * Parameters:  ops - Socket operation structure.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_sock_ip4_get_ops(Ipnet_sock_inet_ops *ops)
{
    ipnet_usr_sock_get_ops(&ops->sock);
    ops->sock.domain           = IP_AF_INET;
    ops->sock.hdr_space        = IPNET_IP_HDR_SIZE;
    ops->sock.extra_sock_space = sizeof(Ipnet_ip4_socket);
    ops->sock.init             = ipnet_sock_ip4_init;
    ops->sock.destroy          = ipnet_sock_unbind;
    ops->sock.bind             = ipnet_sock_bind;
    ops->sock.connect          = ipnet_sock_ip4_connect;
    ops->sock.lookup           = (Ipnet_sock_lookup_f) ipnet_sock_ip4_lookup;
    ops->sock.send             = ipnet_ip4_sendto;
    ops->sock.getname          = ipnet_sock_ip4_getname;
    ops->sock.close            = ipnet_sock_ip4_close;
    ops->network_bind          = ipnet_sock_ip4_bind;
    ops->network_send          = ipnet_ip4_sendto;
}


/*
 *===========================================================================
 *                    ipnet_sock_ip4_register
 *===========================================================================
 * Description: Registers the IP_SOCK_RAW type for the IP_AF_INET domain.
 * Parameters:
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_sock_ip4_register(void)
{
    Ipnet_sock_inet_ops *ops;

    ops = ipcom_calloc(1, sizeof(*ops));
    if (ops == IP_NULL)
    {
        IPCOM_LOG0(CRIT, "Could not register the IP_SOCK_RAW in IP_AF_INET domain, out of memory");
        IP_PANIC();
        return -IP_ERRNO_ENOMEM;
    }

    ipnet_sock_ip4_get_ops(ops);
    ops->sock.type              = IP_SOCK_RAW;
    ops->sock.proto             = -1;
    ops->sock.connect           = ipnet_sock_ip4_raw_connect;
    ops->sock.extra_sock_space += sizeof(struct Ip_icmp_filter);
    return ipnet_sock_register_ops(&ops->sock);
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#endif /* IPCOM_USE_INET */
/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

