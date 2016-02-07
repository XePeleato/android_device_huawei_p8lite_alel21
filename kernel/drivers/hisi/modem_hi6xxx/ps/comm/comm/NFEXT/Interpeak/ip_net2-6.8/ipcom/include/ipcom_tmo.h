/*
******************************************************************************
*                     HEADER FILE
*
*     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_tmo.h,v $ $Revision: 1.15 $
*     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_tmo.h,v $
*     $Author: lob $ $Date: 2004-10-26 14:44:58 $
*     $State: Exp $ $Locker:  $
*
*     INTERPEAK_COPYRIGHT_STRING
*     Design and implementation by Lennart Bang <lob@interpeak.se>
******************************************************************************
*/
#ifndef IPCOM_TMO_H
#define IPCOM_TMO_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Interpeak timeout server API.
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

#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_list.h"

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

/*
 *===========================================================================
 *                         Ipcom_tmo
 *===========================================================================
 * Timeout(tmo) structure to use with ipcom_tmo_xxx() system calls.
 */

/* dummy prototype to solve moment 22. */
struct Ipcom_tmo_struct;

/* User timeout callback function pointer.
 * If the function returns 1, the timeout is reset to its original timeout.
 * Any other return value cancels the timeout.
 */
typedef Ip_s32 (*Ipcom_tmo_func)(struct Ipcom_tmo_struct *tmo, void *cookie);

typedef struct Ipcom_tmo_struct
{
    Ipcom_list        list;      /* List header. */
    Ip_u32            lap;       /* internal 'lap' counter. */
    Ipcom_tmo_func    func;      /* User callback function. */
    void             *cookie;    /* User 32-bit timeout cookie. */
    Ip_u32            msec;      /* Original millisecond timeout. */
}
Ipcom_tmo;

/* Use this macro to initialize a timeout entry. */
#define IPCOM_TMO_INIT(xxtmo)     (xxtmo)->list.next = 0;

/* Use this macro to check if a timeout is pending. */
#define IPCOM_TMO_PENDING(xxtmo)  (xxtmo)->list.next


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_tmo_request
 *===========================================================================
 * Description:   Request a timeout in 'msec' milliseconds. If the timeout
 *                triggers, the function 'func' is called with the timeout
 *                'tmo'.
 * Parameters:    tmo   A pointer to a Ipcom_tmo_tmo structure which stores
 *                      information about the timeout and serves as a handle
 *                      to cancel the timeout.
 *                func  A user callback function that is called if/when the
 *                      timeout triggers.
 *                cookie A user 32-bit value that is passed with the timeout
 *                      callback function 'func'.
 *                msec  Number of timeout milliseconds.
 * Returns:       IPCOM_SUCCESS - Success.
 *
 */
IP_PUBLIC Ip_err ipcom_tmo_request(Ipcom_tmo *tmo, Ipcom_tmo_func func, void *cookie, Ip_u32 msec);


/*
 *===========================================================================
 *                    ipcom_tmo_cancel
 *===========================================================================
 * Description:   Cancel a pending timeout. There is no danger in calling this
 *                function on a timeout that is not pending.
 * Parameters:    tmo - The same Ipcom_tmo timeout handle that was given
 *                      upon requesting the timeout.
 * Returns:       IPCOM_SUCCESS - Success.
 *
 */
IP_PUBLIC Ip_err ipcom_tmo_cancel(Ipcom_tmo *tmo);


/*
 *===========================================================================
 *                    ipcom_tmo_reset
 *===========================================================================
 * Description:   Reset a timeout. The timeout is started over with its
 *                original timeout.
 * Parameters:    tmo - The same Ipcom_tmo timeout handle that was given
 *                      upon requesting the timeout.
 * Returns:       IPCOM_SUCCESS - Success.
 *
 */
IP_PUBLIC Ip_err ipcom_tmo_reset(Ipcom_tmo *tmo);


/*
 *===========================================================================
 *                    ipcom_tmo_get
 *===========================================================================
 * Description:   Get milliseconds left to timeout.
 * Parameters:    tmo - The same Ipcom_tmo timeout handle that was given
 *                      upon requesting the timeout.
 * Returns:       Number of milliseconds before timeout.
 *
 */
IP_PUBLIC Ip_u32 ipcom_tmo_get(Ipcom_tmo *tmo);


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

