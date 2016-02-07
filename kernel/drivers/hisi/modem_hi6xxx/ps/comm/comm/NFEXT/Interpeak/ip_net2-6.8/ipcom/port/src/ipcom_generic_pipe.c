/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_generic_pipe.c,v $ $Revision: 1.12 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_generic_pipe.c,v $
 *   $Author: kenneth $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth.jonsson@windriver.com>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * IPCOM generic pipes implementation. Supports one reader and multiple
 * writers.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_os.h>
#include <ipcom_pipe.h>
#include <ipcom_spinlock.h>
#include <ipcom_type.h>
#include <ipcom_wait_queue.h>


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
 *===========================================================================
 *                         Ipcom_pipe_generic
 *===========================================================================
 * Generic pipe
 */
typedef struct Ipcom_pipe_generic_struct
{
    Ipcom_spinlock   lock;
    void           **msgs;
    int              next_send_slot;
    int              next_recv_slot;
    int              msg_limit;
    Ip_pid_t         creator_pid;
    Ipcom_wait_queue waiting_readers;
    Ipcom_wait_queue waiting_writers;
}
Ipcom_pipe_generic;


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

IP_GLOBAL Ip_err ipcom_pipe_generic_create(Ipcom_pipe *pipe, Ipcom_pipe_attr *attr);


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
 *                   ipcom_generic_pipe_try_recv_locked
 *===========================================================================
 * Description: Tries to receive a message from the pipe.
 * Parameters:  pipe - the pipe the message should be received from
 *              pmsg - pointer to where the message pointer should be
 *                   stored if the call is successful.
 * Returns:     IP_TRUE - one message has been queued from the pipe
 *              IP_FALSE - no message was available
 *
 */
IP_STATIC Ip_bool
ipcom_generic_pipe_try_recv_locked(Ipcom_pipe *pipe, void **pmsg)
{
    Ipcom_pipe_generic *p = pipe->p;
    int                 next_recv_slot = p->next_recv_slot;

    if (next_recv_slot == p->next_send_slot)
        /* Pipe is empty */
        return IP_FALSE;

    *pmsg = p->msgs[next_recv_slot];
    p->next_recv_slot = next_recv_slot + 1;
    if (p->next_recv_slot == p->msg_limit)
        p->next_recv_slot = 0;

    if (p->next_recv_slot == p->next_send_slot
        && IP_UNLIKELY(IP_FALSE == ipcom_wait_queue_is_empty(&p->waiting_writers)))
        ipcom_wait_queue_wakeup_all_procs(&p->waiting_writers);

    return IP_TRUE;
}


/*
 *===========================================================================
 *                   ipcom_generic_pipe_try_send_locked
 *===========================================================================
 * Description: Tries to send one message to the specified pipe. This
 *                   function must only be called with pipe lock the be held.
 * Parameters:  pipe - a pipe
 *              msg - the message to send
 * Returns:     IP_TRUE - message was successfully sent
 *              IP_FALSE - operation was aborted before the message
 *                      got sent
 *
 */
IP_STATIC Ip_bool
ipcom_generic_pipe_try_send_locked(Ipcom_pipe *pipe, void *msg)
{
    Ipcom_pipe_generic *p = pipe->p;
    int                 next_send_slot;
    int                 this_send_slot;

    this_send_slot = p->next_send_slot;
    next_send_slot = this_send_slot + 1;
    if (next_send_slot == p->msg_limit)
        next_send_slot = 0;

    if (IP_UNLIKELY(next_send_slot == p->next_recv_slot))
        /* Pipe is full */
        return IP_FALSE;

    p->msgs[this_send_slot] = msg;
    p->next_send_slot = next_send_slot;

    if (IP_UNLIKELY(IP_FALSE == ipcom_wait_queue_is_empty(&p->waiting_readers)))
        ipcom_wait_queue_wakeup_one_proc(&p->waiting_readers);

    return IP_TRUE;
}



/*
 *===========================================================================
 *                    ipcom_generic_pipe_dtor_helper
 *===========================================================================
 * Description: Destroys a generic pipe.
 * Parameters:  p - the Ipcom_generic_pipe to destroy
 * Returns:
 *
 */
IP_STATIC void
ipcom_generic_pipe_dtor_helper(Ipcom_pipe_generic *p)
{
    if (p->lock != IPCOM_SPINLOCK_INVALID)
        ipcom_spinlock_delete(&p->lock);
    ipcom_free(p->msgs);
    ipcom_free(p);
}


/*
 *===========================================================================
 *                    ipcom_generic_pipe_dtor
 *===========================================================================
 * Description: Destroys a pipe.
 * Parameters:  pipe - the pipe do destroy
 * Returns:
 *
 */
IP_STATIC void
ipcom_generic_pipe_dtor(Ipcom_pipe *pipe)
{
    ipcom_generic_pipe_dtor_helper(pipe->p);
}


/*
 *===========================================================================
 *                      ipcom_generic_pipe_new_fifo
 *===========================================================================
 * Description: Creates a new pipe.
 * Parameters:  msg_limit - maximum number of messages this pipe can hold
 * Returns:     Pointer the the newly created pipe or
 *              IPCOM_PIPE_INVALID if a new pipe could not be created.
 *
 */
IP_STATIC Ipcom_pipe_generic *
ipcom_generic_pipe_new_fifo(int msg_limit)
{
    Ipcom_pipe_generic *pipe;

    pipe = ipcom_calloc(1, sizeof(Ipcom_pipe_generic));
    if (pipe == IP_NULL)
        return IPCOM_PIPE_INVALID;

    pipe->msgs = ipcom_calloc(msg_limit, sizeof(void*));
    if (pipe->msgs == IP_NULL)
        goto errout;

    if (ipcom_spinlock_create(&pipe->lock) != IPCOM_SUCCESS)
        goto errout;

    pipe->msg_limit   = msg_limit;
    pipe->creator_pid = ipcom_getpid();
    ipcom_wait_queue_init(&pipe->waiting_readers);
    ipcom_wait_queue_init(&pipe->waiting_writers);

    return pipe;

 errout:
    ipcom_generic_pipe_dtor_helper(pipe);
    return IPCOM_PIPE_INVALID;
}


/*
 *===========================================================================
 *                        ipcom_generic_pipe_send
 *===========================================================================
 * Description: Sends one message to the specified pipe.
 * Parameters:  pipe - a pipe
 *              msg - the message to send
 * Returns:
 *
 */
IP_STATIC void
ipcom_generic_pipe_send(Ipcom_pipe *pipe, void *msg)
{
    Ipcom_pipe_generic *p = pipe->p;

    ipcom_spinlock_lock(p->lock);

    for (;;)
    {
        if (ipcom_generic_pipe_try_send_locked(pipe, msg))
            break;

        ipcom_wait_queue_add_current_proc(&p->waiting_writers,
                                          IPC_TIMEOUT_INFINITE,
                                          (Ipcom_wait_queue_func) ipcom_spinlock_unlock,
                                          (Ipcom_wait_queue_func) ipcom_spinlock_lock,
                                          p->lock);
    }

    ipcom_spinlock_unlock(p->lock);
}


/*
 *===========================================================================
 *                      ipcom_generic_pipe_try_send
 *===========================================================================
 * Description: Tries to send one message to the specified pipe.
 * Parameters:  pipe - a pipe
 *              msg - the message to send
 * Returns:     IP_TRUE - message was successfully sent
 *              IP_FALSE - operation was aborted before the message
 *                      got sent
 *
 */
IP_STATIC Ip_bool
ipcom_generic_pipe_try_send(Ipcom_pipe *pipe, void *msg)
{
    Ipcom_pipe_generic *p = pipe->p;
    Ip_bool             success;

    ipcom_spinlock_lock(p->lock);

    success = ipcom_generic_pipe_try_send_locked(pipe, msg);

    ipcom_spinlock_unlock(p->lock);

    return success;
}


/*
 *===========================================================================
 *                    ipcom_generic_pipe_recv_with_tmo
 *===========================================================================
 * Description: Receives one message from a pipe, the operation will
 *                    be aborted if nothing is received within a
 *                    specific duration.
 * Parameters:  pipe - the pipe to receive from
 *              psmg - pointer to where a pointer to the message
 *                    should be stored
 *              tv - maximum duration to wait for a message, IP_NULL
 *                    means wait forever.
 * Returns:     IPCOM_SUCCESS - message was received
 *              IPCOM_ERR_TIMEOUT - the specified duration elapsed
 *                    without any message being received.
 *              IPCOM_ERR_INTERRUPT - operation was aborted before any message
 *                    was received.
 *
 */
IP_STATIC int
ipcom_generic_pipe_recv_with_tmo(Ipcom_pipe *pipe, void **pmsg, struct Ip_timeval *tv)
{
    Ipcom_pipe_generic *p = pipe->p;
    Ip_u32              msec = (tv
                                ? tv->tv_sec * 1000 + tv->tv_usec / 1000
                                : IPC_TIMEOUT_INFINITE);
    int                 ret = IPCOM_SUCCESS;

    ipcom_spinlock_lock(p->lock);

    if (!ipcom_generic_pipe_try_recv_locked(pipe, pmsg))
    {
        ret = ipcom_wait_queue_add_current_proc(&p->waiting_readers,
                                                msec,
                                                (Ipcom_wait_queue_func) ipcom_spinlock_unlock,
                                                (Ipcom_wait_queue_func) ipcom_spinlock_lock,
                                                p->lock);

        if (ret == IPCOM_SUCCESS)
            ipcom_generic_pipe_try_recv_locked(pipe, pmsg);
    }

    ipcom_spinlock_unlock(p->lock);

    return ret;
}


/*
 *===========================================================================
 *                    ipcom_generic_pipe_recv
 *===========================================================================
 * Description: Receives one message from the specified pipe.
 * Parameters:  pipe - a pipe
 *              pmsg - pointer to where a pointer to the received
 *                    message should be stored
 *
 */
IP_STATIC int
ipcom_generic_pipe_recv(Ipcom_pipe *pipe, void **pmsg)
{
    int ret;

    do
    {
        ret = ipcom_generic_pipe_recv_with_tmo(pipe, pmsg, IP_NULL);
    } while (ret == IPCOM_ERR_TIMEOUT);

    return ret;
}



/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_pipe_generic_create
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipcom_pipe_generic_create(Ipcom_pipe *pipe, Ipcom_pipe_attr *attr)
{
    static Ipcom_pipe_ops ipcom_generic_pipe_ops = {
        ipcom_generic_pipe_dtor,
        ipcom_generic_pipe_send,
        ipcom_generic_pipe_try_send,
        ipcom_generic_pipe_recv,
        ipcom_generic_pipe_recv_with_tmo,
    };

    pipe->p = ipcom_generic_pipe_new_fifo(attr->msg_limit);
    if (pipe->p == IPCOM_PIPE_INVALID)
        return IPCOM_ERR_FAILED;

    pipe->ops = &ipcom_generic_pipe_ops;
    return IPCOM_SUCCESS;
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

