/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_pkt_queue.h,v $ $Revision: 1.18 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_pkt_queue.h,v $
 *   $Author: kenneth $ $Date: 2008-02-11 22:07:13 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPNET_PKT_QUEUE_H
#define IPNET_PKT_QUEUE_H

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


/* The container queues uses fifo as default queues, this is the
   size those queues will have */
#define IPNET_PKT_QUEUE_DEFAULT_FIFO_SIZE 16

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_cstyle.h>
#include <ipcom_pkt.h>
#include <ipcom_sock.h>
#include <ipcom_type.h>

#include "ipnet_classifier.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#if defined (IPNET_USE_IFO_HTBC) \
    || defined (IPNET_USE_IFO_MBC) \
    || defined (IPNET_USE_IFO_NULL) \
    || defined (IPNET_USE_IFO_NETEMU) \
    || defined (IPNET_USE_IFO_RED) \
    || defined (IPNET_USE_IFO_DPAF) \
    || defined (IPNET_USE_IFO_FIFO)
#define IPNET_HAS_AT_LEAST_ONE_PKT_QUEUE
#endif


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

struct Ipnet_pkt_queue_struct;

/*
 *===========================================================================
 *                  Ipnet_pkt_queue_container_get_queue
 *===========================================================================
 * Description: Returns the queue that matches the specified ID.
 * Parameters:  c - A packet queue.
 *              id - The ID of the contained queue.
 * Returns:     A pointer to the queue or IP_NULL if it was not found.
 *
 */
typedef struct Ipnet_pkt_queue_struct * (*Ipnet_pkt_queue_container_get_queue)
         (struct Ipnet_pkt_queue_struct *c, int id);


/*
 *===========================================================================
 *                  Ipnet_pkt_queue_container_insert_queue
 *===========================================================================
 * Description: Inserts an queue to a container queue.
 * Parameters:  c - The parent queue for 'q', must be container capable
 *              q - The queue to insert.
 * Returns:     0 = success, <0 = error code (-IPNET_ERRNO_xxx)
 *
 */
typedef int (*Ipnet_pkt_queue_container_insert_queue)
         (struct Ipnet_pkt_queue_struct *c, struct Ipnet_pkt_queue_struct *q);


/*
 *===========================================================================
 *                  Ipnet_pkt_queue_container_remove_queue
 *===========================================================================
 * Description: Removes an queue from a container queue.
 * Parameters:  c - The parent queue for 'q', must be container capable
 *              q - The queue to remove.
 * Returns:     0 = success, <0 = error code (-IPNET_ERRNO_xxx)
 *
 */
typedef int (*Ipnet_pkt_queue_container_remove_queue)
         (struct Ipnet_pkt_queue_struct *c, struct Ipnet_pkt_queue_struct *q);


/*
 *===========================================================================
 *                  Ipnet_pkt_queue_container_insert_filter
 *===========================================================================
 * Description: Adds a filter to a queue.
 * Parameters:  c - The queue the filter should be attached to.
 *              filter_íd - The ID that the filter will get.
 *              rule - The fields that must match to trigger this filter.
 *              child_queue_id - The child queue packets matching this filter
 *              will be queued at.
 * Returns:     >0 = success, value is filter tag ID.
 *              <0 = error code (-IPNET_ERRNO_xxx)
 *
 */
typedef int (*Ipnet_pkt_queue_container_insert_filter)
         (struct Ipnet_pkt_queue_struct *c,
          int filter_id,
          struct Ipnet_classifier_rule *rule,
          int child_queue_id);


/*
 *===========================================================================
 *                  Ipnet_pkt_queue_container_remove_filter
 *===========================================================================
 * Description: Removes a filter for the specified queue.
 * Parameters:  c - The queue the filter should be removed from.
 *              filter_id - The ID of the filter.
 * Returns:     0 = success, <0 = error code (-IPNET_ERRNO_xxx)
 *
 */
typedef int (*Ipnet_pkt_queue_container_remove_filter)
         (struct Ipnet_pkt_queue_struct *c, int filter_id);


typedef struct Ipnet_pkt_queue_container_struct
{
    Ipnet_pkt_queue_container_get_queue     q_get;
    Ipnet_pkt_queue_container_insert_queue  q_insert;
    Ipnet_pkt_queue_container_remove_queue  q_remove;
    Ipnet_pkt_queue_container_insert_filter f_insert;
    Ipnet_pkt_queue_container_remove_filter f_remove;
}
Ipnet_pkt_queue_container;


/*
 *===========================================================================
 *                      Ipnet_pkt_queue_enqueue
 *===========================================================================
 * Description: Enqueues a packet.
 * Parameters:  q - A packet queue.
 *              pkt - The packet to queue.
 * Returns:     0 = success, <0 = error code (-IPNET_ERRNO_xxx)
 *
 */
typedef int (*Ipnet_pkt_queue_enqueue)(struct Ipnet_pkt_queue_struct *q,
                                       Ipcom_pkt *pkt);


/*
 *===========================================================================
 *                      Ipnet_pkt_queue_dequeue
 *===========================================================================
 * Description: Dequeus a packet.
 * Parameters:  q - A packet queue.
 * Returns:     A queued packet or IP_NULL if the queue was empty.
 *
 */
typedef Ipcom_pkt * (*Ipnet_pkt_queue_dequeue)(struct Ipnet_pkt_queue_struct *q);


/*
 *===========================================================================
 *                      Ipnet_pkt_queue_requeue
 *===========================================================================
 * Description: Puts back a packet so it will be the first to dequeue next
 *              time.
 * Parameters:  q - A packet queue.
 * Returns:
 *
 */
typedef void (*Ipnet_pkt_queue_requeue)(struct Ipnet_pkt_queue_struct *q,
                                        Ipcom_pkt *pkt);


/*
 *===========================================================================
 *                      Ipnet_pkt_queue_count
 *===========================================================================
 * Description: Returns the number of packet that is currently queued.
 * Parameters:  q - A packet queue.
 * Returns:     The number of packets in the queue.
 *
 */
typedef int (*Ipnet_pkt_queue_count)(struct Ipnet_pkt_queue_struct *q);


/*
 *===========================================================================
 *                      Ipnet_pkt_queue_reset
 *===========================================================================
 * Description: Frees all packet that is queued.
 * Parameters:  q - A packet queue.
 * Returns:
 *
 */
typedef void (*Ipnet_pkt_queue_reset)(struct Ipnet_pkt_queue_struct *q);


/*
 *===========================================================================
 *                      Ipnet_pkt_queue_dump
 *===========================================================================
 * Description: Dumps the configuration to the supplied buffer.
 * Parameters:  q - A packet queue.
 *              ifq - The buffer where the data should be dumped.
 * Returns:
 *
 */
typedef void (*Ipnet_pkt_queue_dump)(struct Ipnet_pkt_queue_struct *q, union Ipnet_ifqueue_data *d);


/*
 *===========================================================================
 *                      Ipnet_pkt_queue_configure
 *===========================================================================
 * Description: Configures a queue with new parameters.
 * Parameters:  q - A packet queue.
 *              ifq - The queue parameters.
 * Returns:     The number of elements currently in the queue.
 *
 */
typedef int (*Ipnet_pkt_queue_configure)(struct Ipnet_pkt_queue_struct *q, union Ipnet_ifqueue_data *d);


/*
 *===========================================================================
 *                      Ipnet_pkt_queue_init
 *===========================================================================
 * Description: Frees the resources held by the queue including all queued
 *              packets.
 * Parameters:  q - A packet queue.
 * Returns:     0 = success, <0 = error code (-IPNET_ERRNO_xxx)
 *
 */
typedef int (*Ipnet_pkt_queue_init)(struct Ipnet_pkt_queue_struct *q);


/*
 *===========================================================================
 *                      Ipnet_pkt_queue_destroy
 *===========================================================================
 * Description: Frees the resources held by the queue including all queued
 *              packets.
 * Parameters:  q - A packet queue.
 * Returns:
 *
 */
typedef void (*Ipnet_pkt_queue_destroy)(struct Ipnet_pkt_queue_struct *q);


typedef struct Ipnet_pkt_queue_struct
{
    const char                *type;      /* Specifies the type of
                                             queue */
    Ip_size_t                  impl_size; /* Space need by the queue
                                             implementation (including
                                             this header) */
    Ipnet_pkt_queue_container *c_ops;
    Ipnet_pkt_queue_enqueue    enqueue;
    Ipnet_pkt_queue_dequeue    dequeue;
    Ipnet_pkt_queue_requeue    requeue;
    Ipnet_pkt_queue_count      count;
    Ipnet_pkt_queue_reset      reset;
    Ipnet_pkt_queue_dump       dump;
    Ipnet_pkt_queue_configure  configure;
    Ipnet_pkt_queue_init       init;
    Ipnet_pkt_queue_destroy    destroy;   /* Do not call directly, use
                                             ipnet_pkt_queue_delete()
                                             instead */
    int                        id;        /* The ID of this queue */
    int                        parent_id; /* The ID of the parent
                                             queue or IP_IFQ_ID_NONE
                                             if this a root queue */
    struct Ipnet_netif_struct *netif;     /* The network interface
                                             this queue is assiged
                                             to */
    struct Ipnet_pkt_queue_struct *prev;  /* Previous queue, used when
                                             located in a container
                                             queue */
    struct Ipnet_pkt_queue_struct *next;  /* Next queue, used when
                                             located in a container
                                             queue */
}
Ipnet_pkt_queue;


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

IP_GLOBAL int  ipnet_pkt_queue_new(struct Ipnet_ifqueue *ifq,
                                   struct Ipnet_netif_struct *netif,
                                   Ipnet_pkt_queue **q);
IP_GLOBAL void ipnet_pkt_queue_delete(Ipnet_pkt_queue *q);

IP_GLOBAL int  ipnet_pkt_queue_init(void);

#ifdef IPNET_USE_IFO_FIFO
IP_GLOBAL Ipnet_pkt_queue * ipnet_pkt_queue_fifo_template(void);
#endif
#ifdef IPNET_USE_IFO_DPAF
IP_GLOBAL Ipnet_pkt_queue * ipnet_pkt_queue_dpaf_template(void);
#endif
#ifdef IPNET_USE_IFO_RED
IP_GLOBAL Ipnet_pkt_queue * ipnet_pkt_queue_red_template(void);
#endif
#ifdef IPNET_USE_IFO_NETEMU
IP_GLOBAL Ipnet_pkt_queue * ipnet_pkt_queue_netemu_template(void);
#endif
#ifdef IPNET_USE_IFO_NULL
IP_GLOBAL Ipnet_pkt_queue * ipnet_pkt_queue_null_template(void);
#endif
#ifdef IPNET_USE_IFO_MBC
IP_GLOBAL Ipnet_pkt_queue * ipnet_pkt_queue_mbc_template(void);
#endif
#ifdef IPNET_USE_IFO_HTBC
IP_GLOBAL Ipnet_pkt_queue * ipnet_pkt_queue_htbc_template(void);
#endif


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
