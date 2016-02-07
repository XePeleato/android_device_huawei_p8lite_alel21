/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_termios.c,v $ $Revision: 1.6 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_termios.c,v $
 *   $Author: lob $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
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

#if IPCOM_USE_FILE != IPCOM_FILE_IPCOM && \
 (defined(IP_PORT_OSE) || defined(IP_PORT_OSE5) || defined(IP_PORT_UNIX))
#ifndef IPCOM_USE_TERMIOS
#define IPCOM_USE_TERMIOS
#endif
#endif


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef IPCOM_USE_TERMIOS
#include <termios.h>
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

#if IPCOM_USE_FILE == IPCOM_FILE_IPCOM
IP_PUBLIC int ipcom_tcgetattr(Ip_fd fd, struct Ip_termios *termios_p);
IP_PUBLIC int ipcom_tcsetattr(Ip_fd fd, int optional_actions, const struct Ip_termios *termios_p);
#endif


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
 *                    ipcom_tcgetattr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_tcgetattr(Ip_fd fd, struct Ip_termios *termios_p)
{
#ifdef IPCOM_USE_TERMIOS
    return tcgetattr((int)fd, (struct termios *)termios_p);
#else
    (void)fd;
    ipcom_memset(termios_p, 0, sizeof(struct Ip_termios));
    return 0;
#endif
}


/*
 *===========================================================================
 *                    ipcom_tcsetattr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_tcsetattr(Ip_fd fd, int optional_actions, const struct Ip_termios *termios_p)
{
#ifdef IPCOM_USE_TERMIOS
    return tcsetattr((int)fd, optional_actions, (const struct termios *)termios_p);
#else
    (void)fd;
    (void)optional_actions;
    (void)termios_p;
    return 0;
#endif
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

