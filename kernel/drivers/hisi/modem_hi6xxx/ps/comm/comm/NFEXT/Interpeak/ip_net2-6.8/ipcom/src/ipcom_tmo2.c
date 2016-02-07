/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_tmo2.c,v $ $Revision: 1.6.110.1 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_tmo2.c,v $
 *     $Author: ulf $ $Date: 2010-07-02 13:21:00 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Interpeak timeout module. The difference between this module and the one
 * in ipcom_tmo.h is that this module does not use an external timeout daemon.
 * It is driven by the process itself, the restriction is that it can only
 * be used by application that is driven solely by socket events.
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
#include "ipcom_list.h"
#include "ipcom_os.h"
#include "ipcom_syslog.h"
#include "ipcom_time.h"
#include "ipcom_tmo2.h"


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
 *===========================================================================
 *                    ipcom_tmo2_pqueue_cmp_func
 *===========================================================================
 * Description: Compares two timeouts entries.
 * Parameters:  tmo1 -
 *              tmo2 -
 * Returns:     1 if tmo1 timeouts before tmo2
 *              0 if tmo1 and tmo2 times out at the same time
 *             -1 if tmo1 timeouts after tmo2
 *
 */
IP_STATIC int
ipcom_tmo2_pqueue_cmp_func(IP_CONST Ipcom_tmo2 *tmo1,
                           IP_CONST Ipcom_tmo2 *tmo2)
{
    if (ip_timeval_cmp(&tmo1->abs, &tmo2->abs, <))
        return 1;
    if (ip_timeval_cmp(&tmo1->abs, &tmo2->abs, >))
        return -1;
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_tmo2_pqueue_store_index
 *===========================================================================
 * Description: Stores the index the passed object has in the priority queue.
 * Parameters:  tmo - the timeout object that has been stored in the queue.
 *              indx - the index the timeout object has in the queue.
 * Returns:
 *
 */
IP_STATIC void
ipcom_tmo2_pqueue_store_index(Ipcom_tmo2 *tmo, int indx)
{
    tmo->pqindex = indx;
}


/*
 ****************************************************************************
 * 10                    PUBLIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_tmo2_new
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ipcom_pqueue *
ipcom_tmo2_new(void)
{
    return ipcom_pqueue_new((Ipcom_pqueue_cmp_func) ipcom_tmo2_pqueue_cmp_func,
                            (Ipcom_pqueue_store_index) ipcom_tmo2_pqueue_store_index);
}



/*
 *===========================================================================
 *                    ipcom_tmo2_delete
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_tmo2_delete(Ipcom_pqueue *head)
{
    ipcom_pqueue_delete(head);
}


/*
 *===========================================================================
 *                     ipcom_tmo2_request
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_tmo2_request(Ipcom_pqueue *head,
                   Ipcom_tmo2 *tmo,
                   Ipcom_tmo2_func func,
                   void *cookie,
                   Ip_u32 msec)
{
    struct Ip_timeval duration;

    duration.tv_sec  = msec / 1000;
    duration.tv_usec = (msec % 1000) * 1000;
    return ipcom_tmo2_request_timeval(head, tmo, func, cookie, &duration);
}


/*
 *===========================================================================
 *                     ipcom_tmo2_request
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_tmo2_request_timeval(Ipcom_pqueue *head,
                           Ipcom_tmo2 *tmo,
                           Ipcom_tmo2_func func,
                           void *cookie,
                           IP_CONST struct Ip_timeval *duration)
{
    if (IPCOM_TMO2_PENDING(tmo))
    {
        IP_PANIC2();
        return IPCOM_ERR_ALREADY_STARTED;
    }

    tmo->head       = head;
    tmo->func       = func;
    tmo->cookie     = cookie;
    tmo->duration   = *duration;

    ipcom_microtime(&tmo->abs);
    ip_timeval_add(&tmo->abs, duration, &tmo->abs);

    return ipcom_pqueue_insert(head, tmo);
}


/*
 *===========================================================================
 *                     ipcom_tmo2_cancel
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_tmo2_cancel(Ipcom_tmo2 *tmo)
{
    if (IPCOM_TMO2_PENDING(tmo) == IP_FALSE)
    {
        IP_PANIC2();
        return IPCOM_ERR_NOT_STARTED;
    }

    (void)ipcom_pqueue_remove(tmo->head, tmo->pqindex);
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                     ipcom_tmo2_reset
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_tmo2_reset(Ipcom_tmo2 *tmo)
{
    if (IPCOM_TMO2_PENDING(tmo) == IP_FALSE)
    {
        IP_PANIC2();
        return IPCOM_ERR_NOT_STARTED;
    }
    (void)ipcom_pqueue_remove(tmo->head, tmo->pqindex);
    return ipcom_tmo2_request_timeval(tmo->head,
                                      tmo, tmo->func,
                                      tmo->cookie,
                                      &tmo->duration);
}



/*
 *===========================================================================
 *                     ipcom_tmo2_get
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_u32
ipcom_tmo2_get(Ipcom_tmo2 *tmo)
{
    struct Ip_timeval tv;

    if (IPCOM_TMO2_PENDING(tmo) == IP_FALSE)
        return 0;

    ipcom_microtime(&tv);
    ip_timeval_sub(&tmo->abs, &tv, &tv);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}


/*
 *===========================================================================
 *                     ipcom_tmo2_select
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_tmo2_select(Ipcom_pqueue *head,
                  int nfds,
                  Ip_fd_set *readfds,
                  Ip_fd_set *writefds,
                  Ip_fd_set *exceptfds,
                  struct Ip_timeval *timeout)
{
    struct Ip_timeval  tv;
    struct Ip_timeval *ptv;
    Ipcom_tmo2        *tmo;
    Ip_fd_set          rset;
    Ip_fd_set          wset;
    Ip_fd_set          eset;
    Ip_bool            user_tmo;
    int                ret;

    /* Save the callers select set(s) */
    if (readfds)
        rset = *readfds;
    if (writefds)
        wset = *writefds;
    if (exceptfds)
        eset = *exceptfds;

    for (;;)
    {
        ipcom_microtime(&tv);
        while (IP_NULL != (tmo = ipcom_pqueue_get_next(head))
               && !ip_timeval_cmp(&tmo->abs, &tv, >))
        {
            tmo = ipcom_pqueue_remove_next(head);
            ret = tmo->func(tmo->cookie);
            if (ret)
                (void)ipcom_tmo2_request_timeval(tmo->head,
                                                tmo,
                                                tmo->func,
                                                tmo->cookie,
                                                &tmo->duration);
        }

        ptv = timeout;
        user_tmo = IP_TRUE;
        if (tmo)
        {
            ip_timeval_sub(&tmo->abs, &tv, &tv);
            if (timeout == IP_NULL || ip_timeval_cmp(&tv, timeout, <))
            {
                ptv = &tv;
                user_tmo = IP_FALSE;
                if (timeout != IP_NULL)
                    ip_timeval_sub(timeout, &tv, timeout);
            }
        }

        ret = ipcom_socketselect(nfds, readfds, writefds, exceptfds, ptv);
        if (ret > 0
            || (ret < 0 && ipcom_errno != IP_ERRNO_EINTR)
            || (ret == 0 && user_tmo))
            /* Sockets ready || got error (but interrupts are ignored) || user timeout */
            break;

        /* Restore the callers original select set(s) */
        if (readfds)
            *readfds = rset; /*lint !e644 */
        if (writefds)
            *writefds = wset; /*lint !e644 */
        if (exceptfds)
            *exceptfds = eset; /*lint !e644 */
    }
    return ret;
}

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */


