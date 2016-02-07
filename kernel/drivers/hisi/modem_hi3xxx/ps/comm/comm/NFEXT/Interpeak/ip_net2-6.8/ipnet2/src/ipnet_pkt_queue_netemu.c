/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_pkt_queue_netemu.c,v $ $Revision: 1.21 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_pkt_queue_netemu.c,v $
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
/* Network emulator.
   This queue can
   - Add latency
   - Drop packets randomly or at a deterministic rate
   - Corrupt packets randomly or at a deterministic rate */

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

#ifdef IPNET_USE_IFO_NETEMU

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_pqueue.h>
#include <ipcom_syslog.h>
#include <ipcom_type.h>

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_pkt_queue.h"

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_NETEMU_DROP_PATTERN_INACTIVE  255

#define IPNET_NETEMU_SET_PKT_SEQ_NUM(pkt, seq)                          \
    ipcom_pkt_set_int(pkt, IPNET_PKT_INFO_NETEMU_SEQ_NUM, seq, Ip_u32)
#define IPNET_NETEMU_GET_PKT_SEQ_NUM(pkt)                               \
    ipcom_pkt_get_int(pkt, IPNET_PKT_INFO_NETEMU_SEQ_NUM, Ip_u32)

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/* netemu queue */
typedef struct Ipnet_pkt_queue_netemu_struct
{
    Ipnet_pkt_queue hdr;
    Ipcom_pqueue   *pq;
    Ipnet_timeout  *tmo;
    int             limit;            /* Maximum number of packets that can be queued */
    Ip_u32          enqueue_count;    /* Total number of packets that has been queued */
    Ip_u32          min_latency_msec; /* Minimum latency added to packets in milliseconds */
    Ip_u32          max_latency_msec; /* Maximum latency added to packets in milliseconds */
    Ip_bool         random_drop;      /* IP_FALSE -> drop every drop_probability packet
                                         IP_TRUE  -> there is 1/every drop_probability
                                         chance that at packet is dropped */
    Ip_u32          drop_probability; /* 1 means every packet dropped, 10 means that
                                         every 10 packet is dropped */
    Ip_u32          drop_pattern;     /* Bit field where 1 means drop and 0 means pass */
    Ip_u8           drop_pattern_len; /* Number of bits that is valid in 'drop_pattern' */
    Ip_u8           drop_pattern_index; /* Current bit used */
    Ip_bool         random_corrupt;   /* IP_FALSE -> corrupt every corrupt_probability packet
                                         IP_TRUE  -> there is 1/every corrupt_probability
                                         chance that a packet is corrupted */
    Ip_u32          corrupt_probability;/* 1 means every packet corrupted, 10 means that
                                         every 10 packet is corrupted */
    Ip_u32          seq;              /* Sequence number that is increased each time
                                         a packet is added */
}
Ipnet_pkt_queue_netemu;


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
 *                    ipnet_pkt_queue_netemu_pq_cmp
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_pkt_queue_netemu_pq_cmp(const Ipcom_pkt *pkt1, const Ipcom_pkt *pkt2)
{
    if (IPNET_PKT_GET_TIMEOUT_ABS(pkt1) == IPNET_PKT_GET_TIMEOUT_ABS(pkt2))
        return IPCOM_IS_LT(IPNET_NETEMU_GET_PKT_SEQ_NUM(pkt1),
                           IPNET_NETEMU_GET_PKT_SEQ_NUM(pkt2))
            ? 1 : -1;
    if (IPCOM_IS_LT(IPNET_PKT_GET_TIMEOUT_ABS(pkt1),
                    IPNET_PKT_GET_TIMEOUT_ABS(pkt2)))
        return 1;
    return -1;
}


/*
 *===========================================================================
 *                    ipnet_pkt_queue_netemu_corrupt
 *===========================================================================
 * Description: Inserts a packet into the queue.
 * Parameters:  pkt - The packet to corrupt
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_queue_netemu_corrupt(Ipcom_pkt *pkt)
{
    int i;
    int c = 1 + ipcom_rand() % 10;

    for (i = 0; i < c; i++)
        pkt->data[pkt->start + ipcom_rand() % (pkt->end - pkt->start)] = (Ip_u8) ipcom_rand();
}


/*
 *===========================================================================
 *                  ipnet_pkt_queue_netemu_to_string
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPNET_DEBUG
IP_STATIC int
ipnet_pkt_queue_netemu_to_string(Ipnet_pkt_queue_netemu *netemu, char *buf, Ip_size_t buf_len)
{
    return ipcom_snprintf(buf, buf_len, "NetEmu  if %s  limit %d  size %d",
                          netemu->hdr.netif->ipcom.name, netemu->limit, ipcom_pqueue_size(netemu->pq));
}
#endif


/*
 *===========================================================================
 *                  ipnet_pkt_queue_netemu_retry_cb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_queue_netemu_retry_cb(Ipnet_pkt_queue_netemu *netemu)
{
    Ipcom_pkt       *pkt;
    Ipnet_pkt_queue *q = netemu->hdr.netif->snd_queue;

    while (IP_NULL != (pkt = q->dequeue(q)))
    {
        int ret = ipnet_if_drv_output(q->netif, pkt);
        if (ret < 0)
            q->requeue(q, pkt);
    }
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_netemu_enqueue
 *===========================================================================
 * Description: Inserts a packet into the queue.
 * Parameters:  q - A NETEMU based queue.
 *              pkt - The packet to store.
 * Returns:     0  = success
 *              <0 = error code
 *
 */
IP_STATIC int
ipnet_pkt_queue_netemu_enqueue(Ipnet_pkt_queue_netemu *q,
                               Ipcom_pkt *pkt)
{
    Ip_u32  tmo_msec;
    Ip_bool drop = IP_FALSE;
    Ip_bool corrupt = IP_FALSE;

    if (ipcom_pqueue_size(q->pq) == q->limit)
    {
        IPCOM_LOG0(DEBUG, "NETEMU enqueue: queue full");
        return -IP_ERRNO_ENOBUFS;
    }

    ++q->enqueue_count;

    if (q->drop_probability)
    {
        Ip_bool restart_pattern = IP_FALSE;

        if(q->random_drop)
            restart_pattern = !((Ip_u32) ipcom_rand() % q->drop_probability);
        else
            restart_pattern = !(q->enqueue_count % q->drop_probability);

        if (restart_pattern)
        {
            if (q->drop_pattern_len == 0)
                drop = IP_TRUE;
            else
                q->drop_pattern_index = 0;
        }

        if (q->drop_pattern_index < q->drop_pattern_len)
        {
            drop = IP_BIT_ISSET(q->drop_pattern, 1 << q->drop_pattern_index);
            ++q->drop_pattern_index;
        }
    }

    if (drop)
    {
        IPCOM_LOG0(DEBUG, "NETEMU enqueue: dropping packet");
        return -IP_ERRNO_ECOMM;
    }

    if (q->corrupt_probability)
    {
        if (q->random_corrupt)
            corrupt = !((Ip_u32) ipcom_rand() % q->corrupt_probability);
        else
            corrupt = !(q->enqueue_count % q->corrupt_probability);
    }

    if (corrupt)
    {
        Ipcom_pkt *corrupt_pkt;
        /* Must clone the packet since IPNET does not expect the data
           be corrupted on the original packet */
        corrupt_pkt = ipnet_pkt_clone(pkt, IP_FALSE);
        if (corrupt_pkt != IP_NULL)
        {
            IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_NETIF_DEQUEUE);
            ipcom_pkt_free(pkt);
            pkt = corrupt_pkt;
            IPCOM_LOG0(DEBUG, "NETEMU enqueue: corrupting packet");
            ipnet_pkt_queue_netemu_corrupt(pkt);
        }
    }

    tmo_msec = q->min_latency_msec;
    if (q->max_latency_msec > q->min_latency_msec)
        tmo_msec += (Ip_u32) ipcom_rand() % (q->max_latency_msec - q->min_latency_msec);

    ipnet->msec_now = ipnet_msec_now();
    IPNET_PKT_SET_TIMEOUT_ABS(pkt, tmo_msec + ipnet->msec_now);

    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_NETIF_OUTQ);
    IPNET_NETEMU_SET_PKT_SEQ_NUM(pkt, ++q->seq);
    return ipcom_pqueue_insert(q->pq, pkt) == IPCOM_SUCCESS ? 0 : -IP_ERRNO_ENOMEM;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_netemu_dequeue
 *===========================================================================
 * Description: Removed a packet from the queue.
 * Parameters:  q - A NETEMU based queue.
 * Returns:     A packet from the queue or IP_NULL if the queue is empty.
 *
 */
IP_STATIC Ipcom_pkt *
ipnet_pkt_queue_netemu_dequeue(Ipnet_pkt_queue_netemu *q)
{
    Ipcom_pkt *pkt;
    Ip_u32     msec_now;

    pkt = ipcom_pqueue_get_next(q->pq);
    if (pkt == IP_NULL)
        return IP_NULL;

    msec_now = ipnet->msec_now;

    if (IPCOM_IS_GT(IPNET_PKT_GET_TIMEOUT_ABS(pkt), msec_now))
    {
        /* More latency to add */
        if (q->tmo == IP_NULL)
        {
            (void)ipnet_timeout_schedule(IPNET_PKT_GET_TIMEOUT_ABS(pkt) - msec_now,
                                         (Ipnet_timeout_handler) ipnet_pkt_queue_netemu_retry_cb,
                                         q,
                                         &q->tmo);
        }
        return IP_NULL;
    }
    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_NETIF_DEQUEUE);
    return ipcom_pqueue_remove_next(q->pq);
}


/*
 *===========================================================================
 *                    ipnet_pkt_queue_netemu_requeue
 *===========================================================================
 * Description: Puts back a packet so it will be the first to dequeue next
 *              time.
 * Parameters:  netemu - A NETEMU packet queue.
 *
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_queue_netemu_requeue(Ipnet_pkt_queue_netemu *netemu, Ipcom_pkt *pkt)
{
    (void)ipcom_pqueue_insert(netemu->pq, pkt);
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_netemu_count
 *===========================================================================
 * Description: Returns the number of packets that is currently queued.
 * Parameters:  netemu - A NETEMU based queue.
 * Returns:
 *
 */
IP_STATIC int
ipnet_pkt_queue_netemu_count(Ipnet_pkt_queue_netemu *netemu)
{
    return ipcom_pqueue_size(netemu->pq);
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_netemu_reset
 *===========================================================================
 * Description: Resets the queue, all packets is discarded.
 * Parameters:  netemu - A NETEMU based queue.
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_queue_netemu_reset(Ipnet_pkt_queue_netemu *netemu)
{
    Ipcom_pkt *pkt;

    while (IP_NULL != (pkt = ipcom_pqueue_remove_next(netemu->pq)))
    {
        IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_NETIF_DEQUEUE);
        ipcom_pkt_free(pkt);
    }
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_netemu_dump
 *===========================================================================
 * Description: Dumps the configuration to the supplied buffer.
 * Parameters:  q - A packet queue.
 *              n - The buffer where the data should be dumped.
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_queue_netemu_dump(Ipnet_pkt_queue_netemu *q, struct Ipnet_ifqueue_netemu *n)
{
    n->netemu_limit               = q->limit;
    n->netemu_min_latency         = q->min_latency_msec;
    n->netemu_max_latency         = q->max_latency_msec;
    n->netemu_random_drop         = q->random_drop;
    n->netemu_drop_probability    = q->drop_probability;
    n->netemu_random_corrupt      = q->random_corrupt;
    n->netemu_corrupt_probability = q->corrupt_probability;
    n->netemu_drop_pattern        = q->drop_pattern;
    n->netemu_drop_pattern_len    = q->drop_pattern_len;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_netemu_configure
 *===========================================================================
 * Description: Configures a queue with new parameters.
 * Parameters:  q - A packet queue.
 *              n - The queue parameters.
 * Returns:     0  = success
 *              <0 = error code
 */
IP_STATIC int
ipnet_pkt_queue_netemu_configure(Ipnet_pkt_queue_netemu *q, struct Ipnet_ifqueue_netemu *n)
{
    q->limit               = n->netemu_limit;
    q->min_latency_msec    = n->netemu_min_latency;
    q->max_latency_msec    = n->netemu_max_latency;
    q->random_drop         = n->netemu_random_drop;
    q->drop_probability    = n->netemu_drop_probability;
    q->random_corrupt      = n->netemu_random_corrupt;
    q->corrupt_probability = n->netemu_corrupt_probability;
    q->drop_pattern        = n->netemu_drop_pattern;
    q->drop_pattern_len    = n->netemu_drop_pattern_len;
    return 0;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_netemu_init
 *===========================================================================
 * Description: Configures a queue with new parameters.
 * Parameters:  q - A packet queue.
 * Returns:
 *
 */
IP_STATIC int
ipnet_pkt_queue_netemu_init(Ipnet_pkt_queue_netemu *q)
{
    ipcom_memset((Ip_u8 *) q + sizeof(Ipnet_pkt_queue),
                 0,
                 sizeof(Ipnet_pkt_queue_netemu) - sizeof(Ipnet_pkt_queue));
    q->drop_pattern_index = IPNET_NETEMU_DROP_PATTERN_INACTIVE;

    q->pq = ipcom_pqueue_new((Ipcom_pqueue_cmp_func) ipnet_pkt_queue_netemu_pq_cmp,
                             ipcom_pqueue_nop_store_index);
    return 0;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_netemu_destroy
 *===========================================================================
 * Description: Frees all contained queues.
 * Parameters:  netemu - A NETEMU based queue.
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_queue_netemu_destroy(Ipnet_pkt_queue_netemu *netemu)
{
    ipnet_timeout_cancel(netemu->tmo);
    ipnet_pkt_queue_netemu_reset(netemu);
    ipcom_pqueue_delete(netemu->pq);
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                      ipnet_pkt_queue_netemu_template
 *===========================================================================
 * Description: Returns a template for NETEMU queue.
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ipnet_pkt_queue *
ipnet_pkt_queue_netemu_template()
{
    static Ipnet_pkt_queue netemu_template =
        {
            "netemu",
            sizeof(Ipnet_pkt_queue_netemu),
            IP_NULL, /* Not a container */
            (Ipnet_pkt_queue_enqueue)   ipnet_pkt_queue_netemu_enqueue,
            (Ipnet_pkt_queue_dequeue)   ipnet_pkt_queue_netemu_dequeue,
            (Ipnet_pkt_queue_requeue)   ipnet_pkt_queue_netemu_requeue,
            (Ipnet_pkt_queue_count)     ipnet_pkt_queue_netemu_count,
            (Ipnet_pkt_queue_reset)     ipnet_pkt_queue_netemu_reset,
            (Ipnet_pkt_queue_dump)      ipnet_pkt_queue_netemu_dump,
            (Ipnet_pkt_queue_configure) ipnet_pkt_queue_netemu_configure,
            (Ipnet_pkt_queue_init)      ipnet_pkt_queue_netemu_init,
            (Ipnet_pkt_queue_destroy)   ipnet_pkt_queue_netemu_destroy,
        };
#ifdef IPNET_DEBUG
    ipnet_timeout_to_string((Ipnet_timeout_handler) ipnet_pkt_queue_netemu_retry_cb,
                            (Ipnet_timeout_to_string_f) ipnet_pkt_queue_netemu_to_string);
#endif

    return &netemu_template;
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#else
int ipnet_pkt_queue_netemu_empty_file;
#endif /* IPNET_USE_IFO_NETEMU */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

