/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_pkt_queue_dpaf.c,v $ $Revision: 1.12 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_pkt_queue_dpaf.c,v $
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
/* Drop Precedence Aware FIFO queue */

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

#ifdef IPNET_USE_IFO_DPAF

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_pkt.h>
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

#define IPNET_DPAF_PKT_NEXT(pkt)            (*(Ipcom_pkt **)&(pkt)->link_cookie)
#define IPNET_DPAF_PKT_PREV(pkt)            ((pkt)->next_fragment)
#define IPNET_DPAF_PKT_DROPTAG(pkt)         ((int) (pkt)->recurs_level)
#define IPNET_DPAF_PKT_SET_DROPTAG(pkt, dt) ((pkt)->recurs_level = (unsigned) dt)
#define IPNET_DPAF_SET_PKT_PQ_INDEX(pkt, pqi)                           \
    ipcom_pkt_set_int(pkt, IPNET_PKT_INFO_DPAF_PQ_INDEX, pqi, unsigned)
#define IPNET_DPAF_GET_PKT_PQ_INDEX(pkt)                            \
    ipcom_pkt_get_int(pkt, IPNET_PKT_INFO_DPAF_PQ_INDEX, unsigned)

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/* DPAF queue (linked list) */
typedef struct Ipnet_pkt_queue_dpaf_struct
{
    Ipnet_pkt_queue hdr;
    Ipcom_pkt      *head;     /* Next packet to dequeue */
    Ipcom_pkt      *tail;     /* Last packet to dequeue */
    int             next_pq_tag; /* Next drop tag */
    int             requeue_pq_index; /* Drop tag of the last dequeued packet */
    Ipcom_pqueue   *mdp;      /* The first packet in this queue is the first one to drop in medium precedence */
    Ipcom_pqueue   *hdp;      /* The first packet in this queue is the first one to drop in high precedence */
    int             limit;    /* Maximum number of packets that can be queued */
    int             count;    /* Current number of packets in the queue */
}
Ipnet_pkt_queue_dpaf;


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
 *                    ipnet_pkt_queue_dpaf_cmp_func
 *===========================================================================
 * Description: Stores the priority queue index the packet has.
 * Parameters:  pkt1 - A packet.
 *              pkt2 - Another packet.
 * Returns:     <0 if pkt1 should be removed from the queue after pkt2
 *               0 if pkt1 and pkt2 should be removed at the same time
 *              >0 if pkt1 should be removed from the queue before pkt2
 *
 */
IP_STATIC int
ipnet_pkt_queue_dpaf_cmp_func(IP_CONST Ipcom_pkt *pkt1, IP_CONST Ipcom_pkt *pkt2)
{
    return IPNET_DPAF_PKT_DROPTAG(pkt1) - IPNET_DPAF_PKT_DROPTAG(pkt2);
}


/*
 *===========================================================================
 *                    ipnet_pkt_queue_dpaf_store_index
 *===========================================================================
 * Description: Stores the priority queue index the packet has.
 * Parameters:  pkt - A packet.
 *              oindex - The index the packet has.
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_queue_dpaf_store_index(Ipcom_pkt *pkt, int oindex)
{
    IPNET_DPAF_SET_PKT_PQ_INDEX(pkt, oindex);
}


/*
 *===========================================================================
 *                    ipnet_pkt_queue_dpaf_dequeue2
 *===========================================================================
 * Description: Removes the specified packet from the queue.
 * Parameters:  q - A DPAF based queue.
 *              pkt - The packet to dequeue. IP_NULL is valid, this function
 *              will do nothing in that case.
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_queue_dpaf_dequeue2(Ipnet_pkt_queue_dpaf *q, Ipcom_pkt *pkt)
{
    if (pkt == IP_NULL)
        return;

    switch (IPCOM_PKT_GET_DROP_PRECEDENCE(pkt))
    {
    case IPCOM_PKT_DROP_PRECEDENCE_LOW:
        break;
    case IPCOM_PKT_DROP_PRECEDENCE_MEDIUM:
    default:
        (void) ipcom_pqueue_remove(q->mdp, IPNET_DPAF_GET_PKT_PQ_INDEX(pkt));
        break;
    case IPCOM_PKT_DROP_PRECEDENCE_HIGH:
        (void) ipcom_pqueue_remove(q->hdp, IPNET_DPAF_GET_PKT_PQ_INDEX(pkt));
        break;
    }

    if (IPNET_DPAF_PKT_PREV(pkt) == IP_NULL)
        q->head = IPNET_DPAF_PKT_NEXT(pkt);
    else
        IPNET_DPAF_PKT_NEXT(IPNET_DPAF_PKT_PREV(pkt)) = IPNET_DPAF_PKT_PREV(pkt);

    if (IPNET_DPAF_PKT_NEXT(pkt) == IP_NULL)
        q->tail = IPNET_DPAF_PKT_PREV(pkt);
    else
        IPNET_DPAF_PKT_PREV(IPNET_DPAF_PKT_NEXT(pkt)) = IPNET_DPAF_PKT_NEXT(pkt);

    if (q->head != IP_NULL)
    {
        ip_assert(q->tail != IP_NULL);
        IPNET_DPAF_PKT_PREV(q->head) = IP_NULL;
        IPNET_DPAF_PKT_NEXT(q->tail) = IP_NULL;
    }

    IPNET_DPAF_PKT_PREV(pkt) = IP_NULL;
    IPNET_DPAF_PKT_NEXT(pkt) = IP_NULL;

    q->count--;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_dpaf_drop
 *===========================================================================
 * Description: Removes and frees the specified packet from the queue.
 * Parameters:  q - A DPAF based queue.
 *              pkt - The packet to drop.
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_queue_dpaf_drop(Ipnet_pkt_queue_dpaf *q, Ipcom_pkt *pkt)
{
    ipnet_pkt_queue_dpaf_dequeue2(q, pkt);
    ipcom_pkt_free(pkt);
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_dpaf_enqueue2
 *===========================================================================
 * Description: Inserts a packet into the queue.
 * Parameters:  q - A DPAF based queue.
 *              pkt - The packet to store.
 *              drop_tag - The packet, within a drop precedence class, with
 *              the highest drop_tag (modulo 32) will be dropped first.
 * Returns:     0  = success
 *              <0 = error code
 *
 */
IP_STATIC int
ipnet_pkt_queue_dpaf_enqueue2(Ipnet_pkt_queue_dpaf *q, Ipcom_pkt *pkt, int drop_tag)
{
    Ip_err err;
    int    dp = IPCOM_PKT_GET_DROP_PRECEDENCE(pkt);

    switch (dp)
    {
    case IPCOM_PKT_DROP_PRECEDENCE_LOW:
        err = IPCOM_SUCCESS;
        break;
    case IPCOM_PKT_DROP_PRECEDENCE_MEDIUM:
    case IPCOM_PKT_DROP_PRECEDENCE_HIGH:
    default:
        IPNET_DPAF_PKT_SET_DROPTAG(pkt, drop_tag);
        err = ipcom_pqueue_insert(dp == IPCOM_PKT_DROP_PRECEDENCE_HIGH ? q->hdp : q->mdp,
                                  pkt);
        break;
    }
    if (err != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "DPAF enqueue: Failed to add packet to priority queue: err=%d",
                   err);
        return -IP_ERRNO_ENOMEM;
    }

    /* Add last */
    if (q->tail)
    {
        IPNET_DPAF_PKT_PREV(pkt) = q->tail;
        IPNET_DPAF_PKT_NEXT(q->tail) = pkt;
        q->tail = pkt;
    }
    else
    {
        IPNET_DPAF_PKT_PREV(pkt) = IP_NULL;
        q->tail = pkt;
        q->head = pkt;
    }
    IPNET_DPAF_PKT_NEXT(pkt) = IP_NULL;

    ip_assert(q->limit > q->count);
    q->count++;
    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_NETIF_OUTQ);

    return 0;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_dpaf_enqueue
 *===========================================================================
 * Description: Inserts a packet into the queue.
 * Parameters:  q - A DPAF based queue.
 *              pkt - The packet to store.
 * Returns:     0  = success
 *              <0 = error code
 *
 */
IP_STATIC int
ipnet_pkt_queue_dpaf_enqueue(Ipnet_pkt_queue_dpaf *q,
                             Ipcom_pkt *pkt)
{
    if (q->limit == q->count)
    {
        switch (IPCOM_PKT_GET_DROP_PRECEDENCE(pkt))
        {
        case IPCOM_PKT_DROP_PRECEDENCE_LOW:
            if (ipcom_pqueue_get_next(q->hdp) == IP_NULL)
            {
                ipnet_pkt_queue_dpaf_drop(q, ipcom_pqueue_get_next(q->mdp));
                break;
            }
            /* Fall through */
        case IPCOM_PKT_DROP_PRECEDENCE_MEDIUM:
        default:
            ipnet_pkt_queue_dpaf_drop(q, ipcom_pqueue_get_next(q->hdp));
            break;
        case IPCOM_PKT_DROP_PRECEDENCE_HIGH:
            break;
        }
    }

    if (q->limit == q->count)
        return -IP_ERRNO_ENOBUFS;

    return ipnet_pkt_queue_dpaf_enqueue2(q, pkt, q->next_pq_tag++);
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_dpaf_dequeue
 *===========================================================================
 * Description: Removed a packet from the queue.
 * Parameters:  q - A DPAF based queue.
 * Returns:     A packet from the queue or IP_NULL if the queue is empty.
 *
 */
IP_STATIC Ipcom_pkt *
ipnet_pkt_queue_dpaf_dequeue(Ipnet_pkt_queue_dpaf *q)
{
    Ipcom_pkt *pkt = q->head;

    if (pkt == IP_NULL)
    {
        ip_assert(q->count == 0);
        return IP_NULL;
    }

    ipnet_pkt_queue_dpaf_dequeue2(q, pkt);

    q->requeue_pq_index = IPNET_DPAF_GET_PKT_PQ_INDEX(pkt);
    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_NETIF_DEQUEUE);
    IPNET_DPAF_PKT_SET_DROPTAG(pkt, 0);
    return pkt;
}


/*
 *===========================================================================
 *                    ipnet_pkt_queue_dpaf_requeue
 *===========================================================================
 * Description: Puts back a packet so it will be the first to dequeue next
 *              time (unless it has been dropped before that).
 * Parameters:  q - A DPAF packet queue.
 *              pkt - The packet to requeue.
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_queue_dpaf_requeue(Ipnet_pkt_queue_dpaf *q, Ipcom_pkt *pkt)
{
    (void)ipnet_pkt_queue_dpaf_enqueue2(q, pkt, q->requeue_pq_index);
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_dpaf_count
 *===========================================================================
 * Description: Returns the number of packets that is currently queued.
 * Parameters:  q - A DPAF based queue.
 * Returns:
 *
 */
IP_STATIC int
ipnet_pkt_queue_dpaf_count(Ipnet_pkt_queue_dpaf *q)
{
    return q->count;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_dpaf_dump
 *===========================================================================
 * Description: Dumps the configuration to the supplied buffer.
 * Parameters:  q - A packet queue.
 *              d - The buffer where the data should be dumped.
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_queue_dpaf_dump(Ipnet_pkt_queue_dpaf *q, struct Ipnet_ifqueue_dpaf *d)
{
    d->dpaf_limit = q->limit;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_dpaf_configure
 *===========================================================================
 * Description: Configures a queue with new parameters.
 * Parameters:  q - A packet queue.
 *              d - The queue parameters.
 * Returns:     0  = success
 *              <0 = error code
 */
IP_STATIC int
ipnet_pkt_queue_dpaf_configure(Ipnet_pkt_queue_dpaf *q, struct Ipnet_ifqueue_dpaf *d)
{
    q->limit = d->dpaf_limit;
    return 0;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_dpaf_destroy
 *===========================================================================
 * Description: Destroys a DPAF queue.
 * Parameters:  q - A DPAF packet queue.
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_queue_dpaf_destroy(Ipnet_pkt_queue_dpaf *q)
{
    if (q->mdp)
        ipcom_pqueue_delete(q->mdp);
    if (q->hdp)
        ipcom_pqueue_delete(q->hdp);
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_dpaf_init
 *===========================================================================
 * Description: Configures a queue with new parameters.
 * Parameters:  q - A packet queue.
 * Returns:
 *
 */
IP_STATIC int
ipnet_pkt_queue_dpaf_init(Ipnet_pkt_queue_dpaf *q)
{
    ipcom_memset((Ip_u8 *) q + sizeof(Ipnet_pkt_queue),
                 0,
                 sizeof(Ipnet_pkt_queue_dpaf) - sizeof(Ipnet_pkt_queue));
    q->mdp = ipcom_pqueue_new((Ipcom_pqueue_cmp_func) ipnet_pkt_queue_dpaf_cmp_func,
                              (Ipcom_pqueue_store_index) ipnet_pkt_queue_dpaf_store_index);
    q->hdp = ipcom_pqueue_new((Ipcom_pqueue_cmp_func) ipnet_pkt_queue_dpaf_cmp_func,
                              (Ipcom_pqueue_store_index) ipnet_pkt_queue_dpaf_store_index);

    if (q->mdp == IP_NULL || q->hdp == IP_NULL)
    {
        ipnet_pkt_queue_dpaf_destroy(q);
        IPCOM_LOG0(ERR, "DPAF init: Failed to create priority queue");
        return -IP_ERRNO_ENOMEM;
    }

    return 0;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                      ipnet_pkt_queue_dpaf_template
 *===========================================================================
 * Description: Returns a template for DPAF queue.
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ipnet_pkt_queue *
ipnet_pkt_queue_dpaf_template()
{
    static Ipnet_pkt_queue dpaf_template =
        {
            "dpaf",
            sizeof(Ipnet_pkt_queue_dpaf),
            IP_NULL, /* Not a container */
            (Ipnet_pkt_queue_enqueue)   ipnet_pkt_queue_dpaf_enqueue,
            (Ipnet_pkt_queue_dequeue)   ipnet_pkt_queue_dpaf_dequeue,
            (Ipnet_pkt_queue_requeue)   ipnet_pkt_queue_dpaf_requeue,
            (Ipnet_pkt_queue_count)     ipnet_pkt_queue_dpaf_count,
            IP_NULL,
            (Ipnet_pkt_queue_dump)      ipnet_pkt_queue_dpaf_dump,
            (Ipnet_pkt_queue_configure) ipnet_pkt_queue_dpaf_configure,
            (Ipnet_pkt_queue_init)      ipnet_pkt_queue_dpaf_init,
            (Ipnet_pkt_queue_destroy)   ipnet_pkt_queue_dpaf_destroy,
        };
    return &dpaf_template;
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#else
int ipnet_pkt_queue_dpaf_empty_file;
#endif /* IPNET_USE_IFO_DPAF */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

