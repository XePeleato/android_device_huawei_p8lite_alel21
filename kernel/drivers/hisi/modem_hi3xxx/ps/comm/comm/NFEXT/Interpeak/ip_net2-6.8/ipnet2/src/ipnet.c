/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet.c,v $ $Revision: 1.471.16.2 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet.c,v $
 *     $Author: jhorteli $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */

#define IPNET_C

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
 ****************************************************************************
 */
#define IPCOM_USE_CLIB_PROTO
#include <ipcom_atomic.h>
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_ipd.h>
#include <ipcom_netif.h>
#include <ipcom_os.h>
#include <ipcom_shell.h>
#include <ipcom_sock2.h>
#include <ipcom_syslog.h>
#include <ipcom_sysvar.h>
#include <ipcom_type.h>

#include "ipnet.h"
#include "ipnet_cmd.h"
#include "ipnet_eth.h"
#include "ipnet_h.h"
#include "ipnet_loopback.h"
#include "ipnet_pkt_queue.h"
#include "ipnet_signal.h"
#include "ipnet_usr_sock.h"

#ifdef IPTCP
#include <iptcp.h>
#endif

#ifdef IPSCTP
#include <ipsctp.h>
#endif

#ifdef IPIPSEC2
#include <ipipsec.h>
#endif

#ifdef IPMPLS
#include <ipnet_mpls.h>
#endif

#ifdef IPFIREWALL
#include <ipfirewall.h>
#include <ipfirewall_h.h>
#endif

#ifdef IPIFPROXY
#include <ipifproxy.h>
#endif

#ifdef IPCOM_WV_INSTRUMENTATION
#include <ipcom_windview.h>
#endif

#ifdef IPCOM_USE_FORWARDER
#include <ipcom_forwarder.h>
#endif

#ifdef IPCOM_USE_JOB_QUEUE
#include <ipcom_job_queue.h>
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* pkt->data size need to be IPNET_PKT_ALIGNMENT larger for rounding up
 * to correct &pkt->data[pkt->start] alignment on incoming buffers.
 */
#ifdef IPCOM_PKT_ALIGNMENT
#define IPNET_PKT_ALIGNMENT IPCOM_PKT_ALIGNMENT
#else
#define IPNET_PKT_ALIGNMENT 64
#endif


/* Number of Ipnet_timeout structures that is allocated each time */
#define IPNET_TMO_ALLOC_COUNT 30

#define IPNET_IPNETD_PIPE_SIZE IPCOM_PIPE_MSG_LIMIT_MANY


#define IPNET_PKT_GET_REPORT_FUNC(pkt)       \
    (void(*)(void *)) ((pkt)->link_cookie)
#define IPNET_PKT_SET_REPORT_FUNC(pkt, func) \
    ((pkt)->link_cookie = (Ip_ptrdiff_t) func)


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/* Data for kioevent that had to be postponed */
typedef struct Ipnet_kioevent_softirq_struct
{
    Ipnet_softirq softirq;
    int           event;
    Ip_u32        ifindex;
}
Ipnet_kioevent_softirq;


/* Data for packet report softirq */
typedef struct Ipnet_pkt_report_softirq_struct
{
    Ipnet_softirq softirq;
    Ipcom_pkt    *pkt;
}
Ipnet_pkt_report_softirq;


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */
#ifdef IPIPSEC2
IP_EXTERN void ipipsec_process_signal(Ipnet_sig *sig);
#endif

#ifdef IPCOM_USE_JOB_QUEUE
IP_PUBLIC void ipcom_job_queue_pipe_schedule(Ipcom_pipe *pipe);
#endif

#if defined(IPCOM_USE_FORWARDER) && !defined(ipcom_forwarder_create_cf)
IP_EXTERN int ipcom_forwarder_create_cf(void);
#endif

#ifdef IPNET_ROUTE_CACHE_ID_IN_SHARED_MEM
IP_EXTERN int * ipcom_route_cache_id_alloc(void);
#endif

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_STATIC int ipnet_init(void);
IP_STATIC int ipnet_open(void);

IP_PUBLIC Ip_err ipnet_create(void);
IP_PUBLIC Ip_err ipnet_start(void);
IP_PUBLIC void ipnet_pkt_input(Ipnet_netif *netif, Ipcom_pkt *pkt);

#if defined(IP_PORT_VXWORKS) && defined(IPCOM_USE_SOCK_COMPAT)
IP_GLOBAL Ip_err ipcom_windnet_socklib_init(void);
IP_GLOBAL int ipcom_windnet_socklib_sock_size(void);
#endif

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Global data
 *===========================================================================
 */

IP_PUBLIC int           ipcom_conf_max_link_hdr_size;  /* set in ipnet_init(). */

IP_PUBLIC IP_FASTDATA Ipnet_data   *ipnet = IP_NULL;
IP_PUBLIC Ip_u32        ipnet_flags = 0;

IP_PUBLIC Ipcom_cmd_int_str_map ipnet_bool_map[] = {
    { IP_TRUE , "yes"      },
    { IP_FALSE, "no"       },
    { IP_TRUE , "enable"   },
    { IP_TRUE , "enabled"  },
    { IP_FALSE, "disable"  },
    { IP_FALSE, "disabled" },
    { IP_TRUE , "on"       },
    { IP_FALSE, "off"      },
    { IP_TRUE,  "true"     },
    { IP_FALSE, "false"    },
    { IP_NULL,  0          }
};


/*
 ****************************************************************************
 * 10                   STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_suspend
 *===========================================================================
 * Description: Suspends the IPNET daemon until a IPNET_SIG_RESUME
 *                    signal is receieved. Is used to workaround
 *                    constructs that used to grab the IPNET lock and
 *                    then directly change IPNET global data.
 *
 * Parameters:  sig - IPNET_SIG_SUSPEND signal.
 * Returns:
 *
 */
IP_STATIC void
ipnet_suspend(Ipnet_sig *sig)
{
    Ipnet_sig       rsig;
    Ipnet_sig      *psig;
    Ipcom_pipe_attr attr;

    ipcom_pipe_attr_init(&attr);
    attr.msg_limit = IPCOM_PIPE_MSG_LIMIT_FEW;
    rsig.response_pipe = ipcom_pipe_new(IPCOM_PIPE_GENERIC, &attr);

    if (rsig.response_pipe == IP_NULL)
    {
        /* Cannot suspend since there is too little memory */
        IP_PANIC2();
        sig->sig_type = IPNET_SIG_RESUME;
        sig->sender_pid   = ipnet->task.pid;
        sig->receiver_pid = ipnet->effective_pid;
        ipcom_pipe_send(sig->response_pipe, sig);
        return;
    }

    rsig.sig_type     = IPNET_SIG_SUSPEND;
    rsig.sender_pid   = ipnet->task.pid;
    rsig.receiver_pid = ipnet->effective_pid;
    ipcom_pipe_send(sig->response_pipe, &rsig);
    ipcom_pipe_recv(rsig.response_pipe, (void **) &psig);

    ip_assert(psig == &rsig);
    ip_assert(psig->sig_type == IPNET_SIG_RESUME);

    ipcom_pipe_free(rsig.response_pipe);
}


/*
 *===========================================================================
 *                    ipnet_kioevent_softirq
 *===========================================================================
 * Description: Issues kioevent that had to be postponed in the
 *              ipnet_kioevent() handler since it was called in interrupt
 *              or non-ipnet context.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_kioevent_softirq(Ipnet_kioevent_softirq *si)
{
    Ipnet_netif *netif;

    netif = ipnet_if_indextonetif(IPCOM_VR_ANY, si->ifindex);
    if (netif != IP_NULL)
        ipnet_kioevent(netif, si->event, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
    ipcom_slab_free(ipnet->slabs.kioevent_softirq, si);
}


/*
 *===========================================================================
 *                     ipnet_update_time
 *===========================================================================
 * Description: Updates the internal timers
 * Parameters:
 * Returns:
 *
 */
IP_INLINE void
ipnet_update_time(void)
{
    Ip_u32 added_sec;

    ipnet->msec_now = ipnet_msec_now();
    added_sec = (ipnet->msec_now - ipnet->msec_last) / 1000;
    ipnet->sec_now   += added_sec;
    ipnet->msec_last += added_sec * 1000;
}


/*
 *===========================================================================
 *                     ipnet_update_time_tmo
 *===========================================================================
 * Description: Notifies IPNET that the internal timer must be update next
 *              time a timeout is scheduled.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_update_time_tmo(void *unused)
{
    IPCOM_UNUSED_ARG(unused);
    ipnet->update_internal_timer = IP_TRUE;
}


/*
 *===========================================================================
 *                   ipnet_update_time_tmo_to_string
 *===========================================================================
 * Description: Adds a element to string function for timeouts that uses
 *              the specified timeout handler.
 * Parameters:  handler - The timeout handler the element is using.
 *              to_str  - The to string handler for the element.
 * Returns:
 *
 */
#ifdef IPNET_DEBUG
IP_STATIC int
ipnet_update_time_tmo_to_string(void *unused, char *buf, int buf_len)
{
    IPCOM_UNUSED_ARG(unused);
    return ipcom_snprintf(buf, buf_len, "Internal timer update");
}
#endif


/*
 *===========================================================================
 *                       ipnet_timeout_pq_cmp
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_timeout_pq_cmp(const Ipnet_timeout *tmo1, const Ipnet_timeout *tmo2)
{
    if (tmo1->msec == tmo2->msec)
        return 0;
    if (IPCOM_IS_LT(tmo1->msec, tmo2->msec))
        return 1;
    return -1;
}


/*
 *===========================================================================
 *                    ipnet_timeout_pq_store_index
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_timeout_pq_store_index(Ipnet_timeout *tmo, int pq_index)
{
    tmo->pq_index = pq_index;
}


/*
 *===========================================================================
 *                    ipnet_timeout_duration_until_next
 *===========================================================================
 * Description: Returns the duration, in milliseconds, until the next timeout
 *              event occurs.
 * Parameters:
 * Returns:     The duration or -1 if no timeout is pending.
 *
 */
IP_STATIC Ip_s32
ipnet_timeout_duration_until_next(void)
{
    Ipnet_timeout *tmo;
    Ip_s32         duration = -1;

    tmo = ipcom_pqueue_get(ipnet->timeouts, 1);
    if (tmo == IP_NULL)
        /* Wait forever */
        duration = -1;
    else if (IPCOM_IS_GEQ(ipnet->msec_now, tmo->msec))
        /* Timeout event is pending now */
        duration = 0;
    else
        duration = (Ip_s32) (tmo->msec - ipnet->msec_now);

    return duration;
}


/*
 *===========================================================================
 *                   ipnet_process_pending_timeouts
 *===========================================================================
 * Description: Calls the handler for all events that has timed out.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_process_pending_timeouts(void)
{
    Ipnet_timeout         *tmo;
    void                  *cookie;
    Ipnet_timeout_handler  handler;

#ifdef IPCOM_USE_JOB_QUEUE
    ipnet_update_time();
#endif

    while (IP_NULL != (tmo = ipcom_pqueue_get_next(ipnet->timeouts))
           && IPCOM_IS_GEQ(ipnet->msec_now, tmo->msec))
    {
        cookie = tmo->cookie;
        handler = tmo->handler;
        ipnet_timeout_cancel(tmo);
        if (handler != IP_NULL)
            handler(cookie);
    }

#ifdef IPCOM_USE_JOB_QUEUE
    do {
        Ip_s32 duration = ipnet_timeout_duration_until_next();
        if (duration >= 0)
            ipcom_job_queue_schedule_singleton_delayed(ipnet->timeout_job, duration);
    } while (0);
#endif
}


#ifdef IPNET_ETH_FASTPATH
/*
 *===========================================================================
 *                    ipnet_ip4_try_eth_fast_forward_path
 *===========================================================================
 * Description: Fast packet forward path.
 * Parameters:  innetif - Incoming network interface.
 *              pkt - The incoming packet.
 *              ip_hdr - The IPv4 header for the packet.
 * Returns:     One of the IPNET_FORWARD_CACHE_xxx constants.
 *
 */
#ifdef IPCOM_USE_INET
IP_STATIC int
ipnet_ip4_try_eth_fast_forward_path(Ipnet_netif *innetif, Ipcom_pkt *pkt, Ipnet_pkt_ip *ip_hdr)
{
    Ipnet_ip4_rt_cache *cache = innetif->inet4_last_forward_cache;
    int                 pass;

    /* Try the last used entry in the first pass
       and look in the forward cache in the 2:nd pass */
    for (pass = 0; pass < 2; pass++)
    {
        if (ipnet_route_cache_is_valid(cache->rt_id)
            && cache->dst[0]  == ip_hdr->dst[0]
            && cache->dst[1]  == ip_hdr->dst[1]
            && cache->src[0]  == ip_hdr->src[0]
            && cache->src[1]  == ip_hdr->src[1]
            && cache->ifindex == innetif->ipcom.ifindex
            && cache->tos     == ip_hdr->tos
            && cache->p       == ip_hdr->p
            && ip_hdr->ttl > 1
            && ip_hdr->v_hl == 0x45)
        {
            Ipnet_netif   *outnetif;
            Ip_u32         path_mtu;
            int            ret;
            Ipnet_pkt_eth *eth;

            outnetif = cache->rt_next_hop->netif;

            /* Test to see if the interface is able to do ethernet fastpath. */
            if (IP_BIT_ISFALSE(outnetif->ipcom.flags, IP_IFF_LINK2))
                return IPNET_FORWARD_CACHE_MISS;

            if (cache->rt_next_hop->metrics.rmx_mtu)
                path_mtu = cache->rt_next_hop->metrics.rmx_mtu;
            else
                path_mtu = outnetif->ipcom.mtu;
            /* pkt->start is the beginning of the Ethernet header, not
               beginning of the IPv4 header */
            path_mtu += innetif->ipcom.link_hdr_size;

            if (IP_UNLIKELY(path_mtu < (Ip_u32) (pkt->end - pkt->start)))
                return IPNET_FORWARD_CACHE_MISS;

            IP_INCREMENTAL_CHECKSUM(ip_hdr);

            pkt->ifindex = outnetif->ipcom.ifindex;

            IPCOM_MIB2(ipForwDatagrams++);
            IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCInForwDatagrams, 1);
            IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCInForwDatagrams, 1, innetif);
            IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCOutForwDatagrams, 1);
            IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCOutForwDatagrams, 1, outnetif);
            IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCOutTransmits, 1);
            IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCOutTransmits, 1, outnetif);
            IPCOM_MIB2_SYSWI_U64_ADD(v4, ipSystemStatsHCOutOctets, pkt->end - pkt->start);
            IPCOM_MIB2_PERIF_U64_ADD(v4, ipIfStatsHCOutOctets, pkt->end - pkt->start, outnetif);

            ++cache->rt_next_hop->use;

            eth = (Ipnet_pkt_eth *)&pkt->data[pkt->start];
            IPNET_SET_ETHADDR(eth->dst, IP_SOCKADDR_DL_LLADDR(cache->rt_next_hop->gateway));
            IPNET_SET_ETHADDR(eth->src, outnetif->ipcom.link_addr);

            IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM);
            IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_FORWARDED);
            ret = ipnet_if_drv_output(outnetif, pkt);
            if (IP_UNLIKELY(ret != 0))
            {
                if (ret == -IP_ERRNO_EWOULDBLOCK)
                {
                    IPNET_STATS(pkt_output_ewouldblock++);
                    ipcom_pkt_free(pkt);
                }
                IPCOM_MIB2(ipRoutingDiscards++);
            }

            innetif->inet4_last_forward_cache = cache;
            return IPNET_FORWARD_CACHE_TAKEN;
        }
        IPNET_IP4_GET_FORWARD_CACHE_ENTRY(cache, ip_hdr);
    }

    return IPNET_FORWARD_CACHE_MISS;
}
#endif /* IPCOM_USE_INET */


/*
 *===========================================================================
 *                    ipnet_ip6_try_eth_fast_forward_path
 *===========================================================================
 * Description: Ethernet Fast packet forward path.
 * Parameters:  innetif - Incoming network interface.
 *              pkt - The incoming packet.
 *              ip_hdr - The IPv6 header for the packet.
 * Returns:     One of the IPNET_FORWARD_CACHE_xxx constants.
 *
 */
#ifdef IPCOM_USE_INET6
IP_STATIC int
ipnet_ip6_try_eth_fast_forward_path(Ipnet_netif *innetif, Ipcom_pkt *pkt, Ipnet_pkt_ip6 *ip_hdr)
{
    Ipnet_ip6_rt_cache *cache = innetif->inet6_last_forward_cache;
    Ipnet_netif        *outnetif;
    Ip_u32              path_mtu;
    Ip_u32              ver_class_flow = IP_GET_32ON16(&ip_hdr->ver_class_flow);
    int                 pass;

    /* Try the last used entry in the first pass
       and look in the forward cache in the 2:nd pass */
    for (pass = 0; pass < 2; pass++)
    {
        if (ipnet_route_cache_is_valid(cache->rt_id)
            && cache->ifindex == innetif->ipcom.ifindex
            && cache->ver_class_flow == ver_class_flow
            && cache->nxt == ip_hdr->nxt
            && IP_IN6_ARE_ADDR_EQUAL(&cache->dst, &ip_hdr->dst)
            && IP_IN6_ARE_ADDR_EQUAL(&cache->src, &ip_hdr->src)
            && ip_hdr->hlim > 1)
        {
            int            ret;
            Ipnet_pkt_eth *eth;

            outnetif = cache->rt_next_hop->netif;

            /* Test if the interface was enabled to do ethernet fastpath. */
            if (IP_BIT_ISFALSE(outnetif->ipcom.flags, IP_IFF_LINK2))
                return IPNET_FORWARD_CACHE_MISS;

            if (cache->rt_next_hop->metrics.rmx_mtu)
                path_mtu = cache->rt_next_hop->metrics.rmx_mtu;
            else
                path_mtu = outnetif->ipcom.mtu;
            /* pkt->start is the beginning of the Ethernet header, not
               beginning of the IPv6 header */
            path_mtu += innetif->ipcom.link_hdr_size;

            if (IP_UNLIKELY(path_mtu < (Ip_u32) (pkt->end - pkt->start)))
                return IPNET_FORWARD_CACHE_MISS;

            --ip_hdr->hlim;
            pkt->ifindex = outnetif->ipcom.ifindex;

            ++cache->rt_next_hop->use;

            eth = (Ipnet_pkt_eth *)&pkt->data[pkt->start];
            IPNET_SET_ETHADDR(eth->dst, IP_SOCKADDR_DL_LLADDR(cache->rt_next_hop->gateway));
            IPNET_SET_ETHADDR(eth->src, outnetif->ipcom.link_addr);

            IPCOM_MIB2_NETIF2(ipv6IfStatsOutForwDatagrams++, outnetif);
            IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCInForwDatagrams, 1);
            IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCInForwDatagrams, 1, innetif);
            IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCOutForwDatagrams, 1);
            IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCOutForwDatagrams, 1, outnetif);
            IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCOutTransmits, 1);
            IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCOutTransmits, 1, outnetif);
            IPCOM_MIB2_SYSWI_U64_ADD(v6, ipSystemStatsHCOutOctets, pkt->end - pkt->start);
            IPCOM_MIB2_PERIF_U64_ADD(v6, ipIfStatsHCOutOctets, pkt->end - pkt->start, outnetif);

            IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM);
            IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_FORWARDED);
            ret = ipnet_if_drv_output(outnetif, pkt);
            if (IP_UNLIKELY(ret != 0))
            {
                if (ret == -IP_ERRNO_EWOULDBLOCK)
                {
                    IPNET_STATS(pkt_output_ewouldblock++);
                    ipcom_pkt_free(pkt);
                }
                IPCOM_MIB2(ipv6DiscardedRoutes++);
            }

            innetif->inet6_last_forward_cache = cache;
            return IPNET_FORWARD_CACHE_TAKEN;
        }

        IPNET_IP6_GET_FORWARD_CACHE_ENTRY(cache, ip_hdr, ver_class_flow);
    }

    return IPNET_FORWARD_CACHE_MISS;
}
#endif /* IPCOM_USE_INET6 */
#endif /* IPNET_ETH_FASTPATH */


/*
 *===========================================================================
 *                    ipnet_pkt_input
 *===========================================================================
 * Context:     Stack context
 * Description: Inputs a packet to the stack.
 * Parameters:  netif - The interface the packet was received on.
 *              pkt - The received packet. pkt->start is the offset to the
 *              the link layer header.
 * Returns:
 *
 */
IP_PUBLIC void
ipnet_pkt_input(Ipnet_netif *netif, Ipcom_pkt *pkt)
{
    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_INPUT);
    IPNET_DEBUG_LINE(ipnet_pkt_check(pkt));

    /* Make sure the link layer header is in the CPU cache */
    IP_PREPARE_FOR_READ(&pkt->data[pkt->start]);

    if (netif == IP_NULL)
    {
        netif = ipnet_if_indextonetif(IPCOM_VR_ANY, pkt->ifindex);
        if (IP_UNLIKELY(netif == IP_NULL))
            goto drop;
    }

    IPCOM_WV_MARKER_1 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_VERBOSE, 1, 1, IPCOM_WV_NETDEVENT_START,
                       ipnet_pkt_input, IPCOM_WV_IPNET_MODULE, IPCOM_WV_NETD_IP4);
    IPNET_STATS(pkt_input++);
    netif->ibytes += pkt->end - pkt->start;
    netif->ipackets++;

#ifdef IPIFPROXY
    if (ipifproxy_frame_rx(&netif->ipcom, pkt) == IPCOM_SUCCESS)
        return;
#endif

#ifdef IPNET_ETH_FASTPATH
    if (netif->ipcom.type == IP_IFT_ETHER
        && IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_LINK2))
    {
        void *network_hdr = &pkt->data[pkt->start + netif->ipcom.link_hdr_size];

        switch (((Ipnet_pkt_eth *)&pkt->data[pkt->start])->d.type)
        {
#ifdef IPCOM_USE_INET
        case IPNET_ETHFRAME_IP:
            if (ipnet_ip4_try_eth_fast_forward_path(netif,
                                                    pkt,
                                                    network_hdr)
                == IPNET_FORWARD_CACHE_TAKEN)
            {
                return;
            }
            break;
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
        case IPNET_ETHFRAME_IPV6:
            if (ipnet_ip6_try_eth_fast_forward_path(netif,
                                                    pkt,
                                                    network_hdr)
                == IPNET_FORWARD_CACHE_TAKEN)
            {
                return;
            }
            break;
#endif /* IPCOM_USE_INET6 */
        default:
            break;
        }
    }
#endif /* IPNET_ETH_FASTPATH */

    /* Interface can be locked after the Ethernet fast forward path
       since the Ethernet fast forward path will never block so that
       the interface can be removed "under our feet's" */
    IPNET_IF_LOCK(netif);

    /* Receiving packets on a non-running interface should be almost impossible since
       the link has been lost of the IFF_RUNNING flag is cleared.
       Do not check the IFF_UP flag here since some link-layer types need to receive
       packet while the interface is still down */
    if (IP_UNLIKELY(IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_RUNNING)))
        goto drop;

#ifdef IPFIREWALL_USE_MAC_FILTER
    if (netif->ipcom.type == IP_IFT_ETHER || netif->ipcom.type == IP_IFT_L2VLAN)
    {
        if (ipfirewall != IP_NULL
            && ipfirewall_mac_input_hook(&netif->ipcom, pkt) == 1)
        {
            /* MAC filter blocked packed */
            goto drop;
        }
    }
#endif /* IPFIREWALL_USE_MAC_FILTER */

    pkt->next     = IP_NULL;
    pkt->ifindex  = netif->ipcom.ifindex;
    pkt->vr_index = netif->vr_index;

    ipnet->effective_vr = netif->vr_index;

#ifdef IPCOM_USE_PACKET
    /* Give a copy of packet to all matching packet sockets,
       Ethernet is handled separately inside the link layer handler */
    if (ipnet->num_packet_sock > 0
        && netif->ipcom.type != IP_IFT_ETHER
        && netif->ipcom.type != IP_IFT_TUNNEL
        && netif->ipcom.type != IP_IFT_IEEE80211
        && ipnet_packet_input(netif, pkt) == IP_FALSE)
    {
        IPNET_IF_UNLOCK(netif);
        return;
    }
#endif

    if (netif->link_input(netif, pkt) < 0)
        netif->iqdrops++;

    IPNET_IF_UNLOCK(netif);
    return;

 drop:

    IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                      1, 2, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                      ipnet_pkt_input, IPCOM_WV_NETD_INFO_DROPPED,
                      IPCOM_WV_IPNET_MODULE, IPCOM_WV_NETD_IP4);
    IPNET_STATS(pkt_input_err++);
    if (netif != IP_NULL)
    {
        netif->iqdrops++;
        IPNET_IF_UNLOCK(netif);
    }
    ipcom_pkt_free(pkt);
}


/*
 *===========================================================================
 *                      ipnet_pkt_kclone
 *===========================================================================
 * Description: Makes a copy of a packet.
 * Parameters:  pkt - The packet to clone.
 *              is_ipstart_valid - Set to IP_TRUE if the pkt->ipstart field
 *              is valid, data can be used a little more efficient in that case.
 *              additional_space - Extra space that the clone will have
 *              compared to the original.
 * Returns:     IP_NULL if no new packet could be allocated, the clone otherwise.
 *
 */
IP_STATIC Ipcom_pkt *
ipnet_pkt_kclone(Ipcom_pkt *pkt, Ip_bool is_ipstart_valid, int additional_space)
{
    Ipcom_pkt *clone;
    int        mtu;
    int        data_start;
    int        data_len;
    void      *clone_buf;

    ip_assert(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_ALLOC));

    data_start = (is_ipstart_valid ? pkt->ipstart : pkt->start);
    data_len = pkt->end - data_start;

    if (is_ipstart_valid)
        /* Make sure there is enough space to fit any link layer header */
        mtu = data_len;
    else
        /* Make sure it is enough space to clone 'start' and 'end' offsets */
        mtu = IP_MAX(pkt->maxlen - IPNET_PKT_ALIGNMENT - ipcom_conf_max_link_hdr_size, data_len);

    clone = ipcom_pkt_malloc(mtu + additional_space,
                             IP_FLAG_FC_STACKCONTEXT);
    if (clone == IP_NULL)
    {
        IPCOM_WV_MARKER_1 ( IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_CRITICAL, 1, 3, IPCOM_WV_NETD_NOBUFS,
                            ipnet_pkt_kclone, IPCOM_WV_IPNET_MODULE, IPCOM_WV_NETD_IP4);
        IPNET_STATS(pkt_clone_nomem++);
        return IP_NULL;
    }

    ipcom_memcpy(clone, pkt, ip_offsetof(Ipcom_pkt, maxlen));

    if (!is_ipstart_valid)
    {
        clone->start = (clone->maxlen - (pkt->end - pkt->start)) & ~0x3;
        clone->end   = clone->start + (pkt->end - pkt->start);
        clone_buf = &clone->data[clone->start];
    }
    else
    {
        /* The link header is also copied in this case */
        int link_hdr = IP_MIN(ipcom_conf_max_link_hdr_size, data_start);

        ip_assert(pkt->ipstart <= pkt->start);
        clone->ipstart = ipcom_conf_max_link_hdr_size;
        clone->start   = clone->ipstart + pkt->start - pkt->ipstart;
        clone->end     = clone->start + pkt->end - pkt->start;
        clone_buf = &clone->data[clone->ipstart - link_hdr];
        data_start -= link_hdr;
        data_len += link_hdr;
    }

    /* Copy packet meta info */
    ipcom_pkt_copy_info(clone, pkt);
    /* Copy packet payload */
    ipcom_memcpy(clone_buf, &pkt->data[data_start], data_len);

    IPNET_DEBUG_LINE(ipnet_pkt_check(clone));

    return clone;
}


/*
 *===========================================================================
 *                        ipnet_pkt_report_wrapper
 *===========================================================================
 * Description: Wrapper that extracts the report function from the packet,
 *              call the report functio and then frees the packet.
 * Parameters:  si - packet report soft IRQ.
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_report_wrapper(Ipnet_pkt_report_softirq *si)
{
    Ipnet_softirq_handler do_report;
    Ipcom_pkt            *pkt = si->pkt;

    ipcom_slab_free(ipnet->slabs.kioevent_softirq, si);
    do_report = IPNET_PKT_GET_REPORT_FUNC(pkt);
    do_report(pkt);
    ipcom_pkt_free(pkt);
}


/*
 *===========================================================================
 *                    ipnet_softirq_run
 *===========================================================================
 * Description: Runs all pending soft interrupt(s).
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_softirq_run(void)
{
    Ipnet_softirq *softirq_head;
    Ipnet_softirq *softirq;

    IPNET_DATA_LOCK(ipnet->softirq_lock);

    softirq = ipnet->softirq_head;
    if (softirq != IP_NULL)
    {
        ipnet->softirq_head    = IP_NULL;
        ipnet->softirq_tail    = IP_NULL;
        ipnet->softirq_pending = IP_FALSE;

        for (;;)
        {
            Ipnet_softirq_handler handler;
            void                 *cookie;

            softirq_head = softirq->next;

            softirq->pending = IP_FALSE;
            softirq->next    = IP_NULL;
            handler = softirq->handler;
            cookie = softirq->cookie;
            IPNET_DATA_UNLOCK(ipnet->softirq_lock);

            handler(cookie);

            /*
             * Note that we do not guarantee that ipnet->softirq_head
             * is IP_NULL when we exit this loop.  We only service the
             * softirq's that were pending at the time we took the
             * softirq_lock when we entered the function. More may
             * have arrived since then. However, no more processes
             * can have been added to the wait queue.
             *
             * Note that this algorithm has only one locking
             * cycle of ipnet->softirq_lock, if there is only one
             * softirq.
             */

            if ((softirq = softirq_head) == IP_NULL)
                break;

            IPNET_DATA_LOCK(ipnet->softirq_lock);
        }
    }
    else
        IPNET_DATA_UNLOCK(ipnet->softirq_lock);
}


/*
 *===========================================================================
 *                    ipnet_handle_statistics_sig
 *===========================================================================
 * Description: Retrieves IPNET protocol statistics
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_handle_statistics_sig(Ipnet_sig *sig)
{
    sig->d.stats.sig_errno = 0;

#ifdef IPCOM_USE_MIB2
    switch (sig->d.stats.proto)
    {
    case IP_IPPROTO_IP:
#ifdef IPCOM_USE_INET
        sig->d.stats.p.ip.in_receives       = ipnet->mib2.ipInReceives;
        sig->d.stats.p.ip.in_hdr_errors     = ipnet->mib2.ipInHdrErrors;
        sig->d.stats.p.ip.in_addr_errors    = ipnet->mib2.ipInAddrErrors;
        sig->d.stats.p.ip.forw_datagrams    = ipnet->mib2.ipForwDatagrams;
        sig->d.stats.p.ip.in_unknown_protos = ipnet->mib2.ipInUnknownProtos;
        sig->d.stats.p.ip.in_discards       = ipnet->mib2.ipInDiscards;
        sig->d.stats.p.ip.in_delivers       = ipnet->mib2.ipInDelivers;
        sig->d.stats.p.ip.out_requests      = ipnet->mib2.ipOutRequests;
        sig->d.stats.p.ip.out_discards      = ipnet->mib2.ipOutDiscards;
        sig->d.stats.p.ip.out_no_routes     = ipnet->mib2.ipOutNoRoutes;
        sig->d.stats.p.ip.reasm_timeout     = ipnet->mib2.ipReasmFails;
        sig->d.stats.p.ip.reasm_reqds       = ipnet->mib2.ipReasmReqds;
        sig->d.stats.p.ip.reasm_oks         = ipnet->mib2.ipReasmOKs;
        sig->d.stats.p.ip.reasm_fails       = ipnet->mib2.ipReasmFails;
        sig->d.stats.p.ip.frag_oks          = ipnet->mib2.ipFragOKs;
        sig->d.stats.p.ip.frag_fails        = ipnet->mib2.ipFragFails;
        sig->d.stats.p.ip.frag_creates      = ipnet->mib2.ipFragCreates;
#else
        sig->d.stats.sig_errno = IP_ERRNO_EPROTONOSUPPORT;
#endif
        break;
    case IP_IPPROTO_IPV6:
#if defined(IPCOM_USE_INET6) && (defined(IPNET_STATISTICS) || defined(IPNET_DEBUG))
        sig->d.stats.p.ip6.ip6_input               = ipnet->stats.ip6_input;
        sig->d.stats.p.ip6.ip6_input_err           = ipnet->stats.ip6_input_err;
        sig->d.stats.p.ip6.ip6_input_hdr_trunc     = ipnet->stats.ip6_input_hdr_trunc;
        sig->d.stats.p.ip6.ip6_input_inval_len     = ipnet->stats.ip6_input_inval_len;
        sig->d.stats.p.ip6.ip6_input_inval_exthdr  = ipnet->stats.ip6_input_inval_exthdr;
        sig->d.stats.p.ip6.ip6_input_not_to_me     = ipnet->stats.ip6_input_not_to_me;
        sig->d.stats.p.ip6.ip6_input_too_big       = ipnet->stats.ip6_input_too_big;
        sig->d.stats.p.ip6.ip6_input_time_exceeded = ipnet->stats.ip6_input_time_exceeded;
        sig->d.stats.p.ip6.ip6_input_too_short     = ipnet->stats.ip6_input_too_short;
        sig->d.stats.p.ip6.ip6_input_tcp_multicast = ipnet->stats.ip6_input_tcp_multicast;
        sig->d.stats.p.ip6.ip6_input_prot_err      = ipnet->stats.ip6_input_prot_err;
        sig->d.stats.p.ip6.ip6_input_hdr_no_next   = ipnet->stats.ip6_input_hdr_no_next;
        sig->d.stats.p.ip6.ip6_input_raw           = ipnet->stats.ip6_input_raw;
        sig->d.stats.p.ip6.ip6_output              = ipnet->stats.ip6_output;
        sig->d.stats.p.ip6.ip6_output_err          = ipnet->stats.ip6_output_err;
        sig->d.stats.p.ip6.ip6_output_blackhole    = ipnet->stats.ip6_output_blackhole;
        sig->d.stats.p.ip6.ip6_output_hdr_space    = ipnet->stats.ip6_output_hdr_space;
        sig->d.stats.p.ip6.ip6_output_noroute      = ipnet->stats.ip6_output_noroute;
        sig->d.stats.p.ip6.ip6_output_nomem        = ipnet->stats.ip6_output_nomem;
        sig->d.stats.p.ip6.ip6_output_enobufs      = ipnet->stats.ip6_output_enobufs;
        sig->d.stats.p.ip6.ip6_output_err_probe    = ipnet->stats.ip6_output_err_probe;
        sig->d.stats.p.ip6.ip6_output_link_err     = ipnet->stats.ip6_output_link_err;
#else
        sig->d.stats.sig_errno = IP_ERRNO_EPROTONOSUPPORT;
#endif
        break;
    case IP_IPPROTO_ICMP:
#ifdef IPCOM_USE_INET
        sig->d.stats.p.icmp.in_msgs            = ipnet->mib2.icmpInMsgs;
        sig->d.stats.p.icmp.in_errors          = ipnet->mib2.icmpInErrors;
        sig->d.stats.p.icmp.in_dest_unreachs   = ipnet->mib2.icmpInDestUnreachs;
        sig->d.stats.p.icmp.in_time_excds      = ipnet->mib2.icmpInTimeExcds;
        sig->d.stats.p.icmp.in_parm_probs      = ipnet->mib2.icmpInParmProbs;
        sig->d.stats.p.icmp.in_src_quenchs     = ipnet->mib2.icmpInSrcQuenchs;
        sig->d.stats.p.icmp.in_redirects       = ipnet->mib2.icmpInRedirects;
        sig->d.stats.p.icmp.in_echos           = ipnet->mib2.icmpInEchos;
        sig->d.stats.p.icmp.in_echo_reps       = ipnet->mib2.icmpInEchoReps;
        sig->d.stats.p.icmp.in_timestamps      = ipnet->mib2.icmpInTimestamps;
        sig->d.stats.p.icmp.in_timestamp_reps  = ipnet->mib2.icmpInTimestampReps;
        sig->d.stats.p.icmp.in_addr_masks      = ipnet->mib2.icmpInAddrMasks;
        sig->d.stats.p.icmp.in_addr_mask_reps  = ipnet->mib2.icmpInAddrMaskReps;
        sig->d.stats.p.icmp.out_msgs           = ipnet->mib2.icmpOutMsgs;
        sig->d.stats.p.icmp.out_errors         = ipnet->mib2.icmpOutErrors;
        sig->d.stats.p.icmp.out_dest_unreachs  = ipnet->mib2.icmpOutDestUnreachs;
        sig->d.stats.p.icmp.out_time_excds     = ipnet->mib2.icmpOutTimeExcds;
        sig->d.stats.p.icmp.out_parm_probs     = ipnet->mib2.icmpOutParmProbs;
        sig->d.stats.p.icmp.out_src_quenchs    = ipnet->mib2.icmpOutSrcQuenchs;
        sig->d.stats.p.icmp.out_redirects      = ipnet->mib2.icmpOutRedirects;
        sig->d.stats.p.icmp.out_echos          = ipnet->mib2.icmpOutEchos;
        sig->d.stats.p.icmp.out_echo_reps      = ipnet->mib2.icmpOutEchoReps;
        sig->d.stats.p.icmp.out_timestamps     = ipnet->mib2.icmpOutTimestamps;
        sig->d.stats.p.icmp.out_timestamp_reps = ipnet->mib2.icmpOutTimestampReps;
        sig->d.stats.p.icmp.out_addr_masks     = ipnet->mib2.icmpOutAddrMasks;
        sig->d.stats.p.icmp.out_addr_mask_reps = ipnet->mib2.icmpOutAddrMaskReps;
#else
        sig->d.stats.sig_errno = IP_ERRNO_EPROTONOSUPPORT;
#endif
        break;
    case IP_IPPROTO_ICMPV6:
#if defined(IPCOM_USE_INET6) && (defined(IPNET_STATISTICS) || defined(IPNET_DEBUG))
        sig->d.stats.p.icmp6.icmp6_input             = ipnet->stats.icmp6_input;
        sig->d.stats.p.icmp6.icmp6_input_err         = ipnet->stats.icmp6_input_err;
        sig->d.stats.p.icmp6.icmp6_input_badchksum   = ipnet->stats.icmp6_input_badchksum;
        sig->d.stats.p.icmp6.icmp6_input_nomem       = ipnet->stats.icmp6_input_nomem;
        sig->d.stats.p.icmp6.icmp6_send_nomem        = ipnet->stats.icmp6_send_nomem;
#else
        sig->d.stats.sig_errno = IP_ERRNO_EPROTONOSUPPORT;
#endif
        break;
    case IP_IPPROTO_TCP:
#ifdef IPTCP
        sig->d.stats.p.tcp.active_opens  = ipnet->mib2.tcpActiveOpens;
        sig->d.stats.p.tcp.passive_opens = ipnet->mib2.tcpPassiveOpens;
        sig->d.stats.p.tcp.attempt_fails = ipnet->mib2.tcpAttemptFails;
        sig->d.stats.p.tcp.estab_resets  = ipnet->mib2.tcpEstabResets;
        sig->d.stats.p.tcp.curr_estab    = ipnet_stats_tcp_established();
        sig->d.stats.p.tcp.in_segs       = ipnet->mib2.tcpInSegs;
        sig->d.stats.p.tcp.out_segs      = ipnet->mib2.tcpOutSegs;
        sig->d.stats.p.tcp.retrans_segs  = ipnet->mib2.tcpRetransSegs;
        sig->d.stats.p.tcp.in_errs       = ipnet->mib2.tcpInErrs;
        sig->d.stats.p.tcp.out_rsts      = ipnet->mib2.tcpOutRsts;
#else
        sig->d.stats.sig_errno = IP_ERRNO_EPROTONOSUPPORT;
#endif
        break;

    case IP_IPPROTO_UDP:
        sig->d.stats.p.udp.in_datagrams  = ipnet->mib2.udpInDatagrams;
        sig->d.stats.p.udp.no_ports      = ipnet->mib2.udpNoPorts;
        sig->d.stats.p.udp.in_errors     = ipnet->mib2.udpInErrors;
        sig->d.stats.p.udp.out_datagrams = ipnet->mib2.udpOutDatagrams;
        break;

    case IP_IPPROTO_SCTP:
#if defined(IPSCTP) && defined(IPSCTP_MIB)
        {
            Ipsctp_mib_stats *sctp_stats = IP_NULL;
            ipsctp_mib_stats_get(&sctp_stats);
            if (sctp_stats)
            {
                sig->d.stats.p.sctp.sctp_Curr_Estab          = sctp_stats->sctp_Curr_Estab        ;
                sig->d.stats.p.sctp.sctp_Active_Estabs       = sctp_stats->sctp_Active_Estabs     ;
                sig->d.stats.p.sctp.sctp_Passive_Estabs      = sctp_stats->sctp_Passive_Estabs    ;
                sig->d.stats.p.sctp.sctp_Aborteds            = sctp_stats->sctp_Aborteds          ;
                sig->d.stats.p.sctp.sctp_Shutdowns           = sctp_stats->sctp_Shutdowns         ;
                sig->d.stats.p.sctp.sctp_OutOfBlues          = sctp_stats->sctp_OutOfBlues        ;
                sig->d.stats.p.sctp.sctp_Checksum_Errors     = sctp_stats->sctp_Checksum_Errors   ;
                sig->d.stats.p.sctp.sctp_Out_Ctrl_Chunks     = sctp_stats->sctp_Out_Ctrl_Chunks   ;
                sig->d.stats.p.sctp.sctp_Out_Order_Chunks    = sctp_stats->sctp_Out_Order_Chunks  ;
                sig->d.stats.p.sctp.sctp_Out_Unorder_Chunks  = sctp_stats->sctp_Out_Unorder_Chunks;
                sig->d.stats.p.sctp.sctp_In_Ctrl_Chunks      = sctp_stats->sctp_In_Ctrl_Chunks    ;
                sig->d.stats.p.sctp.sctp_In_Order_Chunks     = sctp_stats->sctp_In_Order_Chunks   ;
                sig->d.stats.p.sctp.sctp_In_Unorder_Chunks   = sctp_stats->sctp_In_Unorder_Chunks ;
                sig->d.stats.p.sctp.sctp_Frag_UsrMsgs        = sctp_stats->sctp_Frag_UsrMsgs      ;
                sig->d.stats.p.sctp.sctp_Reasm_UsrMsgs       = sctp_stats->sctp_Reasm_UsrMsgs     ;
                sig->d.stats.p.sctp.sctp_Out_SCTP_Packs      = sctp_stats->sctp_Out_SCTP_Packs    ;
                sig->d.stats.p.sctp.sctp_In_SCTP_Packs       = sctp_stats->sctp_In_SCTP_Packs     ;
                sig->d.stats.p.sctp.sctp_Discontinuity_Time  = sctp_stats->sctp_Discontinuity_Time;
            }

        }
#else
        sig->d.stats.sig_errno = IP_ERRNO_EPROTONOSUPPORT;
#endif
        break;

    default:
        sig->d.stats.sig_errno = IP_ERRNO_ENOPROTOOPT;
        break;
    }
#else
    sig->d.stats.sig_errno = IP_ERRNO_ENOENT;
#endif
}

/*=========================================================================== 
 *                             ipnet_sig_free   
 *===========================================================================   
 * Description: Frees a signal.   
 * Parameters:  sig - signal to free   
 * Returns:   
 *   
 */   
  IP_GLOBAL void   
  ipnet_sig_free(Ipnet_sig *sig)   
  {   
      if (sig->free)   
          /* Custom free function */   
          sig->free(sig->freesig);   
      else   
          ipcom_free(sig);   
  }   

/*
 *===========================================================================
 *                          ipnet_handle_signal
 *===========================================================================
 * Description: Handles one signal.
 * Parameters:  sig - signal to take action on
 * Returns:
 *
 */
IP_INLINE void
ipnet_handle_signal(Ipnet_sig *sig)
{
    void           *msg_control;
    int             ret = 0;
    Ip_socklen_t    namelen;

    ipnet->effective_pid = sig->sender_pid;
#if IPCOM_VR_MAX > 1
    ipnet->effective_vr  = sig->sender_vr;
#endif

#ifdef IPIPSEC2
#if defined(IPIPSEC_USE_DAEMONS) || defined(IPHWCRYPTO)
    /* Check IPSEC signal first if compiled in */
    if (sig->sig_type == IPNET_SIG_IPSEC)
    {
        ip_assert(sig != IP_NULL);
        ipipsec_process_signal(sig);
        return;
    }
#endif
#endif

    /* Check if the sender of this message still regards it as
       valid.  If not, just free it.
    */
    if (ipcom_atomic_get(&sig->garbage_tag))
    {
        ipnet_sig_free(sig);
        return;
    }

    /* Check for sendmsg first since that is the most performance
       critical signal and also the most common signal in a busy
       system */
    if (sig->sig_type == IPNET_SIG_SENDMSG)
    {
        Ipnet_sig *rsig = sig->d.sendmsg.rsig;

        IPCOM_PKT_TRACE(sig->d.sendmsg.pkt, IPCOM_PKT_ID_RECV_PIPE);

        /* Save ancillary data pointer since ipsec may reuse
           IPNET_PKT_INFO_SIG and overwrite the original pointer
           that is freed below */
        msg_control = sig->d.sendmsg.msg.msg_control;
        ret = ipnet_sys_sendmsg(sig->d.sendmsg.fd,
                                &sig->d.sendmsg.msg,
                                sig->d.sendmsg.pkt);
        if (msg_control != IP_NULL)
            ipcom_free(msg_control);

        if (rsig == IP_NULL)
            return;
        sig = rsig;
        sig->sig_type = IPNET_SIG_SENDMSG;
    }


    /* Initialize the sender and receiver PID in case this signal is
       used as response */
    sig->receiver_pid = ipnet->effective_pid;
    sig->sender_pid   = ipnet->task.pid;

    /* handlers that must send a response must end with "break;"
       and handler that does need to send a reqponse must end
       with "continue;" */
    switch (sig->sig_type)
    {
    case IPNET_SIG_ACCEPT:
        ret = ipnet_sys_accept(sig->d.accept.fd,
                               (sig->d.accept.fromlen == 0
                                ? IP_NULL
                                : (struct Ip_sockaddr *) &sig->d.accept.from),
                               (sig->d.accept.fromlen == 0
                                ? IP_NULL
                                : &sig->d.accept.fromlen));
        break;

    case IPNET_SIG_BIND:
        ret = ipnet_sys_bind(sig->d.bind.fd,
                             (sig->d.bind.namelen == 0
                              ? IP_NULL
                              : (struct Ip_sockaddr *) &sig->d.bind.name),
                             sig->d.bind.namelen);
        break;

    case IPNET_SIG_CONNECT:
        ret = ipnet_sys_connect(sig->d.connect.fd,
                                (sig->d.connect.tolen == 0
                                 ? IP_NULL
                                 : (struct Ip_sockaddr *) &sig->d.connect.to),
                                sig->d.connect.tolen);
        break;

    case IPNET_SIG_CLOSE:
        ret = ipnet_sys_socketclose(sig->d.close);
        break;

    case IPNET_SIG_GETANCILLARYDATA:
        ret = ipnet_sys_get_ancillary_data(&sig->d.ancillary_data);
        break;

    case IPNET_SIG_GETNAME:
        namelen = sizeof(sig->d.getname.name);
        ret = ipnet_sys_getname(sig->d.getname.fd,
                                (struct Ip_sockaddr *) &sig->d.getname.name,
                                &namelen,
                                sig->d.getname.get_local_name);
        break;

    case IPNET_SIG_GETSOCKOPT:
        ret = ipnet_sys_getsockopt(sig->d.sockopt.fd,
                                   sig->d.sockopt.level,
                                   sig->d.sockopt.optname,
                                   sig->d.sockopt.optval,
                                   &sig->d.sockopt.optlen);
        break;

    case IPNET_SIG_IF_ATTACH:
        ret = ipnet_sys_if_attach(sig->d.netif.netif);

        if (ret > 0 && sig->d.netif.locked)
            IPNET_IF_LOCK(((Ipnet_netif*)sig->d.netif.netif));
        break;

    case IPNET_SIG_IF_DETACH:
        ret = ipnet_sys_if_detach(sig->d.netif.netif);
        break;

    case IPNET_SIG_IF_CHANGENAME:
        ipnet_sys_if_changename(sig->d.netif.netif,
                                sig->d.netif.ifname);
        break;

    case IPNET_SIG_IF_CHANGEMTU:
        ipnet_sys_if_changemtu(sig->d.netif.netif,
                               sig->d.netif.mtu);
        break;

    case IPNET_SIG_IF_CHANGELINKADDR:
        ipnet_sys_if_changelinkaddr(sig->d.netif.netif,
                                    sig->d.netif.link_addr,
                                    sig->d.netif.size);
        break;

    case IPNET_SIG_IF_FREE:
        ret = ipnet_sys_if_free(sig->d.netif.netif);
        break;

    case IPNET_SIG_IF_MALLOC:
        sig->d.netif.netif = ipnet_sys_if_malloc(sig->d.netif.type);
        break;

    case IPNET_SIG_IF_NAMETONETIF:
        ipnet_sys_if_nametonetif(sig);
        break;

    case IPNET_SIG_IF_INDEXTONAME:
    case IPNET_SIG_IF_NAMETOINDEX:
        ipnet_sys_if_indexname(sig);
        break;

    case IPNET_SIG_IOCTL:
        ret = ipnet_sys_socketioctl(sig->d.ioctl.fd,
                                    sig->d.ioctl.request,
                                    sig->d.ioctl.argp);
        break;

    case IPNET_SIG_LISTEN:
        ret = ipnet_sys_listen(sig->d.listen.fd,
                               sig->d.listen.backlog);
        break;

    case IPNET_SIG_POLL:
        ipnet_sys_poll(sig);
        return;

    case IPNET_SIG_POLL_ASYNC:
        ipnet_sys_poll_async(sig);
        return;

    case IPNET_SIG_POLL_DONE:
        ipnet_sys_poll_done(sig);
        return;

    case IPNET_SIG_SENDMSG:
        break;

    case IPNET_SIG_SETSOCKOPT:
        ret = ipnet_sys_setsockopt(sig->d.sockopt.fd,
                                   sig->d.sockopt.level,
                                   sig->d.sockopt.optname,
                                   sig->d.sockopt.optval,
                                   sig->d.sockopt.optlen);
        break;

    case IPNET_SIG_SHUTDOWN:
        ret = ipnet_sys_shutdown(sig->d.shutdown.fd,
                                 sig->d.shutdown.how,
                                 IP_TRUE);
        break;

    case IPNET_SIG_SOCKET:
        ret = ipnet_sys_socket(sig->d.socket.domain,
                               sig->d.socket.type,
                               sig->d.socket.protocol,
                               IP_TRUE);
        break;

    case IPNET_SIG_SOCKETFREE_ASYNC:
        ip_assert(ipcom_atomic_get(&ipnet->sockets[sig->d.close]->snd_bytes) == 0);
        ipnet_sock_free(ipnet->sockets[sig->d.close], IP_TRUE);
        return;

    case IPNET_SIG_SOCKETFREE:
        if (ipcom_atomic_get(&ipnet->sockets[sig->d.close]->snd_bytes) == 0)
            ipnet_sock_free(ipnet->sockets[sig->d.close], IP_TRUE);
        ret = 0;
        break;

    case IPNET_SIG_SUSPEND:
        ipnet_suspend(sig);
        return;

    case IPNET_SIG_SYSCTL:
        ret = ipnet_sys_sysctl(&sig->d.sysctl);
        break;

    case IPNET_SIG_WAKEUP:
        return;

#ifdef IPTCP
    case IPNET_SIG_WINDOW_UPDATE:
        iptcp_window_update(&sig->d.window_update);
        ipcom_slab_free(ipnet->slabs.poll, sig);
        return;
#endif
#ifdef IPSCTP
    case IPNET_SIG_SCTP_SEND_INIT:
        ipsctp_sys_send_init(sig->d.sctp_send_init.fd,
                             sig->d.sctp_send_init.vtag);
        ipcom_slab_free(ipnet->slabs.poll, sig);
        return;

    case IPNET_SIG_SCTP_SET_STATE:
        ipsctp_set_state(&sig->d.sctp_set_state);
        ipcom_slab_free(ipnet->slabs.poll, sig);
        return;

    case IPNET_SIG_SCTP_WINDOW_UPDATE:
        ipsctp_window_update(&sig->d.sctp_wu);
        ipcom_slab_free(ipnet->slabs.poll, sig);
        return;
#endif

    case IPNET_SIG_PORT_SPECIFIC:
#ifdef IPCOM_USE_PORT_SPECIFIC_SIGNALS
        ret = ipcom_handle_port_specific_sig(sig);
        if (ret == IPCOM_PORT_SIG_DO_REPLY)
            break;
#endif
        return;

#ifdef IPIFPROXY
    case IPNET_SIG_CTRL_SOCK_RX:
        ipifproxy_ctrl_sock_rx(sig);
        return;

    case IPNET_SIG_DATA_SOCK_RX:
        ipifproxy_data_sock_rx(sig);
        return;
#endif

    case IPNET_SIG_STATS:
        ipnet_handle_statistics_sig(sig);
        ipcom_pipe_send(sig->response_pipe, sig);
        return;

    default:

        IP_PANIC();
        break;
    }

    sig->reply.return_code = ret;
    if (sig->reply.return_code < 0)
    {
        sig->reply.sock_errno  = -ret;
        sig->reply.return_code = IP_SOCKERR;
    }
    if (sig->response_pipe)
        ipcom_pipe_send(sig->response_pipe, sig);
}



#ifdef IPCOM_USE_JOB_QUEUE
/*
 *===========================================================================
 *                           ipnet_pkt_rx_action
 *===========================================================================
 * Description: Inputs all pending packets into the stack.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_rx_action(void)
{
    int        key;
    Ipcom_pkt *head;
    Ipcom_pkt *pkt;

    /* Unhook all pending packets from the input queue */
    IPNET_MIN_LOCK(ipnet->inq.lock, key);
    head = ipnet->inq.head;
    ipnet->inq.head  = IP_NULL;
    ipnet->inq.count = 0;
    IPNET_MIN_UNLOCK(ipnet->inq.lock, key);

    while (IP_NULL != (pkt = head))
    {
        IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_INQ);
        ip_assert(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_ALLOC));

        head = pkt->next;

        ipnet_pkt_input(IP_NULL, pkt);
    }
}

/*
 *===========================================================================
 *                  ipnet_process_signals
 *===========================================================================
 * Description: Processes all pending signals. This is done in the context
 *              of the task/process that runs the job queue.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_process_signals(void)
{
    Ipnet_sig *sig;
    unsigned   budget = IPCOM_PIPE_MSG_LIMIT_MANY;

    for (;;)
    {
        if (IP_UNLIKELY(budget-- == 0))
        {
            /* Not allowed to process any more signal at this time
               since it could starve other parts of the system. Let's
               reschedule a new process signal job. */
            ipcom_job_queue_pipe_schedule(ipnet->task.pipe);
            break;
        }

        if (ipcom_pipe_try_recv(ipnet->task.pipe, (void **) &sig) != IPCOM_SUCCESS)
            break;

        ipnet_handle_signal(sig);
    }
}


#else /* !IPCOM_USE_JOB_QUEUE */


/*
 *===========================================================================
 *                  ipnet_process_signals
 *===========================================================================
 * Description: Processes all pending signals sent to the "ipnetd" task.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_process_signals(void)
{
    Ipnet_sig        *sig;
    Ip_u32            msec;
    struct Ip_timeval tv;

    do
    {
        msec = ipcom_stackd_event(IP_FALSE);
        tv.tv_sec = msec / 1000;
        tv.tv_usec = (msec % 1000) * 1000;
    } while (ipcom_pipe_recv_with_tmo(ipnet->task.pipe,
                                      (void **) &sig,
                                      &tv)
             != IPCOM_SUCCESS);

    do {
        if (ipnet->softirq_pending)
            ipnet_softirq_run();

        ipnet_handle_signal(sig);

    } while (ipcom_pipe_try_recv(ipnet->task.pipe, (void **) &sig) == IPCOM_SUCCESS);
}


/*
 *===========================================================================
 *                        ipcom_stackd_event
 *===========================================================================
 * Description: Process a IPNET event.
 * Parameters:  is_timeout - IP_TRUE if the event was caused by a timeout
 * Returns:     The number of milliseconds until the next timeout triggers.
 *
 */
IP_PUBLIC Ip_s32
ipcom_stackd_event(Ip_bool is_timeout)
{
    int        key;
    Ipcom_pkt *head;
    Ipcom_pkt *pkt;

    ip_assert(ipnet != IP_NULL);

    if (!is_timeout)
        ipnet->task.wakeup_pending = IP_FALSE;

    /* Update the time, this is an expensive operations on some OS:es... */
    ipnet_update_time();

    /* Unhook all pending packets from the input queue */
    IPNET_MIN_LOCK(ipnet->inq.lock, key);
    head = ipnet->inq.head;
    ipnet->inq.head  = IP_NULL;
    ipnet->inq.count = 0;
    IPNET_MIN_UNLOCK(ipnet->inq.lock, key);

    while (IP_NULL != (pkt = head))
    {
        IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_INQ);
        ip_assert(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_ALLOC));

        head = pkt->next;

        /* Handle any pending soft interrupt first */
        if (ipnet->softirq_pending)
            ipnet_softirq_run();

        ipnet_pkt_input(IP_NULL, pkt);
    }

    ipnet_process_pending_timeouts();

    if (ipnet->softirq_pending)
        ipnet_softirq_run();

    return ipnet_timeout_duration_until_next();
}


/*
 *===========================================================================
 *                        ipnetd
 *===========================================================================
 * Description: IPNET deamon process.
 *              Process any packet that was put on the input queue by
 *              ipcom_pkt_input_queue() and dispatch the packet to the correct
 *              link layer input function.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC
IPCOM_PROCESS(ipnetd)
{
    ipcom_proc_init();
    ip_assert(ipnet != IP_NULL);
#if defined(IP_PORT_LKM) && defined(IPNET_IS_ISOLATED)
    {
         Ip_cpu_set_t cpu_set;
         int cpu_i;

         /* Try to reserve CPU */
         IP_CPU_ZERO(&cpu_set);
         cpu_i = ipcom_cpu_reserved_alloc();
         IP_CPU_SET(cpu_i, &cpu_set);

         if (ipcom_proc_cpu_affinity_set(ipcom_getpid(), &cpu_set) != IPCOM_SUCCESS)
         {
             IPCOM_LOG1(WARNING,"Failed to set affinity for ipnetd to cpu \'%d\'\n", cpu_i);
         }
    }
#endif /* IP_PORT_LKM && IPNET_IS_ISOLATED */

    (void)ipcom_stackd_init();
    for (;;)
        ipnet_process_signals();

    /* Unreachable code */
    /* ipcom_proc_exit(); */
}
#endif /* !IPCOM_USE_JOB_QUEUE */


/*
 *===========================================================================
 *                        ipcom_stackd_init
 *===========================================================================
 * Description: Init IPNET deamon process.
 * Parameters:
 * Returns:     The number of milliseconds until the next timeout triggers.
 *              NOTE: This can be zero which means that a timeout is already
 *              pending.
 *
 */
IP_PUBLIC Ip_s32
ipcom_stackd_init(void)
{
    ipnet->task.pid = ipcom_getpid();
    ipnet->task.pipe->owner = ipnet->task.pid;

    ipnet_update_time();

    /* IPNET is ready to send packets */
    IPCOM_LOG2(INFO, "Initial milliseconds timer:%d  seconds timer:%d",
               ipnet->msec_now, ipnet->sec_now);

    ipnet->update_internal_timer = IP_TRUE;

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_init
 *===========================================================================
 * Description: Allocates and initialize memory needed globally IPNET.
 * Parameters:
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_init(void)
{
    int       ret;
    Ip_size_t ipnet_struct_size = sizeof(Ipnet_data);
    Ipcom_pipe_attr attr;

    if (ipnet_conf_cache_bufsiz == 0)
        ipnet_conf_cache_bufsiz = ipcom_cache_line_size();

    if (IP_FD_SETSIZE < ipnet_conf_max_sockets)
    {
        /* Increase the size of IP_FD_SETSIZE */
        IPCOM_LOG2(DEBUG,
                   "IP_FD_SETSIZE (=%d) must be equal to or greater then"
                   " ipnet_conf_max_sockets (=%d)",
                   IP_FD_SETSIZE, ipnet_conf_max_sockets);
        return -IP_ERRNO_ECONFIG;
    }

    /* Add storage for ipnet->sockets */
    ipnet_struct_size += ipnet_conf_max_sockets * sizeof(Ipnet_socket *);

    /* Round up the size to the nearest cache line aligned size */
    ipnet_struct_size = IP_ROUNDUP(ipnet_struct_size, ipnet_conf_cache_bufsiz);

    ipnet = ipcom_calloc(1, ipnet_struct_size + ipnet_conf_cache_bufsiz);
    if (ipnet == IP_NULL)
    {
        IPCOM_LOG1(DEBUG,
                   "Failed to allocate %d bytes with ipcom_calloc()",
                   ipnet_struct_size);
        return -IP_ERRNO_ENOMEM;
    }
    ipnet = IP_PTR_ALIGN(ipnet, ipnet_conf_cache_bufsiz);

#ifdef IPNET_ROUTE_CACHE_ID_IN_SHARED_MEM
    ipnet->route_cache_id_ptr = ipcom_route_cache_id_alloc();
#endif

#ifdef IPNET_USE_PER_CPU_PKT_CACHE
    {
        unsigned nr_cpus = ipcom_num_configured_cpus();

        ipnet->pkt_cache = ipcom_calloc(nr_cpus, sizeof(Ipnet_pkt_cache *));
        while (nr_cpus--)
        {
            ipnet->pkt_cache[nr_cpus]
                = ipcom_calloc(1, IP_MAX(sizeof(Ipnet_pkt_cache),
                                         ipcom_cache_line_size()));
#ifdef IPNET_USE_PER_CPU_PKT_CACHE_SPINLOCK
            if (ipcom_spinlock_create(&ipnet->pkt_cache[nr_cpus]->lock) != IPCOM_SUCCESS)
                IP_PANIC();
#endif
        }
    }
#endif /* IPNET_USE_PER_CPU_PKT_CACHE */

#ifdef IP_PORT_LKM
    ipnet->memory_pool = IP_NULL;
#else
    ipnet->memory_pool
        = ipcom_memory_pool_new("IPNET memory pool", ipnet_conf_memory_limit);
#endif

    ipnet->slabs.bound_socket
        = ipcom_slab_new("IPNET bound socket",
                         0,
                         ipnet_sock_get_bound_socket_size(),
                         0,
                         IP_NULL,
                         IP_NULL,
                         ipnet->memory_pool);
    ipnet->slabs.timer
        = ipcom_slab_new("IPNET timer",
                         IPCOM_SLAB_F_NO_LOCKING,
                         sizeof(Ipnet_timeout),
                         ipnet_conf_cache_bufsiz,
                         IP_NULL,
                         IP_NULL,
                         ipnet->memory_pool);
    ipnet->slabs.kioevent_softirq
        = ipcom_slab_new("IPNET kioevent softirq",
                         0,
                         IP_MAX(sizeof(Ipnet_kioevent_softirq), sizeof(Ipnet_pkt_report_softirq)),
                         0,
                         IP_NULL,
                         IP_NULL,
                         ipnet->memory_pool);
    if (IPNET_MIN_LOCK_CREATE(&ipnet->inq.lock) != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "Failed to create the deamon spinlock");
        return -IP_ERRNO_ENOMEM;
    }

    /* Set up the sockets and sockets_cache, packet pool and netif pointers */
    ipnet->sockets  = (Ipnet_socket**) (ipnet + 1);
#if defined(IPNET_USE_PACKET_POOL)
    if (ipcom_spinlock_create(&ipnet->pkt_pool_lock) != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "Failed to create packet pool spinlock semaphore");
        return -IP_ERRNO_ENOMEM;
    }
#endif /* IPNET_USE_PACKET_POOL */

    ipnet->last_fd = IPNET_SOCKET_STARTFD;

    if ((ret = ipnet_sock_create_lookup_tables()) < 0)
        return ret;

    if (ipcom_spinlock_create(&ipnet->softirq_lock) != IPCOM_SUCCESS
        || ipcom_spinlock_create(&ipnet->sockets_lock) != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "Failed to create required spinlocks");
        return -IP_ERRNO_ENOMEM;
    }

#ifdef IPNET_USE_PACKET_POOL
    ipcom_wait_queue_init(&ipnet->pkt_pool_wq);
#endif

    /* Initialize all static softirq:s */
    ipnet_softirq_init(&ipnet->drain_softirq);
    ipnet_softirq_init(&ipnet->pktavail_softirq);
    ipnet_softirq_init(&ipnet->pktnew_softirq);

    /* Create pipe to the IPNET daemon */
    ipcom_pipe_attr_init(&attr);
    attr.msg_limit = IPNET_IPNETD_PIPE_SIZE;
    attr.handler   = ipnet_process_signals;
#ifdef IPCOM_USE_JOB_QUEUE
    ipcom_job_queue_init(ipnet->memory_pool);
    ipnet->task.pipe   = ipcom_pipe_new(IPCOM_PIPE_JOB_QUEUE, &attr);
    ipnet->inq.job     = ipcom_job_queue_singleton_new(ipnet_pkt_rx_action,
                                                       IPCOM_JOB_PRIO_NORMAL);
    ipnet->timeout_job = ipcom_job_queue_singleton_new(ipnet_process_pending_timeouts,
                                                       IPCOM_JOB_PRIO_LOW);
    ipnet->softirq_job = ipcom_job_queue_singleton_new(ipnet_softirq_run,
                                                       IPCOM_JOB_PRIO_HIGH);
#else
    ipnet->task.pipe   = ipcom_pipe_new(IPCOM_PIPE_GENERIC, &attr);
#endif
    ipnet->task.rpipe  = ipcom_pipe_new(IPCOM_PIPE_IPNET, IP_NULL);

    if (ipnet->task.pipe == IPCOM_PIPE_INVALID
        || ipnet->task.rpipe == IPCOM_PIPE_INVALID)
    {
        IP_PANIC();
        return IPCOM_ERR_FAILED;
    }

#ifndef IPCOM_USE_JOB_QUEUE
    ipnet->task.wakeup_sig = (Ipnet_sig *) ipnet->task.wakeup_sig_buf;
    ipnet->task.wakeup_sig->sig_type = IPNET_SIG_WAKEUP;
    ipcom_atomic_set(&ipnet->task.wakeup_sig->garbage_tag, 0);
#endif

    /* Use this task as the IPNET daemon during startup, it is OK
       since no other task is using the stack at this stage */
    ipnet->task.pid = ipcom_getpid();

    /* Create the timeout queue */
    ipnet->timeouts = ipcom_pqueue_new((Ipcom_pqueue_cmp_func) ipnet_timeout_pq_cmp,
                                       (Ipcom_pqueue_store_index) ipnet_timeout_pq_store_index);
    if (ipnet->timeouts == IP_NULL)
        goto errout;

    /* Calculate max link header size. */
    ipcom_conf_max_link_hdr_size = 4;    /* Mandatory 32-bit cookie. */
#ifdef IPNET_USE_NAT_PT
    ipcom_conf_max_link_hdr_size += IPNET_IP6_HDR_SIZE - IPNET_IP_HDR_SIZE;
#endif /* IPNET_USE_NAT_PT */
#ifdef IPNET_USE_TUNNEL
#ifdef IPCOM_USE_INET6
    ipcom_conf_max_link_hdr_size += IPNET_IP6_HDR_SIZE;
#else
    ipcom_conf_max_link_hdr_size += IPNET_IP_HDR_SIZE;
#endif
#endif /* IPNET_USE_TUNNEL */
#ifdef IPNET_USE_GRE
    ipcom_conf_max_link_hdr_size += 8;   /* Maximum sized GRE header (with md5). */
#endif
#if defined(IPMIPHA) && defined(IPCOM_USE_INET)
    ipcom_conf_max_link_hdr_size += IPNET_IP_HDR_SIZE; /* for broadcasts. */
#endif
#if defined(IPNET_USE_WLAN)
    ipcom_conf_max_link_hdr_size += 30;  /*!!NIKLAS FIXA */
#elif defined(IPCOM_USE_ETHERNET) && !defined(IPPPP)
    ipcom_conf_max_link_hdr_size += 14;  /* Ethernet header. */
#elif defined(IPCOM_USE_ETHERNET) && defined(IPPPP)
    ipcom_conf_max_link_hdr_size += 22;  /* PPPoE hdr + Ethernet */
#elif !defined(IPCOM_USE_ETHERNET) && defined(IPPPP)
    ipcom_conf_max_link_hdr_size += 4;  /* PPP hdr */
#endif

    IPCOM_LOG1(DEBUG,
               "ipcom_conf_max_link_hdr_size set to %d",
               (int)ipcom_conf_max_link_hdr_size);

    if ((ret = ipnet_usr_sock_init()) < 0)
        goto errout;
    IPCOM_LOG0(DEBUG, "Signal subsystem successfully initialized");

    if ((ret = ipnet_if_init()) < 0)
        goto errout;
    IPCOM_LOG0(DEBUG, "Network interfaces framework successfully initialized");

#ifdef IPNET_USE_PACKET_POOL
    /* Initialize the packet pool */
    if ((ret = ipnet_pkt_pool_init()) < 0)
        goto errout;
    IPCOM_LOG0(DEBUG, "Packet pool was successfully initialized");
#endif

    /* Initialize all modules */
#ifdef IPMPLS
    if ((ret = ipnet_mpls_init()) < 0)
        goto errout;
    IPCOM_LOG0(DEBUG, "IPNET MPLS module was successfully initialized");
#endif

#ifdef IPCOM_USE_INET
    if ((ret = ipnet_ip4_init()) < 0)
        goto errout;
    IPCOM_LOG0(DEBUG, "IPv4 module was successfully initialized");
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    if ((ret = ipnet_ip6_init()) < 0)
        goto errout;
    IPCOM_LOG0(DEBUG, "IPv6 module was successfully initialized");
#endif /* IPCOM_USE_INET6 */

    if ((ret = ipnet_pkt_queue_init()) < 0)
        goto errout;
    IPCOM_LOG0(DEBUG, "Pkt queue module was successfully initialized");

    /* Create the default route table */
    if ((ret = ipnet_route_add_table(0, IPCOM_ROUTE_TABLE_DEFAULT, IP_NULL, IP_NULL)) < 0)
        goto errout;
    IPCOM_LOG0(DEBUG, "The default route was successfully created");

    ip_assert(ipnet->sockets[0] == IP_NULL);

#ifdef IPNET_DEBUG
    ipnet_timeout_to_string((Ipnet_timeout_handler) ipnet_update_time_tmo,
                            (Ipnet_timeout_to_string_f) ipnet_update_time_tmo_to_string);
#endif

    /* Register socket domain/type/protocols */
    ret = ipnet_sock_udp_register();
    if (ret < 0)
        goto errout;

#ifdef IPCOM_USE_INET
    ret = ipnet_sock_ip4_register();
    if (ret < 0)
        goto errout;
#endif

#ifdef IPCOM_USE_INET6
    ret = ipnet_sock_ip6_register();
    if (ret < 0)
        goto errout;
#endif

#ifdef IPTCP
    ret = ipnet_sock_tcp_register();
    if (ret < 0)
        goto errout;
#endif

#ifdef IPSCTP
    ret = ipnet_sock_sctp_register();
    if (ret < 0)
        goto errout;
#endif
#ifdef IPNET_USE_ROUTESOCK
    ret = ipnet_sock_route_register();
    if (ret < 0)
        goto errout;
#endif

#if defined(IPNET_USE_NETLINKSOCK)
    ret = ipnet_sock_netlink_register();
    if (ret < 0)
        goto errout;
#endif

#ifdef IPCOM_USE_PACKET
    ret = ipnet_sock_packet_register();
    if (ret < 0)
        goto errout;
#endif

#ifdef IPIPSEC2
    ret = ipnet_sock_key_register();
    if (ret < 0)
        goto errout;
#endif

#ifdef IPNET_USE_SOCKDEV
    ret = ipnet_sock_sockdev_register();
    if (ret < 0)
        goto errout;
#endif

#if defined(IP_PORT_VXWORKS) && defined(IPCOM_USE_SOCK_COMPAT)
    ret =  ipcom_windnet_socklib_init();
    if (ret < 0)
        goto errout;
#endif

    /* The socket slab must be created here since the size of the
       largest socket type is not known until all socket types has
       been registered */
    ipnet->slabs.socket
        = ipcom_slab_new("IPNET socket",
                         IPCOM_SLAB_F_ZERO,
#if defined(IP_PORT_VXWORKS) && defined(IPCOM_USE_SOCK_COMPAT)
                         ipnet->largest_socket_size + ipcom_windnet_socklib_sock_size(),
#else
                         ipnet->largest_socket_size,
#endif
                         ipnet_conf_cache_bufsiz,
                         IP_NULL,
                         IP_NULL,
                         ipnet->memory_pool);

#ifdef IPNET_DEBUG
    IPCOM_LOG0(INFO, "Initialization done, debug build");
#else
    IPCOM_LOG0(INFO, "Initialization done, speed build");
#endif

    return 0;

 errout:
    IPCOM_LOG0(WARNING, "Failed to initialize");
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_open
 *===========================================================================
 * Description: Open IPNET and informs that the stack is ready to be used.
 * Parameters:
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_open(void)
{
#ifdef IPNET_IS_ROUTER
    /* Enable IPv4/v6 Routing. */
#ifdef IPCOM_USE_INET
    IP_BIT_SET(ipnet_flags, IPNET_FLAG_IPV4_FORWARD_PKT);
#endif
#ifdef IPCOM_USE_INET6
    IP_BIT_SET(ipnet_flags, IPNET_FLAG_IPV6_FORWARD_PKT);
#endif
#endif /* IPNET_IS_ROUTER */

    /* Initialize MIB2 variables */
#ifdef IPCOM_USE_INET
    IPCOM_MIB2(ipReasmTimeout = ipnet_conf_reassembly_timeout);    /*lint !e665 */
#endif
#ifdef IPTCP
    IPCOM_MIB2(tcpRtoAlgorithm = 4);    /*lint !e665 */     /* Van Jacobson */
    IPCOM_MIB2(tcpRtoMin = iptcp_get_rto(IP_TRUE));    /*lint !e665 */
    IPCOM_MIB2(tcpRtoMax = iptcp_get_rto(IP_FALSE));    /*lint !e665 */
    IPCOM_MIB2(tcpMaxConn = ipnet_conf_max_sockets);    /*lint !e665 */
#endif /* IPTCP */

    IPCOM_LOG0(INFO, "Open was successfully completed");
    return 0;
}


/*
 ****************************************************************************
 * 11                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                      ipnet_stats_tcp_established
 *===========================================================================
 * Description: Find number of TCP sockets in estalished state.
 * Parameters:  none.
 * Returns:     The number of established TCP sockets.
 */
#if defined(IPCOM_USE_MIB2) && defined(IPTCP)
IP_GLOBAL Ip_u32
ipnet_stats_tcp_established(void)
{
    int i;
    Ip_u32 num_established = 0;

    for (i=0; i < ipnet_conf_max_sockets; i++)
    {
        /* Count the number of established TCP sockets */
        if (ipnet->sockets[i] != IP_NULL
            && ipnet->sockets[i]->ipcom.type == IP_SOCK_STREAM
            && ipnet->sockets[i]->proto == IP_IPPROTO_TCP
            && (ipnet->sockets[i]->tcb->state == IPTCP_STATE_ESTABLISHED
                || ipnet->sockets[i]->tcb->state == IPTCP_STATE_CLOSE_WAIT))
        {
            ++num_established;
        }
    }
    return num_established;
}
#endif /* IPCOM_USE_MIB2 && IPTCP */


/*
 *===========================================================================
 *                      ipnet_sysvar_netif_get_as_int_ex
 *===========================================================================
 * Description: Returns a system variable as an integer, the system variable
 *              can be set  for either the specific interface or for the
 *              whole stack. The interface value overrides the stack value.
 * Parameters:  domain - IP_AF_INET or IP_AF_INET6.
 *              netif - The interface to check redirect status for.
 *              name - The base name of the system variable.
 *              default_val - The value to return if this sysvar is not set.
 *              map - A mapping from a text representation to an integer.
 * Returns:     Value of the sysvar.
 */
IP_GLOBAL int
ipnet_sysvar_netif_get_as_int_ex(int domain,
                                 Ipnet_netif *netif,
                                 IP_CONST char *name,
                                 int default_val,
                                 IP_CONST Ipcom_cmd_int_str_map *map)
{
    char *domain_str = (domain == IP_AF_INET ? ".inet." : ".inet6.");
    char *name_buf = ipnet->log_buf;
    int value;

    if (netif != IP_NULL)
    {
        /* Look for interface specific config "ifname.[inet|inet6].SomeVariable" */
        ipcom_strcpy(name_buf, netif->ipcom.name);
        ipcom_strcat(name_buf, domain_str);
        ipcom_strcat(name_buf, name);

        if (ipcom_sysvar_get_descriptive_int0(name_buf, default_val, &value, map) >= 0)
            return value;
    }

    /* Look for global config "ipnet.[inet|inet6].SomeVariable" */
    ipcom_strcpy(name_buf, "ipnet");
    ipcom_strcat(name_buf, domain_str);
    ipcom_strcat(name_buf, name);

    (void)ipcom_sysvar_get_descriptive_int0(name_buf, default_val, &value, map);

    return value;
}


/*
 *===========================================================================
 *                      ipnet_sysvar_netif_get_as_int
 *===========================================================================
 * Description: Returns a system variable as an integer, the system variable
 *              can be set  for either the specific interface or for the
 *              whole stack. The interface value overrides the stack value.
 * Parameters:  domain - IP_AF_INET or IP_AF_INET6.
 *              netif - The interface to check redirect status for.
 *              name - The base name of the system variable.
 *              default_val - The value to return if this sysvar is not set.
 * Returns:     Value of the sysvar.
 */
IP_GLOBAL int
ipnet_sysvar_netif_get_as_int(int domain,
                              Ipnet_netif *netif,
                              IP_CONST char *name,
                              int default_val)
{
    return ipnet_sysvar_netif_get_as_int_ex(domain, netif, name, default_val, IP_NULL);
}


/*
 *===========================================================================
 *                    ipnet_softirq_init
 *===========================================================================
 * Context:     Any context.
 * Description: Prepares an Ipnet_softirq structure for
 *              ipnet_softirq_schedule().
 * Parameters:  softirq - Pointer to a soft interrupt structure.
 *              handler - Function that will handle the soft interrupt.
 *              cookie - Parameter that will be passed to the handler.
 * Returns:
 */
IP_GLOBAL void
ipnet_softirq_init(Ipnet_softirq *softirq)
{
    softirq->next    = IP_NULL;
    softirq->pending = IP_FALSE;
}


/*
 *===========================================================================
 *                    ipnet_softirq_schedule
 *===========================================================================
 * Context:     Any context.
 * Description: Schedules a function to be called as soon as possible.
 *              The function will be called from a non-interrupt context
 *              with interrupt enabled and the IPNET code lock taken.
 * Parameters:  softirq - Pointer to a soft interrupt structure, the handler
 *              must free the memory if it is dynamically allocated.
 * Returns:
 */
IP_GLOBAL void
ipnet_softirq_schedule(Ipnet_softirq *softirq,
                       Ipnet_softirq_handler handler,
                       void *cookie)
{
#ifdef IPCOM_USE_JOB_QUEUE
    Ip_bool schedule_job = IP_FALSE;
#endif

    IPNET_DATA_LOCK(ipnet->softirq_lock);

    if (softirq->pending == IP_FALSE)
    {
        if (ipnet->softirq_tail)
            ipnet->softirq_tail->next = softirq;
        else
            ipnet->softirq_head = softirq;
        ipnet->softirq_tail = softirq;

        ip_assert(softirq->next == IP_NULL);
        softirq->pending = IP_TRUE;
        softirq->handler = handler;
        softirq->cookie  = cookie;
#ifdef IPCOM_USE_JOB_QUEUE
        schedule_job = !ipnet->softirq_pending;
#endif
        ipnet->softirq_pending = IP_TRUE;
    }
    /* else: a softirq is already scheduled for this handle */

    IPNET_DATA_UNLOCK(ipnet->softirq_lock);

#ifdef IPCOM_USE_JOB_QUEUE
    if (schedule_job)
        ipcom_job_queue_schedule_singleton(ipnet->softirq_job);
#endif
}


/*
 *===========================================================================
 *                    ipnet_timeout_schedule
 *===========================================================================
 * Description: Schedules a function that will be called after msec_to_timeout
 *              milliseconds.
 * Parameters:  msec_to_timeout - Milliseconds until the timeout will occur.
 *              handler - The timeout handler or IP_NULL if no action should
 *                        be done when the timer expires.
 *              cookie - The user defined arguemnt to the handler.
 *              ptmo - The location where the timeout handler should be stored
 *                     (needed to cancel the timer) or IP_NULL.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_timeout_schedule(Ip_u32 msec_to_timeout,
                       Ipnet_timeout_handler handler,
                       void *cookie,
                       Ipnet_timeout **ptmo)
{
    Ipnet_timeout *tmo;
    Ip_bool        update_internal_timer;
    Ip_bool        need_timer_resched;

    update_internal_timer = ipnet->update_internal_timer;
    if (IP_UNLIKELY(update_internal_timer))
    {
        ipnet->update_internal_timer = IP_FALSE;
        /* The time update must be done here since ipnet->msec_now needs
           to be correct in this function */
        ipnet_update_time();
    }

    tmo = ipcom_slab_alloc(ipnet->slabs.timer);
    if (IP_UNLIKELY(tmo == IP_NULL))
    {
        IP_PANIC2();
        return -IP_ERRNO_ENOMEM;
    }

    tmo->handler = handler;
    tmo->cookie  = cookie;
    tmo->msec    = ipnet->msec_now + msec_to_timeout;
    tmo->ptmo    = ptmo;
    if (ptmo != IP_NULL)
        *ptmo = tmo;

    (void)ipcom_pqueue_insert(ipnet->timeouts, tmo);

    need_timer_resched = (tmo->pq_index == 1);
    if (need_timer_resched)
#ifdef IPCOM_USE_JOB_QUEUE
        ipcom_job_queue_schedule_singleton_delayed(ipnet->timeout_job,
                                                   ipnet_timeout_duration_until_next());
#else
        /* This is the next event to timeout, wake the IPNET daemon
           so it update its sleeping time */
        IPCOM_STACKD_POST(IP_TRUE);
#endif

    if (IP_UNLIKELY(update_internal_timer))
    {
        int ret;
        ret = ipnet_timeout_schedule(500,
                                     ipnet_update_time_tmo,
                                     IP_NULL,
                                     IP_NULL);
       /* If we ran out of memory, try again next time. */
       if (IP_UNLIKELY(ret != 0))
           ipnet_update_time_tmo(IP_NULL);
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_timeout_cancel
 *===========================================================================
 * Description: Cancels a scheduled timeout. If the timeout has a refptr
 *              set, it will be NULL'ed.
 * Parameters:  tmo - The timeout event to remove.
 *              sync - IP_TRUE = The timeout handler is guaranteed not to
 *                               run on any CPU when this function returns
 *                     IP_FALSE = The timeout handler was either canceled
 *                                or is already running.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_timeout_cancel(Ipnet_timeout *tmo)
{
    if (tmo != IP_NULL && tmo->pq_index != 0)
    {
        (void)ipcom_pqueue_remove(ipnet->timeouts, tmo->pq_index);

        /* If there's a reference pointer, clear it */
        if (tmo->ptmo != IP_NULL)
            *tmo->ptmo = IP_NULL;

        ipcom_slab_free(ipnet->slabs.timer, tmo);
    }
}


/*
 *===========================================================================
 *                    ipnet_timeout_msec_until
 *===========================================================================
 * Description: Milliseconds until this timeout triggers.
 * Parameters:  tmo - Handle to the timeout event.
 * Returns:
 *
 */
IP_GLOBAL Ip_u32
ipnet_timeout_msec_until(Ipnet_timeout *tmo)
{
    return tmo->msec - ipnet->msec_now;
}


/*
 *===========================================================================
 *                    ipnet_sec_now
 *===========================================================================
 * Description: Returns a number that is increased by 1 for each seconds.
 * Parameters:
 * Returns:     A value with second resolution.
 *
 */
IP_GLOBAL Ip_u32
ipnet_sec_now(void)
{
    return ipnet->sec_now;
}


/*
 *===========================================================================
 *                    ipnet_msec_now
 *===========================================================================
 * Description: Returns a number that is increased by 1 for each milliseconds.
 * Parameters:
 * Returns:     A value with millisecond resolution.
 *
 */
IP_GLOBAL Ip_u32
ipnet_msec_now(void)
{
    struct Ip_timeval tv;

    ipcom_microtime(&tv);
#ifdef IP_DEBUG
    {
        static Ip_u32 boot_time = 0;

        if (boot_time == 0)
            boot_time = IPNET_TIMEVAL_TO_MSEC(tv);

        return IPNET_TIMEVAL_TO_MSEC(tv) - boot_time;
    }
#else
    return IPNET_TIMEVAL_TO_MSEC(tv);
#endif
}


/*
 *===========================================================================
 *                    ipnet_is_stack_task
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:   IP_TRUE if current process is an IPNET process.
 *
 */
IP_GLOBAL Ip_bool
ipnet_is_stack_task(void)
{
    return ipcom_getpid() == ipnet->task.pid;
}


/*
 *===========================================================================
 *                    ipnet_pkt_clone
 *===========================================================================
 * Description: Makes a clone of a packet. Everything is cloned except for
 *              the total size that might be different for the clone
 *              (pkt->maxlen >= clone->maxlen)
 * Parameters:  pkt - The packet to clone.
 *              is_ipstart_valid - Set to IP_TRUE if pkt->ipstart is valid
 *              and the caller is NOT processing a layer below IP (i.e.
 *              pkt->start >= pkt->ipstart).
 * Returns:     The clone.
 *
 */
IP_GLOBAL Ipcom_pkt *
ipnet_pkt_clone(Ipcom_pkt *pkt, Ip_bool is_ipstart_valid)
{
    return ipnet_pkt_kclone(pkt, is_ipstart_valid, 0);
}


/*
 *===========================================================================
 *                       ipnet_schedule_pkt_report
 *===========================================================================
 * Description: Schedules generation of a report for the passed packet.
 * Parameters:  pkt - packet to generate report for.
 *              do_report - function that will generate the report.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_schedule_pkt_report(Ipcom_pkt *pkt, Ipnet_softirq_handler do_report)
{
    Ipnet_pkt_report_softirq *si;

    /* Must increase the reference to make sure this packet is valid
       until the softirq handler has been run */
    si = ipcom_slab_alloc(ipnet->slabs.kioevent_softirq);
    if (si)
    {
        IPCOM_PKT_ADD_REF(pkt);
        IPNET_PKT_SET_REPORT_FUNC(pkt, do_report);
        si->pkt = pkt;
        ipnet_softirq_init(&si->softirq);
        ipnet_softirq_schedule(&si->softirq,
                               (Ipnet_softirq_handler) ipnet_pkt_report_wrapper,
                               si);
#ifndef IPCOM_USE_JOB_QUEUE
        IPCOM_STACKD_POST(IP_FALSE);
#endif
    }
}


/*
 *===========================================================================
 *                    ipnet_increase_hdr_space
 *===========================================================================
 * Description: Increases the space before the data.
 * Parameters:  ppkt - Packet for which the header space should be increased.
 *              ip_ipstart_valid - IP_TRUE if the IP header is valid.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_increase_hdr_space(Ipcom_pkt **ppkt, Ip_bool is_ipstart_valid)
{
    Ipcom_pkt *pkt;

    pkt = ipnet_pkt_kclone(*ppkt, is_ipstart_valid, 100);
    if (pkt == IP_NULL)
        return -IP_ERRNO_ENOBUFS;
    IP_BIT_CLR((*ppkt)->flags, IPCOM_PKT_FLAG_HAS_IP_HDR);
    ipcom_pkt_free(*ppkt);
    *ppkt = pkt;
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_has_priv
 *===========================================================================
 * Description: Returns if the current user has the privilege to the
 *              the specified operation.
 * Parameters:  priv - one of the IPNET_PRIV_xxx constants.
 * Returns:
 *
 */
IP_GLOBAL Ip_bool
ipnet_has_priv(int cap)
{
    switch (cap)
    {
    case IPNET_PRIV_CHVR:
        return IP_TRUE;
    default:
        IP_PANIC();
        return IP_FALSE;
    }
}


/*
 *===========================================================================
 *                  ipnet_finish_transport_checksum
 *===========================================================================
 * Description: Calculates and writes down the transport layer checksum.
 * Parameters:  pkt - The packet containing the data.
 *                    pkt->tr_start points to the beginning of the checksum,
 *                    pkt->ipstart to the beginning of the IPv6 header
 *                    and pkt->chk contains the offset from pkt->tr_start
 *                    to where the checksum should be stored.
 * Returns:
 *
 */
#ifdef IPCOM_USE_HW_CHECKSUM_TX
IP_GLOBAL void
ipnet_finish_transport_checksum(Ipcom_pkt *pkt)
{
    Ip_u32  sum;
    Ip_u16 *chksum;

    ip_assert((pkt->tlstart & 1) == 0);
    ip_assert((pkt->chk & 1) == 0);
    ip_assert((int) (pkt->chk + pkt->tlstart + sizeof(Ip_u16)) <= pkt->end);

    IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM);
    chksum = (Ip_u16 *) &pkt->data[pkt->tlstart + pkt->chk];
    sum = ipcom_in_checksum_update(&pkt->data[pkt->tlstart], pkt->end - pkt->tlstart);
    *chksum = ipcom_in_checksum_finish(sum);
}
#endif /* IPCOM_USE_HW_CHECKSUM_TX */


/*
 ****************************************************************************
 * 12                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_pkt_create
 *===========================================================================
 * Description: Creates a valid Ipcom_pkt structure from user allocated memory.
 *              This function can be called in any context.
 * Parameters:  pkt - Pointer to user allocated memory that is at least
 *              sizeof(pkt) bytes long.
 *              data - Pointer to user allocated memory that will hold the
 *              IP packet data.
 *              data_len- The length of the buffer pointed to by 'data'.
 *              freefunc - A pointer to a function that will be called
 *              when IPNET is done using the created packet. 1:st argument
 *              will be the created packet ('data' pointer is pkt->data),
 *              2:nd argument is the same as the 'cookie' value passed to this
 *              function.
 *              cookie - User define data.
 * Returns:
 */
IP_FASTTEXT IP_PUBLIC void
ipcom_pkt_create(Ipcom_pkt *pkt,
                 void *data, Ip_size_t data_len,
                 Ipcom_pkt_freefunc data_freefunc, void *data_cookie)
{
    ip_assert(pkt->next_fragment == IP_NULL);
    ip_assert(pkt->next_original == IP_NULL);
    pkt->fd                   = IP_INVALID_SOCKET;
    pkt->vr_index             = 0;
    pkt->ifindex              = 0;
    pkt->next                 = IP_NULL;
    pkt->flags                = IPCOM_PKT_FLAG_ALLOC;
    pkt->ipstart              = 0;
    pkt->maxlen               = data_len;
    pkt->offset               = 0;
    pkt->chk                  = 0;
    pkt->data                 = data;
#ifdef IP_PORT_VXWORKS
    pkt->done                 = IPCOM_PKT_DONE_DEFAULT;
#endif
    pkt->data_freefunc        = data_freefunc;
    pkt->data_freefunc_cookie = data_cookie;
    pkt->dtor                 = IP_NULL;
    pkt->recurs_level         = 0;
    IPCOM_PKT_INIT_REF_COUNT(pkt);
#ifdef IPNET_USE_VLAN
    pkt->link_cookie          = 0;
#endif
#ifdef IPNET_USE_VRRP
    IPCOM_PKT_SET_VRID(pkt, 0);
#endif
    pkt->ipsec_skip           = 0;
    pkt->ipsec_hlen           = 0;
    pkt->ipsec_seq            = 0;
    pkt->ipsec_input          = 0;
    pkt->ipsec_domain         = 0;
    pkt->next_extra_data      = 0;
    pkt->force_frame_type     = 0;

#if defined (IPCOM_NAE) || defined (IPCOM_USE_FORWARDER)
    pkt->nae_ctx = IP_NULL;
#endif

#ifdef IPROHC
    pkt->flag_rohc            = IP_FALSE;
#endif

    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_INIT);
    IPNET_DEBUG_LINE(ipnet_pkt_check(pkt));
}


/*
 *===========================================================================
 *                    ipcom_pkt_input
 *===========================================================================
 * Context:     Any non-interrupt context.
 * Description: Inputs a packet to the stack.
 * Parameters:  netif - The interface the packet was received on.
 *              pkt - The received packet. pkt->start is the offset to the
 *              the link layer header.
 * Returns:
 *
 */
IP_FASTTEXT IP_PUBLIC void
ipcom_pkt_input(Ipcom_netif *ipcomif, Ipcom_pkt *pkt)
{
    ip_assert((pkt->start & 0x1) == 0);

#ifdef IPCOM_USE_JOB_QUEUE
    ipnet_pkt_input((Ipnet_netif *) ipcomif, pkt);
#else
    ipcom_pkt_input_queue(ipcomif, pkt);
#endif /* !IPCOM_USE_JOB_QUEUE */
}


/*
 *===========================================================================
 *                    ipcom_pkt_input_queue
 *===========================================================================
 * Context:     Any
 * Description: Queues packets for the input daemon. This function
 *              should be called directly in the interrupt routine for the
 *              network interface.
 * Parameters:  pkt - The received packet. pkt->start is the offset to the
 *              the link layer header.
 * Returns:     0 = success,
 *              -IP_ERRNO_EPERM = The stack has not finished its initialization
 *              -IP_ERRNO_EWOULDBLOCK = The input queue is full.
 *
 */
IP_PUBLIC int
ipcom_pkt_input_queue(Ipcom_netif *ipcomif, Ipcom_pkt *pkt)
{
    Ipnet_netif *netif = (Ipnet_netif *)ipcomif;
    int          max_inq_size = ipnet_conf_inq_max;
    int          key;
#ifdef IPCOM_USE_JOB_QUEUE
    Ip_bool      schedule_job;
#endif

    ip_assert((pkt->start & 0x1) == 0);

#if !defined(IP_PORT_ATI) && !defined(IP_PORT_VXWORKS) && !defined(IP_PORT_MQX) && !defined(IP_PORT_LKM) && !defined(IP_PORT_RTCORE)
    ip_assert(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_LOOPED_BACK)
              || (pkt->start >= (ipcom_conf_max_link_hdr_size - netif->ipcom.link_hdr_size)));
#endif

    IPNET_DEBUG_LINE(ipnet_pkt_check(pkt));

    pkt->next    = IP_NULL;
    if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_LOOPBACK))
        /* To make sure that the loopback interface always can queue some packets,
           so ipcom_output_done() will be called and release tasks blocking on the
           loopback interface */
        max_inq_size += 8;
    else
        /* Store the receiving interface in the packet */
        pkt->ifindex = netif->ipcom.ifindex;

    IPNET_MIN_LOCK(ipnet->inq.lock, key);

#ifdef IPCOM_USE_JOB_QUEUE
    schedule_job = (ipnet->inq.count == 0);
#endif

    ip_assert(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_ALLOC));
    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_RX);

    if (IP_UNLIKELY(ipnet->inq.count >= max_inq_size))
    {
        netif->iqdrops++;

        IPCOM_WV_EVENT_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_INFO,
                          1, 6, IPCOM_WV_NETDEVENT_INFO, IPCOM_WV_NETD_RECV,
                          ipcom_pkt_input_queue, IPCOM_WV_NETD_INFO_QUEUE_FULL,
                          IPCOM_WV_IPNET_MODULE, IPCOM_WV_NETD_IP4);
        IPNET_STATS(pkt_input_queue_full++);

        IPNET_MIN_UNLOCK(ipnet->inq.lock, key);

        ipcom_pkt_free(pkt);
        return -IP_ERRNO_EWOULDBLOCK;
    }


    if (ipnet->inq.head)
        ipnet->inq.tail->next = pkt;
    else
        ipnet->inq.head = pkt;
    ipnet->inq.tail = pkt;
    ++ipnet->inq.count;

    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_INQ);

    IPNET_MIN_UNLOCK(ipnet->inq.lock, key);

#ifdef IPCOM_USE_JOB_QUEUE
    if (schedule_job)
        ipcom_job_queue_schedule_singleton(ipnet->inq.job);
#else
    IPCOM_STACKD_POST(IP_FALSE);
#endif
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_kioevent
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipnet_kioevent(Ipnet_netif *netif, int event, void *data, int fcflags)
{
#ifndef IP_PORT_LKM
    int i;
#endif

    IPCOM_UNUSED_ARG(fcflags);

    if (!ipnet_is_stack_task())
    {
        Ipnet_kioevent_softirq *si;
        /* Cannot do any processing in this context,
           reschedule the event to a soft interrupt */
        ip_assert(data == IP_NULL);
        si = ipcom_slab_alloc(ipnet->slabs.kioevent_softirq);
        if (si != IP_NULL)
        {
            si->event   = event;
            si->ifindex = netif->ipcom.ifindex;
            ipnet_softirq_init(&si->softirq);
            ipnet_softirq_schedule(&si->softirq,
                                   (Ipnet_softirq_handler)ipnet_kioevent_softirq,
                                   si);
#ifndef IPCOM_USE_JOB_QUEUE
            IPCOM_STACKD_POST(IP_FALSE);
#endif
        }
        return;
    }

    IPNET_IF_LOCK(netif);

    switch (event)
    {
    case IP_EIOXUP:
        if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_UP))
            goto cleanup;
        IP_BIT_SET(netif->ipcom.flags, IP_IFF_UP);
        IPCOM_LOG1(INFO, "%s is now UP", netif->ipcom.name);
        break;

    case IP_EIOXDOWN:
        if (IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_UP))
            goto cleanup;
#ifdef IPCOM_USE_FORWARDER
        ipnet->route_cache_id++;
#endif
        IP_BIT_CLR(netif->ipcom.flags, IP_IFF_UP);
        IPCOM_LOG1(INFO, "%s is now DOWN", netif->ipcom.name);
        break;

    case IP_EIOXRUNNING:
        if (IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_RUNNING))
            goto cleanup;
        IP_BIT_SET(netif->ipcom.flags, IP_IFF_RUNNING);
        IPCOM_LOG1(INFO, "%s is now RUNNING", netif->ipcom.name);
        break;

    case IP_EIOXSTOP:
        if (IP_BIT_ISFALSE(netif->ipcom.flags, IP_IFF_RUNNING))
            goto cleanup;
        IP_BIT_CLR(netif->ipcom.flags, IP_IFF_RUNNING);
        ipnet_if_clean_snd_queue(netif);
        IPCOM_LOG1(INFO, "%s is now STOPPED", netif->ipcom.name);
        break;

    case IP_EIOXCHANGE:
        if (netif->ipcom.drv_ioctl != IP_NULL)
            (void) netif->ipcom.drv_ioctl(&netif->ipcom,
                                          IP_SIOCXCHGFLAGS,
                                          IP_NULL);
        break;

    default:
        break;
    }

    switch (event)
    {
    case IP_EIOXUP:
    case IP_EIOXRUNNING:
        /* Notifications (note: the order below is important!) */
        IPNET_ROUTESOCK(ipnet_routesock_if_change(netif));
        IPNET_NETLINKSOCK(ipnet_rtnetlink_link_change(netif));
#ifdef IPCOM_USE_INET
        ipnet_ip4_kioevent(netif, event);
#endif
#ifdef IPCOM_USE_INET6
        ipnet_ip6_kioevent(netif, event);
#endif
        break;

    case IP_EIOXDOWN:
    case IP_EIOXSTOP:
    case IP_EIOXCHANGE:
        /* Notifications (note: the order below is important!) */
#ifdef IPCOM_USE_INET
        ipnet_ip4_kioevent(netif, event);
#endif
#ifdef IPCOM_USE_INET6
        ipnet_ip6_kioevent(netif, event);
#endif
        IPNET_ROUTESOCK(ipnet_routesock_if_change(netif));
        IPNET_NETLINKSOCK(ipnet_rtnetlink_link_change(netif));

    default:
        break;
    }

#ifndef IP_PORT_LKM
    for (i = 0; i < IPNET_MAX_IF_EVENTCB; i++)
        if (ipnet->if_eventcb[i] != IP_NULL)
            break;

    if (i < IPNET_MAX_IF_EVENTCB)
    {
        /* At least one callback present */
        /* NOTE: Do _NOT_ change below lock code without consulting OSE5 responsible. */
        for (i = 0; i < IPNET_MAX_IF_EVENTCB; i++)
            if (ipnet->if_eventcb[i] != IP_NULL)
                (void)ipnet->if_eventcb[i](netif->ipcom.name, netif->ipcom.ifindex, netif->ipcom.flags, event, data);
    }
#endif

 cleanup:
    IPNET_IF_UNLOCK(netif);
}


/*
 *===========================================================================
 *                    ipnet_timeval_to_msec
 *===========================================================================
 * Description: Converts timeval to millisecond.
 * Parameters:  tv - A pointer to a timeval struct or IP_NULL for inifinity.
 * Returns:     Millisecond representation of the timevalue.
 *
 */
IP_PUBLIC Ip_s32
ipnet_timeval_to_msec(struct Ip_timeval *tv)
{
    if (tv == IP_NULL)
        return -1;
    return tv->tv_sec * 1000 + (tv->tv_usec + 999) / 1000;
}


/*
 *===========================================================================
 *                    ipnet_create
 *===========================================================================
 * Description: Create IPNET
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipnet_create(void)
{
    int ret;

#ifdef IPCOM_USE_SYSLOG
    ipcom_openlog("ipnet", 0, IPCOM_LOG_IPNET);
#endif

    /* Initialize IPNET. */
    ret = ipnet_init();
    if (ret < 0)
    {
        IPCOM_LOG1(ERR, "ipnet_create() :: ipnet_init() failed: %s",
                   ipcom_strerror(-ret));
        goto fail;
    }

    return IPCOM_SUCCESS;

fail:
    IPCOM_LOG0(ERR, "ipnet_create() :: failed");
    IP_PANIC();
    return IPCOM_ERR_FAILED;
}


/*
 *===========================================================================
 *                    ipnet_start
 *===========================================================================
 * Description: Start IPNET
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipnet_start(void)
{
    int    ret;
    Ip_err retval;

#ifdef IPCOM_USE_JOB_QUEUE
    ipcom_job_queue_schedule((Ipcom_job_action) ipcom_stackd_init,
                             IPCOM_JOB_PRIO_HIGH);
#else
    /* Create IPNET processes. */
    retval = ipcom_proc_create("ipnetd",
                               (Ipcom_proc_func)ipnetd,
                               IPCOM_PROC_STACK_LARGE,
                               IP_NULL);
    if (retval != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "ipnet_start() :: failed to start the input daemon");
        goto fail;
    }
#endif

#ifndef IP_PORT_LKM
#ifdef IPCOM_USE_ETHERNET
    retval = ipcom_drv_eth_create();
    if (retval != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "ipnet_create() :: ipcom_drv_eth_create() failed: %ld", retval);
        goto fail;
    }
#endif /* IPCOM_USE_ETHERNET */

#ifdef IPNET_USE_WLAN
#if 0 /* RIME */
    retval = ipcom_drv_wlan_create();
    if (retval != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "ipnet_create() :: ipcom_drv_wlan_create() failed: %ld", retval);
        goto fail;
    }
#endif
#endif /* IPNET_USE_WLAN */

#ifdef IPCOM_USE_LBP
    retval = ipcom_drv_lbp_create();
    if (retval != IPCOM_SUCCESS)
      {
        IPCOM_LOG1(ERR, "ipnet_create() :: ipcom_drv_lbp_create() failed: %ld", retval);
        goto fail;
    }
#endif /* IPCOM_USE_LBP */

#else
    (void)retval;
#endif /* IP_PORT_LKM */

    /* Open IPNET now when drivers are initialized. */
    ret = ipnet_open();
    if (ret < 0)
    {
        IPCOM_LOG1(ERR, "ipnet_start() :: ipnet_open() failed: %s",
                   ipcom_strerror(-ret));
        goto fail;
    }

#ifdef IPNET_USE_NAT
    retval = ipnet_nat_init();
    if(retval != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "ipnet_start() :: ipnet_nat_init() failed: %d", retval);
        goto fail;
    }

#endif /* #ifdef IPNET_USE_NAT */

#ifdef IPNET_USE_DIFFSERV
    retval = ipnet_diffserv_init();
    if(retval != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "ipnet_start() :: ipnet_diffserv_init() failed: %d", retval);
        goto fail;
    }
#endif /* IPNET_USE_DIFFSERV */

#if !defined(IP_PORT_OSE5) && !defined(IP_PORT_LKM)
    if (ipnet_boot_config() < 0)
    {
        IPCOM_LOG0(CRIT, "Failed to set the boot configuration");
    }
#endif

#ifdef IPCOM_USE_FORWARDER
    {
        ipcom_forwarder_create_cf();
    }
#endif

    /* IPNET create done. */
    IPCOM_LOG0(DEBUG, "ipnet_start() :: success");
    return IPCOM_SUCCESS;

fail:
    IPCOM_LOG0(ERR, "ipnet_start() :: failed");
    IP_PANIC();
    return IPCOM_ERR_FAILED;
}


/*
 *===========================================================================
 *                    ipnet_version
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC const char *
ipnet_version(void)
{
    return "@(#) IPNET $Name: VXWORKS_ITER18A_FRZ10 $ - INTERPEAK_COPYRIGHT_STRING";
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

