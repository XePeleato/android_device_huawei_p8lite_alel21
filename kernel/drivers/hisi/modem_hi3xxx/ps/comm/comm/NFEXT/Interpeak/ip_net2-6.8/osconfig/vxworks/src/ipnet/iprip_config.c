/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: iprip_config.c,v $ $Revision: 1.2 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/iprip_config.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */

/*
modification history
--------------------
01e,03jan07,kch  Updated CVS revision log entry for Jan 03 code drop.
01d,01dec06,kch  Updated CVS revision log entry.
01c,13sep06,kch  Refresh with Sept 08, 2006 code drop
01b,28aug06,kch  Update based on Aug 22, 2006 code drop.
01a,22jul06,kch  Modified to use configuration macros.
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

#include "ipcom_cstyle.h"
#include "ipcom_err.h"
#include "ipcom_sysvar.h"
#include <ipcom_shell.h>


/*
 ****************************************************************************
 * 3                    DEFINES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 4                    TYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 5                    EXTERN PROTOTYPES
 ****************************************************************************
 */

#ifdef INCLUDE_IPRIP_CTRL_CMD
IP_PUBLIC int iprip_cmd_ripctrl(int argc, char **argv);
#endif /* INCLUDE_IPRIP_CTRL_CMD */

/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err iprip_configure(void);


/*
 ****************************************************************************
 * 7                    DATA
 ****************************************************************************
 */


/*
 *===========================================================================
 *                         SYSVAR CONFIGURATION
 *===========================================================================
 */

IP_STATIC IP_CONST Ipcom_sysvar iprip_conf_sysvar[] =
{
    /* RIP regular update timeout in seconds (minium interval). */
    { "iprip.update.seconds", IPRIP_UPDATE_INTERVAL },

    /* Variation to RIP updates in seconds (20-40 sec) */
    { "iprip.update.deltaseconds", IPRIP_UPDATE_DELTA },

    /* Seconds til a RIP route expires. Default value accordingly to RFC 2453. */
    { "iprip.expire.seconds", IPRIP_EXPIRE_INTERVAL },

    /* Seconds til a garbage RIP route is removed. Default value accordingly to RFC 2453. */
    { "iprip.garbage.seconds", IPRIP_GARBAGE_INTERVAL },

    /* RIP flash delay in seconds. */
    { "iprip.flash.seconds", IPRIP_FLASH_DELAY },

    /* Set to 1 if you require RIP requests to be authenticated as well. */
    { "iprip.auth.requests", IPRIP_AUTH_ENABLED },

    { IP_NULL, IP_NULL }
};


/*
 *===========================================================================
 *                         INTERFACES
 *===========================================================================
 * Description: Interface configuration.
 * Syntax: "[all | ifname] [options] ...
 * Options:
 *   broadcast        - Use subnet broadcast output.
 *   multicast        - Use multicast output (224.0.0.9).
 *   input-multicast  - Accept multicast input (224.0.0.9).
 *   silent           - Do not output RIP requests/responses.
 *   auth-md5=<pw>    - Enable GateD style md5 authentication with <pw>
 *   auth-simple=<pw] - Enable simple authentication with <pw>.
 *   input=<no|v1|v2> - Change input mode (no,v1 or v2 only).
 *   metric=<num>     - Change default metric from 1 to <num>.
 *   version-1        - Enable RIPv1 (default is RIPv2)
 *   nopoison         - Use simple Split Horizon instead of poisonous.
 *
 * Note: Only use one of broadcast, multicast or silent options.
 */
IP_CONST char *iprip_interface_config[] =
{
#ifdef INCLUDE_IPRIP_IFCONFIG_1    
    IPRIP_IFCONFIG_1,
#endif /* INCLUDE_IPRIP_IFCONFIG_1 */

#ifdef INCLUDE_IPRIP_IFCONFIG_2    
    IPRIP_IFCONFIG_2,
#endif /* INCLUDE_IPRIP_IFCONFIG_2 */

#ifdef INCLUDE_IPRIP_IFCONFIG_3    
    IPRIP_IFCONFIG_3,
#endif /* INCLUDE_IPRIP_IFCONFIG_3 */

    IP_NULL
};


/*
 *===========================================================================
 *                         STATIC RIP ROUTES
 *===========================================================================
 * Description: Static RIP routes.
 * Syntax: "dst=a.b.c.d mask=a.b.c.d gw=a.b.c.d"
 * Options:
 *   metric=<num>     - Change default metric from 1 to <num>.
 */
IP_CONST char *iprip_staticroute_config[] =
{
#ifdef INCLUDE_IPRIP_STATIC_ROUTE_1    
    IPRIP_STATIC_ROUTE_1,
#endif /* INCLUDE_IPRIP_STATIC_ROUTE_1 */

#ifdef INCLUDE_IPRIP_STATIC_ROUTE_2
    IPRIP_STATIC_ROUTE_2,
#endif /* INCLUDE_IPRIP_STATIC_ROUTE_2 */

#ifdef INCLUDE_IPRIP_STATIC_ROUTE_3
    IPRIP_STATIC_ROUTE_3,
#endif /* INCLUDE_IPRIP_STATIC_ROUTE_3 */

    IP_NULL
};


/*
 *===========================================================================
 *                         AUTHENTICATION
 *===========================================================================
 */


/*
 *===========================================================================
 *                    iprip_configure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
iprip_configure(void)
{
    /* Initialize IPRIP system variables. */
    (void)ipcom_sysvar_setv(iprip_conf_sysvar, 0);

#ifdef INCLUDE_IPRIP_CTRL_CMD
    (void)ipcom_shell_add_cmd("ripctrl", "ripctrl <command> [-options] [args]",
                              "RIP daemon control", iprip_cmd_ripctrl,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_DEFAULT);
#endif /* INCLUDE_IPRIP_CTRL_CMD */

    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

