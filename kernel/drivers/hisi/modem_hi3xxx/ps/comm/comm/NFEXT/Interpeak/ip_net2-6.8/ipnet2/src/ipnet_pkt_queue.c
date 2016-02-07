/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_pkt_queue.c,v $ $Revision: 1.27 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_pkt_queue.c,v $
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
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_hash.h>
#include <ipcom_syslog.h>
#include <ipcom_type.h>

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_netif.h"
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

#ifdef IPNET_HAS_AT_LEAST_ONE_PKT_QUEUE
/*
 *===========================================================================
 *                    ipnet_pkt_queue_key_hash
 *===========================================================================
 * Description: Computes a hash value for the specified queue type.
 * Parameters:  type - The type queue.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_pkt_queue_key_hash(const char *type)
{
    return ipcom_hash_update(type, ipcom_strlen(type), 0);
}


/*
 *===========================================================================
 *                    ipnet_pkt_queue_obj_hash
 *===========================================================================
 * Description: Computes a hash value for the specified queue type.
 * Parameters:  template - The template for an queue type.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_pkt_queue_obj_hash(Ipnet_pkt_queue *template)
{
    return ipcom_hash_update(template->type, ipcom_strlen(template->type), 0);
}


/*
 *===========================================================================
 *                    ipnet_pkt_queue_hash_cmp
 *===========================================================================
 * Description: Compares the type of the template with the specified type.
 * Parameters:  template - A template for a queue type.
 *              type - A queue type.
 * Returns:     IP_TRUE if the specified type matches the template.
 *
 */
IP_STATIC Ip_bool
ipnet_pkt_queue_hash_cmp(Ipnet_pkt_queue *template, const char *type)
{
    return ipcom_strcmp(template->type, type) == 0;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_register
 *===========================================================================
 * Description: Creates a new queue of the specified type.
 * Parameters:  template - A template for
 * Returns:     The new queue or IP_NULL if the type is unsupported.
 *
 */
IP_STATIC int
ipnet_pkt_queue_register(Ipnet_pkt_queue *template)
{
    switch (ipcom_hash_add(ipnet->queue_db, template))
    {
    case IPCOM_SUCCESS:
        IPCOM_LOG1(DEBUG2, "Registered queue type %s", template->type);
        return 0;
    case IPCOM_ERR_DUPLICATE:
        IPCOM_LOG1(WARNING, "Queue type %s is already registered", template->type);
        return -IP_ERRNO_EEXIST;
    default:
        IPCOM_LOG1(CRIT, "Failed to register queue type %s, probably out of memory",
                   template->type);
        return -IP_ERRNO_ENOMEM;
    }
}
#endif /* IPNET_HAS_AT_LEAST_ONE_PKT_QUEUE */


/*
 *===========================================================================
 *                      ipnet_pkt_queue_reset
 *===========================================================================
 * Description: Generic implementation of reset.
 *              Removes and frees all queued packets.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_queue_reset(Ipnet_pkt_queue *q)
{
    Ipcom_pkt *pkt;

    while (IP_NULL != (pkt = q->dequeue(q)))
    {
        IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_NETIF_DEQUEUE);
        ipcom_pkt_free(pkt);
    }
}


/*
 *===========================================================================
 *                      ipnet_pkt_select_queue_id
 *===========================================================================
 * Description: Selects an unique queue id.
 * Parameters:  root - The root queue for the interface the new queue
 *              belongs to, will be IP_NULL if this a new root queue.
 * Returns:     Unique (on the current interface) ID.
 *
 */
IP_STATIC int
ipnet_pkt_select_queue_id(Ipnet_pkt_queue *root)
{
    int id;

    if (root == IP_NULL)
        return IP_IFQ_ID_DEFAULT;

    id = 1000;
    while (root->c_ops->q_get(root, id))
        id++;
    return id;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                      ipnet_pkt_queue_new
 *===========================================================================
 * Description: Creates a new queue of the specified type.
 * Parameters:  ifq - The parameters this queue should have.
 *              netif - The network interface this queue is assiged to.
 *              ret_q - IP_NULL or pointer to where the newly created
 *              queue pointer should be stored.
 * Returns:     The new queue or IP_NULL if the type is unsupported.
 *
 */
IP_GLOBAL int
ipnet_pkt_queue_new(struct Ipnet_ifqueue *ifq,
                    Ipnet_netif *netif,
                    Ipnet_pkt_queue **ret_q)
{
    Ipnet_pkt_queue *template_queue;
    Ipnet_pkt_queue *q;
    Ipnet_pkt_queue *parent_q = IP_NULL;
    int              ret;
    int              parent_id = ifq->ifq_parent_id;
    int              id = ifq->ifq_id;
    const char      *type = ifq->ifq_type;
    Ip_bool          is_inserted = IP_FALSE;

    ip_assert(ifq != IP_NULL);
    ip_assert(netif != IP_NULL);

    template_queue = ipcom_hash_get(ipnet->queue_db, (char *) type);
    if (template_queue == IP_NULL)
    {
        IPCOM_LOG1(ERR, "%s is an unknown queue type", type);
        return -IP_ERRNO_EMEDIUMTYPE;
    }

    if (parent_id != IP_IFQ_ID_NONE)
    {
        /* Not a root queue */
        Ipnet_pkt_queue *root_q = netif->snd_queue;

        if (root_q == IP_NULL || root_q->c_ops == IP_NULL)
        {
            IPCOM_LOG2(ERR, "Parent ID %d was specified for queue %s, but the root queue is not a container",
                       parent_id, type);
            return -IP_ERRNO_EINVAL;
        }
        parent_q = root_q->c_ops->q_get(root_q, parent_id);
        if (parent_q == IP_NULL)
        {
            IPCOM_LOG1(ERR, "No (parent) queue with id %d was found", parent_id);
            return -IP_ERRNO_ESRCH;
        }
        if (parent_q->c_ops == IP_NULL)
        {
            IPCOM_LOG2(ERR, "Queue %s:%d is not a container", parent_q->type, parent_q->id);
            return -IP_ERRNO_EINVAL;
        }

        if (id != IP_IFQ_ID_NONE)
        {
            Ipnet_pkt_queue *dup_q;
            /* Verify that the ID specified is potentially
               valid. That means that the ID is either unique or that
               it is an ID of one of the child queues of "parent_q" */

            dup_q = root_q->c_ops->q_get(root_q, id);
            if (dup_q)
            {
                if (dup_q->parent_id != parent_q->id)
                {
                    IPCOM_LOG1(ERR, "Queue ID %d is already in use", id);
                    return -IP_ERRNO_EINVAL;
                }
            }
            /* else: unique */
        }
    }

    ip_assert(template_queue->impl_size >= sizeof(Ipnet_pkt_queue));
    q = ipcom_calloc(1, template_queue->impl_size);
    if (q == IP_NULL)
    {
        IPCOM_LOG2(CRIT, "Failed to allocate %d bytes when creating an queue of type '%s'",
                   template_queue->impl_size, type);
        return -IP_ERRNO_ENOMEM;
    }
    ipcom_memcpy(q, template_queue, sizeof(*q));

    q->netif     = netif;
    q->parent_id = parent_id;
    if (id != IP_IFQ_ID_NONE)
        q->id = id;
    else
        q->id = ipnet_pkt_select_queue_id(parent_q ? netif->snd_queue : IP_NULL);

    if (parent_q == IP_NULL)
    {
        if (netif->snd_queue != IP_NULL)
            ipnet_pkt_queue_delete(netif->snd_queue);
        netif->snd_queue = q;
    }

    if (q->reset == IP_NULL)
        /* Use the standard implementation to empty queue */
        q->reset = ipnet_pkt_queue_reset;

    if (parent_q)
    {
        if ((ret = parent_q->c_ops->q_insert(parent_q, q)) < 0)
        {
            IPCOM_LOG3(CRIT, "Failed to insert %s queue into parent %d (%s)",
                       q->type, parent_q->id, parent_q->type);
            goto errout;
        }
        is_inserted = IP_TRUE;
    }

    if ((ret = q->init(q)) < 0)
    {
        IPCOM_LOG3(ERR, "Failed to initialize queue %s:%d:%d",
                   q->type, q->id, q->parent_id);
        goto errout;
    }

    ret = q->configure(q, &ifq->ifq_data);
    if (ret < 0)
        goto errout;

    if (ret_q)
        *ret_q = q;
    return 0;

 errout:
    if (is_inserted)
        (void) parent_q->c_ops->q_remove(parent_q, q);
    else
        ipnet_pkt_queue_delete(q);
    return ret;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_delete
 *===========================================================================
 * Description: Frees all resources held by the queue.
 * Parameters:  q - The queue to delete.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_pkt_queue_delete(Ipnet_pkt_queue *q)
{
    if (q != IP_NULL)
    {
        if (q->parent_id == IP_IFQ_ID_NONE)
            q->netif->snd_queue = IP_NULL;

        q->reset(q);
        ip_assert(q->count(q) == 0);
        if (q->destroy)
            q->destroy(q);

        ipcom_free(q);
    }
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_init
 *===========================================================================
 * Description: Registers all queues that was specified in the configuration
 *              (<ipnet>/config/ipnet_config.h).
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_pkt_queue_init(void)
{
    int ret = 0;

#ifdef IPNET_HAS_AT_LEAST_ONE_PKT_QUEUE
    ipnet->queue_db = ipcom_hash_new((Ipcom_hash_obj_func) ipnet_pkt_queue_obj_hash,
                                     (Ipcom_hash_key_func) ipnet_pkt_queue_key_hash,
                                     (Ipcom_hash_cmp_func) ipnet_pkt_queue_hash_cmp);
    if (ipnet->queue_db == IP_NULL)
    {
        IPCOM_LOG0(CRIT, "Failed to create pkt queue database");
        return -IP_ERRNO_ENOMEM;
    }

#ifdef IPNET_USE_IFO_FIFO
    if (0 < (ret = ipnet_pkt_queue_register(ipnet_pkt_queue_fifo_template())))
        return ret;
#endif
#ifdef IPNET_USE_IFO_DPAF
    if (0 < (ret = ipnet_pkt_queue_register(ipnet_pkt_queue_dpaf_template())))
        return ret;
#endif
#ifdef IPNET_USE_IFO_RED
    if (0 < (ret = ipnet_pkt_queue_register(ipnet_pkt_queue_red_template())))
        return ret;
#endif
#ifdef IPNET_USE_IFO_NETEMU
    if (0 < (ret = ipnet_pkt_queue_register(ipnet_pkt_queue_netemu_template())))
        return ret;
#endif
#ifdef IPNET_USE_IFO_NULL
    if (0 < (ret = ipnet_pkt_queue_register(ipnet_pkt_queue_null_template())))
        return ret;
#endif
#ifdef IPNET_USE_IFO_MBC
    if (0 < (ret = ipnet_pkt_queue_register(ipnet_pkt_queue_mbc_template())))
        return ret;
#endif
#ifdef IPNET_USE_IFO_HTBC
    if (0 < (ret = ipnet_pkt_queue_register(ipnet_pkt_queue_htbc_template())))
        return ret;
#endif
#endif /* IPNET_HAS_AT_LEAST_ONE_PKT_QUEUE */
    return ret;
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

