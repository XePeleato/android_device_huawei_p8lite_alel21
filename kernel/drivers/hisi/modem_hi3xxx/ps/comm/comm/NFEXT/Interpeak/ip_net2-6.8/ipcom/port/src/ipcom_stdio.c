/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_stdio.c,v $ $Revision: 1.15 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_stdio.c,v $
 *   $Author: lob $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
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

#if IPCOM_USE_SHELL == IPCOM_SHELL_NATIVE || defined(IP_PORT_VXWORKS)
#include <stdio.h>
#else
#define IPCOM_USE_CLIB_PROTO
#endif

#include "ipcom_clib.h"
#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
#include "ipcom_os.h"
#include "ipcom_sock.h"
#endif

#if IPCOM_USE_SHELL == IPCOM_SHELL_MINI
#include "ipcom_shell.h"
#endif


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
 *===========================================================================
 *                    ipcom_shell_stdio
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if !defined(IP_PORT_VXWORKS) && IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
IP_STATIC IP_FILE *
ipcom_shell_stdio(int fd)
{
    Ipcom_proc  *proc;

    proc = ipcom_proc_self();
    if(proc && (proc->shell_fd != IP_INVALID_SOCKET))
        return (IP_FILE *)(Ip_ptrdiff_t)(0x0000fff0 | fd);
    else
        return IP_NULL;
}
#endif


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
 *                    ipcom_stdout
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC IP_FILE *
ipcom_stdout(void)
{
#ifdef IP_PORT_VXWORKS
    return (IP_FILE *)stdout;
#elif IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
    return ipcom_shell_stdio(1);
#elif IPCOM_USE_SHELL == IPCOM_SHELL_MINI
    return IP_FILE_STDOUT;
#elif IPCOM_USE_SHELL == IPCOM_SHELL_NONE
    return (IP_FILE *)IP_NULL;
#else
    return (IP_FILE *)stdout;
#endif
}


/*
 *===========================================================================
 *                    ipcom_stderr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC IP_FILE *
ipcom_stderr(void)
{
#ifdef IP_PORT_VXWORKS
    return (IP_FILE *)stderr;
#elif IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
    return ipcom_shell_stdio(2);
#elif IPCOM_USE_SHELL == IPCOM_SHELL_MINI
    return IP_FILE_STDERR;
#elif IPCOM_USE_SHELL == IPCOM_SHELL_NONE
    return (IP_FILE *)IP_NULL;
#else
    return (IP_FILE *)stderr;
#endif
}


/*
 *===========================================================================
 *                    ipcom_stdin
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC IP_FILE *
ipcom_stdin(void)
{
#ifdef IP_PORT_VXWORKS
    return (IP_FILE *)stdin;
#elif IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
    return ipcom_shell_stdio(0);
#elif IPCOM_USE_SHELL == IPCOM_SHELL_MINI
    return IP_FILE_STDIN;
#elif IPCOM_USE_SHELL == IPCOM_SHELL_NONE
    return (IP_FILE *)IP_NULL;
#else
    return (IP_FILE *)stdin;
#endif
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

