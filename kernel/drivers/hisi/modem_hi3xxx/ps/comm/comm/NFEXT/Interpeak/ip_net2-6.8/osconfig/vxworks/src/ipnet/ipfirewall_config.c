/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipfirewall_config.c,v $ $Revision: 1.2 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/ipfirewall_config.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *     Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */

/*
modification history
--------------------
01e,03jan07,kch  Updated CVS revision log entry for Jan 03 code drop.
01d,01dec06,kch  Updated CVS revision log entry.
01c,13nov06,kch  Removed IPCOM_USE_CLIB_PROTO define.
01b,13sep06,kch  Refresh with Sept 08, 2006 code drop
01a,28aug06,kch  Update based on Aug 22, 2006 code drop.
*/

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Firewall configuration.
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
#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
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

IP_EXTERN Ip_s32 ipfirewall_add_rule_file(Ip_bool mac_rule, Ip_s32 family,
                                          char *filename, Ip_bool silent);

#ifdef INCLUDE_IPFIREWALL_CMD
IP_EXTERN int ipfirewall_cmd_ipfirewall(int argc, char **argv);
#endif

/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipfirewall_configure(void);


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
IP_STATIC IP_CONST Ipcom_sysvar ipfirewall_conf_sysvar[] =
{
    /* The timeout until an icmp stateful mapping expires */
    { "ipfirewall.timeout.icmp", IPF_ICMP_TIMEOUT },

    /* The timeout until an udp stateful mapping expires */
    { "ipfirewall.timeout.udp", IPF_UDP_TIMEOUT },

    /* The timeout until a tcp stateful mapping expires */
    { "ipfirewall.timeout.tcp", IPF_TCP_TIMEOUT},

    /* The timeout until other stateful mappings expires */
    { "ipfirewall.timeout.other", IPF_OTHER_TIMEOUT},

    /* The maximum number of stateful mappings */
    { "ipfirewall.capacity.states", IPF_MAX_STATEFUL_MAPPINGS },

#if IPCOM_USE_FILE != IPCOM_FILE_NONE
#ifdef INCLUDE_IPCOM_USE_INET
    /* IP filter rule specification file for IPv4 */
    { "ipfirewall.rules.configv4", IPF_IPV4_RULE_FILE},
#endif /* INCLUDE_IPCOM_USE_INET */
    
#ifdef INCLUDE_IPCOM_USE_INET6
    /* IP filter rule specification file for IPv6 */
    { "ipfirewall.rules.configv6",  IPF_IPV6_RULE_FILE},
#endif /* INCLUDE_IPCOM_USE_INET6 */

    /* MAC filter rule specification file */
    { "ipfirewall.rules.configmac", IPF_FWMAC_RULE_FILE},
#endif

    { IP_NULL, IP_NULL }
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
 *                    ipfirewall_configure
 *===========================================================================
 * Description: Firewall configure routine.
 * Parameters:  none.
 * Returns:     IPCOM_SUCCESS = ok
 *              IPCOM_ERR_XXX = error code.
 */
IP_PUBLIC Ip_err
ipfirewall_configure(void)
{
#ifdef INCLUDE_IPFIREWALL_CMD
    Ip_err ret = IPCOM_SUCCESS;

    ret = ipcom_shell_add_cmd("ipf", "ipf [ options ] <modifier...>",
                              "Firewall", ipfirewall_cmd_ipfirewall,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_SMALL);
    if(ret != IPCOM_SUCCESS)
        return ret;
#endif /* INCLUDE_IPFIREWALL_CMD */

    return ipcom_sysvar_setv(ipfirewall_conf_sysvar, 0);
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
