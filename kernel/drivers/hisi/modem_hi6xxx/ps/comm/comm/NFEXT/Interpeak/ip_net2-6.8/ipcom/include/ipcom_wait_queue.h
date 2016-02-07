/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_wait_queue.h,v $ $Revision: 1.4 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/include/Attic/ipcom_wait_queue.h,v $
 *   $Author: kenneth $ $Date: 2008-02-19 14:00:24 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth.jonsson@windriver.com>
 ******************************************************************************
 */
#ifndef IPCOM_WAIT_QUEUE_H
#define IPCOM_WAIT_QUEUE_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */
/* Wait queue is a way of letting a process wait for a specific event/signal
   and be put to sleep by the underlying OS while waiting.
   NOTE: The caller must make sure that the proper mutual exclusion protection
         is use for the Ipcom_wait_queue instance when calling wait queue
         related function.
         The pre/post callback function must unlock/lock the mutual exclusion
         object before/after the current process sleeps.
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

#include <ipcom_cstyle.h>
#include <ipcom_list.h>
#include <ipcom_type.h>

#ifdef __cplusplus
extern "C" {
#endif

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


/* Head for a wait queue */
typedef struct Ipcom_wait_queue_struct
{
#ifdef IP_PORT_LKM
    /* TODO: solve this in a more elegant way, it should really
       be space[sizeof(wait_queue_head_t)] */
    char       space[64];
#else
    Ipcom_list proc_fifo; /* FIFO queue of sleeping processes */
#endif
}
Ipcom_wait_queue;


/*
 *===========================================================================
 *                   Ipcom_wait_queue_func
 *===========================================================================
 * Description: Callback hook for the wait queue.
 * Parameters:  userarg - User define argument.
 * Returns:
 *
 */
typedef void (*Ipcom_wait_queue_func)(void *userarg);


/*
 ****************************************************************************
 * 6                    PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*******************************************************************************
*
* ipcom_wait_queue_init - Initializes a wait queue.
*
* Initializes a new wait queue. This function must be called before a wait
* queue is used for the first time.
*
* Parameters:
* \is
* \i <wq>
* A wait queue.
* \ie
*
* RETURNS: N/A
*
* ERRNO: None.
*/
IP_PUBLIC void ipcom_wait_queue_init
(
    Ipcom_wait_queue     *wq
);


/*******************************************************************************
*
* ipcom_wait_queue_is_empty - Checks if the wait queue is empty.
*
* Checks if the wait queue it empty and returns the result.
*
* Parameters:
* \is
* \i <wq>
* Wait queue.
* \ie
*
* RETURNS: 'IP_TRUE' if the queue is empty, 'IP_FALSE' otherwise.
*
* ERRNO: None.
*/
IP_PUBLIC Ip_bool ipcom_wait_queue_is_empty
(
    Ipcom_wait_queue *wq
);


/*******************************************************************************
*
* ipcom_wait_queue_add_current_proc - Add the current process a wait queue.
*
* Add the cirrent process to the specified wait queue. The process will be
* put to sleep until the timer expires or until the process is woken up
* by a call to ipcom_wait_queue_wakeup_one_proc() or
* ipcom_wait_queue_wakeup_all_procs()
*
* Parameters:
* \is
* \i <wq>
* Wait queue.
* \i <msec>
* Timeout, in milliseconds, to wait for a post operation. The macro
* 'IPC_TIMEOUT_INFINITE' makes the function wait forever.
* \i <pre_sleep_func>
* Function called just before the current process is put to sleep.
* Can be used to unlock any mutexes and/or spinlock held by the
* current process.
* \i <post_sleep_func>
* Function called just before ipcom_wait_queue_add_current_proc
* returns to the caller. Can be used to reacquire any mutexes and/or
* spinlock released in pre_sleep_func.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS', or an error value defined in 'include/ipcom_err.h'.
*
* ERRNO: \&'IPCOM_ERR_FAILED'
*/
IP_PUBLIC Ip_err ipcom_wait_queue_add_current_proc
(
    Ipcom_wait_queue *wq,
    Ip_s32 milli_seconds,
    Ipcom_wait_queue_func pre_sleep_func,
    Ipcom_wait_queue_func post_sleep_func,
    void *userarg
);


/*******************************************************************************
*
* ipcom_wait_queue_wakeup_one_proc - Wakeup one process sleeping on the wait queue
*
* Dequeues one process from the wait queue and wakes it up. This function will
* do nothing if no process(es) is sleeping on the wait queue.
*
* Parameters:
* \is
* \i <wq>
* Wait queue.
* \ie
*
* RETURNS: N/A
*
* ERRNO: None.
*/
IP_PUBLIC void ipcom_wait_queue_wakeup_one_proc
(
    Ipcom_wait_queue *wq
);


/*******************************************************************************
*
* ipcom_wait_queue_wakeup_all_procs - Wakeup all processes sleeping on the wait queue
*
* Dequeues all process from the wait queue and wakes them up. This function will
* do nothing if no process(es) is sleeping on the wait queue.
*
* Parameters:
* \is
* \i <wq>
* Wait queue.
* \ie
*
* RETURNS: N/A
*
* ERRNO: None.
*/
IP_PUBLIC void ipcom_wait_queue_wakeup_all_procs
(
    Ipcom_wait_queue *wq
);


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
