/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipmcp_config.c,v $ $Revision: 1.2 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/ipmcp_config.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */

/*
modification history
--------------------
01f,03jan07,kch  Updated CVS revision log entry for Jan 03 code drop.
01e,01dec06,kch  Updated CVS revision log entry.
01d,13sep06,kch  Refresh with Sept 08, 2006 code drop
01c,28aug06,kch  Update based on Aug 22, 2006 code drop.
01b,12aug06,kch  Fixed warning.
01a,03aug06,kch  Renamed INCLUDE_MCAST_PROXY_CMD to INCLUDE_IPMCAST_PROXY_CMD.
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

#include <ipcom_err.h>
#include <ipcom_shell.h>
#include <ipcom_sysvar.h>

#if 0
#include "ipmcp.h"
#include "ipmcp_config.h"
#endif

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
#ifdef INCLUDE_IPMCAST_PROXY_CMD
IP_EXTERN int ipmcp_cmd(int argc, char **argv);
#endif

/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 7                    DATA
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         SYSTEM VARIABLES
 *===========================================================================
 */
IP_STATIC IP_CONST Ipcom_sysvar ipmcp_conf_sysvar[] =
{
    /* Upstream interface, this is the interface where all multicast packets
       arrive at. The proxy is acting as a multicast HOST on this interface */
    { "ipmcp.UpstreamIf", MCP_UPSTREAM_IFNAME},

    /* Downstream interface, this is the collection of interface to which
       multicast packets arriving to the upstream inteface will be forwarded
       to if that multicast group has at least one listener. */
    { "ipmcp.DownstreamIfs", MCP_DOWNSTREAM_IFNAMES},

    /* Multicast router robustness variable */
    { "ipmcp.RobustnessVariable", MCP_ROBUSTNESS_VAR},

    /* Multicast router query interval (seconds) */
    { "ipmcp.QueryInterval", MCP_QUERY_INTERVAL},

    /* Multicast router query response interval (milliseconds) */
    { "ipmcp.QueryResponseInterval", MCP_QUERY_RESP_INTERVAL},

    /* Multicast last listener query interval (milliseconds) */
    { "ipmcp.LastListenerQueryInterval", MCP_LAST_LISTERNER_QUERY_INTERVAL},

    /* End marker */
    { IP_NULL, IP_NULL  }
};


/*
 ****************************************************************************
 * 8                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipmcp_configure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipmcp_configure(void)
{
#ifdef INCLUDE_IPMCAST_PROXY_CMD
    Ip_err ret = IPCOM_SUCCESS;

    ret = ipcom_shell_add_cmd("mcastproxy", "mcastproxy [-4|-6] [-r|-i]",
                              "Interpeak multicast proxy", ipmcp_cmd,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_DEFAULT);
    if(ret != IPCOM_SUCCESS)
        return ret;
#endif /* INCLUDE_IPMCAST_PROXY_CMD */

    return ipcom_sysvar_setv(ipmcp_conf_sysvar, 0);
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

