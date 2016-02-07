
/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipdhcps6_config.c,v $ $Revision: 1.2 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/ipdhcps6_config.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */

/*
modification history
--------------------
01e,21sep07,brg  Merged with CVS code, support for non-vxworks ports
01d,17jul07,ps   Add init call for dhcpv6 auth databases
01c,03jan07,kch  Updated CVS revision log entry for Jan 03 code drop.
01b,01dec06,kch  Updated CVS revision log entry.
01a,13sep06,tkf  Adopted from ipdhcpc6/config/ipdhcpc6_config.c.
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
#if !defined(WRS_IPNET) && !defined(PRJ_BUILD)
#include "ipdhcps6_config.h"
#endif

#include <ipcom_err.h>
#if !defined(WRS_IPNET) && !defined(PRJ_BUILD)
#include <ipcom_shell.h>
#include <ipcom_sock.h>
#include <ipcom_syslog.h>
#endif
#include <ipcom_sysvar.h>

#if !defined(WRS_IPNET) && !defined(PRJ_BUILD)
#include "../src/ipdhcps6_cmd.h"
#include "../src/ipdhcps6_h.h"
#endif

/*
 ****************************************************************************
 * 3                    DEFINES
 ****************************************************************************
 */

#define IPDHCPS6_CMD_NAME "dhcps6"

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
IP_GLOBAL int ipdhcps6_cmd_dhcps6(int argc, char **argv);

/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipdhcps6_configure(void);

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
#if defined(WRS_IPNET) || defined(PRJ_BUILD)

IP_STATIC IP_CONST Ipcom_sysvar ipdhcps6_conf_sysvar[] =
{
    /* The mode the DHCPv6 daemon is running in, can be 'server' or 'relay'.
       Default is 'server' */
    { "ipdhcps6.mode", DHCPS6_MODE },

    /* The interface the server will use to generate the server ID */
    { "ipdhcps6.duid.ifname", DHCPS6_DUID_IFNAME },

    /* Specifies if the server allows rapid commit or not valid values [yes|no] */
    { "ipdhcps6.server.allow_rapid_commit", DHCPS6_ALLOW_RAPID_COMMIT },

    /* Max hop count in a Relay-forward message, default is 32 */
    { "ipdhcps6.relay.hop_count_limit", DHCPS6_MAXHOP_COUNT },

    { "ipdhcps6.authkey", DHCPS6_HMAC_MD5_SECRET },

    { "ipdhcps6.authrealm", DHCPS6_AUTHENTICATION_REALM },

    { IP_NULL, IP_NULL }
};

/* Interface specific list configurations */
IP_CONST Ipcom_sysvar_ext ipdhcps6_conf_sysvar_ext[] =
{
    /*
     * Specifies which DHCPv6 server or relay agent messages received
     * on interface specified after 'ipdhcps6.relay.map.' should be sent to
     * Leave the address field empty to use the all-dhcp-servers (ff05::1:3)
     * multicast address
     *
     * DHCPS6_RELAY_MAP is a list of Source Interface/Destination Interface
     * pairs, each separated from the other with a semi-colon as follows:
     * "eth0=eth1;vlan21=eth2"
     */
    { "ipdhcps6.relay.map", IP_NULL, DHCPS6_IF_RELAY_MAP_LIST },

    { IP_NULL, IP_NULL, IP_NULL }
};

#else

/* Old style sysvars definition for nonvxworks ports */

IP_STATIC IP_CONST Ipcom_sysvar ipdhcps6_conf_sysvar[] =
  {
    /* The mode the DHCPv6 daemon is running in, can be 'server' or 'relay'.
       Default is 'server' */
    { "ipdhcps6.mode",                       "server" },

    /* The interface the server will use to generate the server ID */
    { "ipdhcps6.duid.ifname",                "eth0" },

    /* Specifies if the server allows rapid commit or not valid values [yes|no] */
    { "ipdhcps6.server.allow_rapid_commit",  "yes" },

    /* Max hop count in a Relay-forward message, default is 32 */
    { "ipdhcps6.relay.hop_count_limit",      "32" },
    /* Specifies which DHCPv6 server or relay agent messages received
       on interface specified after 'ipdhcps6.relay.map.' should be sent to
       Leave the address field empty to use the all-dhcp-servers (ff05::1:3)
       multicast address */
    { "ipdhcps6.relay.map.eth1",             "eth0" },

    { IP_NULL, IP_NULL }
  };


#endif 

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
 *                    ipdhcps6_configure
 *===========================================================================
 * Description: Configures the DHCPv6 server.
 * Parameters:
 * Returns:     IPCOM_SUCCESS          - the operation was successful
 *              IPCOM_ERR_NO_MEMORY    - out of memory.
 *
 */

extern void ipdhcps6_authdb_init (void);

IP_PUBLIC Ip_err
ipdhcps6_configure(void)
{
    Ip_err err;
#ifdef INCLUDE_IPDHCPS6_CMD

    err = ipcom_shell_add_cmd(IPDHCPS6_CMD_NAME, IPDHCPS6_CMD_NAME " [ options ] <modifier...>",
                              "DHCPv6 server", ipdhcps6_cmd_dhcps6,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_DEFAULT);
    if (err != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "ipdhcps6_configure() :: failed to register shell command: %d", err);
        return err;
    }
#endif /* INCLUDE_IPDHCPS6_CMD */

    ipdhcps6_authdb_init ();

    if ((err=ipcom_sysvar_setv(ipdhcps6_conf_sysvar, 0)) == IPCOM_SUCCESS)
    {
        /* now configure the interface-specific sysvars */
#if defined(WRS_IPNET) || defined(PRJ_BUILD)
        return ipcom_sysvar_ext_setv (ipdhcps6_conf_sysvar_ext, 0);
#else 
	return IPCOM_SUCCESS;
#endif 
    }
    /* printf ("returning IPCOM_ERR_INVALID_CONFIG\n"); */
    IPCOM_LOG1(ERR, "ipdhcps6_configure() :: failed to set default sysvar(s): %d", err);
	   
    return IPCOM_ERR_INVALID_CONFIG;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

