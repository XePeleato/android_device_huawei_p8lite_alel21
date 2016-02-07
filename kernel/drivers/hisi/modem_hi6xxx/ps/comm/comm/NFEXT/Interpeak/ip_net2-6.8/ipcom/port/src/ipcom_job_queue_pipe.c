/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_job_queue_pipe.c,v $ $Revision: 1.11 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_job_queue_pipe.c,v $
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
 * IPCOM job queue pipes implementation. Supports one reader and multiple
 * writers. A job queue pipe is an IPCOM pipe implementation where
 * pipe is being read in a job posted to a job queue.
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

#ifdef IPCOM_USE_JOB_QUEUE

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_job_queue.h>
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
 *                         Ipcom_pipe_job_queue
 *===========================================================================
 * Job queue pipe
 */
typedef struct Ipcom_pipe_job_queue_struct
{
    Ipcom_spinlock        lock;           /* protects all members of
                                             this struct */
    void                **msgs;           /* array of where the
                                             messages are stored */
    int                   next_send_slot; /* index of the first free
                                             slot in the pipe */
    int                   next_recv_slot; /* index of next next message
                                             to return from this pipe */
    int                   msg_limit;
    Ipcom_singleton_job_t job;            /* job to read this pipe */
    Ipcom_singleton_job_t resume_job;     /* job to continue reading
                                             this pipe when the "job"
                                             handler has exceeded its
                                             budget */
    Ip_pid_t              creator_pid;    /* PID of the creator */
    Ipcom_wait_queue      waiting_writers;/* queue of processes
                                             waiting to read this
                                             pipe */
}
Ipcom_pipe_job_queue;


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

IP_GLOBAL Ip_err ipcom_pipe_job_queue_create(Ipcom_pipe *pipe, Ipcom_pipe_attr *attr);

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
 *                   ipcom_job_queue_pipe_try_recv_locked
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
ipcom_job_queue_pipe_try_recv_locked(Ipcom_pipe *pipe, void **pmsg)
{
    Ipcom_pipe_job_queue *p = pipe->p;
    int                   next_recv_slot = p->next_recv_slot;

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
 *                   ipcom_job_queue_pipe_try_send_locked
 *===========================================================================
 * Description: Tries to send one message to the specified pipe. This
 *                   function must only be called with pipe lock the be held.
 * Parameters:  pipe - a pipe
 *              msg - the message to send
 * Returns:     IP_TRUE - message was successfully sent
 *              IP_FALSE - pipe was full, message not sent
 *
 */
IP_STATIC Ip_bool
ipcom_job_queue_pipe_try_send_locked(Ipcom_pipe *pipe, void *msg)
{
    Ipcom_pipe_job_queue *p = pipe->p;
    int                   next_send_slot;
    int                   this_send_slot;

    this_send_slot = p->next_send_slot;
    next_send_slot = this_send_slot + 1;
    if (next_send_slot == p->msg_limit)
        next_send_slot = 0;

    if (IP_UNLIKELY(next_send_slot == p->next_recv_slot))
        /* Pipe is full */
        return IP_FALSE;

    p->msgs[this_send_slot] = msg;
    p->next_send_slot = next_send_slot;

    if (this_send_slot == p->next_recv_slot)
        /* First message queued to this pipe, let's schedule a net
           job to read this pipe */
        ipcom_job_queue_schedule_singleton(p->job);

    return IP_TRUE;
}



/*
 *===========================================================================
 *                    ipcom_job_queue_pipe_dtor_helper
 *===========================================================================
 * Description: Destroys a job queue pipe.
 * Parameters:  pipe - the pipe do destroy
 * Returns:
 *
 */
IP_STATIC void
ipcom_job_queue_pipe_dtor_helper(Ipcom_pipe_job_queue *p)
{
    if (p->job)
        ipcom_job_queue_singleton_delete(p->job);
    if (p->resume_job)
        ipcom_job_queue_singleton_delete(p->resume_job);
    if (p->lock != IPCOM_SPINLOCK_INVALID)
        ipcom_spinlock_delete(&p->lock);
    ipcom_free(p->msgs);
    ipcom_free(p);
}


/*
 *===========================================================================
 *                    ipcom_job_queue_pipe_dtor
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_job_queue_pipe_dtor(Ipcom_pipe *pipe)
{
    ipcom_job_queue_pipe_dtor_helper(pipe->p);
}


/*
 *===========================================================================
 *                    ipcom_job_queue_pipe_new
 *===========================================================================
 * Description: Creates a new job queue pipe.
 * Parameters:  msg_limit - maximum number of messages this pipe can hold
 *              job_queue_func - handler that knows how to process the
 *              jobs queued on this pipe.
 * Returns:     Pointer the the newly created pipe or
 *              IPCOM_PIPE_INVALID if a new pipe could not be created.
 *
 */
IP_STATIC Ipcom_pipe_job_queue *
ipcom_job_queue_pipe_new(int msg_limit, Ipcom_job_action job_queue_func)
{
    Ipcom_pipe_job_queue *pipe;

    pipe = ipcom_calloc(1, sizeof(Ipcom_pipe_job_queue));
    if (pipe == IP_NULL)
        return IPCOM_PIPE_INVALID;

    pipe->msgs = ipcom_calloc(msg_limit, sizeof(void*));
    if (pipe->msgs == IP_NULL)
        goto errout;

    if (ipcom_spinlock_create(&pipe->lock) != IPCOM_SUCCESS)
        goto errout;

    pipe->job
        = ipcom_job_queue_singleton_new(job_queue_func,
                                        IPCOM_JOB_PRIO_NORMAL);
    pipe->resume_job
        = ipcom_job_queue_singleton_new(job_queue_func,
                                        IPCOM_JOB_PRIO_LOW);
    if (pipe->job == IP_NULL || pipe->resume_job == IP_NULL)
        goto errout;

    pipe->msg_limit   = msg_limit;
    pipe->creator_pid = ipcom_getpid();
    ipcom_wait_queue_init(&pipe->waiting_writers);

    return pipe;

 errout:
    ipcom_job_queue_pipe_dtor_helper(pipe);
    return IPCOM_PIPE_INVALID;
}


/*
 *===========================================================================
 *                    ipcom_job_queue_pipe_send
 *===========================================================================
 * Description: Sends one message to the specified pipe.
 * Parameters:  pipe - a pipe
 *              msg - the message to send
 * Returns:
 *
 */
IP_STATIC void
ipcom_job_queue_pipe_send(Ipcom_pipe *pipe, void *msg)
{
    Ipcom_pipe_job_queue *p = pipe->p;

    ipcom_spinlock_lock(p->lock);

    for (;;)
    {
        if (ipcom_job_queue_pipe_try_send_locked(pipe, msg))
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
 *                    ipcom_job_queue_pipe_try_send
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
ipcom_job_queue_pipe_try_send(Ipcom_pipe *pipe, void *msg)
{
    Ipcom_pipe_job_queue *p = pipe->p;
    Ip_bool               success;

    ipcom_spinlock_lock(p->lock);

    success = ipcom_job_queue_pipe_try_send_locked(pipe, msg);

    ipcom_spinlock_unlock(p->lock);

    return success;
}


/*
 *===========================================================================
 *                    ipcom_job_queue_pipe_try_recv
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
 *
 */
IP_STATIC int
ipcom_job_queue_pipe_try_recv(Ipcom_pipe *pipe, void **pmsg, struct Ip_timeval *tv)
{
    Ipcom_pipe_job_queue *p = pipe->p;
    Ip_bool               success;

    IPCOM_UNUSED_ARG(tv);

    ipcom_spinlock_lock(p->lock);

    success = ipcom_job_queue_pipe_try_recv_locked(pipe, pmsg);

    ipcom_spinlock_unlock(p->lock);

    return success ? IPCOM_SUCCESS : IPCOM_ERR_WOULDBLOCK;
}


/*
 *===========================================================================
 *                    ipcom_job_queue_pipe_recv
 *===========================================================================
 * Description: The blocking version of pipe receive cannot be used
 *              toghether with a job queue since the job queue daemon
 *              is not allowed to block.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_job_queue_pipe_recv(Ipcom_pipe *pipe, void **pmsg)
{
    IPCOM_UNUSED_ARG(pipe);
    IPCOM_UNUSED_ARG(pmsg);

    IP_PANIC();
    return IPCOM_ERR_NOT_SUPPORTED;
}



/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_pipe_job_queue_create
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipcom_pipe_job_queue_create(Ipcom_pipe *pipe, Ipcom_pipe_attr *attr)
{
    static Ipcom_pipe_ops ipcom_job_queue_pipe_ops = {
        ipcom_job_queue_pipe_dtor,
        ipcom_job_queue_pipe_send,
        ipcom_job_queue_pipe_try_send,
        ipcom_job_queue_pipe_recv,
        ipcom_job_queue_pipe_try_recv,
    };

    pipe->p = ipcom_job_queue_pipe_new(attr->msg_limit, attr->handler);
    if (pipe->p == IPCOM_PIPE_INVALID)
        return IPCOM_ERR_FAILED;

    pipe->ops = &ipcom_job_queue_pipe_ops;
    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                     ipcom_job_queue_pipe_schedule
 *===========================================================================
 * Description: Explicitly schedule a job that will poll the pipe.
 * Parameters:  pipe - pipe to poll in the near future.
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_job_queue_pipe_schedule(Ipcom_pipe *pipe)
{
    Ipcom_pipe_job_queue *p = pipe->p;

    ipcom_job_queue_schedule_singleton(p->resume_job);
}

#else
int ipcom_job_queue_pipe_empty_file;
#endif /* IPCOM_USE_JOB_QUEUE */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

