/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_generic_wait_queue.c,v $ $Revision: 1.5 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/src/Attic/ipcom_generic_wait_queue.c,v $
 *     $Author: kenneth $ $Date: 2008-12-16 10:35:08 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Kenneth Jonsson <kenneth.jonsson@windriver.com>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */
/* Implementation of the IPCOM wait queue API using only other parts of
   the public IPCOM API. This implementation will run unmodified on all
   OS:es supported by IPCOM.
   An OS specific implementation can be created if it is possible to make
   a more efficient implementation using direct OS calls. That implementation
   must be put into <IPCOM>/port/<OS-name>/src/ipcom_wait_queue.c
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
#include "ipcom_clib.h"
#include "ipcom_err.h"
#include "ipcom_os.h"
#include "ipcom_wait_queue.h"


/*
 ****************************************************************************
 * 4                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 5                    DEFINES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 6                    TYPES
 ****************************************************************************
 */

typedef struct Ipcom_wait_queue_entry_struct
{
    Ipcom_list  list;
    Ipcom_proc *proc;
}
Ipcom_wait_queue_entry;

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
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                     ipcom_wait_queue_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_wait_queue_init(Ipcom_wait_queue *wq)
{
    ipcom_list_init(&wq->proc_fifo);
}


/*
 *===========================================================================
 *                   ipcom_wait_queue_is_empty
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_bool
ipcom_wait_queue_is_empty(Ipcom_wait_queue *wq)
{
    ip_assert(wq != IP_NULL);
    return (wq->proc_fifo.size == 0);
}


/*
 *===========================================================================
 *                   ipcom_wait_queue_add_current_proc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_wait_queue_add_current_proc(Ipcom_wait_queue *wq,
                                  Ip_s32 milli_seconds,
                                  Ipcom_wait_queue_func pre_sleep_func,
                                  Ipcom_wait_queue_func post_sleep_func,
                                  void *userarg)
{
    Ipcom_wait_queue_entry wqe;
    Ip_err                 err;

    wqe.proc      = ipcom_proc_self();
#ifdef IP_DEBUG
    wqe.proc->wq  = wq;
#endif

    ip_assert(wqe.proc != IP_NULL);
    ip_assert(wqe.proc->wakeup_signal != IPCOM_BLOCK_INVALID);

    ipcom_list_insert_last(&wq->proc_fifo, &wqe.list);

    pre_sleep_func(userarg);
    err = ipcom_block_wait(wqe.proc->wakeup_signal, milli_seconds);
    post_sleep_func(userarg);

    if (IP_LIKELY(wqe.list.head == IP_NULL))
    {
        if (IP_UNLIKELY(err != IPCOM_SUCCESS))
        {
            /* wakeup_signal got posted after ipcom_block_wait() returned
               must call ipcom_block_wait() again to clear that post */
            pre_sleep_func(userarg);
            if (ipcom_block_wait(wqe.proc->wakeup_signal, IPC_TIMEOUT_POLL) != IPCOM_SUCCESS)
                IP_PANIC();
            post_sleep_func(userarg);
        }
    }
    else
    {
        ip_assert(err != IPCOM_SUCCESS);
        ipcom_list_remove(&wqe.list);
    }

#ifdef IP_DEBUG
    wqe.proc->wq = IP_NULL;
#endif

    return err;
}


/*
 *===========================================================================
 *                   ipcom_wait_queue_wakeup_one_proc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_wait_queue_wakeup_one_proc(Ipcom_wait_queue *wq)
{
    Ipcom_wait_queue_entry *wqe;

    ip_assert(wq != IP_NULL);
    wqe = IPCOM_LIST_FIRST(&wq->proc_fifo);
    if (wqe != IP_NULL)
    {
        ip_assert(wqe->proc != IP_NULL);
        ipcom_list_remove(&wqe->list);

        (void)ipcom_block_post(wqe->proc->wakeup_signal);
    }
}


/*
 *===========================================================================
 *                   ipcom_wait_queue_wakeup_all_procs
 *====================='======================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_wait_queue_wakeup_all_procs(Ipcom_wait_queue *wq)
{
    while (!ipcom_wait_queue_is_empty(wq))
        ipcom_wait_queue_wakeup_one_proc(wq);
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
