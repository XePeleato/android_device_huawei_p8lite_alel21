/*
 ******************************************************************************
 *                     IPCOM CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipdhcps_config.c,v $ $Revision: 1.4.24.1 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/ipdhcps_config.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *
 *   Design and implementation by Jonas Rendel <jonas@interpeak.se>
 ******************************************************************************
 */

/*
modification history
--------------------
01h,04sep09,h_s  Fix defect WIND00184353.
01g,02jun09,spw  Add no strict subnet match sysvar
01f,03jan07,kch  Updated CVS revision log entry for Jan 03 code drop.
01e,11dec06,kch  Modified ipdhcps_start_hook() and ipdhcps_stop_hook()
                 to allow for user configurable callback hooks. Also
                 added DHCPS_NETCONF_SYSVAR to allow preconfigure network
                 configurations.
01d,01dec06,kch  Updated CVS revision log entry.
01c,13sep06,kch  Refresh with Sept 08, 2006 code drop
01b,04sep06,kch  Include only minimum set of header files to workaround
                 symbol redeclaration errors.
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
#include "ipcom_err.h"

#if 0
#include "ipcom_clib.h"
#include "ipcom_sock.h"
#include "ipcom_inet.h"
#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#endif

#include "ipcom_sysvar.h"
#include "ipdhcps_config.h"

#if 0
#include "ipdhcps.h"
#endif

/*
 ****************************************************************************
 * 3                    GENERAL
 ****************************************************************************
 */

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

/* typedef to make compiler happy... */
typedef int (*ipdhcps_start_cb)(void);
typedef int (*ipdhcps_stop_cb)(void);

/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

IP_EXTERN int ipdhcps_start_hook (void);
IP_EXTERN int ipdhcps_stop_hook (void);


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
 *===========================================================================
 *                      ipdhcps_conf_sysvar
 *===========================================================================
 * Description: Initiates mandatory system configuration variables specific
 *              for IPDHCPS.
 *
 */
IP_STATIC IP_CONST Ipcom_sysvar ipdhcps_conf_sysvar[] =
{
    /*
     * server_port
     *
     * UDP port used by the IPDHCPS server.
     *
     * Default: 67
     */

    { "ipdhcps.server_port",  DHCPS_SERVER_PORT},

    /*
     * client_port
     *
     * UDP port used by the dhcp/bootp clients.
     *
     * Default: 68
     */

    { "ipdhcps.client_port",  DHCPS_CLIENT_PORT},

    /*
     * default_lease_time
     *
     * The default time a client can hold a dynamic lease before
     * it expires.
     *
     * Default: 864000 seconds (10 days).
     */

    { "ipdhcps.default_lease_time",  DHCPS_DEFAULT_LEASE_TIME},

    /*
     * max_lease_time
     *
     * The maximum time a client can hold a dynamic lease before
     * it expires.
     *
     * Default: 8640000 seconds (100 days).
     */

    { "ipdhcps.max_lease_time", DHCPS_MAX_LEASE_TIME},

    /*
     * min_lease_time
     *
     * The minimimum time a client can hold a dynamic lease before
     * it expires.
     *
     * Default: 60 seconds
     */

    { "ipdhcps.min_lease_time", DHCPS_MIN_LEASE_TIME},

    /*
     * renewal_time
     *
     * The time in seconds when the client must start asking to extend
     * the lease. This is known as T1 or the renewal time.
     * If this constant is set to zero the server will not give
     * the client a value for T1. The client then typically
     * defaults this value to one-half of its lease time.
     *
     * Default: disabled
     *
     */

    { "ipdhcps.renewal_time", DHCPS_RENEWAL_TIME},

    /*
     * rebinding_time
     *
     * The time in seconds when the client must start rebinding
     * the lease if it has failed to renew it. This is known as T2
     * or the rebinding time.
     * If this constant is set to zero the server will not give
     * the client a value for T2. The client then typically defaults
     * value for this parameter.
     *
     * Default: disabled
     */

    { "ipdhcps.rebinding_time", DHCPS_REBINDING_TIME},

    /*
     * in_abandoned_state_max_time
     *
     * The time that must pass before an abandoned lease is allowed
     * to be released and set to state free.
     *
     * Default: 8640000 seconds (100 days)
     */

    { "ipdhcps.in_abandoned_state_max_time", DHCPS_ABANDONED_STATE_MAX_TIME},

    /*
     * in_expired_state_max_time
     *
     * The time in seconds that must pass before an expired lease will
     * be set to state free to be reused.
     *
     * Default: "forever"
     */

    { "ipdhcps.in_expired_state_max_time", DHCPS_EXPIRED_STATE_MAX_TIME},

    /*
     * in_released_state_max_time
     *
     * The time in seconds that must pass before a released lease can
     * be set to state free to be reused.
     *
     * Default: "forever"
     */

    { "ipdhcps.in_released_state_max_time", DHCPS_RELEASED_STATE_MAX_TIME},
    
    /*
     * in_offered_state_max_time
     *
     * The time in seconds that must pass before an offered lease can
     * be set to state free to be reused.
     *
     * Default: 10 seconds
     */

    { "ipdhcps.in_offered_state_max_time", DHCPS_OFFERED_STATE_MAX_TIME},
    
    /*
     * lease_for_bootp_client_max_time
     *
     * The time in seconds that must pass before a lease held by a
     * bootp client can be set to state free to be reused.
     *
     * Default: "forever"
     */

    { "ipdhcps.lease_for_bootp_client_max_time", DHCPS_LEASE_BOOTPC_MAX_TIME},

    /*
     * allow_decline
     *
     * This boolean tells if IPDHCPS shall ignore DHCP DECLINE messages
     * or if it shall honor them in an appropriate way.
     * If set to "true" the IPDHCPS lease database can in worst case be
     * exhausted by a malfunctioning DHCP client or a denial of service
     * attack.
     *
     * Default: "true"
     */

    { "ipdhcps.allow_decline", DHCPS_ALLOW_DECLINE},

    /*
     * packet_size
     *
     * The size in bytes of a DHCP packet used by IPDHCPS. It must never
     * be less than 576 bytes. If set to a higher value the IPDHCPS will
     * automatically announce this size to all contacting clients.
     *
     * Default: 576 bytes
     */

    { "ipdhcps.packet_size", DHCPS_PKT_SIZE},

    /*
     * allow_bootp
     *
     * This boolean tells if IPDHCPS shall accept requests from bootp
     * clients or not.
     *
     * Default: "true"
     */

    { "ipdhcps.allow_bootp", DHCPS_ALLOW_BOOTP},

    /*
     * allow_dynamic_bootp
     *
     * This boolean tells if IPDHCPS shall accept allocating dynamic leases
     * to bootp clients or not.
     * Becasue of limitations in the bootp protocol, this type of clients
     * can not release allocated addresses. Hence it is not recommended to
     * allow them to have dynamic leases.
     *
     * Default    : "false" (do not accept)
     */

    { "ipdhcps.allow_dynamic_bootp", DHCPS_ALLOW_DYNAMIC_BOOTP},

    /*
     * do_icmp_address_check
     *
     * This boolean tells if IPDHCPS shall perfom a icmp ping check on
     * each address before it is offered to a DHCP client.
     *
     * Default    : "false"
     */

    { "ipdhcps.do_icmp_address_check", DHCPS_DO_ICMP_ADDRESS_CHECK},


    /*
     * authorized_agents
     *
     * This parameter is either set to "any" or is a list of IP
     * addresses to dhcp relay agents that are authorized to
     * access this dhcp server.
     *
     * Default    : "any"
     */
    { "ipdhcps.authorized_agents", DHCPS_AUTHORIZED_AGENTS},
    
    /*
     * no_strict_subnet_match
     *
     * This boolean determines whether IPDHCPS should _not_ prefer to issue a lease from
     * a pool with the same subnet that is present on the enabled dhcps interface.
     *
     * Default: "false"
     */
    { "ipdhcps.no_strict_subnet_match", DHCPS_NO_STRICT_SUBNET_MATCH},

    { IP_NULL, IP_NULL }
};

/*
 * ipdhcps_netconf_sysvar - example implementation of dhcp server network
 *                          configuration array
 *
 * DHCP server network pre-configuratio array is used to statically configure
 * the dhcp server with a preconfigured setup of networks and hosts when
 * initiated. Each system variable in this array must be named according to
 * the syntax ipdhcps.netconf.<index>, where <index> is a unique line
 * indicator. The value of each system variable must consist of a IPDHCPS
 * shell command (without the preceeding dhcps that is used only when
 * executing from a shell). The commands are executed in the order they
 * are defined.
 *
 * To use the ipdhcps_netconf_sysvar, defines the INCLUDE_IPDHCPS_USE_EXAMPLE
 * and set the configuration macro DHCPS_NETCONF_SYSVAR in your bsp config.h
 * (or in your VxWorks Image Project) to ipdhcps_netconf_sysvar.
 *
 * IPDHCPS_INCLUDE_CMD is defined in ipdhcps_config.h. If changing that
 * define, the <ipdhcps> directory must be recompiled.
 *
 */
#if defined (IPDHCPS_INCLUDE_CMD) && defined(INCLUDE_IPDHCPS_USE_EXAMPLE)
IP_STATIC IP_CONST Ipcom_sysvar ipdhcps_netconf_sysvar[] =
{
    /* Add a subnet 10.1.0.0 with subnet mask 255.255.0.0 */
    {"ipdhcps.netconf.00", "subnet add 10.1.0.0 255.255.0.0"},

    /* Add an address  pool to this subnet */
    {"ipdhcps.netconf.01", "pool add 10.1.0.0 10.1.0.5 10.1.0.99"},

    /* Add another address pool to this subnet */
    {"ipdhcps.netconf.02", "pool add 10.1.0.0 10.1.0.199 10.1.0.249"},

    /* Add a domain name (dhcp option 15) to this subnet */
    {"ipdhcps.netconf.03", "option add 10.1.0.0 15 green.net"},

    /* Add a router (dhcp option 3) to this subnet */
    {"ipdhcps.netconf.04", "option add 10.1.0.0 3 10.1.0.1"},

    /* Add a name server (dhcp option 5) to this subnet */
    {"ipdhcps.netconf.05", "option add 10.1.0.0 5 10.1.0.2"},

    /* A another subnet 192.168.2.0 with netmask 255.255.255.0 */
    {"ipdhcps.netconf.06", "subnet add 192.168.2.0 255.255.255.0"},

    /* Add an address pool to this subnet*/
    {"ipdhcps.netconf.07", "pool add 192.168.2.0 192.168.2.10 192.168.2.50"},

    /* Add a domain name (dhcp option 15) to this subnet */
    {"ipdhcps.netconf.08", "option add 192.168.2.0 15 blue.net"},

    /* Add two routers (dhcp option 3) to this subnet */
    {"ipdhcps.netconf.09", "option add 192.168.2.0 5 192.168.2.1 192.168.2.2"},

    /* Add a static route (dhcp option 33) on this subnet (10.1.0.0->192.168.2.2 */
    {"ipdhcps.netconf.10", "option add 192.168.2.0 33 10.1.0.0 192.168.2.2"},

    /* Set max lease time to 120 seconds on this subnet */
    {"ipdhcps,netconf.11", "config set 192.168.2.0 max-lease-time 120"},

    /* Set default lease time to 60 seconds on this subnet */
    {"ipdhcps.netconf.12", "config set 192.168.2.0 default-lease-time 60"},

    /* Add a host with the client identifier 'Jack's lap top' */
    {"ipdhcps.netconf.13", "host add 10.1.0.142 -i \"\\0Jack's lap top\""},

    /* Add a host name (dhcp option 12) for this host */
    {"ipdhcps.netconf.14", "option add 10.1.0.142 12 blue-host"},

     /* Add a router (dhcp option 3) to this host */
    {"ipdhcps.netconf.15", "option add 10.1.0.142 3 10.1.0.1"},

    /* Add a host with a standard ethernet MAC address */
    {"ipdhcps.netconf.16", "host add 10.1.0.141 -h BE:60:13:F7:F8:0F"},

    /* Add a host name (dhcp option 12) for this host */
    {"ipdhcps.netconf.17", "option add 10.1.0.141 12 yellow-host"},

    /* Setup a boot file for this host */
    {"ipdhcps.netconf.18", "config set 10.1.0.141 boot-file /boot/yellow-host.boot"},

    /* Setup a boot server for this host to boot from */
    {"ipdhcps.netconf.19", "config set 10.1.0.141 next-server-name black-server"},

    /* Setup the default lease time to 600 seconds on the default net */
    {"ipdhcps.netconf.20", "config set default default-lease-time 600"},

    /* Setup bootp allowance on the default net */
    {"ipdhcps.netconf.21", "config set default allow-bootp false"},

    /* Enable ICMP ping checking on subnet 10.1.0.0 */
    {"ipdhcps.netconf.22", "config set 10.1.0.0 do-icmp-address-check true"},

    /* Add vendor class identifer on subnet 10.1.0.0 */
    {"ipdhcps.netconf.23", "class add 10.1.0.0 first-class vendor first"},

    /* Add vendor class on default */
    {"ipdhcps.netconf.24", "class add default second-class vendor second"},

    /* Setup ARP cache timeout on subnet 10.1.0.0 */
    {"ipdhcps.netconf.25", "option add 10.1.0.0 35 10"},

    /* Setup ARP cache timeout for vendor class on subnet 10.1.0.0 */
    {"ipdhcps.netconf.26", "option add 10.1.0.0 -c first-class 35 20"},

    /* Setup ARP cache on default */
    {"ipdhcps.netconf.27", "option add default 35 60"},

    /* Setup ARP cache on vendor class on default */
    {"ipdhcps.netconf.28", "option add default -c second-class 35 40"},

    /* Setup authorized relay agents on subnet 10.1.0.0 */
    {"ipdhcps.netconf.29", "config set 10.1.0.0  authorized-agents 10.1.0.2 10.1.0.3"},

    /* End of configuration - DO NOT REMOVE! */
    { IP_NULL, IP_NULL}
};
#endif /* IPDHCPS_INCLUDE_CMD && INCLUDE_IPDHCPS_USE_EXAMPLE */


#if DHCPS_INSTALL_CALLBACK_HOOK == TRUE

/*
 * forward declarations for the user-defined dhcp server callback hooks
 * to satisfy the compiler.
 */
/* using NULL here with gnu can result in compile error */
#if (DHCPS_START_CALLBACK_HOOK != 0 && DHCPS_STOP_CALLBACK_HOOK != 0)
int DHCPS_START_CALLBACK_HOOK (void);
int DHCPS_STOP_CALLBACK_HOOK (void);
#endif
#else

/*
 * DHCPS_START_CALLBACK_HOOK and DHCPS_STOP_CALLBACK_HOOK are applicable
 * only if DHCPS_INSTALL_CALLBACK_HOOK is set to TRUE. Explicitly re-define
 * these hooks to NULL so that we don't get compiler errors
 */
#undef DHCPS_START_CALLBACK_HOOK
#define DHCPS_START_CALLBACK_HOOK NULL

#undef DHCPS_STOP_CALLBACK_HOOK
#define DHCPS_STOP_CALLBACK_HOOK  NULL

#endif /* DHCPS_INSTALL_CALLBACK_HOOK */


/*****************************************************************************
 *
 * ipdhcps_start_hook - dhcps server startup callback routine
 *
 * This routine is called by IPDHCPS at start-up. It is intended to be used
 * for static configuration of IPDHCPS, for instance to install a lease
 * 'dump to file' routine and/or to restore a IPDHCPS database from a lease
 * dump.
 *
 * RETURNS:
 * 0 if success or -1 if failure
 *
 * NOMANUAL
 */
IP_GLOBAL int
ipdhcps_start_hook (void)
{
    ipdhcps_start_cb dhcps_cb;

    dhcps_cb = (DHCPS_START_CALLBACK_HOOK != NULL) ?
                DHCPS_START_CALLBACK_HOOK : NULL;
    
    /* 
     * call the user callback routine if provided. Otherwise, simply return 0.
     */
    return (dhcps_cb != NULL) ? dhcps_cb() : 0;
}

/****************************************************************************
 *
 * ipdhcps_stop_hook - dhcp server stop callback routine
 * 
 * This routine is called by IPDHCPS when the server upon termination of the
 * DHCP server. It is intended to be used for static configuration of IPDHCPS,
 * for instance to save the lease database at shutdown so that the lease can 
 * later be restored.
 *
 * RETURNS: 
 * 0 if success or -1 if failure
 *
 * NOMANUAL
 */
IP_GLOBAL int
ipdhcps_stop_hook (void)
{
    ipdhcps_stop_cb dhcps_cb;

    dhcps_cb = (DHCPS_STOP_CALLBACK_HOOK != NULL) ?
               DHCPS_STOP_CALLBACK_HOOK : NULL;
    
    /* 
     * call the user callback routine if provided. Otherwise, simply return 0.
     */
	return (dhcps_cb != NULL) ? dhcps_cb() : 0;
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
ipdhcps_configure(void)
{
    /* Initialize IPDHCPS system configuration. */
    (void)ipcom_sysvar_setv(ipdhcps_conf_sysvar, 0);

    /* 
     * Note: IPDHCPS_INCLUDE_CMD is defined in ipdhcps_config.h. If
     *       changing that define, the <ipdhcps> directory must be
     *       recompiled
     */
#if defined (IPDHCPS_INCLUDE_CMD)
    {
        Ipcom_sysvar *entries;
        
        if ((entries = (Ipcom_sysvar *)DHCPS_NETCONF_SYSVAR) != NULL)
        {
            /* Initialize IPDHCPS network preconfiguration */
            (void)ipcom_sysvar_setv(entries, 0);
        }
    }
#endif /* IPDHCPS_INCLUDE_CMD */

    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

