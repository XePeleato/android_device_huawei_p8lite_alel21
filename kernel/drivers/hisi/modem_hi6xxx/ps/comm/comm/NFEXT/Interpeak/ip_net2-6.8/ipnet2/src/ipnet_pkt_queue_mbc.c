/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_pkt_queue_mbc.c,v $ $Revision: 1.15 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_pkt_queue_mbc.c,v $
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
/* Multiband Container queue.
   Traffic is ordered in 2 or more band, where the priority decreses as the
   band index increases.
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

#ifdef IPNET_USE_IFO_MBC
#ifndef IPNET_USE_IFO_FIFO
#error "MultiBand Container requires the FIFO queue"
#endif

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_syslog.h>

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

typedef struct Ipnet_pkt_queue_mbc_struct
{
    Ipnet_pkt_queue   hdr;
    Ip_u32            number_of_bands;
    Ip_u32            default_band;
    Ipnet_pkt_queue **bands;
    Ipnet_classifier *classifier;
}
Ipnet_pkt_queue_mbc;


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
 *                  ipnet_pkt_queue_mbc_insert_default_queue
 *===========================================================================
 * Description: Inserts a FIFO queue in the MBC.
 * Parameters:  pq - Pointer to where the new FIFO should be stored.
 *              netif - The interface the queue belongs to.
 * Returns:     0  = success
 *              <0 = error code
 *
 */
IP_STATIC int
ipnet_pkt_queue_mbc_insert_default_queue(Ipnet_pkt_queue_mbc *q, int band)
{
    struct Ipnet_ifqueue fifo;

    ipcom_strcpy(fifo.ifq_name, q->hdr.netif->ipcom.name);
    ipcom_strcpy(fifo.ifq_type, "fifo");
    if (q->bands[band] == IP_NULL)
        fifo.ifq_id = IP_IFQ_ID_NONE;
    else
        fifo.ifq_id = q->bands[band]->id;
    fifo.ifq_parent_id            = q->hdr.id;
    fifo.ifq_data.fifo.fifo_limit = IPNET_PKT_QUEUE_DEFAULT_FIFO_SIZE;
    return ipnet_pkt_queue_new(&fifo, q->hdr.netif, &q->bands[band]);
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_mbc_enqueue
 *===========================================================================
 * Description: Inserts a packet into the queue.
 * Parameters:  q - A MBC based queue.
 *              pkt - The packet to store.
 * Returns:     0  = success
 *              <0 = error code
 *
 */
IP_STATIC int
ipnet_pkt_queue_mbc_enqueue(Ipnet_pkt_queue_mbc *q, Ipcom_pkt *pkt)
{
    int              band;
    Ipnet_pkt_queue *child_q;

    band = ipnet_classifier_pkt_to_tag(q->classifier, pkt);
    if (band < 0)
        child_q = q->bands[q->default_band];
    else
        child_q = q->bands[band];

    return child_q->enqueue(child_q, pkt);
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_mbc_dequeue
 *===========================================================================
 * Description: Removed a packet from the queue.
 * Parameters:  q - A MBC based queue.
 * Returns:     A packet from the queue or IP_NULL if all bands of the queue
 *              are empty.
 *
 */
IP_STATIC Ipcom_pkt *
ipnet_pkt_queue_mbc_dequeue(Ipnet_pkt_queue_mbc *q)
{
    Ipcom_pkt *pkt;
    Ip_u32     band;

    for (band = 0; band < q->number_of_bands; band++)
    {
        if (q->bands[band] == IP_NULL)
            continue;

        pkt = q->bands[band]->dequeue(q->bands[band]);
        if (pkt)
            return pkt;
    }
    return IP_NULL;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_mbc_requeue
 *===========================================================================
 * Description: Puts back a packet so it will be the first to dequeue next
 *              time.
 * Parameters:  q - A MBC packet queue.
 *              pkt - The packet to requeue.
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_queue_mbc_requeue(Ipnet_pkt_queue_mbc *q, Ipcom_pkt *pkt)
{
    int              band;
    Ipnet_pkt_queue *child_q;

    band = ipnet_classifier_pkt_to_tag(q->classifier, pkt);
    if (band < 0)
        child_q = q->bands[q->default_band];
    else
        child_q = q->bands[band];

    child_q->requeue(child_q, pkt);
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_mbc_count
 *===========================================================================
 * Description: Returns the number of packets that is currently queued.
 * Parameters:  q - A MBC based queue.
 * Returns:
 *
 */
IP_STATIC int
ipnet_pkt_queue_mbc_count(Ipnet_pkt_queue_mbc *q)
{
    Ip_u32     band;
    int        count = 0;

    for (band = 0; band < q->number_of_bands; band++)
        count += q->bands[band]->count(q->bands[band]);
    return count;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_mbc_reset
 *===========================================================================
 * Description: Resets the queue, all packets is discarded.
 * Parameters:  q - A MBC based queue.
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_queue_mbc_reset(Ipnet_pkt_queue_mbc *q)
{
    Ip_u32     band;

    for (band = 0; band < q->number_of_bands; band++)
        q->bands[band]->reset(q->bands[band]);
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_mbc_dump
 *===========================================================================
 * Description: Dumps the configuration to the supplied buffer.
 * Parameters:  q - A packet queue.
 *              m - The buffer where the data should be dumped.
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_queue_mbc_dump(Ipnet_pkt_queue_mbc *q, struct Ipnet_ifqueue_mbc *m)
{
    Ip_u32     band;

    m->mbc_bands        = q->number_of_bands;
    m->mbc_default_band = q->default_band;

    for (band = 0; band < q->number_of_bands; band++)
        *(int *)&m->mbc_container.child_ids[band] = q->bands[band]->id;

    *(int *)&m->mbc_container.child_count = (int) q->number_of_bands;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_mbc_configure
 *===========================================================================
 * Description: Configures a queue with new parameters.
 * Parameters:  q - A packet queue.
 *              m - The queue parameters.
 * Returns:     0  = success
 *              <0 = error code
 */
IP_STATIC int
ipnet_pkt_queue_mbc_configure(Ipnet_pkt_queue_mbc *q, struct Ipnet_ifqueue_mbc *m)
{
    Ip_u32 band;

    if (q->number_of_bands)
    {
        IPCOM_LOG0(ERR, "MBC configure: Can only be configured once");
        return -IP_ERRNO_EINVAL;
    }

    if (m->mbc_bands < 2 || m->mbc_bands > IPNET_IFQ_CONTAINER_MAX_COUNT)
    {
        IPCOM_LOG1(ERR,
                   "MBC configure: Number of priority bands must be between 2 and %d",
                   IPNET_IFQ_CONTAINER_MAX_COUNT);
        return -IP_ERRNO_EINVAL;
    }

    if (m->mbc_bands <= m->mbc_default_band)
    {
        IPCOM_LOG1(ERR, "MBC configure: invalid default band, must be [0..%d]",
                   m->mbc_bands - 1);
        return -IP_ERRNO_EINVAL;
    }

    q->bands = ipcom_calloc(m->mbc_bands, sizeof(Ipnet_pkt_queue *));
    if (q->bands == IP_NULL)
    {
        IPCOM_LOG0(CRIT, "MBC configure: out of memory");
        return -IP_ERRNO_ENOMEM;
    }

    q->number_of_bands = m->mbc_bands;
    q->default_band    = m->mbc_default_band;

    for (band = 0; band < q->number_of_bands; band++)
    {
        int                    ret;

        ret = ipnet_pkt_queue_mbc_insert_default_queue(q, band);
        if (ret < 0)
        {
            IPCOM_LOG1(ERR, "MBC configure: Failed to create default queue: %s",
                       ipcom_strerror(-ret));
            return ret;
        }
    }

    return 0;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_mbc_init
 *===========================================================================
 * Description: Configures a queue with new parameters.
 * Parameters:  q - A packet queue.
 * Returns:
 *
 */
IP_STATIC int
ipnet_pkt_queue_mbc_init(Ipnet_pkt_queue_mbc *q)
{
    ipcom_memset((Ip_u8 *) q + sizeof(Ipnet_pkt_queue),
                 0,
                 sizeof(Ipnet_pkt_queue_mbc) - sizeof(Ipnet_pkt_queue));

    q->classifier = ipnet_classifier_new();
    if (q->classifier == IP_NULL)
    {
        IPCOM_LOG0(ERR, "MBC init: Failed to create classifier: out of memory");
        return -IP_ERRNO_ENOMEM;
    }

    return 0;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_mbc_destroy
 *===========================================================================
 * Description: Frees all contained queues.
 * Parameters:  q - A MBC based queue.
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_queue_mbc_destroy(Ipnet_pkt_queue_mbc *q)
{
    Ip_u32     band;

    for (band = 0; band < q->number_of_bands; band++)
        ipnet_pkt_queue_delete(q->bands[band]);
    ipcom_free(q->bands);

    if (q->classifier)
        ipnet_classifier_delete(q->classifier);
}


/*
 *===========================================================================
 *                  ipnet_pkt_queue_mbc_get
 *===========================================================================
 * Description: Returns the queue that matches the specified ID.
 * Parameters:  q - A MBC packet queue.
 *              id - The ID of the contained queue.
 * Returns:     A pointer to the queue or IP_NULL if it was not found.
 *
 */
IP_STATIC struct Ipnet_pkt_queue_struct *
ipnet_pkt_queue_mbc_get(Ipnet_pkt_queue_mbc *q, int id)
{
    Ip_u32           band;
    Ipnet_pkt_queue *c;

    if (q->hdr.id == id)
        return &q->hdr;

    for (band = 0; band < q->number_of_bands; band++)
    {
        c = q->bands[band];
        if (c)
        {
            if (c->id == id)
                return c;
            if (c->c_ops && IP_NULL != (c = c->c_ops->q_get(c, id)))
                return c;
        }
    }
    IPCOM_LOG3(DEBUG2, "MBC: Did not find %d queue inside %s:%d",
               id, q->hdr.type, q->hdr.id);
    return IP_NULL;
}


/*
 *===========================================================================
 *                  ipnet_pkt_queue_mbc_remove
 *===========================================================================
 * Description: Removes an queue from a container queue and put pack a
 *              default queue type.
 * Parameters:  c - The MBC parent queue for 'q'.
 *              q - The queue to remove.
 * Returns:     0 = success, <0 = error code (-IP_ERRNO_xxx)
 *
 */
IP_STATIC int
ipnet_pkt_queue_mbc_remove(Ipnet_pkt_queue_mbc *c, Ipnet_pkt_queue *q)
{
    Ip_u32           band;

    ip_assert(c->hdr.id == q->parent_id);
    for (band = 0; band < c->number_of_bands; band++)
    {
        if (q == c->bands[band])
        {
            int ret = ipnet_pkt_queue_mbc_insert_default_queue(c, band);
            if (ret < 0)
                IPCOM_LOG3(WARNING, "MBC: Failed to replace %s:%d with a fifo in remove(): %s",
                           q->type, q->id, ipcom_strerror(-ret));
            return ret;
        }
    }

    return -IP_ERRNO_ESRCH;
}


/*
 *===========================================================================
 *                  ipnet_pkt_queue_mbc_insert
 *===========================================================================
 * Description: Inserts an queue to a container queue.
 * Parameters:  c - The parent queue for 'q', must be container capable
 *              q - The queue to insert.
 * Returns:     0 = success, <0 = error code (-IP_ERRNO_xxx)
 *
 */
IP_STATIC int
ipnet_pkt_queue_mbc_insert(Ipnet_pkt_queue_mbc *c, Ipnet_pkt_queue *q)
{
    Ip_u32           band;

    for (band = 0; band < c->number_of_bands; band++)
    {
        if (c->bands[band] == IP_NULL)
        {
            /* Is adding the initial queues */
            c->bands[band] = q;
            return 0;
        }

        if (q->id == c->bands[band]->id)
        {
            /* Replace this queue */
            ipnet_pkt_queue_delete(c->bands[band]);
            c->bands[band] = q;
            return 0;
        }
    }
    IPCOM_LOG1(ERR, "MBC: No band has queue id %d", q->id);
    return -IP_ERRNO_ESRCH;
}


/*
 *===========================================================================
 *                  ipnet_pkt_queue_htbc_f_insert
 *===========================================================================
 * Description: Inserts a new filter into a MBC queue.
 * Parameters:  c - A HTBC queue.
 *              filter_id - The ID the filter will have.
 *              rule - The rule to match this filter.
 *              child_queue_id - The queue ID of the child filter that packets
 *              that matches the rule will be queued at.
 * Returns:     0 = success, <0 = error code (-IP_ERRNO_xxx)
 *
 */
IP_STATIC int
ipnet_pkt_queue_mbc_f_insert(Ipnet_pkt_queue_mbc *c,
                             int filter_id,
                             struct Ipnet_classifier_rule *rule,
                             int child_queue_id)
{
    int    ret;
    Ip_u32 band;

    for (band = 0; band < c->number_of_bands; band++)
        if (c->bands[band]->id == child_queue_id)
            break;

    if (0 > (ret = ipnet_classifier_add_mapping(c->classifier, rule, filter_id)))
        return ret;
    return ipnet_classifier_set_tag(c->classifier, filter_id, band);
}


/*
 *===========================================================================
 *                  ipnet_pkt_queue_mbc_f_remove
 *===========================================================================
 * Description: Removes a filter from a MBC queue.
 * Parameters:  mbc - A MBC queue.
 *              queue_id - The
 * Returns:     0 = success, <0 = error code (-IP_ERRNO_xxx)
 *
 */
IP_STATIC int
ipnet_pkt_queue_mbc_f_remove(Ipnet_pkt_queue_mbc *c, int filter_id)
{
    return ipnet_classifier_del_mapping(c->classifier, filter_id);
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                      ipnet_pkt_queue_htbc_template
 *===========================================================================
 * Description: Returns a template for Hierarchical Token Bucket Filter queue.
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ipnet_pkt_queue *
ipnet_pkt_queue_mbc_template()
{
    static Ipnet_pkt_queue_container c_ops = {
        (Ipnet_pkt_queue_container_get_queue)    ipnet_pkt_queue_mbc_get,
        (Ipnet_pkt_queue_container_insert_queue) ipnet_pkt_queue_mbc_insert,
        (Ipnet_pkt_queue_container_remove_queue) ipnet_pkt_queue_mbc_remove,
        (Ipnet_pkt_queue_container_insert_filter) ipnet_pkt_queue_mbc_f_insert,
        (Ipnet_pkt_queue_container_remove_filter) ipnet_pkt_queue_mbc_f_remove,
    };

    static Ipnet_pkt_queue mbc_template = {
        "mbc",
        sizeof(Ipnet_pkt_queue_mbc),
        &c_ops,
        (Ipnet_pkt_queue_enqueue)   ipnet_pkt_queue_mbc_enqueue,
        (Ipnet_pkt_queue_dequeue)   ipnet_pkt_queue_mbc_dequeue,
        (Ipnet_pkt_queue_requeue)   ipnet_pkt_queue_mbc_requeue,
        (Ipnet_pkt_queue_count)     ipnet_pkt_queue_mbc_count,
        (Ipnet_pkt_queue_reset)     ipnet_pkt_queue_mbc_reset,
        (Ipnet_pkt_queue_dump)      ipnet_pkt_queue_mbc_dump,
        (Ipnet_pkt_queue_configure) ipnet_pkt_queue_mbc_configure,
        (Ipnet_pkt_queue_init)      ipnet_pkt_queue_mbc_init,
        (Ipnet_pkt_queue_destroy)   ipnet_pkt_queue_mbc_destroy,
    };
    return &mbc_template;
}

#else
int ipnet_pkt_q_container_mbc_empty_file;
#endif /* IPNET_USE_IFO_MBC */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

