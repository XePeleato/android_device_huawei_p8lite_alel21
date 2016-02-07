/*
 ******************************************************************************
 *                     IPCOM CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_telnets_config.c,v $ $Revision: 1.3 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/ipcom_telnets_config.c,v $
 *   $Author: rboden $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */

/*
modification history
--------------------
01g,03jan07,kch  Updated CVS revision log entry for Jan 03 code drop.
01f,21nov06,kch  Refresh with Nov. 15 code drop - added telnet authentication
                 sysvar.
01e,13nov06,kch  Removed IPCOM_USE_CLIB_PROTO define.
01d,13sep06,kch  Refresh with Sept 08, 2006 code drop
01c,28aug06,kch  Update based on Aug 22, 2006 code drop.
01b,11aug06,tkf  Avoid accessing IPCOM_TELNET_PORT if INCLUDE_IPTELNETS is not
                 defined.
01a,30jul06,kch  Ported to be used with VxWorks Image Project.
*/

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 2                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef IPCOM_USE_CLIB_PROTO
#undef IPCOM_USE_CLIB_PROTO
#endif

#include "ipcom_config.h"
#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_err.h"
#include "ipcom_sysvar.h"


/*
 ****************************************************************************
 * 3                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err iptelnets_configure(void);


/*
 ****************************************************************************
 * 5                    DATA
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         SYSTEM VARIABLES
 *===========================================================================
 */
IP_STATIC IP_CONST Ipcom_sysvar iptelnets_conf_sysvar[] =
{
    /* Telnet port */
#ifdef INCLUDE_IPTELNETS
    { "ipcom.telnet.port", IPCOM_TELNET_PORT },
#endif

#ifdef INCLUDE_USE_IPCOM_SHELL
    /* Shell greeting. */
    { "ipcom.shell.greeting", "Copyright (c) Wind River Systems, Inc. 2000-2007. All rights reserved.\n\nWelcome to IPCOM SHELL. Type help for a list of commands.\n\n" },

    /* Shell prompt. Supported special characters are:
     *     \p - shell process name
     *     \P - shell process id
     *     \i - the IP address of the target
     *     \w - the current working directory
     *     \W - the basename of the current working directory
     *     \V - ipcom_getenv("VR")
     */
    /*{ "ipcom.shell.prompt", "\\i$\\w> " },*/
    { "ipcom.shell.prompt", IPCOM_SHELL_PROMPT },
#endif /* INCLUDE_USE_IPCOM_SHELL */

#if defined(INCLUDE_IPCOM_USE_AUTH) && defined(INCLUDE_IPTELNETS)
    /* Require telnet users to authenticate (1 = enabled, 0 = disabled) */
    { "ipcom.telnet.authenticate", IPCOM_TELNET_AUTH_ENABLED },
#endif /* INCLUDE_IPCOM_USE_AUTH && INCLUDE_IPTELNETS */

#ifdef IPCOM_TELNET_MAX_CLIENTS
    { "ipcom.telnet.max_sessions", IPCOM_TELNET_MAX_CLIENTS },
#endif
    { IP_NULL, IP_NULL }
};


/*
 ****************************************************************************
 * 6                   FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                         iptelnets_configure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
iptelnets_configure(void)
{
    /* Set IPTELNETS system variables. */
    (void)ipcom_sysvar_setv(iptelnets_conf_sysvar, 0);

    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

