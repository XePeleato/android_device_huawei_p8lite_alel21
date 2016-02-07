/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_snprintf.c,v $ $Revision: 1.8 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_snprintf.c,v $
 *   $Author: lob $
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

#include <stdarg.h>


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

#ifdef IP_PORT_LKM
IP_EXTERN int ipcom_vsprintf(char *s, const char *format, va_list ap);
#endif

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
 *                    ipcom_vsnprintf
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if (defined(IPCOM_VSNPRINTF) && IPCOM_VSNPRINTF == 1) || (defined(IPCOM_SNPRINTF) && IPCOM_SNPRINTF == 1)
IP_PUBLIC int
ipcom_vsnprintf(char *s, Ip_size_t n, const char *format, va_list ap)
{
    int retval;
    char  *buf;

    buf = ipcom_malloc(IPCOM_STDIO_BUFSIZE + 1);
    if(buf == IP_NULL)
        return -1;

    /* WARNING: This is NOT a save operation, it may overflow the buffer! */
    retval = ipcom_vsprintf(buf, format, ap);

    ip_assert(retval < (int) IPCOM_STDIO_BUFSIZE);
    if(retval > 0 && retval < (int) n)
    {
        ipcom_memcpy(s, buf, retval);
        s[retval] = '\0';
    }

    ipcom_free(buf);
    return retval;
}
#endif


/*
 *===========================================================================
 *                    ipcom_snprintf
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_SNPRINTF) && IPCOM_SNPRINTF == 1
IP_PUBLIC int
ipcom_snprintf(char *s, Ip_size_t n, const char  *format, /*args*/ ...)
{
    va_list   ap;
    int retval;

    va_start(ap, format);
    retval = ipcom_vsnprintf(s, n, format, ap);
    va_end(ap);

    return retval;
}
#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

