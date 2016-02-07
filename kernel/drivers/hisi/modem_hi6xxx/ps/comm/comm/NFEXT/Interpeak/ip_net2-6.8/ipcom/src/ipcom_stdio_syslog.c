/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_stdio_syslog.c,v $ $Revision: 1.10 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_stdio_syslog.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Markus Carlstedt <markus@interpeak.se>
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


#if IPCOM_USE_FILE != IPCOM_FILE_NONE

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_clib.h"
#include "ipcom_clib2.h"
#include "ipcom_syslog.h"
#include "ipcom_os.h"
#include "ipcom_file.h"
#include "ipcom_stdio_syslog.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

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
 *                    ipcom_stdio_format
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_stdio_format(const char *format, va_list ap, char **buf)
{
    int retval;

    *buf = ipcom_malloc(IPCOM_STDIO_BUFSIZE + 1);
    if(*buf == IP_NULL)
        return -1;
    (*buf)[IPCOM_STDIO_BUFSIZE] = '\0';

    retval = ipcom_vsnprintf(*buf, IPCOM_STDIO_BUFSIZE, format, ap);
    retval = IP_MIN(retval, (int) IPCOM_STDIO_BUFSIZE);
    return retval;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_printf_syslog
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_printf_syslog(const char *format, /* args*/ ...)
{
    va_list ap;
    char *buf = IP_NULL;
    int retval, len;

    va_start(ap, format);
    retval = ipcom_stdio_format(format, ap, &buf);
    va_end(ap);
    if(buf == IP_NULL)
        return -1;

    if(retval > 0)
    {
        len = ipcom_strlen(buf);
        if(len && buf[len-1] == '\n')
            buf[len-1] = '\0';          /* syslog will add a LF */
        IPCOM_LOG1(INFO, "%s", buf);
    }
    ipcom_free(buf);
    return retval;
}


/*
 *===========================================================================
 *                    ipcom_fprintf_syslog
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_fprintf_syslog(IP_FILE *stream, const char *format, /* args*/ ...)
{
    va_list ap;
    char *buf = IP_NULL;
    int retval, len;

    va_start(ap, format);
    retval = ipcom_stdio_format(format, ap, &buf);
    va_end(ap);
    if(buf == IP_NULL)
        return -1;

    if(retval > 0)
    {
        if(stream == ip_stdout || stream == ip_stderr)
        {
            len = ipcom_strlen(buf);
            if(len && buf[len-1] == '\n')
                buf[len-1] = '\0';              /* syslog will add a LF */
            IPCOM_LOG1(INFO, "%s", buf);        /* stdout or stderr */
        }
        else
            retval = ipcom_fputs(buf, stream);  /* file */
    }

    ipcom_free(buf);
    return retval;
}


/*
 *===========================================================================
 *                    ipcom_fputs_syslog
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_fputs_syslog(const char *str, IP_FILE *stream)
{
    int retval, len;

    if(stream == ip_stdout || stream == ip_stderr)
    {
        len = ipcom_strlen(str);
        if(len && str[len-1] == '\n')
            ((char *)str)[len-1] = '\0';        /* syslog will add a LF */
        IPCOM_LOG1(INFO, "%s", str);            /* stdout or stderr */
        retval = ipcom_strlen(str);
    }
    else
        retval = ipcom_fputs(str, stream);      /* file */

    return retval;
}


/*
 *===========================================================================
 *                    ipcom_perror_syslog
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_perror_syslog(const char *s)
{
    int len;

    (void)s;
    len = ipcom_strlen(s);
    if(s[len-1] == '\n')
        ((char *)s)[len-1] = '\0';      /* syslog will add a LF */
    IPCOM_LOG0(INFO, s);
}


/*
 *===========================================================================
 *                    ipcom_fputc_syslog
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_fputc_syslog(int c, IP_FILE *stream)
{
    int retval;

    if(stream == ip_stdout || stream == ip_stderr)
    {
        if(c == '\n')
            c = '\0';                           /* syslog will add a LF */
        IPCOM_LOG1(INFO, "%c", c);              /* stdout or stderr */
        retval = c;
    }
    else
        retval = ipcom_fputc(c, stream);        /* file */

    return retval;
}


/*
 *===========================================================================
 *                    ipcom_putc_syslog
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_putc_syslog(int c, IP_FILE *stream)
{
    return ipcom_fputc_syslog(c, stream);
}


/*
 *===========================================================================
 *                    ipcom_putchar_syslog
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_putchar_syslog(int c)
{
    return ipcom_fputc_syslog(c, ip_stdout);
}


/*
 *===========================================================================
 *                    ipcom_fwrite_syslog
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_size_t
ipcom_fwrite_syslog(const void *ptr, Ip_size_t size, Ip_size_t nitems, IP_FILE *stream)
{
    if(stream == ip_stdout || stream == ip_stderr)
    {
        char *buf;
        int retval = 0;

        buf = ipcom_malloc(size * nitems + 1);
        if(buf != IP_NULL)
        {
            ipcom_memcpy(buf, ptr, size * nitems);
            buf[size * nitems] = '\0';
            retval = ipcom_fputs_syslog(buf, stream);
            ipcom_free(buf);
        }
        return (Ip_size_t)retval; /*lint !e571 */
    }
    else
        return ipcom_fwrite(ptr, size, nitems, stream);
}


/*
 *===========================================================================
 *                    ipcom_fflush_syslog
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_fflush_syslog(IP_FILE *file)
{
    if(file == ip_stdout || file == ip_stderr)
        return 0;
    else
        return ipcom_fflush(file);
}


/*
 *===========================================================================
 *                    ipcom_feof_syslog
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_feof_syslog(IP_FILE *stream)
{
    if(stream == ip_stdout || stream == ip_stderr)
        return IP_EOF;
    else
        return ipcom_feof(stream);
}


/*
 *===========================================================================
 *                    ipcom_ferror_syslog
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_ferror_syslog(IP_FILE *stream)
{
    if(stream == ip_stdout || stream == ip_stderr)
        return 0;
    else
        return ipcom_ferror(stream);
}


/*
 *===========================================================================
 *                    ipcom_exit_syslog
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_exit_syslog(int status)
{
    (void)status;

    IPCOM_LOG1(ERR, "exit(%d) was called!", status);

    IP_PANIC();
    for(;;)
    {
        (void)ipcom_sleep(1);
    }
}

#else
int ipcom_stdio_syslog_empty_file;
#endif /* IPCOM_USE_FILE != IPCOM_FILE_NONE */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
