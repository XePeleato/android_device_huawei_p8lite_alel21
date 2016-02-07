/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_raw.c,v $ $Revision: 1.98 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_raw.c,v $
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
 */

/* Raw socket address family functions */

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
#include <ipcom_inet.h>
#include <ipcom_sock.h>
#include <ipcom_syslog.h>
#include <ipcom_type.h>

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_usr_sock.h"

#ifdef IPCOM_WV_INSTRUMENTATION
#include <ipcom_windview.h>
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

#ifdef IPCOM_USE_PACKET

/*
 *===========================================================================
 *                    ipnet_sock_packet_init
 *===========================================================================
 * Description: The netlink domain socket initialization.
 * Parameters:  sock - The socket that is beeing created.
 *              user_context - not used.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_STATIC int
ipnet_sock_packet_init(Ipnet_socket *sock, Ip_bool user_context)
{
    (void) user_context;
    ipnet_sock_set_reachable(sock);
    ipnet->num_packet_sock++;
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_sock_packet_destroy
 *===========================================================================
 * Description: Destroys the netlink socket.
 * Parameters:  sock - The socket beeing closed.
 * Returns:     0 = success
 *
 */
IP_STATIC void
ipnet_sock_packet_destroy(Ipnet_socket *sock)
{
    ipnet_sock_unbind(sock);
    ipnet->num_packet_sock--;
}

/*
 *===========================================================================
 *                    ipnet_sock_packet_lookup
 *===========================================================================
 * Description: Returns IP_AF_PACKET sockets that match the frame type.
 * Parameters:  vr - The virtual router index.
 *              frame_type - The frame type that the matching socket must have.
 * Returns:     >0 = application bytes sent, <0 = error code.
 *
 */
IP_STATIC Ipnet_socket *
ipnet_sock_packet_lookup(Ip_u16 vr, int frame_type,
                         IP_CONST void *unused1, Ip_u16 unused2,
                         IP_CONST void *unused3, Ip_u16 unused4)
{
    Ipnet_socket_lookup_key key;

    (void)unused1;
    (void)unused2;
    (void)unused3;
    (void)unused4;

    ipcom_memset(&key, 0, sizeof(key));
    key.domain = IP_AF_PACKET;
    key.proto  = (Ip_u16)frame_type;
    key.vr     = vr;
    return ipcom_hash_get(ipnet->reachable_sockets, &key);
}


/*
 *===========================================================================
 *                    ipnet_sock_packet_send
 *===========================================================================
 * Description: Sends a raw frame directly to the driver (IP_SOCK_RAW) or
 *              directly to the link layer to get a link header
 *              (IP_SOCK_DGRAM) (but no ARP or similar is performed).
 * Parameters:  sock - The socket to use when sending.
 *              msg - Destination of the packet and (optionally) ancillary data.
 *              pkt - The packet data to send.
 * Returns:     >0 = application bytes sent, <0 = error code.
 *
 */
IP_STATIC int
ipnet_sock_packet_send(Ipnet_socket *sock,
                       IP_CONST struct Ip_msghdr *msg,
                       Ipcom_pkt *pkt)
{
    struct Ip_sockaddr_ll *ll_addr;
    Ipnet_netif           *netif;
    int                    ret;
    int                    length;
    int                    max_pkt_len;

    ip_assert(sock != IP_NULL);
    ip_assert(msg != IP_NULL);

    length = pkt->end - pkt->start;

    if (msg->msg_name == IP_NULL)
    {
        IPCOM_LOG0(DEBUG, "RAW: ipcom_sendto()/ipcom_sendmsg() on packet sockets must have a 'to' addr");
        ret = -IP_ERRNO_EINVAL;
        goto errout;
    }

    ll_addr = (struct Ip_sockaddr_ll *) msg->msg_name;

    if (ll_addr->sll_family != IP_AF_PACKET)
    {
        IPCOM_LOG2(DEBUG, "RAW: sll_family must be IP_AF_PACKET (%d), got %d",
                   IP_AF_PACKET, (int) ll_addr->sll_family);
        ret = -IP_ERRNO_EPROTOTYPE;
        goto errout;
    }

    netif = ipnet_if_indextonetif(sock->vr_index, ll_addr->sll_ifindex);
    if (netif == IP_NULL)
    {
        IPCOM_LOG1(DEBUG, "RAW: %d is not a valid interface index",
                   ll_addr->sll_ifindex);
        ret = -IP_ERRNO_ENXIO;
        goto errout;
    }
    pkt->ifindex = netif->ipcom.ifindex;

    if (sock->ipcom.type == IP_SOCK_RAW)
        /* Link layer header included */
        max_pkt_len = netif->ipcom.mtu + netif->ipcom.link_hdr_size;
    else if (sock->ipcom.type == IP_SOCK_DGRAM)
        /* Link layer must be added by the stack */
        max_pkt_len = netif->ipcom.mtu;
    else
    {
        max_pkt_len = 0;
        IP_PANIC();
    }

    if (pkt->end - pkt->start > max_pkt_len)
    {
        IPCOM_LOG2(DEBUG, "RAW: packet to big (%d bytes), allowed size is %d bytes",
                   pkt->end - pkt->start, max_pkt_len);
        ret = -IP_ERRNO_ENOBUFS;
        goto errout;
    }

    if (sock->ipcom.type == IP_SOCK_DGRAM)
    {
        if (netif->link_raw_output == IP_NULL)
        {
            IPCOM_LOG1(DEBUG,
                       "RAW: interface %s does not support"
                       "IP_SOCK_DGRAM type IP_AF_PACKET sockets",
                       netif->ipcom.name);
            ret = -IP_ERRNO_ENOSYS;
            goto errout;
        }
        if (ll_addr->sll_protocol == 0)
        {
            if (sock->proto == 0)
            {
                IPCOM_LOG1(DEBUG,
                           "RAW: protocol was neither specified when the socket %d was created"
                           " nor in the 'to' address", sock->ipcom.fd);
                ret = -IP_ERRNO_EFAULT;
                goto errout;
            }
            ll_addr->sll_protocol = sock->proto;
        }
        IPNET_IF_LOCK(netif);
        ret = netif->link_raw_output(netif, ll_addr, pkt);
        goto leave;
    }

    if (netif->ipcom.drv_output == IP_NULL)
    {
        IPCOM_LOG1(DEBUG,
                   "RAW: interface %s does not support"
                   "IP_SOCK_RAW type IP_AF_PACKET sockets",
                   netif->ipcom.name);
        ret = -IP_ERRNO_ENOSYS;
        goto errout;
    }

    IPNET_IF_LOCK(netif);
    ret = ipnet_if_output(netif, pkt);

 leave:
    IPNET_IF_UNLOCK(netif);
    return ret >= 0 ? length : ret;

 errout:
    ipcom_pkt_free(pkt);
    return ret;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_packet_input
 *===========================================================================
 * Context:     Any non-interrupt context.
 * Description: Inputs a copy of the packet to the stack.
 * Parameters:  netif - The interface the packet was received on.
 *              pkt - The received packet. pkt->start is the offset to the
 *              the link layer header.
 * Returns:     IP_TRUE if the stack should continue to process the packet
 *              (i.e. the packet is not freed in this case).
 *
 */
IP_GLOBAL Ip_bool
ipnet_packet_input(Ipnet_netif *netif, Ipcom_pkt *pkt)
{
    int     retval;
    Ip_u16  frame_type;
    Ip_bool continue_process = IP_TRUE;

    ip_assert(ipnet->num_packet_sock > 0);

    if (netif->link_get_frame_info == IP_NULL)
    {
        IPCOM_LOG1(DEBUG, "RAW: packet sockets is not supported by interface %d",
                   netif->ipcom.ifindex);
        return IP_TRUE;
    }

    retval = netif->link_get_frame_info(netif, pkt, IP_NULL, &frame_type, IP_NULL, IP_NULL);
    if (retval < 0)
    {
        IPCOM_LOG1(DEBUG, "RAW: failed to get information about the frame (%d)", -retval);
        return IP_TRUE;
    }

    /* Give to all sockets that matches the protocol */
    if (ipnet_raw_input(pkt, IP_FALSE, frame_type, IP_NULL, 0, IP_NULL, 0,
                        (Ipnet_sock_lookup_f) ipnet_sock_packet_lookup) == -IP_ERRNO_ENODATA)
        continue_process = IP_FALSE;

    /* Give to all sockets that should match any protocol -> all packtes */
    frame_type = 0;
    if (ipnet_raw_input(pkt, IP_FALSE, frame_type, IP_NULL, 0, IP_NULL, 0,
                        (Ipnet_sock_lookup_f) ipnet_sock_packet_lookup) == -IP_ERRNO_ENODATA)
        continue_process = IP_FALSE;

    if (!continue_process)
        ipcom_pkt_free(pkt);
    return continue_process;
}


/*
 *===========================================================================
 *                    ipnet_packet_getsockopt
 *===========================================================================
 * Description: Handles IP_SOL_PACKET socket options.
 * Parameters:  optname - The option to get.
 *              mreq - The user data.
 *              optlen - Contains the length of the user supplied data,
 *                       will contain the number of bytes used of the
 *                       supplied buffer if the call is successful.
 * Returns:     0 = success
 *             <0 = error code.
 *
 */
IP_GLOBAL Ip_bool
ipnet_packet_getsockopt(int optname, IP_CONST struct Ip_packet_mreq *mreq, Ip_socklen_t *optlen)
{
    (void)optname;
    (void)mreq;
    (void)optlen;
    return -IP_ERRNO_EINVAL;
}


/*
 *===========================================================================
 *                    ipnet_packet_setsockopt
 *===========================================================================
 * Description: Handles IP_SOL_PACKET socket options.
 * Parameters:  optname - The option to set.
 *              mreq - The user data.
 *              optlen - The length of the user data.
 * Returns:     0 = success
 *             <0 = error code.
 *
 */
IP_GLOBAL Ip_bool
ipnet_packet_setsockopt(Ipnet_socket *sock, int optname, IP_CONST struct Ip_packet_mreq *mreq, Ip_socklen_t optlen)
{
    Ipnet_netif *netif;

    if (optlen != sizeof(struct Ip_packet_mreq))
        return -IP_ERRNO_EINVAL;

    switch (mreq->mr_type)
    {
    case IP_PACKET_MR_MULTICAST:
        break;
    case IP_PACKET_MR_PROMISC:
    case IP_PACKET_MR_ALLMULTI:
        IPCOM_LOG0(WARNING, "ipnet_packet_setsockopt:"
                   "type IP_PACKET_MR_PROMISC and IP_PACKET_MR_ALLMULTI not implemented");
        return -IP_ERRNO_EINVAL;

    default:
        IPCOM_LOG1(ERR, "ipnet_packet_setsockopt:"
                   "unknown mr_type: %d", mreq->mr_type);
        return -IP_ERRNO_EINVAL;
    }

    netif = ipnet_if_indextonetif(sock->vr_index, mreq->mr_ifindex);
    if (netif == IP_NULL)
        return -IP_ERRNO_ENXIO;

    if (netif->ipcom.link_addr_size != mreq->mr_alen)
        return -IP_ERRNO_EINVAL;

    switch (optname)
    {
    case IP_PACKET_ADD_MEMBERSHIP:
        return ipnet_if_drv_ioctl(netif, IP_SIOCXADDMULTI, (void*) mreq->mr_address);
    case IP_PACKET_DROP_MEMBERSHIP:
        return ipnet_if_drv_ioctl(netif, IP_SIOCXDELMULTI, (void*) mreq->mr_address);
    default:
        return -IP_ERRNO_EINVAL;
    }
}

/*
 *===========================================================================
 *                    ipnet_sock_packet_register
 *===========================================================================
 * Description: Registers the IP_AF_ROUTE domain.
 * Parameters:
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_sock_packet_register(void)
{
    Ipnet_sock_ops *ops;
    int             types[] = { IP_SOCK_RAW, IP_SOCK_DGRAM };
    Ip_size_t       t;
    int             ret;

    for (t = 0; t < sizeof(types) / sizeof(types[0]); t++)
    {
        ops = ipcom_calloc(1, sizeof(*ops));
        if (ops == IP_NULL)
        {
            IPCOM_LOG0(CRIT, "Could not register the IP_AF_PACKET domain, out of memory");
            IP_PANIC();
            return -IP_ERRNO_ENOMEM;
        }

        ipnet_usr_sock_get_ops(ops);
        ops->domain     = IP_AF_PACKET;
        ops->type       = types[t];
        ops->proto      = -1;
        ops->destroy    = ipnet_sock_packet_destroy;
        ops->init       = ipnet_sock_packet_init;
        ops->lookup     = ipnet_sock_packet_lookup;
        ops->send       = ipnet_sock_packet_send;
        ops->hdr_space += IPNET_UDP_HDR_SIZE;

        ret = ipnet_sock_register_ops(ops);
        if (ret < 0)
            return ret;
    }
    return 0;
}
#endif /* IPCOM_USE_PACKET */



/*
 *===========================================================================
 *                         ipnet_raw_socket_input
 *===========================================================================
 * Description: Queues raw or multicast UDP packet on the specified socket
 * Parameters:  sock - socket to queue the packet on
 *              pktp - pointer to packet pointer of packet to queue
 *              do_clone - IP_TRUE if the passed packet must be cloned
 *                         before it can be queued.
 * Returns:
 *
 */
IP_STATIC int
ipnet_raw_socket_input(Ipnet_socket  *sock,
                       Ipcom_pkt    **pktp,
                       Ip_bool        do_clone)
{
    Ipcom_pkt           *pkt      = *pktp;
    Ipcom_socket_eventcb event_cb = sock->ipcom.event_cb;

    if (do_clone)
    {
        pkt = ipnet_pkt_clone(pkt,
                              sock->ipcom.domain == IP_AF_INET || sock->ipcom.domain == IP_AF_INET6);
        if (pkt == IP_NULL)
            return -IP_ERRNO_ENOBUFS;
    }

    pkt->fd = (int)sock->ipcom.fd;

    if (ipnet_queue_received_packet(pkt, sock) < 0)
    {
        if (do_clone)
            ipcom_pkt_free(pkt);
        return 0;
    }

    if (event_cb != IP_NULL)
        /* The read-only version of the socket event callback is
           used for select() notification on several OS ports and
           must be called after the data has been queued on the
           socket */
        event_cb(&sock->ipcom, pkt, IP_SOEVENT_CB_INPKT);

    if (!do_clone)
        *pktp = IP_NULL;
    return 0;
}

/*
 *===========================================================================
 *                    ipnet_raw_input
 *===========================================================================
 * Description: Generic input handler for packet.
 * Parameters:  pkt - The received packet.
 *              take_ownership - Set to IP_TRUE if this function should control
 *              the lifetime of the packet.
 *              proto - The protocol number.
 *              src_addr - The address (Ipv4 or IPv6) of the sender.
 *              src_port - The source port.
 *              dst_addr - The address (Ipv4 or IPv6) of the receiver (this system).
 *              src_port - The destination port.
 *              socklookup - A pointer to a function that can return the socket
 *              that is associated with this packet.
 * Returns:     0 >= number of matching sockets, <0 = error code
 *
 */
IP_GLOBAL int
ipnet_raw_input(Ipcom_pkt *pkt,
                Ip_bool take_ownership,
                int proto,
                IP_CONST void *src_addr, Ip_u16 src_port,
                IP_CONST void *dst_addr, Ip_u16 dst_port,
                Ipnet_sock_lookup_f socklookup)
{
    Ip_bool       checksum_ok = IP_FALSE;
    int           matching_sock = 0;
    int           ret;
    Ipnet_socket *sock;
    Ipnet_socket *prev_sock = IP_NULL;
    Ip_u32        checksum;
    Ip_u16        vrs[2];
    Ip_size_t     v;

    ip_assert(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_ALLOC));

    vrs[0] = pkt->vr_index;
    vrs[1] = IPCOM_VR_ANY;
    for (v = 0; v < sizeof(vrs) / sizeof(vrs[0]); v++)
    {
        /* Do this for the sockets matching the table and the once
           bound to the wildcard group */
        sock = socklookup(vrs[v], proto,
                          dst_addr, dst_port,
                          src_addr, src_port);
        for (; sock != IP_NULL && matching_sock >= 0; sock = sock->next)
        {
#ifdef IPCOM_USE_INET
            if (sock->ipcom.domain == IP_AF_INET)
            {
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
                Ip_u32 dst4 = IPNET_IP4_GET_IPADDR(dst_addr);
                if (IP_IN_CLASSD(dst4))
                {
                    /* RFC 3376, chapter 3.2
                      Filtering of packets based upon a socket's multicast reception
                      state is a new feature of this service interface.  The previous
                      service interface [RFC1112] described no filtering based upon
                      multicast join state; rather, a join on a socket simply caused the
                      host to join a group on the given interface, and packets destined
                      for that group could be delivered to all sockets whether they had
                      joined or not.
                     */
                    Ipnet_ip4_sock_mcast_data *mcd;

                    mcd = *ipnet_sock_ip4_get_mcast_data(sock,
                                                         ipnet_if_indextonetif(pkt->vr_index, pkt->ifindex),
                                                         (struct Ip_in_addr *) &dst4);
                    if (mcd != IP_NULL
                        && mcd->sources != IP_NULL
                        && ((mcd->sources->user == IPCOM_SET_IS_EXCLUDE)
                            == ipcom_set_contains(mcd->sources, src_addr)))
                            /* Source part of exclude or not part of include list */
                            continue;
                }
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */

                if (sock->proto == IP_IPPROTO_ICMP)
                {
                    Ipnet_pkt_ip   *ip4  = (Ipnet_pkt_ip *) &pkt->data[pkt->start];
                    Ipnet_pkt_icmp *icmp;

                    icmp = (Ipnet_pkt_icmp *) &pkt->data[pkt->start + IPNET_IP4_GET_HDR_LEN(ip4) * sizeof(Ip_u32)];

                    if (IP_ICMP_FILTER_WILLBLOCK(icmp->type, sock->ip4->icmp_filter))
                        continue;
                }
            }
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
            if (sock->ipcom.domain == IP_AF_INET6)
            {
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
                if (IP_IN6_IS_ADDR_MULTICAST(dst_addr))
                {
                    /* RFC 3810, chapter 4.2
                       Requiring the filtering of packets based upon a socket's multicast
                       reception state is a new feature of this service interface.  The
                       previous service interface described no filtering based upon
                       multicast listening state; rather, a Start Listening operation on a
                       socket simply caused the node to start to listen to a multicast
                       address on the given interface; packets sent to that multicast
                       address could be delivered to all sockets, whether they had started
                       to listen or not.
                     */
                    Ipnet_ip6_sock_mcast_data *mcd;

                    mcd = *ipnet_sock_ip6_get_mcast_data(sock,
                                                         ipnet_if_indextonetif(pkt->vr_index, pkt->ifindex),
                                                         dst_addr);
                    if (mcd != IP_NULL
                        && mcd->sources != IP_NULL
                        && ((mcd->sources->user == IPCOM_SET_IS_EXCLUDE)
                            == ipcom_set_contains(mcd->sources, src_addr)))
                            /* Source part of exclude or not part of include list */
                            continue;
                }
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */

                if (sock->proto == IP_IPPROTO_ICMPV6)
                {
                    Ipnet_pkt_icmp6 *icmp = (Ipnet_pkt_icmp6 *) &pkt->data[pkt->start];

                    if (IP_ICMP6_FILTER_WILLBLOCK(icmp->icmp6_type, sock->ip6->icmp6_filter))
                        continue;
                }
            }
#endif /* IPCOM_USE_INET6 */

#ifdef IPNET_USE_NETLINKSOCK
            if (sock->ipcom.domain == IP_AF_NETLINK)
            {
                Ip_u32  nl_groups = dst_addr? *(IP_CONST Ip_u32 *)dst_addr : 0;

                if ((sock->nl->nl_src_mc_groups & IP_NLMSG_GROUP_MASK(nl_groups)) == 0)
                    continue;

                /* We've got a match here */
                checksum_ok = IP_TRUE;
            }
#endif /* IPNET_USE_NETLINKSOCK */

            if (checksum_ok == IP_FALSE
                && IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_ADDCHECKSUM)
                && sock->proto != IP_IPPROTO_ICMPV6
                && sock->proto != IP_IPPROTO_UDP
                && IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_LOOPED_BACK))
            {
                /* The checksum must be checked for this packet
                   UDP and ICMPv6 packets has already been checked */
                checksum = ipcom_in_checksum_update(&pkt->data[pkt->start],
                                                    (Ip_size_t) (pkt->end - pkt->start));
                if (ipcom_in_checksum_finish(pkt->chk + checksum) != 0)
                {
                    /* Wrong checksum. */
                    IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                                      1, 1, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                                      ipnet_raw_input, IPCOM_WV_NETD_BADSUM,
                                      IPCOM_WV_IPNET_RAW_MODULE, IPCOM_WV_NETD_RAWIP);
                    IPNET_STATS(raw_input_badchksum++);
                    matching_sock = -IP_ERRNO_EINVAL;
                    goto leave;
                }
                checksum_ok = IP_TRUE;
            }

            if (prev_sock)
            {
                if ((ret = ipnet_raw_socket_input(prev_sock, &pkt, IP_TRUE)) < 0)
                {
                    matching_sock = ret;
                    goto leave;
                }
            }

            prev_sock = sock;
            matching_sock++;
        }
    }

    if (prev_sock)
    {
        if ((ret = ipnet_raw_socket_input(prev_sock, &pkt, !take_ownership)) < 0)
            matching_sock = ret;
    }


 leave:
    if (take_ownership && pkt)
        ipcom_pkt_free(pkt);
    return matching_sock;
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

