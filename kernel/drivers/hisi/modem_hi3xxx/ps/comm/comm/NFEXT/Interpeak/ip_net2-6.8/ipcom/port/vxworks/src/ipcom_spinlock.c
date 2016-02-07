/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_spinlock.c,v $ $Revision: 1.31 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_spinlock.c,v $
 *   $Author: kenneth $
 *   $State: Exp $ $Locker:  $
 *
 * INTERPEAK_COPYRIGHT_STRING
 * Design and implementation by Kenneth Jonsson <kenneth.jonsson@windriver.com>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */
/*
  Implementation of IPCOM 'spinlock' and 'minlock' APIs for VxWorks.

  No code based on IPCOM will run in interrupt context on VxWorks, but
  the ISR spinlocks seems to be faster than the task spinlocks.

  Nondeterministic ISR spinlocks are faster than standard VxWorks ISR
  spinlocks, but do not guarantee fairness or determinism for
  acquisition of such locks under contention. Since we don't
  anticipate very high contention for the spinlocks used, and in any
  case the network stack is nondeterministic for other reasons, in the
  'minlock' APIs we use the nondeterministic locks, which perform well
  in practice.

  For now, for the 'spinlock' APIs we use a local implementation based
  upon atomic variables and (when there is contention) VxWorks binary
  semaphores on SMP builds and taskLock()/taskUnlock() on UP builds,
  since we cannot nest VxWorks spin locks, and cannot call
  non-blocking kernel APIs like semGive() with spinlocks
  held. Benchmarking on various boards suggests that
  taskLock()/taskUnlock() is faster than semTake()/semGive on a mutex
  semaphore. intLock()/intUnlock() is even faster than
  taskLock()/taskUnlock() but cannot be used since there are parts of
  the code that call semGive() with a IPCOM spinlock taken. One could
  also consider minimal semaphores for this purpose.

  Unlike real spinlocks, the current variety is subject to indefinite
  priority inversion.  When a task that has taken the spinlock is
  preempted by an unrelated task of higher priority, a yet higher
  priority task may not acquire the spinlock for an indefinite time as
  a result.
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

#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_os.h>
#include <ipcom_spinlock.h>
#include <ipcom_type.h>

#include <stdlib.h>
#include <vxWorks.h>

#ifdef IPCOM_USE_SMP
#include <semLib.h>
#include <vxAtomicLib.h>
#else
#include <taskLib.h>
#endif

#ifdef IPCOM_USE_SPINLOCK_ISR_ND
#include <spinlockIsrNdLib.h>
#include <private/spinlockIsrNdLibP.h>
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#ifndef IPCOM_USE_SMP
#define IPCOM_SPINLOCK_TAG ((Ipcom_spinlock)0x1973) /* Can be any number */
#endif

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

#ifdef IPCOM_USE_SMP
typedef struct Ipcom_vxworks_spinlock_struct
{
    atomic_t count; /* 1 -> lock not taken,
                       0 -> lock taken, no contention
                      <0 -> lock taken and contention for this lock */
    SEM_ID   sem;
}
Ipcom_vxworks_spinlock_t;
#endif

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
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#ifdef IPCOM_USE_SPINLOCK_ISR_ND
/*
 *===========================================================================
 *                    ipcom_minlock_create
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_minlock_create(Ipcom_spinlock *sl_handle)
{
#ifdef IPCOM_USE_SMP
    spinlockIsrNd_t *sl;

    sl = ipcom_malloc(sizeof(*sl));
    if (sl != IP_NULL)
    {
        *sl_handle = sl;
        spinlockIsrNdInit (sl);
    }

    return sl ? IPCOM_SUCCESS : IPCOM_ERR_FAILED;
#else
    *sl_handle = (Ipcom_spinlock) 0xabadaba2; /* arbitrary */
    return IPCOM_SUCCESS;
#endif
}


/*
 *===========================================================================
 *                    ipcom_minlock_delete
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_minlock_delete(Ipcom_spinlock *sl_handle)
{
#ifdef IPCOM_USE_SMP
    spinlockIsrNd_t *sl = (spinlockIsrNd_t*) *sl_handle;

    ipcom_free(sl);
#endif

    *sl_handle = IPCOM_SPINLOCK_INVALID;

    return IPCOM_SUCCESS;
}
#endif /* IPCOM_USE_SPINLOCK_ISR_ND */


/*
 *===========================================================================
 *                    ipcom_spinlock_create
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_spinlock_create(Ipcom_spinlock *sl_handle)
{
#ifdef IPCOM_USE_SMP
    Ipcom_vxworks_spinlock_t *sl;

    *sl_handle = sl = ipcom_malloc(sizeof(*sl));
    if (sl != IP_NULL)
    {
        vxAtomicSet(&sl->count, 1);
        sl->sem = semBCreate(SEM_Q_PRIORITY | SEM_Q_FIFO, SEM_EMPTY);
    }

    return *sl_handle ? IPCOM_SUCCESS : IPCOM_ERR_FAILED;
#else
    *sl_handle = IPCOM_SPINLOCK_TAG;
    return IPCOM_SUCCESS;
#endif
}


/*
 *===========================================================================
 *                    ipcom_spinlock_delete
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_spinlock_delete(Ipcom_spinlock *sl_handle)
{
#ifdef IPCOM_USE_SMP
    Ipcom_vxworks_spinlock_t *sl = (Ipcom_vxworks_spinlock_t*) *sl_handle;
    Ip_err                    ret;

    ret = semDelete(sl->sem);
    *sl_handle = IPCOM_SPINLOCK_INVALID;
    ipcom_free(sl);

    return (ret == OK) ? IPCOM_SUCCESS : IPCOM_ERR_FAILED;
#else
    *sl_handle = IPCOM_SPINLOCK_INVALID;
    return IPCOM_SUCCESS;
#endif
}


/*
 *===========================================================================
 *                    ipcom_spinlock_lock
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */

IP_FASTTEXT IP_PUBLIC void
ipcom_spinlock_lock(Ipcom_spinlock sl_handle)
{
#ifdef IPCOM_USE_SMP
    Ipcom_vxworks_spinlock_t *sl = (Ipcom_vxworks_spinlock_t*) sl_handle;
#ifdef IPCOM_SPINLOCK_BUSY_PERSIST
    int i;
    atomicVal_t count;

 try_again:

    if (vxCas(&sl->count, 1, 0))
        goto got_it;

    for (i = IPCOM_SPINLOCK_BUSY_PERSIST; i > 0; --i)
    {
        count = vxAtomicGet(&sl->count);
        if (count < 0)
            break;
        if (count == 1)
            goto try_again;
    }
#endif /* IPCOM_SPINLOCK_BUSY_PERSIST */
    if (vxAtomicDec(&sl->count) <= 0)
        /* Contention for this lock, let's sleep until the lock gets
           available */
        semTake(sl->sem, WAIT_FOREVER);
    else
    {
#ifdef IPCOM_SPINLOCK_BUSY_PERSIST
    got_it:
#endif
        /* prevent load/store re-ordering beyond this point */
        VX_MEM_BARRIER_RW();
    }
#else
    IPCOM_UNUSED_ARG(sl_handle);
    ip_assert(sl_handle == IPCOM_SPINLOCK_TAG);
    taskLock();
#endif
}


/*
 *===========================================================================
 *                    ipcom_spinlock_unlock
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_FASTTEXT IP_PUBLIC void
ipcom_spinlock_unlock(Ipcom_spinlock sl_handle)
{
#ifdef IPCOM_USE_SMP
    Ipcom_vxworks_spinlock_t *sl = (Ipcom_vxworks_spinlock_t*) sl_handle;

    VX_MEM_BARRIER_RW();  /* build a memory fence for load and store */

    if (vxAtomicInc(&sl->count) < 0)
        /* Contention for this lock, let's wakeup one of the tasks
           that is blocked on this lock */
        semGive(sl->sem);
#else
    IPCOM_UNUSED_ARG(sl_handle);
    ip_assert(sl_handle == IPCOM_SPINLOCK_TAG);
    taskUnlock();
#endif
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

