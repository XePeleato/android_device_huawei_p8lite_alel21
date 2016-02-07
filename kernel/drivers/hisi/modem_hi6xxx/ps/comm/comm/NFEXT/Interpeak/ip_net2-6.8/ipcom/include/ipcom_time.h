/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_time.h,v $ $Revision: 1.20 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_time.h,v $
 *     $Author: joel $ $Date: 2007-08-22 13:07:14 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_TIME_H
#define IPCOM_TIME_H

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

#include "ipcom_cstyle.h"
#include "ipcom_clibmap.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* ipcom_clock() divided by IP_CLOCKS_PER_SEC -> seconds. */
#define IP_CLOCKS_PER_SEC     1000000

/* ipcom_times() divided by IP_CLK_TCK -> seconds. */ 
#define IP_CLK_TCK            1000      


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         struct Ip_tm
 *===========================================================================
 * The struct Ip_tm type specifies time broken down into culturally semantic
 * components. This representation is also called 'broken down time'. 
 */
struct Ip_tm
{
    int         tm_sec;	 /* Seconds after the minute [0-61] */
    int         tm_min;	 /* Minutes after the hour [0-59] */
    int         tm_hour;	 /* Hours since midnight [0-23] */
    int         tm_mday;	 /* Day of the month [1-31] */
    int         tm_mon;	 /* Months since January [0-11] */
    int         tm_year;	 /* Years since 1900 */
    int         tm_wday;	 /* Days since Sunday [0-6] */
    int         tm_yday;	 /* Days since January 1 [0-365] */
    int         tm_isdst;	 /* Daylight Savings Time flag */

#if defined(IP_PORT_UNIX) || defined(IP_PORT_LAS) || defined(IP_PORT_LKM)
    long int    __tm_gmtoff; /* Seconds east of UTC.  */
    const char  *__tm_zone;  /* Timezone abbreviation.  */
#endif    
};


/*
 *===========================================================================
 *                         Ip_timeval
 *===========================================================================
 */
#ifndef IP_TIMEVAL_TYPE
#define IP_TIMEVAL_TYPE
struct Ip_timeval
{
    long   tv_sec;         /* Seconds. */
    long   tv_usec;        /* Microseconds. */
};
#endif

/* Ip_timeval structure operation macros. */
#define ip_timeval_clear(tp)   (tp)->tv_sec = (tp)->tv_usec = 0
#define ip_timeval_isset(tp)   ((tp)->tv_sec || (tp)->tv_usec)
/* Note: ip_timevalcmp does not work for >= or <= */
#define ip_timeval_cmp(tp1,tp2,CMP) \
   (((tp1)->tv_sec == (tp2)->tv_sec) ? \
   ((tp1)->tv_usec CMP (tp2)->tv_usec) : \
   ((tp1)->tv_sec  CMP (tp2)->tv_sec))
#define ip_timeval_add(tp1,tp2,tpr) \
   do \
   { \
       (tpr)->tv_sec  = (tp1)->tv_sec  + (tp2)->tv_sec; \
       (tpr)->tv_usec = (tp1)->tv_usec + (tp2)->tv_usec; \
       if((tpr)->tv_usec >= 1000000) \
       { \
           (tpr)->tv_usec -= 1000000; \
	       ++(tpr)->tv_sec; \
       } \
   } \
   while((0))
#define ip_timeval_sub(tp1,tp2,tpr) \
   do \
   { \
       (tpr)->tv_sec  = (tp1)->tv_sec  - (tp2)->tv_sec; \
       (tpr)->tv_usec = (tp1)->tv_usec - (tp2)->tv_usec; \
       if((tpr)->tv_usec < 0) \
       { \
           (tpr)->tv_usec += 1000000; \
            --(tpr)->tv_sec; \
       } \
   } \
   while((0))
#define ip_timeval_double(tp) \
   do \
   { \
       (tp)->tv_sec  <<= 1; \
       (tp)->tv_usec <<= 1; \
       while((tp)->tv_usec >= 1000000) \
       { \
           (tp)->tv_usec -= 1000000; \
	       ++(tp)->tv_sec; \
       } \
   } \
   while((0))


/* 
 *=========================================================================== 
 *                         Ip_tms 
 *=========================================================================== 
 * Structure returned by ipcom_times(). All times in milliseconds. 
 */ 
struct Ip_tms  
{ 
    Ip_time_t   tms_utime;     /* user time */ 
    Ip_time_t   tms_stime;     /* system time */ 
    Ip_time_t   tms_cutime;    /* user time, children */ 
    Ip_time_t   tms_cstime;    /* system time, children */ 
};


/* 
 *=========================================================================== 
 *                         Ip_timezone 
 *=========================================================================== 
 * Timezone structure.
 */ 
struct Ip_timezone 
{
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};


/*
 ****************************************************************************
 * 6                    FUNCTION PROTOTYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         ipcom_[g|s]ettimeofday
 *===========================================================================
 * Port specific implementations in port/<rtos>/src/ipcom_time.c
 */

#if defined(IPCOM_GETTIMEOFDAY) && IPCOM_GETTIMEOFDAY != 1
#define ipcom_gettimeofday IPCOM_GETTIMEOFDAY
#elif !defined(IPCOM_GETTIMEOFDAY)
#define ipcom_gettimeofday gettimeofday
#endif

#if (defined(IPCOM_GETTIMEOFDAY) && IPCOM_GETTIMEOFDAY == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int
ipcom_gettimeofday(struct Ip_timeval *tp, void *nullptr);
#endif


#if defined(IPCOM_SETTIMEOFDAY) && IPCOM_SETTIMEOFDAY != 1
#define ipcom_settimeofday IPCOM_SETTIMEOFDAY
#elif !defined(IPCOM_SETTIMEOFDAY)
#define ipcom_settimeofday settimeofday
#endif

#if (defined(IPCOM_SETTIMEOFDAY) && IPCOM_SETTIMEOFDAY == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int
ipcom_settimeofday(struct Ip_timeval *tp, void *nullptr);
#endif


/*
 *===========================================================================
 *                         ipcom_xtime
 *===========================================================================
 * Generic implementations in port/src/ipcom_xtime.c
 */

#if defined(IPCOM_MICROTIME) && IPCOM_MICROTIME != 1
#define ipcom_microtime IPCOM_MICROTIME
#elif !defined(IPCOM_MICROTIME)
#define ipcom_microtime microtime
#endif

#if (defined(IPCOM_MICROTIME) && IPCOM_MICROTIME == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC void ipcom_microtime(struct Ip_timeval *tv);
#endif


#if defined(IPCOM_CLOCK) && IPCOM_CLOCK != 1
#define ipcom_clock IPCOM_CLOCK
#elif !defined(IPCOM_CLOCK)
#define ipcom_clock clock
#endif

#if (defined(IPCOM_CLOCK) && IPCOM_CLOCK == 1) || defined(IPCOM_USE_CLIB_PROTO)
/*  Note: Returns microseconds since CLOCKS_PER_SEC = 1000000 in ANSI C/POSIX */
IP_PUBLIC Ip_clock_t ipcom_clock(void);
#endif


#if defined(IPCOM_TIME) && IPCOM_TIME != 1
#define ipcom_time IPCOM_TIME
#elif !defined(IPCOM_TIME)
#define ipcom_time time
#endif

#if (defined(IPCOM_TIME) && IPCOM_TIME == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC Ip_time_t ipcom_time(Ip_time_t *tloc);
#endif


/*
 *===========================================================================
 *                         ipcom_strftime
 *===========================================================================
 * Generic implementation in port/src/ipcom_strftime.c
 */

#if defined(IPCOM_STRFTIME) && IPCOM_STRFTIME != 1
#define ipcom_strftime IPCOM_STRFTIME
#elif !defined(IPCOM_STRFTIME)
#define ipcom_strftime strftime
#endif

#if (defined(IPCOM_STRFTIME) && IPCOM_STRFTIME == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC Ip_size_t
ipcom_strftime(char *buf, Ip_size_t maxsize, const char *format, const struct Ip_tm *tm);
#endif


/*
 *===========================================================================
 *                         ipcom_times
 *===========================================================================
 * Generic implementation in port/src/ipcom_perror.c - ipcom_perror()
 */

#if defined(IPCOM_TIMES) && IPCOM_TIMES != 1
#define ipcom_times IPCOM_TIMES
#elif !defined(IPCOM_TIMES)
#define ipcom_times times
#endif

#if (defined(IPCOM_TIMES) && IPCOM_TIMES == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC Ip_clock_t ipcom_times(struct Ip_tms *buf);
#endif


/*
 *===========================================================================
 *                    ipcom_gmtime/ipcom_localtime
 *===========================================================================
 * Generic implementations in port/src/ipcom_gmtime.c
 */

#if defined(IPCOM_GMTIME) && IPCOM_GMTIME != 1
#define ipcom_gmtime IPCOM_GMTIME
#elif !defined(IPCOM_GMTIME)
#define ipcom_gmtime gmtime
#endif

#if (defined(IPCOM_GMTIME) && IPCOM_GMTIME == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC struct Ip_tm *
ipcom_gmtime(const Ip_time_t *timep);
#endif


#if defined(IPCOM_GMTIME_R) && IPCOM_GMTIME_R != 1
#define ipcom_gmtime_r IPCOM_GMTIME_R
#elif !defined(IPCOM_GMTIME_R)
#define ipcom_gmtime_r gmtime_r
#endif

#if (defined(IPCOM_GMTIME_R) && IPCOM_GMTIME_R == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC struct Ip_tm *
ipcom_gmtime_r(const Ip_time_t *timep, struct Ip_tm *tm);
#endif


#if defined(IPCOM_LOCALTIME) && IPCOM_LOCALTIME != 1
#define ipcom_localtime IPCOM_LOCALTIME
#elif !defined(IPCOM_LOCALTIME)
#define ipcom_localtime localtime
#endif

#if (defined(IPCOM_LOCALTIME) && IPCOM_LOCALTIME == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC struct Ip_tm *
ipcom_localtime(const Ip_time_t *timep);
#endif


#if defined(IPCOM_LOCALTIME_R) && IPCOM_LOCALTIME_R != 1
#define ipcom_localtime_r IPCOM_LOCALTIME_R
#elif !defined(IPCOM_LOCALTIME_R)
#define ipcom_localtime_r localtime_r
#endif

#if (defined(IPCOM_LOCALTIME_R) && IPCOM_LOCALTIME_R == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC struct Ip_tm *
ipcom_localtime_r(const Ip_time_t *timep, struct Ip_tm *tm);
#endif


#if defined(IPCOM_TZSET) && IPCOM_TZSET != 1
#define ipcom_tzset IPCOM_TZSET
#elif !defined(IPCOM_TZSET)
#define ipcom_tzset tzset
#endif

#if (defined(IPCOM_TZSET) && IPCOM_TZSET == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC void
ipcom_tzset(void);   /* empty function, time zones currently not supported. */
#endif


/*
 *===========================================================================
 *                    ipcom_ctime/ipcom_asctime
 *===========================================================================
 * Generic implementations in port/src/ipcom_ctime.c
 */

#if defined(IPCOM_ASCTIME) && IPCOM_ASCTIME != 1
#define ipcom_asctime IPCOM_ASCTIME
#elif !defined(IPCOM_ASCTIME)
#define ipcom_asctime asctime
#endif

#if (defined(IPCOM_ASCTIME) && IPCOM_ASCTIME == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC char *
ipcom_asctime(const struct Ip_tm *tm);
#endif


#if defined(IPCOM_ASCTIME_R) && IPCOM_ASCTIME_R != 1
#define ipcom_asctime_r IPCOM_ASCTIME_R
#elif !defined(IPCOM_ASCTIME_R)
#define ipcom_asctime_r asctime_r
#endif

#if (defined(IPCOM_ASCTIME_R) && IPCOM_ASCTIME_R == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC char *
ipcom_asctime_r(const struct Ip_tm *tm, char *buf);
#endif


#if defined(IPCOM_CTIME) && IPCOM_CTIME != 1
#define ipcom_ctime IPCOM_CTIME
#elif !defined(IPCOM_CTIME)
#define ipcom_ctime ctime
#endif

#if (defined(IPCOM_CTIME) && IPCOM_CTIME == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC char *
ipcom_ctime(const Ip_time_t *timep);
#endif


#if defined(IPCOM_CTIME_R) && IPCOM_CTIME_R != 1
#define ipcom_ctime_r IPCOM_CTIME_R
#elif !defined(IPCOM_CTIME_R)
#define ipcom_ctime_r ctime_r
#endif

#if (defined(IPCOM_CTIME_R) && IPCOM_CTIME_R == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC char *
ipcom_ctime_r(const Ip_time_t *timep, char *buf);
#endif


/*
 *===========================================================================
 *                         ipcom_mktime
 *===========================================================================
 * Generic implementation in port/src/ipcom_mktime.c.
 */

#if defined(IPCOM_MKTIME) && IPCOM_MKTIME != 1
#define ipcom_mktime IPCOM_MKTIME
#elif !defined(IPCOM_MKTIME)
#define ipcom_mktime mktime
#endif

#if (defined(IPCOM_MKTIME) && IPCOM_MKTIME == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC Ip_time_t ipcom_mktime(struct Ip_tm *tm);
#endif


#ifdef __cplusplus
}
#endif

#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
