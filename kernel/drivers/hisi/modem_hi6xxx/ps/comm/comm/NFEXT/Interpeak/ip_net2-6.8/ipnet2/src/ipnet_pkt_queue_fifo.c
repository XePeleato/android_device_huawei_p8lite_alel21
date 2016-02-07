/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_pkt_queue_fifo.c,v $ $Revision: 1.12 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_pkt_queue_fifo.c,v $
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
/* Single band packet fifo */

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

#ifdef IPNET_USE_IFO_FIFO

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_hash.h>
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

#define IPNET_FIFO_PKT_NEXT(pkt)       (*(Ipcom_pkt **)&(pkt)->link_cookie)


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/* FIFO queue (linked list) */
typedef struct Ipnet_pkt_queue_fifo_struct
{
    Ipnet_pkt_queue hdr;
    Ipcom_pkt      *head;   /* Points to the next packet to be dequeued */
    Ipcom_pkt      *tail;   /* Points to the last packet in the queue */
    int             limit;  /* Maximum number of packets that can be queued */
    int             count;  /* Current number of packets in the queue */
}
Ipnet_pkt_queue_fifo;


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
 *                      ipnet_pkt_queue_fifo_enqueue
 *===========================================================================
 * Description: Inserts a packet into the queue.
 * Parameters:  fifo - A FIFO based queue.
 *              pkt - The packet to store.
 * Returns:     0  = success
 *              <0 = error code
 *
 */
IP_STATIC int
ipnet_pkt_queue_fifo_enqueue(Ipnet_pkt_queue_fifo *fifo,
                             Ipcom_pkt *pkt)
{
    ip_assert(fifo->count != 0 || fifo->head == IP_NULL);
    ip_assert(fifo->count != 0 || fifo->tail == IP_NULL);
    IP_DEBUGLINE(ip_assert(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_ALLOC)));

    if (fifo->count >= fifo->limit)
        return -IP_ERRNO_ENOBUFS;

    if (fifo->tail)
        IPNET_FIFO_PKT_NEXT(fifo->tail) = pkt;
    else
        fifo->head = pkt;

    fifo->tail = pkt;
    fifo->count++;
    IPNET_FIFO_PKT_NEXT(pkt) = IP_NULL;
    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_NETIF_OUTQ);

    return 0;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_fifo_dequeue
 *===========================================================================
 * Description: Removed a packet from the queue.
 * Parameters:  fifo - A FIFO based queue.
 * Returns:     A packet from the queue or IP_NULL if the queue is empty.
 *
 */
IP_STATIC Ipcom_pkt *
ipnet_pkt_queue_fifo_dequeue(Ipnet_pkt_queue_fifo *fifo)
{
    Ipcom_pkt *pkt = fifo->head;

    if (pkt)
    {
        fifo->count--;
        fifo->head = IPNET_FIFO_PKT_NEXT(pkt);
        if (fifo->head == IP_NULL)
            fifo->tail = IP_NULL;

        IP_DEBUGLINE(ip_assert(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_ALLOC)));
        IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_NETIF_DEQUEUE);

        IPNET_FIFO_PKT_NEXT(pkt) = IP_NULL;
    }

    return pkt;
}


/*
 *===========================================================================
 *                    ipnet_pkt_queue_fifo_requeue
 *===========================================================================
 * Description: Puts back a packet so it will be the first to dequeue next
 *              time.
 * Parameters:  htbc - A FIFO packet queue.
 *
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_queue_fifo_requeue(Ipnet_pkt_queue_fifo *fifo, Ipcom_pkt *pkt)
{
    IP_DEBUGLINE(ip_assert(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_ALLOC)));

    if (fifo->count == 0)
    {
        (void)ipnet_pkt_queue_fifo_enqueue(fifo, pkt);
        return;
    }
    IPNET_FIFO_PKT_NEXT(pkt) = fifo->head;
    fifo->head = pkt;
    fifo->count++;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_fifo_count
 *===========================================================================
 * Description: Returns the number of packets that is currently queued.
 * Parameters:  fifo - A FIFO based queue.
 * Returns:
 *
 */
IP_STATIC int
ipnet_pkt_queue_fifo_count(Ipnet_pkt_queue_fifo *fifo)
{
    return fifo->count;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_fifo_dump
 *===========================================================================
 * Description: Dumps the configuration to the supplied buffer.
 * Parameters:  q - A packet queue.
 *              f - The buffer where the data should be dumped.
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_queue_fifo_dump(Ipnet_pkt_queue_fifo *q, struct Ipnet_ifqueue_fifo *f)
{
    f->fifo_limit = q->limit;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_fifo_configure
 *===========================================================================
 * Description: Configures a queue with new parameters.
 * Parameters:  q - A packet queue.
 *              f - The queue parameters.
 * Returns:     0  = success
 *              <0 = error code
 */
IP_STATIC int
ipnet_pkt_queue_fifo_configure(Ipnet_pkt_queue_fifo *q, struct Ipnet_ifqueue_fifo *f)
{
    q->limit = f->fifo_limit;
    return 0;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_fifo_init
 *===========================================================================
 * Description: Configures a queue with new parameters.
 * Parameters:  q - A packet queue.
 * Returns:
 *
 */
IP_STATIC int
ipnet_pkt_queue_fifo_init(Ipnet_pkt_queue_fifo *q)
{
    q->limit = 0;
    q->count = 0;
    q->head  = IP_NULL;
    q->tail  = IP_NULL;
    return 0;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                      ipnet_pkt_queue_fifo_template
 *===========================================================================
 * Description: Returns a template for FIFO queue.
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ipnet_pkt_queue *
ipnet_pkt_queue_fifo_template()
{
    static Ipnet_pkt_queue fifo_template =
        {
            "fifo",
            sizeof(Ipnet_pkt_queue_fifo),
            IP_NULL, /* Not a container */
            (Ipnet_pkt_queue_enqueue)   ipnet_pkt_queue_fifo_enqueue,
            (Ipnet_pkt_queue_dequeue)   ipnet_pkt_queue_fifo_dequeue,
            (Ipnet_pkt_queue_requeue)   ipnet_pkt_queue_fifo_requeue,
            (Ipnet_pkt_queue_count)     ipnet_pkt_queue_fifo_count,
            IP_NULL,
            (Ipnet_pkt_queue_dump)      ipnet_pkt_queue_fifo_dump,
            (Ipnet_pkt_queue_configure) ipnet_pkt_queue_fifo_configure,
            (Ipnet_pkt_queue_init)      ipnet_pkt_queue_fifo_init,
            IP_NULL,
        };
    return &fifo_template;
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#else
int ipnet_pkt_queue_fifo_empty_file;
#endif /* IPNET_USE_IFO_FIFO */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

