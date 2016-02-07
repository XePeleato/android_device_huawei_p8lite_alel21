#define IPNET_PKT_CHECK_DISABLED
/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_debug.c,v $ $Revision: 1.55 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_debug.c,v $
 *     $Author: jhorteli $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *      Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Debug code for IPNET. Will make consistency tests and keep track of
 * allocated memory when building IPNET in debug mode.
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

#ifdef IPNET_DEBUG

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>

#include "ipnet_cmd.h"
#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_loopback.h"


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
 * 9                    GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_pkt_remove_from_alloc_list
 *===========================================================================
 * Description: Removes the packet from the list of allocated packets.
 * Parameters:  pkt - The packet to remove.
 *              NOTE: must be called with the packet pool lock held.
 * Returns:
 *
 */
#ifdef IPNET_USE_PACKET_POOL
IP_GLOBAL void
ipnet_pkt_remove_from_alloc_list(Ipcom_pkt *pkt)
{
    Ipcom_pkt **pkt_it;
    Ip_bool     found;

    found = IP_FALSE;
    for (pkt_it = &ipnet->allocated_pkt_list; *pkt_it != IP_NULL; pkt_it = &(*pkt_it)->next_alloc)
    {
        if (*pkt_it == pkt)
        {
            /* Unhook the packet */
            *pkt_it = pkt->next_alloc;
            pkt->next_alloc = IP_NULL;
            found = IP_TRUE;
            break;
        }
    }
    ip_assert(found);

    pkt->ifindex = 0;
    pkt->start   = pkt->maxlen + 3;
    pkt->end     = pkt->maxlen + 1;
}
#endif /* IPNET_USE_PACKET_POOL */

/*
 *===========================================================================
 *                    ipnet_pkt_add_to_alloc_list
 *===========================================================================
 * Description: Adds a packet to the list of allocated packets.
 *              NOTE: must be called with the packet pool lock held.
 * Parameters:  pkt - The packet to add.
 * Returns:
 *
 */
#ifdef IPNET_USE_PACKET_POOL
IP_GLOBAL void
ipnet_pkt_add_to_alloc_list(Ipcom_pkt *pkt)
{
    pkt->next_alloc = ipnet->allocated_pkt_list;
    ipnet->allocated_pkt_list = pkt;
    pkt->num_allocated++;
}
#endif /* IPNET_USE_PACKET_POOL */

/*
 *===========================================================================
 *                    ipnet_pkt_check
 *===========================================================================
 * Description: Check that an active packet is not in free list or in inputq
 * Parameters:  pkt - The packet to check.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_pkt_check(Ipcom_pkt *pkt)
{
#ifndef IPNET_PKT_CHECK_DISABLED
    Ipcom_pkt            *tmp_pkt;
    int                   num;
    int                   key;
#ifdef IPNET_USE_PACKET_POOL
    Ipnet_pkt_pool_entry *ppe;
    Ipnet_pkt_pool_entry *prev_ppe;
    int                   i;
    Ip_bool               found;
#endif /* IPNET_USE_PACKET_POOL */

    ip_assert(pkt->id != IPCOM_PKT_ID_FREEQ);
    if (IPCOM_PKT_HAS_MULTIPLE_OWNERS(pkt) == IP_FALSE)
        ip_assert(pkt->id != IPCOM_PKT_ID_INQ);


#ifdef IPNET_USE_PACKET_POOL
    /*
     * VxWorks note - one shouldn't really call ip_assert() with the
     * packet pool lock still held, since in case of assertion failure,
     * ip_assert() will do things that shouldn't be done with a spinlock
     * held.
     */
    IPNET_DATA_LOCK(ipnet->pkt_pool_lock);

    /* Check that the packet is in the list of allocated packets */
    found = IP_FALSE;
    for (tmp_pkt = ipnet->allocated_pkt_list; tmp_pkt != IP_NULL; tmp_pkt = tmp_pkt->next_alloc)
    {
        if (tmp_pkt == pkt)
        {
            found = IP_TRUE;
            break;
        }
    }
    ip_assert(found);

    /* Check that pkt is not in the free list. */
    for (tmp_pkt = ipnet->pkt_hdr_list; tmp_pkt != IP_NULL; tmp_pkt = tmp_pkt->next)
    {
        ip_assert(tmp_pkt->id == IPCOM_PKT_ID_FREEQ);
        ip_assert(pkt != tmp_pkt);
    }

    /* Check that the pool is sane */
    for (i = 0; i < ipnet->pkt_pool_elem; i++)
    {
        num = 0;
        for (prev_ppe = IP_NULL, ppe = ipnet->pkt_pool[i].free_list;
             ppe != IP_NULL;
             prev_ppe = ppe, ppe = ppe->next)
        {
            num++;
            (void)prev_ppe;
        }
        ip_assert(ipnet->pkt_pool[i].total >= num);
    }

    IPNET_DATA_UNLOCK(ipnet->pkt_pool_lock);
#endif /* IPNET_USE_PACKET_POOL */

    /* Check that pkt is not in the inqueue as well. */
    IPNET_MIN_LOCK(ipnet->inq.lock, key);

    num = 0;
    for (tmp_pkt = ipnet->inq.head; tmp_pkt != IP_NULL; tmp_pkt = tmp_pkt->next)
    {
        if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_LOOPED_BACK))
            /* This assert can fail for loopbacked packets under normal operation */
            ip_assert(tmp_pkt->id == IPCOM_PKT_ID_INQ);
        if (IPCOM_PKT_HAS_MULTIPLE_OWNERS(pkt) == IP_FALSE)
            ip_assert(pkt != tmp_pkt);
        num++;
    }
    ip_assert(ipnet->inq.count == num);

    IPNET_MIN_UNLOCK(ipnet->inq.lock, key);

    if (IPCOM_PKT_HAS_MULTIPLE_OWNERS(pkt) == IP_FALSE)
    {
        ip_assert(pkt->id != IPCOM_PKT_ID_OUTQ);
        ip_assert(pkt->id != IPCOM_PKT_ID_NETIF_OUTQ);
    }

#else /* IPNET_PKT_CHECK_DISABLED */
    (void) pkt;
#endif /* IPNET_PKT_CHECK_DISABLED */
}


/*
 *===========================================================================
 *                    ipnet_debug_sock_to_str
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL char *
ipnet_debug_sock_to_str(Ipnet_socket *sock, char *str, Ip_size_t str_len)
{
#ifdef IPNET_DEBUG
#ifdef IPCOM_USE_INET6
    char src_str[IP_INET6_ADDRSTRLEN];
    char dst_str[IP_INET6_ADDRSTRLEN];
#else
    char src_str[IP_INET_ADDRSTRLEN];
    char dst_str[IP_INET_ADDRSTRLEN];
#endif
#endif /* IPNET_DEBUG*/

    switch (sock->ipcom.domain)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        (void)ipcom_inet_ntop(IP_AF_INET, &sock->ip4->saddr_n, src_str, sizeof(src_str));
        (void)ipcom_inet_ntop(IP_AF_INET, &sock->ip4->daddr_n, dst_str, sizeof(dst_str));
        break;
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        (void)ipcom_inet_ntop(IP_AF_INET6, &sock->ip6->saddr, src_str, sizeof(src_str));
        (void)ipcom_inet_ntop(IP_AF_INET6, &sock->ip6->daddr, dst_str, sizeof(dst_str));
        break;
#endif /* IPCOM_USE_INET6 */
    default:
        src_str[0] = '\0';
        dst_str[0] = '\0';
    }
    ipcom_snprintf(str, str_len, "fd:%d ref:%d, domain:%d proto:%d %s:%d %s:%d",
                   sock->ipcom.fd,
                   ipcom_atomic_get(&sock->ref_count),
                   sock->ipcom.domain,
                   sock->proto,
                   src_str, sock->sport,
                   dst_str, sock->dport);
    return str;
}


/*
 *===========================================================================
 *                     ipnet_timeout_to_string
 *===========================================================================
 * Description: Adds a element to string function for timeouts that uses
 *              the specified timeout handler.
 * Parameters:  handler - The timeout handler the element is using.
 *              to_str  - The to string handler for the element.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_timeout_to_string(Ipnet_timeout_handler handler,
                        Ipnet_timeout_to_string_f to_str)
{
    ipnet->tmo_map = ipcom_realloc(ipnet->tmo_map,
                                   ++ipnet->tmo_map_size * sizeof(*ipnet->tmo_map));
    ipnet->tmo_map[ipnet->tmo_map_size - 1].key         = handler;
    ipnet->tmo_map[ipnet->tmo_map_size - 1].to_str_func = to_str;
}


/*
 *===========================================================================
 *                    ipnet_nd_state_to_string
 *===========================================================================
 * Description: Convert ARP state to a string representation.
 * Parameters:  state - The state to convert.
 * Returns:     A string representation
 *
 */
#ifdef IPCOM_USE_INET6
IP_GLOBAL IP_CONST char *
ipnet_nd_state_to_string(int state)
{
    switch (state)
    {
    case IPNET_ND_UNINITIALIZED:
        return "UNINITIALIZED";
    case IPNET_ND_INCOMPLETE:
        return "INCOMPLETE";
    case IPNET_ND_REACHABLE:
        return "REACHABLE";
    case IPNET_ND_STALE:
        return "STALE";
    case IPNET_ND_DELAY:
        return "DELAY";
    case IPNET_ND_PROBE:
        return "PROBE";
    case IPNET_ND_PROXY:
        return "PROXY";
    default:
        IP_PANIC();
        return "Unknown ND state";
    }
}
#endif /* #ifdef IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                         ipnet_print_sock_flags
 *===========================================================================
 * Description: Converts the flags field from a socket to a human readable
 *              form
 * Parameters:  flags - flags from a TCP socket.
 * Returns:
 *
 */
#ifdef NOT_CALLED_BY_ANY_OTHER_FUNCTION
IP_PUBLIC void
ipnet_print_sock_flags(Ip_u32 flags)
{
    printf("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n",
           flags & IPNET_SOCKET_FLAG_REUSEADDR ? "REUSEADDR\n" : "",
           flags & IPNET_SOCKET_FLAG_REUSEPORT ? "REUSEPORT\n" : "",
           flags & IPNET_SOCKET_FLAG_DONTROUTE ? "DONTROUTE\n" : "",
           flags & IPNET_SOCKET_FLAG_LOOP_MULTICAST ? "MULTICAST\n" : "",
           flags & IPNET_SOCKET_FLAG_TCP_NODELAY ? "NODELAY\n" : "",
           flags & IPNET_SOCKET_FLAG_RECV_IF ? "RECV_IF\n" : "",
           flags & IPNET_SOCKET_FLAG_RECVPKTINFO ? "RECVPKTINFO\n" : "",
           flags & IPNET_SOCKET_FLAG_KEEPALIVE ? "KEEPALIVE\n" : "",
           flags & IPNET_SOCKET_FLAG_NONBLOCKING ? "NONBLOCKING\n" : "",
           flags & IPNET_SOCKET_FLAG_ADDCHECKSUM ? "ADDCHECKSUM\n" : "",
           flags & IPNET_SOCKET_FLAG_OOBINLINE ? "OOBINLINE\n" : "",
           flags & IPNET_SOCKET_FLAG_SENDER_LINK_ADDR ? "SENDER_LINK_ADDR\n" : "",
           flags & IPNET_SOCKET_FLAG_REACHABLE ? "REACHABLE\n" : "",
           flags & IPNET_SOCKET_FLAG_BOUND ? "BOUND\n" : "",
           flags & IPNET_SOCKET_FLAG_CONNECTING ? "CONNECTING\n" : "",
           flags & IPNET_SOCKET_FLAG_CONNECTED ? "CONNECTED\n" : "",
           flags & IPNET_SOCKET_FLAG_READ_CLOSED ? "READ_CLOSED\n" : "",
           flags & IPNET_SOCKET_FLAG_WRITE_CLOSED ? "WRITE_CLOSED\n" : "",
           flags & IPNET_SOCKET_FLAG_PEER_WRITE_CLOSED ? "PEER_WRITE_CLOSED\n" : "",
           flags & IPNET_SOCKET_FLAG_CLOSED ? "CLOSED\n" : "",
           flags & IPNET_SOCKET_FLAG_MROUTE_CONTROL ? "MROUTE_CONTROL\n" : "",
           flags & IPNET_SOCKET_FLAG_DRAINABLE ? "DRAINABLE\n" : "",
           flags & IPNET_SOCKET_FLAG_GRE_KEY ? "GRE_KEY\n" : "",
           flags & IPNET_SOCKET_FLAG_RX_CACHED ? "RX_CACHED\n" : "",
           flags & IPNET_SOCKET_FLAG_PKTAVAIL_CB_PENDING ? "PKTAVAIL_CB_PENDING\n" : "",
           flags & IPNET_SOCKET_FLAG_DEBUG ? "DEBUG\n" : "",
           flags & IPNET_SOCKET_FLAG_HAS_STICKY_OPT ? "HAS_STICKY_OPT\n" : "");
}
#endif

/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#endif /* IPNET_DEBUG */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
