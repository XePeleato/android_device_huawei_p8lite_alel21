/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_once.c,v $ $Revision: 1.12 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_once.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * The first process to call ipcom_once() with a given "once", will
 * call the init() routine with argument "param". Subsequent calls to
 * ipcom_once() with the same "once" will not call the init()
 * routine. On return from ipcom_once(), init() routine is guaranteed
 * to have completed.
 * The init() routine must not call ipcom_once(), failure to do so
 * will result in a deadlock.
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
#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_clib.h"
#include "ipcom_err.h"
#include "ipcom_os.h"


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

IP_PUBLIC Ipcom_mutex ipcom_once_mutex;


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_once
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_PUBLIC Ip_err
ipcom_once(Ipcom_once_t *once, Ip_err (*init)(void *param), void *param)
{
    Ip_err           ret = IPCOM_SUCCESS;
    int              old_state;
    Ipcom_wait_queue wq;

    if (once->state == IPCOM_ONCE_STATE_DONE)
        return IPCOM_SUCCESS;

    /* Enter the once monitor */
    ip_assert(ipcom_once_mutex != IPCOM_MUTEX_INVALID);
    ipcom_mutex_lock(ipcom_once_mutex);

    /* Check state */
    switch (old_state = once->state)
    {
    case IPCOM_ONCE_STATE_UNINITIALIZED:
        /* First call, will do the init */
        ipcom_wait_queue_init(&wq);
        once->wq    = &wq;
        once->state = IPCOM_ONCE_STATE_RUNNING;
        break;
    case IPCOM_ONCE_STATE_RUNNING:
        /* Sleep until the init() function has been run */
        ret = ipcom_wait_queue_add_current_proc(once->wq,
                                                IPC_TIMEOUT_INFINITE,
                                                ipcom_mutex_unlock,
                                                ipcom_mutex_lock,
                                                ipcom_once_mutex);
        break;
    case IPCOM_ONCE_STATE_DONE:
        break;
    default:
        IP_PANIC();
        break;
    }

    /* Exit the once monitor */
    ipcom_mutex_unlock(ipcom_once_mutex);

    if (old_state == IPCOM_ONCE_STATE_UNINITIALIZED)
    {
        /* Do init call */
        ret = init(param);

        /* Enter the once monitor again */
        ipcom_mutex_lock(ipcom_once_mutex);

        /* Update the state to done */
        once->state = IPCOM_ONCE_STATE_DONE;

        /* All pending processes are unlinked from the wait-queue by
           this call, so it is safe to set once->wq to IP_NULL */
        ipcom_wait_queue_wakeup_all_procs(once->wq);
        once->wq = IP_NULL;

        /* Exit the once monitor */
        ipcom_mutex_unlock(ipcom_once_mutex);
    }

    return ret;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
