/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_gmtime.c,v $ $Revision: 1.9 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_gmtime.c,v $
 *     $Author: lob $ $Date: 2004-05-10 17:31:55 $
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
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_time_timetotm
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if (defined(IPCOM_GMTIME_R) && IPCOM_GMTIME_R == 1) || (defined(IPCOM_LOCALTIME_R) && IPCOM_LOCALTIME_R == 1)
IP_STATIC struct Ip_tm *
ipcom_time_timetotm(const Ip_time_t *timep, struct Ip_tm *tm)
{
    Ip_u32   secs;
    Ip_u32   days;
    Ip_u32   years;
    Ip_u32   months;
    int      inc_day = 0;
    static const int days_in_month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    ip_assert(timep != IP_NULL);

    /* Get days and seconds. */
    secs  = (Ip_u32)*timep;
    days  = secs / SECS_IN_DAY;
    secs -= days * SECS_IN_DAY;

    /* Set hours */
    tm->tm_hour = (int)(secs / SECS_IN_HR);
    secs -= tm->tm_hour * SECS_IN_HR;

    /* Set minutes */
    tm->tm_min = (int)secs / SECS_IN_MIN;
    secs -= tm->tm_min * SECS_IN_MIN;

    /* Set seconds */
    tm->tm_sec = (int)secs;

    /* Set week day */
    tm->tm_wday = (int)((days + 4) % 7);	/* + 4 since 1 Jan 1970 was a thursday. */
    
    /* Set years */
    for(years = 1970;; years++)
    {
        if(LEAP_YEAR(years))
        {
            if(days < 366)
                break;
            days -= 366;
        }
        else
        {
            if(days < 365)
                break;
            days -= 365;
        }
    }
    tm->tm_year = (int)(years - 1900);

    /* Set year days */
    tm->tm_yday = (int)days;

    /* Set months. */
    if(LEAP_YEAR(years) && days > 58)
    {
        if(days == 59)
            inc_day = 1;
        days--;  /* Adjust for leap year extra day in february. */
    }
    ip_assert(days <= 365);

    for(months = 0;; months++)
    {
        if(days < (Ip_u32)days_in_month[months])
            break;
        days -= (Ip_u32)days_in_month[months];
    }
    tm->tm_mon = (int)months;

    /* Set month day. */
	tm->tm_mday = (int)(days + 1 + inc_day);

    /*!! Not supported: Set daylight savings time flag. */
	tm->tm_isdst = 0;

    return tm;
}
#endif


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_gmtime_r
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_GMTIME_R) && IPCOM_GMTIME_R == 1
IP_PUBLIC struct Ip_tm *
ipcom_gmtime_r(const Ip_time_t *timep, struct Ip_tm *tm)
{
    return ipcom_time_timetotm(timep, tm);
}
#endif


/*
 *===========================================================================
 *                    ipcom_gmtime
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_GMTIME) && IPCOM_GMTIME == 1
IP_PUBLIC struct Ip_tm *
ipcom_gmtime(const Ip_time_t *timep)
{
    static struct Ip_tm  tm;
 
    return ipcom_gmtime_r(timep, (struct Ip_tm *)&tm);
}
#endif


/*
 *===========================================================================
 *                    ipcom_tzset
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_TZSET) && IPCOM_TZSET == 1
IP_PUBLIC void
ipcom_tzset(void)
{
}
#endif


/*
 *===========================================================================
 *                    ipcom_localtime_r
 *===========================================================================
 * Context:
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_LOCALTIME_R) && IPCOM_LOCALTIME_R == 1
IP_PUBLIC struct Ip_tm *
ipcom_localtime_r(const Ip_time_t *timep, struct Ip_tm *tm)
{
    /*!!timezone*/
    return ipcom_time_timetotm(timep, tm);
}
#endif


/*
 *===========================================================================
 *                    ipcom_localtime
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_LOCALTIME) && IPCOM_LOCALTIME == 1
IP_PUBLIC struct Ip_tm *
ipcom_localtime(const Ip_time_t *timep)
{
    static struct Ip_tm  tm;
 
    /*!!timezone*/
    return ipcom_localtime_r(timep, (struct Ip_tm *)&tm);
}
#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

