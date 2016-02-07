/*
******************************************************************************
*                     HEADER FILE
*
*     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_tmo2.h,v $ $Revision: 1.4 $
*     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_tmo2.h,v $
*     $Author: kenneth $ $Date: 2005-11-21 08:26:26 $
*     $State: Exp $ $Locker:  $
*
*     INTERPEAK_COPYRIGHT_STRING
*     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
******************************************************************************
*/
#ifndef IPCOM_TMO2_H
#define IPCOM_TMO2_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Interpeak timeout module. The difference between this module and the one
 * in ipcom_tmo.h is that this module does not use an external timeout daemon.
 * It is driven by the process itself, the restriction is that it can only
 * be used by application that is driven solely by socket events.
 * Applications using this API must use ipcom_tmo2_select() instead of
 * ipcom_socketselect() so that the timeout handlers can be run when needed.
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

#include "ipcom_cstyle.h"
#include "ipcom_pqueue.h"
#include "ipcom_sock.h"
#include "ipcom_type.h"

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
 *                      Ipcom_tmo2_func
 *===========================================================================
 * Description: User defined timeout handler.
 * Parameters:  cookie - User defined data specified when the timeout was
 *                       scheduled.
 * Returns:     IP_TRUE if this timeout should be reset to its original timeout.
 *              IP_FALSE if this timeout should not be rescheduled.
 */
typedef Ip_bool (*Ipcom_tmo2_func)(void *cookie);

typedef struct Ipcom_tmo2_struct
{
    Ipcom_pqueue     *head;
    int               pqindex;    /* The index this entry has in the timeout priority queue */
    struct Ip_timeval duration;   /* The original timeout duration */
    struct Ip_timeval abs;        /* Absolute time when this entry expires */
    Ipcom_tmo2_func   func;       /* User callback function. */
    void             *cookie;     /* User timeout cookie. */
}
Ipcom_tmo2;


/* Use this macro to initialize a timeout entry. */
#define IPCOM_TMO2_INIT(xxtmo)     (xxtmo)->pqindex = 0;

/* Use this macro to check if a timeout is pending. */
#define IPCOM_TMO2_PENDING(xxtmo)  ((xxtmo)->pqindex != 0)


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_tmo2_new
 *===========================================================================
 * Description: Creates a new timeout queue.
 * Parameters:
 * Returns:     IP_NULL if out of memory or a pointer to the new timeout queue.
 *
 */
IP_PUBLIC Ipcom_pqueue * ipcom_tmo2_new(void);


/*
 *===========================================================================
 *                    ipcom_tmo2_delete
 *===========================================================================
 * Description: Frees all resources helds by a timeout queue.
 * Parameters:  head - The timeout queue to delete.
 * Returns:
 *
 */
IP_PUBLIC void ipcom_tmo2_delete(Ipcom_pqueue *head);


/*
 *===========================================================================
 *                    ipcom_tmo2_request
 *===========================================================================
 * Description:   Request a timeout in 'msec' milliseconds. If the timeout
 *                triggers, the function 'func' is called with the timeout
 *                'tmo'.
 * Parameters:    head - The timeout queue head where.
 *                tmo   A pointer to a Ipcom_tmo2_tmo structure which stores
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
IP_PUBLIC Ip_err ipcom_tmo2_request(Ipcom_pqueue *head,
                                    Ipcom_tmo2 *tmo,
                                    Ipcom_tmo2_func func,
                                    void *cookie,
                                    Ip_u32 msec);


/*
 *===========================================================================
 *                    ipcom_tmo2_request_timeval
 *===========================================================================
 * Description:   Request a timeout in using the timeval structure.
 *                This allows for longer duration that using 'msec'.
 *                If the timeout triggers, the function 'func' is called with
 *                the timeout 'tmo'.
 * Parameters:    head - The timeout queue head where.
 *                tmo   A pointer to a Ipcom_tmo2_tmo structure which stores
 *                      information about the timeout and serves as a handle
 *                      to cancel the timeout.
 *                func  A user callback function that is called if/when the
 *                      timeout triggers.
 *                cookie A user 32-bit value that is passed with the timeout
 *                      callback function 'func'.
 *                duration  Duration until the timeout will occur.
 * Returns:       IPCOM_SUCCESS - Success.
 *
 */
IP_PUBLIC Ip_err ipcom_tmo2_request_timeval(Ipcom_pqueue *head,
                                            Ipcom_tmo2 *tmo,
                                            Ipcom_tmo2_func func,
                                            void *cookie,
                                            IP_CONST struct Ip_timeval *duration);

/*
 *===========================================================================
 *                    ipcom_tmo2_cancel
 *===========================================================================
 * Description:   Cancel a pending timeout. There is no danger in calling this
 *                function on a timeout that is not pending.
 * Parameters:    tmo - The same Ipcom_tmo timeout handle that was given
 *                      upon requesting the timeout.
 * Returns:       IPCOM_SUCCESS - Success.
 *                IPCOM_ERR_NOT_STARTED - No such timeout pending.
 */
IP_PUBLIC Ip_err ipcom_tmo2_cancel(Ipcom_tmo2 *tmo);


/*
 *===========================================================================
 *                    ipcom_tmo2_reset
 *===========================================================================
 * Description:   Reset a timeout. The timeout is started over with its
 *                original timeout.
 * Parameters:    tmo - The same Ipcom_tmo timeout handle that was given
 *                      upon requesting the timeout.
 * Returns:       IPCOM_SUCCESS - Success.
 *
 */
IP_PUBLIC Ip_err ipcom_tmo2_reset(Ipcom_tmo2 *tmo);


/*
 *===========================================================================
 *                    ipcom_tmo2_get
 *===========================================================================
 * Description:   Get milliseconds left to timeout.
 * Parameters:    tmo - The same Ipcom_tmo timeout handle that was given
 *                      upon requesting the timeout.
 * Returns:       Number of milliseconds before timeout.
 *
 */
IP_PUBLIC Ip_u32 ipcom_tmo2_get(Ipcom_tmo2 *tmo);


/*
 *===========================================================================
 *                    ipcom_tmo2_select
 *===========================================================================
 * Description: Processes any pending timeouts and then puts the process
 *              to sleep until at least one of the sockets reports an event
 *              or until the timeout expires.
 * Parameters:  head - The timeout queue head.
 *              nfds - The first 'nfds' number of sockets in each set are
 *                     tested for events.
 *              readfds - Set of sockets to be tested for readability.
 *              writefds - Set of sockets to be tested for writability.
 *              execptfds - Set of sockets to be tested for exceptions.
 *              timeout - Time that can elaps before this operation must
 *                        return. IP_NULL means infinite time.
 * Returns:     0 = The specified 'timeout' has expired.
 *             >0 = Number of sockets that has events to report.
 *             <0 = Error, error code can be read from ipcom_errno.
 */
IP_PUBLIC int ipcom_tmo2_select(Ipcom_pqueue *head,
                                int nfds,
                                Ip_fd_set *readfds,
                                Ip_fd_set *writefds,
                                Ip_fd_set *exceptfds,
                                struct Ip_timeval *timeout);


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

