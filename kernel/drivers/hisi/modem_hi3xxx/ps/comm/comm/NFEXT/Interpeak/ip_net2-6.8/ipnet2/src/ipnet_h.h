/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_h.h,v $ $Revision: 1.229.18.1 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_h.h,v $
 *     $Author: ulf $ $Date: 2010-07-02 13:21:14 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPNET_H_H
#define IPNET_H_H

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

#include "ipnet_config.h"

#define IPNET_MAX_IF_EVENTCB 8


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipnet.h"
#include "ipnet_netif.h"
#ifdef IPCOM_USE_INET
#include "ipnet_ip4.h"
#endif
#ifdef IPCOM_USE_INET6
#include "ipnet_ip6.h"
#endif
#ifdef IPIPSEC2
#include <ipipsec.h>
#endif
#include "ipnet_nat_h.h"

#ifdef IPNET_USE_DIFFSERV
#include "ipnet_diffserv.h"
#include <ipcom_tree.h>
#endif
#include "ipnet_route.h"
#include "ipnet_signal.h"
#include "ipnet_timeout.h"

#include <ipcom_hash.h>
#include <ipcom_inet.h>
#include <ipcom_list.h>
#include <ipcom_mib2.h>
#include <ipcom_os.h>
#include <ipcom_pkt.h>
#include <ipcom_pqueue.h>
#include <ipcom_pqueue.h>
#include <ipcom_route.h>
#include <ipcom_slab.h>
#include <ipcom_sock.h>
#include <ipcom_spinlock.h>
#include <ipcom_sysvar.h>
#include <ipcom_time.h>
#include <ipcom_wait_queue.h>

#ifdef IPCOM_USE_JOB_QUEUE
#include <ipcom_job_queue.h>
#endif

#ifdef IPPPP
#include <ipppp_ipstack.h>
#endif

#include "ipnet_netlink_h.h"
#include "ipnet_rtnetlink_h.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_FD_TO_SOCKET_EX(fd, sock, must_be_open)          \
    if (IP_UNLIKELY(fd < IPNET_SOCKET_STARTFD) \
        || IP_UNLIKELY(fd >= ipnet_conf_max_sockets) \
        || IP_UNLIKELY(IP_NULL == (sock = ipnet->sockets[fd])) \
        || (must_be_open && IP_UNLIKELY(IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_CLOSED)))) \
    { \
        IPCOM_WV_MARKER_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING, \
                        1, 1, IPCOM_WV_NETDEVENT_WARNING, \
                        ipcom_socket, IPCOM_WV_NETD_NOSOCK, \
                        IPCOM_WV_IPNET_SOCK_MODULE, IPCOM_WV_NETD_SOCKET); \
        IPNET_STATS(sock_ebadf++); \
        return IPNET_ERRNO(EBADF); \
    }

#define IPNET_FD_TO_SOCKET(fd, sock)      IPNET_FD_TO_SOCKET_EX(fd, sock, IP_FALSE)
#define IPNET_FD_TO_OPEN_SOCKET(fd, sock) IPNET_FD_TO_SOCKET_EX(fd, sock, IP_TRUE)

#define IPNET_MAKE_ERRNO(err)      -(err)
#define IPNET_ERRNO(err)           IPNET_MAKE_ERRNO(IP_ERRNO_ ## err)

#define IPNET_DATA_SETU16(pkt,off,val16)   *(Ip_u16*)(&(pkt)->data[off]) = (Ip_u16)(val16)

#define IPNET_TIMEVAL_TO_MSEC(tv) ((Ip_u32) ((tv).tv_sec * 1000 + (tv).tv_usec / 1000))

/* Return a number N that where (size % align == 0) and N >= size
   i.e. the smallest size that has the specified alignment requirements */
#define IPNET_ALIGNED_SIZE(size, align) (((size) + (align) - 1) & ~((align) - 1))

#define IPNET_PKT_FLAG_URGENT IPCOM_PKT_FLAG_LINK2

/* Store the offset to the urgent data and mark the packet as urgent */
#define IPNET_MARK_PKT_AS_URGENT(pkt) \
    do { \
        ipcom_pkt_set_int(pkt, IPNET_PKT_INFO_URGENT_OFFSET, (pkt)->end - (pkt)->start - 1, Ip_u16); \
        IP_BIT_SET((pkt)->flags, IPNET_PKT_FLAG_URGENT); \
    } while (0)

#define IPNET_DATA_LOCK(lock)   ipcom_spinlock_lock(lock)
#define IPNET_DATA_UNLOCK(lock) ipcom_spinlock_unlock(lock)

#if defined(IP_PORT_VXWORKS) && defined(IPCOM_USE_SPINLOCK_ISR_ND)
#define IPNET_MIN_LOCK_CREATE(plock) ipcom_minlock_create (plock)
#define IPNET_MIN_LOCK_DELETE(plock) ipcom_minlock_delete (plock)
#define IPNET_MIN_LOCK(lock, val) \
    do { val = ipcom_minlock_lock(lock); } while ((0))
#define IPNET_MIN_UNLOCK(lock, val) \
    do { ipcom_minlock_unlock(lock, val); } while ((0))
#else
#define IPNET_MIN_LOCK_CREATE(plock) ipcom_spinlock_create (plock)
#define IPNET_MIN_LOCK_DELETE(plock) ipcom_spinlock_delete (plock)
#define IPNET_MIN_LOCK(lock, val) \
    do { IPNET_DATA_LOCK(lock); (void)val; } while ((0))
#define IPNET_MIN_UNLOCK(lock, val) \
    do { (void)val; IPNET_DATA_UNLOCK(lock); } while ((0))
#endif /* IP_PORT_VXWORKS */

#define ipnet_copy_to_user(s1,s2,n) ipcom_copy_to_user(s1,s2,n)
#define ipnet_copy_from_user(s1,s2,n) ipcom_copy_from_user(s1,s2,n)
#define ipnet_in_checksum_memcpy(s1,s2,n) ipcom_in_checksum_memcpy(s1,s2,n)

/*
 *===========================================================================
 *                        IPNET privilege
 *===========================================================================
 */

#define IPNET_PRIV_CHVR 1    /* Use has privilege to change the
                                current virtual router */

/*
 *===========================================================================
 *                         routing socket optional code
 *===========================================================================
 */
#ifdef IPNET_USE_ROUTESOCK
#define IPNET_ROUTESOCK(x) x
#else
#define IPNET_ROUTESOCK(x) IP_NOOP
#endif

/*
 *===========================================================================
 *                         netlink socket optional code
 *===========================================================================
 */
#ifdef IPNET_USE_NETLINK
#define IPNET_NETLINKSOCK(x) x
#else
#define IPNET_NETLINKSOCK(x) IP_NOOP
#endif

#define IPNET_NETLINK_DEF_BUF_SZ 8192
#define IPNET_NETLINK_EXTRA_PAD 50

/*
 *===========================================================================
 *                         DEBUG/ASSERT
 *===========================================================================
 */

#ifdef IPNET_DEBUG
/* #define IPNET_DEBUG_PRINTF -- Now handled per-file */
#define IPNET_DEBUG_LINE(x)    x
#else
#define IPNET_DEBUG_LINE(x)        IP_NOOP
#endif /* IPNET_DEBUG */


/*
 *===========================================================================
 *                         pkt id
 *===========================================================================
 */
#if defined(IPNET_DEBUG) || defined(IPNET_ASSERT)

#define IPNET_MEM_BEGIN_PAD    0xA110CA7E
#define IPNET_MEM_END_PAD      0x12DADE34

#endif /* IPNET_DEBUG */


/*
 *===========================================================================
 *                         syslog
 *===========================================================================
 */
#define IPCOM_SYSLOG_PRIORITY    IPNET_SYSLOG_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPNET


/*
 *===========================================================================
 *                   fast forward cache return values
 *===========================================================================
 */
/* No matching entry found in the forward cache, proceed with normal path */
#define IPNET_FORWARD_CACHE_MISS           0
/* The forward cache was used, no more processing needed */
#define IPNET_FORWARD_CACHE_TAKEN          1
/* Packet blocked by firewall */
#define IPNET_FORWARD_CACHE_BLOCKED_BY_FW  2
/* Packet blocked by NAT */
#define IPNET_FORWARD_CACHE_BLOCKED_BY_NAT  3
/* Packet failed by ROHC */
#define IPNET_FORWARD_CACHE_BLOCKED_BY_ROHC  4

/*
 *===========================================================================
 *                     IPNET event semaphore
 *===========================================================================
 */
/* Select stack daemon configuration */
#ifdef IPCOM_STACKD_POST_IMPL
/*
 *===========================================================================
 *                      IPCOM_STACKD_POST_IMPL
 *===========================================================================
 * Description: Notifies the external stack daemon that a stack event
 *                      has occured.
 * Parameters:  new_timeout - IP_TRUE if called because a new timeout
 *                      has occured.
 *              is_posted - IP_TRUE if this function returned IP_TRUE
 *                      the last call and ipcom_stackd_event() has not
 *                      since the last call.
 * Returns:     IP_TRUE if 'is_posted' should be IP_TRUE in the next
 *                      call if this function is called more times
 *                      from the same ipcom_stackd_event() run.
 *
 */
IP_EXTERN Ip_bool IPCOM_STACKD_POST_IMPL(Ip_bool new_timeout, Ip_bool is_posted);
#define IPCOM_STACKD_POST(new_timeout) \
    ipnet->task_posted = IPCOM_STACKD_POST_IMPL(new_timeout, ipnet->task_posted)

#else /* !IPCOM_STACKD_POST_IMPL */

#ifdef IPCOM_USE_JOB_QUEUE
#define IPCOM_STACKD_POST(new_timeout)
#else
 /* The built-in daemon process is used */
#define IPCOM_STACKD_POST(new_timeout) \
    if (!ipnet->task.wakeup_pending) \
    { \
        ipnet->task.wakeup_pending = IP_TRUE; \
        ipcom_pipe_try_send(ipnet->task.pipe, ipnet->task.wakeup_sig);  \
    }
#endif /* IPCOM_USE_JOB_QUEUE */
#endif /* IPCOM_STACKD_POST_IMPL */


/*
 * Adjust the checksum to reflect that the TTL had been decremented.
 *
 * Flip the bits on the checksum, decrement the high byte of the
 * checksum, fold in any carry, and then flip the bits back.  Rather
 * than convert the checksum to host byte order and then back to
 * network byte order, just convert the increment to network byte
 * order.  Note: in 1's complement arithmetic, subtracting by x is the
 * same as adding the 1's complement of x.  So, in 16 bit arithmetic,
 * rather than subtracting by (1<<8), we can add by (1<<8)^0xffff.
 * Since it's all constants, that should be evaluated by the compiler
 * at compile time.
 *
 * Doing the ^0xffff to initially flip the bits keeps the upper bits
 * from also being flipped. Using the ~ operation at the end doesn't
 * matter, because the upper bits get tossed when we assign it to the
 * 16 bit sum field, so let the compiler do whatever is fastest.
 */
#define IP_INCREMENTAL_CHECKSUM(ip_hdr)                         \
        do {                                                    \
            int     chksum;                                     \
            /* Calculate the incremental checksum update */     \
            --ip_hdr->ttl;                                      \
            /* increment checksum high byte */                  \
            chksum  = (int)(ip_hdr->sum ^ 0xFFFF) +             \
                      (int)ip_htons((1<<8) ^ 0xFFFF);           \
            /* Fold the carry bit into the checksum */          \
            ip_hdr->sum = ~(chksum + (chksum >> 16));           \
        } while((0))


/*
 *===========================================================================
 *                     WindView
 *===========================================================================
 */
#ifdef IPCOM_WV_INSTRUMENTATION
#define IPCOM_WV_MARKER_1(classId, eventLevel, eventOffset, eventTag, eventId, arg1, module, entity) \
    ipcom_wv_marker_1(classId, eventLevel, eventOffset, eventTag, eventId, arg1, module, entity)

#define IPCOM_WV_MARKER_2(classId, eventLevel, eventOffset, eventTag, eventId, arg1, arg2, module, entity) \
    ipcom_wv_marker_2(classId, eventLevel, eventOffset, eventTag, eventId, arg1, arg2, module, entity)

#define IPCOM_WV_EVENT_2(classId, eventLevel, eventOffset, eventTag, eventId, status, arg1, arg2, module, entity) \
    ipcom_wv_event_2(classId, eventLevel, eventOffset, eventTag, eventId, status, arg1, arg2, module, entity)
#else
#define IPCOM_WV_MARKER_1(classId, eventLevel, eventOffset, eventTag, eventId, arg1, module, entity) \
    do {} while (0)

#define IPCOM_WV_MARKER_2(classId, eventLevel, eventOffset, eventTag, eventId, arg1, arg2, module, entity) \
    do {} while (0)

#define IPCOM_WV_EVENT_2(classId, eventLevel, eventOffset, eventTag, eventId, status, arg1, arg2, module, entity) \
    do {} while (0)
#endif  /* #ifdef IPCOM_WV_INSTRUMENTATION */

/* IPNET packet information types */
#define IPNET_PKT_INFO_SIG                  64 /* Ipnet_sig */
#define IPNET_PKT_INFO_SOCK_SND_BUF         65 /* Ipnet_pkt_info_sock_snd_buf */
#define IPNET_PKT_INFO_URGENT_OFFSET        66 /* Ip_u16 */
#define IPNET_PKT_INFO_TUNNEL_IPSTART       67 /* int */
#define IPNET_PKT_INFO_NETEMU_SEQ_NUM       68 /* Ip_u32 */
#define IPNET_PKT_INFO_DPAF_PQ_INDEX        69 /* unsigned */
#define IPNET_PKT_INFO_TIMEOUT              70 /* Ip_u32 */
#define IPNET_PKT_INFO_ENCAP_UDP           (73 | IPCOM_PKT_INFO_COPY)  /* 8 bytes */
#define IPNET_PKT_INFO_ENCAP_IP6           (75 | IPCOM_PKT_INFO_COPY)  /* 16 bytes */
#define IPNET_PKT_INFO_ENCAP_IP            (76 | IPCOM_PKT_INFO_COPY)  /* 4 bytes */

#define IPNET_PKT_INFO_GRE_KEY_OUTPUT       (74 | IPCOM_PKT_INFO_COPY) /* Ip_u32 */
#define IPNET_PKT_INFO_SEND_SIG_VERIFIED    (64 | IPCOM_PKT_INFO_COPY) /* void */

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */


/* Forward declaration */
struct Ipnet_netif_struct;

/*
 *===========================================================================
 *                         Ipnet_pkt_udp
 *===========================================================================
 */
#include "ipcom_align16.h"
typedef IP_PACKED1 struct Ipnet_pkt_udp_struct
{
    Ip_u16   sport;     /* Source port. */
    Ip_u16   dport;     /* Destination port. */
    Ip_u16   ulen;      /* UDP length. */
    Ip_u16   sum;       /* UDP checksum. */
}
IP_PACKED2 Ipnet_pkt_udp;

#define IPNET_UDP_HDR_SIZE   (8)   /* UDP header is 8 octets long */


/*
 *===========================================================================
 *                         Ipnet_frame
 *===========================================================================
 */
typedef IP_PACKED1 union Ipnet_pkt_union_union
{
#ifdef IPCOM_USE_INET
    Ipnet_pkt_icmp   icmp;
#endif
    Ipnet_pkt_udp    udp;
}
IP_PACKED2 IP_Ipnet_pkt_union;
#include "ipcom_align16.h"


/* Packet information describing how much of the socket send buffer
   this packet occupies */
typedef struct Ipnet_pkt_info_sock_snd_buf_struct
{
    /* Pointer to the socket used to sent this packet. Direct pointer
       is OK since the socket will not be freed until the send buffer
       is ==0 */
    struct Ipnet_socket_struct *sock;
    /* Number of bytes this packet has reserved in the send buffer. Is
       always >0 */
    int                         payload_len;
}
Ipnet_pkt_info_sock_snd_buf;


/*
 *===========================================================================
 *                      Ipnet_sock_init_f
 *===========================================================================
 * Description: Called last in the socket creation and can proviced custom
 *              initialization of the socket.
 * Parameters:  sock - The affected socket.
 *              user_context - IP_TRUE if called in the context of a user process
 *                             IP_FALSE if called in the context of the kernel.
 * Returns:     0 = success, <0 = error code (-IPNET_ERRNO_xxx)
 *
 */
typedef int (*Ipnet_sock_init_f)(struct Ipnet_socket_struct *sock,
                                 Ip_bool user_context);

/*
 *===========================================================================
 *                      Ipnet_sock_destroy_f
 *===========================================================================
 * Description: Called when the socket is going to be deleted.
 *              Can contain domain specific cleanups.
 * Parameters:  sock - The affected socket.
 * Returns:
 *
 */
typedef void (*Ipnet_sock_destroy_f)(struct Ipnet_socket_struct *sock);


/*
 *===========================================================================
 *                      Ipnet_sock_close_f
 *===========================================================================
 * Description: Called when the socket is beeing closed. Should be defined
 *              if special handling, not done by shutdown(sock, IP_SHUT_RDWR),
 *              is required.
 * Parameters:  sock - The affected socket.
 * Returns:     0 = success, <0 = error code (-IPNET_ERRNO_xxx)
 *
 */
typedef int (*Ipnet_sock_close_f)(struct Ipnet_socket_struct *sock);


/*
 *===========================================================================
 *                      Ipnet_sock_lookup_f
 *===========================================================================
 * Description: Returns the first socket that matches the specified parameters.
 * Parameters:  vr - The virtual route table the socket must match.
 *              proto - the protocol to match.
 *              local_addr - the local address to match or IP_NULL for any address.
 *              local_port - the local port to match or 0 for any port.
 *              peer_addr - the peer address to match or IP_NULL for any address.
 *              peer_port - the peer port to match or 0 for any port.
 *              first - [in] the socket descriptor to from where the scan should start
 *                      [out] the socket descriptor to start the next search from.
 *                      set to IP_NULL to start from 0.
 * Returns:     The matching socket or IP_NULL if no socket (from [*first..sock_max]
 *              matched.
 *
 */
typedef struct Ipnet_socket_struct *
(*Ipnet_sock_lookup_f)(Ip_u16 vr, int proto,
                       IP_CONST void *local_addr, Ip_u16 local_port,
                       IP_CONST void *peer_addr, Ip_u16 peer_port);

/*
 *===========================================================================
 *                      Ipnet_sock_send_f
 *===========================================================================
 * Description: Send handler for the domain/type/protocol tuple
 * Parameters:  sock - The socket the packet beeing sent on.
 *              msg - Message structure with (unconnected sockets) destination
 *              address and (optional) auxillary data. Can be IP_NULL
 *              if the socket is connected and no auxillary data is
 *                      needed. 'msg_flags' contains the flags passed
 *                      in the 'flags' field to ipcom_sendmsg().
 *
 *              pkt - The
 * Returns:     0 = success, <0 = error code (-IPNET_ERRNO_xxx)
 *
 */
typedef int (*Ipnet_sock_send_f)(struct Ipnet_socket_struct *sock,
                                 IP_CONST struct Ip_msghdr *msg,
                                 Ipcom_pkt *pkt);


/*
 *===========================================================================
 *                      Ipnet_sock_bind_f
 *===========================================================================
 * Description: Binds a socket.
 * Parameters:  sock - The socket to bind.
 *              name - The socket name.
 *              namelen - The length of the name variable.
 *              user_context - IP_TRUE if called in the context of a user process
 *                             IP_FALSE if called in the context of the kernel.
 * Returns:     0 = success, <0 = error code (-IPNET_ERRNO_xxx)
 *
 */
typedef int (*Ipnet_sock_bind_f)(struct Ipnet_socket_struct *sock,
                                 IP_CONST struct Ip_sockaddr *name,
                                 Ip_socklen_t namelen,
                                 Ip_bool user_context);

/*
 *===========================================================================
 *                      Ipnet_sock_connect_f
 *===========================================================================
 * Description: Binds a socket.
 * Parameters:  sock - The socket to bind.
 *              name - The socket name.
 *              namelen - The length of the name variable.
 *              check_reuse - Set to IP_TRUE if the name should be checked
 *              for address reuse.
 * Returns:     0 = success, <0 = error code (-IPNET_ERRNO_xxx)
 *
 */
typedef int (*Ipnet_sock_connect_f)(struct Ipnet_socket_struct *sock,
                                    IP_CONST struct Ip_sockaddr *to,
                                    Ip_socklen_t tolen);

/*
 *===========================================================================
 *                      Ipnet_sock_shutdown_f
 *===========================================================================
 * Description: Shutdown read and/or write end of the socket, IPNET
 *              takes care of flagging the socket read/write end as shut,
 *              and notifies blocked sockets. Protocol specific things has
 *              to be done in this handler though (like sending FIN for TCP).
 * Parameters:  sock - The socket to shutdown.
 *              how - IP_SHUT_xxx constant.
 * Returns:
 *
 */
typedef void (*Ipnet_sock_shutdown_f)(struct Ipnet_socket_struct *sock, int how);


/*
 *===========================================================================
 *                      Ipnet_sock_accept_f
 *===========================================================================
 * Description: Accepts a socket.
 * Parameters:  sock - The socket to bind.
 *              name - The address of the accepted socket.
 *              namelen - The length of the name variable.
 * Returns:     >=0 = success (file descripto), <0 = error code (-IPNET_ERRNO_xxx)
 *
 */
typedef int (*Ipnet_sock_accept_f)(struct Ipnet_socket_struct *sock,
                                   struct Ip_sockaddr *name,
                                   Ip_socklen_t *namelen);


/*
 *===========================================================================
 *                      Ipnet_sock_listen_f
 *===========================================================================
 * Description: Puts a socket in listen mode.
 * Parameters:  sock - The socket to put in listen mode.
 *              backlog - The size of the socket backlog.
 * Returns:     0 = success, <0 = error code (-IPNET_ERRNO_xxx)
 *
 */
typedef int (*Ipnet_sock_listen_f)(struct Ipnet_socket_struct *sock,
                                   int backlog);


/*
 *===========================================================================
 *                      Ipnet_sock_setopt_f
 *===========================================================================
 * Description: Handles socket option.
 * Parameters:  sock - The socket to set an option on.
 *              level - The protocol level that should be affected by the
 *              option.
 *              optname - The option name.
 *              optval - Data to set for the option.
 *              optlen - The length of the buffer pointed to by optval.
 * Returns:     0 = success,
 *             -IP_ERRNO_EOPNOTSUPP = This handler does not implement this option,
 *                                    try other handlers
 *             <0 = error code (-IPNET_ERRNO_xxx), do not try other handlers
 *
 */
typedef int (*Ipnet_sock_setopt_f)(struct Ipnet_socket_struct *sock,
                                   int level,
                                   int optname,
                                   IP_CONST void *optval,
                                   Ip_socklen_t optlen);


/*
 *===========================================================================
 *                      Ipnet_sock_getopt_f
 *===========================================================================
 * Description: Handles socket option.
 * Parameters:  sock - The socket to get an option from.
 *              level - The protocol level that should be affected by the
 *              option.
 *              optname - The option name.
 *              optval - Data to set for the option.
 *              optlen - The length of 'optval' when calling,
 *                       number of bytes copied to 'optval' when returning.
 * Returns:     0 = success,
 *             -IP_ERRNO_EOPNOTSUPP = This handler does not implement this option,
 *                                    try other handlers
 *             <0 = error code (-IPNET_ERRNO_xxx)
 *
 */
typedef int (*Ipnet_sock_getopt_f)(struct Ipnet_socket_struct *sock,
                                   int level,
                                   int optname,
                                   void *optval,
                                   Ip_socklen_t *optlen);



/*
 *===========================================================================
 *                      Ipnet_sock_getname_f
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
typedef int (*Ipnet_sock_getname_f)(struct Ipnet_socket_struct *sock,
                                    struct Ip_sockaddr *name,
                                    Ip_socklen_t  *namelen,
                                    Ip_bool is_local_name);


/*
 *===========================================================================
 *                      Ipnet_sock_ioctl_f
 *===========================================================================
 * Description: Handles IO control on the socket.
 * Parameters:  sock - The socket to get an option from.
 *              request - The IO control request.
 *              argp - Request specific data.
 * Returns:     0 = success,
 *             <0 = error code (-IPNET_ERRNO_xxx)
 *             -IP_ERRNO_EOPNOTSUPP = This handler does not implement this option,
 *                                    try other handlers
 *
 */
typedef int (*Ipnet_sock_ioctl_f)(struct Ipnet_socket_struct *sock,
                                  unsigned long request, void *argp);


/*
 *===========================================================================
 *                      Ipnet_usr_sock_recv_f
 *===========================================================================
 * Description: Called in context of the receiving task. Proper
 *                      locking must be done by the this function to
 *                      safely extract the data from the socket.
 * Parameters:  sock - The socket to receive from.
 *              msg - 'msg_iov' contains the buffer(s) to where the
 *                      received data should be copied.
 *              flags - IP_MSG_xxx flags.
 * Returns:     0 = success, <0 = error code (-IPNET_ERRNO_xxx)
 *
 */
typedef int (*Ipnet_usr_sock_recv_f)(struct Ipnet_socket_struct *sock,
                                     struct Ip_msghdr *msg,
                                     int flags);

/*
 *===========================================================================
 *                      Ipnet_usr_sock_pkts_from_iov_f
 *===========================================================================
 * Description: Called in context of the sending task. Proper
 *                      locking must be done by the this function if
 *                      members of the socket are read or modified.
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
 *
 */
typedef int (*Ipnet_usr_sock_pkts_from_iov_f)(struct Ipnet_socket_struct *sock,
                                              IP_CONST struct Ip_msghdr *msg,
                                              int flags,
                                              int offset,
                                              int total_buf_len,
                                              Ipcom_pkt **ppkt);


/* Socket operations for a domain/type/proto tuple, functions can be left as
   IP_NULL if that operation is not supported */
typedef struct Ipnet_sock_ops_struct
{
    int                   domain;           /* The domain this operations is valid for */
    int                   type;             /* The socket type operations is valid for */
    int                   proto;            /* The protocol or -1 for any protocol */
    Ip_size_t             extra_sock_space; /* Extra space to be allocated at the end of the socket */
    Ip_size_t             hdr_space;        /* Minimum header space needed at by the domain/type/proto */
    Ip_bool               allow_async_send; /* IP_TRUE if this socket type allows asynchronous send */
    Ipnet_sock_accept_f   accept;
    Ipnet_sock_bind_f     bind;
    Ipnet_sock_close_f    close;
    Ipnet_sock_connect_f  connect;
    Ipnet_sock_destroy_f  destroy;
    Ipnet_sock_getname_f  getname;
    Ipnet_sock_getopt_f   getopt;
    Ipnet_sock_init_f     init;
    Ipnet_sock_ioctl_f    ioctl;
    Ipnet_sock_listen_f   listen;
    Ipnet_sock_lookup_f   lookup;
    Ipnet_sock_send_f     send;
    Ipnet_sock_setopt_f   setopt;
    Ipnet_sock_shutdown_f shutdown;

    Ipnet_usr_sock_recv_f          usr_recv;
    Ipnet_usr_sock_pkts_from_iov_f pkts_from_iov;
}
Ipnet_sock_ops;



/* Internal socket structure used by IPNET to implement
   IP_AF_INET and IP_AF_INET 6 */
typedef struct Ipnet_sock_inet_ops_struct
{
    Ipnet_sock_ops       sock;
    Ipnet_sock_bind_f    network_bind;
    Ipnet_sock_send_f    network_send;
}
Ipnet_sock_inet_ops;


/*
 *===========================================================================
 *                      Ipnet_dynhdrspc
 *===========================================================================
 * Description: Returns the number of octets that must be allocated to fit
 *              IP options and/or extension headers.
 * Parameters:  sock - The socket used to send the datagram.
 *              msg - The message data passed to ipcom_sendmsg().
 * Returns:     Number of extra octets that must be allocated.
 *
 */
typedef Ip_size_t (*Ipnet_dynhdrspc)(struct Ipnet_socket_struct *sock,
                                     IP_CONST struct Ip_msghdr *msg);


/*
 *===========================================================================
 *                    Ipnet_is_sock_ready_cb
 *===========================================================================
 * Description: Callback that will determine if the specified socket
 *                    is ready or not.
 * Parameters:  sock - a socket.
 *              lowat - minimum number of bytes that has to be
 *                    available in the send/receive buffer in order to
 *                    consider the socket to be writable/readable. 0
 *                    means that the socket default value should be used.
 * Returns:     IP_TRUE if the socket is ready.
 *
 */
typedef Ip_bool (*Ipnet_is_sock_ready_cb)(struct Ipnet_socket_struct *sock,
                                          int lowat);


/*
 *===========================================================================
 *                         Ipnet_socket
 *===========================================================================
 */
typedef struct Ipnet_socket_struct
{
    /* All socket members can be modified if the IPNET lock is taken in WRITE mode */
    Ipcom_socket   ipcom;                /* Generic socket parameters */
    Ipcom_atomic_t ref_count;            /* Socket reference count */
    Ip_u32         flags;                /* Socket flags. */

    struct Ipnet_socket_struct *prev;    /* Previous socket with the same match criteria */
    struct Ipnet_socket_struct *next;    /* Next socket with the same match criteria */

    Ip_u16   proto;                      /* Protocol */
    Ip_u16   vr_index;                   /* The virtual router to use when sending/receiving packets on this socket */

    Ip_u16   max_hdrspace;               /* Maximum needed header space for network (IP) and transport layer. */
    Ip_u16   chk_offset;                 /* Offset into the transport layer (TCP, UDP, e.t.c) where the
                                           16 bit internet checksum should be stored, used if the flag
                                           IPNET_SOCKET_FLAG_ADDCHECKSUM is set */

    Ip_u16   sport;                      /* Source port. */
    Ip_u16   dport;                      /* Destination port. */

#ifdef IPNET_USE_PER_SOCKET_VLAN_TAG
    Ip_u16   vlan_tag;                   /* Bit 0-2 is priority,
                                            bit 3 is priority tagging on/off
                                            bit 4-15 is the VLAN tag */
#endif

    Ip_u8    uni_hop_limit;              /* The hop limit for unicast packtes */
    Ip_u8    multi_hop_limit;            /* The hop limit for multicast packtes */
    Ip_u8    promisc_count;              /* Number of times promisc mode has been entered on this socket */
    Ip_u8    udp_encapsulation;          /* UDP encapsulation method */

    Ip_u32   bind_to_ifindex;         /* The interface this socket is
                                            bound to (with
                                            IP_SO_BINDTODEVICE). ==0
                                            -> not bound */
#ifdef IPNET_USE_RFC2890
    Ip_u32   gre_key;                 /* GRE key for this socket */
#endif
    int      keep_alive_probe_delay;  /* Number of seconds an TCP
                                         connection must be idle
                                         before the first keep-alive
                                         probe is sent */

    Ipcom_pkt_dtor    pkt_dtor;       /* Packet destruction that most
                                         be used for packets sent
                                         using this socket */

    Ipnet_sock_ops   *ops;            /* Socket operations */

    Ipnet_dynhdrspc   dynamic_header_space;

    int               send_max_bytes; /* Max number of bytes that can be stored in the send buffer */
    int               recv_max_bytes; /* Max number of bytes that can be stored in the recieve buffer */

    int               snd_route_cache_id; /* Value of ipnet->route_cache_id at last successful send */
    Ipcom_atomic_t    snd_bytes;      /* Bytes in send buffer */
    Ipcom_atomic_t    waiting_writers; /* Number of waiting writers on this socket */

    Ipcom_pkt        *snd_head;      /* Output queue, only used if IP_MSG_MORE flag is used */
    struct Ip_sockaddr *snd_to;      /* Cached to address specified in a sendmsg() with IP_MSG_MORE */

    /* Variables used by protocols that is datagram based, the TCP
       receive queue is found in 'tcb'  */
    Ipcom_spinlock    rcv_lock;      /* Must be locked when modifying
                                        rcv_head, rcv_tail and
                                        ipcom.rcv_bytes  */

    Ipcom_pkt        *rcv_head;      /* Input packet queue head, pkt->start is the offset
                                         to the transport layer header. */
    Ipcom_pkt        *rcv_tail;      /* Input packet queue tail. */

    /* Synchronization objects used when receiving data */
    struct Ip_timeval *rcv_timeout;   /* Time that a blocking
                                         ipcom_recv can bloc, IP_NULL
                                         means infinite time */

    /* List of struct Ipnet_sig_poll instances added by task(s) that
       are waiting for the socket to become readable, writable and/or
       exceptional */
    Ipcom_list        poll_list_head;

#ifdef IPTCP
    struct Iptcp_tcb_struct *tcb;       /* Transmission control block (used by TCP) */
    void (*notify_cb)(struct Ipnet_socket_struct *sock, int event); /* See Iptcp_notifycb in iptcp.h */
#endif
#ifdef IPSCTP
    struct Ipsctp_instance_struct *sctp_inst;  /* Transmission control block (used by SCTP) */
#endif


    struct Ipnet_socket_struct *addr_prev;    /* Previous socket bound to the same address */
    struct Ipnet_socket_struct *addr_next;    /* Next socket bound to the same address */
    void                       *addr_entry;   /* Pointer to the address entry for the bind address */

#if defined(IP_PORT_VXWORKS) && defined(IPCOM_USE_SOCK_COMPAT)
    void                        *select_ctx;
#ifdef IPNET_SO_WAKEUP_HOOK
    void                        *windnet_so;
#endif
#endif
    /* Buffer for sending asynchronous socket free signal */
    char              async_free[ip_offsetof(Ipnet_sig, d.close) + sizeof(Ip_fd)];

    void             *sock_end;    /* The current end of the socket, used to set up layer specific space */


#ifdef IPCOM_USE_INET
    Ipnet_ip4_socket *ip4;
#endif
#ifdef IPCOM_USE_INET6
    Ipnet_ip6_socket *ip6;
#endif
#ifdef IPNET_USE_NETLINKSOCK
    Ipnet_netlink_socket *nl;
#endif

    /* !NOTE! do not put any struct members below this point */
}
Ipnet_socket;

/* socket flags. */
/* Flags controlled by socket options */
#define IPNET_SOCKET_FLAG_REUSEADDR           (1u << 0)  /* Resuse of address is ok */
#define IPNET_SOCKET_FLAG_REUSEPORT           (1u << 1)  /* Resuse of port and address is ok */
#define IPNET_SOCKET_FLAG_DONTROUTE           (1u << 2)  /* Bypass routing for outgoing messages */
#define IPNET_SOCKET_FLAG_LOOP_MULTICAST      (1u << 3)  /* Loopback multicast packets sent from this host */
#define IPNET_SOCKET_FLAG_TCP_NODELAY         (1u << 4)  /* Nagle algorithm is OFF when this flag is set */
#define IPNET_SOCKET_FLAG_RECV_IF             (1u << 5)  /* Returns the recv if for datagram proto */
#define IPNET_SOCKET_FLAG_RECVPKTINFO         (1u << 6)  /* Returns extra information about received packets */
#define IPNET_SOCKET_FLAG_KEEPALIVE           (1u << 7)  /* Probe idle connection */
#define IPNET_SOCKET_FLAG_NONBLOCKING         (1u << 8)  /* Socket is in non-blocking mode */
#define IPNET_SOCKET_FLAG_ADDCHECKSUM         (1u << 9)  /* Pseudo header checksum should be added to outgoing packets */
#define IPNET_SOCKET_FLAG_OOBINLINE           (1u << 10) /* Out-of-band data should be mixed with normal data */
#define IPNET_SOCKET_FLAG_SENDER_LINK_ADDR    (1u << 11) /* Get link address of sender */

/* Socket long term state */
#define IPNET_SOCKET_FLAG_REACHABLE           (1u << 12) /* Set if the socket can match incoming packets */
#define IPNET_SOCKET_FLAG_BOUND               (1u << 13) /* Socket is bound (explicit or implicit) */
#define IPNET_SOCKET_FLAG_CONNECTING          (1u << 14) /* Asynchronous connect in progress */
#define IPNET_SOCKET_FLAG_CONNECTED           (1u << 15) /* Socket is connected */

#define IPNET_SOCKET_FLAG_READ_CLOSED         (1u << 16) /* The read end of the socket is closed */
#define IPNET_SOCKET_FLAG_WRITE_CLOSED        (1u << 17) /* The write end of the socket is closed */
#define IPNET_SOCKET_FLAG_PEER_WRITE_CLOSED   (1u << 18) /* The peer has closed its write end */
#define IPNET_SOCKET_FLAG_CLOSED              (1u << 19) /* The socket has been closed */

#define IPNET_SOCKET_FLAG_MROUTE_CONTROL      (1u << 23) /* This socket is used for multicast routing control */
#define IPNET_SOCKET_FLAG_DRAINABLE           (1u << 24) /* Packets queued on this socket might be freed
                                                            in order to satisfy a packet allocation call */
#define IPNET_SOCKET_FLAG_GRE_KEY             (1u << 25) /* Socket has a RFC 2890 key set */

/* Socket short term state */
#define IPNET_SOCKET_FLAG_RX_CACHED           (1u << 26) /* This socket is placed in RX cache */
#define IPNET_SOCKET_FLAG_PKTAVAIL_CB_PENDING (1u << 27) /* This socket want an IP_SOEVENT_CB_PKTAVAIL event when
                                                            the next packet is freed */

/* Misc. */
#define IPNET_SOCKET_FLAG_DEBUG               (1u << 29) /* Debug is enabled */
#define IPNET_SOCKET_FLAG_HAS_STICKY_OPT      (1u << 30) /* At least 1 sticky option is set */

/* socket start index (0,1,2 are reserved) */
#define IPNET_SOCKET_STARTFD    3


/*
 *===========================================================================
 *                         Ipnet_socket_lookup_key
 *===========================================================================
 */
typedef struct Ipnet_socket_lookup_key_struct
{
    Ip_u32  vr;             /* The virtual router the socket must be assigned to */
    Ip_u16  domain;         /* The domain this socket must be part of */
    Ip_u16  proto;          /* The protocol the socket must use */
    Ip_u16  local_port;     /* The local port the socket must bound to */
    Ip_u16  peer_port;      /* The peer port the socket must connected to */
    union Ip_in_addr_union local_addr; /* The local address */
    union Ip_in_addr_union peer_addr;  /* The peer address */
}
Ipnet_socket_lookup_key;


/*
 *===========================================================================
 *                         Ipnet_stats
 *===========================================================================
 * IPNET debug statistics useful for debugging the stack if things go wrong.
 */
#if defined(IPNET_DEBUG) || defined(IPNET_STATISTICS)
typedef struct Ipnet_stats_struct
{
    int    pkt_num_tcp_out_of_order_seg;
    int    pkt_num_ip4_reassembly_list;
    int    pkt_num_ip6_reassembly_list;

    int    pkt_input;
    int    pkt_input_err;
    int    pkt_input_opened_err;
    int    pkt_input_bound_to_other_if;
    int    pkt_input_queue_full;
    int    pkt_drain_ip4_reasm;
    int    pkt_drain_ip6_reasm;
    int    pkt_clone_nomem;
    int    pkt_frag_nomem;
    int    pkt_reasm_nomem;
    int    pkt_reasm_too_big;
    int    pkt_queue_enobufs;
    int    pkt_queue_emsgsize;

    int    pkt_output;
    int    pkt_output_drv;
    int    pkt_output_ewouldblock;   /* errors other than non-blocking. */
    int    pkt_output_err;   /* errors other than non-blocking. */
    int    pkt_output_queue;
    int    pkt_output_discard;
    int    pkt_output_delayed;
    int    pkt_output_sleep;
    int    pkt_output_requeue;
    int    pkt_output_fast_drv_ready;

    int    pkt_netif_output_done;
    int    pkt_netif_output_done_pkt;
    int    pkt_netif_output_done_discard;
    int    pkt_netif_output_delayed_queue;
    int    pkt_netif_output_delayed_wake;
    int    pkt_netif_output_user_wake;
    int    pkt_inputd_output;
    int    pkt_inputd_output_err;
    int    pkt_inputd_output_ewouldblock;
    int    pkt_inputd_output_err_nonetif;

    int    eth_input;
    int    eth_input_down;
    int    eth_input_arp;
    int    eth_input_arp_unknown;
    int    eth_input_arp_bad;
    int    eth_input_arp_reperr;
    int    eth_input_arp_reqother;
    int    eth_input_ip4;
    int    eth_input_ip6;
    int    eth_input_other;
    int    eth_output_ip4;
    int    eth_output_ip4_err;
    int    eth_output_ip4_drverr;
    int    eth_output_ip4_multierr;
    int    eth_output_ip6;
    int    eth_output_ip6_err;
    int    eth_output_ip6_drverr;
    int    eth_output_arp_req;
    int    eth_output_arp_req_nomem;
    int    eth_output_arp_rep;

#ifdef IPNET_USE_VLAN
    int    vlan_input_no_match;
    int    vlan_input_down;
#endif /* IPNET_USE_VLAN */

    int    ip4_input;
    int    ip4_input_err;
    int    ip4_input_hdr_trunc;
    int    ip4_input_hdr_toolong;
    int    ip4_input_inval_len;
    int    ip4_input_inval_version;
    int    ip4_input_not_to_me;
    int    ip4_input_iph_badchksum;
    int    ip4_input_reasm_enobufs;
    int    ip4_input_time_exceeded;
    int    ip4_input_need_frag;

    int    ip4_output;
    int    ip4_output_blackhole;
    int    ip4_output_hdr_space;
    int    ip4_output_noroute;
    int    ip4_output_enobufs;
    int    ip4_output_link_err;

    int    icmp4_input;
    int    icmp4_input_echo_request;
    int    icmp4_input_dst_unreach;
    int    icmp4_input_dst_unreach_match;
    int    icmp4_input_dst_unreach_nomatch;
    int    icmp4_input_dst_unreach_badcode;
    int    icmp4_input_hdrsize;
    int    icmp4_input_badchksum;
    int    icmp4_input_hdrsize2;
    int    icmp4_send;
    int    icmp4_send_nomem;

    int    ip6_input;
    int    ip6_input_err;
    int    ip6_input_hdr_trunc;
    int    ip6_input_inval_len;
    int    ip6_input_inval_exthdr;
    int    ip6_input_not_to_me;
    int    ip6_input_too_big;
    int    ip6_input_time_exceeded;
    int    ip6_input_too_short;
    int    ip6_input_tcp_multicast;
    int    ip6_input_prot_err;
    int    ip6_input_hdr_no_next;
    int    ip6_input_raw;

    int    ip6_output;
    int    ip6_output_err;
    int    ip6_output_blackhole;
    int    ip6_output_hdr_space;
    int    ip6_output_noroute;
    int    ip6_output_nomem;
    int    ip6_output_enobufs;
    int    ip6_output_err_probe;
    int    ip6_output_link_err;

    int    icmp6_input;
    int    icmp6_input_err;
    int    icmp6_input_badchksum;
    int    icmp6_input_nomem;

    int    icmp6_send_nomem;

    int    udp4_input;
    int    udp4_input_ok;
    int    udp4_input_err;

    int    tcp4_input;
    int    tcp4_input_ok;
    int    tcp4_input_err;

    int    raw4_input;
    int    raw6_input;
    int    raw_input_badchksum;

    int    udp_input;
    int    udp_input_hdr_trunc;
    int    udp_input_badchksum;
    int    udp_input_sock_multicast;
    int    udp_input_sock_match;
    int    udp_input_sock_nomatch;
    int    udp_input_queue_pkt_ok;
    int    udp_input_queue_pkt_err;
    int    udp_input_multicast;
    int    udp_input_econnrefused;
    int    udp_output;
    int    udp_output_hdr_trunc;
    int    udp_output_eaddrinuse;
    int    udp_output_eafnosupport;

    int    sock_ebadf;

    int    sock_sendmsg;

    int    sock_recvmsg;
    int    sock_recvmsg_tcp_err;
    int    sock_recvmsg_err;

    int    usr_sock_recvmsg;
    int    usr_sock_recvmsg_ewouldblock;

    int    usr_iov_emsgsize;
    int    usr_iov_nomem;

    int    neigh_pending;
    int    neigh_pending_drop;
}
Ipnet_stats;

#define IPNET_STATS(xvar)      ipnet->stats.xvar
#define IPNET_STAT_LINE(xline) xline

#else
#define IPNET_STATS(xvar)      IP_NOOP
#define IPNET_STAT_LINE(xline) IP_NOOP
#endif

#ifdef IPNET_USE_DIFFSERV
typedef struct Ipnet_diffserv_data_struct
{
    /* Array of diffserv handlers */
    Ipnet_diffserv_handlers *handlers[IPNET_DIFFSERV_MAX_HANDLERS];
    /* Database of meter/marker constructors */
    Ipcom_hash              *meter_marker_ctor_db;
#ifdef IPNET_DIFFSERV_CLASSIFIER_MODE_BA
    /* Defines which marker/meter to use for each traffic class */
    Ip_u32                   mapping_active[8];
    Ip_u8                    ds_to_handler_map[256];
#else
    /* The classifier used by diffserv */
    Ipnet_classifier        *classifier;
#endif
}
Ipnet_diffserv_data;
#endif /* IPNET_USE_DIFFSERV */


/*
 *===========================================================================
 *                        Ipnet_inq
 *===========================================================================
 *
 */
typedef struct Ipnet_input_queue_struct
{
    /* Stack input queue. */
    Ipcom_spinlock lock;        /* Protects head, tail and count variables */
    Ipcom_pkt     *head;        /* Head of the input queue (first packet to be processed) */
    Ipcom_pkt     *tail;        /* Tail of the input queue (last packet to be processed) */
    int            count;       /* Number of packets currently in the input queue */
#ifdef IPCOM_USE_JOB_QUEUE
    Ipcom_singleton_job_t job;    /* Job for processing this input queue */
#endif
}
Ipnet_inq;


/*
 *===========================================================================
 *                        Ipnet_task
 *===========================================================================
 * Information that is per IPNET daemon task.
 */
typedef struct Ipnet_task_struct
{
    Ipcom_pipe      *pipe;           /* Pipe the net-task polls for
                                        stack requests */
    Ipcom_pipe      *rpipe;          /* Pipe the net-task uses to send
                                        responses. Used if the
                                        sig->response_pipe is IP_NULL */
    Ip_pid_t         pid;            /* PID of the ipnet daemon if
                                        IPCOM_USE_JOB_QUEUE is
                                        undefined. PID of the job
                                        daemon if IPCOM_USE_JOB_QUEUE
                                        is defined */
    Ipnet_inq        inq;            /* Packet inqueue served by this
                                        daemon */
#ifndef IPCOM_USE_JOB_QUEUE
    volatile Ip_bool wakeup_pending; /* IP_TRUE if at least one wakeup
                                        signal is pending */
    Ipnet_sig       *wakeup_sig;
    Ip_ptrdiff_t     wakeup_sig_buf[ip_offsetof(Ipnet_sig, d) / sizeof(Ip_ptrdiff_t)];
#endif
}
Ipnet_task;


/*
 *===========================================================================
 *                        Ipnet_slabs
 *===========================================================================
 * All the slab caches used by IPNET
 */
typedef struct Ipnet_slabs_struct
{
#ifdef IPNET_USE_PACKET_POOL
    Ipcom_memory_cache *pkt_hdr;
    Ipcom_memory_cache *pkt_buf_1500;
    Ipcom_memory_cache *pkt_buf_3000;
    Ipcom_memory_cache *pkt_buf_10000;
    Ipcom_memory_cache *pkt_buf_65535;
#endif
    Ipcom_memory_cache *socket;
    Ipcom_memory_cache *bound_socket;
    Ipcom_memory_cache *timer;
    Ipcom_memory_cache *kioevent_softirq;
    Ipcom_memory_cache *poll;
}
Ipnet_slabs;

/* #define IPNET_USE_EVENTLOG */
#ifdef IPNET_USE_EVENTLOG
struct Ipnet_events
{
    Ipcom_eventlog log;
    Ipcom_event    sock;
    Ipcom_event    netif;
    Ipcom_event    drv;
    Ipcom_event    drv_wouldblock;
};
#endif

/*
 *===========================================================================
 *                       Ipnet_pkt_cache
 *===========================================================================
 */
#ifdef IPNET_USE_PER_CPU_PKT_CACHE

/* The packet cache is per-CPU so the actual number of packets that
   can be cached must be multiplied with total number of configured
   CPU:s. That is why the number is lower on SMP builds */
#ifdef IPCOM_USE_SMP
#define IPNET_PKT_CACHE_MAX_COUNT 10
#else
#define IPNET_PKT_CACHE_MAX_COUNT 40
#endif

typedef struct Ipnet_pkt_cache_struct
{
#ifdef IPNET_USE_PER_CPU_PKT_CACHE_SPINLOCK
    Ipcom_spinlock   lock;       /* Protects all members of this structure */
#endif
    Ipcom_pkt       *hdrs;       /* Header only packets, "data" is IP_NULL */
    unsigned         hdrs_count; /* Number of headers */
    Ipcom_pkt       *pkts;       /* Header + data packets */
    unsigned         pkts_count; /* Number of packets */
}
Ipnet_pkt_cache;
#endif /* IPNET_USE_PER_CPU_PKT_CACHE */


/* per-cpu packet cache locking and unlocking macros */
#ifdef IPNET_USE_PER_CPU_PKT_CACHE_SPINLOCK
#define IPNET_PKT_CACHE_KEY_DECLARE(k)
#define IPNET_PKT_CACHE_LOCK(c, cpu, key)	\
    do  { \
        c = ipnet->pkt_cache[(cpu)];	\
        ipcom_spinlock_lock((c)->lock); \
    } while ((0))
#define IPNET_PKT_CACHE_UNLOCK(c, key) ipcom_spinlock_unlock((c)->lock)
#else
#define IPNET_PKT_CACHE_KEY_DECLARE(k) Ip_preempt_key k;
/*
 * Note that this is a macro, not a function; if cpu is 'ipcom_get_cpuid()',
 * it is evaluated _after_ the macro expansion, which is what we want!
 */
#define IPNET_PKT_CACHE_LOCK(c, cpu, key)	\
    do  { \
        key = ipcom_preempt_disable(); \
        c = ipnet->pkt_cache[(cpu)];   \
    } while ((0))
#define IPNET_PKT_CACHE_UNLOCK(c, key) ipcom_preempt_enable(key)
#endif /* IPNET_USE_PER_CPU_PKT_CACHE_SPINLOCK */

#ifdef IPCOM_FORWARDER_AMP
#define IPNET_ROUTE_CACHE_ID_IN_SHARED_MEM
#endif

#ifdef IPNET_ROUTE_CACHE_ID_IN_SHARED_MEM
#define route_cache_id route_cache_id_ptr[0]
#endif


/*
 *===========================================================================
 *                         Ipnet_data
 *===========================================================================
 */
#ifdef IPNET_USE_NETLINK
struct Ipnet_rtnetlink_table;
#endif

typedef struct Ipnet_data_struct
{
    Ipcom_memory_pool  *memory_pool;
    Ipnet_slabs         slabs;

    /* Timeout */
    Ip_u32          sec_now;            /* The current time with seconds as resolution */
    Ip_u32          msec_now;           /* The current time with milliseconds as resolution */
    Ip_u32          msec_last;          /* Used to update the sec_now correctly */
    Ipcom_pqueue   *timeouts;           /* Priority queue of timeouts */
    Ip_bool         update_internal_timer; /* IP_TRUE if the internal timer should be updated
                                           when scheduleing the next timeout */
#ifdef IPCOM_USE_JOB_QUEUE
    Ipcom_singleton_job_t timeout_job;    /* Handle for timeout processing job */
#endif
#ifdef IPNET_DEBUG
    int             tmo_map_size;
    Ipnet_cmd_tmo_handler_to_str *tmo_map;
#endif

    /* Soft interrupts, softirq_xxx can only be modified with interrupts disabled */
    Ipcom_spinlock  softirq_lock;       /* Protects all 'softirq_xxx' variables */
    Ip_bool         softirq_pending;    /* Set to IP_TRUE if at least 1 soft interrupt is pending */
    Ipnet_softirq  *softirq_head;       /* Points to the first soft interrupt to run */
    Ipnet_softirq  *softirq_tail;       /* Points to the last soft interrupt to run */
#ifdef IPCOM_USE_JOB_QUEUE
    Ipcom_singleton_job_t softirq_job;    /* Handle for softirq processing job */
#endif

    Ipnet_softirq   drain_softirq;      /* Static buffer to signal drain soft interrupts */
    Ipnet_softirq   pktavail_softirq;   /* Static buffer to signal packet available soft interrupts */
    Ipnet_softirq   pktnew_softirq;     /* Static buffer to that more packets must be added to the pool */

#ifdef IPNET_USE_PACKET_POOL
    /* Packet pool */
    Ipcom_spinlock   pkt_pool_lock;     /* Protects all 'pkt_pool_xxx' variable + 'pkt_hdr_list' */

    Ipcom_wait_queue pkt_pool_wq;       /* Wait queue of processes that are waiting for
                                           more packages to be avaiable in the packet pool */
    Ipnet_timeout   *pkt_pool_tmo;       /* Protected by the timeout lock: next wakeup for
                                           processes waiting for more packets to become available */
#ifdef IPNET_USE_PER_CPU_PKT_CACHE
    Ipnet_pkt_cache **pkt_cache;
#endif
#ifdef IPNET_DEBUG
    Ipcom_pkt       *allocated_pkt_list; /* List of currently allocated packets */
#endif
#endif /* IPNET_USE_PACKET_POOL */

    /* Socket stuff. */
    Ip_size_t      largest_socket_size;
    Ipcom_spinlock sockets_lock;        /* Lock that protects the 'sockets' member */
    Ipnet_socket **sockets;             /* Array of socket pointers, ipnet_conf_max_sockets entries */
    Ipcom_hash    *bound_sockets;       /* All source ports that sockets are bound to */
    Ipcom_hash    *reachable_sockets;   /* All sockets that could match an incoming packet */
    Ipcom_hash    *socket_ops;          /* All registered domain/type/proto tuples */

    Ip_pid_t         effective_pid;  /* The PID of the process that
                                        sent the signal that we are
                                        currently processing */

#if IPCOM_VR_MAX > 1
    Ip_u16           effective_vr;
#endif

    Ip_u16        next_port;
    Ip_fd         last_fd;              /* Last returned fd */

    Ip_u32        last_policy;          /* Last assigned policy identifier */

#ifdef IPCOM_USE_PACKET
    int           num_packet_sock;      /* Number of packet sockets currently active */
#endif

    /* Router variables */
#ifdef IPNET_ROUTE_CACHE_ID_IN_SHARED_MEM
    volatile int *route_cache_id_ptr;   /* Pointer to the current route cache
					   ID, used to determine if a cached
					   router entry can still be valid */
#else
    volatile int  route_cache_id;       /* The current route cache ID, used to determine if a
                                           cached router entry can still be valid */
#endif
    Ipcom_hash   *rtabs;                /* Table where all route table head are stored */
    Ipcom_hash   *policy_routing_rules; /* Table with all policy routing lists (one for each domain/virtual router tuple) */
    unsigned      cloned_route_count;   /* Total number of cloned routes */

    Ipcom_hash   *queue_db;             /* Collection of queue constructors */

    Ipnet_inq     inq;                  /* Queue for RX packets + task that inputs them to the stack */
    Ipnet_task    task;

    /* Link/interface layer. */
    Ipcom_hash   *netifs;       /* Hash table that contains all attached interfaces */
#ifdef IPNET_USE_6OVER4
    Ipnet_netif **netif_6over4; /* Array of active 6over4 interfaces */
    int           netif_6over4_count;
#endif
    Ipcom_if_eventcb  if_eventcb[IPNET_MAX_IF_EVENTCB];
#ifdef IPNET_USE_FAST_IFNAME_LOOKUP
    Ipcom_hash   *ifname_tab;     /* Interface name hash table */
#endif /* IPNET_USE_FAST_IFNAME_LOOKUP */
#ifdef IPNET_USE_ROUTE_TABLE_NAMES
    Ipcom_hash   *rtab_names;
#endif /* IPNET_USE_ROUTE_TABLE_NAMES */
#ifdef IPNET_USE_ROUTE_COOKIES
    Ipcom_hash   *rtab_cookies;
#endif /* IPNET_USE_ROUTE_COOKIES */

    Ip_u32        next_ifindex; /* The next ifindex to use */

    /* Temporary storage to create log messages, anything stored here
       might be overwritten by any function. */
    char          log_buf[256];

    /* Statistics */
#if defined(IPNET_DEBUG) || defined(IPNET_STATISTICS)
    Ipnet_stats   stats;
#endif

#if defined(IP_X_SIOCSDEBUG) || defined(IP_X_SIOCGDEBUG)
    /* Value that can be manipulated via the
       IP_X_SIOCSDEBUG/IP_X_SIOCGDEBUG ioctl(). This value is not used
       anywhere in the stack. The indented usage is to create
       breakpoints that will only trigger if 'debug' has a specific
       value. */
    int debug;
#endif

    /* IP version 4 */
#ifdef IPCOM_USE_INET
    Ipnet_ip4_data      ip4;
    /* Multicast routing */
#ifdef IPNET_USE_MCAST_ROUTING
    Ipcom_hash         *ip4_mcast;
#endif
#endif /* IPCOM_USE_INET */

    /* IP version 6 */
#ifdef IPCOM_USE_INET6
    Ipnet_ip6_data      ip6;
#ifdef IPNET_USE_MCAST_ROUTING
    Ipcom_hash         *ip6_mcast;
#endif
#endif

    /* PPP and PPPoE support */
#ifdef IPPPP
    Ipppp_ipstack_data   ppp;
#endif

    /* Differentiated Services */
#ifdef IPNET_USE_DIFFSERV
    Ipnet_diffserv_data diffserv;
#endif /* IPNET_USE_DIFFSERV */

#ifdef IPNET_USE_VRRP
    Ipcom_hash *vrrp_addrs;
#endif

    /* NAT */
#ifdef IPNET_USE_NAT
    Ipnet_nat_data      nat;
#endif

    /* Route/netlink sockets. */
#ifdef IPNET_USE_ROUTESOCK
    int          num_route_sock;   /* Number of routing sockets. */
#endif
#ifdef IPNET_USE_NETLINKSOCK
    int                         num_netlink_sock;   /* Number of netlink sockets. */
    struct Ip_rtnl_link_ops         *rtnetlink_ops;
#endif
#ifdef IPNET_USE_NETLINK
    struct Ipnet_rtnetlink_table    **rtnetlink_links[IP_AF_MAX];
#endif

    /* MIB-2 */
#ifdef IPCOM_USE_MIB2
    Ipcom_mib2_stats mib2;
#endif
}
Ipnet_data;


/* Ipnet_data flags: */
#define IPNET_FLAG_IPV4_FORWARD_PKT     0x02    /* Set if IPv4 packets should be forward */
#define IPNET_FLAG_IPV6_FORWARD_PKT     0x04    /* Set if IPv6 packets should be forward */
#define IPNET_FLAG_PKTAVAIL_CB_PENDING  0x10    /* A non-blocking socket call, on a socket with a */
                                                /* callback handler, failed due to no packets in
                                                   the packet pool */
#define IPNET_FLAG_IPV6_RFC4941_INIT    0x20    /* IPv6 privacy extension has been initialized */



/*
 *===========================================================================
 *                         Ipnet_icmp_param
 *===========================================================================
 */
#ifdef IPCOM_USE_INET
typedef struct Ipnet_icmp_param_struct
{
    /* The virtual router to use when sending the ICMP message */
    Ip_u16         vr_index;
    /* The ifindex for the outgoing ICMP message (set to 0 for default) */
    Ipnet_netif   *netif;
    /* The packet that caused the ICMP response */
    Ipcom_pkt     *recv_pkt;
    /* The the ICMP message type */
    int            type;
    /* The value to use in the code field */
    int            code;
    /* If this ICMPv4 message is subject to throttling */
    Ip_bool        rate_limit;

    /* The type of service the packet should be sent with */
    Ip_u8          tos;

    /* TTL, if specified */
    Ip_u8          ttl;

    /* The address to use as source address, set to IP_NULL if IPNET should choose this */
    IP_CONST struct Ip_in_addr  *from;
    /* The address to which the ICMP message should be sent, must not be IP_NULL */
    IP_CONST struct Ip_in_addr  *to;

    union {
        /* Redirect msg: the on link address to use instead of the address of this host */
        Ip_u8                       param_pointer;
        IP_CONST struct Ip_in_addr *gateway_addr;
        Ip_u32                      max_path_mtu;
        Ipnet_ip4_addr_entry       *igmp_addr_entry;
    } data;
}
Ipnet_icmp_param;
#endif /* #ifdef IPCOM_USE_INET */

/*
 *===========================================================================
 *                         Ipnet_lock_state
 *===========================================================================
 */
#ifdef IP_PORT_LKM
typedef struct Ipnet_lock_state_struct
{
    Ip_bool       has_write_lock;
    Ip_bool       has_snd_rcv_lock;
    Ipnet_socket *sock;
}
Ipnet_lock_state;
#endif

/*
 ****************************************************************************
 * 6                    PROTOTYPES
 ****************************************************************************
 */

#ifndef IPNET_C
IP_EXTERN IP_FASTDATA Ipnet_data *ipnet;
IP_EXTERN Ip_u32      ipnet_flags;
IP_EXTERN Ipcom_cmd_int_str_map ipnet_bool_map[];
#endif


/*
 *===========================================================================
 *                         Driver/link upcalls
 *===========================================================================
 */

/* Note: The caller must be IPNET code locked to call ipnet_kioevent() */
IP_PUBLIC void ipnet_kioevent(Ipnet_netif *netif, int command,
                              void *data, int fcflags);


/*
 *===========================================================================
 *                         IPNET Internal
 *===========================================================================
 */
IP_GLOBAL void
ipcom_pause_os_network_daemon(void);
IP_GLOBAL void
ipnet_pause_os_network_daemon(void);
IP_GLOBAL void
ipnet_resume_os_network_daemon(void);

#if IPCOM_VR_MAX > 1
#define ipnet_get_effective_vr() ipnet->effective_vr
#else
#define ipnet_get_effective_vr() 0
#endif

#define ipnet_get_effective_pid() ipnet->effective_pid

IP_GLOBAL int ipnet_sysvar_netif_get_as_int(int domain,
                                            Ipnet_netif *netif,
                                            IP_CONST char *name,
                                            int default_val);
IP_GLOBAL int ipnet_sysvar_netif_get_as_int_ex(int domain,
                                               Ipnet_netif *netif,
                                               IP_CONST char *name,
                                               int default_val,
                                               IP_CONST Ipcom_cmd_int_str_map *map);
IP_GLOBAL Ip_u32 ipnet_sec_now(void);
IP_GLOBAL Ip_u32 ipnet_msec_now(void);
IP_GLOBAL Ip_bool ipnet_is_stack_task(void);

IP_GLOBAL Ipcom_pkt *ipnet_pkt_clone(Ipcom_pkt *pkt, Ip_bool is_ipstart_valid);
IP_GLOBAL void ipnet_schedule_pkt_report(Ipcom_pkt *pkt, Ipnet_softirq_handler do_report);
IP_GLOBAL void ipnet_pkt_data_freefunc(void *data, void *cookie);

IP_GLOBAL int ipnet_udp_input(Ipcom_pkt *pkt, Ip_bool is_multicast,
                              IP_CONST void *src_addr, IP_CONST void *dst_addr,
                              Ipnet_sock_lookup_f socklookup,
                              int *nat_t);

IP_GLOBAL void * ipnet_sysctl_if_add_addrs_one(void *buf, Ipnet_netif *netif,
                                               int domain, void *addrx, Ip_size_t *lenp);

IP_GLOBAL Ip_bool ipnet_has_priv(int priv);
IP_PUBLIC Ip_s32 ipnet_timeval_to_msec(struct Ip_timeval *tv);

#ifdef IPCOM_USE_MIB2
IP_GLOBAL Ipnet_netif *ipnet_ip6_socket_to_netif(struct Ipnet_socket_struct *sock);
#endif

#ifdef IPCOM_USE_MIB2_NEW
IP_GLOBAL void ipnet_eth_update_mib2_lastchange(Ipnet_route_entry *rt);
#endif

IP_GLOBAL Ip_u16 ipnet_sysctl_create_ifinfo(void *buf, Ipnet_netif *netif);


#ifdef IPCOM_USE_INET
IP_GLOBAL void ipnet_icmp4_param_init(Ipnet_icmp_param *icmp_param,
                                      Ipcom_pkt *pkt,
                                      Ipnet_ip4_output_param *ip4_param);
IP_GLOBAL int ipnet_icmp4_send(Ipnet_icmp_param *icmp_param, Ip_bool is_igmp);
#endif  /* #ifdef IPCOM_USE_INET */

#ifdef IPCOM_USE_HW_CHECKSUM_TX
IP_GLOBAL void ipnet_finish_transport_checksum(Ipcom_pkt *pkt);
#endif

IP_GLOBAL Ip_u32 ipnet_stats_tcp_established(void);

/*
 *===========================================================================
 *                         IPNET socket
 *===========================================================================
 */

IP_GLOBAL Ip_u32 ipnet_sock_get_bound_socket_size(void);
IP_GLOBAL Ip_bool ipnet_sock_has_waiting_writers(Ipnet_socket *sock);
IP_GLOBAL Ip_bool ipnet_is_sock_readable(Ipnet_socket *sock, int lowat);
IP_GLOBAL Ip_bool ipnet_is_sock_writable(Ipnet_socket *sock, int lowat);
IP_GLOBAL Ip_bool ipnet_is_sock_exceptional(Ipnet_socket *sock, int lowat);

IP_PUBLIC void ipnet_sock_data_avail(Ipnet_socket *sock, int so_errno, int how);

IP_GLOBAL Ip_bool ipnet_sock_has_waiting_writers(Ipnet_socket *sock);
IP_GLOBAL void ipnet_sock_pkt_drain(void);
IP_GLOBAL int ipnet_sock_register_ops(Ipnet_sock_ops *ops);
IP_GLOBAL void *ipnet_sock_alloc_private_data(Ipnet_socket *sock, Ip_size_t size);

IP_GLOBAL int ipnet_boot_config(void);
IP_GLOBAL int ipnet_raw_input(Ipcom_pkt *pkt, Ip_bool take_ownership, int proto,
                              IP_CONST void *src_addr, Ip_u16 src_port,
                              IP_CONST void *dst_addr, Ip_u16 dst_port,
                              Ipnet_sock_lookup_f socklookup);

IP_GLOBAL int ipnet_sock_bind(Ipnet_socket *sock, IP_CONST struct Ip_sockaddr *name,
                              Ip_socklen_t namelen, Ip_bool check_resuse);
IP_GLOBAL Ip_u16 ipnet_next_ephemeral_port(Ipnet_socket *sock);
IP_GLOBAL int  ipnet_queue_received_packet(Ipcom_pkt *pkt, Ipnet_socket *sock);
IP_GLOBAL void ipnet_sock_free(Ipnet_socket *sock, Ip_bool clear_sockets_arr);
IP_GLOBAL int ipnet_do_close(Ipnet_socket *sock);
IP_GLOBAL int ipnet_do_socketselect(int nfds,
                                    Ip_fd_set *readfds,
                                    Ip_fd_set *writefds,
                                    Ip_fd_set *exceptfds,
                                    struct Ip_timeval *timeout);
IP_GLOBAL int ipnet_sock_drop_all_multicast_memberships(Ipnet_socket *sock, Ipnet_netif *netif,
                                                        Ip_bool drop_ip4, Ip_bool drop_ip6);
IP_GLOBAL int ipnet_increase_hdr_space(Ipcom_pkt **ppkt, Ip_bool is_iphdr_valid);

IP_GLOBAL Ipnet_route_entry * ipnet_sock_get_route(Ipnet_socket *sock);
#define ipnet_sock_invalidate_ip4_cache(sock_ptr) \
    ((sock_ptr)->ip4->cache.rt_id = ipnet->route_cache_id - 1)
#define ipnet_sock_invalidate_ip6_cache(sock_ptr) \
    ((sock_ptr)->ip6->cache.rt_id = ipnet->route_cache_id - 1)

IP_GLOBAL int ipnet_ioctl_move_if_rt_to_vr(Ipnet_netif *netif, Ip_u16 vr);
IP_GLOBAL int ipnet_sock_create_lookup_tables(void);
IP_GLOBAL void ipnet_sock_rx_cache_invalidate(int domain);
IP_GLOBAL int ipnet_sock_set_reachable(Ipnet_socket *sock);
IP_GLOBAL void ipnet_sock_set_unreachable(Ipnet_socket *sock);
IP_GLOBAL void ipnet_sock_add_to_rx_cache(Ipnet_socket *sock,
                                          IP_CONST void *local_addr,
                                          Ip_u16 local_port,
                                          IP_CONST void *peer_addr,
                                          Ip_u16 peer_port,
                                          Ipnet_rx_cache_fast_input_f func);
IP_GLOBAL void ipnet_sock_bind_addr_removed(Ipnet_socket *sock_list);
IP_GLOBAL void ipnet_sock_change_addr_entry(Ipnet_socket *old_sock_list,
                                            Ipnet_socket **pnew_sock_list,
                                            void *addr_entry);
IP_GLOBAL int ipnet_sock_unconnect(Ipnet_socket *sock);
IP_GLOBAL int ipnet_sock_bind_to_port(Ipnet_socket *sock,
                                      IP_CONST struct Ip_sockaddr *name,
                                      Ip_bool check_reuse);
IP_GLOBAL void ipnet_sock_unbind(Ipnet_socket *sock);

IP_GLOBAL Ipnet_sock_ops *
ipnet_sock_get_ops(int domain, int type, int protocol);
#ifdef IPSCTP
IP_GLOBAL int ipnet_sock_sctp_register(void);
#endif
IP_GLOBAL int ipnet_sock_udp_register(void);
#ifdef IPTCP
IP_GLOBAL int ipnet_sock_tcp_register(void);
#endif
#ifdef IPCOM_USE_INET
IP_GLOBAL int ipnet_sock_ip4_register(void);
#endif
#ifdef IPCOM_USE_INET6
IP_GLOBAL int ipnet_sock_ip6_register(void);
#endif
#ifdef IPNET_USE_ROUTESOCK
IP_GLOBAL int ipnet_sock_route_register(void);
#endif
#ifdef IPNET_USE_NETLINKSOCK
IP_GLOBAL int ipnet_sock_netlink_register(void);
#endif
#ifdef IPCOM_USE_PACKET
IP_GLOBAL int ipnet_sock_packet_register(void);
#endif
#ifdef IPIPSEC2
IP_GLOBAL int ipnet_sock_key_register(void);
#endif
#ifdef IPNET_USE_SOCKDEV
IP_GLOBAL int ipnet_sock_sockdev_register(void);
#endif

IP_GLOBAL void ipnet_sock_ip4_get_ops(Ipnet_sock_inet_ops *ops);
IP_GLOBAL void ipnet_sock_ip6_get_ops(Ipnet_sock_inet_ops *ops);


/*
 *===========================================================================
 *          socket function callable by IPNET in user context
 *===========================================================================
 */

IP_GLOBAL int ipnet_usr_sock_pkt_recv(Ipnet_socket *sock, struct Ip_msghdr *msg, int flags);


/*
 *===========================================================================
 *                         IPNET packet socket
 *===========================================================================
 */
#ifdef IPCOM_USE_PACKET
IP_GLOBAL Ip_bool ipnet_packet_input(Ipnet_netif *netif, Ipcom_pkt *pkt);
IP_GLOBAL int     ipnet_packet_output(Ipnet_socket *sock, IP_CONST struct Ip_msghdr *msg, Ipcom_pkt *pkt);
IP_GLOBAL Ip_bool ipnet_packet_getsockopt(int optname, IP_CONST struct Ip_packet_mreq *mreq, Ip_socklen_t *optlen);
IP_GLOBAL Ip_bool ipnet_packet_setsockopt(Ipnet_socket *sock, int optname, IP_CONST struct Ip_packet_mreq *mreq, Ip_socklen_t optlen);
#endif /* IPCOM_USE_PACKET */


/*
 *===========================================================================
 *                         Fragmentation & reassembly
 *===========================================================================
 */
typedef Ip_bool (*Ipnet_is_part_of_same_pkt_func)(Ipcom_pkt *frag1, Ipcom_pkt *frag2);
typedef int (*Ipnet_get_offset_func)(Ipcom_pkt *frag);
typedef Ip_bool (*Ipnet_more_fragments_func)(Ipcom_pkt *frag);
typedef void (*Ipnet_update_ip_header_func)(Ipcom_pkt *pkt, Ipcom_pkt *frag_list);
typedef void (*Ipnet_reassemble_err_func)(Ipcom_pkt *frag_list);

IP_GLOBAL Ipcom_pkt *
ipnet_reassembly(Ipcom_pkt **reassebly_list,
                 Ipcom_pkt *pkt,
                 Ipnet_is_part_of_same_pkt_func is_part_of_same_pkt,
                 Ipnet_get_offset_func get_offset,
                 Ipnet_more_fragments_func more_fragments,
                 Ipnet_timeout_handler tmo_cb,
                 int frag_hdr_len);
IP_GLOBAL Ipcom_pkt *
ipnet_create_reassembled_packet(Ipcom_pkt *fragment_list,
                                int ip_hdr_size,
                                int frag_hdr_size,
                                Ipnet_update_ip_header_func update_ip_header,
                                Ipnet_reassemble_err_func report_reassemble_err);
IP_GLOBAL Ipcom_pkt *
ipnet_fragment_packet(Ipcom_pkt *pkt,
                      int ip_hdr_size,
                      int other_hdr_size,
                      int mtu,
                      Ipnet_more_fragments_func more_fragments);


/*
 *===========================================================================
 *                         ROUTING SOCKET
 *===========================================================================
 *
 */
#ifdef IPNET_USE_ROUTESOCK
IP_GLOBAL void ipnet_routesock_addr_add(struct Ipnet_netif_struct *netif, int domain, void *addr);
IP_GLOBAL void ipnet_routesock_addr_delete(struct Ipnet_netif_struct *netif, int domain, void *addr);
IP_GLOBAL void ipnet_routesock_if_change(struct Ipnet_netif_struct *netif);
IP_GLOBAL void ipnet_routesock_if_announce(Ipnet_netif *netif, Ip_u16 what);
IP_GLOBAL void ipnet_routesock_rt_add(Ipnet_route_entry *rt);
IP_GLOBAL void ipnet_routesock_rt_delete(Ipnet_route_entry *rt);
IP_GLOBAL void ipnet_routesock_rt_change(Ipnet_route_entry *rt);
IP_GLOBAL void ipnet_routesock_rt_miss(Ipcom_route *rtab, IP_CONST void *key);
IP_GLOBAL void ipnet_routesock_rt_redirect(Ipcom_route *rtab, Ip_u32 ifindex,
                                           IP_CONST void *dst, IP_CONST void *new_gw);
IP_GLOBAL void ipnet_routesock_rt_resolve(Ipnet_route_entry *rt);
#endif /* IPNET_USE_ROUTESOCK */




/*
 *===========================================================================
 *                         PACKET POOL FUNCTIONS
 *===========================================================================
 *
 */
#ifdef IPNET_USE_PACKET_POOL
IP_GLOBAL int ipnet_pkt_pool_init(void);
IP_GLOBAL int ipnet_pkt_pool_hdr_count_hint(void);
#endif /* IPNET_USE_PACKET_POOL */

/*
 *===========================================================================
 *                         DEBUG FUNCTIONS
 *===========================================================================
 *
 */
#ifdef IPNET_DEBUG
IP_GLOBAL void ipnet_pkt_remove_from_alloc_list(Ipcom_pkt *pkt);
IP_GLOBAL void ipnet_pkt_add_to_alloc_list(Ipcom_pkt *pkt);
IP_GLOBAL void ipnet_pkt_check(Ipcom_pkt *pkt);
IP_GLOBAL char * ipnet_debug_sock_to_str(Ipnet_socket *sock, char *str, Ip_size_t str_len);
IP_GLOBAL IP_CONST char * ipnet_nd_state_to_string(int state);
#endif /* IPNET_DEBUG */

#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
