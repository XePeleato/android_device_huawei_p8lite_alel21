/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_tmo.c,v $ $Revision: 1.23 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_tmo.c,v $
 *     $Author: lob $ $Date: 2006-11-23 10:31:55 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Interpeak timeout server.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"


/* Timeout server tick resolution.
 * The tick interval of the Interpeak timeout server 'ipcom_tickd' used to
 * tick the timeouts.
 */
#define IPCOM_TMO_RESOLUTION_MSEC   100


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_tmo.h"
#include "ipcom_err.h"
#include "ipcom_list.h"
#include "ipcom_clib.h"
#include "ipcom_os.h"
#include "ipcom_syslog.h"


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

#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM

/* Protect timeout server API functions from race conditions. */
#define IPCOM_USE_TMOMUTEX

#define IPCOM_TMO_SIZE   64
#define IPCOM_TMO_MASK   (IPCOM_TMO_SIZE - 1)
#define IPCOM_TMO_SHIFT  6

#ifndef IPCOM_USE_TMOMUTEX
#define ipcom_mutex_create(x)     IP_NOOP
#define ipcom_mutex_delete(x)     IP_NOOP
#define ipcom_mutex_lock(x)       IP_NOOP
#define ipcom_mutex_unlock(x)     IP_NOOP
#endif

#define IPCOM_TMO_ACTIVE   0xffffffff


/*
 ****************************************************************************
 * 6                    TYPES
 ****************************************************************************
 */


/*
 *===========================================================================
 *                   Global ipcom_tmo data
 *===========================================================================
 * Internal global Ipcom_tmo data.
 */
typedef struct Ipcom_tmo_data_struct
{
#ifdef IPCOM_USE_TMOMUTEX
    Ipcom_mutex   mutex;
#endif

    /* Timeout server */
    Ipcom_list  tmo[IPCOM_TMO_SIZE];  /* Timeout tick array. */
    Ip_u32      tick;              /* Timeout tick array pointer. */
    Ip_u32      msec_tmo_res;      /* Minimum milliseconds between each timeout. */
    Ip_u32      msec_tmo_acc;      /* Accumulated msec. */

    Ipcom_list *next;    /* next timeout entry in tickd */
}
Ipcom_tmo_data;


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_STATIC Ip_err ipcom_tmo_init(void *ununsed);

IP_STATIC IPCOM_PROCESS(ipcom_tickd);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 *===========================================================================
 *                   Global ipcom_tmo data
 *===========================================================================
 */
IP_STATIC Ipcom_tmo_data   ipcom_tmo;

IP_STATIC Ipcom_once_t  ipcom_tmo_once = IPCOM_ONCE_INIT;


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_tickd
 *===========================================================================
 * Description:
 *
 */
IP_STATIC
IPCOM_PROCESS(ipcom_tickd)
{
    ipcom_proc_init();

    IPCOM_LOG1(DEBUG, "ipcom_tickd :: starting tickd with %ld ms resolution",
               IPCOM_TMO_RESOLUTION_MSEC);

    /*********** tick loop ***********/
    for (;;)
    {
        /* Sleep for X milliseconds. */
        (void)ipcom_millisleep(IPCOM_TMO_RESOLUTION_MSEC);

        ipcom_mutex_lock(ipcom_tmo.mutex);
        ipcom_tmo.msec_tmo_acc += IPCOM_TMO_RESOLUTION_MSEC;

        /* Check the timeout lists. */
        while(ipcom_tmo.msec_tmo_acc >= IPCOM_TMO_RESOLUTION_MSEC)
        {
            Ipcom_list *entry;
            Ipcom_tmo  *tmo;

            ipcom_tmo.msec_tmo_acc -= IPCOM_TMO_RESOLUTION_MSEC;
            if (++ipcom_tmo.tick == IPCOM_TMO_SIZE)
                ipcom_tmo.tick = 0;

            for (entry = ipcom_list_first(&ipcom_tmo.tmo[ipcom_tmo.tick]);
                 entry; entry = ipcom_tmo.next)
            {
                ipcom_tmo.next = ipcom_list_next(entry);
                tmo = (Ipcom_tmo *)entry;

                if (tmo->lap != 0)
                    tmo->lap--;
                else
                {
                    /* Remember this timeout's callback func. */
                    Ipcom_tmo_func   func;
                    void    *cookie;
                    Ip_err   retval;

                    func   = tmo->func;
                    cookie = tmo->cookie;

                    /* Cancel the timeout first so the same one can be reused. */
                    if (IPCOM_TMO_PENDING(tmo))
                    {
                        ipcom_list_remove(&tmo->list);
                        IPCOM_TMO_INIT(tmo);
                    }

                    /* Mark timeout active */
                    tmo->lap = IPCOM_TMO_ACTIVE;

                    /* Call the timeout callback function (unlocked). */
                    ipcom_mutex_unlock(ipcom_tmo.mutex);
                    retval = func(tmo, cookie);
                    ipcom_mutex_lock(ipcom_tmo.mutex);

                    /* Unmark timeout active */
                    if (tmo->lap == IPCOM_TMO_ACTIVE)
                        tmo->lap = 0;

                    if (retval == 1)
                    {
                        Ip_u32  ticks;

                        /* request a new timeout. Since already on an exact tick, we may not add '1'. */
                        ticks    = tmo->msec / IPCOM_TMO_RESOLUTION_MSEC;
                        if (tmo->msec % IPCOM_TMO_RESOLUTION_MSEC)
                            ticks++;
                        ip_assert(ticks > 0);
                        tmo->lap = (ticks - 1) >> IPCOM_TMO_SHIFT;
                        ticks    = (ipcom_tmo.tick + ticks) & IPCOM_TMO_MASK;

                        ipcom_list_insert_first(&ipcom_tmo.tmo[ticks], &tmo->list);
                    }
                }
            }
            ipcom_tmo.next = IP_NULL;
        }

        ipcom_mutex_unlock(ipcom_tmo.mutex);
    } /* for(;;) */

    /*NOT REACHED: ipcom_proc_exit();*/
}


/*
 *===========================================================================
 *                    ipcom_tmo_init
 *===========================================================================
 * Description:   Initialize and create timeout server.
 * Parameters:    .
 * Returns:       .
 *
 */
IP_STATIC Ip_err
ipcom_tmo_init(void *unused)
{
    int i;
    Ip_err  err;
    Ipcom_proc_attr  attr;

    (void)unused;

    ipcom_memset(&ipcom_tmo, 0, sizeof(Ipcom_tmo_data));

    for (i = 0; i < IPCOM_TMO_SIZE; i++)
        ipcom_list_init(&ipcom_tmo.tmo[i]);

    err = ipcom_mutex_create(&ipcom_tmo.mutex);
    ip_assert(err == IPCOM_SUCCESS);
    if (err !=  IPCOM_SUCCESS)
        return err;

    ipcom_proc_attr_init(&attr);
    attr.priority = IPCOM_PRIORITY_MAX;
    err = ipcom_proc_acreate("ipcom_tickd", ipcom_tickd, &attr, IP_NULL);
    ip_assert(err == IPCOM_SUCCESS);

    return err;
}


/*
 ****************************************************************************
 * 10                    PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_tmo_request
 *===========================================================================
 * Description:   Request a timeout in 'msec' milliseconds. If the timeout
 *                triggers the function 'func' is called with the timeout
 *                'tmo'.
 * Parameters:    tmo   A pointer to a Ipcom_tmo structure which stores
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
IP_PUBLIC Ip_err
ipcom_tmo_request(Ipcom_tmo *tmo, Ipcom_tmo_func func, void *cookie, Ip_u32 msec)
{
    Ip_u32  ticks;
    Ip_err  err;

    err = ipcom_once(&ipcom_tmo_once, ipcom_tmo_init, IP_NULL);
    if (err != IPCOM_SUCCESS)
        return err;

#ifdef IP_ASSERT
    {
        int i;
        Ipcom_list *entry, *next;

        ipcom_mutex_lock(ipcom_tmo.mutex);
        for (i = 0; i < IPCOM_TMO_SIZE; i++)
            for (entry = ipcom_list_first(&ipcom_tmo.tmo[i]); entry; entry = next)
            {
                next = ipcom_list_next(entry);
                ip_assert(entry != &tmo->list);
            }
        ipcom_mutex_unlock(ipcom_tmo.mutex);
    }
#endif

    tmo->func = func;
    tmo->cookie = cookie;
    tmo->msec = msec;

    ipcom_mutex_lock(ipcom_tmo.mutex);

    ticks    = msec / IPCOM_TMO_RESOLUTION_MSEC;
    tmo->lap = ticks >> IPCOM_TMO_SHIFT;
    ticks    = (ipcom_tmo.tick + ticks + 1) & IPCOM_TMO_MASK;

    ipcom_list_insert_first(&ipcom_tmo.tmo[ticks], &tmo->list);

    ipcom_mutex_unlock(ipcom_tmo.mutex);

#ifdef IP_ASSERT2
    {
        Ip_u32  msec2;
        msec2 = ipcom_tmo_get(tmo);
        if (msec2 != 0)
        {
            ip_assert(IP_ABS((int)msec2 - (int)msec) <= 100);
        }
    }
#endif

    return IPCOM_SUCCESS;
}


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
IP_PUBLIC Ip_err
ipcom_tmo_cancel(Ipcom_tmo *tmo)
{
    Ip_err  err;

    err = ipcom_once(&ipcom_tmo_once, ipcom_tmo_init, IP_NULL);
    if (err != IPCOM_SUCCESS)
        return err;

    ipcom_mutex_lock(ipcom_tmo.mutex);

    /* Only cancel timeout if it is running (baby safe). */
    if (IPCOM_TMO_PENDING(tmo))
    {
        if (&tmo->list == ipcom_tmo.next)
            /* Pending timeout is being cancelled, skip it. */
            ipcom_tmo.next = ipcom_list_next(ipcom_tmo.next);
        ipcom_list_remove(&tmo->list);
        IPCOM_TMO_INIT(tmo);
    }
    else if (tmo->lap == IPCOM_TMO_ACTIVE)
        err = IPCOM_ERR_PENDING;

    ipcom_mutex_unlock(ipcom_tmo.mutex);

    return err;
}


/*
 *===========================================================================
 *                    ipcom_tmo_reset
 *===========================================================================
 * Description:   Reset a timeout. The timeout is started over with its
 *                original time.
 * Parameters:    tmo - The same Ipcom_tmo timeout handle that was given
 *                      upon requesting the timeout.
 * Returns:       IPCOM_SUCCESS - Success.
 *
 */
IP_PUBLIC Ip_err
ipcom_tmo_reset(Ipcom_tmo *tmo)
{
    Ip_err   retval;

    /* First cancel the timeout if it is running. */
    retval = ipcom_tmo_cancel(tmo);
    if (retval != IPCOM_SUCCESS)
        return retval;

    /* Request a new timeout with the same time. */
    return ipcom_tmo_request(tmo, tmo->func, tmo->cookie, tmo->msec);
}


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
IP_PUBLIC Ip_u32
ipcom_tmo_get(Ipcom_tmo *tmo)
{
    Ip_u32  ret = 0;
    int  indx;

    if (ipcom_once(&ipcom_tmo_once, ipcom_tmo_init, IP_NULL) != IPCOM_SUCCESS)
        return 0;

    ipcom_mutex_lock(ipcom_tmo.mutex);
    if (!IPCOM_TMO_PENDING(tmo))
        goto leave;

    /* Lap counter */
    ret = tmo->lap << IPCOM_TMO_SHIFT;

    /* Slots */
    indx = ((Ip_ptrdiff_t)tmo->list.head - (Ip_ptrdiff_t)ipcom_tmo.tmo) / sizeof(Ipcom_list);
    indx = (int)indx - ipcom_tmo.tick;
    if (indx < 0)
        indx += IPCOM_TMO_SIZE;
    ret += indx;

    /* Resolution */
    ret *= IPCOM_TMO_RESOLUTION_MSEC;

 leave:
    ipcom_mutex_unlock(ipcom_tmo.mutex);
    return ret;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */


