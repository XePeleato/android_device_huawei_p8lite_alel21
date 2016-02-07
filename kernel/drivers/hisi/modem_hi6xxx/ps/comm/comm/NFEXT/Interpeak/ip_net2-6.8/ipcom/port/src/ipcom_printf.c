/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_printf.c,v $ $Revision: 1.23 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_printf.c,v $
 *   $Author: kenneth $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by FirstName LastName <email@interpeak.se>
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
#include "ipcom_clib2.h"
#include "ipcom_file.h"
#include "ipcom_os.h"
#include "ipcom_shell.h"


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
 *                    ipcom_printf
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_PRINTF) && IPCOM_PRINTF == 1
IP_PUBLIC int
ipcom_printf(const char *format, /* args*/ ...)
{
#if IPCOM_USE_SHELL == IPCOM_SHELL_NONE
    (void)format;
    return -1;
#else
    va_list ap;
    char   *buf;
    int     retval;

    buf = ipcom_malloc(IPCOM_STDIO_BUFSIZE + 1);
    if (buf == IP_NULL)
        return -1;

    va_start(ap, format);
    retval = ipcom_vsnprintf(buf, IPCOM_STDIO_BUFSIZE, format, ap);
    retval = IP_MIN(retval, (int) IPCOM_STDIO_BUFSIZE);
    va_end(ap);

    if (retval > 0)
    {
#if IPCOM_USE_SHELL != IPCOM_SHELL_NONE
        retval = ipcom_shell_fwrite(buf, retval, 1, ip_stdout);
#else
        retval = ipcom_fwrite(buf, retval, 1, ip_stdout);
#endif
    }
    else
        retval = -1;

    ipcom_free(buf);
    return retval;
#endif /* IPCOM_USE_SHELL == IPCOM_SHELL_NONE */
}
#endif


/*
 *===========================================================================
 *                    ipcom_fprintf
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_FPRINTF) && IPCOM_FPRINTF == 1
IP_PUBLIC int
ipcom_fprintf(IP_FILE *stream, const char *format, /* args*/ ...)
{
#if IPCOM_USE_SHELL == IPCOM_SHELL_NONE && IPCOM_USE_FILE == IPCOM_FILE_NONE
    (void)stream;
    (void)format;
    return -1;
#else
    va_list   ap;
    char  *buf;
    int retval;

#ifndef IP_PORT_UNIX
    if (stream == IP_NULL)
        return -1;
#endif /* IP_PORT_UNIX */

    buf = ipcom_malloc(IPCOM_STDIO_BUFSIZE + 1);
    if (buf == IP_NULL)
    {
        ipcom_errno = IP_ERRNO_ENOMEM;
        return -1;
    }

    va_start(ap, format);

    retval = ipcom_vsnprintf(buf, IPCOM_STDIO_BUFSIZE, format, ap);

    if (retval > 0)
    {
        retval = IP_MIN(retval, (int) IPCOM_STDIO_BUFSIZE);
#if IPCOM_USE_SHELL != IPCOM_SHELL_NONE
        if (stream == ip_stdout || stream == ip_stderr)
            retval = ipcom_shell_fwrite(buf, retval, 1, stream);
        else
#endif
            retval = ipcom_fwrite(buf, retval, 1, stream);

    }
    else
        retval = -1;

    ipcom_free(buf);
    va_end(ap);

    return retval;
#endif /* IPCOM_USE_SHELL == IPCOM_SHELL_NONE && IPCOM_USE_FILE == IPCOM_FILE_NONE */
}
#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

