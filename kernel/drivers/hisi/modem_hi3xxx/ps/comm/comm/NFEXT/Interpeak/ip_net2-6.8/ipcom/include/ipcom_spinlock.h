/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_spinlock.h,v $ $Revision: 1.6 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_spinlock.h,v $
 *     $Author: kenneth $ $Date: 2009-06-12 14:15:34 $
 *     $State: Exp $ $Locker:  $
 *
 * INTERPEAK_COPYRIGHT_STRING
 * Design and implementation by Kenneth Jonsson <kenneth.jonsson@windriver.com>
 ******************************************************************************
 */
#ifndef IPCOM_SPINLOCK_H
#define IPCOM_SPINLOCK_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
DESCRIPTION
API for IPCOM spinlocks
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
 *                    SPINLOCK INVALID
 *===========================================================================
 */
#define IPCOM_SPINLOCK_INVALID       IP_NULL


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/* Spinlock types */
typedef void * Ipcom_spinlock;

/*
 ****************************************************************************
 * 6                 PUBLIC FUNCTIONS
 ****************************************************************************
 */

#ifdef IP_PORT_VXWORKS
#ifdef IPCOM_USE_SPINLOCK_ISR_ND

struct spinlockIsrNd_s;

extern int spinlockIsrNdTake(struct spinlockIsrNd_s * sl);
extern void spinlockIsrNdGive(struct spinlockIsrNd_s * sl, int key);

/******************************************************************************
 *
 * ipcom_minlock_create - create a minimal spinlock
 *
 * This routine creates a new unlocked non-deterministic ISR-callable spinlock.
 * Such 'minimal' spinlocks may not be held across calls to most VxWorks kernel
 * functions.  These spinlocks should also not be taken in a nested fashion
 * with any other spinlock.
 *
 * Parameters:
 * \is
 * \i <sl>
 * Pointer to a location to store the spinlock handle in.
 * \ie
 *
 * RETURNS: 'IPCOM_SUCCESS', or an error value defined in 'include/ipcom_err.h'.
 *
 * ERRNO: \&'IPCOM_ERR_FAILED'
 */
IP_PUBLIC Ip_err ipcom_minlock_create
(
    Ipcom_spinlock *sl
);

/******************************************************************************
 *
 * ipcom_minlock_delete - delete a minimal spinlock
 *
 * This routine deletes a minimal spinlock.
 *
 * Parameter:
 * \is
 * \i <sl>
 * Pointer to spinlock handle.
 * \ie
 *
 * RETURNS: 'IPCOM_SUCCESS', or an error value defined in 'include/ipcom_err.h'.
 *
 * ERRNO: \&'IPCOM_ERR_FAILED'
 */
IP_PUBLIC Ip_err ipcom_minlock_delete
(
    Ipcom_spinlock *sl
);

/******************************************************************************
 *
 * ipcom_minlock_lock - acquire the minimal spinlock.
 *
 * This routine will lock the non-deterministic ISR-callable spinlock <sl>.
 * Note, such spinlocks may not be held locked across calls to most VxWorks
 * kernel functions.
 *
 * The return value of this function is passed as the second argument to
 * ipcom_minlock_unlock() when unlocking the spinlock.
 *
 * Parameter:
 * \is
 * \i <sl>
 * Spinlock handle.
 * \ie
 *
 * RETURNS: An interrupt locking key to be passed to ipcom_spinlock_unlock().
 *
 * ERRNO: None.
 */
IP_INLINE int ipcom_minlock_lock
(
     Ipcom_spinlock sl
)
{
    return spinlockIsrNdTake (sl);
}


/******************************************************************************
 *
 * ipcom_minlock_unlock - release the minimal spinlock
 *
 * This routine releases a minimal spinlock acquired with
 * ipcom_minlock_lock().
 *
 * Parameter:
 * \is
 * \i <sl>
 * Spinlock handle.
 * \i <key>
 * Interrupt locking key returned from ipcom_minlock_lock().
 * \ie
 *
 * RETURNS: \&N/A.
 *
 * ERRNO: None.
 */
IP_INLINE void ipcom_minlock_unlock
(
    Ipcom_spinlock sl,
    int key
)
{
    spinlockIsrNdGive (sl, key);
}

#endif /* IPCOM_USE_SPINLOCK_ISR_ND */
#endif /* IP_PORT_VXWORKS */

/*******************************************************************************
 *
 * ipcom_spinlock_create - create a spinlock
 *
 * This routine creates a new unlocked spinlock.
 *
 * Parameters:
 * \is
 * \i <sl>
 * Pointer to a location to store the spinlock handle in.
 * \ie
 *
 * RETURNS: 'IPCOM_SUCCESS', or an error value defined in 'include/ipcom_err.h'.
 *
 * ERRNO: \&'IPCOM_ERR_FAILED'
 */
IP_PUBLIC Ip_err ipcom_spinlock_create
(
    Ipcom_spinlock *sl
);
#if defined(IPCOM_USE_DMALLOC) && defined(IP_PORT_UNIX)
IP_PUBLIC Ip_err ipcom_dspinlock_create(Ipcom_spinlock *sl,
                                        const char *file,
                                        Ip_u32 line);
#define ipcom_spinlock_create(sl) \
    ipcom_dspinlock_create(sl, __FILE__, __LINE__)
#endif



/*******************************************************************************
 *
 * ipcom_spinlock_delete - delete a spinlock
 *
 * This routine deletes a spinlock.
 *
 * Parameter:
 * \is
 * \i <sl>
 * Pointer to spinlock handle.
 * \ie
 *
 * RETURNS: 'IPCOM_SUCCESS', or an error value defined in 'include/ipcom_err.h'.
 *
 * ERRNO: \&'IPCOM_ERR_FAILED'
 */
IP_PUBLIC Ip_err ipcom_spinlock_delete
(
    Ipcom_spinlock *sl
);


/*******************************************************************************
 *
 * ipcom_spinlock_lock - grab the spinlock.
 *
 * This routine will lock the spinlock.
 * Parameter:
 * \is
 * \i <sl>
 * Spinlock handle.
 * \ie
 *
 * RETURNS: \&N/A.
 *
 * ERRNO: None.
 */
IP_FASTTEXT IP_PUBLIC void ipcom_spinlock_lock
(
     Ipcom_spinlock sl
);


/*******************************************************************************
 *
 * ipcom_spinlock_unlock - release the spinlock
 *
 * This routine releases a spinlock grabbed with ipcom_spinlock_lock().
 *
 * Parameter:
 * \is
 * \i <sl>
 * Spinlock handle.
 * \ie
 *
 * RETURNS: \&N/A.
 *
 * ERRNO: None.
 */
IP_FASTTEXT IP_PUBLIC void ipcom_spinlock_unlock
(
    Ipcom_spinlock sl
);


#ifdef __cplusplus
}
#endif

#endif /* IPCOM_SPINLOCK_H */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

