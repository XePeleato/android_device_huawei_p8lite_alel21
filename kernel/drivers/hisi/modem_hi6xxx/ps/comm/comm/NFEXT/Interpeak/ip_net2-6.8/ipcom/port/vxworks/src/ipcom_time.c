/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_time.c,v $ $Revision: 1.20 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_time.c,v $
 *     $Author: jhorteli $ $Date: 2007-12-13 09:00:27 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Roger Boden <roger@interpeak.se>
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


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */
#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_clib.h>
#include <ipcom_sock.h>
#include <ipcom_err.h>

#include <vxWorks.h>
#include <stdio.h>
#include <time.h>
#include <taskLib.h>
#include <tickLib.h>
#include <drv/timer/timerDev.h>


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
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_microtime
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_microtime(struct Ip_timeval *tv)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    tv->tv_sec  = ts.tv_sec;
    tv->tv_usec = ts.tv_nsec / 1000;
}

/*
 *===========================================================================
 *                    ipcom_gettimeofday
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_gettimeofday(struct Ip_timeval *tp, void *nullptr)
{
    struct timespec t_spec;

    (void)nullptr;

    if(tp == IP_NULL)
        return IPCOM_ERR_FAILED;

    if(clock_gettime(CLOCK_REALTIME, &t_spec) == OK)
    {
        /* clock_gettime returns nanoseconds, while Ip_timeval holds micro seconds. */
        tp->tv_sec = t_spec.tv_sec;
        tp->tv_usec = t_spec.tv_nsec / 1000;
    }
    else
    {
        tp->tv_sec = 0;
        tp->tv_usec = 0;
        return IPCOM_ERR_FAILED;
    }
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_settimeofday
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_settimeofday(struct Ip_timeval *tp, void *nullptr)
{
    struct timespec t_spec;

    (void)nullptr;

    if(tp == IP_NULL)
        return IPCOM_ERR_FAILED;

    /* clock_settime expects nanoseconds, while Ip_timeval holds micro seconds. */
    t_spec.tv_sec  = tp->tv_sec;
    t_spec.tv_nsec = tp->tv_usec * 1000;

    if (clock_settime(CLOCK_REALTIME, &t_spec) == OK)
        return IPCOM_SUCCESS;
    else
        return IPCOM_ERR_FAILED;
}


/*
 *===========================================================================
 *                    ipcom_asctime_r_vxworks
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC char *
ipcom_asctime_r_vxworks(const struct tm *tm, char *buf)
{
    size_t len = 26;

    len = asctime_r(tm, buf, &len);
    if (len > 26)
    {
        IP_PANIC();
        return IP_NULL;
    }
    else
        return buf;
}


/*
 *===========================================================================
 *                    ipcom_ctime_r_vxworks
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC char *
ipcom_ctime_r_vxworks(const time_t *timep, char *buf)
{
    size_t len = 26;

    ctime_r(timep, buf, &len);
    if (len > 26)
    {
        IP_PANIC();
        return IP_NULL;
    }
    else
        return buf;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
