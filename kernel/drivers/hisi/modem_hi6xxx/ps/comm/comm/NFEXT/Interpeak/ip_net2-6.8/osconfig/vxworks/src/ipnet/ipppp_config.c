/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipppp_config.c,v $ $Revision: 1.5.18.1.2.2 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/ipppp_config.c,v $
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
01d,22jan07,kch  Added sysvars for PPP username and password (WIND00085576).
01c,03jan07,kch  Removed PPPOE_IF_MAX_SESSIONS_LIST from pppoe.
01b,01dec06,kch  Updated CVS revision log entry.
01a,17sep06,kch  Integrated to vxWorks build system.
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


#include <ipcom_type.h>

#include <ipcom_sysvar.h>
#include <ipcom_err.h>
#include <ipcom_shell.h>

#include "ipppp_config.h"

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

IP_PUBLIC int ipppp_cmd_pppconfig(int argc, char **argv);


/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipppp_configure(void);


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

/*
 *===========================================================================
 *                         ipppp_conf_sysvar
 *===========================================================================
 */
IP_STATIC IP_CONST Ipcom_sysvar ipppp_conf_sysvar[] =
{
#ifdef INCLUDE_IPPPP

    /* Device driver name. */
#ifdef IP_PORT_VXWORKS
    { "ipcom.if.ppp0.devname", "/tyCo/1" },  /* COM2 */
#else
    { "ipcom.if.ppp0.devname", "/dev/ttyS0" }, /* UNIX default */
#endif

    /* Default baudrate. */
    { "ipppp.baudrate", PPP_DEFAULT_BAUDRATE },

    /* PPP runmode
     * Supported modes: start, passive, exit
     */
    { "ipppp.runmode", PPP_RUNMODE },

    /* PPP flags
     * Supported flags: defaultroute, proxyarp
     */
    { "ipppp.flags", PPP_FLAGS },

    /* Authentication mode.
     * Supported modes: auth, noauth, pap, refuse-pap, chap, refuse-chap
     */
    { "ipppp.auth", PPP_AUTH_MODES },

    /* LCP maximum receive unit */
    { "ipppp.lcp.mru", PPP_LCP_MRU },

    /* LCP maximum transmit unit */
    { "ipppp.lcp.mtu", PPP_LCP_MTU },

    /* Default LCP echo requests interval in seconds (0 to disable) */
    { "ipppp.lcp.echo_interval",  PPP_LCP_ECHO_REQ_INTERVAL },

    /* Max LCP EchoReq failures before link termination. */
    { "ipppp.lcp.echo_failure", PPP_LCP_ECHO_REQ_FAILURE },

    /* Local IPv4 address. */
    { "ipppp.ipcp.addr", PPP_IPCP_IPV4_ADDRESS },

    /* Peer IPv4 address */
    { "ipppp.ipcp.dstaddr",PPP_IPCP_PEER_IPV4_ADDRESS },

    /* Peer IPv4 address pool name */
    { "ipppp.ipcp.pool", PPP_IPCP_PEER_IPV4_POOL_NAME },

    /* IPv4 peer address pool */
    { "ipppp.ipcp.pool." PPP_IPCP_PEER_IPV4_POOL_NAME,
       PPP_IPCP_PEER_IPV4_ADDRESS_POOL },

    /* PPP 'client' username and password. */
    { "ipppp.username", PPP_USERNAME},
    { "ipppp.password", PPP_PASSWD },

    /* Primary and secondary DNS and NBNS(WINS) addresses */
    { "ipppp.ipcp.primary_dns_address", PPP_IPCP_PRIMARY_DNS_ADDRESS },
    { "ipppp.ipcp.secondary_dns_address", PPP_IPCP_SECONDARY_DNS_ADDRESS },
    { "ipppp.ipcp.primary_nbns_address",  PPP_IPCP_PRIMARY_NBNS_ADDRESS},
    { "ipppp.ipcp.secondary_nbns_address", PPP_IPCP_SECONDARY_NBNS_ADDRESS },

#endif /* INCLUDE_IPPPP */

#ifdef INCLUDE_IPPPPOE
    /* Enable(1) /disable(0) PPPoE server */
    { "ipppp.pppoe.server", PPPOE_SERVER},

    /* Access Concentrator Name (AC-Name) */
    { "ipppp.pppoe.ac_name", PPPOE_AC_NAME},

    /* Secret name (for PPPoE cookie). This macro is used in a MD5 checksum
     * calculation to thwart DOS (denial-of-service) attacks. The definition of
     * the string need not match any remote configuration! The value should be
     * changed from its factory default.
     */
    { "ipppp.pppoe.secret_name", PPPOE_SECRET_NAME},

    /* Maximum total number of PPPoE sessions. */
    { "ipppp.pppoe.max_sessions", PPPOE_MAX_SESSIONS},

    /* Maximum number of PPPoE sessions per Ethernet interface. */
    { "ipppp.pppoe.max_eth_sessions", PPPOE_MAX_ETH_SESSIONS},

    /* Service name we accept. Undefine to accept any service name */
    { "ipppp.pppoe.service_name", PPPOE_SERVICE_NAME},

#endif /* INCLUDE_IPPPPOE */

#ifdef INCLUDE_IPPPPMP
    /* Enable(1) / disable(0) PPP multilink protocol */
    { "ipppp.mp.enable",        PPPMP_ENABLE},

    /* Set the maximum reconstructed received unit */
    { "ipppp.mp.mrru",          PPPMP_MRRU},

#ifdef INCLUDE_IPPPPOE

    /* Set the number of PPPoE links to start up at connection time */
    { "ipppp.pppoe.mp_links",   PPPMP_PPPOE_LINKS},

#endif /* INCLUDE_IPPPPOE */
#endif /* INCLUDE_IPPPPMP */

#ifdef INCLUDE_IPPPP_VJCOMP
    /* Enable(1) / disable(0) PPP Van Jacobson Compression */
    { "ipppp.vjc.enable",        PPPVJC_ENABLE},
#endif /* INCLUDE_IPPPP_VJCOMP */

#ifdef IPPPP_USE_PPPL2TP
     { "ipppp.l2tp.lnsAddrDefault", PPP_L2TP_LNS_ADDRESS_DEFAULT},
#ifdef IPPPP_USE_PPPL2TP_LNS
     { "ipppp.l2tp.server", "0"},
#endif
#endif

    { IP_NULL, IP_NULL }
};

/* Interface specific list configurations */
IP_CONST Ipcom_sysvar_ext ipppp_conf_sysvar_ext[] =
{
#ifdef INCLUDE_IPPPP_INTERFACE_CONFIG
    /*
     * Interface initial baudrate. This parameter is configurable on a
     * per-interface basis in the form of "<ifparam>=<value>", each separated
     * from the other by a semicolon. E.g, "ppp0=38400".
     */
    {"ipcom.if", "ipppp.baudrate", PPP_IF_DEFAULT_BAUDRATE_LIST},

    /*
     * Interface runmode. Specifies one or more runmode options on an
     * interface. Possible runmode options are "start", "passive" and "exit".
     * This parameter is configurable on a per-interface basis in the form
     * of "<ifparam>=<value>", each separated from the other by a semicolon.
     * E.g, "ppp0=start,passive".
     */
    {"ipcom.if", "ipppp.runmode", PPP_IF_RUNMODE_LIST},

    /*
     * Interface PPP flags. Specifies one or more flag behavioural options
     * on an interface. Possible flags options are "defaultroute" and
     * "proxyarp". This parameter is configurable on a per-interface basis
     * in the form of "<ifparam>=<value>", each separated from the other by
     * a semicolon. E.g, "ppp0=proxyarp".
     */
    {"ipcom.if", "ipppp.flags", PPP_IF_FLAGS_LIST},

    /*
     * Configures the required authentication modes. There are currently
     * six authentication options defined - "auth", "noauth", "pap",
     * "refuse-pap", "chap", and "refuse-chap". This parameter is configurable
     * on a per-interface basis in the form of "<ifparam>=<value>", each
     * separated from the other by a semicolon. E.g. "ppp0=chap;ppp1=pap".
     */
    { "ipcom.if", "ipppp.auth", PPP_IF_AUTH_MODES_LIST},

    /*
     * Interface LCP maximum receive unit. This parameter is configurable
     * on a per-interface basis in the form of "<ifparam>=<value>", each
     * separated from the other by a semicolon. E.g, "ppp0=1500".
     */
    {"ipcom.if", "ipppp.lcp.mru", PPP_IF_LCP_MRU_LIST},

    /*
     * Interface LCP maximum transmit unit. This parameter is configurable
     * on a per-interface basis in the form of "<ifparam>=<value>", each
     * separated from the other by a semicolon. E.g, "ppp0=1500".
     */
    {"ipcom.if", "ipppp.lcp.mtu", PPP_IF_LCP_MTU_LIST},

    /*
     * LCP echo request output interval in seconds. 0 to disable. This
     * parameter is configurable on a per-interface basis in the form of
     * "<ifparam>=<value>", each separated from the other by a semicolon.
     * E.g, "ppp0=60".
     */
    {"ipcom.if", "ipppp.lcp.echo_interval", PPP_IF_LCP_ECHO_REQ_INTERVAL_LIST},

    /*
     * Max echo request failures before link termination. This parameter can
     * be configured on a per-interface basis in the form of
     * "<ifparam>=<value>", each separated from the other by a semicolon.
     * E.g, "ppp0=5"
     */
    {"ipcom.if", "ipppp.lcp.echo_failure", PPP_IF_LCP_ECHO_REQ_FAILURE_LIST},

    /*
     * Local IPv4 address list
     * Specifies the desired local IPv4 address. Setting local IPv4 address
     * to 0 means asking the peer to suggest our IPv4 address. This parameter
     * is configurable on a per-interface basis in the form of
     * "<ifparam>=<value>", each separated from the other by a semicolon.
     * E.g, "ppp0=12.0.0.100".
     */
    {"ipcom.if", "ipppp.ipcp.addr", PPP_IF_IPCP_IPV4_ADDRESS_LIST},

    /*
     * Peer IPv4 address list
     * Suggest peer IPv4 address if asked for. Note that the peer IPv4
     * address will only get suggested to the peer if it ask for it by
     * requesting IP address 0. This parameter is configurable on a
     * per-interface basis in the form of "<ifparam>=<value>", each separated
     * from the other by a semicolon. E.g, "ppp0=10.1.4.1".
     */
    { "ipcom.if", "ipppp.ipcp.dstaddr", PPP_IF_IPCP_PEER_IPV4_ADDRESS_LIST},

    /*
     * Interface IPv4 peer address pool list
     * Configure a IPv4 address pool used for the interface to supply peers
     * with IPv4 addresses when running as a PPP or PPPoE server. This
     * parameter is configurable on a per-interface basis in the form of
     * "<ifparam>=<value>", each separated from the other by a semicolon.
     * E.g., "ppp0=10.1.3.1-10.1.3.255".
     */
    { "ipcom.if", "ipppp.ipcp.pool",
      PPP_IF_IPCP_PEER_IPV4_ADDRESS_POOL_LIST},

    /*
     * Configures the PPP client IPv4 primary DNS address. The configured
     * IPv4 address will be given to the peer (the client) if the peer
     * requests a primary DNS address. This parameter is configurable on
     * a per-interface basis in the form of "<ifparam>=<value>", each
     * separated from the other by a semicolon.
     * E.g. "ppp0=10.1.2.3;ppp1=10.2.2.3".
     */
    { "ipcom.if", "ipppp.ipcp.primary_dns_address",
      PPP_IF_IPCP_PRIMARY_DNS_ADDRESS_LIST },

    /*
     * Configures the PPP client IPv4 secondary DNS address. The configured
     * IPv4 address will be given to the peer (the client) if the peer
     * requests a secondary DNS address. This parameter is configurable on
     * a per-interface basis in the form of "<ifparam>=<value>", each
     * separated from the other by a semicolon.
     * E.g. "ppp0=195.42.198.5;ppp1=195.43.198.5".
     */
    { "ipcom.if", "ipppp.ipcp.secondary_dns_address",
      PPP_IF_IPCP_SECONDARY_DNS_ADDRESS_LIST },

#endif /* INCLUDE_IPPPP_INTERFACE_CONFIG */

#ifdef  INCLUDE_IPPPP_USERS_CONFIG
    /*
     * LCP echo request output interval in seconds. 0 to disable. This
     * parameter is configurable on a per-user basis in the form of
     * "<username>=<value>", each separated from the other by a semicolon.
     * E.g, "user1=60;user2=60".
     */
    {"ipcom.users", "ipppp.lcp.echo_interval",
      PPP_USERS_LCP_ECHO_REQ_INTERVAL_LIST},

    /*
     * user local IPv4 address list
     * Specifies the desired local IPv4 address. Setting local IPv4 address
     * to 0 means asking the peer to suggest our IPv4 address. This parameter
     * is configurable on a per-user basis in the form of
     * "<username>=<value>". Each pair of "<username>=<value>" is semicolon
     * separated. E.g, "user1=12.0.0.100;user2=12.1.0.100".
     */
    {"ipcom.users", "ipppp.ipcp.addr", PPP_USERS_IPCP_IPV4_ADDRESS_LIST},

    /*
     * Users Peer IPv4 address list
     * Suggest peer IPv4 address if asked for. Note that the peer IPv4
     * address will only get suggested to the peer if it ask for it by
     * requesting IP address 0. This parameter is configurable on a
     * per-user basis in the form of "<username>=<value>". Each pair
     * of "<username>=<value>" is semicolon separated.
     * E.g, "user1=10.1.4.1;user2=10.2.4.1"
     */
    { "ipcom.users", "ipppp.ipcp.dstaddr",
      PPP_USERS_IPCP_PEER_IPV4_ADDRESS_LIST},

    /*
     * Users IPv4 peer address pool list
     * Configure a IPv4 address pool used for a user  to supply peers with
     * IPv4 addresses when running as a PPP or PPPoE server. This parameter
     * is configurable on a per-user basis in the form of
     * "<username>=<value>". Each pair of "<username>=<value>" is semicolon
     * separated. E.g., "user1=10.1.3.1-10.1.3.255;user2=10.2.3.1-10.2.3.255".
     */
    { "ipcom.users", "ipppp.ipcp.pool",
      PPP_USERS_IPCP_PEER_IPV4_ADDRESS_POOL_LIST},

    /*
     * Configures the PPP client IPv4 primary DNS address. The configured
     * IPv4 address will be given to the peer (the client) if the peer
     * requests a primary DNS address. This parameter is configurable on
     * a per-user basis in the form of "<ifparam>=<value>", each separated
     * from the other by a semicolon.
     * E.g. "user1=10.1.2.3;user2=10.2.2.3"
     */
    { "ipcom.users", "ipppp.ipcp.primary_dns_address",
      PPP_USERS_IPCP_PRIMARY_DNS_ADDRESS_LIST },

    /*
     * Configures the PPP client IPv4 secondary DNS address. The configured
     * IPv4 address will be given to the peer (the client) if the peer
     * requests a secondary DNS address. This parameter is configurable on
     * a per-user basis in the form of "<ifparam>=<value>", each separated
     * from the other by a semicolon.
     * E.g. "user1=195.42.198.5;user2=195.43.198.5".
     */
    { "ipcom.users", "ipppp.ipcp.secondary_dns_address",
      PPP_USERS_IPCP_SECONDARY_DNS_ADDRESS_LIST },

#endif /* INCLUDE_IPPPP_USERS_CONFIG */

#ifdef INCLUDE_IPPPPOE
    /*
     * Maximum number of PPPoE sessions per Ethernet interface. Note that the
     * interface name used is the Ethernet interface name and not the pppoe
     * interface name. This parameter is configurable on a per-interface basis
     * in the form of "<ifparam>=<value>", each separated from the other by
     * a semicolon. E.g. "eth0=8;eth1=8".
     */
    { "ipcom.if", "ipppp.pppoe.max_eth_sessions",
      PPPOE_IF_MAX_ETH_SESSIONS_LIST},

    /*
     * Service name to accept. Do not set the service name to accept any
     * service name. This parameter is configurable on a per-interface basis
     * in the form of "<ifparam>=<value>", each separated from the other by
     * a semicolon. E.g. "ppp0=myservicename1;ppp1=myservicename2".
     */
    { "ipcom.if", "ipppp.pppoe.service_name", PPPOE_IF_SERVICE_NAME_LIST},
#endif /* INCLUDE_IPPPPOE */

    { IP_NULL, IP_NULL, IP_NULL }
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
 *                    ipppp_configure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipppp_configure(void)
{
    /* Set IPPPP system variables. */
    (void)ipcom_sysvar_setv(ipppp_conf_sysvar, 0);

    /* now configure the interface-specific sysvars */
    (void)ipcom_sysvar_ext_setv(ipppp_conf_sysvar_ext, 0);

#ifdef INCLUDE_IPPPP_CMD
    (void)ipcom_shell_add_cmd("pppconfig", "pppconfig <ifname> [options]",
                              "ppp config", ipppp_cmd_pppconfig,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_SMALL);
#endif /* INCLUDE_IPPPP_CMD */

    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
