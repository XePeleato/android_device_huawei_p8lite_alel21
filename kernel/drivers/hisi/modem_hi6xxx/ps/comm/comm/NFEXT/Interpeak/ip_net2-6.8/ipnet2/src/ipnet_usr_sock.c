/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_usr_sock.c,v $ $Revision: 1.93.14.2 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_usr_sock.c,v $
 *   $Author: kenneth $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth.jonsson@windriver.com>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */
/* Implementation of all public socket calls. This implementation will
   translate every call into a signal and sent it to the "ipnetd" task
   trough an IPCOM pipe. All IP_PUBLIC function in this module must be
   called in kernel address space and in the context of the calling
   user task.
*/

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#define IPCOM_SKIP_NATIVE_SOCK_API
#include "ipnet_config.h"

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_errno.h>
#include <ipcom_pipe.h>
#include <ipcom_sysctl.h>
#include <ipcom_syslog.h>
#include <ipcom_type.h>

#ifdef IPCOM_WV_INSTRUMENTATION
#include <ipcom_windview.h>
#endif

#include "ipnet_h.h"
#include "ipnet_signal.h"
#include "ipnet_usr_sock.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_SOCK_TASK_PIPE_SIZE   IPCOM_PIPE_MSG_LIMIT_FEW

#ifdef IPCOM_ZEROCOPY
/* Maximum length (in bytes) of transport-, network- and link-layer
   header that can be added to a packet using the zBuf API */
#define IPNET_ZBUF_MAX_HEADER_LEN 100
#endif

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    Ipnet_for_each_sock_in_set_cb
 *===========================================================================
 * Description: Callback that will be called for each present socket
 *                    in an Ip_fd_set structure.
 * Parameters:  sock - a socket.
 *              arg1 - user defined argument
 *              arg2 - user defined argument
 * Returns:     IP_TRUE if the socket is ready.
 *
 */
typedef Ip_bool (*Ipnet_for_each_sock_in_set_cb)(struct Ipnet_socket_struct *sock,
                                                 void *arg1,
                                                 void *arg2);


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

IP_STATIC void ipnet_usr_sock_poll_done(Ipnet_sig *poll_done_sig);

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
 *                    ipnet_user_vr
 *===========================================================================
 * Description: Returns the virtual router for the current user.
 * Parameters:
 * Returns:     The user VR.
 *
 */
#if IPCOM_VR_MAX > 1
IP_STATIC Ip_u16
ipnet_user_vr(void)
{
#ifdef IP_PORT_LKM
    return ipcom_get_user_vr();
#else
#if IPCOM_VR_MAX > 1
    return (Ip_u16)ipcom_proc_vr_get();
#else
    return 0;
#endif
#endif
}
#endif /* IPCOM_VR_MAX > 1 */


/*
 *===========================================================================
 *                    ipnet_pipe_handle_interrupted_recv
 *===========================================================================
 * Description: Called when a pipe receive has been interrupted during
 *              a synchronous send-receive operation.
 * Parameters:  sig       - Points to the previoulsy sent signal
 *              reply_sig - Place to store a received signal.
 *
 * Returns:     IP_TRUE   - Found a signal to receive.
 *              IP_FALSE  - Found no signal to receive.
 *
 */
IP_INLINE Ip_bool
ipnet_pipe_handle_interrupted_recv(Ipnet_sig *sig, Ipnet_sig **reply_sig)
{
    /*
     * Interrupted while sleeping in receive
     * Tag the sent message as invalid to make
     * the other end silently drop it and free it.
     */
    ipcom_atomic_set(&sig->garbage_tag, 1);

    /* Try one additional receive to avoid race condition */
    if (ipcom_pipe_try_recv(sig->response_pipe,
                            (void **) reply_sig) == IPCOM_SUCCESS)
        return IP_TRUE;

    return IP_FALSE;
}


/*
 *===========================================================================
 *                        ipnet_usr_sock_pipe_recv
 *===========================================================================
 * Description: Recieves a specific message from the specified pipe.
 * Parameters:  pipe - pipe to receive the message from
 *              psig - location where the buffer pointer must be stored
 *              rsig - pointer value that the received buffer must
 *                        have or IP_NULL if any pointer is OK
 *              types - array of acceptable types of the received message.
 *              types_len - length of the "types" array.
 * Returns:     0  = if success
 *              <0 = error code
 *
 */
IP_STATIC int
ipnet_usr_sock_pipe_recv(Ipcom_pipe *pipe,
                         Ipnet_sig **psig,
                         Ipnet_sig *rsig,
                         enum Ipnet_sig_type *types,
                         Ip_size_t types_len)
{
    int ret;

    do
    {
        ret = ipcom_pipe_recv(pipe, (void **) psig);
        if (ret >= 0)
        {
            Ip_size_t i;
            Ipnet_sig *reply_sig = *psig;

            if (rsig == reply_sig)
                return 0;

            for (i = 0; i < types_len; i++)
                if (reply_sig->sig_type == types[i])
                    return 0;

            /* Will only reach this line if the task using this
               the same PID as the current task has been
               restarted/killed while that task was blocked in a
               socket call. */
            ipnet_sig_free(reply_sig);
        }
    } while (ret >= 0);

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_pipe_basic_send_recv
 *===========================================================================
 * Description: Sends a socket operator signal and waits for the response.
 * Parameters:  sig - The signal to send.
 * Returns:     The returns code from the operation.
 *
 */
IP_INLINE int
ipnet_pipe_basic_send_recv(Ipnet_sig *sig)
{
    Ipnet_sig *reply_sig;
    int        ret = IPCOM_SUCCESS;

    ipnet_usr_sock_init_sig(sig, sig->sig_type);
    ipcom_pipe_send(ipnet->task.pipe, sig);

    if (ipnet_usr_sock_pipe_recv(sig->response_pipe,
                                 &reply_sig,
                                 sig,
                                 IP_NULL,
                                 0) == IPCOM_ERR_INTERRUPT)
    {
        /* Interrupted while sleeping in receive. */
        if (ipnet_pipe_handle_interrupted_recv(sig,
                                               &reply_sig) == IP_FALSE)
            ret = IPCOM_ERR_INTERRUPT;
    }

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_usr_sock_basic_send_recv
 *===========================================================================
 * Description: Sends a socket operator signal and waits for the response.
 * Parameters:  sig - The signal to send.
 * Returns:     The returns code from the operation.
 *
 */
IP_STATIC int
ipnet_usr_sock_basic_send_recv(Ipnet_sig *sig)
{
    Ipnet_sig *reply_sig;
    int        ret;

    ipnet_usr_sock_init_sig(sig, sig->sig_type);

    ipcom_pipe_send(ipnet->task.pipe, sig);
    ret = ipnet_usr_sock_pipe_recv(sig->response_pipe,
                                   &reply_sig,
                                   sig,
                                   IP_NULL,
                                   0);

    if (ret == IPCOM_ERR_INTERRUPT)
    {
        /* Interrupted while sleeping in receive. */
        if (ipnet_pipe_handle_interrupted_recv(sig,
                                               &reply_sig) == IP_FALSE)
        {
            /* Bail out */
            ipcom_errno_set(IP_ERRNO_EINTR);
            return ret;
        }
    }

    ip_assert(sig == reply_sig);

    if (reply_sig->reply.return_code < 0)
        ipcom_errno_set(reply_sig->reply.sock_errno);

    return reply_sig->reply.return_code;
}


/*
 *===========================================================================
 *                    ipnet_usr_sock_pkt_to_sig
 *===========================================================================
 * Description: Turns the specified packet structure into a signal.
 * Parameters:  pkt - packet to convert into a signal.
 * Returns:     Signal header that contains the packet.
 *
 */
IP_INLINE Ipnet_sig *
ipnet_usr_sock_pkt_to_sig(Ipcom_pkt *pkt)
{
    return ipcom_pkt_alloc_info(pkt,
                                IPNET_PKT_INFO_SIG,
                                sizeof(Ipnet_sig));
}


/*
 *===========================================================================
 *                    ipnet_usr_sockopt
 *===========================================================================
 * Description: Sends setsockopt or getsockopt signal to the stack deamon.
 * Parameters:  fd - A socket descriptor.
 *              level - The protocol level that should be affected by the
 *              option.
 *              optname - The option name.
 *              optval - Data to set for the option.
 *              optlen - The length of the buffer poited to by optval.
 * Returns:     0 = success, -1 = error.
 *
 */
IP_STATIC int
ipnet_usr_sockopt(Ip_fd fd,
                  int level,
                  int optname,
                  void *optval,
                  Ip_socklen_t  *optlen,
                  enum Ipnet_sig_type sig_type)
{

    Ipnet_sig   *sig;
    int          ret = -1;

    /* Allocate an extra sizeof(void*) byte to be sure that
       IP_PTR_ALIGN() does cause this function to reference data
       beyond the end of the buffer. */
    sig = ipcom_malloc(sizeof(Ipnet_sig) + *optlen + sizeof(void*));

    if (sig == IP_NULL)
    {
        ipcom_errno_set(IP_ERRNO_ENOMEM);
        goto done;
    }

    sig->sig_type          = sig_type;
    sig->d.sockopt.fd      = fd;
    sig->d.sockopt.level   = level;
    sig->d.sockopt.optname = optname;
    sig->d.sockopt.optval  = IP_PTR_ALIGN((sig + 1), sizeof(void*));
    sig->d.sockopt.optlen  = *optlen;

    /* Cannot pass the "optval" pointer, since the memory pointed to
       will not be valid in the network task when the caller is a
       VxWorks RTP */

    ipcom_memcpy(sig->d.sockopt.optval, optval, *optlen);

    ret = ipnet_usr_sock_basic_send_recv(sig);

    if (ret == IPCOM_ERR_INTERRUPT)
    {
        ipcom_errno_set(IP_ERRNO_EINTR);
        goto done;
    }

    if (ret == 0 && sig_type == IPNET_SIG_GETSOCKOPT)
    {
        *optlen = sig->d.sockopt.optlen;
        ipcom_memcpy(optval, sig->d.sockopt.optval, *optlen);
    }

    ipcom_free(sig);

 done:

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_usr_sock_getname
 *===========================================================================
 * Description: Returns the socket address of the local side (if bound) or
 *              the peer side (if connected).
 * Parameters:  fd - Socket descriptor.
 *              name - Will hold the socket address if the call succeeds.
 *              namelenp - The length of the 'name' buffer, will be updated
 *              to hold the actual size of 'name'.
 *              local - Set to IP_TRUE if the name of local size should be
 *              returned and IP_FALSE if the peer socket name should be returned.
 * Returns:     0 = success, -1 error.
 *
 */
IP_STATIC int
ipnet_usr_sock_getname(Ip_fd fd,
                       struct Ip_sockaddr *name,
                       Ip_socklen_t *namelenp,
                       Ip_bool local)
{
    Ipnet_sig *sig;
    int       ret = -1;

    if (name == IP_NULL || namelenp == IP_NULL)
    {
        (void)ipcom_errno_set(IP_ERRNO_EFAULT);
        goto errout;
    }

    sig = ipcom_malloc(sizeof(Ipnet_sig));

    if (sig == IP_NULL)
    {
        (void)ipcom_errno_set(IP_ERRNO_ENOMEM);
        goto errout;
    }

    sig->sig_type                 = IPNET_SIG_GETNAME;
    sig->d.getname.fd             = fd;
    sig->d.getname.get_local_name = local;

    ret = ipnet_usr_sock_basic_send_recv(sig);

    if (ret == IPCOM_ERR_INTERRUPT)
    {
        (void)ipcom_errno_set(IP_ERRNO_EINTR);
        goto errout;
    }

    if (ret == 0)
    {
        if (*namelenp < IPCOM_SA_LEN_GET(&sig->d.getname.name))
        {
            ipcom_errno_set(IP_ERRNO_ENOBUFS);
            return IP_SOCKERR;
        }
        *namelenp = IPCOM_SA_LEN_GET(&sig->d.getname.name);
        ipcom_memcpy(name, &sig->d.getname.name, *namelenp);
    }

    ipcom_free(sig);

    return ret;

 errout:

    return IP_SOCKERR;
}


/*
 *===========================================================================
 *                    ipnet_copy_sender_info
 *===========================================================================
 * Description: Copies the sender info from the received packet.
 * Parameters:  sock - The socket.
 *              pkt - A received packet
 *              from - The address of the sender of the packet.
 *              fromlen - The length of the buffer pointed to by from.
 * Returns:
 *
 */
IP_STATIC void
ipnet_copy_sender_info(Ipnet_socket *sock,
                       Ipcom_pkt *pkt,
                       struct Ip_sockaddr *from,
                       Ip_size_t *fromlen)
{
    Ip_u16 sport_n = 0;

    /* Fill in sender port. */
    if (sock->ipcom.domain == IP_AF_INET || sock->ipcom.domain == IP_AF_INET6)
    {
        if (sock->proto == IP_IPPROTO_UDP)
        {
            Ipnet_pkt_udp *udp;

            udp = (Ipnet_pkt_udp *) &pkt->data[pkt->start - IPNET_UDP_HDR_SIZE];
            sport_n = udp->sport;
        }
        else
        {
            ip_assert(sock->proto != IP_IPPROTO_TCP);
            sport_n = 0;
        }
    }

    switch(sock->ipcom.domain)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        /* Fill in sender IPv4 address. */
        if (*fromlen >= sizeof(struct Ip_sockaddr_in))
        {
            Ipnet_pkt_ip *ip4;
            struct Ip_sockaddr_in *from_in = (struct Ip_sockaddr_in *) from;

            ipcom_memset(from, 0, sizeof(struct Ip_sockaddr_in));
            ip4 = (Ipnet_pkt_ip *) &pkt->data[pkt->ipstart];
            IPCOM_SA_LEN_SET(from_in, sizeof(struct Ip_sockaddr_in));
            from_in->sin_family = IP_AF_INET;
            from_in->sin_port   = sport_n;
            ipcom_memcpy(&from_in->sin_addr, ip4->src, sizeof(struct Ip_in_addr));
        }
        *fromlen = sizeof(struct Ip_sockaddr_in);
        break;
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        /* Fill in sender IPv6 address. */
        if (*fromlen >= sizeof(struct Ip_sockaddr_in6))
        {
            struct Ip_sockaddr_in6 *from_in6 = (struct Ip_sockaddr_in6 *) from;

            ipcom_memset(from, 0, sizeof(struct Ip_sockaddr_in6));
            IPCOM_SA_LEN_SET(from_in6, sizeof(struct Ip_sockaddr_in6));
            from_in6->sin6_family = IP_AF_INET6;
            from_in6->sin6_port   = sport_n;
#ifdef IPCOM_USE_INET
            if ((pkt->data[pkt->ipstart] & 0xf0) == 0x40)
            {
                /* Use Ipv6 mapped IPv4 address (packet is Ipv4) */
                if (*fromlen >= sizeof(struct Ip_sockaddr_in6))
                {
                    Ipnet_pkt_ip *ip4;

                    ip4 = (Ipnet_pkt_ip *) &pkt->data[pkt->ipstart];
                    ipnet_ip6_create_mapped_addr((struct Ip_in_addr *) ip4->src, &from_in6->sin6_addr);
                    from_in6->sin6_flowinfo = 0;
                    from_in6->sin6_scope_id = 0;
                }
            }
            else
#endif /* IPCOM_USE_INET */
            {
                /* Normal Ipv6 packet */
                if (*fromlen >= sizeof(struct Ip_sockaddr_in6))
                {
                    Ipnet_pkt_ip6 *ip6;

                    ip6 = (Ipnet_pkt_ip6*) &pkt->data[pkt->ipstart];
                    IPNET_IP6_SET_ADDR(&from_in6->sin6_addr, &ip6->src);
                    from_in6->sin6_flowinfo = ip_htonl(IPNET_IP6_GET_FLOW(ip6));
                    if (ipnet_ip6_is_scoped_addr(&ip6->src))
                        from_in6->sin6_scope_id = ipnet_ip6_get_zone(pkt->ifindex,
                                                                     &ip6->src);
                    else
                        /* Use default scope of global addresses */
                        from_in6->sin6_scope_id = IPNET_IP6_DEFAULT_ZONE;
                }
            }
        }
        *fromlen = sizeof(struct Ip_sockaddr_in6);
        break;
#endif /* IPCOM_USE_INET6 */

#ifdef IPCOM_USE_PACKET
    case IP_AF_PACKET:
        if (*fromlen >= sizeof(struct Ip_sockaddr_ll))
        {
            Ipnet_netif           *netif;
            struct Ip_sockaddr_ll *ll_addr = (struct Ip_sockaddr_ll *) from;

            ipcom_memset(from, 0, sizeof(struct Ip_sockaddr_ll));
            netif = ipnet_if_indextonetif(pkt->vr_index, pkt->ifindex);
            ip_assert(netif != IP_NULL);
            (void)netif->link_get_frame_info(netif,
                                             pkt,
                                             &ll_addr->sll_pkttype,
                                             &ll_addr->sll_protocol,
                                             ll_addr->sll_addr,
                                             IP_NULL);
            IPCOM_SA_LEN_SET(ll_addr, sizeof(struct Ip_sockaddr_ll));
            ll_addr->sll_family  = IP_AF_PACKET;
            ll_addr->sll_ifindex = netif->ipcom.ifindex;
            ll_addr->sll_hatype  = netif->ipcom.type;
            ll_addr->sll_halen   = (Ip_u8) netif->ipcom.link_addr_size;
        }
        *fromlen = sizeof(struct Ip_sockaddr_ll);
        break;
#endif /* IPCOM_USE_PACKET */
#ifdef IPNET_USE_NETLINKSOCK
    case IP_AF_NETLINK:
        if (*fromlen >= sizeof(struct Ip_sockaddr_nl))
        {
            struct Ip_sockaddr_nl *nl = (struct Ip_sockaddr_nl *)from;
            ipcom_memset(nl, 0, sizeof(*nl));
            IPCOM_SA_LEN_SET(nl, sizeof(*nl));
            nl->nl_family   = IP_AF_NETLINK;
            nl->nl_pid      = pkt->link_cookie;
        }
        *fromlen = sizeof(struct Ip_sockaddr_nl);
        break;
#endif

    default:
#ifdef IPCOM_USE_SA_LEN
        if (*fromlen >= sizeof(from->sa_len) + sizeof(from->sa_family))
#else
        if (*fromlen >= sizeof(from->sa_family))
#endif
        {
#ifdef IPCOM_USE_SA_LEN
            from->sa_len    = (Ip_u8) (sizeof(from->sa_len) + sizeof(from->sa_family));
#endif
            from->sa_family = (Ip_u8) sock->ipcom.domain;
        }
#ifdef IPCOM_USE_SA_LEN
        *fromlen = from->sa_len;
#else
        *fromlen = IPCOM_SA_LEN_GET(from);
#endif
        break;
    }
}


/*
 *===========================================================================
 *                        ipnet_usr_sock_poll_free
 *===========================================================================
 * Description: Frees all resources allocated in a poll operation
 * Parameters:  poll_done_sig - the poll-done signal that acts as the
 *                        session tracker for this operation.
 * Returns:
 *
 */
IP_STATIC void
ipnet_usr_sock_poll_free(Ipnet_sig *poll_done_sig)
{
    if (ipnet_is_stack_task())
        /* Already running in the context of the stack task, can run
           the poll done handler right away */
        ipnet_sys_poll_done(poll_done_sig);
    else
        /* Not in the context of the stack task, send this signal to
           the stack task */
        ipnet_usr_sock_poll_done(poll_done_sig);
}


/*
 *===========================================================================
 *                    ipnet_usr_sock_poll
 *===========================================================================
 * Description: Registers/unregister a watch for a socket.
 * Parameters:  sock - a socket
 *              poll_done - payload of the poll done signal that will
 *                    finish this poll session.
 *              timeout - maximum time to wait for the poll to finish,
 *                    IP_NULL means infinite.
 *              what - what to poll for.
 *              lowat - minimum number of bytes that must be available
 *                    to consider the socket to be ready. 0 will use
 *                    the lo water mark set on the socket.
 * Returns:     !IP_NULL - operation successfull
 *              IP_NULL  - failed to allocate memory (critical error).
 *
 */
IP_STATIC Ipnet_sig*
ipnet_usr_sock_poll(Ipnet_socket *sock,
                    struct Ipnet_sig_poll_done *poll_done,
                    struct Ip_timeval *timeout,
                    enum Ipnet_sock_poll what,
                    int lowat)
{
    Ipnet_sig *sig;

    sig = ipcom_slab_alloc(ipnet->slabs.poll);
    if (sig == IP_NULL)
    {
        IP_PANIC2();
        return IP_NULL;
    }

    ipcom_list_insert_last(&poll_done->session_list_head,
                           &sig->d.poll.session_list);

    ipnet_usr_sock_init_sig(sig, IPNET_SIG_POLL);

    sig->free                  = ipnet_usr_sock_poll_free;
    sig->freesig               = ipcom_list_entry(poll_done,
                                                  Ipnet_sig,
                                                  d.poll_done);
    sig->d.poll.fd             = sock->ipcom.fd;
    sig->d.poll.what           = what;
    sig->d.poll.lowat          = lowat;
    sig->d.poll.timeout        = timeout;
    sig->d.poll.ptmo           = &poll_done->tmo;
    sig->d.poll.responses_sent = &poll_done->responses_sent;
    ipcom_pipe_send(ipnet->task.pipe, sig);

    return sig;
}


/*
 *===========================================================================
 *                 ipnet_usr_sock_alloc_poll_done_sig
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ipnet_sig *
ipnet_usr_sock_alloc_poll_done_sig(void)
{
    Ipnet_sig *sig;

    sig = ipcom_slab_alloc(ipnet->slabs.poll);
    if (sig != IP_NULL)
    {
        ipnet_usr_sock_init_sig(sig, IPNET_SIG_POLL_DONE);
        ipcom_atomic_set(&sig->d.poll_done.responses_sent, 0);
        ipcom_list_init(&sig->d.poll_done.session_list_head);
        sig->d.poll_done.tmo = IP_NULL;
    }

    return sig;
}


/*
 *===========================================================================
 *                    ipnet_usr_sock_poll_done
 *===========================================================================
 * Description: Unregister a watch for a socket.
 * Parameters:  poll_signal - poll signal that was used to register
 *                    the watch using IPNET_SIG_POLL signal.
 * Returns:
 *
 */
IP_STATIC void
ipnet_usr_sock_poll_done(Ipnet_sig *poll_done_signal)
{
    ip_assert(poll_done_signal->sig_type == IPNET_SIG_POLL_DONE);

    ipcom_pipe_send(ipnet->task.pipe, poll_done_signal);
}


/*
 *===========================================================================
 *                    ipnet_usr_sock_poll_readable
 *===========================================================================
 * Description: Registers/unregister a watch for readability on the
 *                    specified socket.
 * Parameters:  sock - a socket
 *              poll_done - payload of the poll done signal that will
 *                    finish this poll session.
 *              timeout - maximum time to wait for the poll to finish,
 *                    IP_NULL means infinite.
 * Returns:     Will always return IP_TRUE.
 *
 */
IP_STATIC Ip_bool
ipnet_usr_sock_poll_readable(Ipnet_socket *sock,
                             struct Ipnet_sig_poll_done *poll_done,
                             struct Ip_timeval *timeout)
{
    return (ipnet_usr_sock_poll(sock,
                                poll_done,
                                timeout,
                                IPNET_SOCK_POLL_READ,
                                0) != IP_NULL);
}


/*
 *===========================================================================
 *                    ipnet_usr_sock_poll_writable
 *===========================================================================
 * Description: Registers/unregister a watch for writability on the
 *                    specified socket.
 * Parameters:  sock - a socket
 *              poll_done - payload of the poll done signal that will
 *                    finish this poll session.
 *              timeout - maximum time to wait for the poll to finish,
 *                    IP_NULL means infinite.
 * Returns:     Will always return IP_TRUE.
 *
 */
IP_STATIC Ip_bool
ipnet_usr_sock_poll_writable(Ipnet_socket *sock,
                             struct Ipnet_sig_poll_done *poll_done,
                             struct Ip_timeval *timeout)
{
    return (ipnet_usr_sock_poll(sock,
                                poll_done,
                                timeout,
                                IPNET_SOCK_POLL_WRITE,
                                0) != IP_NULL);
}


/*
 *===========================================================================
 *                    ipnet_usr_sock_poll_exceptional
 *===========================================================================
 * Description: Registers/unregister a watch for exceptional event on the
 *                    specified socket.
 * Parameters:  sock - a socket
 *              poll_done - payload of the poll done signal that will
 *                    finish this poll session.
 *              timeout - maximum time to wait for the poll to finish,
 *                    IP_NULL means infinite.
 * Returns:     Will always return IP_TRUE.
 *
 */
IP_STATIC Ip_bool
ipnet_usr_sock_poll_exceptional(Ipnet_socket *sock,
                                struct Ipnet_sig_poll_done *poll_done,
                                struct Ip_timeval *timeout)
{
    return (ipnet_usr_sock_poll(sock,
                                poll_done,
                                timeout,
                                IPNET_SOCK_POLL_EXCEPT,
                                0) != IP_NULL);
}


/*
 *===========================================================================
 *                    ipnet_is_sock_readable_adapter
 *===========================================================================
 * Description: Checks if the specified socket is readable.
 * Parameters:  sock - a socket
 * Returns:     IP_TRUE if the socket is readable.
 *
 */
IP_STATIC Ip_bool
ipnet_is_sock_readable_adapter(Ipnet_socket *sock,
                               void *unused1,
                               void *unused2)
{
    IPCOM_UNUSED_ARG(unused1);
    IPCOM_UNUSED_ARG(unused2);
    return ipnet_is_sock_readable(sock, 0);
}


/*
 *===========================================================================
 *                    ipnet_is_sock_writable_adapter
 *===========================================================================
 * Description: Checks if the specified socket is writable.
 * Parameters:  sock - a socket
 * Returns:     IP_TRUE if the socket is writable.
 *
 */
IP_STATIC Ip_bool
ipnet_is_sock_writable_adapter(Ipnet_socket *sock,
                               void *unused1,
                               void *unused2)
{
    IPCOM_UNUSED_ARG(unused1);
    IPCOM_UNUSED_ARG(unused2);
    return ipnet_is_sock_writable(sock, 0);
}


/*
 *===========================================================================
 *                    ipnet_is_sock_exceptional_adapter
 *===========================================================================
 * Description: Checks if the specified socket is exceptional.
 * Parameters:  sock - a socket
 * Returns:     IP_TRUE if the socket is exceptional.
 *
 */
IP_STATIC Ip_bool
ipnet_is_sock_exceptional_adapter(Ipnet_socket *sock,
                                  void *unused1,
                                  void *unused2)
{
    IPCOM_UNUSED_ARG(unused1);
    IPCOM_UNUSED_ARG(unused2);
    return ipnet_is_sock_exceptional(sock, 0);
}


/*
 *===========================================================================
 *                    ipnet_usr_sock_for_each_sock_in_set
 *===========================================================================
 * Description: Calls the specified function for each socket set in
 *                    the set.
 * Parameters:  nfds - The highest socket number in the set rounded up
 *                    to nearest number that can be evenly divided by
 *                    32.
 *              fds - The set to iterate over.
 *              unset_non_ready_sock - IP_TRUE if socket that is
 *                    currently no ready should be cleared in the
 *                    'fds' set.
 *              is_sock_ready - function that knows how to check if
 *                    the socket is ready.
 *              arg1-3 - user defined argument that is passed verbatim
 *                    to the 'is_ready_sock' function.
 * Returns:     >=0 = the number of ready sockets, <0 = error code.
 *
 */
IP_STATIC int
ipnet_for_each_sock_in_set(int nfds,
                           Ip_fd_set *fds,
                           Ip_bool unset_non_ready_sock,
                           Ipnet_for_each_sock_in_set_cb is_sock_ready,
                           void *arg1,
                           void *arg2)
{
    Ipnet_socket *sock;
    Ip_fd         fd;
    Ip_fd         fd32_div_32;
    Ip_fd         fd8;
    Ip_fd         fd_bit;
    int           nfds32 = nfds >> 5;
    int           ready_sock_count = 0;
    int           err = 0;

    ip_assert((nfds % 32) == 0);

    if (fds == IP_NULL)
        return 0;

    for (fd32_div_32 = 0; fd32_div_32 < nfds32; fd32_div_32++)
    {
        if (fds->set[fd32_div_32] == 0)
            continue;
        for (fd8 = 0; fd8 < 32; fd8 += 8)
        {
            if ((fds->set[fd32_div_32] & (0xff << fd8)) == 0)
                continue;
            fd = (fd32_div_32 << 5) + fd8;
            for (fd_bit = 0; fd_bit < 8; fd_bit++)
                if (IP_FD_ISSET(fd + fd_bit, fds))
                {
                    sock = ipnet_get_sock(fd + fd_bit);
                    if (sock == IP_NULL)
                        err = -IP_ERRNO_EBADF;
                    else
                    {
                        if (is_sock_ready(sock, arg1, arg2))
                            ready_sock_count++;
                        else if (unset_non_ready_sock)
                            IP_FD_CLR(fd + fd_bit, fds);

                        ipnet_release_sock(sock);
                    }
                }
        }
    }
    return err ? err : ready_sock_count;
}


/*
 *===========================================================================
 *                 ipnet_usr_sock_wait_until
 *===========================================================================
 * Description: Waits until the specified descriptor is ready.
 * Parameters:  sock - the socket to wait for.
 *              lowat - number of bytes that must be available for the
 *                 socket to be considered ready
 *              timeout - maximum time to wait for socket to become ready.
 *              what - what to wait for.
 * Returns:     IP_TRUE - socket is ready.
 *              IP_FALSE - poll operation timed out.
 *
 */
IP_STATIC Ip_bool
ipnet_usr_sock_wait_until(Ipnet_socket *sock,
                          int lowat,
                          struct Ip_timeval *timeout,
                          enum Ipnet_sock_poll what)
{
    Ipnet_sig     *reply;
    Ipnet_sig     *poll_done_sig;
    Ipnet_sig     *poll_sig;
    Ip_bool        timed_out;
    Ipcom_pipe    *pipe;

    poll_done_sig = ipnet_usr_sock_alloc_poll_done_sig();
    if (poll_done_sig == IP_NULL
        || (poll_sig = ipnet_usr_sock_poll(sock,
                                           &poll_done_sig->d.poll_done,
                                           timeout,
                                           what,
                                           lowat)) == IP_NULL)
    {
        IP_PANIC2();
        return IP_FALSE;
    }

    pipe = ipnet_usr_sock_get_response_pipe();
    if (ipnet_usr_sock_pipe_recv(pipe,
                                 &reply,
                                 poll_sig,
                                 IP_NULL,
                                 0) == IPCOM_ERR_INTERRUPT)
    {
        ipcom_atomic_set(&poll_sig->garbage_tag, 1);
        ipnet_usr_sock_poll_done(poll_done_sig);
        sock->ipcom.so_errno = IP_ERRNO_EINTR;
        return IP_FALSE;
    }
    timed_out = (reply->sig_type == IPNET_SIG_POLL_TIMEOUT);
    ip_assert(reply->sig_type == IPNET_SIG_POLL
              || reply->sig_type == IPNET_SIG_POLL_TIMEOUT);

    ipnet_usr_sock_poll_done(poll_done_sig);

    return !timed_out;
}


/*
 *===========================================================================
 *                 ipnet_usr_sock_wait_until_readable
 *===========================================================================
 * Description: Waits until the specified descriptor is readable.
 * Parameters:  sock - the socket to wait for.
 *              lowat - number of bytes that must be available for the
 *                 socket to be considered ready
 *              timeout - maximum time to wait for the socket to become
 *                 readable or IP_NULL for indefinite.
 * Returns:     IP_TRUE - socket is ready.
 *              IP_FALSE - poll operation timed out.
 *
 */
IP_STATIC Ip_bool
ipnet_usr_sock_wait_until_readable(Ipnet_socket *sock,
                                   int lowat,
                                   struct Ip_timeval *timeout)
{
    if (ipnet_is_sock_readable(sock, lowat))
        return IP_TRUE;
    return ipnet_usr_sock_wait_until(sock,
                                     lowat,
                                     timeout,
                                     IPNET_SOCK_POLL_READ);
}


/*
 *===========================================================================
 *                 ipnet_usr_sock_create_pkt_from_iov
 *===========================================================================
 * Description: Default packet creator handler. All datagram and raw
 *              socket types should be able to use this one. Stream
 *              based protocols like TCP and SCTP must have their
 *              own 'iov' to packet(s) handler.
 * Parameters:  sock - The socket the data is beeing sent at.
 *              msg - The send parameters specified by the sender.
 *              flags - IP_MSG_xxx flags.
 *              offset - must always be zero.
 *              total_buf_len - Total length of all buffers in the
 *                 msg->msg_iov parameter.
 *              ppkt - Will contain a Ipcom_pkt if the call is successful.
 * Returns:     0> = success, number of octets copied into packet.
 *              <0 = error code
 *
 */
IP_STATIC int
ipnet_usr_sock_create_pkt_from_iov(Ipnet_socket *sock,
                                   IP_CONST struct Ip_msghdr *msg,
                                   int flags,
                                   int offset,
                                   int total_buf_len,
                                   Ipcom_pkt **ppkt)
{
    Ipcom_pkt *pkt;
    int        len = 0;
    Ip_size_t  i;
    Ip_size_t  dynamic_hdrspace = 0;
    Ip_bool    non_blocking;
    Ip_bool    align_buf_on_64_bit = IP_FALSE;
    Ip_bool    fast_path;

    IPCOM_UNUSED_ARG(offset);
    ip_assert(offset == 0);

    non_blocking = IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_NONBLOCKING)
        || IP_BIT_ISSET(flags, IP_MSG_DONTWAIT);

    while (ipcom_atomic_add_and_return(&sock->snd_bytes, total_buf_len)
           >= sock->send_max_bytes + total_buf_len)
    {
        ipcom_atomic_sub(&sock->snd_bytes, total_buf_len);

        /* The requested number of bytes cannot be sent at the
           moment */
        if (non_blocking)
            /* Non blocking send */
            return -IP_ERRNO_EWOULDBLOCK;

        ipnet_usr_sock_wait_until_writable(sock, total_buf_len, IP_NULL);
        if (sock->ipcom.so_errno)
            return -sock->ipcom.so_errno;
    }

    fast_path = (msg->msg_iovlen == 1
                 && sock->snd_head == IP_NULL
                 && IP_BIT_ISFALSE(flags, IP_MSG_MORE));

    if (IP_UNLIKELY(sock->snd_head != IP_NULL))
        len = (Ip_size_t) sock->snd_head->end - sock->snd_head->start;

    len += total_buf_len;

    /* Check if the packet is small enough to be sent
     * (this value can be increased using IP_SO_SNDBUF socket option)
     */
    if (len > sock->send_max_bytes)
    {
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                          1, 5, IPCOM_WV_NETDEVENT_WARNING, IPCOM_WV_NETD_SEND,
                          ipnet_usr_sock_create_pkt_from_iov, IPCOM_WV_NETD_FULLSOCK,
                          IPCOM_WV_IPNET_SOCK_MODULE, IPCOM_WV_NETD_SOCKET);
        IPNET_STATS(usr_iov_emsgsize++);
        IPCOM_LOG3(WARNING, "sent message discarded on socket %d since the send buffer is only %d, %d was required",
                   sock->ipcom.fd, sock->send_max_bytes, len);
        return -IP_ERRNO_EMSGSIZE;
    }

    if (sock->dynamic_header_space != IP_NULL
        && (msg->msg_control != IP_NULL || IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_HAS_STICKY_OPT)))
        dynamic_hdrspace = sock->dynamic_header_space(sock, msg);

    pkt = ipcom_pkt_malloc(len + sock->max_hdrspace + dynamic_hdrspace,
                           IP_FLAG_FC_STACKCONTEXT | (non_blocking ? 0 : IP_FLAG_FC_BLOCKOK));
    if (pkt == IP_NULL)
    {
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_CRITICAL,
                          1, 6, IPCOM_WV_NETDEVENT_CRITICAL, IPCOM_WV_NETD_SEND,
                          ipnet_usr_sock_create_pkt_from_iov, IPCOM_WV_NETD_NOBUFS,
                          IPCOM_WV_IPNET_SOCK_MODULE, IPCOM_WV_NETD_SOCKET);
        IPNET_STATS(usr_iov_nomem++);

        if (sock->ipcom.event_cb != IP_NULL)
        {
            IP_BIT_SET(ipnet_flags, IPNET_FLAG_PKTAVAIL_CB_PENDING);
            IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_PKTAVAIL_CB_PENDING);
        }

        IPCOM_LOG2(NOTICE, "failed to allocate %d bytes packet on socket %d",
                   len + sock->max_hdrspace + dynamic_hdrspace, sock->ipcom.fd);
        return -IP_ERRNO_ENOMEM;
    }

    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_DONTROUTE)
        || IP_BIT_ISSET(flags, IP_MSG_DONTROUTE))
        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_DONTROUTE);

    if (sock->ipcom.domain == IP_AF_ROUTE
        || sock->ipcom.domain == IP_AF_NETLINK
        || sock->ipcom.domain == IP_AF_KEY)
    {
        /* These (internal) protocols contains C structures and which
           must have all data on the correct alignment */
        align_buf_on_64_bit = IP_TRUE;
        pkt->start = (-(Ip_ptrdiff_t)pkt->data) & 0x7;
    }
    else
    {
        pkt->start = ipcom_conf_max_link_hdr_size + sock->max_hdrspace + dynamic_hdrspace;

#if defined(IPCOM_PKT_TX_ALIGN)
        /* Optimize alignment for Ethernet header */
        IPCOM_PKT_TX_ALIGN(pkt, sock->max_hdrspace + IP_ETH_HDR_SIZE, 1);
#endif
    }

    pkt->vr_index = sock->vr_index;
    pkt->end = pkt->start;

    if (fast_path)
    {
#if defined(IPCOM_PKT_TX_ALIGN)
        /* Don't mess with alignment */
        (void)align_buf_on_64_bit;
#else
        if (align_buf_on_64_bit == IP_FALSE
            && ((Ip_ptrdiff_t) &pkt->data[pkt->start]
                ^ (Ip_ptrdiff_t) msg->msg_iov[0].iov_base) & 0x3)
            /* Align user buffer with pkt buffer */
            pkt->start += 2;
#endif /* IPCOM_PKT_TX_ALIGN */

#ifdef IPCOM_USE_HW_CHECKSUM_TX
        ipcom_memcpy(&pkt->data[pkt->start],
                     msg->msg_iov[0].iov_base,
                     msg->msg_iov[0].iov_len);
        if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_ADDCHECKSUM))
            IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM);
#else /* !IPCOM_USE_HW_CHECKSUM_TX */
        if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_ADDCHECKSUM))
        {
            pkt->chk = ipnet_in_checksum_memcpy(&pkt->data[pkt->start],
                                                msg->msg_iov[0].iov_base,
                                                msg->msg_iov[0].iov_len);
        }
        else
            ipcom_memcpy(&pkt->data[pkt->start],
                         msg->msg_iov[0].iov_base,
                         msg->msg_iov[0].iov_len);
#endif /* IPCOM_USE_HW_CHECKSUM_TX */

        pkt->end = pkt->start + msg->msg_iov[0].iov_len;
    }
    else /* normal path */
    {
        if (sock->snd_head != IP_NULL)
        {
            i = sock->snd_head->end - sock->snd_head->start;
            ipcom_memcpy(&pkt->data[pkt->end],
                         &sock->snd_head->data[sock->snd_head->start],
                         i);
            pkt->end += i;
            len -= i;
            ipcom_pkt_free(sock->snd_head);
        }

        if (IP_BIT_ISFALSE(flags, IP_MSG_MORE))
            sock->snd_head = IP_NULL;
        else
        {
            sock->snd_head = pkt;
            IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_SOCKSENDQ);
            if (msg->msg_name != IP_NULL)
            {
                /* Save this to address since later sendmsg() might
                   skip to specify it */
                if (sock->snd_to != IP_NULL
                    && IPCOM_SA_LEN_GET(sock->snd_to) < msg->msg_namelen)
                {
                    ipcom_free(sock->snd_to);
                    sock->snd_to = IP_NULL;
                }

                if (sock->snd_to == IP_NULL)
                    sock->snd_to = ipcom_malloc(msg->msg_namelen);

                if (sock->snd_to != IP_NULL && sock->snd_to != msg->msg_name)
                    ipcom_memcpy(sock->snd_to,
                                 msg->msg_name,
                                 msg->msg_namelen);
            }
        }

        for (i = 0; i < msg->msg_iovlen; i++)
        {
            /* Copy user data into pkt->data with enough space for
               anything. 0 bytes is valid. */
            if (msg->msg_iov[i].iov_len > 0)
            {
                ipcom_memcpy(&pkt->data[pkt->end],
                                     msg->msg_iov[i].iov_base,
                                     msg->msg_iov[i].iov_len);
                pkt->end += (int)msg->msg_iov[i].iov_len;
            }
        }

        if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_ADDCHECKSUM)
            && IP_BIT_ISFALSE(flags, IP_MSG_MORE))
            pkt->chk = ipcom_in_checksum_update(&pkt->data[pkt->start],
                                                pkt->end - pkt->start);

        if (IP_BIT_ISSET(flags, IP_MSG_MORE))
        {
            /* Just append to the packet until send is called with the
               MSG_MORE flag cleared */
            *ppkt = IP_NULL;
            return len;
        }
    }

#ifdef IPCOM_USE_INET6
    if (IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_ADDCHECKSUM)
        && sock->ipcom.domain == IP_AF_INET6
        && sock->ipcom.type == IP_SOCK_RAW)
    {
        /* The checksum must be calculated by the stack, the application
           might have left the checksum field uninitialized.
        */
        Ip_u16 *chksum_ptr = (Ip_u16 *) &pkt->data[pkt->start + sock->chk_offset];
#ifdef IPCOM_USE_HW_CHECKSUM_TX
        *chksum_ptr = 0;
#else
        if (*chksum_ptr)
        {
            *chksum_ptr = 0;
            pkt->chk = ipcom_in_checksum_update(&pkt->data[pkt->start], pkt->end - pkt->start);
        }
#endif
    }
#endif /* IPCOM_USE_INET6 */

    pkt->dtor = sock->pkt_dtor;
    if (sock->pkt_dtor)
    {
        Ipnet_pkt_info_sock_snd_buf snd_buf_info;

        snd_buf_info.sock        = sock;
        snd_buf_info.payload_len = pkt->end - pkt->start;
        ipcom_pkt_set_info(pkt,
                           IPNET_PKT_INFO_SOCK_SND_BUF,
                           sizeof(snd_buf_info),
                           &snd_buf_info);
    }

    *ppkt = pkt;
    return len;
}


/*
 *===========================================================================
 *                    ipcom_do_if_attach
 *===========================================================================
 * Description: Attaches a new network interface to IPNET.
 *
 * Parameters:  netif   - An initialized netif structure.
 *              locked  - Tells wheter IPNET shall lock the interface or not.
 *
 *
 * Returns:      The interface index for this interface or negative errno.
 *
 */
IP_STATIC int
ipnet_do_if_attach(Ipcom_netif *netif, Ip_bool locked)
{
    Ipnet_sig *sig;
    int        ret;

    sig = ipcom_malloc(sizeof(Ipnet_sig));

    if (sig == IP_NULL)
    {
        (void)ipcom_errno_set(IP_ERRNO_ENOMEM);
        goto errout;
    }

    sig->sig_type       = IPNET_SIG_IF_ATTACH;
    sig->d.netif.netif  = netif;
    sig->d.netif.locked = locked;

    if (ipnet_pipe_basic_send_recv(sig) == IPCOM_ERR_INTERRUPT)
    {
        (void)ipcom_errno_set(IP_ERRNO_EINTR);
        goto errout;
    }

    ret = sig->reply.return_code;

    if (ret < 0)
        (void)ipcom_errno_set(IP_ERRNO_ENXIO);

    ipcom_free(sig);

    return ret;

 errout:

    return -ipcom_errno;
}

/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                     ipnet_usr_sock_init
 *===========================================================================
 * Description: Initializes the user socket module.
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipnet_usr_sock_init()
{
    ipnet->slabs.poll = ipcom_slab_new("IPNET poll",
                                       IPCOM_SLAB_F_POISON,
                                       ip_offsetof(Ipnet_sig, d.poll.last),
                                       0,
                                       IP_NULL,
                                       IP_NULL,
                                       ipnet->memory_pool);

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipnet_usr_sock_get_ops
 *===========================================================================
 * Description: Sets all members that can be used by most socket types.
 * Parameters:  ops - Socket operation structure.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_usr_sock_get_ops(Ipnet_sock_ops *ops)
{
    ops->usr_recv      = ipnet_usr_sock_recvmsg;
    ops->pkts_from_iov = ipnet_usr_sock_create_pkt_from_iov;
}


/*
 *===========================================================================
 *                       ipnet_get_sock
 *===========================================================================
 * Description: Returns the socket descriptor that matches the
 *                       specified file descriptor. The caller must
 *                       call ipnet_release_sock() when done using the
 *                       socket.
 * Parameters:  fd - a file descriptor.
 * Returns:     The matching socket descriptor or IP_NULL if no
 *                       matching socket exists.
 *
 */
IP_GLOBAL Ipnet_socket *
ipnet_get_sock(int fd)
{
    Ipnet_socket *sock;

    IPNET_DATA_LOCK(ipnet->sockets_lock);

    if (IP_LIKELY(fd >= IPNET_SOCKET_STARTFD)
        && IP_LIKELY(fd < ipnet_conf_max_sockets)
        && IP_LIKELY(IP_NULL != (sock = ipnet->sockets[fd]))
        && IP_LIKELY(IP_BIT_ISFALSE(sock->flags, IPNET_SOCKET_FLAG_CLOSED)))
    {
        ipcom_atomic_inc(&sock->ref_count);
    }
    else
    {
        sock = IP_NULL;
        ipcom_errno_set(IP_ERRNO_EBADF);
    }

    IPNET_DATA_UNLOCK(ipnet->sockets_lock);

    return sock;
}


/*
 *===========================================================================
 *                     ipnet_usr_sock_get_pipe
 *===========================================================================
 * Description: Returns a pipe that can be used to send messages from
 *                   the "ipnetd" task back to this task.
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ipcom_pipe *
ipnet_usr_sock_get_pipe()
{
    return ipnet->task.pipe;
}


/*
 *===========================================================================
 *                   ipnet_usr_sock_get_response_pipe
 *===========================================================================
 * Description: Returns a pipe that can be used to send messages from
 *              the "ipnetd" task back to this task.
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ipcom_pipe *
ipnet_usr_sock_get_response_pipe(void)
{
#if 0
    Ipcom_proc *proc = ipcom_proc_self();

    ip_assert(proc != IP_NULL);

    if (IP_UNLIKELY(proc->pipe == IPCOM_PIPE_INVALID))
    {
        Ipcom_pipe_attr attr;

        ipcom_pipe_attr_init(&attr);
        attr.msg_limit = IPNET_SOCK_TASK_PIPE_SIZE;
        proc->pipe = ipcom_pipe_new(IPCOM_PIPE_GENERIC, &attr);

        if (proc->pipe == IPCOM_PIPE_INVALID)
            IP_PANIC();
    }

    return proc->pipe;
#else
    return ipnet->task.rpipe;
#endif
}


/*
 *===========================================================================
 *                    ipnet_usr_sock_init_sig
 *===========================================================================
 * Description: Initialize a new signal.
 * Parameters:  sig - the signal to initialize
 *              sig_type - the signal type
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_usr_sock_init_sig(Ipnet_sig *sig, enum Ipnet_sig_type sig_type)
{
    sig->sig_type      = sig_type;
    sig->response_pipe = ipnet_usr_sock_get_response_pipe();
    sig->sender_pid    = ipcom_getpid();
    sig->receiver_pid  = ipnet->task.pid;
    sig->free          = IP_NULL;
    sig->freesig       = IP_NULL;
#if IPCOM_VR_MAX > 1
    sig->sender_vr     = ipnet_user_vr();
#endif
    ipcom_atomic_set(&sig->garbage_tag, 0);
}


/*
 *===========================================================================
 *                     ipnet_usr_sock_get_route_cache_id
 *===========================================================================
 * Description: Returns a pointer to the 'route_cache_id', which changes when the
 *              'route_cache' is invalidated
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL volatile int *
ipnet_usr_sock_get_route_cache_id(void)
{
    return &ipnet->route_cache_id;
}


/*
 *===========================================================================
 *                       ipnet_release_sock
 *===========================================================================
 * Description:
 * Parameters:  sock - socket to release.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_release_sock(Ipnet_socket *sock)
{
    if (IP_UNLIKELY(ipcom_atomic_sub_and_return(&sock->ref_count, 1) == 0))
    {
        Ipnet_sig *sig;

        sig = ipcom_malloc(sizeof(Ipnet_sig));

        if (sig == IP_NULL)
            return;

        sig->sig_type = IPNET_SIG_SOCKETFREE;
        sig->d.close  = sock->ipcom.fd;

        if (ipnet_usr_sock_basic_send_recv(sig) != IPCOM_ERR_INTERRUPT)
            ipcom_free(sig);
    }
}


/*
 *===========================================================================
 *                 ipnet_usr_sock_wait_until_writable
 *===========================================================================
 * Description: Waits until the specified descriptor is writable.
 * Parameters:  fd - file descriptor that must be writable.
 * Returns:     IP_TRUE - socket is ready.
 *              IP_FALSE - poll operation timed out.
 *
 */
IP_GLOBAL Ip_bool
ipnet_usr_sock_wait_until_writable(Ipnet_socket *sock,
                                   int lowat,
                                   struct Ip_timeval *timeout)
{
    if (ipnet_is_sock_writable(sock, lowat))
        return IP_TRUE;
    return ipnet_usr_sock_wait_until(sock,
                                     lowat,
                                     timeout,
                                     IPNET_SOCK_POLL_WRITE);
}


/*
 *===========================================================================
 *                    ipnet_usr_sock_recvmsg
 *===========================================================================
 * Description: Receive implementation that works for all protocols that
 *              has its data queued as Ipcom_pkt:s at the sock->rcv_head
 *              and sock->rcv_tail pointers. One packet will be returned
 *              in each call and removed from the queue.
 * Parameters:  sock - The socket to receive the packet from.
 *              msg - Receive parameters.
 *              flags - IP_MSG_xxx flags.
 * Returns:     0 = a packet was successfully received, -1 = error.
 *
 */

IP_GLOBAL int
ipnet_usr_sock_recvmsg(Ipnet_socket *sock, struct Ip_msghdr *msg, int flags)
{
    Ipcom_pkt *pkt;
    int        pkt_offset;
    int        buf_len;
    Ip_u8     *buf;
    int        bytes;
    Ip_size_t  i;
    int        read_bytes;

    IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                      1, 12, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                      ipnet_usr_sock_recvmsg, IPCOM_WV_NETD_INFO_RECEIVE,
                      IPCOM_WV_IPNET_SOCK_MODULE, IPCOM_WV_NETD_SOCKET);
    IPNET_STATS(usr_sock_recvmsg++);
    ip_assert(sock != IP_NULL);

    ipcom_spinlock_lock(sock->rcv_lock);
    pkt = sock->rcv_head;
    if (pkt == IP_NULL)
    {
        ipcom_spinlock_unlock(sock->rcv_lock);
        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                          1, 13, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                          ipnet_usr_sock_recvmsg, IPCOM_WV_NETD_INFO_WAIT,
                          IPCOM_WV_IPNET_SOCK_MODULE, IPCOM_WV_NETD_SOCKET);
        IPNET_STATS(usr_sock_recvmsg_ewouldblock++);
        return -IP_ERRNO_EWOULDBLOCK;
    }

    if (IP_LIKELY(IP_BIT_ISFALSE(flags, IP_MSG_PEEK)))
    {
        /* Unlink the first packet from the queue head. */
        sock->ipcom.rcv_bytes -= pkt->maxlen;
        sock->rcv_head = pkt->next;
        if (sock->rcv_head == IP_NULL)
            sock->rcv_tail = IP_NULL;
    }
    ipcom_spinlock_unlock(sock->rcv_lock);

    /* Copy of sender info. */
    if (msg->msg_name != IP_NULL)
    {
        Ip_size_t namelen = msg->msg_namelen;

        ipnet_copy_sender_info(sock,
                               pkt,
                               msg->msg_name,
                               &namelen);
        msg->msg_namelen = namelen;
    }

#ifdef IPCOM_USE_PACKET
    if (sock->ipcom.domain == IP_AF_PACKET && sock->ipcom.type == IP_SOCK_DGRAM)
    {
        /* The link header should not be included when the type is
           IP_SOCK_DGRAM */
        Ipnet_netif *netif = ipnet_if_indextonetif(pkt->vr_index, pkt->ifindex);
        if (netif != IP_NULL)
            pkt->start += netif->ipcom.link_hdr_size;
    }
#endif /* IPCOM_USE_PACKET */

#ifdef IPCOM_ZEROCOPY
    if (IP_BIT_ISSET(flags, IP_MSG_ZBUF))
    {
        /* The buffer will contain a pointer to a Ipcom_pkt upon
           return */
        ip_assert(msg->msg_iov[0].iov_len >= sizeof(Ipcom_pkt*));

        *(Ipcom_pkt**)msg->msg_iov[0].iov_base = pkt;
        return pkt->end - pkt->start;
    }
#endif /*IPCOM_ZEROCOPY*/

    if (msg->msg_control != IP_NULL)
    {
        Ipnet_sig *sig;

        sig = ipcom_malloc(sizeof(Ipnet_sig));

        if (sig == IP_NULL)
        {
            (void)ipcom_errno_set(IP_ERRNO_ENOMEM);
            goto errout;
        }

        sig->sig_type             = IPNET_SIG_GETANCILLARYDATA;
        sig->d.ancillary_data.fd  = sock->ipcom.fd;
        sig->d.ancillary_data.msg = msg;
        sig->d.ancillary_data.pkt = pkt;

        if (ipnet_usr_sock_basic_send_recv(sig) == IPCOM_ERR_INTERRUPT)
        {
            (void)ipcom_errno_set(IP_ERRNO_EINTR);
            goto errout;
        }

        ipcom_free(sig);
    }

    pkt_offset = pkt->start;
    for (bytes = 0, i = 0; i < msg->msg_iovlen && pkt_offset < pkt->end; i++)
    {
        buf = (Ip_u8 *) msg->msg_iov[i].iov_base;
        buf_len = msg->msg_iov[i].iov_len;

        /* Copy over minimum of bytes requested and available,
           i.e. truncation is ok. */
        read_bytes = IP_MIN(buf_len, (int) pkt->end - pkt_offset);

        if (read_bytes > 0)
        {
            ipcom_memcpy(buf, &pkt->data[pkt_offset], read_bytes);
            bytes += read_bytes;
            pkt_offset += read_bytes;
        }
    }

    if (pkt->end > pkt_offset)
        IP_BIT_SET(msg->msg_flags, IP_MSG_TRUNC);

    if (IP_BIT_ISFALSE(flags, IP_MSG_PEEK))
        ipcom_pkt_free(pkt);

    return bytes;

 errout:

    return IP_SOCKERR;
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 * The prototypes for these functions are located in
 * <IPCOM>/include/ipnet_sock.h
 */

/*
 *===========================================================================
 *                    ipcom_accept
 *===========================================================================
 * Description: Accepts an incoming connection attempt on a socket.
 * Parameters:  fd - A socket that is in listen state (ipcom_listen())
 *                    has been called.
 *              from - Will hold the address of the peer (set to
 *                    IP_NULL if that information is not wanted).
 *              addrlen - Must be initialized to the length of the
 *                    'from' buffer, will hold the actual 'from'
 *                    length upon return. Set to IP_NULL if 'from' is
 *                    IP_NULL.
 * Returns:     0 >= the socket to use for communication (success),
 *              -1 = error.
 *
 */
IP_PUBLIC int
ipcom_accept(Ip_fd fd,
             struct Ip_sockaddr *from,
             Ip_socklen_t *fromlen)
{
    Ipnet_sig    *sig;
    Ipnet_socket *sock;
    int           ret;

    sock = ipnet_get_sock(fd);
    if (sock == IP_NULL)
        return IP_SOCKERR;
    if (IP_BIT_ISFALSE(sock->flags, IPNET_SOCKET_FLAG_NONBLOCKING))
        ipnet_usr_sock_wait_until_readable(sock, 0, IP_NULL);
    ipnet_release_sock(sock);

    sig = ipcom_malloc(sizeof(Ipnet_sig));

    if (sig == IP_NULL)
    {
        (void)ipcom_errno_set(IP_ERRNO_ENOMEM);
        goto errout;
    }

    sig->sig_type    = IPNET_SIG_ACCEPT;
    sig->d.accept.fd = fd;
    if (from == IP_NULL || fromlen == IP_NULL)
        sig->d.accept.fromlen = 0;
    else
        sig->d.accept.fromlen = *fromlen;

    ret = ipnet_usr_sock_basic_send_recv(sig);

    if (ret == IPCOM_ERR_INTERRUPT)
    {
        (void)ipcom_errno_set(IP_ERRNO_EINTR);
        goto errout;
    }

    if (ret >= 0 && from && fromlen)
    {
        ipcom_memcpy(from, &sig->d.accept.from, *fromlen);
    }

    ipcom_free(sig);

    return ret;

 errout:

    return IP_SOCKERR;
}


/*
 *===========================================================================
 *                    ipcom_bind
 *===========================================================================
 * Description: Assigns a name to a unnamed socket.
 * Parameters:  fd - The socket to name.
 *              name - The socket name.
 *              namelen - The length of the name buffer.
 * Returns:     0 = success, -1 = error
 *
 */
IP_PUBLIC int
ipcom_bind(Ip_fd fd,
           IP_CONST struct Ip_sockaddr *name,
           Ip_socklen_t namelen)
{
    Ipnet_sig *sig;
    int        ret;

    sig = ipcom_malloc(sizeof(Ipnet_sig));

    if (sig == IP_NULL)
    {
        (void)ipcom_errno_set(IP_ERRNO_ENOMEM);
        goto errout;
    }

    sig->sig_type  = IPNET_SIG_BIND;
    sig->d.bind.fd = fd;
    if (name == IP_NULL || namelen == IP_NULL)
        sig->d.bind.namelen = 0;
    else
    {
        sig->d.bind.namelen = namelen;
        ipcom_memcpy(&sig->d.bind.name,
                     name,
                     IP_MIN(namelen, sizeof(sig->d.bind.name)));
    }

    ret = ipnet_usr_sock_basic_send_recv(sig);

    if (ret == IPCOM_ERR_INTERRUPT)
    {
        (void)ipcom_errno_set(IP_ERRNO_EINTR);
        goto errout;
    }

    ipcom_free(sig);
    return ret;

 errout:

    return IP_SOCKERR;
}


/*
 *===========================================================================
 *                    ipcom_connect
 *===========================================================================
 * Description: Connects the socket to a specified peer.
 * Parameters:  fd - The socket to connect.
 *              to - The address of the peer.
 *              tolen - The length of the to buffer.
 * Returns:     0= connection was successful, -1 = error.
 *
 */
IP_PUBLIC int
ipcom_connect(Ip_fd fd,
              IP_CONST struct Ip_sockaddr *to,
              Ip_socklen_t tolen)
{
    Ipnet_sig *sig;
    int        ret;

    sig = ipcom_malloc(sizeof(Ipnet_sig));

    if (sig == IP_NULL)
    {
        (void)ipcom_errno_set(IP_ERRNO_ENOMEM);
        goto errout;
    }

    sig->sig_type  = IPNET_SIG_CONNECT;
    sig->d.connect.fd = fd;
    if (to == IP_NULL || tolen == IP_NULL)
        sig->d.connect.tolen = 0;
    else
    {
        sig->d.connect.tolen = tolen;
        ipcom_memcpy(&sig->d.connect.to,
                     to,
                     IP_MIN(tolen, sizeof(sig->d.connect.to)));
    }

    ret = ipnet_usr_sock_basic_send_recv(sig);

    if (ret == IPCOM_ERR_INTERRUPT)
    {
        (void)ipcom_errno_set(IP_ERRNO_EINTR);
        goto errout;
    }

    ipcom_free(sig);

    if (ret < 0 && ipcom_errno == IP_ERRNO_EINPROGRESS)
    {
        Ipnet_socket *sock;
        Ip_bool       is_blocking;

        sock = ipnet_get_sock(fd);
        if (sock == IP_NULL)
            return IP_SOCKERR;

        is_blocking = IP_BIT_ISFALSE(sock->flags, IPNET_SOCKET_FLAG_NONBLOCKING);
        if (is_blocking)
        {
            ipnet_usr_sock_wait_until_writable(sock, IP_NULL, 0);
            ipcom_errno_set(sock->ipcom.so_errno);
            if (sock->ipcom.so_errno == 0)
                ret = 0;
        }

        ipnet_release_sock(sock);
    }

    return ret;

 errout:

    return IP_SOCKERR;
}


/*
 *===========================================================================
 *                    ipcom_getsockname
 *===========================================================================
 * Description: Returns the name of the specified socket.
 * Parameters:  fd - A socket descriptor.
 *              name - A buffer that will receive the name of the socket.
 *              namelenp - A pointer to the length of name, will be updated
 *              to contain the actual number of octets copied to name.
 * Returns:     0 = success, -1 = error
 *
 */
IP_PUBLIC int
ipcom_getsockname(Ip_fd fd, struct Ip_sockaddr *name, Ip_socklen_t *namelenp)
{
    return ipnet_usr_sock_getname(fd, name, namelenp, IP_TRUE);
}


/*
 *===========================================================================
 *                    ipcom_getsockopt
 *===========================================================================
 * Description: Gets the specified option on a socket. Options may
 *              exist at multiple protocol levels.
 * Parameters:  fd - A socket descriptor.
 *              level - The protocol level that should be affected by the
 *              option.
 *              optname - The option name.
 *              optval - Pointer to a buffer where the option data should be
 *              copied..
 *              optlen - The length of the buffer poited to by optval.
 * Returns:     0 = success, -1 = error.
 *
 */
IP_PUBLIC int
ipcom_getsockopt(Ip_fd fd,
                 int level,
                 int optname,
                 void *optval,
                 Ip_socklen_t *optlen)
{
    return ipnet_usr_sockopt(fd,
                             level,
                             optname,
                             optval,
                             optlen,
                             IPNET_SIG_GETSOCKOPT);
}


/*
 *===========================================================================
 *                    ipcom_getpeername
 *===========================================================================
 * Description: Returns the name of the peer connected to the specified socket.
 * Parameters:  fd - A socket descriptor.
 *              name - A buffer that will receive the name of the peer.
 *              namelenp - A pointer to the length of name, will be updated
 *              to contain the actual number of octets copied to name.
 * Returns:     0 = success, -1 = error
 *
 */
IP_PUBLIC int
ipcom_getpeername(Ip_fd fd, struct Ip_sockaddr *name, Ip_socklen_t *namelenp)
{
    return ipnet_usr_sock_getname(fd, name, namelenp, IP_FALSE);
}


/*
 *===========================================================================
 *                    ipcom_socketioctl
 *===========================================================================
 * Description: IO control.
 * Parameters:  fd - A socket descriptor.
 *              request - The IO control request.
 *              argp - Request specific data.
 * Returns:     0 = success, -1 = error (error code from ipcom_errno).
 *
 */
IP_PUBLIC int
ipcom_socketioctl(Ip_fd fd, unsigned long request, void *argp)
{
    int       ret = -1;
    /* Allocate an extra sizeof(void*) rationale same as for setsockopt */
#ifndef IP_PORT_VXWORKS
    char         sig_buf[sizeof(Ipnet_sig)];
    Ipnet_sig   *sig = (Ipnet_sig *) sig_buf;
#else
    Ipnet_sig   *sig;
    Ip_size_t    arg_len;
    struct Ip_ifconf *arg_ifconf = argp;
    struct Ip_ifconf *ifconf = IP_NULL;

    arg_len = IP_IOCPARM_LEN(request);
    if (request == IP_SIOCGIFCONF)
        arg_len += arg_ifconf->ifc_len;

    /* parameter check, for exception. */
    if (IP_NULL == argp)
    {
        ipcom_errno_set(IP_ERRNO_EINVAL);
        return ret;
    }
    sig = ipcom_malloc(sizeof(Ipnet_sig)
                       + sizeof(void*)
                       + arg_len);
    if (sig == IP_NULL)
    {
        ipcom_errno_set(IP_ERRNO_ENOMEM);
        return ret;
    }
#endif

    sig->sig_type        = IPNET_SIG_IOCTL;
    sig->d.ioctl.fd      = fd;
    sig->d.ioctl.request = request;

#ifdef IP_PORT_VXWORKS
    sig->d.ioctl.argp    = IP_PTR_ALIGN((sig + 1), sizeof(void*));

    /* vxWorks RTP copy user data */
    ipcom_memcpy(sig->d.ioctl.argp, argp, IP_IOCPARM_LEN(request));
    if (request == IP_SIOCGIFCONF)
    {
        ifconf = sig->d.ioctl.argp;
        ifconf->ifc_ifcu.ifcu_buf = (void *) (ifconf + 1);
    }

    if ((ret = ipnet_usr_sock_basic_send_recv(sig)) == 0)
    {
        /* Copy results back to RTP space */
        if (request == IP_SIOCGIFCONF)
        {
            /* Copy back the embedded buffer */
            arg_ifconf->ifc_len = ifconf->ifc_len;
            memcpy(arg_ifconf->ifc_ifcu.ifcu_buf,
                   ifconf->ifc_ifcu.ifcu_buf,
                   ifconf->ifc_len);
        }
        else
            ipcom_memcpy(argp, sig->d.ioctl.argp, IP_IOCPARM_LEN(request));
    }
    ipcom_free(sig);
#else
    sig->d.ioctl.argp    = argp;
    ret = ipnet_usr_sock_basic_send_recv(sig);
#endif

    return ret;
}


/*
 *===========================================================================
 *                    ipcom_listen
 *===========================================================================
 * Description: Places a socket a state where it is listening for
 *                    incoming connection(s).
 * Parameters:  fd - the socket to create a backlog for.
 *              backlog - The maximum length of the queue of pending
 *                    connections. Set to 0 to allow the stack to
 *                    select a reasonably sized backlog.
 * Returns:     0 = success, -1 = error.
 *
 */
IP_PUBLIC int
ipcom_listen(Ip_fd fd, int backlog)
{
    Ipnet_sig *sig;
    int        ret;

    sig = ipcom_malloc(sizeof(Ipnet_sig));

    if (sig == IP_NULL)
    {
        (void)ipcom_errno_set(IP_ERRNO_ENOMEM);
        goto errout;
    }

    sig->sig_type         = IPNET_SIG_LISTEN;
    sig->d.listen.fd      = fd;
    sig->d.listen.backlog = backlog;

    ret = ipnet_usr_sock_basic_send_recv(sig);

    if (ret == IPCOM_ERR_INTERRUPT)
    {
        (void)ipcom_errno_set(IP_ERRNO_EINTR);
        goto errout;
    }

    ipcom_free(sig);

    return ret;

 errout:

    return IP_SOCKERR;
}


/*
 *===========================================================================
 *                    ipcom_recv
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_ssize_t
ipcom_recv(Ip_fd sock, void *buf, Ip_size_t len, int flags)
{
    return ipcom_recvfrom(sock, buf, len, flags, IP_NULL, IP_NULL);
}


/*
 *===========================================================================
 *                    ipcom_recvfrom
 *===========================================================================
 * Description: Receives data from a socket.
 * Parameters:  fd - A socket descriptor.
 *              buf - A buffer to where the data should be copied.
 *              len - The length of buf buffer.
 *              flags - Receive flags
 *              from - The sender of the data.
 *              fromlen - The length of the from buffer, will be updated to
 *              contain the number of octets copied to the from buffer.
 * Returns:     >=0 = the number of bytes copied to buf, -1 = error.
 *
 */
IP_PUBLIC Ip_ssize_t
ipcom_recvfrom(Ip_fd fd,
               void *buf,
               Ip_size_t len,
               int flags,
               struct Ip_sockaddr *from,
               Ip_socklen_t *fromlen)
{
    struct Ip_msghdr msg;
    struct Ip_iovec  iov[1];
    int              ret;

    iov[0].iov_base = buf;
    iov[0].iov_len  = len;

    if (fromlen != IP_NULL)
        msg.msg_namelen = *fromlen;
    else
        msg.msg_namelen = 0;
    msg.msg_name       = from;
    msg.msg_iov        = iov;
    msg.msg_iovlen     = 1;
    msg.msg_control    = IP_NULL;
    msg.msg_controllen = 0;
    msg.msg_flags      = flags;

    ret = ipcom_recvmsg(fd, &msg, flags);
    if (fromlen != IP_NULL)
        *fromlen = msg.msg_namelen;
    return (Ip_ssize_t)ret;
}


/*
 *===========================================================================
 *                    ipcom_recvmsg
 *===========================================================================
 * Description: Receives data from a socket.
 * Parameters:  fd - A socket descriptor.
 *              msg - Receive parameters.
 *              flags - IP_MSG_xxx flags.
 * Returns:     0 = a packet was successfully received, -1 = error.
 *
 */
IP_PUBLIC Ip_ssize_t
ipcom_recvmsg(Ip_fd fd, struct Ip_msghdr *msg, int flags)
{
    int           bytes;
    int           lowat;
    Ipnet_socket *sock;
    Ip_bool       is_blocking;

    IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                      1, 10, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                      ipcom_recvmsg, IPCOM_WV_NETD_INFO_RECEIVE,
                      IPCOM_WV_IPNET_SOCK_MODULE, IPCOM_WV_NETD_SOCKET);
    IPNET_STATS(sock_recvmsg++);

    ip_assert2(msg != IP_NULL);

 retry:
    lowat = 0;
    sock = ipnet_get_sock(fd);
    if (IP_UNLIKELY(sock == IP_NULL))
    {
        ipcom_errno_set(IP_ERRNO_EBADF);
        return IP_SOCKERR;
    }

    is_blocking = IP_BIT_ISFALSE(sock->flags, IPNET_SOCKET_FLAG_NONBLOCKING)
        && IP_BIT_ISFALSE(flags, IP_MSG_DONTWAIT);

    if (is_blocking)
    {
        Ip_size_t i;

        bytes = 0;
        for (i = 0; i < msg->msg_iovlen; i++)
            bytes += msg->msg_iov[i].iov_len;

        if (IP_UNLIKELY(IP_BIT_ISSET(flags, IP_MSG_WAITALL)))
            lowat = bytes;

        if (IP_LIKELY(IP_BIT_ISFALSE(flags, IP_MSG_OOB))
            && IP_LIKELY(bytes > 0))
            is_blocking = ipnet_usr_sock_wait_until_readable(sock,
                                                             lowat,
                                                             sock->rcv_timeout);
        /* else: always possible to read zero bytes */
    }

    if (sock->ipcom.rcv_bytes > 0
        || (IP_LIKELY(sock->ipcom.so_errno == 0)
            && IP_LIKELY(IP_BIT_ISFALSE(sock->flags, IPNET_SOCKET_FLAG_READ_CLOSED))))
    {
        /* Any queued data should be delivered before the error is
           delivered */
        bytes = sock->ops->usr_recv(sock, msg, flags);
    }
    else
    {
        bytes = -sock->ipcom.so_errno;
#ifdef IP_PORT_LKM
        if (sock->ipcom.so_errno == IP_ERRNO_EINTR)
            sock->ipcom.so_errno = 0;
#endif
        msg->msg_controllen = 0;
    }

    ipnet_release_sock(sock);

    if (is_blocking
        && IP_UNLIKELY(bytes == -IP_ERRNO_EWOULDBLOCK)
        && IP_BIT_ISFALSE(flags, IP_MSG_OOB))
        goto retry;

    if (IP_UNLIKELY(bytes < 0))
    {
        ipcom_errno_set(-bytes);
        return IP_SOCKERR;
    }
    return bytes;
}


/*
 *===========================================================================
 *                    ipcom_send
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_ssize_t
ipcom_send(Ip_fd sock, IP_CONST void *msg, Ip_size_t len, int flags)
{
    return ipcom_sendto(sock, msg, len, flags, IP_NULL, IP_NULL);
}


/*
 *===========================================================================
 *                      ipcom_sendmsg
 *===========================================================================
 * Description: Sends a packet to the address to which the socket is
 *              connected or to the 'to_param' address if the socket
 *              is not connected.
 * Parameters:  fd - The descriptor for the socket to use.
 *              msg - Send parameters.
 *              flags - IP_MSG_xxx flags.
 * Returns:     >=0 = number of bytes sent, IP_SOCKERR = error code is
 *              retrieved by using ipcom_errno.
 *
 */

IP_PUBLIC Ip_ssize_t
ipcom_sendmsg(Ip_fd fd, IP_CONST struct Ip_msghdr *msg, int flags)
{
    Ip_ssize_t    len;
    int           total_buf_len = 0;
    int           offset = 0;
    int           err;
    Ip_size_t     i;
    Ipnet_socket *sock;
    Ipnet_sig    *sig;
    Ipnet_sig    *rsig = IP_NULL;
    Ip_bool       is_synchronous;
    int           sock_route_cache_id;
    Ipcom_pkt    *pkt;
    struct Ip_msghdr msgbuf;

#ifdef IPCOM_ZEROCOPY
    if (IP_BIT_ISSET(flags, IP_MSG_ZBUF))
    {
        sock = ipnet_get_sock(fd);
        /* TCP or RAW socket.. */
        if (sock->ipcom.domain != IP_AF_INET
            || sock->ipcom.type != IP_SOCK_DGRAM)
        {
            struct Ip_msghdr lmsg;
            struct Ip_iovec  iovec[10];
            int              ret = 0;
            Ipcom_pkt       *zpkt;

            zpkt = msg->msg_iov[0].iov_base;
            lmsg = *msg;
            lmsg.msg_iov = iovec;
            lmsg.msg_iovlen = 0;
            for (pkt = zpkt, i = 0;
                 pkt != IP_NULL;
                 pkt = pkt->next_part, i++)
            {
                lmsg.msg_iov[i].iov_base = &pkt->data[pkt->start];
                lmsg.msg_iov[i].iov_len = pkt->end - pkt->start;
                lmsg.msg_iovlen++;
            }
            ip_assert(lmsg.msg_iovlen <= 10);

            ipnet_release_sock(sock);

            ret = ipcom_sendmsg(fd, &lmsg, flags & ~IP_MSG_ZBUF);
            ipcom_pkt_free(zpkt);
            return ret;
        }
        ipnet_release_sock(sock);
    }
#endif /* IPCOM_ZEROCOPY */

    for (i = 0; i < msg->msg_iovlen; i++)
        total_buf_len += (int) msg->msg_iov[i].iov_len;

    do
    {
        sock = ipnet_get_sock(fd);
        if (IP_UNLIKELY(sock == IP_NULL))
        {
            err = IP_ERRNO_EBADF;
            goto errout;
        }

        if (IP_UNLIKELY(sock->ipcom.so_errno != 0))
        {
            err = sock->ipcom.so_errno;
            goto errout;
        }

        if (sock->ops->connect != IP_NULL)
        {
            if (IP_BIT_ISSET(sock->flags,
                             IPNET_SOCKET_FLAG_CONNECTED | IPNET_SOCKET_FLAG_CONNECTING))
            {
                if (IP_UNLIKELY(msg->msg_name != IP_NULL))
                {
                    IPCOM_LOG1(DEBUG,
                               "SOCKET: a 'to' address must NOT be specified, socket %d is connected",
                               sock->ipcom.fd);
                    err = IP_ERRNO_EISCONN;
                    goto errout;
                }
            }
            else                {

                if (IP_UNLIKELY(msg->msg_name == IP_NULL))
                {
                    if (sock->snd_head == IP_NULL)
                    {
                        IPCOM_LOG1(DEBUG,
                                   "SOCKET: a 'to' address must be specified for socket %d",
                                   sock->ipcom.fd);
                        err = IP_ERRNO_EDESTADDRREQ;
                        goto errout;
                    }
                    /* else: the to address has been specified in an
                       earlier call that had IP_MSG_MORE set, reuse the
                       address */
                    ipcom_memcpy(&msgbuf, msg, sizeof(msgbuf));
                    msg = &msgbuf;
                    msgbuf.msg_name    = sock->snd_to;
                    msgbuf.msg_namelen = IPCOM_SA_LEN_GET(sock->snd_to);
                }
                else if (IP_UNLIKELY(((struct Ip_sockaddr *)msg->msg_name)->sa_family
                                     != sock->ipcom.domain))
                {
                    IPCOM_LOG3(DEBUG, "invalid domain for socket %d, must be %d, found %d",
                               sock->ipcom.fd,
                               sock->ipcom.domain,
                               ((struct Ip_sockaddr *)msg->msg_name)->sa_family);
                    err = IP_ERRNO_EINVAL;
                    goto errout;
                }
            }
        }

#ifdef IPCOM_ZEROCOPY
        /* zbuf UDP packets go here */
        if (IP_BIT_ISSET(flags, IP_MSG_ZBUF)
            && sock->ipcom.type == IP_SOCK_DGRAM)
        {
            Ipcom_pkt                  *header;
            Ip_bool                     znon_blocking;
            Ipnet_pkt_info_sock_snd_buf snd_buf_info;

            header = ipcom_pkt_malloc(IPNET_ZBUF_MAX_HEADER_LEN, IP_FLAG_FC_STACKCONTEXT);
            pkt = msg->msg_iov[0].iov_base;
            header->next_part = pkt;
            header->flags |= IPCOM_PKT_FLAG_ZBUF;
            len = ipcom_pkt_get_length(pkt);

            header->chk = 0;
            /*do
            {
                header->chk += ipcom_in_checksum_update(&pkt->data[pkt->start], pkt->end - pkt->start);
                pkt = pkt->next_part;
            } while (pkt != IP_NULL);*/

            pkt = header;
            pkt->start = pkt->end = pkt->maxlen;
            pkt->dtor = sock->pkt_dtor;

            znon_blocking = IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_NONBLOCKING)
                || IP_BIT_ISSET(flags, IP_MSG_DONTWAIT);

            while (ipcom_atomic_add_and_return(&sock->snd_bytes, len)
                   >= sock->send_max_bytes + len)
            {
                ipcom_atomic_sub(&sock->snd_bytes, len);

                /* The requested number of bytes cannot be sent at the
                   moment */
                if (znon_blocking)
                {
                    /* Non blocking send */
                    ipcom_pkt_free(pkt);
                    err = -IP_ERRNO_EWOULDBLOCK;
                    goto errout;
                }
                ipnet_usr_sock_wait_until_writable(sock, len, IP_NULL);
                if (sock->ipcom.so_errno)
                {
                    ipcom_pkt_free(pkt);
                    err = -sock->ipcom.so_errno;
                    goto errout;

                }
            }

            snd_buf_info.sock        = sock;
            snd_buf_info.payload_len = len;
            ipcom_pkt_set_info(pkt,
                               IPNET_PKT_INFO_SOCK_SND_BUF,
                               sizeof(snd_buf_info),
                               &snd_buf_info);
        }
        else
#endif /* IPCOM_ZEROCOPY */
            len = sock->ops->pkts_from_iov(sock,
                                           msg,
                                           flags,
                                           offset,
                                           total_buf_len,
                                           &pkt);
        sock_route_cache_id = sock->snd_route_cache_id;
        ipnet_release_sock(sock);
        sock = IP_NULL;

        if (IP_UNLIKELY(len < 0))
        {
            if (offset)
                return offset;
            err = -len;
            goto errout;
        }

        /* Packet will be IP_NULL if all the data was appended to a
           previous packet that has not been sent yet */
        if (pkt != IP_NULL)
        {
            IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_SENT_PIPE);

            sig = ipnet_usr_sock_pkt_to_sig(pkt);
            ipnet_usr_sock_init_sig(sig, IPNET_SIG_SENDMSG);

            sig->d.sendmsg.fd              = fd;
            sig->d.sendmsg.pkt             = pkt;
            sig->d.sendmsg.msg.msg_namelen = IP_MIN(msg->msg_namelen, (int) sizeof(sig->d.sendmsg.to));
            sig->d.sendmsg.msg.msg_name    = msg->msg_namelen ? &sig->d.sendmsg.to : IP_NULL;

            if (msg->msg_control == IP_NULL
                || msg->msg_controllen == 0)
            {
                sig->d.sendmsg.msg.msg_control    = IP_NULL;
                sig->d.sendmsg.msg.msg_controllen = 0;
            }
            else
            {
                sig->d.sendmsg.msg.msg_controllen = msg->msg_controllen;
                sig->d.sendmsg.msg.msg_control    = ipcom_malloc(msg->msg_controllen);
                if (sig->d.sendmsg.msg.msg_control == IP_NULL)
                {
                    ipcom_pkt_free(pkt);
                    err = IP_ERRNO_ENOMEM;
                    goto errout;
                }
                ipcom_memcpy(sig->d.sendmsg.msg.msg_control,
                             msg->msg_control,
                             msg->msg_controllen);
            }

            /* The 'msg_flags' is not used by send operations, lets store the
               send 'flags' in the 'msg_flags' field. */
            sig->d.sendmsg.msg.msg_flags   = flags;
            ipcom_memcpy(&sig->d.sendmsg.to,
                         msg->msg_name,
                         sig->d.sendmsg.msg.msg_namelen);
            ip_assert(sig->sig_type == IPNET_SIG_SENDMSG);

            is_synchronous = (ipnet->route_cache_id != sock_route_cache_id);
            if (IP_UNLIKELY(is_synchronous))
            {
                if (rsig == IP_NULL)
                {
                    rsig = ipcom_malloc(sizeof(*rsig));
                    if (rsig == IP_NULL)
                    {
                        ipcom_pkt_free(pkt);
                        err = IP_ERRNO_ENOMEM;
                        goto errout;
                    }
                }
                /* First call or last call failed, must make a
                   synchronous call to see if the send succeeds */
                ipnet_usr_sock_init_sig(rsig, IPNET_SIG_SENDMSG);
                sig->d.sendmsg.rsig = rsig;
            }
            else
            {
                /* Stack state unchanged since last successful call,
                   this call will succeed as well, so the send can be
                   asynchronous */
                sig->d.sendmsg.rsig = IP_NULL;
            }

            ipcom_pipe_send(ipnet->task.pipe, sig);

            if (IP_UNLIKELY(is_synchronous))
            {
                ipnet_usr_sock_pipe_recv(rsig->response_pipe,
                                         &sig,
                                         rsig,
                                         IP_NULL,
                                         0);

                ip_assert(sig == rsig);
                ip_assert(sig->sig_type == IPNET_SIG_SENDMSG);

                if (sig->reply.return_code < 0)
                {
                    if (offset)
                    {
                        if (rsig)
                            ipcom_free(rsig);
                        /* At least one byte has successfully been sent */
                        return offset;
                    }
                    err = sig->reply.sock_errno;
                    goto errout;
                }
            }
        }

        offset += len;
    } while (total_buf_len > offset);

    if (rsig)
        ipcom_free(rsig);

    return total_buf_len;

 errout:
    if (rsig)
        ipcom_free(rsig);
    if (sock)
        ipnet_release_sock(sock);
    ipcom_errno_set(err);
    return IP_SOCKERR;

}


/*
 *===========================================================================
 *                    ipcom_sendto
 *===========================================================================
 * Description: Sends a packet to the address to which the socket is connected
 *              or to the 'to' param address if the socket is not connected.
 * Parameters:  fd - The descriptor for the socket to use.
 *              buf - The buffer to send.
 *              len - The length of the buffer in octets.
 *              flags - send flags
 *              to - The destination for the data if the socket is
 *              unconnected, ignored for connected sockets.
 *              tolen - The size of the 'to_param' buffer in octets.
 * Returns:     >=0 = number of bytes sent, IP_SOCKERR = error code is
 *              retrivered by using ipcom_errno.
 *
 */
IP_PUBLIC Ip_ssize_t
ipcom_sendto(Ip_fd fd,
             IP_CONST void *buf,
             Ip_size_t len,
             int flags,
             IP_CONST struct Ip_sockaddr *to,
             Ip_socklen_t tolen)
{
    struct Ip_msghdr msg;
    struct Ip_iovec iov[1];

    msg.msg_name        = (struct Ip_sockaddr *) to;
    msg.msg_namelen     = tolen;
    iov[0].iov_base     = (void *) buf;
    iov[0].iov_len      = len;
    msg.msg_iov         = iov;
    msg.msg_iovlen      = 1;
    msg.msg_control     = IP_NULL;
    msg.msg_controllen  = 0;
    msg.msg_flags       = 0;
    return ipcom_sendmsg(fd, &msg, flags);
}


/*
 *===========================================================================
 *                    ipcom_setsockopt
 *===========================================================================
 * Description: Sets the specified option on a socket. Options may
 *              exist at multiple protocol levels.
 * Parameters:  fd - A socket descriptor.
 *              level - The protocol level that should be affected by the
 *              option.
 *              optname - The option name.
 *              optval - Data to set for the option.
 *              optlen - The length of the buffer poited to by optval.
 * Returns:     0 = success, -1 = error.
 *
 */
IP_PUBLIC int
ipcom_setsockopt(Ip_fd fd,
                 int level,
                 int optname,
                 IP_CONST void *optval,
                 Ip_socklen_t optlen)
{
    return ipnet_usr_sockopt(fd,
                             level,
                             optname,
                             (void *) optval,
                             &optlen,
                             IPNET_SIG_SETSOCKOPT);
}


/*
 *===========================================================================
 *                    ipcom_socketselect
 *===========================================================================
 * Description: Checks if any of the sockets in a set is ready for reading,
 *              writing or have an exceptional condition.
 * Parameters:  ndfs - The first 'ndfs' descriptors are checked in each set
 *              i.e., the descriptors from 0 through nfds-1 in the descriptor
 *              sets are examined.
 *              readfds - The set of descriptors that should be checked for
 *              reading.
 *              writeds - The set of descriptors that should be checked for
 *              writing.
 *              exceptfds - The set of descriptors that should be checked for
 *              exceptions.
 * Returns:     The total number of ready descriptors in each set.
 *
 */
IP_PUBLIC int
ipcom_socketselect(int nfds,
                   Ip_fd_set *readfds,
                   Ip_fd_set *writefds,
                   Ip_fd_set *exceptfds,
                   struct Ip_timeval *timeout)
{
    int                    ready_socks;
    int                    round;
    Ip_fd_set             *fds[3];
    int                    err;
    static Ipnet_for_each_sock_in_set_cb is_ready_func[] = {
        ipnet_is_sock_readable_adapter,
        ipnet_is_sock_writable_adapter,
        ipnet_is_sock_exceptional_adapter
    };
    static Ipnet_for_each_sock_in_set_cb send_pool_signal_func[] = {
        (Ipnet_for_each_sock_in_set_cb) ipnet_usr_sock_poll_readable,
        (Ipnet_for_each_sock_in_set_cb) ipnet_usr_sock_poll_writable,
        (Ipnet_for_each_sock_in_set_cb) ipnet_usr_sock_poll_exceptional
    };

    /* Round nfds up to nearest number evenly dividable by 32 */
    nfds = (nfds + 31) & ~31;

    fds[0] = readfds;
    fds[1] = writefds;
    fds[2] = exceptfds;

    for (round = (ipnet_timeval_to_msec(timeout) == 0 ? 1 : 2);
         round > 0;
         round--)
    {
        int r;
        int i;

        ready_socks = 0;
        err = 0;
        for (i = 0; i < 3; i++)
        {
            r = ipnet_for_each_sock_in_set(nfds,
                                           fds[i],
                                           round == 1,
                                           is_ready_func[i],
                                           IP_NULL,
                                           IP_NULL);
            if (r >= 0)
                ready_socks += r;
            else if (err == 0)
                err = r;
        }

        if (err != 0)
        {
            ipcom_errno_set(-err);
            return IP_SOCKERR;
        }

        if (round == 2 && ready_socks == 0)
        {
            Ipnet_sig                  *poll_done_sig;
            struct Ipnet_sig_poll_done *poll_done;

            poll_done_sig = ipnet_usr_sock_alloc_poll_done_sig();
            if (poll_done_sig == IP_NULL)
            {
                ipcom_errno_set(IP_ERRNO_ENOMEM);
                return IP_SOCKERR;
            }
            poll_done = &poll_done_sig->d.poll_done;

            /* Send poll signal for all sockets we are interested in */
            for (i = 0; i < 3 && err == 0; i++)
                err = ipnet_for_each_sock_in_set(nfds,
                                                 fds[i],
                                                 IP_FALSE,
                                                 send_pool_signal_func[i],
                                                 poll_done,
                                                 timeout);

            if (IP_LIKELY(err >= 0)
                || ipcom_atomic_add_and_return(&poll_done->responses_sent, 1) != 1)
            {
                Ipnet_sig  *sig;
                Ipcom_pipe *pipe;
                enum Ipnet_sig_type types[] = {
                    IPNET_SIG_POLL,
                    IPNET_SIG_POLL_TIMEOUT
                };

                /* Wait until at least one socket gets ready or the
                   timeout expires */
                pipe = ipnet_usr_sock_get_response_pipe();
                ipnet_usr_sock_pipe_recv(pipe,
                                         &sig,
                                         IP_NULL,
                                         types,
                                         2);
                ip_assert(sig->sig_type == IPNET_SIG_POLL
                          || (timeout
                              && sig->sig_type == IPNET_SIG_POLL_TIMEOUT));
            }

            ipnet_usr_sock_poll_done(poll_done_sig);
        }
    }

    return ready_socks;
}


/*
 *===========================================================================
 *                    ipcom_socketread
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_ssize_t
ipcom_socketread(Ip_fd sock, void *buf, Ip_size_t nbytes)
{
    return ipcom_recvfrom(sock, buf, nbytes, 0, IP_NULL, IP_NULL);
}


/*
 *===========================================================================
 *                    ipcom_socketwrite
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_ssize_t
ipcom_socketwrite(Ip_fd sock, IP_CONST void *buf, Ip_size_t nbytes)
{
    return ipcom_sendto(sock, buf, nbytes, 0, IP_NULL, 0);
}


/*
 *===========================================================================
 *                    ipcom_socketwritev
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_ssize_t
ipcom_socketwritev(Ip_fd fd, IP_CONST struct Ip_iovec *iov, int iovlen)
{
    struct Ip_msghdr msghdr;

    ipcom_memset(&msghdr, 0, sizeof(msghdr));
    msghdr.msg_iov = (struct Ip_iovec *) iov;
    msghdr.msg_iovlen = iovlen;

    return ipcom_sendmsg(fd, &msghdr, 0);
}


/*
 *===========================================================================
 *                    ipcom_socket
 *===========================================================================
 * Description: Creates a new socket.
 * Parameters:  domain - The address domain.
 *              type - The socket type.
 *              protocol - The protocol the stack must use for
 *                    communication when using this socket.
 * Returns:     0 >= The new socket descriptor.
 *              0 < error, error code can be read from ipcom_errno.
 *
 */
IP_PUBLIC Ip_fd
ipcom_socket(int domain, int type, int protocol)
{
    Ipnet_sig *sig;
    int        ret;

    sig = ipcom_malloc(sizeof(Ipnet_sig));

    if (sig == IP_NULL)
    {
        (void)ipcom_errno_set(IP_ERRNO_ENOMEM);
        goto errout;
    }

    sig->sig_type          = IPNET_SIG_SOCKET;
    sig->d.socket.domain   = domain;
    sig->d.socket.type     = type;
    sig->d.socket.protocol = protocol;

    ret = ipnet_usr_sock_basic_send_recv(sig);

    if (ret == IPCOM_ERR_INTERRUPT)
    {
        ipcom_errno_set(IP_ERRNO_EINTR);
        goto errout;
    }

    ipcom_free(sig);
    return ret;

 errout:

    return IP_SOCKERR;
}


/*
 *===========================================================================
 *                    ipcom_socketclose
 *===========================================================================
 * Description: Closes the socket and frees all resouces held by it.
 * Parameters:  sock - The socket to close.
 * Returns:     0 = success, <0 error code,
 */
IP_PUBLIC int
ipcom_socketclose(Ip_fd fd)
{
    Ipnet_sig    *sig;
    int           ret;

    sig = ipcom_malloc(sizeof(Ipnet_sig));

    if (sig == IP_NULL)
    {
        (void)ipcom_errno_set(IP_ERRNO_ENOMEM);
        goto errout;
    }

    sig->sig_type = IPNET_SIG_CLOSE;
    sig->d.close  = fd;

    ret = ipnet_usr_sock_basic_send_recv(sig);

    if (ret == IPCOM_ERR_INTERRUPT)
    {
        (void)ipcom_errno_set(IP_ERRNO_EINTR);
        goto errout;
    }

    ipcom_free(sig);
    return ret;

 errout:

    return IP_SOCKERR;
}


/*
 *===========================================================================
 *                    ipcom_shutdown
 *===========================================================================
 * Description: Shuts down the read and/or the write end of a socket.
 * Parameters:  fd - The file descriptor to shut down.
 *              how - How the socket should be shut down.
 * Returns:     0 = success, -1 = error
 */
IP_PUBLIC int
ipcom_shutdown(Ip_fd fd, int how)
{
    Ipnet_sig    *sig;
    int           ret;

    sig = ipcom_malloc(sizeof(Ipnet_sig));

    if (sig == IP_NULL)
    {
        (void)ipcom_errno_set(IP_ERRNO_ENOMEM);
        goto errout;
    }

    sig->sig_type       = IPNET_SIG_SHUTDOWN;
    sig->d.shutdown.fd  = fd;
    sig->d.shutdown.how = how;

    ret = ipnet_usr_sock_basic_send_recv(sig);

    if (ret == IPCOM_ERR_INTERRUPT)
    {
        (void)ipcom_errno_set(IP_ERRNO_EINTR);
        goto errout;
    }

    ipcom_free(sig);
    return ret;

 errout:
    return IP_SOCKERR;
}


/*
 *===========================================================================
 *                    ipcom_sysctl
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
 * Returns:     0 == success, <0 == error code.
 *
 */
IP_PUBLIC int
ipcom_sysctl(int *name, unsigned int namelen,
             void *oldp, Ip_size_t *oldlenp,
             void *newp, Ip_size_t newlen)
{
    Ipnet_sig *sig;
    int        ret;
    Ip_size_t  oldlen = (oldp ? *oldlenp : 0);

    sig = ipcom_malloc(sizeof(Ipnet_sig)
                       + oldlen
                       + newlen
                       + sizeof(void*));

    if (sig == IP_NULL)
    {
        (void)ipcom_errno_set(IP_ERRNO_ENOMEM);
        goto errout;
    }

    sig->sig_type         = IPNET_SIG_SYSCTL;
    /* name does not need to be copied since it has already been
       copied to a memory buffer residing within the kernel when using
       VxWorks RTPs */
    sig->d.sysctl.name    = name;
    sig->d.sysctl.namelen = namelen;
    sig->d.sysctl.oldp    = (sig + 1);
    sig->d.sysctl.oldlenp = &oldlen;
    sig->d.sysctl.newp    = IP_PTR_ALIGN(((char*) sig->d.sysctl.oldp + oldlen), sizeof(void*));
    sig->d.sysctl.newlen  = newlen;

    if (oldp == IP_NULL)
        sig->d.sysctl.oldp = IP_NULL;

    if (newp == IP_NULL)
        sig->d.sysctl.newp = IP_NULL;
    else
        ipcom_memcpy(sig->d.sysctl.newp, newp, newlen);

    ret = ipnet_usr_sock_basic_send_recv(sig);

    if (ret == IPCOM_ERR_INTERRUPT)
    {
        (void)ipcom_errno_set(IP_ERRNO_EINTR);
        goto errout;
    }

    if (ret == 0)
    {
        if (oldp)
            ipcom_memcpy(oldp, sig->d.sysctl.oldp, oldlen);

        if (oldlenp)
            *oldlenp = oldlen;
    }

    ipcom_free(sig);
    return ret;

 errout:
    return IP_SOCKERR;
}


/*
 *===========================================================================
 *                    ipcom_if_attach
 *===========================================================================
 * Description: Attaches a new network interface to IPNET.
 *
 * Parameters:  netif   - An initialized netif structure.
 *
 * Returns:      The interface index for this interface or negative errno.
 *
 */
IP_PUBLIC int
ipcom_if_attach(Ipcom_netif *netif)
{
    if (ipnet_is_stack_task())
        return ipnet_sys_if_attach(netif);
    return ipnet_do_if_attach(netif, IP_FALSE);
}


/*
 *===========================================================================
 *                    ipcom_if_attach_and_lock
 *===========================================================================
 * Description: Attaches a new network interface to IPNET and lock it.
 *              The caller is responsible for unlocking it when finished.
 *
 * Parameters:  netif   - An initialized netif structure.
 *
 * Returns:      The interface index for this interface or negative errno.
 *
 */
IP_PUBLIC int
ipcom_if_attach_and_lock(Ipcom_netif *ipcomif)
{
    if (ipnet_is_stack_task())
    {
        Ipnet_netif *netif = (Ipnet_netif*)ipcomif;
        int ret;

        ret = ipnet_sys_if_attach(ipcomif);

        if (ret >= 0)
            IPNET_IF_LOCK(netif);

        return ret;
    }

    return ipnet_do_if_attach(ipcomif, IP_TRUE);
}


/*
 *===========================================================================
 *                    ipcom_if_changelinkaddr
 *===========================================================================
 * Description: Change the link address on a network interface.
 * Parameters:  ipcomif - The interface for which the lladdr has changed.
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_if_changelinkaddr(Ipcom_netif *ipcomif, Ip_u8 *lladdr, Ip_u16 lladdr_sz)
{
    Ipnet_netif *netif = (Ipnet_netif*)ipcomif;
    Ipnet_sig   *sig;

    /* IPNET does not call this directly, if that changes in the
       future then make sure ipnet_sys_if_changemtu() is called if
       the caller is the IPNET daemon */
    ip_assert(ipnet_is_stack_task() == IP_FALSE);

    IPNET_IF_LOCK(netif);

    sig = ipcom_malloc(sizeof(Ipnet_sig));

    if (sig == IP_NULL)
    {
        (void)ipcom_errno_set(IP_ERRNO_ENOMEM);
        goto done;
    }

    sig->sig_type          = IPNET_SIG_IF_CHANGELINKADDR;
    sig->d.netif.netif     = ipcomif;
    sig->d.netif.link_addr = lladdr;
    sig->d.netif.size      = lladdr_sz;

    if (ipnet_pipe_basic_send_recv(sig) == IPCOM_ERR_INTERRUPT)
    {
        (void)ipcom_errno_set(IP_ERRNO_EINTR);
        goto done;
    }

    ipcom_free(sig);

 done:

    IPNET_IF_UNLOCK(netif);
}


/*
 *===========================================================================
 *                    ipcom_if_changemtu
 *===========================================================================
 * Description: Change the mtu of a network interface.
 * Parameters:  ipcomif - The interface for which the name has changed.
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_if_changemtu(Ipcom_netif *ipcomif, Ip_u32 new_mtu)
{
    Ipnet_netif *netif = (Ipnet_netif*)ipcomif;
    Ipnet_sig   *sig;

    /* IPNET does not call this directly, if that changes in the
       future then make sure ipnet_sys_if_changemtu() is called if
       the caller is the IPNET daemon */
    ip_assert(ipnet_is_stack_task() == IP_FALSE);

    IPNET_IF_LOCK(netif);

    sig = ipcom_malloc(sizeof(Ipnet_sig));

    if (sig == IP_NULL)
    {
        (void)ipcom_errno_set(IP_ERRNO_ENOMEM);
        goto done;
    }

    sig->sig_type      = IPNET_SIG_IF_CHANGEMTU;
    sig->d.netif.netif = ipcomif;
    sig->d.netif.mtu   = new_mtu;

    if (ipnet_pipe_basic_send_recv(sig) == IPCOM_ERR_INTERRUPT)
    {
        (void)ipcom_errno_set(IP_ERRNO_EINTR);
        goto done;
    }

    ipcom_free(sig);

 done:
    IPNET_IF_UNLOCK(netif);
}


/*
 *===========================================================================
 *                    ipcom_if_changename
 *===========================================================================
 * Description: Change the name of a network interface.
 * Parameters:  ipcomif - The interface for which the name has changed.
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_if_changename(Ipcom_netif *ipcomif, const char *new_name)
{
    Ipnet_netif *netif = (Ipnet_netif*)ipcomif;
    Ipnet_sig   *sig;

    /* IPNET does not call this directly, if that changes in the
       future then make sure ipnet_sys_if_changename() is called if
       the caller is the IPNET daemon */
    ip_assert(ipnet_is_stack_task() == IP_FALSE);

    IPNET_IF_LOCK(netif);

    sig = ipcom_malloc(sizeof(Ipnet_sig));

    if (sig == IP_NULL)
    {
        (void)ipcom_errno_set(IP_ERRNO_ENOMEM);
        goto done;
    }

    sig->sig_type      = IPNET_SIG_IF_CHANGENAME;
    sig->d.netif.netif = ipcomif;
    ipcom_strncpy(sig->d.netif.ifname, new_name, IP_IFNAMSIZ);

    if (ipnet_pipe_basic_send_recv(sig) == IPCOM_ERR_INTERRUPT)
    {
        (void)ipcom_errno_set(IP_ERRNO_EINTR);
        goto done;
    }

    ipcom_free(sig);

 done:
    IPNET_IF_UNLOCK(netif);
}


/*
 *===========================================================================
 *                    ipcom_if_detach
 *===========================================================================
 * Description: Detach a network interface with IPNET.
 * Parameters:  netif - The interface to detach.
 * Returns:     The interface index for this interface or negative errno.
 *
 */
IP_PUBLIC int
ipcom_if_detach(Ipcom_netif *ipcomif)
{
    Ipnet_netif *netif = (Ipnet_netif*)ipcomif;
    Ipnet_sig   *sig;
    int          ret;

    if (ipnet_is_stack_task())
        return ipnet_sys_if_detach(ipcomif);

    IPNET_IF_LOCK(netif);

    sig = ipcom_malloc(sizeof(Ipnet_sig));

    if (sig == IP_NULL)
    {
        (void)ipcom_errno_set(IP_ERRNO_ENOMEM);
        goto errout;
    }

    sig->sig_type      = IPNET_SIG_IF_DETACH;
    sig->d.netif.netif = ipcomif;

    if (ipnet_pipe_basic_send_recv(sig) == IPCOM_ERR_INTERRUPT)
    {
        (void)ipcom_errno_set(IP_ERRNO_EINTR);
        goto errout;
    }

    IPNET_IF_UNLOCK(netif);

    ret = sig->reply.return_code;
    ipcom_free(sig);

    if (ret < 0)
        (void)ipcom_errno_set(IP_ERRNO_ENXIO);

    return ret;

 errout:
    IPNET_IF_UNLOCK(netif);
    return -ipcom_errno;
}


/*
 *===========================================================================
 *                    ipcom_if_free
 *===========================================================================
 * Description: Frees the resources held by the network interface. The
 *                    interface must be detached for this call to succeed.
 * Parameters:  ipcomif - network interface to free
 * Returns:     0 = success, -1 = error
 *
 */
IP_PUBLIC int
ipcom_if_free(Ipcom_netif *ipcomif)
{
    Ipnet_sig *sig;
    int        ret;

    if (ipnet_is_stack_task())
        return ipnet_sys_if_free(ipcomif);

    sig = ipcom_malloc(sizeof(Ipnet_sig));

    if (sig == IP_NULL)
    {
        (void)ipcom_errno_set(IP_ERRNO_ENOMEM);
        goto errout;
    }

    ipnet_usr_sock_init_sig(sig, IPNET_SIG_IF_FREE);
    sig->d.netif.netif = ipcomif;

    ret = ipnet_usr_sock_basic_send_recv(sig);

    if (ret == IPCOM_ERR_INTERRUPT)
    {
        (void)ipcom_errno_set(IP_ERRNO_EINTR);
        goto errout;
    }

    ipcom_free(sig);

    return ret;

 errout:

    return IP_SOCKERR;
}


/*
 *===========================================================================
 *                    ipcom_if_malloc
 *===========================================================================
 * Description: Creates a new uninitialized network interface structure.
 * Parameters:  type - One of the IP_IFT_xxx types.
 * Returns:     The network interface structure of IP_NULL if out of memory.
 *
 */
IP_PUBLIC Ipcom_netif *
ipcom_if_malloc(Ip_u16 type)
{
    Ipcom_netif *ipcomif;
    Ipnet_sig   *sig;

    if (ipnet_is_stack_task())
        return ipnet_sys_if_malloc(type);

    sig = ipcom_malloc(sizeof(Ipnet_sig));

    if (sig == IP_NULL)
    {
        (void)ipcom_errno_set(IP_ERRNO_ENOMEM);
        goto errout;
    }

    sig->sig_type      = IPNET_SIG_IF_MALLOC;
    sig->d.netif.type  = type;
    sig->d.netif.netif = IP_NULL;

    if (ipnet_pipe_basic_send_recv(sig) == IPCOM_ERR_INTERRUPT)
    {
        (void)ipcom_errno_set(IP_ERRNO_EINTR);
        goto errout;
    }

    ipcomif = sig->d.netif.netif;
    ipcom_free(sig);

    return ipcomif;

 errout:

    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipcom_if_nametoindex
 *===========================================================================
 * Description: Maps the interface name to the index of the interface.
 * Parameters:  ifname - The name of the index.
 * Returns:     The index of the interface or 0 if no interface has the
 *              specified name.
 *
 */
IP_PUBLIC Ip_u32
ipcom_if_nametoindex(IP_CONST char *ifname)
{
    Ipnet_sig *sig;
    Ipnet_sig *reply_sig;
    Ip_u32    ifindex;

    sig = ipcom_malloc(sizeof(Ipnet_sig));

    if (sig == IP_NULL)
        goto errout;

    ipnet_usr_sock_init_sig(sig, IPNET_SIG_IF_NAMETOINDEX);

    ipcom_memcpy(sig->d.indexname.ifname,
                 ifname,
                 IP_MIN(ipcom_strlen(ifname) + 1, IP_IFNAMSIZ));
    sig->d.indexname.ifname[IP_IFNAMSIZ] = '\0';

    ipcom_pipe_send(ipnet->task.pipe, sig);
    if (ipnet_usr_sock_pipe_recv(sig->response_pipe,
                                 &reply_sig,
                                 sig,
                                 IP_NULL,
                                 0) == IPCOM_ERR_INTERRUPT)
        goto errout;

    ifindex = reply_sig->d.indexname.ifindex;
    ipcom_free(sig);

    if (ifindex == 0)
        (void)ipcom_errno_set(IP_ERRNO_ENXIO);

    return ifindex;

 errout:

    ipcom_free(sig);
    return 0;

}


/*
 *===========================================================================
 *                    ipcom_if_indextoname
 *===========================================================================
 * Description: Maps the interface index to the name of the interface.
 * Parameters:  ifindex - The index of the interface.
 *              ifname - A buffer to hold the name, must be at least
 *              IP_IFNAMSIZ characters long.
 * Returns:     The ifname pointer containing the interface name or IP_NULL
 *              if no interface has the specified index.
 *
 */
IP_PUBLIC char *
ipcom_if_indextoname(Ip_u32 ifindex, char *ifname)
{
    Ipnet_sig *sig;

    sig = ipcom_malloc(sizeof(Ipnet_sig));

    if (sig == IP_NULL)
    {
        (void)ipcom_errno_set(IP_ERRNO_ENOMEM);
        goto errout;
    }

    sig->sig_type            = IPNET_SIG_IF_INDEXTONAME;
    sig->d.indexname.ifindex = ifindex;
    sig->d.indexname.ifname[IP_IFNAMSIZ] = '\0';

    if (ipnet_pipe_basic_send_recv(sig) == IPCOM_ERR_INTERRUPT)
    {
        (void)ipcom_errno_set(IP_ERRNO_EINTR);
        goto errout;
    }

    if (sig->d.indexname.ifindex == 0)
    {
        (void)ipcom_errno_set(IP_ERRNO_ENXIO);
        ipcom_free(sig);
        goto errout;
    }

    ipcom_memcpy(ifname,
                 sig->d.indexname.ifname,
                 ipcom_strlen(sig->d.indexname.ifname) + 1);

    ipcom_free(sig);

    return ifname;

 errout:

    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipcom_if_nameindex
 *===========================================================================
 * Description: Returns an array of Ip_if_nameindex, one for each
 *              interface installed. The array ends with a entry with all
 *              fields set to 0.
 * Parameters:
 * Returns:     Interface array.
 *
 */
IP_PUBLIC struct Ip_if_nameindex *
ipcom_if_nameindex(void)
{
    struct Ip_if_nameindex *pif;
    Ipnet_sig              *sig;
    Ip_u32                  ifindex;
    Ip_u32                  num_attached;
    Ip_u32                  highest_index;
    Ip_u32                  if_added;
    char                   *ifname;

    sig = ipcom_malloc(sizeof(Ipnet_sig));

    if (sig == IP_NULL)
    {
        (void)ipcom_errno_set(IP_ERRNO_ENOMEM);
        goto errout;
    }

    sig->sig_type            = IPNET_SIG_IF_INDEXTONAME;
    sig->d.indexname.ifindex = 0;
    sig->d.indexname.ifname[IP_IFNAMSIZ] = '\0';

    if (ipnet_pipe_basic_send_recv(sig) == IPCOM_ERR_INTERRUPT)
    {
        (void)ipcom_errno_set(IP_ERRNO_EINTR);
        goto errout;
    }

    num_attached  = sig->d.indexname.num_attached;
    highest_index = sig->d.indexname.highest_assigned_ifindex;
    ipcom_free(sig);

    pif = ipcom_calloc((num_attached + 1), sizeof(*pif) + IP_IFNAMSIZ);
    if (pif == IP_NULL)
    {
        (void)ipcom_errno_set(IP_ERRNO_ENOMEM);
        return IP_NULL;
    }

    ifname = (char *) &pif[num_attached + 1];

    for (ifindex = 1, if_added = 0;
         ifindex <= highest_index && if_added < num_attached;
         ifindex++)
    {
        if (ipcom_if_indextoname(ifindex, ifname))
        {
            pif[if_added].if_name    = ifname;
            pif[if_added++].if_index = ifindex;
            ifname += IP_IFNAMSIZ;
        }
    }
    (void)ipcom_errno_set(0);

    return pif;

 errout:

    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipcom_if_freenameindex
 *===========================================================================
 * Description: Frees the array returned from ipcom_if_nameindex().
 * Parameters:  pif - The array to free.
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_if_freenameindex(struct Ip_if_nameindex *pif)
{
    if (pif != IP_NULL)
        ipcom_free(pif);
}


/*
 *===========================================================================
 *                    ipcom_if_nametonetif
 *===========================================================================
 * Description: Returns a pointer to the interface descriptor identified by
 *              its name.
 *              This function ensures that the interface is locked (reference counter
 *              has been increased).
 *              The user MUST self unlock the interface when finished with it.
 *
 * Parameters:  vr     - Actual VR to search for interface within.
 *              ifname - A buffer to hold the name, must be at least
 *              IP_IFNAMSIZ characters long.
 * Returns:     A pointer to the interface descriptor if found
 *              IP_NULL otherwise.
 *
 */
IP_PUBLIC Ipcom_netif*
ipcom_if_nametonetif(Ip_u16 vr, IP_CONST char *ifname)
{
    Ipcom_netif *ipcomif;
    Ipnet_sig   *sig;

#ifdef IP_PORT_LKM
    if (ipnet_is_stack_task())
        return ipnet_if_nametonetif(vr, ifname);
#endif /* IP_PORT_LKM */

    sig = ipcom_malloc(sizeof(Ipnet_sig));

    if (sig == IP_NULL)
    {
        (void)ipcom_errno_set(IP_ERRNO_ENOMEM);
        goto errout;
    }

    ipcom_memcpy(sig->d.netif.ifname,
                 ifname,
                 IP_MIN(ipcom_strlen(ifname) + 1, IP_IFNAMSIZ));

    sig->d.netif.ifname[IP_IFNAMSIZ] = '\0';
    sig->sig_type      = IPNET_SIG_IF_NAMETONETIF;
    sig->d.netif.vr    = vr;
    sig->d.netif.netif = IP_NULL;

    if (ipnet_pipe_basic_send_recv(sig) == IPCOM_ERR_INTERRUPT)
    {
        (void)ipcom_errno_set(IP_ERRNO_EINTR);
        goto errout;
    }

    ipcomif = sig->d.netif.netif;
    ipcom_free(sig);

    if (ipcomif == IP_NULL)
        (void)ipcom_errno_set(IP_ERRNO_ENXIO);

    return ipcomif;

 errout:

    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_suspend_stack
 *===========================================================================
 * Description: Mimics the semantic of the old IPNET_CODE_LOCK() macro.
 * Parameters:
 * Returns:     A handle that must be passed to ipnet_resume_stack()
 *                    when the stack should resume again. IP_NULL is
 *                    return if the stack refused or could not resume.
 *
 */
IP_PUBLIC void *
ipnet_suspend_stack()
{
    Ipnet_sig  sig;
    Ipnet_sig *psig;
    enum Ipnet_sig_type types[] = {
        IPNET_SIG_SUSPEND,
        IPNET_SIG_RESUME
    };

    ipnet_usr_sock_init_sig(&sig, IPNET_SIG_SUSPEND);

    ipcom_pipe_send(ipnet->task.pipe, &sig);
    ipnet_usr_sock_pipe_recv(sig.response_pipe,
                             &psig,
                             IP_NULL,
                             types,
                             2);

    if (psig->sig_type == IPNET_SIG_RESUME)
        return IP_NULL;
    return psig;
}


/*
 *===========================================================================
 *                    ipnet_resume_stack
 *===========================================================================
 * Description: Mimics the semantic of the old IPNET_CODE_UNLOCK() macro.
 * Parameters:  suspend_handle - handle return from ipnet_suspend_stack()
 * Returns:
 *
 */
IP_PUBLIC void
ipnet_resume_stack(void *suspend_handle)
{
    Ipnet_sig *sig = suspend_handle;

    sig->sig_type = IPNET_SIG_RESUME;
    ipcom_pipe_send(sig->response_pipe, sig);
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

