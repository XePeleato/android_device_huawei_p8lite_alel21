/* ipnet_sock.c - IPNET SCTP Socket API Extensions */

/*
 * Copyright (c) 2007 Wind River Systems, Inc.
 *
 * The right to copy, distribute or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement. No license to Wind River intellectual property rights
 * is granted herein. All rights not licensed by Wind River are reserved
 * by Wind River.
 */

/*
modification history
--------------------
01a,24jul08,fei  Created, base on kcn 19nov07 version.
*/

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 *
 * Sockets API Extensions for SCTP. Implementation is based on SCTP Socket
 * draft-15.
 *
 * The socket layer for SCTP is an optional layer which sits above the
 * SCTP ULP. It is the implementation goal to have this layer handles all
 * the socket specific details and simply calls the SCTP ULP routine for
 * futher SCTP specific processing.
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
#ifdef IPSCTP

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_syslog.h>
#include <ipcom_type.h>

#include "ipcom_sock.h"
#include "ipnet_h.h"
#include "ipnet_usr_sock.h"


#include <ipsctp.h>
#include <ipsctp_sock.h>


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

typedef struct Ipnet_sock_sctp_ops_struct
{
    Ipnet_sock_inet_ops  i;
    Ipnet_sock_init_f    network_init;
    Ipnet_sock_destroy_f network_destroy;
    Ipnet_sock_connect_f network_connect;
    Ipnet_sock_bind_f    network_bind;
#ifdef IPCOM_USE_INET
    Ipnet_sock_bind_f    network_bind_v4;
#endif
#ifdef IPCOM_USE_INET6
    Ipnet_sock_bind_f    network_bind_v6;
#endif
}
Ipnet_sock_sctp_ops;

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
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_sock_sctp_init
 *===========================================================================
 * Description: SCTP socket layer initialization.
 * Parameters: sock - The socket that is beeing created.
 *             user_context - IP_TRUE if this function is called in user 
 *                            process context. IP_FALSE if it called from 
 *                            stack context.
 * Returns: 0 = success, <0 = error code
 */
IP_STATIC int
ipnet_sock_sctp_init(Ipnet_socket *sock, 
                     Ip_bool user_context)
{
    Ipnet_sock_sctp_ops *ops = IP_NULL;
    int                  ret = 0;

    if (IP_NULL == sock)
    {
        ip_assert(IP_FALSE);
        return -IP_ERRNO_EINVAL;
    }

    ops = (Ipnet_sock_sctp_ops *)sock->ops;
    if (IP_NULL == ops)
    {
        ip_assert(IP_FALSE);
        return -IP_ERRNO_EINVAL;
    }
    
    sock->proto = IP_IPPROTO_SCTP;

    /*
     * If socket is created in user-context, add an extra reference for the
     * SCTP module. Otherwise, the SCTP modul will add a reference for the
     * user when returning the socket in accept().
     * NOTE : we will wait until the shutdown process finish, if we send
     *        SHUTDOWN packet, so socket also need be taken till that
     *        time. [20081120]
     */
    if (IP_TRUE == user_context)
        ipcom_atomic_inc(&sock->ref_count);

    /*
     * Defines the offset into the SCTP transport layer where the checksum 
     * should be stored. Other transport protocols such as TCP and UDP use
     * 16 bit checksum but SCTP checksum is 32 bits. So perhaps the chk_offset
     * should not be set, which means the SCTP transport layer will have to
     * validate the checksum in the SCTP common header itself.
     */
    sock->chk_offset = 0;
    IP_BIT_CLR(sock->flags, IPNET_SOCKET_FLAG_ADDCHECKSUM);
    
    sock->sctp_inst = ipnet_sock_alloc_private_data(sock, 
                                                    IPSCTP_SIZE_SOCK_DATA );
    ipcom_memset(sock->sctp_inst, 0, IPSCTP_SIZE_INST);
    /* init socket IP4 or IP6 data, and set bound flag */
    if (IP_NULL != ops->network_init)
    {
        ret = ops->network_init(sock, user_context);
        if (ret < 0)
            return ret;
    }

    sock->pkt_dtor = IP_NULL;
    if ((ret = ipsctp_sock_init(sock)) < 0)
        return ret;

    return 0;
}

/*
 *===========================================================================
 *                    ipnet_sock_sctp_destroy
 *===========================================================================
 * Description: Frees all resources held by the SCTP socket.
 * Parameters:  sock - The socket that is beeing destroyed.
 * Returns:
 */
IP_STATIC void
ipnet_sock_sctp_destroy(Ipnet_socket *sock)
{
    Ipnet_sock_sctp_ops *ops = IP_NULL;
    
    if (IP_NULL == sock)
    {
        ip_assert(IP_FALSE);
        return;
    }

    ops = (Ipnet_sock_sctp_ops *)sock->ops;
    if (IP_NULL == ops)
    {
        ip_assert(IP_FALSE);
        return;
    }
    
    if (ops->network_destroy != IP_NULL)
        ops->network_destroy(sock);
    
    ipsctp_sock_free(sock);

    return;
}

/*
 *===========================================================================
 *                    ipnet_sock_sctp_bind
 *===========================================================================
 * Description: The SCTP network layer bind.
 * Parameters:  sock - The socket to bind.
 *              name - The socket name.
 *              namelen - The length of the name variable.
 *              check_reuse - Check reuse for port or address.
 * Returns:     0 = success, <0 = error code
 */
IP_STATIC int
ipnet_sock_sctp_bind(struct Ipnet_socket_struct *sock,
                     IP_CONST struct Ip_sockaddr *name,
                     Ip_socklen_t namelen,
                     Ip_bool check_reuse)
{
    Ipnet_sock_sctp_ops *ops       = IP_NULL;
    int                  ret       = 0;

    if ((IP_NULL == sock)||(IP_NULL == name)||(namelen <= 0))
    {
        ip_assert(IP_FALSE);
        return -IP_ERRNO_EINVAL;
    }

    ops = (Ipnet_sock_sctp_ops *)sock->ops;
    if (IP_NULL == ops)
    {
        ip_assert(IP_FALSE);
        return -IP_ERRNO_EINVAL;
    }
    
    /* call the common function: 
     *  1)do bound IP address verifying
     *  2)add to Ipnet_ip4_addr_entry 
     */
    ret = ops->network_bind(sock, name, namelen, check_reuse);
    if (ret >= 0)
    {
        ret = ipnet_sock_bind_to_port(sock, name, check_reuse);
        if (ret >= 0)
            ret = ipsctp_bind(sock, name, namelen);
    }
    /* do rollback in ipcom_bind for error */
    return ret;
}

/*
 *===========================================================================
 *                    ipnet_sock_sctp_connect
 *===========================================================================
 * Description: The SCTP transport layer connect.
 * Parameters:  sock - The socket to connect.
 *              to - The address to connect to.
 *              tolen - The size of the 'to' buffer.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_STATIC int
ipnet_sock_sctp_connect(Ipnet_socket *sock, 
                        IP_CONST struct Ip_sockaddr *to,
                        Ip_socklen_t tolen)
{
    Ipnet_sock_sctp_ops *ops = IP_NULL;
    int                  ret = 0;

    if (IP_NULL == sock)
    {
        ip_assert(IP_FALSE);
        return -IP_ERRNO_EINVAL;
    }

    ops = (Ipnet_sock_sctp_ops *)sock->ops;
    if (IP_NULL == ops)
    {
        ip_assert(IP_FALSE);
        return -IP_ERRNO_EINVAL;
    }

    /* call the common function: 
     *  1)do binding operation if have not bind now
     *  2)check the address validation 
     */
    ret = ops->network_connect(sock, to, tolen);
    if (ret < 0)
        return ret;

    if (0 == sock->dport)
        /* Invalid port */
        return -IP_ERRNO_EINVAL;

    /* The socket is a sctp socket, do the sctp connect */
    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_CONNECTING))
        /* Asynchronous connect is still not finished */
        return -IP_ERRNO_EALREADY;

    return ipsctp_connect(sock, to);
}

/*
 *===========================================================================
 *                   ipnet_sock_usr_pkts_from_iov_sctp
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
ipnet_sock_usr_pkts_from_iov_sctp(Ipnet_socket *sock,
                                  IP_CONST struct Ip_msghdr *msg,
                                  int flags,
                                  int offset,
                                  int total_buf_len,
                                  Ipcom_pkt **ppkt)
{
    struct Ip_cmsghdr *cmsghdr       = IP_NULL;
    struct Ipsctp_sndrcvinfo *sndrcv = IP_NULL;
    Ipcom_pkt         *pkt_head      = IP_NULL;
    Ipcom_pkt         *pkt_tail      = IP_NULL;
    Ipsctp_instance   *inst          = IP_NULL;
    Ip_u8             *buf           = IP_NULL;
    int                buf_len       = 0;
    int                i             = 0;
    int                len           = 0;
    int                bytes_to_copy = total_buf_len - offset;
    Ip_bool            non_blocking  = IP_FALSE;
    Ip_u32             pmtu          = 0;
    Ip_u32             mss           = 0;
    void               *tcb          = IP_NULL;    
    
    ip_assert(IP_NULL != msg);
    ip_assert(IP_NULL != sock);
    inst = (Ipsctp_instance *)sock->sctp_inst;
    ip_assert(IP_NULL != inst);

    /* loop the ancillary data, seach for what we needed */
    for (cmsghdr = (struct Ip_cmsghdr *)IP_CMSG_FIRSTHDR(msg); 
         IP_NULL != cmsghdr; 
         cmsghdr = (struct Ip_cmsghdr *)IP_CMSG_NXTHDR(msg, cmsghdr)) 
    {
        /* check message level */
        if (IP_IPPROTO_SCTP != cmsghdr->cmsg_level)
            continue;
        
        /* check message type */
        if (IPSCTP_SNDRCV != cmsghdr->cmsg_type)
            continue;

        /* check send-receive length */
        if (cmsghdr->cmsg_len < IP_CMSG_LEN(sizeof(*sndrcv)))
        {
            IPCOM_LOG2(ERR,
                       "control message size is %u, should be %u", 
                       cmsghdr->cmsg_len, 
                       IP_CMSG_LEN(sizeof(*sndrcv)));
            *ppkt = IP_NULL;
            return 0;
        }

        sndrcv = (struct Ipsctp_sndrcvinfo *)IP_CMSG_DATA(cmsghdr);
        break;
    }

    non_blocking = IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_NONBLOCKING)
                   || IP_BIT_ISSET(flags, IP_MSG_DONTWAIT);
    /*
     * If offset == total_buf_len, which can in particular happen
     * if both are zero, then the loop immediately below can
     * access beyond the end of the iovec array. Avoid this.
     */
    if (offset == total_buf_len)
    {
        /* for only EOF/ABORT flag set and no data need to be sent. */
        if ((0 == total_buf_len)
            &&(sndrcv)
            &&((IPSCTP_EOF & sndrcv->sinfo_flags) || (IPSCTP_ABORT & sndrcv->sinfo_flags)))
            /* only hdr alloc */
            *ppkt = ipcom_pkt_malloc(0,
                                     IP_FLAG_FC_STACKCONTEXT | (non_blocking ? 0 : IP_FLAG_FC_BLOCKOK));
        else
            *ppkt = IP_NULL;
        return 0;
    }

    IPNET_DATA_LOCK(sock->sctp_inst->tcb_lock);
    
    if (sock->sctp_inst->get_tcb_info(sock, msg, sndrcv, &pmtu, &mss, (void **)&tcb))
    {
        IPCOM_LOG0(WARNING, "IPSCTP : no TCB");
        *ppkt = IP_NULL;        
        IPNET_DATA_UNLOCK(sock->sctp_inst->tcb_lock);
        return -IP_ERRNO_ENOTCONN;
    }
    /* release the lock if tcb is null */
    if(tcb == IP_NULL)
    {
        IPNET_DATA_UNLOCK(sock->sctp_inst->tcb_lock);
    }

    for (buf_len = 0; buf_len <= 0; offset = - buf_len)
    {
        struct Ip_iovec *iov = &msg->msg_iov[i++];

#if 1
        if (IP_UNLIKELY(IP_NULL == iov->iov_base))
            return -IP_ERRNO_EFAULT;
#endif

        buf     = (Ip_u8 *) iov->iov_base + offset;
        buf_len = (int) iov->iov_len - offset;
    }
    bytes_to_copy = 0;

    do
    {
        int bytes_to_alloc = IP_MIN(mss, buf_len - bytes_to_copy);

        if (ipsctp_usr_sock_sendbuf_add_and_return(sock, tcb, bytes_to_alloc)
            < sock->send_max_bytes + bytes_to_alloc)
        {
            bytes_to_copy += bytes_to_alloc;
        }
        else
        {
            int lowat = 0;

            ipsctp_usr_sock_sendbuf_sub(sock, tcb, bytes_to_alloc);

            if (bytes_to_copy > 0 || non_blocking)
                break;

            lowat = IP_MAX(mss, inst->send_lowat);
            lowat = IP_MIN(lowat, sock->send_max_bytes);
            /* release the lock if tcb is non-null */
            if(tcb != IP_NULL)
            {
                IPNET_DATA_UNLOCK(sock->sctp_inst->tcb_lock);
            }
            (void)ipnet_usr_sock_wait_until_writable(sock,
                                                     lowat,
                                                     IP_NULL);

            tcb = IP_NULL; 
            IPNET_DATA_LOCK(sock->sctp_inst->tcb_lock);
            if (sock->sctp_inst->get_tcb_info(sock, IP_NULL, sndrcv, &pmtu, &mss, (void **)&tcb))
            {
                *ppkt = IP_NULL;        

                IPNET_DATA_UNLOCK(sock->sctp_inst->tcb_lock);
                return -IP_ERRNO_ENOTCONN;
            }
            
            if(tcb == IP_NULL)
            {
                IPNET_DATA_UNLOCK(sock->sctp_inst->tcb_lock);
            }

            if (sock->ipcom.so_errno)
            {
#if 0
                /* here, bytes_to_copy is 0. */ 
                ipsctp_usr_sock_sendbuf_sub(sock, tcb, bytes_to_copy);
#endif
                *ppkt = IP_NULL;
                if(tcb != IP_NULL)
                {
                    IPNET_DATA_UNLOCK(sock->sctp_inst->tcb_lock);
                }
                return -sock->ipcom.so_errno;
            }
        }
    } while (bytes_to_copy < buf_len);

    if(tcb != IP_NULL)
    {
        IPNET_DATA_UNLOCK(sock->sctp_inst->tcb_lock);
    }

    pkt_head = pkt_tail = IP_NULL;
    while (bytes_to_copy > 0)
    {
        Ipcom_pkt *pkt     = IP_NULL;
        int        seg_len = IP_MIN(bytes_to_copy, ((mss & ~0x3) - sizeof(Ipsctp_data_chunk_hdr)));

        pkt = ipcom_pkt_malloc(pmtu,
                               IP_FLAG_FC_STACKCONTEXT | (non_blocking ? 0 : IP_FLAG_FC_BLOCKOK));
        if (IP_UNLIKELY(pkt == IP_NULL))
            break;

        /* make the start addr begin with 4*X; yes also padding it */
        pkt->start = (pkt->maxlen - mss) & ~0x3;
        pkt->end   = pkt->start + sizeof(Ipsctp_data_chunk_hdr) + seg_len;
        ipcom_memcpy(&pkt->data[pkt->start + sizeof(Ipsctp_data_chunk_hdr)], 
                     buf, seg_len);
        #if 0
        pkt->chk = ipnet_in_checksum_memcpy(&pkt->data[pkt->start],
                                            buf,
                                            seg_len);
        #endif

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

    *ppkt = pkt_head;
    return (len == 0 ? -IP_ERRNO_EWOULDBLOCK : len);
}


/*
 *===========================================================================
 *                    ipnet_usr_sock_sctp_recv
 *===========================================================================
 * Description: Receives data from a SCTP socket.
 * Parameters:  sock - The socket to receive data from.
 *              msg - Receive parameters.
 *              flags - IP_MSG_xxx flags.
 * Returns:     0 = a packet was successfully received, -1 = error.
 *
 */
IP_STATIC int
ipnet_usr_sock_sctp_recv(Ipnet_socket *sock, 
                         struct Ip_msghdr *msg, 
                         int flags)
{
    int       bytes = 0;
    if (msg->msg_iovlen != 1)
    {
        /* if no buffer just return 0 */
        if (0== msg->msg_iovlen)
            return 0;
        /* The scatter/gather buffers, or I/O vectors (pointed to by the msg_iov
         * field) are treated as a single SCTP data chunk, rather than multiple
         * chunks, for both sendmsg() and recvmsg(). 
         */
         
        return -IP_ERRNO_EINVAL;
    }

    /* msg_waitall and msg_peek must not be IP_TRUE at the same time,
       that combination is not supported by the current
       implementation */
    ip_assert(IP_BIT_ISFALSE(flags, IP_MSG_WAITALL) 
              || IP_BIT_ISFALSE(flags, IP_MSG_PEEK));

    if (IP_BIT_ISTRUE(flags, IP_MSG_OOB))
    {
        return -IP_ERRNO_EOPNOTSUPP;
    }
    
#if 0
    if (IP_BIT_ISTRUE(sock->sctp_inst->feature_flags, IPSCTP_INST_FLAG_EXPLICIT_EOR))
    {
        
    }
#endif

    bytes = ipsctp_usr_recv(sock, msg, flags); 

    if (IP_UNLIKELY(bytes == -1))
    {
        return 0;
    }

    if ((bytes == 0) || (bytes == -IP_ERRNO_EWOULDBLOCK))
    {
        ipcom_atomic_inc(&sock->sctp_inst->wait_readable);
        if (IP_BIT_ISFALSE(sock->flags, IPNET_SOCKET_FLAG_NONBLOCKING))
            return IPNET_ERRNO(EWOULDBLOCK);
        else
            return IPNET_ERRNO(EAGAIN);
    }
    
    return bytes;
}

#ifdef IPCOM_USE_INET6
/*
 *===========================================================================
 *                    ipnet_sock_sctp_ip6_lookup
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
IP_STATIC Ipnet_socket *
ipnet_sock_sctp_ip6_lookup(Ip_u16 vr,
                           int proto,
                           IP_CONST void *local_addr_ptr,
                           Ip_u16 local_port,
                           IP_CONST void *peer_addr_ptr,
                           Ip_u16 peer_port)
{
    Ipnet_socket *ret_sock = IP_NULL;

    IPCOM_UNUSED_ARG(peer_port);
    /* NOTE : don't support peer address */
    if ((IP_NULL == local_addr_ptr) || (IP_NULL != peer_addr_ptr))
        return IP_NULL;

    /* get socket by LP+LA in ipnet_bind_check_reuse */
    ret_sock = ipsctp_sock_lookup_sa(vr,
                                     IP_AF_INET6,
                                     local_addr_ptr,
                                     local_port);

#ifdef IPCOM_USE_INET
    if ((IP_NULL == ret_sock)
        && IP_IN6_IS_ADDR_V4MAPPED((struct Ip_in6_addr *)local_addr_ptr))
    {
        struct Ip_in_addr local_in_addr;

        /* copy V4 address */
        ipcom_memcpy((void *)&local_in_addr,
                     (void *)&((struct Ip_in6_addr *)local_addr_ptr)->in6.addr32[3],
                     4);

        ret_sock = ipsctp_sock_lookup_sa(vr,
                                         IP_AF_INET,
                                         (void *)&local_in_addr,
                                         local_port);
    }
#endif /* IPCOM_USE_INET */

    return ret_sock;
}
#endif /* IPCOM_USE_INET6 */


#ifdef IPCOM_USE_INET
/*
 *===========================================================================
 *                    ipnet_sock_sctp_ip4_lookup
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
IP_STATIC Ipnet_socket *
ipnet_sock_sctp_ip4_lookup(Ip_u16 vr,
                           int proto,
                           IP_CONST void *local_addr_ptr,
                           Ip_u16 local_port,
                           IP_CONST void *peer_addr_ptr,
                           Ip_u16 peer_port)
{
    Ipnet_socket *ret_sock = IP_NULL;

    IPCOM_UNUSED_ARG(peer_port);
    /* NOTE : don't support peer address */
    if ((IP_NULL == local_addr_ptr) || (IP_NULL != peer_addr_ptr))
        return IP_NULL;

    /* get socket by LP+LA in ipnet_bind_check_reuse */
    ret_sock = ipsctp_sock_lookup_sa(vr,
                                     IP_AF_INET,
                                     local_addr_ptr,
                                     local_port);

#ifdef IPCOM_USE_INET6
    if (IP_NULL == ret_sock)
    {
        union Ip_in_addr_union local_addr;
        struct Ip_in6_addr     local_in6;

        local_addr.in.s_addr = IP_GET_32ON16(local_addr_ptr);
        ipnet_ip6_create_mapped_addr(&local_addr.in, &local_in6);

        ret_sock = ipsctp_sock_lookup_sa(vr,
                                         IP_AF_INET6,
                                         &local_in6,
                                         local_port);
        if (IP_NULL == ret_sock)
            return IP_NULL;

        if (IP_BIT_ISSET(ret_sock->ip6->flags, IPNET_SOCKET_FLAG_IP6_V6ONLY))
            return IP_NULL;
    }
#endif /* IPCOM_USE_INET6 */

    return ret_sock;
}
#endif /* IPCOM_USE_INET */

/*
 *===========================================================================
 *                    ipnet_sock_sctp_network_bind
 *===========================================================================
 * Description: The SCTP network layer bind for IPV4&IPV6.
 * Parameters:  sock - The socket to bind.
 *              name - The socket name.
 *              namelen - The length of the name variable.
 *              check_reuse - Check reuse for port or address.
 * Returns:     0 = success, <0 = error code
 */
IP_STATIC int
ipnet_sock_sctp_network_bind(struct Ipnet_socket_struct *sock,
                             IP_CONST struct Ip_sockaddr *name,
                             Ip_socklen_t namelen,
                             Ip_bool check_reuse)
{
    Ipnet_sock_bind_f net_bind = IP_NULL;

    if ((IP_NULL == sock)||(IP_NULL == sock->ops)||(IP_NULL == name)||(namelen <= 0))
        return -IP_ERRNO_EINVAL;

    switch (name->sa_family)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        net_bind = ((Ipnet_sock_sctp_ops *)sock->ops)->network_bind_v4;
        break;
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        net_bind = ((Ipnet_sock_sctp_ops *)sock->ops)->network_bind_v6;
        break;
#endif /* IPCOM_USE_INET6 */
    default:
        ip_assert(IP_FALSE);
        return -IP_ERRNO_EINVAL;
    }
    if (IP_NULL == net_bind)
        return -IP_ERRNO_EINVAL;

    return net_bind(sock, name, namelen, check_reuse);
}


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_sock_sctp_register
 *===========================================================================
 * Description: Registers SCTP protocol socket support for IPv4 and/or IPv6.
 * Parameters:  N/A
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_sock_sctp_register(void)
{
    Ipnet_sock_bind_f    bind_v4 = IP_NULL;
    Ipnet_sock_sctp_ops *ops     = IP_NULL;
    int domains[] = {
#ifdef IPCOM_USE_INET
        IP_AF_INET,
#endif
#ifdef IPCOM_USE_INET6
        IP_AF_INET6,
#endif
        0
    };
    int types[] = {
#ifdef IPSCTP_INCLUDE_UDP_LIKE
                    IP_SOCK_DGRAM, 
                    IP_SOCK_SEQPACKET ,
#endif
                    IP_SOCK_STREAM
                  };
    Ip_size_t t = 0, d = 0;
    int ret = 0;

    for (d = 0; d < sizeof(domains) / sizeof(domains[0]) - 1; d++)
    {
        for (t = 0; t < sizeof(types) / sizeof(types[0]); t++)
        {
            ops = ipcom_calloc(1, sizeof(*ops));
            if (IP_NULL == ops)
            {
                IPCOM_LOG0(CRIT, "Could not register SCP protocol, "
                           "out of memory");
                IP_PANIC();
                return -IP_ERRNO_ENOMEM;
            }

            switch (domains[d])
            {
#ifdef IPCOM_USE_INET
            case IP_AF_INET:
                ipnet_sock_ip4_get_ops(&ops->i);
                ops->i.sock.lookup   = ipnet_sock_sctp_ip4_lookup;
                ops->network_bind_v4 = ops->i.network_bind;
                bind_v4              = ops->i.network_bind;
                break;
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
            case IP_AF_INET6:
                ipnet_sock_ip6_get_ops(&ops->i);
                ops->i.sock.lookup   = ipnet_sock_sctp_ip6_lookup;
                ops->network_bind_v6 = ops->i.network_bind;
#ifdef IPCOM_USE_INET
                ops->network_bind_v4 = bind_v4; /* IP_AF_INET must first */
#endif /* IPCOM_USE_INET6 */
                break;
#endif /* IPCOM_USE_INET6 */
            default:
                (void)ipcom_free(ops);
                ip_assert(IP_FALSE);
                continue;
            }

            ops->network_bind             = ipnet_sock_sctp_network_bind;
            ops->i.network_bind           = ipnet_sock_sctp_bind;

            ops->network_init             = ops->i.sock.init;
            ops->i.sock.init              = ipnet_sock_sctp_init;
            ops->network_destroy          = ops->i.sock.destroy;
            ops->i.sock.destroy           = ipnet_sock_sctp_destroy;
            ops->network_connect          = ops->i.sock.connect;
            ops->i.sock.connect           = ipnet_sock_sctp_connect;

            ops->i.sock.type              = types[t];
            ops->i.sock.proto             = IP_IPPROTO_SCTP;
            ops->i.sock.accept            = ipsctp_accept;
            ops->i.sock.close             = ipsctp_close;
            ops->i.sock.listen            = ipsctp_listen;
            ops->i.sock.usr_recv          = ipnet_usr_sock_sctp_recv;
            ops->i.sock.pkts_from_iov     = ipnet_sock_usr_pkts_from_iov_sctp;
            ops->i.sock.send              = ipsctp_output;
            ops->i.sock.shutdown          = ipsctp_shutdown;
            ops->i.sock.getopt            = ipsctp_getsockopt;
            ops->i.sock.setopt            = ipsctp_setsockopt;
            ops->i.sock.getname           = ipsctp_getname;
            ops->i.sock.ioctl             = ipsctp_ioctl;
            ops->i.sock.hdr_space        += IPSCTP_SIZE_SCTP_HDR;
            ops->i.sock.extra_sock_space += IPSCTP_SIZE_SOCK_DATA;

            ret = ipnet_sock_register_ops(&ops->i.sock);
            if (ret < 0)
            {
                (void)ipcom_free(ops);
                return ret;
            }
        }
    }

    return 0;
}
#endif
