/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_ipc_spinlock.c,v $ $Revision: 1.2.8.3 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_ipc_spinlock.c,v $
 *     $Author: dlkrejsa $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"

/* For now, include contents only if Nehalem forwarder is enabled. */
#ifdef IPCOM_USE_NEHALEM_FORWARDER

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_ipc_spinlock.h>

#include <vxWorks.h>
#include <multios_ipc/mipc.h>


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
 * 12                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

IP_PUBLIC void
ipcom_ipc_spinlock_init(Ipcom_ipc_spinlock_t * lock)
{
    ipcom_atomic_set (&lock->flag, 0);
    MULTIOS_MEM_BARRIER();
}

/*
 * ipcom_ipc_spinlock_take() and ipcom_ipc_spinlock_give()
 * are defined in the VxWorks architecture-specific code.
 * Need portable VxWorks version here.
 */

#ifdef IPCOM_PORTABLE_IPC_SPINLOCK
/*
 * TODO -- portable versions of ipcom_ipc_spinlock_take() and
 * ipcom_ipc_spinlock_give().
 */
#endif

#else
int ipcom_ipc_spinlock_empty_file;
#endif /* IPCOM_USE_NEHALEM_FORWARDER */
/*
 ******************************************************************************
 * End of file
 ******************************************************************************
 */
