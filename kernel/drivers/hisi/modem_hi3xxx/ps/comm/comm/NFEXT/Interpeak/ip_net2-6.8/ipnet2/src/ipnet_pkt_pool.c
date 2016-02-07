/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_pkt_pool.c,v $ $Revision: 1.76.12.1 $
 *     $Author: kenneth $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 *                    DESCRIPTION
 ****************************************************************************
 * Contains the implementation of the IPNET packet pool.
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

#ifdef IPNET_USE_PACKET_POOL


#define IPCOM_USE_CLIB_PROTO
#include <ipcom_atomic.h>
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_ipd.h>
#include <ipcom_netif.h>
#include <ipcom_os.h>
#include <ipcom_sock2.h>
#include <ipcom_syslog.h>
#include <ipcom_sysvar.h>
#include <ipcom_type.h>

#include "ipnet.h"
#include "ipnet_cmd.h"
#include "ipnet_eth.h"
#include "ipnet_h.h"
#include "ipnet_loopback.h"
#include "ipnet_netif.h"
#include "ipnet_pkt_queue.h"

#ifdef IPTCP
#include <iptcp.h>
#endif

#ifdef IPSCTP
#include <ipsctp.h>
#endif

#ifdef IPCOM_WV_INSTRUMENTATION
#include <ipcom_windview.h>
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* pkt->data size need to be IPNET_PKT_ALIGNMENT larger for rounding up
 * to correct &pkt->data[pkt->start] alignment on incoming buffers.
 */
#define IPNET_PKT_ALIGNMENT  64

/*
 * 16-bit length field + 40 bytes of IPv6 header
 */
#define IPNET_PKT_POOL_MAX_MTU (65535 + 40)


#define IPNET_PKT_SET_RECURSE_LEVEL(fcflags, lvl) (((fcflags) & 0xffff) | ((lvl) << 16))
#define IPNET_PKT_GET_RECURSE_LEVEL(fcflags)      ((fcflags) >> 16)


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/* Descriptor for a packet pool buffer slab */
struct Ipnet_pkt_pool_slab_desc
{
    Ip_size_t            mtu;    /* The maximum MTU this buffer can
                                    hold */
    Ip_size_t            size;   /* The actual length of this
                                    buffer */
    Ip_u32               flags;  /* The IPCOM_SLAB_F_xxx flags for
                                    this buffer */
    Ipcom_memory_cache **pslab;  /* Pointer to where the memory cache
                                    should be stored */
};


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

IP_EXTERN int ipcom_conf_max_link_hdr_size;

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_STATIC Ip_bool
ipnet_pkt_pool_hdr_ctor(Ipcom_memory_cache *mc, Ipcom_pkt *pkt);

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

/*
 * The port may use this function pointer to customize the packet header
 * constructor at start-up based upon link-time configuration.
 */
IP_GLOBAL Ipcom_slab_ctor ipnet_pkt_pool_hdr_ctor_func =
    (Ipcom_slab_ctor)ipnet_pkt_pool_hdr_ctor;

/* Port-specific extra space allocated after each Ipcom_pkt */
Ip_size_t ipcom_port_pkt_hdr_extra_size = 0;

/*
 * ipcom_conf_pkt_pool_buffer_alignment and ipcom_conf_pkt_pool_buffer_extra_space
 * may be increased by the port before the call to ipnet_pkt_pool_init().
 * Typically this would be done on behalf of network device drivers that
 * have more stringent alignment or storage requirements.
 */
Ip_size_t ipcom_conf_pkt_pool_buffer_alignment = 0;
Ip_size_t ipcom_conf_pkt_pool_buffer_extra_space = 0;

struct Ipnet_pkt_pool_slab_desc ipnet_pkt_pool_slab_desc[] = {
    { IPNET_ETH_MTU,              0, IPCOM_SLAB_F_AVERAGE, IP_NULL },
    { 3000,                       0, IPCOM_SLAB_F_FEW,     IP_NULL },
    { 10000,                      0, IPCOM_SLAB_F_ONE,     IP_NULL },
    { IPNET_PKT_POOL_MAX_MTU + 1, 0, IPCOM_SLAB_F_ONE,     IP_NULL },
    { 0 }
};


/*
 ****************************************************************************
 * 10                   STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_pktslab_alloc
 *===========================================================================
 * Context:     Task level with packet pool locked
 * Description: Allocate an object from the specified packet pool memory cache.
 *              NOTE: May unlock the packet pool to allocate a new slab from
 *              the system heap.
 * Parameters: mc - the packet pool cache to allocate from
 * Returns: a pointer to the allocated object, or IP_NULL
 *
 */
IP_INLINE void *
ipnet_pktslab_alloc(Ipcom_memory_cache * mc)
{
    void *obj;
    void *slab;

    obj = ipcom_slab_alloc_try(mc, IP_FALSE);
    if (IP_UNLIKELY(obj == IP_NULL))
    {
        ipcom_spinlock_unlock (ipnet->pkt_pool_lock);
        slab = ipcom_slab_add(mc);
        ipcom_spinlock_lock (ipnet->pkt_pool_lock);
        if (slab)
            obj = ipcom_slab_alloc_from(mc, slab);
    }
    return obj;
}


/*
 *===========================================================================
 *                    ipnet_pkt_drain
 *===========================================================================
 * Context:     Soft interrupt
 * Description: Frees all packets that is in reassembly queues and
 *              out-of-order TCP queues.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_drain(void *unused)
{
    Ipcom_pkt *pkt;

    IPCOM_UNUSED_ARG(unused);

#ifdef IPCOM_USE_INET
    pkt = ipnet->ip4.reassembly_list;
    while (pkt != IP_NULL)
    {
        ipnet_timeout_cancel(IPNET_PKT_GET_TMO(pkt));
        ipnet->ip4.reassembly_list = pkt->next;
        IPCOM_WV_MARKER_2 (IPCOM_WV_NETD_IP4_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                           1, 1, IPCOM_WV_NETDEVENT_WARNING,
                           ipnet_pkt_drain, IPCOM_WV_NETD_PKTLOST,
                           IPCOM_WV_IPNET_PKT_POOL_MODULE, IPCOM_WV_NETD_NETBUF);
        IPNET_STATS(pkt_drain_ip4_reasm++);
        IPNET_STATS(pkt_num_ip4_reassembly_list--);
        ipcom_pkt_free(pkt);
        pkt = ipnet->ip4.reassembly_list;
    }
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    pkt = ipnet->ip6.reassembly_list;
    while (pkt != IP_NULL)
    {
        ipnet_timeout_cancel(IPNET_PKT_GET_TMO(pkt));
        ipnet->ip6.reassembly_list = pkt->next;
        IPCOM_WV_MARKER_2 (IPCOM_WV_NETD_IP6_DATAPATH_EVENT, IPCOM_WV_NETD_WARNING,
                           1, 2, IPCOM_WV_NETDEVENT_WARNING,
                           ipnet_pkt_drain, IPCOM_WV_NETD_PKTLOST,
                           IPCOM_WV_IPNET_PKT_POOL_MODULE, IPCOM_WV_NETD_NETBUF);
        IPNET_STATS(pkt_drain_ip6_reasm++);
        IPNET_STATS(pkt_num_ip6_reassembly_list--);
        ipcom_pkt_free(pkt);
        pkt = ipnet->ip6.reassembly_list;
    }
#endif /* IPCOM_USE_INET6 */

    ipnet_sock_pkt_drain();

#ifdef IPTCP
    iptcp_pkt_drain();
#endif /* IPTCP */
#ifdef IPSCTP
    ipsctp_pkt_drain();
#endif /* IPSCTP */
}

/*
 *===========================================================================
 *                    ipnet_pkt_pool_unlock_sched_drain
 *===========================================================================
 * Context:     Task level with packet pool locked
 * Description: This routine is the ipcom_wait_queue_add_current_proc()
 *              unlock callback installed from ipcom_pkt_malloc(). It
 *              unlocks the packet pool and schedules ipnet_pkt_drain()
 *              as a softirq.
 * Parameters:  arg - the packet pool spinlock
 * Returns:
 */
IP_STATIC void
ipnet_pkt_pool_unlock_sched_drain(void * arg)
{
    Ipcom_spinlock lock = arg;
    /*
     * The packet pool lock cannot protect the softirq queue,
     * and we must not nest spinlocks; so unlock the packet pool, and
     * let ipnet_softirq_schedule do its own locking.
     */
    ipcom_spinlock_unlock(lock);
    ipnet_softirq_schedule(&ipnet->drain_softirq,
                           (Ipnet_softirq_handler) ipnet_pkt_drain,
                           IP_NULL);
}

/*
 *===========================================================================
 *                     ipnet_pkt_pool_wakeup_tmo
 *===========================================================================
 * Description: Wakeup all processes waiting for more packets to become
 *              available in the packet pool.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_pool_wakeup_tmo(void *unused)
{
    IPCOM_UNUSED_ARG(unused);

    ipcom_spinlock_lock(ipnet->pkt_pool_lock);
    ipcom_wait_queue_wakeup_all_procs(&ipnet->pkt_pool_wq);
    ipcom_spinlock_unlock(ipnet->pkt_pool_lock);
}


/*
 *===========================================================================
 *                    ipnet_issue_pktavail_cb
 *===========================================================================
 * Context:     Soft interrupt
 * Description: Sends a IP_SOEVENT_CB_PKTAVAIL event to all sockets that
 *              want to know when more packets are available in the pool
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_issue_pktavail_cb(void *unused)
{
    int           i;
    Ipnet_socket *sock;

    IPCOM_UNUSED_ARG(unused);

    if (IP_BIT_ISSET(ipnet_flags, IPNET_FLAG_PKTAVAIL_CB_PENDING))
    {
        IP_BIT_CLR(ipnet_flags, IPNET_FLAG_PKTAVAIL_CB_PENDING);

        for (i = 0; i < ipnet_conf_max_sockets; i++)
        {
            Ipcom_socket_eventcb event_cb;

            sock = ipnet->sockets[i];
            if (sock == IP_NULL
                || (event_cb = sock->ipcom.event_cb) == IP_NULL
                || IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_PKTAVAIL_CB_PENDING) == IP_FALSE)
                continue;
            IP_BIT_CLR(sock->flags, IPNET_SOCKET_FLAG_PKTAVAIL_CB_PENDING);
            event_cb(&sock->ipcom, IP_NULL, IP_SOEVENT_CB_PKTAVAIL);
        }
    }

    if (ipnet->pkt_pool_tmo == IP_NULL)
    {
        /* The timeout should just be short, but still long enough to
           allow at least one scheduling time-slice to pass */
        (void)ipnet_timeout_schedule(10,
                                     (Ipnet_timeout_handler) ipnet_pkt_pool_wakeup_tmo,
                                     IP_NULL,
                                     &ipnet->pkt_pool_tmo);
        /* Do an immediate wake-up to release all processes whose
           allocation request can be satisfied by the packet freed
           since this softirq was scheduled */
        ipnet_pkt_pool_wakeup_tmo(IP_NULL);
    }
}


/*
 *===========================================================================
 *                     ipnet_pkt_pool_hdr_ctor
 *===========================================================================
 * Description: Packet header constructor.
 * Parameters:  mc - memory slab cache managing this packet.
 *              pkt - packet header.
 * Returns:     IP_TRUE - the packet header was successfully initialized.
 *              IP_FALSE - the packet header initialization failed.
 *
 */
IP_STATIC Ip_bool
ipnet_pkt_pool_hdr_ctor(Ipcom_memory_cache *mc, Ipcom_pkt *pkt)
{
    IPCOM_UNUSED_ARG(mc);

    pkt->next_fragment = IP_NULL;
    pkt->next_original = IP_NULL;
    pkt->next_part = IP_NULL;
    return IP_TRUE;
}


/*
 *===========================================================================
 *                     ipnet_pkt_pool_hdr_dtor
 *===========================================================================
 * Description: Packet header destructor.
 * Parameters:  mc - memory slab cache managing this packet.
 *              pkt - packet header.
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_pool_hdr_dtor(Ipcom_memory_cache *mc, Ipcom_pkt *pkt)
{
    IPCOM_UNUSED_ARG(mc);
    IPCOM_UNUSED_ARG(pkt);

    ip_assert(IP_BIT_ISFALSE(pkt->flags, IPCOM_PKT_FLAG_ALLOC));
    ip_assert(pkt->next_fragment == IP_NULL);
    ip_assert(pkt->next_original == IP_NULL);
    ip_assert(pkt->next_part == IP_NULL);
}


/*
 ****************************************************************************
 * 11                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 12                   PUBLIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_pktbuf_free
 *===========================================================================
 * Description: Frees a packet buffer allocted with an Ipcom_pkt using
 *              ipcom_pkt_malloc() with non-zero mtu, and later disassociated
 *              from its Ipcom_pkt.
 * Parameters:  buf - The buffer to free (old pkt->data)
 *              cookie - Cookie to free buffer (old pkt->data_freefunc_cookie)
 * Returns: N/A
 *
 */
IP_PUBLIC void
ipcom_pktbuf_free(Ip_u8 *buf, void * cookie)
{
    Ipcom_memory_cache *pkt_buf_slab = cookie;

    ipcom_spinlock_lock(ipnet->pkt_pool_lock);
    ipcom_slab_free(pkt_buf_slab, buf);
    ipcom_spinlock_unlock(ipnet->pkt_pool_lock);
}


/*
 *===========================================================================
 *                    ipcom_pkt_free
 *===========================================================================
 * Description: Frees a packet that was allocted with ipcom_pkt_malloc().
 * Parameters:  pkt - The packet to free.
 * Returns:
 *
 */
#ifdef IP_DEBUG
IP_PUBLIC void
ipcom_pkt_dfree(Ipcom_pkt *pkt, char *file, int line)
#else
IP_FASTTEXT IP_PUBLIC void
ipcom_pkt_free(Ipcom_pkt *pkt)
#endif /* IP_DEBUG */
{
    Ipcom_pkt *next_pkt;

#ifdef IPNET_ASSERT
    ip_assert(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_ALLOC));
#endif

#ifdef IP_DEBUG
    ipnet_pkt_check(pkt);
#ifdef IPTCP_DEBUG
    iptcp_pkt_freecheck(pkt);
#endif
    IPCOM_PKT_TRACE_FL(pkt, IPCOM_PKT_ID_DELREF, file, line);
#endif /* IP_DEBUG */

    if (IP_UNLIKELY(pkt->next_original != IP_NULL))
    {
        ipcom_pkt_free(pkt->next_original);
        pkt->next_original = IP_NULL;
    }

    if (pkt->next_part != IP_NULL)
    {
        ipcom_pkt_free(pkt->next_part);
        pkt->next_part = IP_NULL;
    }

    do
    {
        next_pkt = pkt->next_fragment;
        pkt->next_fragment = IP_NULL;

        if (IPCOM_PKT_DEL_REF(pkt) <= 0)
        {
#ifdef IPNET_USE_PER_CPU_PKT_CACHE
            Ip_bool          is_cached = IP_FALSE;
            Ipnet_pkt_cache *cache;
            IPNET_PKT_CACHE_KEY_DECLARE(key)
#endif

            if (pkt->dtor)
                pkt->dtor(pkt);

            IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_ALLOC);
            IPCOM_PKT_TRACE_FL(pkt, IPCOM_PKT_ID_FREEQ, file, line);
            if (pkt->data_freefunc)
            {
                /* Custom free function, call without the packet pool lock */
                pkt->data_freefunc(pkt->data, pkt->data_freefunc_cookie);
                pkt->data = IP_NULL;
            }

#ifdef IPNET_USE_PER_CPU_PKT_CACHE
            IPNET_PKT_CACHE_LOCK(cache, ipcom_get_cpuid(), key);

            if (pkt->data == IP_NULL)
            {
                /* Only a packet header to cache */
                if (cache->hdrs_count < IPNET_PKT_CACHE_MAX_COUNT)
                {
                    cache->hdrs_count++;
                    pkt->next = cache->hdrs;
                    cache->hdrs = pkt;
                    is_cached = IP_TRUE;
                }
            }
            else if (cache->pkts_count < IPNET_PKT_CACHE_MAX_COUNT
                     && pkt->data_freefunc_cookie == (void *)ipnet->slabs.pkt_buf_1500)
            {
                ++cache->pkts_count;
                pkt->next = cache->pkts;
                cache->pkts = pkt;
                is_cached = IP_TRUE;
            }

            IPNET_PKT_CACHE_UNLOCK(cache, key);

#ifdef IP_DEBUG
            if (is_cached)
                IPCOM_PKT_TRACE_FL(pkt, IPCOM_PKT_ID_PER_CPU_CACHE, file, line);
#endif
            if (!is_cached)
#endif /* IPNET_USE_PER_CPU_PKT_CACHE */
            {
                ipcom_spinlock_lock(ipnet->pkt_pool_lock);

                if (pkt->data != IP_NULL)
                {
                    Ipcom_memory_cache *pkt_buf_slab = pkt->data_freefunc_cookie;
                    ipcom_slab_free(pkt_buf_slab, pkt->data);
                    pkt->data = IP_NULL;
                }
                IPNET_DEBUG_LINE(ipnet_pkt_remove_from_alloc_list(pkt));
                ipcom_slab_free(ipnet->slabs.pkt_hdr, pkt);

                ipcom_spinlock_unlock(ipnet->pkt_pool_lock);
            }
        }
    }
    while (IP_UNLIKELY(IP_NULL != (pkt = next_pkt)));

    if (IP_UNLIKELY(IP_BIT_ISSET(ipnet_flags, IPNET_FLAG_PKTAVAIL_CB_PENDING))
        || IP_UNLIKELY(!ipcom_wait_queue_is_empty(&ipnet->pkt_pool_wq)
                       && ipnet->pkt_pool_tmo == IP_NULL))
    {
        ipnet_softirq_schedule(&ipnet->pktavail_softirq,
                               (Ipnet_softirq_handler) ipnet_issue_pktavail_cb,
                               IP_NULL);
        IPCOM_STACKD_POST(IP_FALSE);
    }
}


/*
 *===========================================================================
 *                    ipcom_pkt_malloc
 *===========================================================================
 * Description: Allocates the requested amount of MTU data. MTU data is
 *              defined as IP + transport + application (not link header).
 * Parameters:  [in] mtu - The amount of payload the packet must be able to hold.
 *              [in] fcflags - IP_FLAG_FC_xxx flags.
 * Returns:     An uninitialized packet.
 *
 */
#ifdef IP_DEBUG
IP_PUBLIC Ipcom_pkt *
ipcom_pkt_dmalloc(Ip_size_t mtu, int fcflags, char *file, int line)
#else
IP_FASTTEXT IP_PUBLIC Ipcom_pkt *
ipcom_pkt_malloc(Ip_size_t mtu, int fcflags)
#endif /* IP_DEBUG */
{
    Ip_size_t             data_len;
    Ipcom_pkt            *pkt;
    Ip_bool               only_hdr = (mtu == 0);
    Ip_bool               wake_ipnet_daemon = IP_FALSE;
    void                 *data_buf = IP_NULL;
    Ipcom_memory_cache   *pkt_buf_slab = IP_NULL;
#ifdef IPNET_USE_PER_CPU_PKT_CACHE
    Ipnet_pkt_cache      *cache;
    IPNET_PKT_CACHE_KEY_DECLARE(key)
#endif

#ifdef IPNET_ASSERT
    if (IP_BIT_ISSET(fcflags, IP_FLAG_FC_BLOCKOK))
    {
        ip_assert(IP_BIT_ISFALSE(fcflags, IP_FLAG_FC_ISRCONTEXT));
        /* Must no use blocking ipcom_pkt_malloc() when running in the
           context of an IPNET system task */
        ip_assert2(IP_BIT_ISFALSE(ipnet_flags, IPNET_FLAG_CONFIGURED)
                   || ipnet_is_stack_task() == IP_FALSE);
    }
#endif

    if (mtu > IPNET_PKT_POOL_MAX_MTU)
        return IP_NULL;

#ifdef IPNET_USE_PER_CPU_PKT_CACHE

    IPNET_PKT_CACHE_LOCK(cache, ipcom_get_cpuid(), key);

    if (only_hdr)
    {
        pkt = cache->hdrs;
        if (IP_NULL != pkt)
        {
            --cache->hdrs_count;
            cache->hdrs = pkt->next;
            data_len = 0;
            IPNET_PKT_CACHE_UNLOCK(cache, key);
            goto done;
        }
    }
    else
    {
        pkt = cache->pkts;
        /* The cache holds packets with an MTU suitable for Ethernet,
           larger packets has to be allocated from the normal packet
           pool. */
        if (pkt != IP_NULL && IPNET_ETH_MTU >= mtu)
        {
            --cache->pkts_count;
            cache->pkts = pkt->next;
            data_len = pkt->maxlen;
            data_buf = pkt->data;
            pkt_buf_slab = pkt->data_freefunc_cookie;
            IPNET_PKT_CACHE_UNLOCK(cache, key);
            goto done;
        }
    }

    IPNET_PKT_CACHE_UNLOCK(cache, key);
#endif /* IPNET_USE_PER_CPU_PKT_CACHE */

    ipcom_spinlock_lock(ipnet->pkt_pool_lock);

    for (;;)
    {
        pkt = ipnet_pktslab_alloc(ipnet->slabs.pkt_hdr);
        if (pkt != IP_NULL)
        {
            struct Ipnet_pkt_pool_slab_desc *ppsd;

            if (only_hdr)
            {
                data_len = 0;
                goto packet_allocated;
            }

            for (ppsd = ipnet_pkt_pool_slab_desc;
                 ppsd->mtu > 0;
                 ppsd++)
            {
                if (ppsd->mtu >= mtu)
                {
                    pkt_buf_slab = *ppsd->pslab;
                    data_buf = ipnet_pktslab_alloc(pkt_buf_slab);
                    if (data_buf != IP_NULL)
                    {
                        data_len = ppsd->size;
                        goto packet_allocated;
                    }
                    if (fcflags & IPCOM_FLAG_FC_EXACT)
                        break;
                }
            }

            /* No data buffers available */
            ipcom_slab_free(ipnet->slabs.pkt_hdr, pkt);
            pkt = IP_NULL;
        }
        /* else: No packet headers */

        if (IP_BIT_ISFALSE(fcflags, IP_FLAG_FC_BLOCKOK))
            goto no_pkt_avail;

        wake_ipnet_daemon = IP_TRUE;

        /* Sleep until more packets becomes available */
        (void)ipcom_wait_queue_add_current_proc(&ipnet->pkt_pool_wq,
                IPC_TIMEOUT_INFINITE,
                (Ipcom_wait_queue_func) ipnet_pkt_pool_unlock_sched_drain,
                (Ipcom_wait_queue_func) ipcom_spinlock_lock,
        (void *)ipnet->pkt_pool_lock);

    }

 packet_allocated:
    IPNET_DEBUG_LINE(ipnet_pkt_add_to_alloc_list(pkt));
    ipcom_spinlock_unlock(ipnet->pkt_pool_lock);

#ifdef IPNET_USE_PER_CPU_PKT_CACHE
    /* The per-CPU cache could not satisfy this request, lets add a
       couple (up to half the size of the packet cache) of packets to
       the per-CPU cache for this CPU. This should be faster than take
       since all code and data used to allocate packets are most
       likely cache hot right now. */
    if (mtu <= 1500)
    {
        int recurse_lvl = IPNET_PKT_GET_RECURSE_LEVEL(fcflags);

        if (recurse_lvl < IPNET_PKT_CACHE_MAX_COUNT / 2)
        {
            Ipcom_pkt *cache_pkt;

            cache_pkt = ipcom_pkt_malloc(mtu,
                                         IPNET_PKT_SET_RECURSE_LEVEL(fcflags, recurse_lvl + 1));
            if (IP_LIKELY(cache_pkt != IP_NULL))
                ipcom_pkt_free(cache_pkt);
        }
    }

 done:
#endif /* IPNET_USE_PER_CPU_PKT_CACHE */

    IPCOM_PKT_TRACE_FL(pkt, IPCOM_PKT_ID_MALLOC, file, line);
    ipcom_pkt_create(pkt, data_buf, data_len, IP_NULL, pkt_buf_slab);

    if (wake_ipnet_daemon)
        IPCOM_STACKD_POST(IP_FALSE);

    /* Let the allocated packet inherit the same block/non-block status as the called */
    if (IP_BIT_ISFALSE(fcflags, IP_FLAG_FC_BLOCKOK))
        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_NONBLOCKING);

    return pkt;

 no_pkt_avail:
    /* No more packets to hand out. */
    ipcom_spinlock_unlock(ipnet->pkt_pool_lock);
    ipnet_softirq_schedule(&ipnet->drain_softirq,
                           (Ipnet_softirq_handler) ipnet_pkt_drain,
                           IP_NULL);
    IPCOM_STACKD_POST(IP_FALSE);
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_pkt_pool_init
 *===========================================================================
 * Description: Allocates a pool of Ipcom_pkt buffers that IPNET can use.
 * Parameters:
 * Returns:     0 = success, >0 = error code.
 *
 */
IP_GLOBAL int
ipnet_pkt_pool_init(void)
{
    struct Ipnet_pkt_pool_slab_desc *slab_desc = ipnet_pkt_pool_slab_desc;
    int i;
    Ip_size_t align;
    int       extra;

    slab_desc[0].pslab = &ipnet->slabs.pkt_buf_1500;
    slab_desc[1].pslab = &ipnet->slabs.pkt_buf_3000;
    slab_desc[2].pslab = &ipnet->slabs.pkt_buf_10000;
    slab_desc[3].pslab = &ipnet->slabs.pkt_buf_65535;

    ipnet->slabs.pkt_hdr
        = ipcom_slab_new("IPNET packet header",
                         IPCOM_SLAB_F_NO_LOCKING | IPCOM_SLAB_F_MANY,
                         sizeof(Ipcom_pkt) + ipcom_port_pkt_hdr_extra_size,
                         ipnet_conf_cache_bufsiz,
                         ipnet_pkt_pool_hdr_ctor_func,
                         (Ipcom_slab_dtor) ipnet_pkt_pool_hdr_dtor,
                         ipnet->memory_pool);

    align = ipcom_conf_pkt_pool_buffer_alignment;
    if (align < ipnet_conf_cache_bufsiz)
        align = ipnet_conf_cache_bufsiz;

    extra = ipcom_conf_pkt_pool_buffer_extra_space;
    if (extra < ipcom_conf_max_link_hdr_size + IPNET_PKT_ALIGNMENT)
        extra = ipcom_conf_max_link_hdr_size + IPNET_PKT_ALIGNMENT;

    for (i = 0; slab_desc[i].mtu > 0; i++)
    {
        char      slab_name[40];

        slab_desc[i].size = (extra + slab_desc[i].mtu);

        ipcom_sprintf(slab_name, "IPNET %d bytes packet buffer", slab_desc[i].mtu);
        *slab_desc[i].pslab
            = ipcom_slab_new(slab_name,
                             IPCOM_SLAB_F_NO_LOCKING | slab_desc[i].flags,
                             slab_desc[i].size + align,
                             align,
                             IP_NULL,
                             IP_NULL,
                             ipnet->memory_pool);
    }

    return 0;
}


/*
 *===========================================================================
 *                     ipnet_pkt_pool_hdr_count_hint
 *===========================================================================
 * Description: Returns a hint on how many packet headers that this system
 *              will use.
 * Parameters:
 * Returns:     Hint of how many packet header that will be needed.
 *
 */
IP_GLOBAL int
ipnet_pkt_pool_hdr_count_hint(void)
{
    Ipcom_memory_cache *pkt_hdr_slab;
    unsigned            objs_per_slab;

    pkt_hdr_slab = ipnet->slabs.pkt_hdr;
    objs_per_slab = ipcom_slab_get_objects_per_slab(pkt_hdr_slab);

    /* Assume that up to half the memory pool could be used for 1500
       bytes packet buffers at the most. Returns a count that is
       evenly dividable with the number of packet headers per slab  */
    return ((pkt_hdr_slab->memory_pool->limit + 2 * 1500 * objs_per_slab - 1)
            / 2 / 1500 / objs_per_slab)
        * objs_per_slab;
}


#else
/*
 *===========================================================================
 *                    ipnet_pkt_data_freefunc
 *===========================================================================
 * Description: Dummy version of the freefunc
 * Parameters:  [in] data - The data buffer to free
 *              [in] cookie - Contains the packet pool to use.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_pkt_data_freefunc(void *data, void *cookie)
{
    IPCOM_UNUSED_ARG(data);
    IPCOM_UNUSED_ARG(cookie);
}

#endif /* IPNET_USE_PACKET_POOL */

