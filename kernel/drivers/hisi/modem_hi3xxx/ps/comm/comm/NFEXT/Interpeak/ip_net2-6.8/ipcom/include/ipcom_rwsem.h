/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_rwsem.h,v $ $Revision: 1.2 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_rwsem.h,v $
 *     $Author: kenneth $ $Date: 2007-04-30 14:29:16 $
 *     $State: Exp $ $Locker:  $
 *
 * INTERPEAK_COPYRIGHT_STRING
 * Design and implementation by Kenneth Jonsson <kenneth.jonsson@windriver.com>
 ******************************************************************************
 */
#ifndef IPCOM_RWSEM_H
#define IPCOM_RWSEM_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
DESCRIPTION
API for IPCOM read/write semaphores
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

#include "ipcom_os.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         RWSEM INVALID
 *===========================================================================
 */
#define IPCOM_RWSEM_INVALID       IP_NULL


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/* Read/write semaphore types */
typedef enum Ipcom_rwsem_lock_mode_enum
{
    IPCOM_RWSEM_UNLOCKED = 0,
    IPCOM_RWSEM_READ,
    IPCOM_RWSEM_WRITE
}
Ipcom_rwsem_lock_mode;

typedef void * Ipcom_rwsem;


/*
 ****************************************************************************
 * 6                 PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*******************************************************************************
 *
 * ipcom_rwsem_create - create a read/write semaphore and initialize its counter
 *
 * This routine creates a read/write semaphore and initializes its counter to 1,
 * i.e the semaphore is initially unlocked.
 * semaphore handle is obtained in the location pointed to by <sem>.
 *
 * Parameters:
 * \is
 * \i <sem>
 * Pointer to a location to store the semaphore handle in.
 * \ie
 *
 * RETURNS: 'IPCOM_SUCCESS', or an error value defined in 'include/ipcom_err.h'.
 *
 * ERRNO: \&'IPCOM_ERR_FAILED'
 */
IP_PUBLIC Ip_err ipcom_rwsem_create
(
    Ipcom_rwsem *sem
);


/*******************************************************************************
 *
 * ipcom_rwsem_delete - delete a read/write semaphore
 *
 * This routine deletes a semaphore.
 *
 * Parameter:
 * \is
 * \i <sem>
 * Pointer to semaphore handle.
 * \ie
 *
 * RETURNS: 'IPCOM_SUCCESS', or an error value defined in 'include/ipcom_err.h'.
 *
 * ERRNO: \&'IPCOM_ERR_FAILED'
 */
IP_PUBLIC Ip_err ipcom_rwsem_delete
(
    Ipcom_rwsem *sem
);


/*******************************************************************************
 *
 * ipcom_sem_lock_mode - get the current lock mode of the read/write semaphore
 *
 * This routine gets the current lock mode which can be one of
 * unlocked, read or write.
 *
 * Parameter:
 * \is
 * \i <sem>
 * Semaphore handle.
 * \ie
 *
 * RETURNS: The lock mode of the read/write semaphore.
 *
 * ERRNO: None.
 */
IP_PUBLIC Ipcom_rwsem_lock_mode ipcom_rwsem_lock_mode
(
    Ipcom_rwsem sem
);


/*******************************************************************************
 *
 * ipcom_rwsem_wait - decrement the read/write semaphore read or write counter
 *
 * This routine decrements the semaphore <sem>. The caller will be blocked if
 * - there is at least one writer already holding or waiting for the lock
 * - there is at least one writer or reader already holding the lock and the
 *   caller tries to take it in write mode
 * Parameter:
 * \is
 * \i <sem>
 * Semaphore handle.
 * \i <mode>
 * Semaphore lock type.
 * \ie
 *
 * RETURNS: \&N/A.
 *
 * ERRNO: None.
 */
IP_FASTTEXT IP_PUBLIC Ip_err ipcom_rwsem_wait
(
     Ipcom_rwsem sem,
     Ipcom_rwsem_lock_mode mode
);


/*******************************************************************************
 *
 * ipcom_rwsem_post - increment the read/write semaphore sem
 *
 * This routine increments the read/write semaphore read counter if it was locked
 * in read mode by the caller or the write counter if it was locked in write by
 * the caller.
 *
 * First waiting writer will be woken.
 * If there is no waiting writer then all waiting readers will be woken if the
 * write counter went above zero.
 *
 * Parameter:
 * \is
 * \i <sem>
 * Semaphore handle.
 * \ie
 *
 * RETURNS: \&N/A.
 *
 * ERRNO: None.
 */
IP_FASTTEXT IP_PUBLIC Ip_err ipcom_rwsem_post
(
    Ipcom_rwsem sem
);

#ifdef __cplusplus
}
#endif

#endif /* IPCOM_RWSEM_H */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

