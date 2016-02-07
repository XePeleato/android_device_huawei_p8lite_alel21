/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_vxworks_nehalem_forwarder_data.c,v $ $Revision: 1.2.6.3 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_vxworks_nehalem_forwarder_data.c,v $
 *     $Author: dlkrejsa $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * This is the port specific part of the 'ipcom_forwarder'. It is only valid
 * if IPCOM_USE_NEHALEM_FORWARDER is defined (typically in
 * ipcom/config/ipcom_config.h). When IPCOM_USE_NEHALEM_FORWARDER is not
 * defined, the contents of this file is "ifdefed out".
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"

/* Refer to description above*/
#ifdef IPCOM_USE_NEHALEM_FORWARDER

#define FW_DEBUG

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <vxWorks.h>
#include <string.h>
#include <ctype.h>
#include <vxCpuLib.h>
#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_netif.h>
#include <ipcom_vxworks.h>

#include <ipcom_forwarder.h>
#include <ipcom_vxworks_nehalem_forwarder.h>

#ifdef IPCOM_FORWARDER_AMP
#include <ipcom_shared_mem.h>
#include <wrhv/vbi.h>
#include <multios_ipc/mipc.h>
#endif

#include <stdio.h>
#include <memLib.h>
#include <sysLib.h>

#include <netLib.h>

#include <end.h>
#include <muxLib.h>
#include <net/ethernet.h>
#include <hwif/vxbus/vxBus.h>
#undef BSP_VERSION


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */


#define FW_MAX_LINKHDR  18      /* 14-byte ethernet header + 1 VLAN tag */


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

#ifdef IPCOM_USE_FORWARDER_IPSEC

typedef struct Ipcom_fw_sa_lock_s {
    Ipcom_ipc_spinlock_t lock;
#undef SA_LOCK_SIZE     /* Define as 128 for utmost performance and memory usage */
#ifdef SA_LOCK_SIZE
    Ip_u8 pad [SA_LOCK_SIZE - sizeof(Ipcom_ipc_spinlock_t)];
#endif
} Ipcom_fw_sa_lock_t;

#endif /* IPCOM_USE_FORWARDER_IPSEC */

/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

IP_EXTERN int ipcom_forwarder_config_cache_size(void);

#ifdef IPCOM_USE_FORWARDER_IPSEC
/* Cavium crypto library */
IP_EXTERN Ip_u16 hw_crc16(Ip_u32 *buf, Ip_u32 num);
IP_EXTERN Ip_u32 hw_crc32(Ip_u32 *buf, Ip_u32 num);
#endif

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

/* DATA */
IP_STATIC Ipcom_fw_data_funcs_t * ipcom_fw_data_funcs_get(char *ifname);

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Local data
 *===========================================================================
 */

#ifdef IPCOM_USE_FORWARDER_IPSEC
/* Array of locks, one for each SA */
/* target array needs to be in shared memory, since
 * the locks are accessed by all forwarders.
 */
IP_STATIC Ipcom_fw_sa_lock_t * ipcom_fw_sa_locks;
#endif

/*
 *===========================================================================
 *                         Global data
 *===========================================================================
 */


/* DATA */
Ipcom_fw_data_funcs_t * ipcom_fw_data_funcs [IPCOM_FW_MAX_DRIVERS] = {IP_NULL};

Ipcom_forwarder_info_t *ipcom_fw_info_dbg;

Ip_u32 ipcom_mipc_sendexpress64_failures = 0;

/*
 ****************************************************************************
 * 9                     DEBUG FUNCTIONS
 ****************************************************************************
 */

#ifdef FW_DEBUG

Ip_u32 ipcom_fw_pkt_log = 0;

/*
 *===========================================================================
 *                    hwDumpPkt
 *===========================================================================
 * Context:
 * Description: Initialise forwarding properties of a network interface
 * Parameters:
 * Returns:     True: success, False: failure
 *
 */
IP_STATIC void
hwDumpPkt(void *data, int len)
{
    int i;
    int m = 20;
    unsigned char *c;


    for (i = 0, c = data; i < len; i++)
    {
        if (i != 0 && (i % m) == 0)
        {
            m = 20;
            printf("\n");
        }
        printf("%02x ", *c++);
    }
    printf("\n");
}


/*
 *===========================================================================
 *                    ipcom_print_fw_pkt
 *===========================================================================
 * Context:
 * Description: Print contents of 'hw_msg'
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_print_fw_pkt(Ipcom_fw_pkt_t * pkt, int fw_id)
{
    printf ("fw_id=%d pkt=%p start=%p len=%d\n",
            fw_id, pkt, pkt->start, pkt->len);
    printf (" errsts=0x%x pRxQ=%p pool=%p\n",
            pkt->errsts, pkt->rxu.pRxQ, pkt->pool);
    printf ("fw_msg: tbl0,1=0x%x,0x%x  fw_key=%u  packet=%p ipprt=%d\n",
            pkt->fw_msg.tbl[0], pkt->fw_msg.tbl[1], pkt->fw_msg.fw_key,
            pkt->fw_msg.packet, pkt->fw_msg.ipprt);

    if (ipcom_fw_pkt_log & 0x10000)
        hwDumpPkt((char *)pkt + pkt->start, pkt->len);
}

#endif /* FW_DEBUG */


/*
 ****************************************************************************
 * 10                    LOCAL FUNCTIONS
 ****************************************************************************
 */

#ifdef IPCOM_FW_HIGH_EVENTS
/*
 *===========================================================================
 *                    ipcom_forwarder_specific_event_high
 *===========================================================================
 * Context:       forwarder task
 * Description:   Handle high-priority forwarder-specific events
 * Parameters:    info - forwarder control data structure
 *
 * Returns:       N/A
 */
IP_STATIC __inline__ void
ipcom_forwarder_specific_event_high(Ipcom_forwarder_info_t *info)
{
    atomicVal_t events;
    Ipcom_fw_high_events_t *high = info->high_events;

    /* Atomically obtain the current events and clear them */
    events = vxAtomicSet(&high->events, 0);

#ifdef IPCOM_FW_EVT_JOB
    if (events & IPCOM_FW_EVT_JOB)
    {
        Ipcom_fw_job_t *job;
        Ipcom_int_key_t key;
        Ipcom_fw_offset_t next;
#ifdef IFPROXY_CMD_PACKET_OUT_JOB
        iei_tx_queue_t * pTxQ;
        Ipcom_fw_tx_job_t *txjob;
#endif

        key = ipcom_ipc_spinlock_take(&high->jobq.lock);
        next = high->jobq.head_off;
        high->jobq.head_off = 0;
        high->jobq.ptail_off = (char *)&high->jobq.head_off - (char *)high;
        ipcom_ipc_spinlock_give(&high->jobq.lock, key);

        while (next != 0)
        {
            job = (Ipcom_fw_job_t *)((char *)high + next);
            next = job->next_off;

            switch (job->hdr.Command)
            {
#ifdef IFPROXY_CMD_PACKET_OUT_JOB
            case IFPROXY_CMD_PACKET_OUT:
                pTxQ = info->txQs[job->hdr.PortIndex];
                txjob = (Ipcom_fw_tx_job_t *)job;
                txjob->pkt.len = txjob->job.EndOffset - txjob->job.StartOffset;
                txjob->pkt.fw_msg.packet = (char *)txjob +
                    sizeof (ifproxy_cmd_hdr_t) + txjob->job.StartOffset;
                pkt = (Ipcom_fw_pkt_t *)((char *)&txjob->pkt + 1);
                (void)pTxQ->fw_tx(pTxQ, pkt); /* no TX flow control... */
                /* fw_tx() or fw_tx_clean() takes care of freeing the job */
                break;
#endif
            default:
                /* Don't know how to free this job! */
                vbiKputs ("!! Unknown job in ipcom_forwarder_specicific_event_high()\n");
                break;
            }
        }
    }
#endif /* IPCOM_FW_EVT_JOB */

    /* if (events & IPCOM_FW_HI_EVENT_SOMETHING ELSE) {...} */
}
#endif /* IPCOM_FW_HIGH_EVENTS */

/*
 *===========================================================================
 *                    ipcom_forwarder_specific_event_low
 *===========================================================================
 * Context:       forwarder task
 * Description:   Handle low-priority forwarder-specific events
 * Parameters:    info - forwarder control data structure
 *
 * Returns:       1 if ipcom_forwarder_rx() should return indicating a
 *                  trigger message to its caller.
 *                0 otherwise.
 */
IP_STATIC __inline__ int
ipcom_forwarder_specific_event_low(Ipcom_forwarder_info_t *info)
{
    atomicVal_t events;
    Ipcom_fw_low_events_t *low = info->low_events;

    /* Atomically obtain the current events and clear them */
    events = vxAtomicSet(&low->events, 0);

#ifdef IPCOM_FW_EVT_PKTS_TO_FREE
    if (events & IPCOM_FW_EVT_PKTS_TO_FREE)
    {
        int key;
        Ipcom_fw_pkt_t *pkt;
        Ipcom_fw_pkt_t *next;

        key = ipcom_ipc_spinlock_take (&low->pktStack.lock);
        IPCOM_FW_PKT_STACK_CLEAR(&low->pktStack, pkt);
        ipcom_ipc_spinlock_give (&low->pktStack.lock, key);

        while (pkt != NULL)
        {
            IPCOM_FW_PKT_STACK_NEXT(pkt, next);
            ipcom_fw_pkt_free(pkt, info);
            pkt = next;
        }
    }
#endif

#ifdef IPCOM_FW_EVT_TRIGGER_PIPE_READ
    if (IP_UNLIKELY(events & IPCOM_FW_EVT_TRIGGER_PIPE_READ))
    {
        Ip_u32 qnum;
        fw_tx_queue_t * pTxQ;

        /* Flush all TX queues */
        for (qnum = 0; qnum < info->txQcur; ++qnum)
        {
            pTxQ = info->txQs[qnum];
            pTxQ->fw_tx_clean(pTxQ, info, pTxQ->deferred);
        }
        return 1; /* ipcom_forwarder_rx() should return */
    }
#endif

    return 0;
}

/*
 ****************************************************************************
 * 11                   GLOBAL_FUNCTIONS
 ****************************************************************************
 */


/*
 ****************************************************************************
 * 12                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#if defined (IPCOM_FW_EVT_JOB) && defined (IPCOM_FW_HIGH_EVENTS)
IP_PUBLIC void
ipcom_fw_job_free(Ipcom_fw_job_pool_t *pool, Ipcom_fw_job_t *job)
{
    Ipcom_int_key_t key;
    key = ipcom_ipc_spinlock_take(&pool->lock);
    job->next_off = pool->head_off;
    pool->head_off = (char *)job - (char *)pool;
    ipcom_ipc_spinlock_give(&pool->lock, key);
}
#endif

#ifdef IPCOM_USE_FORWARDER_IPSEC
/*
 *===========================================================================
 *                    ipcom_forwarder_atomic_lock
 *===========================================================================
 * Context:       This routine is called only from a forwarder task
 * Description:   Lock a resource (SA) associated with the specified index
 *                against concurrent access by multiple forwarder tasks.
 *                The lock persists untile ipcom_forwarder_atomic_unlock()
 *                is subsequently called.
 * Parameters:    index - which resource to lock
 *
 * Returns:       A key value to be passed to the subsequent call to
 *                ipcom_forwarder_atomic_unlock()
 *
 */
/* DATA */
IP_PUBLIC fw_lock_key_t
ipcom_forwarder_atomic_lock(int index)
{
    /*
     * Note, the OCTEON code implements this by switching to an ATOMIC
     * tag with tag value offset by index. We must use spinlocks.
     */
    return (fw_lock_key_t)ipcom_ipc_spinlock_take (&ipcom_fw_sa_locks[index].lock)
}


/*
 *===========================================================================
 *                    ipcom_forwarder_atomic_unlock
 *===========================================================================
 * Context:      This routine is called only from a forwarder task
 * Description:  Unlock a resource (SA) associated with the specified index.
 *               This routine is called after a previous call to
 *               ipcom_forwarder_atomic_lock()
 * Parameters:   index - which resource to lock
 *               key - return value from previous call to
 *               ipcom_forwarder_atomic_lock()
 * Returns:      N/A
 *
 */
/* DATA */
IP_PUBLIC void
ipcom_forwarder_atomic_unlock(int index, fw_lock_key_t key)
{
    /*
     * Note, the OCTEON code implements this by switching back to an
     * ORDERED-type tag.
     */
    ipcom_ipc_spinlock_give (&ipcom_fw_sa_locks[index].lock, (int)key);
}


/*
 *===========================================================================
 *                    ipcom_forwarder_hw_crc16
 *===========================================================================
 * Context:
 * Description: Calculate crc16 on a buffer
 * Parameters:  buf : pointer to buffer
 *              num : length of buffer
 * Returns:
 *
 */
/* TODO */
/* DATA */
IP_PUBLIC Ip_u16
ipcom_forwarder_hw_crc16(Ip_u32 *buf, Ip_u32 num)
{
    return hw_crc16(buf, num);
}


/*
 *===========================================================================
 *                    ipcom_forwarder_hw_crc32
 *===========================================================================
 * Context:
 * Description: Calculate crc32 on a buffer
 * Parameters:  buf : pointer to buffer
 *              num : length of buffer
 * Returns:
 *
 */
/* TODO */
/* DATA */
IP_PUBLIC Ip_u32
ipcom_forwarder_hw_crc32(Ip_u32 *buf, Ip_u32 num)
{
#if 0
    return hw_crc32(buf, num);
#else
    return 0;  /* TBD: write an IA-specific CRC routine using the CRC32
                  instruction */
#endif
}
#endif /* IPCOM_USE_FORWARDER_IPSEC */


/*
 *===========================================================================
 *                    ipcom_ipc_msgq_send
 *===========================================================================
 * Context:     forwarder task, typically
 * Description: send a message to an IPC message queue
 * Parameters:  msgq - the IPC message client structure
 *              sd - client-owned MIPC socket descriptor used to send
 *                   express data event to wake up the message queue service
 *              msg - the IPC message to send
 * Returns:     0 if successful; a negative error code on failure
 *
 * This routine sends the message <msg> to the message queue <msgq>,
 * for servicing on the same or a different virtual board.  The message
 * queue msgq must have been initialized earlier using
 * ipcom_ipc_msgq_init() on the virtual board where the server will run.
 * The MIPC socket descriptor sd must be used by the client only and
 * must be bound on the same MIPC bus specified when the message queue
 * was created.
 *
 * Both the queue and the message must be in memory shared by the sender
 * and the receiver. Often <msg> would be a member of a larger data structure.
 *
 * After the message <msg> has been sent, it should not be modified or
 * reused until the message has been serviced by the recipient.  It is
 * the responsibility of the command function indicated by <msg> to
 * make <msg> available for reuse in some application-specific manner,
 * such as returning it to a shared free list, sending it back to the
 * sender on a separate IPC message queue, or marking it somehow.
 */
IP_STATIC void
ipcom_ipc_msgq_send(Ipcom_ipc_msgq_t *msgq, int sd, Ipcom_ipc_msg_t *msg)
{
    Ipcom_fw_offset_t offset;
    Ipcom_fw_offset_t orig_offset_head;
    Ipcom_ipc_msg_t *tail_msg;
    Ipcom_int_key_t key;

    msg->offset_next = 0;
    offset = (char *)msg - (char *)msgq;

    key = ipcom_ipc_spinlock_take (&msgq->u.lock);
    if ((orig_offset_head = msgq->msg_offset_head) == 0)
        msgq->msg_offset_head = offset;
    else
        {
        tail_msg = (Ipcom_ipc_msg_t *)((char *)msgq + msgq->msg_offset_tail);
        tail_msg->offset_next = offset;
        }
    msgq->msg_offset_tail = offset;
    ipcom_ipc_spinlock_give (&msgq->u.lock, key);

    if (orig_offset_head != 0)
        return;

    /*
     * The queue was initially empty, send an express data event to
     * awaken the MIPC event processing task.  The call should never
     * fail, since we never have more than one express event outstanding,
     * and we allocate several in the forwarder image.
     * If it does fail, the queue is left with unhandled packets in it,
     * a complete hang, unless the epxress data callback happens to be still
     * running on the control NAE.
     */
    if (mipc_sendexpress64(sd, &msgq->to, 0ULL) == 0)
        return;

    ipcom_mipc_sendexpress64_failures++; /* debug aid */
    vbiKputs("ipcom_ipc_msgq_send: express send failed!\n");
    return;
}

/*
 *===========================================================================
 *                    ipcom_forwarder_slow_path
 *===========================================================================
 * Context:     called only in a forwarder task
 * Description: deliver the packet to the driver for slow-path receive
 * Parameters:  fw_msg : the packet to be delivered
 * Returns:
 *
 */
/* DATA */
IP_PUBLIC void
ipcom_forwarder_slow_path(fw_msg_t *fw_msg, void *fw_cookie)
{
    Ipcom_fw_pkt_t * pkt;
    fw_rx_queue_t * pRxQ;
    Ipcom_forwarder_info_t *info = fw_cookie;

    pkt = FW_MSG_TO_PKT (fw_msg);

    /*
     * Here pkt->start and pkt->len should still be valid from
     * when the packet was received, unless IPsec has added or
     * removed headers...
     *
     * TODO: Find out if we should be using the original fw_key
     * (before IPSEC modified it), in which case we need to stash
     * the original key somewhere.  At present, it looks like
     * ipcom_forwarder() sometimes restores the original key before
     * calling us, and sometimes doesn't.
     *
     * Markus says we should always restore the original key, and the
     * forwarder should always restore the packet to its original
     * form before calling this routine.
     */

#ifdef IPCOM_USE_FORWARDER_IPSEC
    pkt->start = pkt->fw_msg.packet - (Ip_u8 *)pkt;
    pkt->len = pkt->fw_msg.len;
    pkt->fw_msg.fw_key = pkt->orighash;
#endif

    pRxQ = pkt->rxu.pRxQ;

#ifdef IPCOM_FORWARDER_AMP
    {
        void *msg;

        /*
         * Note, the MAC header starts on a cache-aligned boundary.
         * We omit the two-byte message header; at present, received packets
         * are the only data sent to the ifproxy driver on this connection.
         */
        msg = pkt->fw_msg.packet;
#define SLOW_PKT_HDR_SIZE 0

        /*
         * For now, just use mipc_sendto(). Later we may wish
         * to experiment with avoiding at least one of two copies by
         * putting all our packet buffers inside of MIPC_ZBUFFERs.
         * At present, this is hard to do in a NUMA-aware fashion.
         * Also the ifproxy driver design is currently to access only
         * MIPC shared memory -- relaxing that would also allow zero
         * copy, provided we could map enough of the forwarder shared
         * memory under linux.
         *
         * TODO: Also, we may want to avoid dropping the packet
         * at this point if there are no ZBUFFFERs available;
         * but we may be running inside the MIPC message handling
         * job queue, and shouldn't block that context. To
         * avoid dropping it, we would have to queue the packet
         * somewhere, maintain a TXAVAIL callback, and dequeue
         * when more packets were available.
         */
        if (IP_LIKELY(mipc_sendto(info->sock,
                                  msg, pkt->len + SLOW_PKT_HDR_SIZE,
                                  MIPC_MSG_DONTWAIT,
                                  &pRxQ->ifproxy_addr,
                                  sizeof (&pRxQ->ifproxy_addr)) > 0))
        {
            pRxQ->stats->non_fw_frames++;
            pRxQ->stats->non_fw_bytes += pkt->len;
        }
        else
        {
            /* These are actually the hardware drops statistics; perhaps
             * we should have a separate statistic to indicate software
             * drops? TODO: Better, just don't drop 'em.
             */
            pRxQ->stats->dropped_packets++;
            pRxQ->stats->dropped_octets += pkt->len;
        }

        ipcom_fw_pkt_free (pkt, info);
    }
#else /* ! IPCOM_FORWARDER_AMP */
    /*
     * Hmm, either pkt needs to contain the Ipcom_forwarder_info_t
     * pointer (for freeing it), or else we need to be passed
     * that as an argument also.
     */
    pkt->job.priInfo = NET_TASK_QJOB_PRI;
    pkt->job.func = pRxQ->rx_slow;

    jobQueuePost (netJobQueueId, &pkt->job);
#endif
}

#ifndef IPCOM_FORWARDER_AMP
/*
 *===========================================================================
 *                    ipcom_forwarder_free_trigger_msg
 *===========================================================================
 * Context:
 * Description: Free the "trigger" message posted by
 *              ipcom_forwarder_trigger_pipe_read()
 * Parameters:  msg - the message to be dropped
 *              fw_cookie - the forwarder cookie (unused)
 * Returns:     N/A
 *
 */
IP_PUBLIC void
ipcom_forwarder_free_trigger_msg(fw_msg_t *msg, void *fw_cookie)
{
    (void)msg;
    (void)fw_cookie;
    /* do nothing, there's just one trigger message buffer */
}
#endif /* IPCOM_FORWARDER_AMP */

/*
 *===========================================================================
 *                    ipcom_forwarder_drop_fw_msg
 *===========================================================================
 * Context: Called only in the forwarder task that owns the buffer
 * Description: Free the packet or 'trigger' message.
 * Parameters:
 * Returns:
 *
 */
/* DATA */
IP_PUBLIC void
ipcom_forwarder_drop_fw_msg(fw_msg_t *msg, void * fw)
{
    Ipcom_forwarder_info_t * info = fw;
    Ipcom_fw_pkt_t * pkt;

    if (msg != &info->triggerMsg)
    {
        pkt = FW_MSG_TO_PKT(msg);
        ipcom_fw_pkt_free(pkt, info);
    }
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ipnet_msg_len
 *===========================================================================
 * Context:      called from forwarder task
 * Description:  returns original data length of packet, as returned by
 *               ipcom_forwarder_rx(), before any IPsec modifications
 * Parameters:   msg - the fw_msg_t describing the packet
 * Returns:      original packet length
 *
 */
/* DATA */
IP_PUBLIC int
ipcom_forwarder_ipnet_msg_len(fw_msg_t *msg)
{
    Ipcom_fw_pkt_t * pkt = FW_MSG_TO_PKT(msg);
    return pkt->len;
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ipnet_msg_packet
 *===========================================================================
 * Context:      called from forwarder task
 * Description:  returns address of original start of packet data, as returned
 *               by ipcom_forwarder_rx(), before any IPsec modifications
 * Parameters:   msg -  the fw_msg_t describing the packet
 * Returns:      original packet data starting address
 *
 */
/* DATA */
IP_PUBLIC Ip_u8 *
ipcom_forwarder_ipnet_msg_packet(fw_msg_t *msg)
{
    Ipcom_fw_pkt_t * pkt = FW_MSG_TO_PKT(msg);
    return (Ip_u8 *)pkt + pkt->start;
}


/******************************************************************************
*
* ipcom_forwarder_rx - poll for input packets or control jobs
*
* This function is only called by the "forwarder" core, which has been locked
* to one physical core.  The <fw> argument, which is opaque to the generic
* ipcom_forwarder() code, identifies the forwarder, and provides
* context and state that controls what queues to poll, and so forth.
*
* This function polls for any input packets on any of the RX queues
* that the specified forwarder monitors. The function also (with
* lower frequency) polls for control jobs from ipnet on the forwarder's
* control queue.
*
*   - Control jobs that simply request that the forwarder free packets
*     are handled here invisibly to the caller.
*   - Other control jobs are passed back to the caller as fw_msg_t.
*
* This function may also do occasionaly transmit cleanup polling,
* invisibly to the caller.
*
* Since the upper-level code would do nothing (except continue to poll)
* if this function returned NULL, this function only returns if packets
* or control jobs are available.
*
* RETURNS: A pointer to a fw_msg_t.
*
* ERRNO: N/A
*/
/* DATA */
IP_PUBLIC fw_msg_t *
ipcom_forwarder_rx(void * fw)
{
    Ipcom_forwarder_info_t * info = fw;
    Ipcom_fw_pkt_t * replacement;
    fw_rx_queue_t * pRxQ;
    fw_tx_queue_t * pTxQ;
    Ipcom_fw_pkt_t * pkt;
    fw_msg_t *msg;
    Ip_u32 ctr;
    Ip_u32 qnum;
    Ip_u32 fw_key;

    ctr = info->counter;

    pkt = NULL;
    while (TRUE)
    {
        if (IP_UNLIKELY (ctr == 0))
        {
            /* TX cleanup polling. Check just one queue here for now. */
            qnum = info->txQcur;
            pTxQ = info->txQs[qnum];

            pTxQ->fw_tx_clean(pTxQ, info, pTxQ->deferred);

            if (qnum == 0)
                qnum = info->nTxQs;
            --qnum;
            info->txQcur = qnum;

            if (info->low_events->events &&
                ipcom_forwarder_specific_event_low(info) != 0)
            {
                /* Return to caller indicating a trigger message */
                info->counter = IPCOM_FW_MAX_RX;
                return (&info->triggerMsg);
            }

            ctr = IPCOM_FW_MAX_RX;
        }

#if 0
        if (IP_UNLIKELY(info->nRxQs <= 0))
        {
#ifdef IPCOM_FW_HIGH_EVENTS
            if (info->high_events.events)
                ipcom_forwarder_specific_event_high(info);
#endif
            ctr = 0;
            continue;
        }
#endif

        --ctr;

        qnum = info->rxQcur;

        /*
         * If we've returned the maximum number of consecutive
         * packets for one queue, move on to poll the next.
         */
        if (info->rxQreps >= IPCOM_FW_MAX_1Q)
        {
            info->rxQreps = 0;
            if (qnum == 0)
                qnum = info->nRxQs;
            --qnum;
        }

        while (IP_TRUE)
        {
#ifdef IPCOM_FW_HIGH_EVENTS
            if (info->high_events->events)
                ipcom_forwarder_specific_event_high(info);
#endif
            IPCOM_FWD_WAIT();  /* shared event... Generalize? */

            pRxQ = info->rxQs[qnum];
            pkt = pRxQ->rx_poll(pRxQ);
            if (pkt != IP_NULL)
            {
                ++info->rxQreps;

                if (IP_LIKELY (pkt > IPCOM_FW_RX_POLL_NOBUFS))
                    goto check_pkt; /* got good packet */

                if (IP_UNLIKELY(pkt == IPCOM_FW_RX_POLL_NOBUFS))
                    break;  /* out of replacement buffers */

                /* Must have been an errored packet... */
                if (info->rxQreps < IPCOM_FW_MAX_1Q)
                    continue;

                /* too many errors, move on to next queue */
            }

            /* No good packet found, try next queue */
            info->rxQreps = 0;

            if (qnum == 0)
                qnum = info->nRxQs;
            --qnum;

            /*
             * If we've cycled through all the RX queues without finding
             * any good packet, check for prefetch queue work, then misc.
             * work.
             */
            if (qnum == info->rxQcur)
                break;
        }
        pkt = IP_NULL; /* don't pollute prefetch queue */

        ctr = 0;  /* If we found no RX packets (or ran out of packet buffers),
                     might as well do some TX cleanup soon */

 check_pkt:

        info->rxQcur = qnum;

        /*
         * Note, pRxQ->rx_poll() did a prefetch on the packet data
         * in the current packet,  and the _next_ Ipcom_fw_pkt_t
         * in the RX ring... Let them sit for a while.
         */
        replacement = info->prefetchQ[0];
        info->prefetchQ[0] = pkt;
        pkt = replacement;

        if (pkt != NULL)  /* We have got a packet to return */
        {
            info->counter = ctr;

#ifdef FW_PKT_VAR_FREE
            /*
             * Test only. We plan to get rid of these members
             * and hardcode the free routine, to avoid having
             * to access the packet in order to free it.
             */
            pkt->free = (FW_PKT_FREE_RTN)ipcom_fw_pkt_free;
#endif
            msg = &pkt->fw_msg;

            msg->tbl[1] = 0; /* should be port index (but not used); must be
                                < FW_MSG_CONTROL_BASE */;

            /*
             * If the packet type is not IPv4 or IPv6...
             */
            if (IP_UNLIKELY (msg->tbl[0] == IPCOM_FORWARDER_PACKET_TYPE_NONE))
            {
#ifdef IPCOM_USE_FORWARDER_IPSEC
                msg->len = pkt->len;
                /*
                 * The 'orig_packet' member is not used by the generic
                 * forwarder code, and gei doesn't need it either.
                 */
                /* msg->orig_packet = 0; */
                msg->ipsec_dec = 0;
#endif
            }
            else /* It is IPv4 or IPv6 */
            {

                /* Consider optimizing these stores later */
                if (msg->tbl[0] == IPCOM_FORWARDER_PACKET_TYPE_IPV6)
                {
                    /* IPv6 dst addr offset */
                    uint16_t *pw = (uint16_t *)(msg->packet + 14 + 8 + 16);
                    uint32_t a,b,c;

#ifdef IPCOM_USE_FORWARDER_VLAN
                    if (*(Ip_u16 *)(msg->packet + 12) == ip_htons(IP_VLAN))
                    {
                        msg->tbl[0] = IPCOM_FORWARDER_PACKET_TYPE_IPV6_VLAN;
                        pw += 2;  /* skip vlan tag */
                        /* msg->vlan_id = *(Ip_u16 *)(msg->packet + 14); */
                    }
#endif
                    /* Inline Bob Jenkins' lookup3() hash */
                    /* Set up the internal state */
                    a = b = c = 0xdeadbeef + 16 + *info->fw_secret_seed;
                    a += pw[0] + ((uint32_t)pw[1] << 16); 
                    b += pw[2] + ((uint32_t)pw[3] << 16); 
                    c += pw[4] + ((uint32_t)pw[5] << 16);
                    mix(a,b,c);
                    pw += 6;
                    a += pw[0] + ((uint32_t)pw[1] << 16); 
                    final(a,b,c);
                    fw_key = c;
                
                }
                else /* IPv4 */
                {
                    /* IPv6 dst addr offset */
                    uint16_t *pw = (uint16_t *)(msg->packet + 14 + 16);
                    uint32_t a,b,c;

#ifdef IPCOM_USE_FORWARDER_VLAN
                    if (*(Ip_u16 *)(msg->packet + 12) == ip_htons(IP_VLAN))
                    {
                        msg->tbl[0] = IPCOM_FORWARDER_PACKET_TYPE_IPV4_VLAN;
                        pw += 2;  /* skip vlan tag */
                        /* msg->vlan_id = *(Ip_u16 *)(msg->packet + 14); */
                    }
#endif

                    /* Inline Bob Jenkins' lookup3() hash */
                    /* Set up the internal state */
                    a = b = c = 0xdeadbeef + 4 + *info->fw_secret_seed;
                    a += pw[0] + ((uint32_t)pw[1] << 16); 
                    final(a,b,c);
                    fw_key = c;
                }
                fw_key &= info->fw_key_mask;

#if IPCOM_FORWARDER_TRUE_CACHE_WAYS_SHIFT > 0
                fw_key <<= IPCOM_FORWARDER_TRUE_CACHE_WAYS_SHIFT;
#endif
                msg->fw_key = fw_key;

#ifdef IPCOM_USE_FORWARDER_IPSEC
                pkt->orighash = msg->fw_key;
                msg->len = pkt->len;
                /*
                 * The 'orig_packet' member is not used by the generic
                 * forwarder code, and gei doesn't need it either.
                 */
                /* msg->orig_packet = work->packet_ptr.s.addr; */
#if 0 /* TODO: investigate what to do here instead */
                /* Attempt IPsec decryption if dec_ipsec bit is set
                   and exception bits are cleared */
                w2 = work->word2.u64 & 0x00037f00;
                if (w2 == 0x00020000)
                    msg->ipsec_dec = IP_AF_INET;
                else if (w2 == 0x00030000)
                    msg->ipsec_dec = IP_AF_INET6;
                else
                    msg->ipsec_dec = 0;
#endif
#endif
            }

#ifdef FW_DEBUG
            if ((ipcom_fw_pkt_log & 0xffff) == info->index)
                ipcom_print_fw_pkt(pkt, info->index);
#endif
            return msg;
        }

    }
}


/******************************************************************************
*
* ipcom_forwarder_tx - Transmit a packet to the specified output queue
*
* port_index identifies the output port, fw_msg is the packet
* and fw_cookie identifies the forwarder.
*
* ERRNO: N/A
*/
/* DATA */
IP_PUBLIC int
ipcom_forwarder_tx(int port_index, fw_msg_t *fw_msg, void * fw_cookie)
{
    Ipcom_forwarder_info_t * info = fw_cookie;
    Ipcom_fw_pkt_t * pkt;
    fw_tx_queue_t * pTxQ = info->txQs[port_index];

    /*
     * Do we need to handle a single shared output queue case
     * for IPSEC ?  Would we use spinlock mutual exclusion for that?
     */

    pkt = FW_MSG_TO_PKT (fw_msg);

    /*
     * Here pkt->start and pkt->len should still be valid from
     * when the packet was received, unless IPsec has added or
     * removed headers...
     */

#ifdef IPCOM_USE_FORWARDER_IPSEC
    pkt->start = pkt->fw_msg.packet - (Ip_u8 *)pkt;
    pkt->len = pkt->fw_msg.len;
#endif

    return pTxQ->fw_tx(pTxQ, pkt);
}

/*
 * Functions called by the gei and tei drivers with forwarding support.
 */

/* DATA */
IP_STATIC Ipcom_forwarder_info_t *
ipcom_fw_alloc(unsigned int cpuId, int index)
{
    Ipcom_forwarder_info_t * info;

    info = ipcom_fw_cpu_mem_alloc (cpuId,
                                       sizeof (*info),
                                       _CACHE_ALIGN_SIZE, 0);

    if (info == NULL)
        return NULL;

    bzero ((caddr_t)info, sizeof (*info));

    info->cpuId = cpuId;
    info->index = index; /* must equal fw_id used by generic forwarder code */

    info->triggerMsg.tbl[0] = IPCOM_FORWARDER_PACKET_TYPE_NONE;
    info->triggerMsg.tbl[1] = FW_MSG_TRIGGER_PIPE_READ;

    return info;
}

IP_PUBLIC void
ipcom_fw_heartbeat_rendezvous(Ipcom_fw_anchor_t * anchor,
                              unsigned int my_cpu)
{
    Ip_u32 hb_orig;
    int i;

    /* Wait for 3 beats of management core's 'heart' */
    for (i = 0; i < 3; ++i)
    {
        hb_orig = anchor->master_heartbeat;
        while (anchor->master_heartbeat == hb_orig)
            taskDelay (1);
    }

    /*
     * The manager zeros my heartbeat before starting its
     * own initial heartbeat.  Change my heartbeat so the
     * manager can tell when all expected forwarders have
     * seen its initial heartbeat; then it can stop its
     * initial heartbeat.
     */
    anchor->heartbeat[my_cpu] = 1;

    VX_MEM_BARRIER_RW();
}


IP_STATIC int
ipcom_forwarder_mipc_socket(void)
{
    int sock;
    int tx_bufs;
    int rx_bufs;
    int nbio;
    int busno;
    struct mipc_sockaddr addr;

    sock = mipc_socket (MIPC_AF, MIPC_SOCK_DGRAM, 0);
    if (sock < 0)
        return sock;

    tx_bufs = 256; /* hmm -- how many should we really use? */
    rx_bufs = 1;
    nbio = 1;      /* The socket should be non-blocking */
    if ((mipc_setsockopt(sock, MIPC_SOL, MIPC_SO_TXBUFS,
                         (void *)&tx_bufs, sizeof(tx_bufs)) < 0) ||
        (mipc_setsockopt(sock, MIPC_SOL, MIPC_SO_RXBUFS,
                         (void *)&rx_bufs, sizeof(rx_bufs)) < 0) ||
        (mipc_setsockopt(sock, MIPC_SOL, MIPC_SO_NBIO,
                         (void *)&nbio, sizeof(nbio)) < 0))
        goto err_close;

    if (mipc_getbusbyname(IPCOM_FW_MIPC_BUS_NAME, &busno) < 0)
        goto err_close;

    addr.family = MIPC_AF;
    addr.busNum = busno;
    addr.nodeNum = MIPC_NODE_ANY;
    addr.portNum = MIPC_PORT_ANY;


    if (mipc_bind(sock, &addr, sizeof (addr)) < 0)
        goto err_close;

    return sock;

err_close:
    (void)mipc_close(sock);
    return -1;
}

IP_STATIC Ipcom_forwarder_info_t *
ipcom_fw_setup(Ipcom_fw_anchor_t * anchor, Ipcom_fw_cmd_getinfo_t *msg, int mipc_sock)
{
    Ipcom_forwarder_info_t *info; /* who should really create *info? */
    unsigned int cpu = msg->cpu;
    unsigned int i;
    unsigned int n;
    Ip_u64 regs;
    Ipcom_fw_data_funcs_t *drv_funcs;

    info = ipcom_fw_alloc(cpu, msg->fw_ix);

    if (info == IP_NULL)
        return IP_NULL;

    /* store in global for debugging */
    ipcom_fw_info_dbg = info;

    FW_LOGMSG("Forwarder core %u  info: %p\n", msg->cpu, info);

    info->anchor = anchor;
    info->sock = mipc_sock;

    info->msg_sync = msg; /* save it for possible later reuse */

    /* Initialize our ipcom_fw variable. */
#define FW_OFF2PTR(t, mbr) \
    ipcom_fw.mbr = (t)((char *)msg + msg->mbr##_off)

    FW_OFF2PTR(volatile int *, rt_cache_id);
    FW_OFF2PTR(fw_cache_entry_v4_t *, fw_cache_tbl_v4);
    FW_OFF2PTR(fw_cache_entry_v6_t *, fw_cache_tbl_v6);
    ipcom_fw.fw_v4_entries = msg->fw_v4_entries;
    ipcom_fw.fw_v6_entries = msg->fw_v6_entries;
    FW_OFF2PTR(fw_port_t *, fw_port);

#ifdef IPCOM_FORWARDER_USE_UPPER_TAG_BITS
    ipcom_fw.fw_key_shift = msg->fw_key_shift;
#endif
#ifdef IPCOM_USE_FORWARDER_UPDATE_CACHE_LOCK
    FW_OFF2PTR(fw_cache_lock_t *, fw_lock);
#endif
#ifdef IPCOM_USE_FORWARDER_IPSEC
    FW_OFF2PTR(Ipcom_fw_ipsec_sa_t *, fw_cache_tbl_enc);
    FW_OFF2PTR(Ipcom_fw_ipsec_sa_t *, fw_cache_tbl_dec);
    ipcom_fw.fw_ipsec_entries = msg->fw_ipsec_entries;
    ipcom_fw_sa_locks = (Ipcom_fw_sa_lock_t *)((char *)msg +
                                               msg->fw_sa_locks_off);
#endif

    info->fw_key_mask = msg->fw_key_mask;
    info->fw_secret_seed = (Ip_u32 *)((char *)msg +
                                      msg->fw_secret_seed_off);

    info->low_events =
        (Ipcom_fw_low_events_t *)((char *)msg + msg->low_events_off);
#ifdef IPCOM_FW_HIGH_EVENTS
    info->high_events =
        (Ipcom_fw_high_events_t *)((char *)msg + msg->high_events_off);
#endif

    info->rxq_stats_base =
	(rxq_statistics_t *)((char *)msg + msg->rxq_stats_off);

    info->pkt_buf_size = msg->pkt_buf_size;
    info->nominal_mtu = msg->nominal_mtu;

    n = 0;
    for (i = 0; i < IPCOM_FW_PORTS_MAX; ++i)
    {
	regs = msg->txqc[i].txq_regs;
	if (regs == (Ip_u64)0)
	    continue;
        drv_funcs = ipcom_fw_data_funcs_get(msg->txqc[i].ifname);
        if (drv_funcs == IP_NULL)
	{
	    FW_LOGMSG("TXQC: cannot get drv_funcs for %s\n",
		     msg->txqc[i].ifname);
            goto err_cleanup;
	}

	if (drv_funcs->fw_txq_add(info, (void *)(unsigned long)regs,
				  i, msg->txqc[i].ifname) < 0)
	{
	    FW_LOGMSG("fw_txq_add() failed for port %u\n", i);
	    goto err_cleanup;
	}
        ++n;
    }
    info->nTxQs = n;

    n = 0;
    for (i = 0; i < IPCOM_FW_RX_QUEUES_MAX; ++i)
    {
	regs = msg->rxqc[i].rxq_regs;
	if (regs == (Ip_u64)0)
	    continue;
        drv_funcs = ipcom_fw_data_funcs_get(msg->rxqc[i].ifname);
        if (drv_funcs == IP_NULL)
	{
	    FW_LOGMSG("RXQC: cannot get drv_funcs for %s\n",
		     msg->rxqc[i].ifname);
            goto err_cleanup;
	}

	if (drv_funcs->fw_rxq_add(info, (void *)(unsigned long)regs,
				  msg->rxqc[i].port_index,
				  (info->rxq_stats_base +
				   msg->rxqc[i].rxq_stats_ix),
				  &msg->rxqc[i].ifproxy_addr,
				  msg->rxqc[i].ifname) < 0)
	{
	    FW_LOGMSG("fw_rxq_add() failed for queue %u port %u\n",
		     i, msg->rxqc[i].port_index);
	    goto err_cleanup;
	}
        ++n;
    }

    return info;

err_cleanup:
    /* TODO: cleanup */
    FW_LOGMSG ("ipcom_fw_setup failed for forwarder core %u\n",  cpu);
    return IP_NULL;
}

/*
 * DATA - the function that the forwarder task is spawned to execute
 * for AMP case...
 */
IP_PUBLIC int
ipcom_fw_task(void)
{
    unsigned int cpu = ipcom_forwarder_current_core();
    Ipcom_fw_anchor_t * anchor = ipcom_fw_anchor_get();
    Ipcom_forwarder_info_t * info; /* who should really create *info? */
    Ipcom_fw_cmd_getinfo_t *msg;
    char buf [80];
    int mipc_sock;

    sprintf (buf, "fw core %u\n", cpu);
    vbiKputs (buf);

    /* Wait until we see the manager's initial heartbeat,
       (restart: or other indication that shared memory
       and the manager's IPC message queue are ready?)
       then indicate our presence to the manager. */
    ipcom_fw_heartbeat_rendezvous(anchor, cpu);

    /*
     * We need to get a buffer to use for synchronous
     * requests/commands to the manager.
     * (We could possibly get this as the output of the
     * heartbeat rendezvous...)
     */

    msg = ipcom_fw_cpu_mem_alloc(cpu, IPCOM_FW_SYNC_MSG_SIZE, 0,
                                 IPCOM_FW_CPU_MEM_ALLOC_SHARED);

    if (msg == IP_NULL)
        return -1;

    msg->ipc_msg.cmd = IPCOM_FW_MSGQ_CMD_GETINFO;
    msg->cpu = cpu;
    msg->response_flag = IP_FALSE;

    mipc_sock = ipcom_forwarder_mipc_socket();
    if (mipc_sock < 0)
    {
        vbiKputs("Couldn't create MIPC socket for forwarder.");
        ipcom_fw_cpu_mem_free(msg);
        return -1;
    }

    ipcom_ipc_msgq_send(&anchor->msg_q, mipc_sock, &msg->ipc_msg);

    while (!msg->response_flag)
        taskDelay (1);

    VX_MEM_BARRIER_R();

    /*
     * TODO: consider polling with delay or pending waiting for
     * reconfiguration, if we are not assigned as a forwarder.
     * (If reconfiguration only happens when we reboot, we don't
     * need to...)
     */
    if (msg->fw_ix == 0)
    {
	FW_LOGMSG("CPU %u not assigned as forwarder\n", cpu);
        return -1;  /* we must not be assigned as a forwarder! */
    }

    info = ipcom_fw_setup(anchor, msg, mipc_sock);

    if (info == IP_NULL)
        return -1; /* TODO: send message back? Yes, must, otherwise
                      ipcom_forwarder_update_cache_lock() would block
                      forever waiting for a forwarder that doesn't exist
                      any more. */

    /* Ready to roll, enter main loop */
    ipcom_forwarder_loop (info, NULL /* For now */);

    /* NOT REACHED */
    return 0;
}

/*
 * Note, the driver controlling the port provides the slow-path receive
 * routine that is called for the IPCOM_FW_MSGQ_CMD_SLOWPKT command.
 */


IP_STATIC Ipcom_fw_data_funcs_t *
ipcom_fw_data_funcs_get(char *ifname)
{
    char drvname [IP_IFNAMSIZ];
    char ch;
    Ipcom_fw_data_funcs_t * dfuncs;
    int i;

    if (ifname == IP_NULL)
        return IP_NULL;

    /* Copy driver name from ifname, without unit number. */
    for (i = 0; i < IP_IFNAMSIZ - 1; ++i)
    {
        ch = ifname[i];
        if (ch == '\0' || isdigit(ch))
            break;
        drvname[i] = ch;
    }
    drvname[i] = '\0';

    for (i = 0; i < IPCOM_FW_MAX_DRIVERS; ++i)
    {
        dfuncs = ipcom_fw_data_funcs[i];
        if (dfuncs != NULL && strcmp(drvname, dfuncs->drvname) == 0)
            return dfuncs;
    }
    return IP_NULL;
}


IP_PUBLIC int
ipcom_fw_data_funcs_register(Ipcom_fw_data_funcs_t *dfuncs)
{
    int i;
    for (i = 0; i < IPCOM_FW_MAX_DRIVERS; ++i)
    {
        if (ipcom_fw_data_funcs[i] == NULL)
        {
            ipcom_fw_data_funcs[i] = dfuncs;
            return 0;
        }
    }
    return -1;
}

IP_PUBLIC int
ipcom_fw_pkts_add(Ipcom_forwarder_info_t *info,
		  Ip_u32 num_pkts)
{
    size_t size;
    char * buf;
    Ipcom_fw_pkt_t *pkt;

    /*
     * Assert: info->maxPkts >= info->neededPkts.
     *         info->maxPkts <= IPCOM_FW_MAX_PKTS
     *         info->pkt_pool_index <= info->maxPkts
     */

    if (info->maxPkts - info->neededPkts >= num_pkts &&
	info->pkt_pool_index >= num_pkts)
    {
	info->neededPkts += num_pkts;
	return 0; /* Already have enough */
    }

    if (IPCOM_FW_MAX_PKTS - info->maxPkts < num_pkts)
    {
	/* log message ? */
	return -1;
    }

    size = info->pkt_buf_size;
    buf = ipcom_fw_cpu_mem_alloc(info->cpuId,
				 size * num_pkts, _CACHE_ALIGN_SIZE,
                                 (IPCOM_FW_CPU_MEM_ALLOC_SHARED |
				  IPCOM_FW_CPU_MEM_ALLOC_DMA));

    if (buf == IP_NULL)
	return -1;

    info->neededPkts += num_pkts;
    info->maxPkts += num_pkts;

    while (num_pkts > 0)
    {
	pkt = (Ipcom_fw_pkt_t *)buf;
	ipcom_fw_pkt_free(pkt, info);
	buf += size;
	--num_pkts;
    }

    return 0;
}

IP_PUBLIC void
ipcom_fw_pkts_release(Ipcom_forwarder_info_t *info,
		      Ip_u32 num_pkts)
{
    info->neededPkts -= num_pkts;
}

#ifdef IPCOM_FORWARDER_SHOW

IP_PUBLIC int
ipcom_fw_show(Ipcom_forwarder_info_t * info)
{
    int i;
    fw_rx_queue_t * rQ;
    fw_tx_queue_t * tQ;

    if (info == NULL)
	info = ipcom_fw_info_dbg;

    if (info == NULL)
	return -1;

    printf ("Forwarder %p cpuId=%u index=%u\n", info, info->cpuId, info->index);
    printf ("  %u pkts of %u free; pkts=%p\n",
	    info->pkt_pool_index, info->maxPkts, info->pkts);
    printf ("  counter=%u   rxQcur=%u  nRxQs=%u   txQcur=%u  nTxQs=%u\n",
	    info->counter, info->rxQcur, info->nRxQs,
	    info->txQcur, info->nTxQs);

    printf ("RX queues:\n");
    for (i = 0; i < info->nRxQs; ++i)
    {
	rQ = info->rxQs[i];
	rQ->funcs->fw_rxq_show(info, i);
    }

    printf ("TX queues:\n");
    for (i = 0; i < info->nTxQs; ++i)
	{
	tQ = info->txQs[i];
	tQ->funcs->fw_txq_show(info, i);
	}
    
    return OK;
    }
#endif /* IPCOM_FORWARDER_SHOW */

#else
char ipcom_vxworks_nehalem_forwarder_data_empty_file;
#endif /* IPCOM_USE_NEHALEM_FORWARDER */

/*
 ******************************************************************************
 * End of file
 ******************************************************************************
 */
