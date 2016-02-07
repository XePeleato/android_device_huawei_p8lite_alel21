/*
 ******************************************************************************
 *                     IPCOM CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipdhcpr_config.c,v $ $Revision: 1.2.60.1 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/ipdhcpr_config.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *   Design and implementation by Jonas Rendel <jonas@interpeak.se>
 ******************************************************************************
 */

/*
modification history
--------------------
01f,04sep09,h_s  Fix WIND00184353.
01e,03jan07,kch  Updated CVS revision log entry for Jan 03 code drop.
01d,14dec06,kch  Modified ipdhcpr_start_hook() to allow for user configurable
                 callback hook. Also added DHCPR_NETCONF_SYSVAR to allow
                 preconfigure network configurations.
01c,13sep06,kch  Refresh with Sept 08, 2006 code drop
01b,05sep06,dlk  Eliminate some compiler warnings.
01a,28aug06,kch  Update based on Aug 22, 2006 code drop.
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

#include "ipcom_config.h"
#if 0
#include "ipcom_sock.h"
#include "ipcom_clib.h"
#include "ipcom_inet.h"
#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#endif

#include "ipcom_err.h"
#include "ipcom_sysvar.h"

#if 0
#include "ipdhcpr.h"
#endif

#include "ipdhcpr_config.h"

/*
 ****************************************************************************
 * 3                    TYPES
 ****************************************************************************
 */
/* typedef to make compiler happy... */
typedef int (*ipdhcpr_start_cb)(void);


/*
 ****************************************************************************
 * 4                    GENERAL
 ****************************************************************************
 */
/*
 *===========================================================================
 *                      ipdhcpr_conf_sysvar
 *===========================================================================
 * Description: Initiates mandatory system configuration variables specific
 *              for IPDHCPR.
 *
 */ 
IP_STATIC IP_CONST Ipcom_sysvar ipdhcpr_conf_sysvar[] =
{
    /*
     * server_port
     *
     * UDP port used by the DHCP server.
     * 
     * Default: 67 
     */ 

    { "ipdhcpr.ServerPort",  DHCPR_SERVER_PORT},

    /*
     * client_port
     *
     * UDP port used by the dhcp/bootp clients. 
     *
     * Default: 68 
     */

    { "ipdhcpr.ClientPort",  DHCPR_CLIENT_PORT},

    /*
     * packet_size
     *
     * Max DHCP packet size the DHCPR can handle
     *
     * Default: 576 
     */

    { "ipdhcpr.PacketSize",   DHCPR_MAX_PKT_SIZE },
 
    /*
     * hops_treshold
     *
     * Max number of relay agent allowed between a client and
     * and a server. Packets with a hops count greater than this
     * treshold value are silently dropped.
     *
     * Default: 3 
     */

    { "ipdhcpr.HopsThreshold",   DHCPR_HOPS_THRESHOLD},

    { IP_NULL, IP_NULL}
};


/*
 * ipdhcpr_netconf_sysvar - example implementation of dhcp relay network
 *                          configuration array
 *
 * DHCP relay network pre-configuratio array is used to statically configure
 * the dhcp relay with a preconfigured setup of networks and hosts when
 * initiated. Each system variable in this array must be named according to
 * the syntax ipdhcpr.netconf.<index>, where <index> is a unique line
 * indicator. The value of each system variable must consist of a IPDHCPR
 * shell command (without the preceeding dhcpr that is used only when
 * executing from a shell). The commands are executed in the order they
 * are defined.
 *
 * To use the ipdhcpr_netconf_sysvar, defines INCLUDE_IPDHCPR_USE_EXAMPLE
 * and set the configuration macro DHCPR_NETCONF_SYSVAR in your bsp config.h
 * (or in your VxWorks Image Project) to ipdhcpr_netconf_sysvar.
 *
 * IPDHCPR_INCLUDE_CMD is defined in ipdhcpr_config.h. If changing that 
 * define, the <ipdhcpr> directory must be recompiled.
 *
 */ 
#if defined (IPDHCPR_INCLUDE_CMD) && defined(INCLUDE_IPDHCPR_USE_EXAMPLE)
IP_STATIC IP_CONST Ipcom_sysvar ipdhcpr_netconf_sysvar[] =
{
    {"ipdhcpr.netconf.0", "server add 10.1.2.165"},

    /* End of configuration - DO NOT REMOVE! */
    {IP_NULL, IP_NULL}
};
#endif /* IPDHCPR_INCLUDE_CMD && INCLUDE_IPDHCPR_USE_EXAMPLE */


#if DHCPR_INSTALL_CALLBACK_HOOK == TRUE

/*
 * forward declarations for the user-defined dhcp relay callback hooks
 * to satisfy the compiler.
 */
/* using NULL here with gnu can result in compile error */ 
#if (DHCPR_START_CALLBACK_HOOK != 0)
int DHCPR_START_CALLBACK_HOOK (void);
#endif
#else

/*
 * DHCPR_START_CALLBACK_HOOK is applicable only if DHCPR_INSTALL_CALLBACK_HOOK
 * is set to TRUE. Explicitly re-define the hook to NULL so that we don't get
 * compiler errors
 */
#undef DHCPR_START_CALLBACK_HOOK
#define DHCPR_START_CALLBACK_HOOK NULL

#endif /* DHCPR_INSTALL_CALLBACK_HOOK */



/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */
/*
 *===========================================================================
 *                    ipdhcpr_start_hook
 *===========================================================================
 * Description: This routine is called by IPDHCPR at start-up. It is
 *              intended to be used for static configuration of IPDHCPR.
 *
 * Parameters:  - 
 * Returns:     0 - success
 *             -1 - failure
 *
 */ 
IP_GLOBAL int
ipdhcpr_start_hook (void)
{
    ipdhcpr_start_cb dhcpr_cb;

    dhcpr_cb = (DHCPR_START_CALLBACK_HOOK != NULL) ?
                DHCPR_START_CALLBACK_HOOK : NULL;
    
    /* 
     * call the user callback routine if provided. Otherwise, simply return 0.
     */
    return (dhcpr_cb != NULL) ? dhcpr_cb() : 0;
}


/*
 ****************************************************************************
 * 11                   GLOBAL FUNCTIONS
 ****************************************************************************
 */
/*
 *===========================================================================
 *                    ipdhcps_configure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipdhcpr_configure(void)
{
    /* Initialize IPDHCPR system configuration. */
    (void)ipcom_sysvar_setv(ipdhcpr_conf_sysvar, 0);

    /* 
     * Note: IPDHCPR_INCLUDE_CMD is defined in ipdhcpr_config.h. If
     *       changing that define, the <ipdhcpr> directory must be
     *       recompiled
     */
#ifdef IPDHCPR_INCLUDE_CMD
    {
        Ipcom_sysvar *entries;

        if ((entries = (Ipcom_sysvar *)DHCPR_NETCONF_SYSVAR) != NULL)
        {
            /* Initialize IPDHCPR network preconfiguration */
            (void)ipcom_sysvar_setv(entries, 0);
        }
    }
#endif /* IPDHCPR_INCLUDE_CMD */

    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

