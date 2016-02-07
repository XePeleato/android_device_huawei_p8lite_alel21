/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_syslogd.c,v $ $Revision: 1.96 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_syslogd.c,v $
 *     $Author: rboden $
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

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#include "ipcom_config.h"

/* Default syslog daemon flags. */
#define IPCOM_SYSLOG_FLAGS          (IP_LOG_PID)

/* Optimize syslog on memory instead of speed. */
#define IPCOM_SYSLOG_MEM_OPT

/* Define to enable syslog file system support */
#if IPCOM_USE_FILE != IPCOM_FILE_NONE && !defined(IP_PORT_OSE5)
#define IPCOM_SYSLOG_FILE
#endif

/* Define to enable syslog UDP system support */
#if IPCOM_USE_SOCK != IPCOM_SOCK_NONE
#ifdef IP_DEBUG
#define IPCOM_SYSLOG_UDP
#endif
#endif

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_clib.h"
#include "ipcom_clib2.h"
#include "ipcom_time.h"
#include "ipcom_os.h"
#include "ipcom_syslog.h"
#include "ipcom_err.h"
#include "ipcom_sysvar.h"
#include "ipcom_shell.h"
#ifdef IPCOM_SYSLOG_UDP
#include "ipcom_sock.h"
#include "ipcom_inet.h"
#endif

#include <stdarg.h>


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Maximum size of a syslog ident. */
#define IPCOM_SYSLOG_MAX_IDENT      16

#ifdef IPCOM_USE_TIME
#define IPCOM_SYSLOGD_TIMESTRING_LEN  (32 + 2)
#endif


#ifndef IPCOM_SYSLOG_FILE_MAXSIZE
#define IPCOM_SYSLOG_FILE_MAXSIZE 32768
#endif

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    Ipcom_syslog_entry
 *===========================================================================
 */
typedef struct Ipcom_syslog_entry_struct
{
#ifdef IP_ASSERT
    Ip_u32    magic;
#endif
    struct    Ipcom_syslog_entry_struct   *next;
    Ip_u32    priority;
    Ip_u32    missed;
    Ip_pid_t  pid;         /* optional, 0 if not set. */
    int       loglen;

    Ip_u8     logdata[3];  /* 3 bytes space for \n\r and \0. */
}
Ipcom_syslog_entry;


/*
 *===========================================================================
 *                         Ipcom_syslog_facility
 *===========================================================================
 */
typedef struct Ipcom_syslog_facility_struct
{
    Ip_u32   flags;

    Ip_u16   hdr_space;
    Ip_s16   priority;

    char     buf[IPCOM_SYSLOG_MAX_IDENT+1];
}
Ipcom_syslog_facility;


/*
 *===========================================================================
 *                         ipcom_syslogd_data
 *===========================================================================
 */
typedef struct Ipcom_syslogd_data_struct
{
    Ipcom_syslog_facility  *logf[IPCOM_LOG_FACILITY_MAX];
    Ipcom_ipc               ipc;
    int                     q_num;
    int                     q_max;
    Ip_u32                  missed;

#ifdef IPCOM_SYSLOG_FILE
    int                     file_size;
    char                   *file_name;
    IP_FILE                *file_fp;
    int                     file_tries;
    Ip_size_t               file_maxsize;
#endif
#ifdef IPCOM_SYSLOG_UDP
    union Ip_sockaddr_union udp_addr;
    Ip_fd                   udp_fd;
#endif
}
Ipcom_syslogd_data;


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipcom_syslogd_init(void *ununsed);
IP_PUBLIC Ip_err ipcom_syslogd_start(void);

IP_PUBLIC void ipcom_vsyslog(Ip_u32 priority, const char *format, va_list ap);

IP_STATIC IPCOM_PROCESS(ipcom_syslogd);

IP_STATIC Ip_u32 ipcom_checklog(Ip_u32 priority, Ip_u32 *missedp, Ip_u32 *flagsp);

/* ipcom_adjustlog() must be called for each ipcom_checklog() that does not
 * result in ipcom_syslogd_syslog() being called or the size of the syslog
 * queue will be corrupt (too large).
 */
IP_STATIC void ipcom_adjustlog(Ip_u32 missed);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_STATIC Ipcom_syslogd_data   ipcom_logd;

IP_STATIC Ipcom_once_t ipcom_syslogd_once = IPCOM_ONCE_INIT;


/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_checklog
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_u32
ipcom_checklog(Ip_u32 priority, Ip_u32 *missedp, Ip_u32 *flagsp)
{
    Ipcom_syslog_facility   *fcy;
    Ip_u32   facility;
    Ip_u32   msr;
    Ip_u32   hdr_space = 0;

    if (ipcom_once(&ipcom_syslogd_once, ipcom_syslogd_init, IP_NULL) != IPCOM_SUCCESS)
        return 0;

    ip_assert(ipcom_logd.q_max >= 20);

    /* syslogd queue full. */
    if ((ipcom_logd.q_num + ((int)IPCOM_LOG_PRI(priority) * ipcom_logd.q_max / 20)) >= ipcom_logd.q_max)
    {
        msr = ipcom_interrupt_disable();
        ipcom_logd.missed++;
        goto leave;
    }

    /* Invalid facility, ignore. */
    facility = IPCOM_LOG_FAC(priority);
    if (facility >= IPCOM_LOG_FACILITY_MAX)
        return 0;

    msr = ipcom_interrupt_disable();
    if (ipcom_logd.logf[facility] == IP_NULL)
    {
        ipcom_interrupt_enable(msr);
        ipcom_openlog(IP_NULL, 0, facility << 3);
        msr = ipcom_interrupt_disable();
    }
    fcy = ipcom_logd.logf[facility];
    ip_assert(fcy != IP_NULL);

    /* Not logging this priority. */
    if ((Ip_s16)IPCOM_LOG_PRI(priority) > fcy->priority)
        goto leave;

    /* Return the number of missed logs. */
    ipcom_logd.q_num++;
    *missedp = ipcom_logd.missed;
    ipcom_logd.missed = 0;
    *flagsp = fcy->flags;
    hdr_space = (Ip_u32)fcy->hdr_space;

 leave:
    ipcom_interrupt_enable(msr);
    return hdr_space;
}


/*
 *===========================================================================
 *                    ipcom_adjustlog
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_adjustlog(Ip_u32 missed)
{
    Ip_u32 msr;

    msr = ipcom_interrupt_disable();

    ipcom_logd.q_num--;
    ipcom_logd.missed += (missed + 1);

    ipcom_interrupt_enable(msr);
}


/*
 *===========================================================================
 *                    ipcom_syslogd_log
 *===========================================================================
 * Description:   Called by syslogd daemon to log syslog.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_syslogd_log(const char *output, Ip_u32 priority)
{
#if defined(IPCOM_SYSLOG_FILE) || defined(IPCOM_SYSLOG_UDP)
    int   len = ipcom_strlen(output);
#endif

    /*********** Debug printf ***********/
#ifdef IP_DEBUG
    (void)ipcom_debug_printf("%s", output);
#endif

    /*********** File system ***********/
#ifdef IPCOM_SYSLOG_FILE
    if (ipcom_logd.file_name != IP_NULL)
    {
        if (ipcom_logd.file_fp == IP_NULL)
        {
            char  buf[48];

            /* No syslog file open, remove old backup and create new. */
            ipcom_snprintf(buf, sizeof(buf), "%s.0", ipcom_logd.file_name);
            (void)ipcom_unlink(buf);
            (void)ipcom_rename(ipcom_logd.file_name, buf);
            ipcom_logd.file_size = 0;

            /* Create new syslog file. */
        file_retry:
            ipcom_logd.file_fp = ipcom_fopen(ipcom_logd.file_name, "w");
            if (ipcom_logd.file_fp == IP_NULL)
            {
#ifdef IP_DEBUG
                (void)ipcom_debug_printf("IPCOM SYSLOG WARNING: failed to open syslog file '%s'\n",
                                         ipcom_logd.file_name);
#endif /* IP_DEBUG */
                ++ipcom_logd.file_tries;
                if (ipcom_logd.file_tries < 5)
                {
                    ipcom_millisleep(ipcom_logd.file_tries * 100);
                    goto file_retry;
                }
                if (ipcom_logd.file_tries > 20)
                {
                    ipcom_free(ipcom_logd.file_name); /* to avoid excessive spam */
                    ipcom_logd.file_name = IP_NULL;
                }
            }
        }

        if (ipcom_logd.file_fp != IP_NULL)
        {
            /* Write file to disk. Flush output to make sure written. */
            (void)ipcom_fprintf(ipcom_logd.file_fp, "%s", output);
            (void)ipcom_fflush(ipcom_logd.file_fp);

            /* Check max size to not overflow disk. */
            ipcom_logd.file_size += len;
            if (ipcom_logd.file_size > (int)ipcom_logd.file_maxsize)
            {
                /* syslog reached max size, close file to create a backup. */
                (void)ipcom_fclose(ipcom_logd.file_fp);
                ipcom_logd.file_fp = IP_NULL;
            }
        }
    }
#endif /* IPCOM_SYSLOG_FILE */

#ifdef IPCOM_SYSLOG_UDP
    if (ipcom_logd.udp_addr.sa.sa_family != 0
       && IPCOM_LOG_FAC(priority) != IPCOM_LOG_FAC(IPCOM_LOG_IPNET)
       && IPCOM_LOG_FAC(priority) != IPCOM_LOG_FAC(IPCOM_LOG_IPLITE)
       && IPCOM_LOG_FAC(priority) != IPCOM_LOG_FAC(IPCOM_LOG_IPIPSEC)
       && IPCOM_LOG_FAC(priority) != IPCOM_LOG_FAC(IPCOM_LOG_IPFIREWALL)
       && IPCOM_LOG_FAC(priority) != IPCOM_LOG_FAC(IPCOM_LOG_IPPPP))
    {
        if (ipcom_logd.udp_fd == IP_INVALID_SOCKET)
        {
            ipcom_logd.udp_fd = ipcom_socket(ipcom_logd.udp_addr.sa.sa_family, IP_SOCK_DGRAM, 0);
            if (ipcom_logd.udp_fd != IP_INVALID_SOCKET)
            {
                /* Connect to destination. */
                if (ipcom_connect(ipcom_logd.udp_fd, (struct Ip_sockaddr *)&ipcom_logd.udp_addr, IPCOM_SA_LEN_GET(&ipcom_logd.udp_addr.sa)) == IP_SOCKERR)
                {
                    ipcom_socketclose(ipcom_logd.udp_fd);
                    ipcom_logd.udp_fd = IP_INVALID_SOCKET;
                }
            }
        }
        if (ipcom_logd.udp_fd != IP_INVALID_SOCKET)
        {
            (void)ipcom_socketwrite(ipcom_logd.udp_fd, (IP_CONST void *)output, len);
        }
    }
#else
    (void)priority;
#endif /* IPCOM_SYSLOG_UDP */
}


/*
 *===========================================================================
 *                         ipcom_syslogd
 *===========================================================================
 */
IP_STATIC
IPCOM_PROCESS(ipcom_syslogd)
{
    Ip_err ret;
    Ipcom_syslog_entry *se;
    char   buf[128];

    ipcom_proc_init();

    (void)ipcom_ipc_install();

#ifdef IP_PORT_VXWORKS
    /* Wait a while so that the vxWorks logo is printed before we start to output
     * syslog entries. Otherwise the output becomes garbled and virtually unreadable.
     */
    ipcom_millisleep(3000);
#endif

    for (;;)
    {
        int i, len;
        Ip_u32  msr;
        Ip_u8  *logdata;
#ifdef IPCOM_USE_TIME
        char       tbuf[26];
        Ip_time_t  ct;
        char      *tmstr;
#endif
        Ip_u16   fcy_hdr_space;
        char     fcy_buf[IPCOM_SYSLOG_MAX_IDENT+1];

        ret = ipcom_ipc_receive(IP_NULL, &se, -1);
        if (ret != IPCOM_SUCCESS)
            continue;

        ip_assert(se != IP_NULL);
        ip_assert(se->magic == 0x07413370);
        i = 0;

        /* Do checklog if it hasn't already been done by the client */
        if ((int)se->missed < 0)
        {
            Ip_u32 flags;

            /* The 'missed' entry really contains the used header space */
            i = -(int)se->missed;

            /* Do an 11'th hour checklog */
            if (ipcom_checklog(se->priority, &se->missed, &flags) <= 0)
            {
                /* Ignore this log request */
                ipcom_ipc_free(se);
                continue;
            }

            if (!IP_BIT_ISSET(flags, IP_LOG_PID))
            {
                se->pid = 0;
            }
        }

        /* First print number of missed log entries. */
        if (se->missed > 0)
        {
            ipcom_sprintf(buf, "*** lost %ld log entries ***\n", se->missed);
            (void)ipcom_syslogd_log(buf, IPCOM_LOG_SYSLOG | IPCOM_LOG_INFO);
        }

        /* Get facility info. */
        msr = ipcom_interrupt_disable();
        ipcom_logd.q_num--;
        {
            Ipcom_syslog_facility   *fcy;

            fcy = ipcom_logd.logf[se->priority >> 3];
            if (fcy != IP_NULL)
            {
                fcy_hdr_space = fcy->hdr_space;
                ipcom_strcpy(fcy_buf, fcy->buf);
            }
            else
                fcy_hdr_space = 0xffff;
        }
        ipcom_interrupt_enable(msr);

        if (fcy_hdr_space == 0xffff)
        {
            ipcom_ipc_free(se);
            continue;
        }
        if (i == 0)
            i = fcy_hdr_space;
        logdata = &se->logdata[0];

        /* Append linefeed and null-terminate. */
        BOUNDS_CHECKING_OFF;
        logdata[(long)(i + se->loglen)]   = '\n';
        logdata[(long)(i + se->loglen+1)] = '\0';

        /* Prepend priority text string. */
        len = ipcom_strlen(ipcom_syslog_priority_names[se->priority & 0x7]) + 2;
        ipcom_strcpy((char *)logdata + i - len, ipcom_syslog_priority_names[se->priority & 0x7]);
        logdata[i - 1] = ' ';
        logdata[i - 2] = ':';
        i -= len;
        ip_assert(i >= 0);

        /* Prepend ': ' in preparation for facility string & optional hex [pid] */
        logdata[i - 1] = ' ';
        logdata[i - 2] = ':';
        i -= 2;
        ip_assert(i >= 0);

        /* Optionally prepend pid string. */
        if (se->pid != 0)
        {
            len = ipcom_sprintf(buf, "[%lx]", (Ip_u32)se->pid);
            ipcom_memcpy((char *)logdata + i - len, buf, len);
            i -= len;
            ip_assert(i >= 0);
        }

        /* Prepend facility text string. */
        len = ipcom_strlen(fcy_buf); /*lint !e645 Symbol 'fcy_buf' may not have been initialized */
        ipcom_memcpy((char *)logdata + i - len, fcy_buf, len);
        i -= len;
        ip_assert(i >= 0);

        /* Optionally prepend time. */
#ifdef IPCOM_USE_TIME
        ct    = ipcom_time(0);
        tmstr = ipcom_ctime_r(&ct, tbuf);
        len   = ipcom_strlen(tmstr) + 1;  /* only +1 due to internal linefeed. */
        ip_assert((i - len) >= 0);
        ipcom_strcpy((char *)logdata + i - len, tmstr);
        logdata[i - 1] = ' ';
        logdata[i - 2] = ':';
        i -= len;
#endif /* IPCOM_USE_TIME */
        BOUNDS_CHECKING_ON;

        /* Log */
        ipcom_syslogd_log((const char *)logdata + i, se->priority);

        /* Free IPC buffer. */
        ipcom_ipc_free(se);
    } /* for(;;) */

    /* Remember this if we in the future would like to shut down the daemon
     * ipcom_proc_exit();
     */
}


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_openlog
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_openlog(const char *ident, Ip_u32 option, Ip_u32 facility)
{
    Ipcom_syslog_facility   *fcy;
    Ip_u32 msr;

    (void)option;

    if (ipcom_once(&ipcom_syslogd_once, ipcom_syslogd_init, IP_NULL) != IPCOM_SUCCESS)
    {
        IP_PANIC();
        return;
    }

    facility = IPCOM_LOG_FAC(facility);
    if (facility >= IPCOM_LOG_FACILITY_MAX)
        return;

    msr = ipcom_interrupt_disable();
    if (ipcom_logd.logf[facility] != IP_NULL)
        goto leave;
    ipcom_interrupt_enable(msr);

    fcy = ipcom_malloc(sizeof(Ipcom_syslog_facility));
    if (fcy == IP_NULL)
        goto leave;

    /* Initiliaze facility entry. */
    ipcom_memset(fcy, 0, sizeof(Ipcom_syslog_facility));

    fcy->priority  = (Ip_s16)ipcom_sysvar_get_as_int("ipcom.syslogd.default_priority", IPCOM_LOG_INFO);
    fcy->hdr_space = IPCOM_LOG_PRI_MAXLEN + 10; /* 10 = [hex pid] */
#ifdef IPCOM_SYSLOG_FLAGS
    fcy->flags = IPCOM_SYSLOG_FLAGS;
#endif

    if (ident != IP_NULL)
        ipcom_strncpy(fcy->buf, ident, IPCOM_SYSLOG_MAX_IDENT);
    else
        ipcom_strcpy(fcy->buf, ipcom_syslog_facility_names[facility]);

    fcy->hdr_space += (Ip_u16) (ipcom_strlen(fcy->buf) + 2);
#ifdef IPCOM_USE_TIME
    fcy->hdr_space += IPCOM_SYSLOGD_TIMESTRING_LEN;
#endif

    /* Store facility entry. */
    msr = ipcom_interrupt_disable();
    ip_assert(ipcom_logd.logf[facility] == IP_NULL);
    ipcom_logd.logf[facility] = fcy;
 leave:
    ipcom_interrupt_enable(msr);
}


/*
 *===========================================================================
 *                    ipcom_closelog
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_closelog(void)
{
  /* Since we got no facility we can't close the logs...bah! */
}


/*
 *===========================================================================
 *                    ipcom_setlogmask_on
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_setlogmask_on(int maskpri, Ip_u32 facility)
{
    Ipcom_syslog_facility   *fcy;
    int old_priority = -1;
    Ip_u32 msr;

    facility = IPCOM_LOG_FAC(facility);
    if (facility >= IPCOM_LOG_FACILITY_MAX)
        return -1;

    msr = ipcom_interrupt_disable();
#ifdef IP_PORT_OSE5
    if (ipcom_logd.logf[facility] == IP_NULL)
    {
        ipcom_interrupt_enable(msr);
        ipcom_openlog(IP_NULL, 0, facility << 3);
        msr = ipcom_interrupt_disable();
    }
#endif
    fcy = ipcom_logd.logf[facility];

    if (fcy != IP_NULL)
    {
        old_priority  = fcy->priority;
        fcy->priority = (Ip_s16)IPCOM_LOG_PRI(maskpri);
    }

    ipcom_interrupt_enable(msr);

    return old_priority;
}


/*
 *===========================================================================
 *                    ipcom_getlogmask_on
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_getlogmask_on(Ip_u32 facility)
{
    Ipcom_syslog_facility   *fcy;
    int priority = -1;
    Ip_u32 msr;

    facility = IPCOM_LOG_FAC(facility);
    if (facility >= IPCOM_LOG_FACILITY_MAX)
        return -1;

    msr = ipcom_interrupt_disable();
#ifdef IP_PORT_OSE5
    if (ipcom_logd.logf[facility] == IP_NULL)
    {
        ipcom_interrupt_enable(msr);
        ipcom_openlog(IP_NULL, 0, facility << 3);
        msr = ipcom_interrupt_disable();
    }
#endif
    fcy = ipcom_logd.logf[facility];

    if (fcy != IP_NULL)
        priority = fcy->priority;

    ipcom_interrupt_enable(msr);
    return priority;
}


/*
 *===========================================================================
 *                    ipcom_setlogfile
 *===========================================================================
 * Description:  Set logfile.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_setlogfile(const char *file)
{
#ifdef IPCOM_SYSLOG_FILE
    if (ipcom_once(&ipcom_syslogd_once, ipcom_syslogd_init, IP_NULL) != IPCOM_SUCCESS)
    {
        IP_PANIC();
        return 0;
    }

    if (ipcom_logd.file_name != IP_NULL)
    {
        ipcom_free(ipcom_logd.file_name);
        ipcom_logd.file_name = IP_NULL;
    }

    if (ipcom_logd.file_fp != IP_NULL)
    {
        (void)ipcom_fclose(ipcom_logd.file_fp);
        ipcom_logd.file_fp = IP_NULL;
    }

    if (file != IP_NULL)
    {
        ipcom_logd.file_name = ipcom_strdup(file);
        if (ipcom_logd.file_name == IP_NULL)
            return -1;
    }
#else
    (void)file;
#endif
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_setlogudp
 *===========================================================================
 * Description:  Set log IP address & port for UDP syslog output.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_setlogudp(const char *ipaddr, Ip_u16 port)
{
#ifdef IPCOM_SYSLOG_UDP
    if (ipcom_once(&ipcom_syslogd_once, ipcom_syslogd_init, IP_NULL) != IPCOM_SUCCESS)
    {
        IP_PANIC();
        return 0;
    }

    if (ipaddr != IP_NULL && port != 0)
    {
        int ret;

#ifdef IPCOM_USE_INET6
        if (ipcom_strchr(ipaddr, ':'))
        {
            ret = ipcom_getsockaddrbyaddrname(IP_AF_INET6, IP_FALSE /* no DNS */, (IP_CONST char *)ipaddr, &ipcom_logd.udp_addr.sa);
            ipcom_logd.udp_addr.sin6.sin6_port = ip_htons(port);
            if (ret != 0)
                return -1;
        }
#endif
#ifdef IPCOM_USE_INET
        ret = ipcom_getsockaddrbyaddrname(IP_AF_INET, IP_FALSE /* no DNS */, (IP_CONST char *)ipaddr, &ipcom_logd.udp_addr.sa);
        ipcom_logd.udp_addr.sin.sin_port = ip_htons(port);
        if (ret != 0)
            return -1;
#endif
    }
    else
        ipcom_logd.udp_addr.sa.sa_family = 0; /* disabled */

    if (ipcom_logd.udp_fd != IP_INVALID_SOCKET)
    {
        (void)ipcom_socketclose(ipcom_logd.udp_fd);
        ipcom_logd.udp_fd = IP_INVALID_SOCKET;
    }
#else
    (void)ipaddr;
    (void)port;
#endif /* IPCOM_SYSLOG_UDP */

    return 0;
}


/*
 *===========================================================================
 *                    ipcom_vsyslog
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_vsyslog(Ip_u32 priority, const char *format, va_list ap)
{
    Ip_u32  hdr_space;
    Ip_u32  missed, flags;
    Ip_err  retval;

    hdr_space = ipcom_checklog(priority, &missed, &flags);
    if (hdr_space > 0)
    {
        Ipcom_syslog_entry *se;
        char               *logdata;
        int                 loglen;

#ifdef IPCOM_SYSLOG_MEM_OPT
        static char buf[IPCOM_SYSLOG_MAX_SIZE+1];

        loglen = ipcom_vsnprintf(buf, IPCOM_SYSLOG_MAX_SIZE, format, ap);

        /* Some vsnprintf return -1 if buffer is full, i.e. we will
         * not even get the truncated output, bah!
         */
        if (loglen <= 0)
        {
            ipcom_adjustlog(missed);
            return;
        }

        se = ipcom_ipc_malloc(sizeof(Ipcom_syslog_entry) + hdr_space + loglen);
        if (se == IP_NULL)
            return;
        logdata = (char *)&se->logdata[0];
        ipcom_memcpy(logdata + hdr_space, buf, IP_MIN((Ip_size_t) loglen, sizeof(buf)));
#else
        se = ipcom_ipc_malloc(sizeof(Ipcom_syslog_entry) + hdr_space + IPCOM_SYSLOG_MAX_SIZE);
        if (se == IP_NULL)
            return;
        logdata = (char *)&se->logdata[0];

        loglen = ipcom_vsnprintf(logdata + hdr_space, IPCOM_SYSLOG_MAX_SIZE, format, ap);

        /* Some vsnprintf return -1 if buffer is full, i.e. we will
         * not even get the truncated output, bah!
         */
        if (loglen <= 0)
        {
            ipcom_adjustlog(missed);
            ipcom_ipc_free(se);
            return;
        }
#endif /* IPCOM_SYSLOG_MEM_OPT */

        /* C99 standard -> truncated. */
        if (loglen >= IPCOM_SYSLOG_MAX_SIZE)
        {
            loglen = IPCOM_SYSLOG_MAX_SIZE;
            logdata[hdr_space + IPCOM_SYSLOG_MAX_SIZE - 1] = '*';
        }

        IP_ASSERTLINE(se->magic = 0x07413370);
        se->priority = priority;
        se->loglen   = loglen;
        se->missed   = missed;
        if (IP_BIT_ISSET(flags, IP_LOG_PID))
            se->pid = ipcom_getpid();
        else
            se->pid = 0;

        /* Send log entry to syslog daemon. */
        while (ipcom_ipc_isopen(&ipcom_logd.ipc) == IP_FALSE)
        {
            ipcom_millisleep(10);
        }
        retval = ipcom_ipc_send(&ipcom_logd.ipc, se);
        ip_assert(retval == IPCOM_SUCCESS);
        (void)retval;
    }
}


/*
 *===========================================================================
 *                    ipcom_syslog
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_PUBLIC void
ipcom_syslog(Ip_u32 priority, const char *format, ...)
{
    va_list   ap;

    va_start(ap, format);

    ipcom_vsyslog(priority, format, ap);

    va_end(ap);
}


/*
 *===========================================================================
 *                    ipcom_syslogd_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_PUBLIC Ip_err
ipcom_syslogd_init(void *ununsed)
{
    Ipcom_proc_attr  attr;
    Ip_err           retval;

    (void)ununsed;

    ipcom_memset(&ipcom_logd, 0, sizeof(Ipcom_syslogd_data));

    ipcom_logd.q_max = ipcom_sysvar_get_as_int("ipcom.syslogd.queue.max", 128);
    ipcom_logd.q_max = IP_MAX(20, ipcom_logd.q_max);

#ifdef IPCOM_SYSLOG_FILE
    ipcom_logd.file_maxsize = ipcom_sysvar_get_as_int("ipcom.syslogd.file.max", IPCOM_SYSLOG_FILE_MAXSIZE);
    ipcom_logd.file_maxsize = IP_MAX(1024, ipcom_logd.file_maxsize);
#endif

#ifdef IPCOM_SYSLOG_UDP
    ipcom_logd.udp_fd = IP_INVALID_SOCKET;
#endif

    ipcom_proc_attr_init(&attr);
#ifdef IP_DEBUG
    attr.priority = IPCOM_PRIORITY_MAX;
#endif
    retval = ipcom_proc_acreate("ipcom_syslogd", ipcom_syslogd, &attr, IP_NULL);
    if (retval != IPCOM_SUCCESS)
        return retval;

    retval = ipcom_ipc_open(&ipcom_logd.ipc, "ipcom_syslogd", -1);
    ip_assert(retval == IPCOM_SUCCESS);
    return retval;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
