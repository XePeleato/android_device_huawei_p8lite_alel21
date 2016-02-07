/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_strftime.c,v $ $Revision: 1.3 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_strftime.c,v $
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
 *                    ipcom_strftime
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_STRFTIME) && IPCOM_STRFTIME == 1
IP_PUBLIC Ip_size_t
ipcom_strftime(char *buf, Ip_size_t maxsize, const char *format, const struct Ip_tm *tm)
{
    Ip_size_t  size = 0;
    const char *add;
    char addbuf[32];
    static const char *day_name[8] =
        { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "ERR"  };
    static const char *abbr_day_name[8] =
        { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "ERR"  };
    static const char *month_name[13] = 
        { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December", "ERR"  };
    static const char *abbr_month_name[13] = 
        { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", "ERR"  };

    ip_assert(buf != IP_NULL);
    ip_assert(format != IP_NULL);

    for(; *format && size < maxsize; format++)
    {
        if(*format != '%')
        {
            buf[size++] = *format;
            continue;
        }

        format++;
        add = addbuf;
        switch(*format)
        {
        case 'A':  /* full weekday name */
            add = day_name[IP_MIN(tm->tm_wday, 7)];
            break; 
        case 'a':  /* abbreviated weekday name */
            add = abbr_day_name[IP_MIN(tm->tm_wday, 7)];
            break;
        case 'B':  /* full month name */
            add = month_name[IP_MIN(tm->tm_mon, 12)];
            break;
        case 'b':  /* abbreviated month name */
        case 'h':
            add = abbr_month_name[IP_MIN(tm->tm_mon, 12)];
            break;
        case 'm':  /* month as a decimal number [01,12] */
            ipcom_sprintf(addbuf, "%.2d", tm->tm_mon);
            break;
        case 'd':  /* day of the month as a decimal number [01,31]. */
            ipcom_sprintf(addbuf, "%.2d", tm->tm_mday);
            break;
        case 'Y':  /* year with century as a decimal number */
            ipcom_sprintf(addbuf, "%4d", tm->tm_year + 1900);
            break;
        case 'y':  /* year without century as a decimal number [00,99] */
            ipcom_sprintf(addbuf, "%.2d", tm->tm_year % 100);
            break;
        case 'H':  /* hour (24-hour clock) as a decimal number [00,23] */
            ipcom_sprintf(addbuf, "%.2d", tm->tm_hour);
            break;
        case 'M':  /* minute as a decimal number [00,59] */
            ipcom_sprintf(addbuf, "%.2d", tm->tm_min);
            break;
        case 'S':  /* second as a decimal number [00,61] */
            ipcom_sprintf(addbuf, "%.2d", tm->tm_sec);
            break;
        case 'D':  /* date in the format ``%m/%d/%y */
            ipcom_sprintf(addbuf, "%.2d/%.2d/%.2d", tm->tm_mon, tm->tm_mday, tm->tm_year % 100);
            break;
        case 'T': /* time in the format ``%H:%M:%S'' */
            ipcom_sprintf(addbuf, "%.2d:%.2d:%.2d", tm->tm_hour, tm->tm_min, tm->tm_sec);
            break;
        default:
            ipcom_sprintf(addbuf, "%%%c", *format);
            break;
        }

        /* Add field. */
        for(; *add && size < maxsize; size++)
        {
            buf[size] = *add;
            add++;
        }
    }

    /* Null terminate 'buf'. */
    if(size < maxsize)
        buf[size] = '\0';
    else
        goto overflow;

    /* Succcess. */
    return size;

overflow:
    IP_PANIC2();
    return 0;
}

#else
int ipcom_strftime_empty_file;
#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

