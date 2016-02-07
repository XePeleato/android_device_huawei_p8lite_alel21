/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_sock_tcp.c,v $ $Revision: 1.39 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_sock_tcp.c,v $
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
 * TCP specific socket operations
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

#ifdef IPTCP

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_syslog.h>
#include <ipcom_type.h>

#include <iptcp.h>

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

typedef struct Ipnet_sock_tcp_ops_struct
{
    Ipnet_sock_inet_ops  inet;
    Ipnet_sock_init_f    network_init;
    Ipnet_sock_destroy_f network_destroy;
    Ipnet_sock_connect_f network_connect;
    Ipnet_sock_bind_f    network_bind;
}
Ipnet_sock_tcp_ops;


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
 *                    ipnet_sock_tcp_init
 *===========================================================================
 * Description: The TCP transport layer initialization.
 * Parameters:
 *              user_context - IP_TRUE if this function is called in user process context
 *                             IP_FALSE if it called from stack context.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_STATIC int
ipnet_sock_tcp_init(Ipnet_socket *sock, Ip_bool user_context)
{
    Ipnet_sock_tcp_ops *ops = (Ipnet_sock_tcp_ops *) sock->ops;

    sock->proto = IP_IPPROTO_TCP;
    if (user_context)
        /* Socket create in user-context, add an extra reference for the TCP module */
        ipcom_atomic_inc(&sock->ref_count);
    /* else: the TCP module will add an reference for the user when returning
       the socket in accept() */
    sock->chk_offset = 16;
    IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_ADDCHECKSUM);

    sock->tcb = ipnet_sock_alloc_private_data(sock, sizeof(Iptcp_tcb));
    iptcp_sock_init(sock);

    if (ops->network_init != IP_NULL)
        return ops->network_init(sock, user_context);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_sock_tcp_destroy
 *===========================================================================
 * Description: Frees all resources held by the TCP socket.
 * Parameters:  sock - The socket that is beeing destroyed.
 * Returns:
 *
 */
IP_STATIC void
ipnet_sock_tcp_destroy(Ipnet_socket *sock)
{
    Ipnet_sock_tcp_ops *ops = (Ipnet_sock_tcp_ops *) sock->ops;
    if (ops->network_destroy != IP_NULL)
        ops->network_destroy(sock);
    iptcp_sock_free(sock);
}


/*
 *===========================================================================
 *                    ipnet_sock_tcp_bind
 *===========================================================================
 * Description: The TCP network layer bind.
 * Parameters:  sock - The socket to bind.
 *              name - The socket name.
 *              namelen - The length of the name variable.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_STATIC int
ipnet_sock_tcp_bind(struct Ipnet_socket_struct *sock,
                    IP_CONST struct Ip_sockaddr *name,
                    Ip_socklen_t namelen,
                    Ip_bool check_reuse)
{
    int                 ret;
    Ipnet_sock_tcp_ops *ops = (Ipnet_sock_tcp_ops *) sock->ops;

    ret = ops->network_bind(sock, name, namelen, check_reuse);
    if (ret >= 0)
        ret = ipnet_sock_bind_to_port(sock, name, check_reuse);
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_sock_tcp_connect
 *===========================================================================
 * Description: The UDP transport layer connect.
 * Parameters:  sock - The socket to connect.
 *              to - The address to connect to.
 *              tolen - The size of the 'to' buffer.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_STATIC int
ipnet_sock_tcp_connect(Ipnet_socket *sock, IP_CONST struct Ip_sockaddr *to, Ip_socklen_t tolen)
{
    Ipnet_sock_tcp_ops *ops = (Ipnet_sock_tcp_ops *) sock->ops;
    int                 ret;

    ret = ops->network_connect(sock, to, tolen);
    if (ret < 0)
        return ret;

    if (sock->dport == 0)
        /* Invalid port */
        return -IP_ERRNO_EINVAL;

    /* The socket is a TCP socket, do the TCP connect */
    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_CONNECTING))
        /* Asynchronous connect is still not finished */
        return -IP_ERRNO_EALREADY;

    if ((ret = ipnet_sock_set_reachable(sock)) < 0)
        return ret;

    return iptcp_connect(sock);
}


/*
 *===========================================================================
 *                    ipnet_usr_sock_tcp_recv
 *===========================================================================
 * Description: Receives data from a TCP stream.
 * Parameters:  sock - The socket to receive data from.
 *              msg - Receive parameters.
 *              flags - IP_MSG_xxx flags.
 * Returns:     0 = a packet was successfully received, -1 = error.
 *
 */
IP_STATIC int
ipnet_usr_sock_tcp_recv(Ipnet_socket *sock, struct Ip_msghdr *msg, int flags)
{
    Ip_size_t i;
    int       r;
    int       bytes = 0;

    /* No support for ancillary data on TCP sockets */
    msg->msg_controllen = 0;

    if (msg->msg_name != IP_NULL)
    {
        Ip_socklen_t msg_namelen = msg->msg_namelen;

        r = ipnet_sys_getname(sock->ipcom.fd,
                              msg->msg_name,
                              &msg_namelen,
                              IP_FALSE);
        if (r < 0)
            return r;
        msg->msg_namelen = msg_namelen;
    }

    for (i = 0; i < msg->msg_iovlen; i++)
    {
        r = iptcp_usr_recv(sock,
                           msg->msg_iov[i].iov_base,
                           msg->msg_iov[i].iov_len,
                           flags);

        if (r == 0)
            /* EOF received */
            break;

        if (r < 0)
        {
            if (bytes > 0)
                break;
            IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                              1, 1, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_RECV,
                              ipnet_usr_sock_tcp_recv, IPCOM_WV_NETD_BADSOCK,
                              IPCOM_WV_IPNET_SOCK_TCP_MODULE, IPCOM_WV_NETD_TCP4);
            IPNET_STATS(sock_recvmsg_tcp_err++);
            return r;
        }
        bytes += r;

        if ((Ip_size_t) r != msg->msg_iov[i].iov_len)
            /* Continue only if the current buffer was filled to its
               limit */
            break;
        if (IP_BIT_ISFALSE(flags, IP_MSG_WAITALL))
            /* We have some data, return the data we have so far if there
               isn't anything more to receive */
            IP_BIT_SET(flags, IP_MSG_DONTWAIT);
    }
    return bytes;
}


/*
 *===========================================================================
 *                     ipnet_sock_tcp_append_send_data
 *===========================================================================
 * Description: Append data to the last packet in the send queue up to
 *              a full MSS segment.
 * Parameters:  send_tail - The last element of the send queue.
 *              pbuf - Pointer to the data buffer pointer. The buffer
 *                     pointer (*pbuf) will point to the first byte
 *                     that has not been copied.
 *              plen - A pointer to the length length of *pbuf, will
 *                     contain the number of bytes not copied when this
 *                     function returns.
 *              mss - The maximum segment size for this segment.
 * Returns:     Number of bytes appended.
 *
 */
IP_STATIC int
ipnet_sock_tcp_append_send_data(Ipcom_pkt *send_tail,
                                Ip_u8 **pbuf,
                                int *plen,
                                int mss)
{
    int bytes_appended;
    int len = *plen;

    if (mss <= send_tail->end - send_tail->start)
        /* mss must have been decresed after the send_tail was created */
        return 0;

    bytes_appended = IP_MIN(mss - (send_tail->end - send_tail->start), len);
    ip_assert(bytes_appended >= 0);

    if (send_tail->chk != 0 && (send_tail->end & 0x1) == 0)
    {
        /* End is even, just add the checksum of the appended data to
           the current checksum */
        send_tail->chk += ipnet_in_checksum_memcpy(&send_tail->data[send_tail->end],
                                                   *pbuf,
                                                   bytes_appended);
    }
    else
    {
        /* The checksum will be wrong unless the last odd byte is
           included in the new checksum, since odd bytes is handled as
           if the byte behind it is 0 (which is probably false now
           when more data is appended) */
        ipnet_copy_from_user(&send_tail->data[send_tail->end],
                             *pbuf,
                             bytes_appended);
        /* Calculate the checksum when sending instead */
        send_tail->chk = 0;
    }

    send_tail->end += bytes_appended;

    ip_assert(send_tail->end <= send_tail->maxlen);

    *pbuf = *pbuf + bytes_appended;
    *plen -= bytes_appended;

    return bytes_appended;
}


/*
 *===========================================================================
 *                   ipnet_usr_sock_tcp_pkts_from_iov
 *===========================================================================
 * Description: Copies the user data into MSS sized packets.
 * Parameters:  sock - The socket that will be use to send.
 *              msg - 'msg_iov' contains the buffer(s) to send.
 *              flags - IP_MSG_xxx flags.
 *              offset - Number of bytes into the msg->msg_iov buffers
 *                      where the copy into packets should start.
 *              total_buf_len - the sum of the length of all
 *                      msg->msg_iov buffers in 'msg'.
 *              ppkt -  Will point to the created packet(s) if the
 *                      operation is successfil. The (*ppkt)->next
 *                      points to the next packet in case of more than
 *                      one packet was allocated.
 * Returns:     <0 = error code (-IPNET_ERRNO_xxx)
 *               0 = this operation released the socket lock, the
 *                      sendmsg operation must be restarted
 *              >0 = number of bytes that has been allocated in the
 *                      socket send buffer and copied into the
 *                      packet(s) pointed to by *ppkt.
 */
IP_STATIC int
ipnet_usr_sock_tcp_pkts_from_iov(Ipnet_socket *sock,
                                 IP_CONST struct Ip_msghdr *msg,
                                 int flags,
                                 int offset,
                                 int total_buf_len,
                                 Ipcom_pkt **ppkt)
{
    Ipcom_pkt *pkt_head;
    Ipcom_pkt *pkt_tail;
    Iptcp_tcb *tcb = sock->tcb;
    int        mss = tcb->mss;
    Ip_u8     *buf;
    int        buf_len;
    int        i = 0;
    int        len = 0;
    int        bytes_to_copy = total_buf_len - offset;
    Ip_bool    non_blocking;
    Ip_bool    urgent = IP_BIT_ISSET(flags, IP_MSG_OOB);

    *ppkt = IP_NULL;

    /*
     * If offset == total_buf_len, which can in particular happen
     * if both are zero, then the loop immediately below can
     * access beyond the end of the iovec array. Avoid this.
     */
    if (offset == total_buf_len)
        return 0;

    for (buf_len = 0; buf_len <= 0; offset = - buf_len)
    {
        struct Ip_iovec *iov = &msg->msg_iov[i++];

        buf = (Ip_u8 *) iov->iov_base + offset;
        buf_len = (int) iov->iov_len - offset;
    }

    if (IP_LIKELY(urgent == IP_FALSE))
    {
        iptcp_partial_lock(tcb->send.partial_lock);
        if (tcb->send.partial != IP_NULL)
        {
            len = ipnet_sock_tcp_append_send_data(tcb->send.partial,
                                                  &buf,
                                                  &buf_len,
                                                  mss);
            if (len > 0)
                ipcom_atomic_add(&sock->snd_bytes, len);
        }
        iptcp_partial_unlock(tcb->send.partial_lock);

        if (len == bytes_to_copy)
            /* All data that should be sent was queued to the end of the
               partial finished TCP segment */
            return len;
    }
    /* else: urgent packet must be sent in a separate signal since the
       flag field must have IP_MSG_OOB set for correct processing. */

    non_blocking = IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_NONBLOCKING)
        || IP_BIT_ISSET(flags, IP_MSG_DONTWAIT);

    bytes_to_copy = 0;
    do
    {
        int bytes_to_alloc = IP_MIN(mss, buf_len - bytes_to_copy);

        if (ipcom_atomic_add_and_return(&sock->snd_bytes, bytes_to_alloc)
            < sock->send_max_bytes + bytes_to_alloc)
        {
            bytes_to_copy += bytes_to_alloc;
        }
        else
        {
            int lowat;

            ipcom_atomic_sub(&sock->snd_bytes, bytes_to_alloc);

            if (bytes_to_copy > 0 || non_blocking)
                break;

            lowat = IP_MAX(mss, (int) sock->tcb->send.lowat);
            lowat = IP_MIN(lowat, sock->send_max_bytes);
            (void)ipnet_usr_sock_wait_until_writable(sock,
                                                     lowat,
                                                     IP_NULL);
            if (sock->ipcom.so_errno)
            {
                ipcom_atomic_sub(&sock->snd_bytes, bytes_to_copy);
                return len > 0 && sock->ipcom.so_errno != IP_ERRNO_EINTR ? len : -sock->ipcom.so_errno;
            }
        }
    } while (bytes_to_copy < buf_len);

    pkt_head = IP_NULL;
    pkt_tail = IP_NULL;
    while (bytes_to_copy > 0)
    {
        Ipcom_pkt *pkt;
        int        seg_len = IP_MIN(bytes_to_copy, mss);

        pkt = ipcom_pkt_malloc(mss + sock->max_hdrspace,
                               IP_FLAG_FC_STACKCONTEXT | (non_blocking ? 0 : IP_FLAG_FC_BLOCKOK));
        if (IP_UNLIKELY(pkt == IP_NULL))
            break;

        pkt->start = (pkt->maxlen - mss) & ~0x3;
        pkt->end   = pkt->start + seg_len;

        pkt->chk = ipnet_in_checksum_memcpy(&pkt->data[pkt->start],
                                            buf,
                                            seg_len);

        /* Add this packet to the list of packets that will be sent
           to ipnetd */
        if (pkt_head == IP_NULL)
            pkt_head = pkt;

        else
            pkt_tail->next = pkt;
        pkt_tail = pkt;

        len += seg_len;
        buf += seg_len;
        bytes_to_copy -= seg_len;
    }

    if (IP_LIKELY(pkt_tail != IP_NULL))
    {
        if (IP_UNLIKELY(urgent))
            IPNET_MARK_PKT_AS_URGENT(pkt_tail);

        iptcp_partial_lock(tcb->send.partial_lock);
        tcb->send.partial = pkt_tail;
        iptcp_partial_unlock(tcb->send.partial_lock);
    }

    *ppkt = pkt_head;
    return (len == 0 ? -IP_ERRNO_EWOULDBLOCK : len);
}



/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_sock_tcp_register
 *===========================================================================
 * Description: Registers the TCP protocol for IPv4 and/or IPv6.
 * Parameters:
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_sock_tcp_register(void)
{
    Ipnet_sock_tcp_ops *ops;
    int domains[] = {
#ifdef IPCOM_USE_INET
        IP_AF_INET,
#endif
#ifdef IPCOM_USE_INET6
        IP_AF_INET6,
#endif
    };
    int       protos[] = { 0, IP_IPPROTO_TCP };
    Ip_size_t p, d;
    int       ret;

    for (d = 0; d < sizeof(domains) / sizeof(domains[0]); d++)
        for (p = 0; p < sizeof(protos) / sizeof(protos[0]); p++)
        {
            ops = ipcom_calloc(1, sizeof(*ops));
            if (ops == IP_NULL)
            {
                IPCOM_LOG0(CRIT, "Could not register the TCP protocol, out of memory");
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

            ops->network_bind                = ops->inet.network_bind;
            ops->inet.network_bind           = ipnet_sock_tcp_bind;

            ops->network_init                = ops->inet.sock.init;
            ops->inet.sock.init              = ipnet_sock_tcp_init;
            ops->network_destroy             = ops->inet.sock.destroy;
            ops->inet.sock.destroy           = ipnet_sock_tcp_destroy;
            ops->network_connect             = ops->inet.sock.connect;
            ops->inet.sock.connect           = ipnet_sock_tcp_connect;

            ops->inet.sock.type              = IP_SOCK_STREAM;
            ops->inet.sock.proto             = protos[p];
            ops->inet.sock.allow_async_send  = IP_TRUE;
            ops->inet.sock.accept            = iptcp_accept;
            ops->inet.sock.close             = iptcp_close;
            ops->inet.sock.listen            = iptcp_listen;
            ops->inet.sock.usr_recv          = ipnet_usr_sock_tcp_recv;
            ops->inet.sock.pkts_from_iov     = ipnet_usr_sock_tcp_pkts_from_iov;
            ops->inet.sock.send              = iptcp_send;
            ops->inet.sock.shutdown          = iptcp_shutdown;
            ops->inet.sock.getopt            = iptcp_getsockopt;
            ops->inet.sock.setopt            = iptcp_setsockopt;
            ops->inet.sock.ioctl             = iptcp_ioctl;
            ops->inet.sock.hdr_space        += IPTCP_TCP_HDR_SIZE;
            ops->inet.sock.extra_sock_space += sizeof(Iptcp_tcb);

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

#endif /* IPTCP */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

