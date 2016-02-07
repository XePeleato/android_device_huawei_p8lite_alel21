/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_shellfile.c,v $ $Revision: 1.2 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_shellfile.c,v $
 *     $Author: lob $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Markus Carlstedt <markus@interpeak.se>
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

#if IPCOM_USE_FILE == IPCOM_FILE_NONE \
&& (IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM || IPCOM_USE_SHELL == IPCOM_SHELL_NONE)

/* Ipcom include files */
#define IPCOM_USE_CLIB_PROTO
#include "ipcom_clib.h"
#include "ipcom_shell.h"
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
 *                    ipcom_fflush
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_fflush(IP_FILE *file)
{
    (void)file;
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_fwrite
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_size_t
ipcom_fwrite(const void *ptr, Ip_size_t size, Ip_size_t nitems, IP_FILE *stream)
{
#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
    if (stream == ip_stdout || stream == ip_stderr)
        return ipcom_shell_fwrite(ptr, size, nitems, stream);
    if (stream == IP_NULL)
        return 0;
#endif
    ipcom_errno = IP_ERRNO_EBADF;
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_fread
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_size_t
ipcom_fread(void *ptr, Ip_size_t size, Ip_size_t nitems, IP_FILE *stream)
{
#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
    if (stream == ip_stdin)
        return ipcom_shell_fread(ptr, size, nitems, stream);
#endif
    ipcom_errno = IP_ERRNO_EBADF;
    return 0;
}

#else
int ipcom_shellfile_empty_file;
#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
