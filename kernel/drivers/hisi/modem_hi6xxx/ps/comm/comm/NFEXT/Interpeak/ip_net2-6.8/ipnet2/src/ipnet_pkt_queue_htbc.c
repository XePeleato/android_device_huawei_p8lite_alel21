/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_pkt_queue_htbc.c,v $ $Revision: 1.25 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_pkt_queue_htbc.c,v $
 *   $Author: jonas $
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
/* Hierarchical Token Bucket Container, used to set a maximum speed for
   a specific type of traffic */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#include "ipnet_config.h"

#define IPNET_PKT_QUEUE_HTBC_MIN_TIMEOUT       10


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef IPNET_USE_IFO_HTBC
#ifndef IPNET_USE_IFO_FIFO
#error "Hierarchical Token Bucket Container requires the FIFO queue"
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

typedef struct Ipnet_pkt_queue_htbc_struct
{
    Ipnet_pkt_queue   hdr;
    Ip_u32            byterate;    /* Byterate for this queue, in bytes per second */
    Ip_s32            tokens;      /* Tokens left, each byte consumes 1 token */
    Ip_s32            token_limit; /* Maximum number of tokens this queue can have
                                      at any time, must be >=MTU of the interface,
                                      but a higher value should be used. How high
                                      depends on the resolution of ipcom_microtime()
                                      on target OS */
    Ip_u32            last_recred; /* Last time new tokes was added */
    Ipnet_pkt_queue  *dequeue_q;
    Ipnet_pkt_queue  *default_q;   /* Default leaf queue, used for everything
                                      that does not match something else */
    Ipcom_pkt        *requeued_pkt;
    Ip_u32            msec_before_retry; /* Minimum time to wait before retry sending
                                      this time is time to get enough tokens to send
                                      one full MTU + minimum wait time */
    Ipnet_timeout    *tmo;         /* Is != IP_NULL if a timeout is currently active */
    Ipnet_classifier *classifier;
}
Ipnet_pkt_queue_htbc;


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

IP_STATIC struct Ipnet_pkt_queue_struct *
ipnet_pkt_queue_htbc_get(Ipnet_pkt_queue_htbc *htbc, int id);

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
 *                  ipnet_pkt_queue_htbc_to_string
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPNET_DEBUG
IP_STATIC int
ipnet_pkt_queue_htbc_to_string(Ipnet_pkt_queue_htbc *htbc, char *buf, int buf_len)
{
    return ipcom_snprintf(buf, buf_len, "HTBC tokens %u", (unsigned) htbc->tokens);
}
#endif


/*
 *===========================================================================
 *                  ipnet_pkt_queue_htbc_retry_cb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_queue_htbc_retry_cb(Ipnet_pkt_queue_htbc *htbc)
{
#ifdef IP_PORT_LKM
    Ipcom_pkt       *pkt;
	Ipnet_netif     *netif = htbc->hdr.netif;
    Ipnet_pkt_queue *q = &htbc->hdr;

	ipcom_pkt_output_done(&netif->ipcom, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
    if (htbc->tmo == IP_NULL && htbc->tokens <= 0)
    {
        /* Still more packet waiting to be sent */
        (void) ipnet_timeout_schedule(htbc->msec_before_retry,
                                      (Ipnet_timeout_handler) ipnet_pkt_queue_htbc_retry_cb,
                                      htbc,
                                      &htbc->tmo);
    }
#else /* !IP_PORT_LKM */
    Ipcom_pkt       *pkt;
    Ipnet_pkt_queue *q = &htbc->hdr;

    if (IP_NULL != (pkt = q->dequeue(q)))
    {
        int ret = ipnet_if_drv_output(q->netif, pkt);
        if (ret == -IP_ERRNO_EWOULDBLOCK)
        {
            q->requeue(q, pkt);
            (void) ipnet_timeout_schedule(htbc->msec_before_retry,
                                          (Ipnet_timeout_handler) ipnet_pkt_queue_htbc_retry_cb,
                                          htbc,
                                          &htbc->tmo);
        }
    }
#endif
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_htbc_enqueue
 *===========================================================================
 * Description: Inserts a packet into the queue.
 * Parameters:  q - A HTBC based queue.
 *              pkt - The packet to store.
 * Returns:     0  = success
 *              <0 = error code
 *
 */
IP_STATIC int
ipnet_pkt_queue_htbc_enqueue(Ipnet_pkt_queue_htbc *q, Ipcom_pkt *pkt)
{
    int              child_id;
    Ipnet_pkt_queue *child_q;

#ifdef IP_PORT_LKM
    ipnet_pkt_queue_htbc_retry_cb(q);
#endif

    child_id = ipnet_classifier_pkt_to_tag(q->classifier, pkt);
    if (child_id != IPNET_CLASSIFIER_INVALID_TAG)
    {
        child_q = ipnet_pkt_queue_htbc_get(q, child_id);

        if (child_q)
            return child_q->enqueue(child_q, pkt);

        IPCOM_LOG1(WARNING,
                   "HTBC enqueue: the packet was sent to a removed queue (%d)",
                   child_id);
    }

    return q->default_q->enqueue(q->default_q, pkt);
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_htbc_dequeue
 *===========================================================================
 * Description: Removed a packet from the queue, all contained queues
 *              are tries in a Round-Robin fashion until a packet is returned.
 *              The maximum rate is also controlled in this function.
 * Parameters:  htbc - A HTBC based queue.
 * Returns:     A packet from the queue or IP_NULL if the queue is empty or
 *              is throttled.
 *
 */
IP_STATIC Ipcom_pkt *
ipnet_pkt_queue_htbc_dequeue(Ipnet_pkt_queue_htbc *htbc)
{
    Ipnet_pkt_queue *start_q;
    Ipnet_pkt_queue *q;
    Ipcom_pkt       *pkt;
    Ip_u32           now;
    Ip_s32           new_tokens;

    if (htbc->requeued_pkt)
    {
        /* Tokens has already been deducted for this packet */
        pkt = htbc->requeued_pkt;
        htbc->requeued_pkt = IP_NULL;
        return pkt;
    }

    now = ipnet_msec_now();
    new_tokens = ((now - htbc->last_recred) * htbc->byterate) >> 10;
    htbc->last_recred = now;
    if (new_tokens + htbc->tokens > htbc->token_limit)
        htbc->tokens = htbc->token_limit;
    else
        htbc->tokens += new_tokens;

    if (htbc->tokens <= 0)
    {
        /* Schedule a retry timeout */
        if (htbc->tmo != IP_NULL)
            return IP_NULL;

        (void) ipnet_timeout_schedule(htbc->msec_before_retry,
                                      (Ipnet_timeout_handler) ipnet_pkt_queue_htbc_retry_cb,
                                      htbc,
                                      &htbc->tmo);
        IPCOM_LOG3(DEBUG2, "Out of tokens on %s queue %s:%d",
                   htbc->hdr.netif->ipcom.name, htbc->hdr.type, htbc->hdr.id);
        return IP_NULL;
    }

    if (htbc->tmo != IP_NULL)
        /* Packet could be sent and a timeout is running */
        ipnet_timeout_cancel(htbc->tmo);

    q = start_q = htbc->dequeue_q;
    while (IP_NULL == (pkt = q->dequeue(q))
           && q->next != start_q)
        q = q->next;

    if (pkt)
        htbc->tokens -= pkt->end - pkt->start;

    htbc->dequeue_q = q->next;
    return pkt;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_htbc_requeue
 *===========================================================================
 * Description: Puts back a packet so it will be the first to dequeue next
 *              time.
 * Parameters:  htbc - A HTBC packet queue.
 *              pkt - The packet that should be requeued.
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_queue_htbc_requeue(Ipnet_pkt_queue_htbc *htbc, Ipcom_pkt *pkt)
{
    ip_assert(htbc->requeued_pkt == IP_NULL);

    htbc->requeued_pkt = pkt;
    pkt->next = IP_NULL;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_htbc_count
 *===========================================================================
 * Description: Returns the number of packets that is currently queued.
 * Parameters:  htbc - A HTBC based queue.
 * Returns:
 *
 */
IP_STATIC int
ipnet_pkt_queue_htbc_count(Ipnet_pkt_queue_htbc *htbc)
{
    Ipnet_pkt_queue *q = htbc->default_q;
    int              count = (htbc->requeued_pkt ? 1 : 0);

    if (q != IP_NULL)
    {
        do
        {
            count += q->count(q);
            q = q->next;
        } while (q != htbc->default_q);
    }
    return count;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_htbc_reset
 *===========================================================================
 * Description: Resets the queue, all packets is discarded.
 * Parameters:  htbc - A HTBC based queue.
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_queue_htbc_reset(Ipnet_pkt_queue_htbc *htbc)
{
    Ipnet_pkt_queue *q = htbc->default_q;

    if (q != IP_NULL)
    {
        do
        {
            q->reset(q);
            q = q->next;
        } while (q != htbc->default_q);
    }
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_htbc_dump
 *===========================================================================
 * Description: Dumps the configuration to the supplied buffer.
 * Parameters:  c - A HTBC packet queue.
 *              h - The buffer where the data should be dumped.
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_queue_htbc_dump(Ipnet_pkt_queue_htbc *c, struct Ipnet_ifqueue_htbc *h)
{
    Ipnet_pkt_queue *q = c->default_q;
    int                  count = 0;

    h->htbc_byterate    = c->byterate;
    h->htbc_token_limit = c->token_limit;
    *(int *)&h->htbc_default_id = c->default_q->id;

    do
    {
        *(int *)&h->htbc_container.child_ids[count++] = q->id;
        q = q->next;
    } while (q != c->default_q);

    *(int *)&h->htbc_container.child_count = count;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_htbc_configure
 *===========================================================================
 * Description: Configures a queue with new parameters.
 * Parameters:  q - A packet queue.
 *              h - The queue parameters.
 * Returns:     0  = success
 *              <0 = error code
 */
IP_STATIC int
ipnet_pkt_queue_htbc_configure(Ipnet_pkt_queue_htbc *q, struct Ipnet_ifqueue_htbc *h)
{
    Ipnet_netif *netif = q->hdr.netif;

    if (h->htbc_token_limit > 0
        && h->htbc_token_limit < netif->ipcom.mtu + netif->ipcom.link_hdr_size)
    {
        IPCOM_LOG4(ERR, "Token limit (%d) to small for %s:%d, must be >=%d",
                   h->htbc_token_limit,
                   q->hdr.type,
                   q->hdr.id,
                   netif->ipcom.mtu + netif->ipcom.link_hdr_size);
        return -IP_ERRNO_EINVAL;

    }

    if (h->htbc_token_limit)
        q->token_limit = h->htbc_token_limit;
    else
        q->token_limit = q->hdr.netif->ipcom.mtu * 4;

    q->byterate          = h->htbc_byterate;
    q->tokens            = IP_MIN(q->tokens, q->token_limit);
    q->last_recred       = ipnet_msec_now();
    q->msec_before_retry = 2 * 1000 * netif->ipcom.mtu / q->byterate
        + IPNET_PKT_QUEUE_HTBC_MIN_TIMEOUT;

    return 0;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_htbc_init
 *===========================================================================
 * Description: Configures a queue with new parameters.
 * Parameters:  q - A packet queue.
 * Returns:
 *
 */
IP_STATIC int
ipnet_pkt_queue_htbc_init(Ipnet_pkt_queue_htbc *q)
{
    int                       ret;
    struct Ipnet_ifqueue      fifo;

    ipcom_memset((Ip_u8 *) q + sizeof(Ipnet_pkt_queue),
                 0,
                 sizeof(Ipnet_pkt_queue_htbc) - sizeof(Ipnet_pkt_queue));

    q->classifier = ipnet_classifier_new();
    if (q->classifier == IP_NULL)
    {
        IPCOM_LOG0(ERR, "HTBC init: Failed to create classifier: out of memory");
        return -IP_ERRNO_ENOMEM;
    }

    ipcom_strcpy(fifo.ifq_name, q->hdr.netif->ipcom.name);
    ipcom_strcpy(fifo.ifq_type, "fifo");
    fifo.ifq_id                   = IP_IFQ_ID_NONE;
    fifo.ifq_parent_id            = q->hdr.id;
    fifo.ifq_data.fifo.fifo_limit = IPNET_PKT_QUEUE_DEFAULT_FIFO_SIZE;
    ret = ipnet_pkt_queue_new(&fifo, q->hdr.netif, &q->default_q);
    if (ret < 0)
    {
        IPCOM_LOG1(ERR, "HTBC init: Failed to create default queue: %s", ipcom_strerror(-ret));
        return -IP_ERRNO_ENOMEM;
    }

    q->dequeue_q       = q->default_q;
    q->default_q->next = q->default_q;
    q->default_q->prev = q->default_q;
    return 0;
}


/*
 *===========================================================================
 *                      ipnet_pkt_queue_htbc_destroy
 *===========================================================================
 * Description: Frees all contained queues.
 * Parameters:  htbc - A HTBC based queue.
 * Returns:
 *
 */
IP_STATIC void
ipnet_pkt_queue_htbc_destroy(Ipnet_pkt_queue_htbc *htbc)
{
    Ipnet_pkt_queue *q = htbc->default_q;
    Ipnet_pkt_queue *next_q;

    if (htbc->tmo != IP_NULL)
        ipnet_timeout_cancel(htbc->tmo);

    if (htbc->classifier != IP_NULL)
        ipnet_classifier_delete(htbc->classifier);

    if (htbc->default_q != IP_NULL)
    {
        do
        {
            next_q = q->next;
            ipnet_pkt_queue_delete(q);
            q = next_q;
        } while (q != htbc->default_q);
    }
    htbc->dequeue_q = IP_NULL;
    htbc->default_q = IP_NULL;
}


/*
 *===========================================================================
 *                  ipnet_pkt_queue_htbc_get
 *===========================================================================
 * Description: Returns the queue that matches the specified ID.
 * Parameters:  htbc - A HTBC packet queue.
 *              id - The ID of the contained queue.
 * Returns:     A pointer to the queue or IP_NULL if it was not found.
 *
 */
IP_STATIC struct Ipnet_pkt_queue_struct *
ipnet_pkt_queue_htbc_get(Ipnet_pkt_queue_htbc *htbc, int id)
{
    Ipnet_pkt_queue *q = htbc->default_q;
    Ipnet_pkt_queue *ret_q;

    if (id == htbc->hdr.id)
        return &htbc->hdr;

    if (q)
    {
        do
        {
            if (q->id == id)
                return q;
            if (q->c_ops && IP_NULL != (ret_q = q->c_ops->q_get(q, id)))
                return ret_q;
            q = q->next;
        } while (q != htbc->default_q);
    }
    IPCOM_LOG3(DEBUG, "Did not find %d queue inside %s:%d",
               id, htbc->hdr.type, htbc->hdr.id);
    return IP_NULL;
}


/*
 *===========================================================================
 *                  ipnet_pkt_queue_htbc_remove
 *===========================================================================
 * Description: Removes an queue from a container queue.
 * Parameters:  htbc - The HTBC parent queue for 'q'.
 *              q - The queue to remove.
 * Returns:     0 = success, <0 = error code (-IP_ERRNO_xxx)
 *
 */
IP_STATIC int
ipnet_pkt_queue_htbc_remove(Ipnet_pkt_queue_htbc *htbc, Ipnet_pkt_queue *q)
{
    ip_assert(htbc->hdr.id == q->parent_id);

    if (htbc->dequeue_q == q)
    {
        htbc->dequeue_q = htbc->dequeue_q->next;
        if (htbc->dequeue_q == q)
            /* There is only one contained queue */
            htbc->dequeue_q = IP_NULL;
    }

    if (htbc->default_q == q)
        htbc->default_q = IP_NULL;

    q->next->prev = q->prev;
    q->prev->next = q->next;
    q->prev = IP_NULL;
    q->next = IP_NULL;

    return 0;
}


/*
 *===========================================================================
 *                  ipnet_pkt_queue_htbc_insert
 *===========================================================================
 * Description: Inserts an queue to a container queue.
 * Parameters:  htbc - The parent queue for 'q', must be container capable
 *              q - The queue to insert.
 * Returns:     0 = success, <0 = error code (-IP_ERRNO_xxx)
 *
 */
IP_STATIC int
ipnet_pkt_queue_htbc_insert(Ipnet_pkt_queue_htbc *htbc, Ipnet_pkt_queue *q)
{
    Ipnet_pkt_queue *old_q;

    old_q = ipnet_pkt_queue_htbc_get(htbc, q->id);
    if (old_q)
    {
        IPCOM_LOG6(NOTICE, "Replacing %s:%d queue with %s:%d inside %s:%d",
                   old_q->type, old_q->id, q->type, q->id, htbc->hdr.type, htbc->hdr.id);
        (void)ipnet_pkt_queue_htbc_remove(htbc, old_q);
    }

    if (htbc->dequeue_q)
    {
        /* Insert the new entry after the current dequeue entry */
        q->prev       = htbc->dequeue_q;
        q->next       = htbc->dequeue_q->next;
        htbc->dequeue_q->next->prev = q;
        htbc->dequeue_q->next       = q;
    }
    else
    {
        htbc->dequeue_q = q;
        q->next = q;
        q->prev = q;
    }

    if (htbc->default_q == IP_NULL)
        /* This is the new default queue */
        htbc->default_q = q;

    return 0;
}


/*
 *===========================================================================
 *                  ipnet_pkt_queue_htbc_f_insert
 *===========================================================================
 * Description: Inserts a new filter into a HTBC queue.
 * Parameters:  htbc - A HTBC queue.
 *              filter_id - The ID the filter will have.
 *              rule - The rule to match this filter.
 *              child_queue_id - The queue ID of the child filter that packets
 *              that matches the rule will be queued at.
 * Returns:     0 = success
 *             <0 = error code (-IP_ERRNO_xxx)
 *
 */
IP_STATIC int
ipnet_pkt_queue_htbc_f_insert(Ipnet_pkt_queue_htbc *htbc,
                              int filter_id,
                              struct Ipnet_classifier_rule *rule,
                              int child_queue_id)
{
    int ret;

    if (0 > (ret = ipnet_classifier_add_mapping(htbc->classifier, rule, filter_id)))
        return ret;
    return ipnet_classifier_set_tag(htbc->classifier, filter_id, child_queue_id);
}


/*
 *===========================================================================
 *                  ipnet_pkt_queue_htbc_f_remove
 *===========================================================================
 * Description: Removes a filter from a HTBC queue.
 * Parameters:  htbc - A HTBC queue.
 *              queue_id - The
 * Returns:     0 = success, <0 = error code (-IP_ERRNO_xxx)
 *
 */
IP_STATIC int
ipnet_pkt_queue_htbc_f_remove(Ipnet_pkt_queue_htbc *htbc, int filter_id)
{
    return ipnet_classifier_del_mapping(htbc->classifier, filter_id);
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
ipnet_pkt_queue_htbc_template()
{
    static Ipnet_pkt_queue_container c_ops = {
        (Ipnet_pkt_queue_container_get_queue)    ipnet_pkt_queue_htbc_get,
        (Ipnet_pkt_queue_container_insert_queue) ipnet_pkt_queue_htbc_insert,
        (Ipnet_pkt_queue_container_remove_queue) ipnet_pkt_queue_htbc_remove,
        (Ipnet_pkt_queue_container_insert_filter) ipnet_pkt_queue_htbc_f_insert,
        (Ipnet_pkt_queue_container_remove_filter) ipnet_pkt_queue_htbc_f_remove,
    };

    static Ipnet_pkt_queue htbc_template = {
        "htbc",
        sizeof(Ipnet_pkt_queue_htbc),
        &c_ops,
        (Ipnet_pkt_queue_enqueue)   ipnet_pkt_queue_htbc_enqueue,
        (Ipnet_pkt_queue_dequeue)   ipnet_pkt_queue_htbc_dequeue,
        (Ipnet_pkt_queue_requeue)   ipnet_pkt_queue_htbc_requeue,
        (Ipnet_pkt_queue_count)     ipnet_pkt_queue_htbc_count,
        (Ipnet_pkt_queue_reset)     ipnet_pkt_queue_htbc_reset,
        (Ipnet_pkt_queue_dump)      ipnet_pkt_queue_htbc_dump,
        (Ipnet_pkt_queue_configure) ipnet_pkt_queue_htbc_configure,
        (Ipnet_pkt_queue_init)      ipnet_pkt_queue_htbc_init,
        (Ipnet_pkt_queue_destroy)   ipnet_pkt_queue_htbc_destroy,
    };

#ifdef IPNET_DEBUG
    ipnet_timeout_to_string((Ipnet_timeout_handler) ipnet_pkt_queue_htbc_retry_cb,
                            (Ipnet_timeout_to_string_f) ipnet_pkt_queue_htbc_to_string);
#endif

    return &htbc_template;
}

#else
int ipnet_pkt_q_container_htbc_empty_file;
#endif /* IPNET_USE_IFO_HTBC */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

