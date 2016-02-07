/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_udp.c,v $ $Revision: 1.78 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_udp.c,v $
 *   $Author: jhorteli $
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

/* User datagram protocol transport layer */

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

typedef struct Ipnet_sock_udp_ops_struct
{
    Ipnet_sock_inet_ops  inet;
    Ipnet_sock_init_f    network_init;
    Ipnet_sock_connect_f network_connect;
    Ipnet_sock_bind_f    network_bind;
}
Ipnet_sock_udp_ops;

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
 *                    ipnet_sock_udp_init
 *===========================================================================
 * Description: The UDP transport layer initialization.
 * Parameters:  sock - The socket that is being created.
 *              user_context - unused by UDP.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_STATIC int
ipnet_sock_udp_init(Ipnet_socket *sock, Ip_bool user_context)
{
    Ipnet_sock_udp_ops *ops = (Ipnet_sock_udp_ops *) sock->ops;

    (void) user_context;
    sock->proto      = IP_IPPROTO_UDP;
    sock->chk_offset = 6;
    if (ipcom_sysvar_get_as_int("ipnet.inet.UdpChecksum", 1))
        IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_ADDCHECKSUM);

    if (ops->network_init != IP_NULL)
        return ops->network_init(sock, user_context);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_sock_udp_bind
 *===========================================================================
 * Description: The UDP network layer bind.
 * Parameters:  sock - The socket to bind.
 *              name - The socket name.
 *              namelen - The length of the name variable.
 *              check_reuse - IP_TRUE if the port should be checked for reuse.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_STATIC int
ipnet_sock_udp_bind(struct Ipnet_socket_struct *sock,
                    IP_CONST struct Ip_sockaddr *name,
                    Ip_socklen_t namelen,
                    Ip_bool check_reuse)
{
    int                 ret;
    Ipnet_sock_udp_ops *ops = (Ipnet_sock_udp_ops *) sock->ops;

    ret = ops->network_bind(sock, name, namelen, check_reuse);
    if (ret >= 0)
        ret = ipnet_sock_bind_to_port(sock, name, check_reuse);
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_sock_udp_connect
 *===========================================================================
 * Description: The UDP transport layer connect.
 * Parameters:  sock - The socket to connect.
 *              to - The address to connect to.
 *              tolen - The size of the 'to' buffer.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_STATIC int
ipnet_sock_udp_connect(Ipnet_socket *sock, IP_CONST struct Ip_sockaddr *to, Ip_socklen_t tolen)
{
    Ipnet_sock_udp_ops *ops = (Ipnet_sock_udp_ops *) sock->ops;
    int                 ret;

    ret = ops->network_connect(sock, to, tolen);
    if (ret < 0)
        return ret;

    if (ret == 1)
        /* Socket was unconnected */
        return 0;

    if (sock->dport == 0)
        /* Invalid port */
        return -IP_ERRNO_EINVAL;

    IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_CONNECTED);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_sock_udp_send
 *===========================================================================
 * Description: Adds a UDP header to the application data.
 * Parameters:  sock - The socket to use when sending.
 *              msg - The send parameters.
 *              pkt - Packet that contains the UDP payload.
 * Returns:     ==0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_sock_udp_send(Ipnet_socket *sock, IP_CONST struct Ip_msghdr *msg, Ipcom_pkt *pkt)
{
    Ipnet_sock_udp_ops *ops = (Ipnet_sock_udp_ops *) sock->ops;
    Ip_u16              to_port_n;
    Ipnet_pkt_udp      *udp;
    int                 ret;

    IPCOM_WV_MARKER_1 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_VERBOSE, 1, 1, IPCOM_WV_NETDEVENT_START,
                       ipnet_sock_udp_send, IPCOM_WV_IPNET_UDP_MODULE, IPCOM_WV_NETD_UDP4);
    IPNET_STATS(udp_output++);
    ip_assert(sock->ipcom.type == IP_SOCK_DGRAM);
    ip_assert(sock->proto == IP_IPPROTO_UDP);

    if (msg->msg_name == IP_NULL)
        to_port_n = ip_htons(sock->dport);
    else
        /* Ok for both IPv4 and IPv6 */
        to_port_n = ((struct Ip_sockaddr_in*) msg->msg_name)->sin_port;

    pkt->start -= IPNET_UDP_HDR_SIZE;

    /* Fill in UDP header. */
    udp = (Ipnet_pkt_udp *)&pkt->data[pkt->start];

    if (sock->sport)
        udp->sport = ip_htons(sock->sport);
    else
    {
        /* UDP socket unbound, get a temporary port. */
        udp->sport = ipnet_next_ephemeral_port(sock);
        if (udp->sport == 0)
        {
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                              1, 2, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_SEND,
                              ipnet_sock_udp_send, IPCOM_WV_NETD_ADDRINUSE,
                              IPCOM_WV_IPNET_UDP_MODULE, IPCOM_WV_NETD_UDP4);
            IPNET_STATS(udp_output_eaddrinuse++);
            ret = -IP_ERRNO_EADDRINUSE;
            goto errout;
        }
        udp->sport = ip_htons(udp->sport);
    }

    udp->dport = to_port_n;
    udp->sum   = 0;
#ifdef IPCOM_ZEROCOPY
    udp->ulen  = (Ip_u16) ip_htons(ipcom_pkt_get_length(pkt));
#else
    udp->ulen  = (Ip_u16) ip_htons(pkt->end - pkt->start);
#endif

    /* Update the checksum with the UDP header */
#ifdef IPCOM_USE_HW_CHECKSUM_TX
    if (IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM))
#endif
#ifdef IPCOM_ZEROCOPY
    	if (IP_BIT_ISFALSE(msg->msg_flags, IP_MSG_ZBUF))
#endif
    		pkt->chk += ipcom_in_checksum_update(&pkt->data[pkt->start], IPNET_UDP_HDR_SIZE);
    IPCOM_MIB2(udpOutDatagrams++);
    IPCOM_MIB2_U64_INC(udpHCOutDatagrams);

    return ops->inet.network_send(sock, msg, pkt);

errout:
    ipcom_pkt_free(pkt);
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_udp_nat_traversal
 *===========================================================================
 * Description: Check if this packet should be processed by the NAT
 *              traversal functionality.
 * Parameters:  sock        - The socket
 *              pkt         - The packet in question.
 * Returns:     IP_FALSE; give to UDP socket.
 *              IP_TRUE; Do NAT_T processing.
 *
 */
IP_STATIC Ip_bool
ipnet_udp_encapsulation(Ipnet_socket *sock, Ipcom_pkt *pkt, int *nat_t)
{
    /* Did we have any NAT-T's? */
    if (IP_UNLIKELY(sock->udp_encapsulation != IP_UDP_NAT_T_DISABLED))
    {
#if defined(IPNET_USE_TUNNEL) && defined(IPCOM_USE_INET)
        /* NAT-T processing? */
        if (sock->ipcom.domain == IP_AF_INET && sock->udp_encapsulation == IP_UDP_NAT_T_RFC3519)
        {
            Ip_u8               *data;
            Ip_u8               nproto;

            /* Sanity; There MUST be atleast 4 bytes available */
            if ((pkt->end - pkt->start) < 4)
            {
                /* Give to UDP socket */
                return IP_FALSE;
            }

            /* Grab the start */
            data = (Ip_u8 *) &pkt->data[pkt->start];

            /* NAT-T according to RFC3519
             * IP HEADER <tunnel endpoint - tunnel endpoint>
             * UDP HEADER
             * 4 byte NAT-T header;
             *  TYPE(1 byte) := 4
             *  NH(1 byte)   := 4, 47 or 55
             *  RESERVED(2 bytes) := 0
             */

            /* Must be 4 */
            if (data[0] != 4)
            {
                /* Give to UDP socket */
                return IP_FALSE;
            }

            /* Verify that the tunnel protocol is correct */
            nproto = data[1];
            if (nproto == IP_IPPROTO_IPIP
                || nproto == IP_IPPROTO_GRE
                || nproto == IP_IPPROTO_MOBILE)
            {
                /* Correct UDP RFC3519 encap; do process */
                *nat_t = IP_UDP_NAT_T_RFC3519;
                return IP_TRUE;
            }

            /* Feed to socket */
            return IP_FALSE;
        }
#else
        (void) pkt;
        (void) nat_t;
#endif /* IPNET_USE_TUNNEL */


#if defined(IPIPSEC2) && defined(IPCOM_USE_INET)
        if (sock->ipcom.domain == IP_AF_INET && sock->udp_encapsulation == IP_UDP_NAT_T_RFC3948)
        {
            if ((pkt->end - pkt->start == 1) && (pkt->data[pkt->start] == 0xff))
            {
                /* NAT-keepalive Packet; Give to socket */
                return IP_FALSE;
            }

            if ((pkt->end - pkt->start > 4) && (IP_GET_32ON16(&pkt->data[pkt->start]) == 0))
            {
                /* Non-ESP marker set; do give to socket - this is an IKE packet */
                return IP_FALSE;
            }

            /* Deal with the NAT-T for IKE; IPSEC */
            *nat_t = IP_UDP_NAT_T_RFC3948;

            /* Tell the world we need further processing */
            return IP_TRUE;
        }
#endif

#ifdef IPNET_USE_TUNNEL
        /* NAT-T processing? */
        if (sock->udp_encapsulation == IP_UDP_NAT_T_DSMIPV6)
        {
            /* Correct UDP RFC3519 encap; do process */
            *nat_t = IP_UDP_NAT_T_DSMIPV6;
            return IP_TRUE;
        }
#endif
    }

    return IP_FALSE;
}


/*
 *===========================================================================
 *                     ipnet_udp_fast_deliver_data
 *===========================================================================
 * Description: Deliver an UDP packet to a socket that is bound to a unicast
 *              address.
 * Parameters:  sock - The socket the packet will be delivered to.
 *              pkt - The UDP packet to deliver.
 *              nat_t - NAT_T processing.
 * Returns:     IP_TRUE - The fast path processing was successful.
 *              IP_FALSE - Need to use the normal receive path.
 *
 */
IP_STATIC Ip_bool
ipnet_udp_fast_deliver_data(Ipnet_socket *sock, Ipcom_pkt *pkt, int *nat_t)
{
    Ipnet_pkt_udp       *udp_hdr;
    int                  original_start = pkt->start;
    Ipcom_socket_eventcb event_cb = sock->ipcom.event_cb;

    udp_hdr = (Ipnet_pkt_udp*) &pkt->data[pkt->tlstart];
    if (IP_UNLIKELY(pkt->end - pkt->tlstart < IPNET_UDP_HDR_SIZE
                    || pkt->end - pkt->tlstart < ip_ntohs(udp_hdr->ulen)))
        return IP_FALSE;

    if (udp_hdr->sum != 0 && IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_LOOPED_BACK))
    {
        Ip_u16 chksum;
        /* Only check packets that was not created locally */
#ifdef IPCOM_USE_HW_CHECKSUM_RX
        if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM))
            chksum = 0;
        else if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_TL_CHECKSUM))
            chksum = ipcom_in_checksum_finish(pkt->chk);
        else
#endif /* IPCOM_USE_HW_CHECKSUM_RX */
        {
            pkt->chk += ipcom_in_checksum_update(&pkt->data[pkt->tlstart],
                                                 (Ip_size_t) ip_ntohs(udp_hdr->ulen));
            chksum = ipcom_in_checksum_finish(pkt->chk);
        }

        if (IP_UNLIKELY(chksum != 0))
            return IP_FALSE;
    }

    pkt->fd    = sock->ipcom.fd;
    pkt->start = pkt->tlstart + IPNET_UDP_HDR_SIZE;

    if (IP_UNLIKELY(ipnet_udp_encapsulation(sock, pkt, nat_t)))
    {
        /* Continue processing */
        ipcom_pkt_set_info(pkt, IPNET_PKT_INFO_ENCAP_UDP, sizeof(*udp_hdr), udp_hdr);
        pkt->start = original_start;
        return IP_FALSE;
    }

    if (ipnet_queue_received_packet(pkt, sock) != 0)
    {
        pkt->start = original_start;
        return IP_FALSE;
    }

    if (event_cb != IP_NULL)
        event_cb(&sock->ipcom, pkt, IP_SOEVENT_CB_INPKT);

    IPCOM_MIB2(udpInDatagrams++);
    IPCOM_MIB2_U64_INC(udpHCInDatagrams);
    return IP_TRUE;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_udp_input
 *===========================================================================
 * Description: Input handler for packet to UDP sockets. This function will
 *              NOT free the packet if the operation fails.
 * Parameters:  pkt - The received packet, must be freed by the called if
 *              this function fails.
 *              is_multicast - Set to IP_TRUE if the packet was sent to a
 *              multicast address.
 *              proto - The protocol number.
 *              src_addr - The address (Ipv4 or IPv6) of the sender.
 *              dst_addr - The address (Ipv4 or IPv6) of the receiver (this system).
 *              socklookup - A pointer to a function that can return the socket
 *              that is associated with this packet.
 *              nat_t   - NAT traversal; if used.
 * Returns:     0 >= number of matching sockets, <0 = error code
 *
 */
IP_GLOBAL int
ipnet_udp_input(Ipcom_pkt *pkt, Ip_bool is_multicast,
                IP_CONST void* src_addr, IP_CONST void* dst_addr,
                Ipnet_sock_lookup_f socklookup,
                int   *nat_t)
{
    Ipnet_pkt_udp *udp_hdr;
    int            retval;
    int            ulen_h;
    Ipnet_socket  *sock;
    Ip_u16         src_port;
    Ip_u16         dst_port;
    Ip_bool        pkt_no_rx_cache;


    IPCOM_WV_MARKER_1 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_VERBOSE, 1, 3, IPCOM_WV_NETDEVENT_START,
                       ipnet_udp_input, IPCOM_WV_IPNET_UDP_MODULE, IPCOM_WV_NETD_UDP4);
    IPNET_STATS(udp_input++);
    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_UDP_INPUT);

    if (IP_UNLIKELY(pkt->end - pkt->start < IPNET_UDP_HDR_SIZE))
    {
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                          1, 4, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                          ipnet_udp_input, IPCOM_WV_NETD_BADHLEN,
                          IPCOM_WV_IPNET_UDP_MODULE, IPCOM_WV_NETD_UDP4);
        IPNET_STATS(udp_input_hdr_trunc++);
        IPCOM_MIB2(udpInErrors++);
        return -IP_ERRNO_EINVAL;
    }

    udp_hdr = (Ipnet_pkt_udp*) &pkt->data[pkt->start];
    ulen_h = ip_ntohs(udp_hdr->ulen);
    src_port = ip_ntohs(udp_hdr->sport);
    dst_port = ip_ntohs(udp_hdr->dport);

    if (IP_UNLIKELY(pkt->end - pkt->start < ulen_h))
    {
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                          1, 5, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                          ipnet_udp_input, IPCOM_WV_NETD_BADLEN,
                          IPCOM_WV_IPNET_UDP_MODULE, IPCOM_WV_NETD_UDP4);
        IPNET_STATS(udp_input_hdr_trunc++);
        IPCOM_MIB2(udpInErrors++);
        return -IP_ERRNO_EINVAL;
    }
    pkt->end = pkt->start + ulen_h;

    if (udp_hdr->sum != 0 && IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_LOOPED_BACK))
    {
        Ip_u16 chksum;
        /* Only check packets that was not created locally */
#ifdef IPCOM_USE_HW_CHECKSUM_RX
        if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM))
            chksum = 0;
        else if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_TL_CHECKSUM))
            chksum = ipcom_in_checksum_finish(pkt->chk);
        else
#endif /* IPCOM_USE_HW_CHECKSUM_RX */
        {
            pkt->chk += ipcom_in_checksum_update(&pkt->data[pkt->start], (Ip_size_t) ulen_h);
            chksum = ipcom_in_checksum_finish(pkt->chk);
        }

        if (IP_UNLIKELY(chksum != 0))
        {
            /* Wrong checksum */
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                              1, 6, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                              ipnet_udp_input, IPCOM_WV_NETD_BADSUM,
                              IPCOM_WV_IPNET_UDP_MODULE, IPCOM_WV_NETD_UDP4);
            IPNET_STATS(udp_input_badchksum++);
            IPCOM_MIB2(udpInErrors++);
            return -IP_ERRNO_EINVAL;
        }
    }

    /* Move the start to beginning of application data */
    pkt->start += IPNET_UDP_HDR_SIZE;

    if (is_multicast)
    {
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                          1, 7, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                          ipnet_udp_input, IPCOM_WV_NETD_INFO_RECEIVE,
                          IPCOM_WV_IPNET_UDP_MODULE, IPCOM_WV_NETD_UDP4);
        IPNET_STATS(udp_input_multicast++);
        retval = ipnet_raw_input(pkt, IP_FALSE, IP_IPPROTO_UDP,
                                 src_addr, src_port,
                                 dst_addr, dst_port,
                                 socklookup);
        if (retval >= 0)
        {
            /* The packet must be freed unlesss an error occured */
            ipcom_pkt_free(pkt);
            IPCOM_MIB2(udpInDatagrams++);
            IPCOM_MIB2_U64_INC(udpHCInDatagrams);
        }
        else
        {
            IPCOM_MIB2(udpInErrors++);
        }
        return retval;
    }

    sock = socklookup(pkt->vr_index,
                      IP_IPPROTO_UDP,
                      dst_addr,
                      dst_port,
                      src_addr,
                      src_port);
    if (sock == IP_NULL)
        /* No matching socket, try the wildcard group */
        sock = socklookup(IPCOM_VR_ANY,
                          IP_IPPROTO_UDP,
                          dst_addr,
                          dst_port,
                          src_addr,
                          src_port);

    if (sock != IP_NULL)
    {
        Ipcom_socket_eventcb event_cb = sock->ipcom.event_cb;

        /* Verify NAT traversal */
        if (IP_UNLIKELY(ipnet_udp_encapsulation(sock, pkt, nat_t)))
        {
            ipcom_pkt_set_info(pkt, IPNET_PKT_INFO_ENCAP_UDP, sizeof(*udp_hdr), udp_hdr);
            pkt->start -= IPNET_UDP_HDR_SIZE;
            return 0;
        }

        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                          1, 8, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                          ipnet_udp_input, IPCOM_WV_NETD_INFO_RECEIVE,
                          IPCOM_WV_IPNET_UDP_MODULE, IPCOM_WV_NETD_UDP4);
        IPNET_STATS(udp_input_sock_match++);
        pkt->fd = sock->ipcom.fd;
        pkt_no_rx_cache = IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_NO_RX_CACHE);

        retval = ipnet_queue_received_packet(pkt, sock);
        if (retval < 0)
        {
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                              1, 9, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                              ipnet_udp_input, IPCOM_WV_NETD_INFO_QUEUE_FULL,
                              IPCOM_WV_IPNET_UDP_MODULE, IPCOM_WV_NETD_UDP4);
            IPNET_STATS(udp_input_queue_pkt_err++);
            IPCOM_MIB2(udpInErrors++);
            return retval;
        }

        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                          1, 10, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                          ipnet_udp_input, IPCOM_WV_NETD_INFO_RECEIVE,
                          IPCOM_WV_IPNET_UDP_MODULE, IPCOM_WV_NETD_UDP4);
        IPNET_STATS(udp_input_queue_pkt_ok++);

        if (event_cb != IP_NULL)
            event_cb(&sock->ipcom, pkt, IP_SOEVENT_CB_INPKT);

        IPCOM_MIB2(udpInDatagrams++);
        IPCOM_MIB2_U64_INC(udpHCInDatagrams);

        if (IP_BIT_ISFALSE(sock->flags, IPNET_SOCKET_FLAG_RX_CACHED)
            && !pkt_no_rx_cache)
            ipnet_sock_add_to_rx_cache(sock,
                                       src_addr,
                                       src_port,
                                       dst_addr,
                                       dst_port,
                                       ipnet_udp_fast_deliver_data);

        return 1;
    }
    else
    {
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                          1, 13, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                          ipnet_udp_input, IPCOM_WV_NETD_NOPORT,
                          IPCOM_WV_IPNET_UDP_MODULE, IPCOM_WV_NETD_UDP4);
        IPNET_STATS(udp_input_sock_nomatch++);
        IPCOM_MIB2(udpNoPorts++);
    }

    IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                      1, 14, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                      ipnet_udp_input, IPCOM_WV_NETD_NOPORT,
                      IPCOM_WV_IPNET_UDP_MODULE, IPCOM_WV_NETD_UDP4);
    IPNET_STATS(udp_input_econnrefused++);
    return -IP_ERRNO_ECONNREFUSED;
}


/*
 *===========================================================================
 *                    ipnet_sock_udp_register
 *===========================================================================
 * Description: Registers the UDP protocol for IPv4 and/or IPv6.
 * Parameters:
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_sock_udp_register(void)
{
    Ipnet_sock_udp_ops *ops;
    int domains[] = {
#ifdef IPCOM_USE_INET
        IP_AF_INET,
#endif
#ifdef IPCOM_USE_INET6
        IP_AF_INET6,
#endif
    };
    int       protos[] = { 0, IP_IPPROTO_UDP };
    Ip_size_t d, p;
    int       ret;

    for (d = 0; d < sizeof(domains) / sizeof(domains[0]); d++)
        for (p = 0; p < sizeof(protos) / sizeof(protos[0]); p++)
        {
            ops = ipcom_calloc(1, sizeof(*ops));
            if (ops == IP_NULL)
            {
                IPCOM_LOG0(CRIT, "Could not register the UDP protocol, out of memory");
                IP_PANIC();
                return -IP_ERRNO_ENOMEM;
            }

            switch (domains[d])
            {
#ifdef IPCOM_USE_INET
            case IP_AF_INET:
                ipnet_sock_ip4_get_ops(&ops->inet);
                break;
#endif
#ifdef IPCOM_USE_INET6
            case IP_AF_INET6:
                ipnet_sock_ip6_get_ops(&ops->inet);
                break;
#endif
            default:
                break;
            }

            ops->network_bind          = ops->inet.network_bind;
            ops->inet.network_bind     = ipnet_sock_udp_bind;

            ops->network_init          = ops->inet.sock.init;
            ops->network_connect       = ops->inet.sock.connect;
            ops->inet.sock.init        = ipnet_sock_udp_init;
            ops->inet.sock.connect     = ipnet_sock_udp_connect;

            ops->inet.sock.type        = IP_SOCK_DGRAM;
            ops->inet.sock.proto       = protos[p];
            ops->inet.sock.allow_async_send = IP_TRUE;
            ops->inet.sock.usr_recv    = ipnet_usr_sock_recvmsg;
            ops->inet.sock.send        = ipnet_sock_udp_send;
            ops->inet.sock.hdr_space  += IPNET_UDP_HDR_SIZE;

            ret = ipnet_sock_register_ops(&ops->inet.sock);
            if (ret < 0)
                return ret;
        }
    return 0;
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

