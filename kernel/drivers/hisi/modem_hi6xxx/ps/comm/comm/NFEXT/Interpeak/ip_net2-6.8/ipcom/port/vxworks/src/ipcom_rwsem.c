/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_rwsem.c,v $ $Revision: 1.9 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_rwsem.c,v $
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
  Reader/writer semaphores using the VxWorks Reader/Writer mutex semaphore.
  This feature requires VxWorks 6.6 or later.
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

#ifdef IPCOM_SMP_OPTIMIZED

#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_os.h>
#include <ipcom_rwsem.h>
#include <ipcom_spinlock.h>

#include <semLibCommon.h>
#include <vxAtomicLib.h>
#include <vxCpuLib.h>
#include <stdlib.h>

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

typedef struct Ipcom_rwsem_struct
{
    SEM_ID   semaphore;
    atomic_t readers; /* >0 = number of readers holding this lock,
                          0 = unlocked
                         -1 = locked in write mode (by a single writer) */
}
Ipcom_rwsem_t;


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

/*
 *===========================================================================
 *                    ipcom_rwsem_create
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_rwsem_create(Ipcom_rwsem *sem)
{
    Ipcom_rwsem_t *rwsem;

    ip_assert(sem != IP_NULL);

    *sem = IPCOM_RWSEM_INVALID;

    rwsem = ipcom_malloc(sizeof(*rwsem));
    if (rwsem != IP_NULL)
    {
        (void)vxAtomicClear(&rwsem->readers);
        rwsem->semaphore = semRWCreate(SEM_INVERSION_SAFE | SEM_Q_PRIORITY | SEM_DELETE_SAFE,
                                       vxCpuConfiguredGet());
        if (rwsem->semaphore == IP_NULL)
        {
            ipcom_free(rwsem);
            rwsem = IP_NULL;
        }
    }

    *sem = (Ipcom_rwsem) rwsem;
    return rwsem == IP_NULL ? IPCOM_ERR_FAILED : IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_rwsem_delete
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_rwsem_delete(Ipcom_rwsem *sem)
{
    Ip_err err = IPCOM_ERR_FAILED;

    if (sem != IP_NULL && *sem != IPCOM_RWSEM_INVALID)
    {
        Ipcom_rwsem_t *rwsem = (Ipcom_rwsem_t *) *sem;

        if (semDelete(rwsem->semaphore) == OK)
            err = IPCOM_SUCCESS;

        ipcom_free(rwsem);
        *sem = IPCOM_RWSEM_INVALID;
    }

    return err;
}


/*
 *===========================================================================
 *                    ipcom_rwsem_lock_mode
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ipcom_rwsem_lock_mode
ipcom_rwsem_lock_mode(Ipcom_rwsem sem)
{
    Ipcom_rwsem_t *rwsem = (Ipcom_rwsem_t *) sem;
    atomicVal_t    readers = vxAtomicGet(&rwsem->readers);

    ip_assert(readers >= -1);

    if (readers > 0)
        return IPCOM_RWSEM_READ;
    if (readers == -1)
        return IPCOM_RWSEM_WRITE;

    /* It is not safe to call this function
       if the caller does not have this semaphore locked */
    IP_PANIC();
    return IPCOM_RWSEM_UNLOCKED;
}


/*
 *===========================================================================
 *                    ipcom_rwsem_wait
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_FASTTEXT IP_PUBLIC Ip_err
ipcom_rwsem_wait(Ipcom_rwsem sem, Ipcom_rwsem_lock_mode mode)
{
    Ipcom_rwsem_t *rwsem = (Ipcom_rwsem_t *) sem;
    Ip_err         err = IPCOM_ERR_FAILED;

    IPCOM_CAN_SLEEP_ASSERT();

    switch (mode)
    {
    case IPCOM_RWSEM_READ:
        if (IP_LIKELY(semRTake(rwsem->semaphore, WAIT_FOREVER) == OK))
        {
            err = IPCOM_SUCCESS;
            (void) vxAtomicInc(&rwsem->readers);
            ip_assert(vxAtomicGet(&rwsem->readers) > 0);
        }
        break;

    case IPCOM_RWSEM_WRITE:
        if (IP_LIKELY(semWTake(rwsem->semaphore, WAIT_FOREVER) == OK))
        {
            err = IPCOM_SUCCESS;
            ip_assert(vxAtomicGet(&rwsem->readers) == 0);
            (void) vxAtomicDec(&rwsem->readers);
        }
        break;

    default:
        err = IPCOM_ERR_INVALID_ARG;
        IP_PANIC();
        break;
    }

    return err;
}


/*
 *===========================================================================
 *                    ipcom_rwsem_post
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_FASTTEXT IP_PUBLIC Ip_err
ipcom_rwsem_post(Ipcom_rwsem sem)
{
    Ipcom_rwsem_t *rwsem = (Ipcom_rwsem_t *) sem;
    Ip_err         err = IPCOM_SUCCESS;

    switch (ipcom_rwsem_lock_mode(sem))
    {
    case IPCOM_RWSEM_READ:
        ip_assert(vxAtomicGet(&rwsem->readers) > 0);
        (void)vxAtomicDec(&rwsem->readers);
        break;

    case IPCOM_RWSEM_WRITE:
        (void)vxAtomicInc(&rwsem->readers);
        ip_assert(vxAtomicGet(&rwsem->readers) == 0);
        break;

    default:
        err = IPCOM_ERR_INVALID_ARG;
        IP_PANIC();
        break;
    }

    if (IP_UNLIKELY(semGive(rwsem->semaphore) == ERROR))
        IP_PANIC();

    return err;
}

#endif /* IPCOM_SMP_OPTIMIZED */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

