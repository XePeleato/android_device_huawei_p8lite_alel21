/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_pkt_queue_null.c,v $ $Revision: 1.4 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_pkt_queue_null.c,v $
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
/* Black hole queue that is used to drop packets silently */

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

#ifdef IPNET_USE_IFO_NULL

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_pkt.h>

#include "ipnet_pkt_queue.h"

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
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                      ipnet_pkt_queue_null_enqueue
 *===========================================================================
 * Description: Inserts a packet into the queue.
 * Parameters:  null - A NULL queue.
 *              pkt - The packet to store.
 * Returns:     0  = success
 *              <0 = error code
 *
 */
IP_STATIC int
ipnet_pkt_queue_null_enqueue(Ipnet_pkt_queue *null,
                             Ipcom_pkt *pkt)
{
    (void) null;
    ipcom_pkt_free(pkt);
    return 0;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_null_dequeue
 *===========================================================================
 * Description: Removed a packet from the queue.
 * Parameters:  null - A NULL queue.
 * Returns:     IP_NULL since this queue is always empty.
 *
 */
IP_STATIC Ipcom_pkt *
ipnet_pkt_queue_null_dequeue(Ipnet_pkt_queue *null)
{
    (void) null;
    return IP_NULL;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_null_count
 *===========================================================================
 * Description: Returns the number of packets that is currently queued.
 * Parameters:  null - A NULL queue.
 * Returns:
 *
 */
IP_STATIC int
ipnet_pkt_queue_null_count(Ipnet_pkt_queue *null)
{
    (void) null;
    return 0;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_null_dump
 *===========================================================================
 * Description: Dumps the configuration to the supplied buffer.
 * Parameters:  q - A packet queue.
 *              ifq - The buffer where the data should be dumped.
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_queue_null_dump(Ipnet_pkt_queue *q, struct Ipnet_ifqueue *ifq)
{
    (void) q;
    (void) ifq;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_null_configure
 *===========================================================================
 * Description: Configures a queue with new parameters.
 * Parameters:  q - A packet queue.
 *              ifq - The queue parameters.
 * Returns:     0  = success
 *              <0 = error code
 */
IP_STATIC int
ipnet_pkt_queue_null_configure(Ipnet_pkt_queue *q, struct Ipnet_ifqueue *ifq)
{
    (void) q;
    (void) ifq;
    return 0;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_null_init
 *===========================================================================
 * Description: Configures a queue with new parameters.
 * Parameters:  q - A packet queue.
 * Returns:
 *
 */
IP_STATIC int
ipnet_pkt_queue_null_init(Ipnet_pkt_queue *q)
{
    (void) q;
    return 0;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                      ipnet_pkt_queue_null_template
 *===========================================================================
 * Description: Returns a template for NULL queue.
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ipnet_pkt_queue *
ipnet_pkt_queue_null_template()
{
    static Ipnet_pkt_queue null_template =
        {
            "null",
            sizeof(Ipnet_pkt_queue),
            IP_NULL, /* Not a container */
            (Ipnet_pkt_queue_enqueue)   ipnet_pkt_queue_null_enqueue,
            (Ipnet_pkt_queue_dequeue)   ipnet_pkt_queue_null_dequeue,
            (Ipnet_pkt_queue_requeue)   ipnet_pkt_queue_null_enqueue,
            (Ipnet_pkt_queue_count)     ipnet_pkt_queue_null_count,
            IP_NULL,
            (Ipnet_pkt_queue_dump)      ipnet_pkt_queue_null_dump,
            (Ipnet_pkt_queue_configure) ipnet_pkt_queue_null_configure,
            (Ipnet_pkt_queue_init)      ipnet_pkt_queue_null_init,
            IP_NULL,
        };
    return &null_template;
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#else
int ipnet_pkt_queue_null_empty_file;
#endif /* IPNET_USE_IFO_NULL */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

