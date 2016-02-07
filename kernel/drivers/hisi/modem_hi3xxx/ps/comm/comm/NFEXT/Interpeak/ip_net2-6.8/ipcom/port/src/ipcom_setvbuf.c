/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_setvbuf.c,v $ $Revision: 1.9 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_setvbuf.c,v $
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
 * setvbuf implementation (required by IPSSL crypto/rand/rand_win.c).
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

#if IPCOM_USE_FILE != IPCOM_FILE_NONE

#if IPCOM_USE_FILE == IPCOM_FILE_NATIVE && \
 (defined(IP_PORT_OSE) || defined(IP_PORT_OSE5) || defined(IP_PORT_VXWORKS) || defined(IP_PORT_UNIX))
#include "stdio.h"
#else
#define IPCOM_USE_CLIB_PROTO
#endif

#include "ipcom_clib.h"
#include "ipcom_file.h"


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
 *                    ipcom_setvbuf
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_setvbuf(IP_FILE *file, char *buffer, int mode, Ip_size_t bufsize)
{
#if IPCOM_USE_FILE == IPCOM_FILE_NATIVE && (defined(IP_PORT_OSE) \
                                            || defined(IP_PORT_VXWORKS) \
                                            || defined(IP_PORT_UNIX))
#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
    if(file == ip_stdin || file == ip_stdout || file == ip_stderr)
    {
        (void)file;
        (void)buffer;
        (void)mode;
        (void)bufsize;
        return 0;
    }
    else
        return setvbuf((FILE *)file, buffer, mode, (size_t)bufsize);
#else
    return setvbuf((FILE *)file, buffer, mode, (size_t)bufsize);
#endif
#else
    (void)file;
    (void)buffer;
    (void)mode;
    (void)bufsize;
    return 0;
#endif
}

#else
int ipcom_setvbuf_empty_file;
#endif /* IPCOM_USE_FILE != IPCOM_FILE_NONE */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

