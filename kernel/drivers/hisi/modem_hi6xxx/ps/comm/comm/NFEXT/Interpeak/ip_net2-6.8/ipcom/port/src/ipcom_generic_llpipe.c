/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_generic_llpipe.c,v $ $Revision: 1.7 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_generic_llpipe.c,v $
 *   $Author: dlkrejsa $
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
 * IPCOM generic lockless pipes implementation.
 * Two restrictions apply:
 *
 * 1: Only one sender and one receiver must use the pipe at the same time.
 * 2: Sender and receiver processes must run with the same priority or have
 *    hard affinity to different cores.
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
#include <ipcom_type.h>
#include <ipcom_atomic.h>


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
 *                         Ipcom_pipe_lockless
 *===========================================================================
 * Lockless pipe
 */
typedef struct Ipcom_pipe_lockless_struct
{
    int            capacity;
    Ipcom_atomic_t head;
    Ipcom_atomic_t tail;
    void           **elem;
}
Ipcom_pipe_lockless;


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

IP_GLOBAL Ip_err ipcom_pipe_lockless_create(Ipcom_pipe *pipe, Ipcom_pipe_attr *attr);


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
 *                    ipcom_lockless_pipe_dtor_helper
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_lockless_pipe_dtor_helper(Ipcom_pipe_lockless *p)
{
    ipcom_free(p->elem);
    ipcom_free(p);
}


/*
 *===========================================================================
 *                    ipcom_lockless_pipe_dtor
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_lockless_pipe_dtor(Ipcom_pipe *pipe)
{
    ipcom_lockless_pipe_dtor(pipe->p);
}


/*
 *===========================================================================
 *                    ipcom_lockless_pipe_new_fifo
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ipcom_pipe_lockless *
ipcom_lockless_pipe_new_fifo(int msg_limit)
{
    Ipcom_pipe_lockless *pipe;

    pipe = ipcom_calloc(1, sizeof(Ipcom_pipe_lockless));
    if (pipe == IP_NULL)
        return IPCOM_PIPE_INVALID;

    pipe->elem = ipcom_calloc(msg_limit, sizeof(void*));
    if (pipe->elem == IP_NULL)
        goto errout;

    pipe->capacity = msg_limit;
    ipcom_atomic_set(&pipe->head, 0);
    ipcom_atomic_set(&pipe->tail, 0);

    return pipe;

 errout:
    ipcom_lockless_pipe_dtor_helper(pipe);
    return IPCOM_PIPE_INVALID;
}


/*
 *===========================================================================
 *                    ipcom_lockless_pipe_poll_send
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipcom_lockless_pipe_poll_send(Ipcom_pipe *pipe, void *msg, Ip_bool poll)
{
    Ipcom_pipe_lockless *p = pipe->p;
    int                  tail;
    int                  new_tail;

    /* p->tail can only be modified in ipcom_lockless_pipe_send() */
    tail = ipcom_atomic_get(&p->tail);
    new_tail = tail + 1;
    if (new_tail == p->capacity)
        new_tail = 0;

    for (;;)
    {
        if (new_tail != ipcom_atomic_get(&p->head))
        {
            p->elem[tail] = msg;
            ipcom_wmb();
            ipcom_atomic_set(&p->tail, new_tail);
            break;
        }
        else if (poll == IP_FALSE)
            return IP_FALSE;
        ipcom_proc_yield();   /* Yield for receiver */
    }
    return IP_TRUE;
}


/*
 *===========================================================================
 *                    ipcom_lockless_pipe_send
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_lockless_pipe_send(Ipcom_pipe *pipe, void *msg)
{
    (void)ipcom_lockless_pipe_poll_send(pipe, msg, IP_TRUE);
}


/*
 *===========================================================================
 *                    ipcom_lockless_pipe_try_send
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipcom_lockless_pipe_try_send(Ipcom_pipe *pipe, void *msg)
{
    return ipcom_lockless_pipe_poll_send(pipe, msg, IP_FALSE);
}


/*
 *===========================================================================
 *                    ipcom_lockless_pipe_poll_recv
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipcom_lockless_pipe_poll_recv(Ipcom_pipe *pipe, void **pmsg, Ip_bool poll)
{
    Ipcom_pipe_lockless *p = pipe->p;
    int                  head;

    head = ipcom_atomic_get(&p->head);
    for (;;)
    {
        if (head != ipcom_atomic_get(&p->tail))
        {
            *pmsg = p->elem[head];
            ipcom_rmb();
            if (++head == p->capacity)
                head = 0;
            ipcom_atomic_set(&p->head, head);
            break;
        }
        else if (poll == IP_FALSE)
            return IP_FALSE;
        ipcom_proc_yield();   /* Yield for sender */
    }
    return IP_TRUE;
}


/*
 *===========================================================================
 *                    ipcom_lockless_pipe_recv
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_lockless_pipe_recv(Ipcom_pipe *pipe, void **pmsg)
{
    (void)ipcom_lockless_pipe_poll_recv(pipe, pmsg, IP_TRUE);

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_lockless_pipe_recv_with_tmo
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_lockless_pipe_recv_with_tmo(Ipcom_pipe *pipe, void **pmsg, struct Ip_timeval *tv)
{
    struct Ip_timeval timeout;
    struct Ip_timeval now;

    if (tv->tv_sec == 0 && tv->tv_usec == 0)
    {
        if (ipcom_lockless_pipe_poll_recv(pipe, pmsg, IP_FALSE))
            return IPCOM_SUCCESS;

        goto empty;
    }

    ipcom_microtime(&now);
    ip_timeval_add(&now, tv, &timeout);

    do {
        if (ipcom_lockless_pipe_poll_recv(pipe, pmsg, IP_FALSE))
            return IPCOM_SUCCESS;
        ipcom_microtime(&now);
    } while (ip_timeval_cmp(&now, &timeout, <));

 empty:

    return IPCOM_ERR_TIMEOUT;
}



/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_pipe_lockless_create
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipcom_pipe_lockless_create(Ipcom_pipe *pipe, Ipcom_pipe_attr *attr)
{
    static Ipcom_pipe_ops ipcom_generic_llpipe_ops = {
        ipcom_lockless_pipe_dtor,
        ipcom_lockless_pipe_send,
        ipcom_lockless_pipe_try_send,
        ipcom_lockless_pipe_recv,
        ipcom_lockless_pipe_recv_with_tmo,
    };

    pipe->p = ipcom_lockless_pipe_new_fifo(attr->msg_limit);
    if (pipe->p == IPCOM_PIPE_INVALID)
        return IPCOM_ERR_FAILED;

    pipe->ops = &ipcom_generic_llpipe_ops;
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
