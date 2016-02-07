/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_mktime.c,v $ $Revision: 1.11 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_mktime.c,v $
 *     $Author: markus $ $Date: 2004-11-29 14:13:04 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by John Lawitzke <jhl@interpeak.se>
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

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_clib.h"
#include "ipcom_time.h"


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

#define SECS_IN_MIN            60
#define MINS_IN_HR             60
#define HRS_IN_DAY             24
#define DAYS_IN_NON_LEAP_YR    365

#define SECS_IN_HR             (SECS_IN_MIN * MINS_IN_HR)
#define SECS_IN_DAY            (SECS_IN_HR  * HRS_IN_DAY)
#define SECS_IN_NON_LEAP_YR    (SECS_IN_DAY * DAYS_IN_NON_LEAP_YR)

#define LEAP_YEAR(yr)          ((((yr) & 0x3) == 0 && ((yr) % 100) != 0) || ((yr) % 400) == 0)


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
 *===========================================================================
 *                         ipcom_mktime_secs_in_prev_mons_cumul
 *===========================================================================
 */
#if defined(IPCOM_MKTIME) && IPCOM_MKTIME == 1
IP_STATIC Ip_time_t ipcom_mktime_secs_in_prev_mons_cumul[12] =
{
    0,
    ( 31 * SECS_IN_DAY),
    ( 59 * SECS_IN_DAY),
    ( 90 * SECS_IN_DAY),
    (120 * SECS_IN_DAY),
    (151 * SECS_IN_DAY),
    (181 * SECS_IN_DAY),
    (212 * SECS_IN_DAY),
    (243 * SECS_IN_DAY),
    (273 * SECS_IN_DAY),
    (304 * SECS_IN_DAY),
    (334 * SECS_IN_DAY)
};
#endif


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
 *                    ipcom_mktime
 *===========================================================================
 * Description: Converts a broken down time structure for local time to
 *              calendar time.  Calendar time is defined as the number of
 *              seconds elapsed since 00:00:00 on January 1, 1970 UTC
 * Parameters:  struct Ip_tm *tm - broken down local time structure
 * Returns:     Ip_time_t - seconds since 00:00:00, Jan. 1, 1970 UTC
 *
 */
#if defined(IPCOM_MKTIME) && IPCOM_MKTIME == 1
IP_PUBLIC Ip_time_t
ipcom_mktime(struct Ip_tm *tm)
{
    Ip_time_t  secs;
    int        yrs_since_1970;
    int        leap_yrs_since_1970;

    /* Handle previous full years since beginning of 1970. */
    yrs_since_1970 = (int)(tm->tm_year - 70);
    secs = (Ip_time_t)(yrs_since_1970 * SECS_IN_NON_LEAP_YR);

    /* Non-intuitive +1 follows, we're working on previous years here
     * and not the current year.
     * Note: this will break in 2400AD, which is not a leap year
     */
    leap_yrs_since_1970 = (yrs_since_1970 + 1) / 4;
    secs += (Ip_time_t)(leap_yrs_since_1970 * SECS_IN_DAY);

    /* Handle previous full days in this year. */
    secs += ipcom_mktime_secs_in_prev_mons_cumul[tm->tm_mon];
    secs += (Ip_time_t)((tm->tm_mday - 1) * SECS_IN_DAY);

    /* If this year is a leap year and we're past February 29, add the day in. */
    if (LEAP_YEAR(tm->tm_year + 1900) && tm->tm_mon > 1)  /* tm->mon is zero-based */
      secs += SECS_IN_DAY;

    /* If daylight savings time is in effect, subtract 1 hour. */
    if (tm->tm_isdst != 0)
      secs -= SECS_IN_HR;

    /* Handle the current day. */
    secs += (Ip_time_t)(tm->tm_hour * SECS_IN_HR);
    secs += (Ip_time_t)(tm->tm_min * SECS_IN_MIN);
    secs += (Ip_time_t)tm->tm_sec;

    /*!! TODO - adjust for local time zone from GMT */

    return secs;
}

#else
int ipcom_mktime_empty_file;
#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

