/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_radvd_config.c,v $ $Revision: 1.2.60.1 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/ipnet_radvd_config.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 ******************************************************************************
 */

/*
modification history
--------------------
01f,03jan07,kch  Updated CVS revision log entry for Jan 03 code drop.
01e,01dec06,kch  Updated CVS revision log entry.
01d,13sep06,kch  Refresh with Sept 08, 2006 code drop
01c,28aug06,kch  Update based on Aug 22, 2006 code drop.
01b,14aug06,kch  Added ipnet_radvd_conf_sysvar_ext[] to support per-interface
                 configuration parameters.
01a,03aug06,kch  Renamed INCLUDE_RADVD_CMD to INCLUDE_IPRADVD_CMD.
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

#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_sysvar.h>
#include <ipcom_shell.h>


/*
 ****************************************************************************
 * 3                    DEFINES
 ****************************************************************************
 */

#ifndef RADVD_IFLIST_PREFIX_ADV_VALID_LIFETIME_DECREMENT
#define RADVD_IFLIST_PREFIX_ADV_VALID_LIFETIME_DECREMENT ""
#endif

#ifndef RADVD_IFLIST_PREFIX_ADV_PREFERRED_LIFETIME_DECREMENT
#define RADVD_IFLIST_PREFIX_ADV_PREFERRED_LIFETIME_DECREMENT ""
#endif

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
#ifdef INCLUDE_IPRADVD_CMD
IP_EXTERN Ip_err ipnet_cmd_radvd(int argc, char **argv);
#endif

/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_radvd_configure
 *===========================================================================
 * Description: IPD hook to set the default configuration for the VRRP daemon.
 * Parameters:
 * Returns:     IPCOM_SUCCESS if the operation was successful.
 *
 */
IP_PUBLIC Ip_err ipnet_radvd_configure(void);


/*
 ****************************************************************************
 * 7                    DATA
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         ipnet_radvd_conf_sysvar
 *===========================================================================
 * Router Advertisment Daemon Configuration Variables
 * All time values are in milliseconds.
 */
IP_CONST Ipcom_sysvar ipnet_radvd_conf_sysvar[] =
{
    /*
     * The name of the interface radvd will send router advertisement and
     * listen for router solicitation messages. The format is a
     * space-separated list of interface names, e.g. "eth0 eth1". Default:
     * empty list.
     */
    { "ipnet.inet6.radvd.interfaces", RADVD_IFNAME_LIST },

    /*
     * Interface-specific sysvar configurations are now defined in
     * ipnet_radvd_conf_sysvar_ext[]
     */

	{ IP_NULL, IP_NULL }
};

/* Interface specific list configurations */
IP_CONST Ipcom_sysvar_ext ipnet_radvd_conf_sysvar_ext[] =
{
    /*
     * The TRUE(=1) /FALSE(=0) value to be placed in the "Managed address
     * configuration" flag field in the Router Advertisement. This parameter
     * can be specified for each interface that are listed in the
     * RADVD_IFNAME_LIST in the form of "<ifparam>=<value>". Each pair of
     * "<ifparam>=<value>" is semicolon separated. Default: "0" (=FALSE).
     * E.g. "eth0=0" or "eth0=0;eth1=0".
     */
    { "ipnet.inet6.radvd", "AdvManagedFlag", RADVD_IFLIST_ADV_MANAGED_FLAG},

    /*
     * The TRUE(=1) /FALSE(=0) value to be placed in the "Other stateful
     * configuration" flag field in the Router Advertisement. This parameter
     * can be specified for each interface that are listed in the
     * RADVD_IFNAME_LIST in the form of "<ifparam>=<value>". Each pair of
     * "<ifparam>=<value>" is semicolon separate. Default: "0" (=FALSE).
     * E.g. "eth0=0" or "eth0=0;eth1=0".
     */
    { "ipnet.inet6.radvd", "AdvOtherConfigFlag",
       RADVD_IFLIST_ADV_OTHER_CONFIG_FLAG},

    /*
     * The value to be placed in MTU options sent by the router. A value of
     * zero indicates that no MTU options are sent, which means that the MTU
     * of the interface will be used by all hosts. This parameter can be
     * specified for each interface that are listed in the RADVD_IFNAME_LIST
     * in the form of "<ifparam>=<value>". Each pair of "<ifparam>=<value>"
     * is semicolon separated. Default: "0". E.g. "eth0=0" or "eth0=0;eth1=0".
     */
    {"ipnet.inet6.radvd", "AdvLinkMTU", RADVD_IFLIST_ADV_LINK_MTU},

    /*
     * The value to be placed in the Reachable Time field in the Router
     * Advertisement messages sent by the router. The value zero means
     * unspecified (by this router). MUST be no greater than 3,600,000
     * millisecon ds (1 hour). This parameter can be specified for each
     * interface that are listed in the RADVD_IFNAME_LIST in the form of
     * "<ifparam>=<value>". Each pair of "<ifparam>=<value>" is semicolon
     * separated. Default: "0". E.g. "eth0=0" or "eth0=0;eth1=0".
     */
    {"ipnet.inet6.radvd", "AdvReachableTime", RADVD_IFLIST_ADV_REACHABLE_TIME},

    /*
     * The value to be placed in the Retrans Timer field in the Router
     * Advertisement messages sent by the router. The value zero means
     * unspecified (by this router). This parameter can be specified for
     * each interface that are listed in the RADVD_IFNAME_LIST in the form of
     * "<ifparam>=<value>". Each pair of "<ifparam>=<value>" is semicolon
     * separated. Default: "0". E.g. "eth0=0" or "eth0=0;eth1=0".
     */
    {"ipnet.inet6.radvd", "AdvRetransTimer", RADVD_IFLIST_ADV_RETRANS_TIMER},

    /*
     * The default value to be placed in the Cur Hop Limit field in the
     * Router Advertisement messages sent by the router. The value should be
     * set to that current diameter of the Internet. The value zero means
     * unspecified (by this router). This parameter can be specified for
     * each interface that are listed in the RADVD_IFNAME_LIST in the form of
     * "<ifparam>=<value>". Each pair of "<ifparam>=<value>" is semicolon
     * separated. Default: "64". E.g. "eth0=64" or "eth0=64;eth1=64".
     */
    {"ipnet.inet6.radvd", "AdvCurHopLimit", RADVD_IFLIST_ADV_CUR_HOP_LIMIT},

    /*
     * The TRUE(=1) /FALSE(=0) value to be placed in the "Home Agent Flag"
     * flag field in the Router Advertisement. If this flag is set, it
     * indicates that the router in question also serves as a Home Agent.
     * This parameter can be specified for each interface that are listed
     * in the RADVD_IFNAME_LIST in the form of "<ifparam>=<value>". Each
     * pair of "<ifparam>=<value>" is semicolon separated. Default: "0"
     * (=FALSE).  E.g. "eth0=0" or "eth0=0;eth1=0".
     */
    { "ipnet.inet6.radvd", "AdvHomeAgentFlag", RADVD_IFLIST_ADV_HA_FLAG},

    /*
     * If TRUE(=1), the Router Advertisment daemon will append a Router
     * Advertisement Interval option to the router advertisement. This option
     * is specified in RFC 3775. This parameter can be specified for each
     * interface that are listed in the RADVD_IFNAME_LIST in the form of
     * "<ifparam>=<value>". Each pair of "<ifparam>=<value>" is semicolon
     * separated. Default: "0" (=FALSE). E.g. "eth0=0" or "eth0=0;eth1=0".
     */
    { "ipnet.inet6.radvd", "AdvIntervalOpt", RADVD_IFLIST_ADV_INTERVAL_OPT},

    /*
     * If TRUE(=1), the Router Advertisment daemon will append a home agent
     * information option to its router advertisement. This parameter can
     * be specified for each interface that are listed in the
     * RADVD_IFNAME_LIST in the form of "<ifparam>=<value>". Each pair of
     * "<ifparam>=<value>" is semicolon separated. Default: "0" (=FALSE).
     *  E.g. "eth0=0" or "eth0=0;eth1=0".
     */
    { "ipnet.inet6.radvd", "AdvHomeAgentOpt", RADVD_IFLIST_ADV_HA_OPT },

    /*
     * Specifies the Home Agents lifetime in milliseconds. This parameter
     * can be specified for each interface that are listed in the
     * RADVD_IFNAME_LIST in the form of "<ifparam>=<value>". Each pair of
     * "<ifparam>=<value>" is semicolon separated. Default: "1800000"
     * (RADVD_IFLIST_ADV_DEFAULT_LIFETIME). E.g. "eth0=1800000" or
     * "eth0=1800000;eth1=1800000".
     */
    { "ipnet.inet6.radvd", "AdvHomeAgentOptLifetime",
       RADVD_IFLIST_HA_OPT_LIFETIME},

    /*
     * Specifies the Home Agents preference. This parameter can be specified
     * for each interface that are listed in the RADVD_IFNAME_LIST in the
     * form of "<ifparam>=<value>". Each pair of "<ifparam>=<value>" is
     * semicolon separated. Default: "0".  E.g. "eth0=0" or "eth0=0;eth1=0".
     */
    { "ipnet.inet6.radvd", "AdvHomeAgentOptPreference",
        RADVD_IFLIST_HA_OPT_PREFERENCE},

    /*
     * The minimum time allowed (in seconds) between sending unsolicited
     * multicast Router Advertisements from the interface. MUST be no less
     * than 30 milliseconds and no greater than 75% of MaxRtrAdvInterval.
     * The interval limitations are set according to RFC3775. This parameter
     * can be specified for each interface that are listed in the
     * RADVD_IFNAME_LIST in the form of "<ifparam>=<value>". Each pair of
     * "<ifparam>=<value>" is semicolon separated. Default: "200000"
     * (MaxRtrAdvInterval / 3). E.g "eth0=200000" or "eth0=200000;eth1=200000".
     */
    { "ipnet.inet6.radvd", "MinRtrAdvInterval",
        RADVD_IFLIST_MIN_RTR_ADV_INTERVAL},

    /*
     * The maximum time (in seconds) allowed between sending unsolicited
     * multicast Router Advertisements from the  interface. MUST be no less
     * than 70 milliseconds and no greater than 1800000 seconds. This
     * parameter can be specified for each interface that are listed in the
     * RADVD_IFNAME_LIST in the form of "<ifparam>=<value>". Each pair of
     * "<ifparam>=<value>" is semicolon separated. Default: "600000".
     * E.g. "eth0=600000" or "eth0=600000;eth1=600000".
     */
    { "ipnet.inet6.radvd", "MaxRtrAdvInterval",
        RADVD_IFLIST_MAX_RTR_ADV_INTERVAL},

    /*
     * The min delay between solicited RA's. Will default to MinRtrAdvInterval
     * if not specified. This configuration variable is defined in RFC 3775
     * and can be specified for each interface that are listed in the the
     * RADVD_IFNAME_LIST in the form of "<ifparam>=<value>". Each pair of
     * "<ifparam>=<value>" is semicolon separated. Default: "200000"
     * (MinRtrAdvInterval). E.g. "eth0=200000" or "eth0=200000;eth1=200000".
     */
    { "ipnet.inet6.radvd", "MinDelayBetweenRAs",
        RADVD_IFLIST_MIN_DELAY_BETWEEN_RAS},

    /*
     * The value (in seconds) to be placed in the Router Lifetime field of
     * Router Advertisements sent from the interface. Must be either zero or
     * between 1000 and 9000000 milliseconds. A value of zero indicates that
     * the router is not to be used as a default router. This parameter can
     * be specified for each interface that are listed in the
     * RADVD_IFNAME_LIST in the form of "<ifparam>=<value>". Each pair of
     * "<ifparam>=<value>" is semicolon separated. Default: 3 *
     * MaxRtrAdvInterval  but at least 1000 milliseconds.
     * E.g. "eth0=1800000" or "eth0=1800000;eth1=1800000".
     */
    { "ipnet.inet6.radvd", "AdvDefaultLifetime",
        RADVD_IFLIST_ADV_DEFAULT_LIFETIME},

    /*
     * List of prefixes that should be added to router advertisement messages.
     * The elements are an identifier for the prefix, not the actual prefix.
     * This parameter can be specified for each interface that are listed in
     * the RADVD_IFNAME_LIST in the form of "<ifparam>=<value>". Each pair
     * of "<ifparam>=<value>" is semicolon separated. Default: Empty list.
     * E.g."eth0=6to4net" or "eth0=6to4net;eth1=devnet".
     */
    { "ipnet.inet6.radvd", "AdvPrefixList", RADVD_IFLIST_ADV_PREFIX},

    /*
     * Specifies the prefix and prefix length. The prefix length must be
     * between 1 and 127 and must be 64 for Ethernet if it should be used for
     * automatic address configuration. This parameter can be specified for
     * each interface that are listed in the RADVD_IFNAME_LIST in  the form
     * of "<ifparam>=<value>". Each pair of "<ifparam>=<value>" is semicolon
     * separated. Default: "" (not specified). E.g.
     * "eth0.prefix.6to4net=2002:0a01:0264::/64" or
     * "eth0.prefix.6to4net=2002:0a01:0264::/64;eth1.prefix.devnet=
     * 2002:a01:201:1::/64".
     */
    { "ipnet.inet6.radvd", IP_NULL, RADVD_IFLIST_PREFIX},

    /*
     * The value to be placed in the On-Link Flag field in the Prefix
     * Information option. This parameter can be specified for each interface
     * that are listed in the RADVD_IFNAME_LIST in the form of
     * "<ifparam>=<value>". Each pair of "<ifparam>=<value>" is semicolon
     * separated. Default: 1 (TRUE). E.g. "eth0.prefix.6to4net=1" or
     * "eth0.prefix.6to4net=1;eth1.prefix.devnet=1".
     */
    { "ipnet.inet6.radvd", "AdvOnLinkFlag",
       RADVD_IFLIST_PREFIX_ADV_ONLINK_FLAG},

    /*
     * The value to be placed in the Autonomus Flag field in the Prefix
     * Information option. This parameter can be specified for each interface
     * that are listed in the RADVD_IFNAME_LIST in the form of
     * "<ifparam>=<value>". Each pair of "<ifparam>=<value>" is semicolon
     * separated. Default: 1 (TRUE). E.g. "eth0.prefix.6to4net=1" or
     * "eth0.prefix.6to4net=1;eth1.prefix.devnet=1".
     */
    { "ipnet.inet6.radvd", "AdvAutonomousFlag",
       RADVD_IFLIST_PREFIX_ADV_AUTONOMOUS_FLAG},

    /*
     * The value to be placed in the Router Address flag field in the Prefix
     * Information option. This indicates that the specified prefix is also
     * a complete, global address for the router. This flag is specified in
     * RFC3775 and can be specified for each interface that are listed in the
     * RADVD_IFNAME_LIST in the form of "<ifparam>=<value>". Each pair of
     * "<ifparam>=<value>" is semicolon separated. Default: 0 (FALSE).
     * E.g. "eth0.prefix.6to4net=0" or
     * "eth0.prefix.6to4net=0;eth1.prefix.devnet=0".
     */
    { "ipnet.inet6.radvd", "AdvRouterAddressFlag",
      RADVD_IFLIST_PREFIX_ADV_RTR_ADDR_FLAG},

     /*
     * The value to be placed in the Valid Lifetime in the Prefix Information
     * option, in seconds. The designated value of all 1's ( -1) represents
     * infinity. This parameter can be specified for each interface that are
     * listed in the RADVD_IFNAME_LIST in the form of "<ifparam>=<value>".
     * Each pair of "<ifparam>=<value>" is semicolon separated. Default: "-1"
     * (infinite). E.g. "eth0.prefix.6to4net=-1" or
     * "eth0.prefix.6to4net=-1;eth1.prefix.devnet=-1".
     */
    { "ipnet.inet6.radvd", "AdvValidLifetime",
      RADVD_IFLIST_PREFIX_ADV_VALID_LIFETIME},

    /*
     * Enable/disable auto-decrementing valid lifetime Default: "0"
     * (no auto-decrement).
     * E.g. "eth0.prefix.6to4net=1"
     */
    { "ipnet.inet6.radvd", "AdvValidLifetimeDecrement",
      RADVD_IFLIST_PREFIX_ADV_VALID_LIFETIME_DECREMENT },

    /*
     * The value to be placed in the Preferred Lifetime in the Prefix
     * Information option, in seconds. The designated value of all 1' s ( -1)
     * represent s infinity. This parameter can be specified for each
     * interface that are listed in the RADVD_IFNAME_LIST in the form of
     * "<ifparam>=<value>". Each pair of "<ifparam>=<value>" is semicolon
     * separated. Default: "-1" (infinite). E.g. "eth0.prefix.6to4net=-1" or
     *  "eth0.prefix.6to4net=-1;eth1.prefix.devnet=-1".
     */
    { "ipnet.inet6.radvd", "AdvPreferredLifetime",
      RADVD_IFLIST_PREFIX_ADV_PREFERRED_LIFETIME},

    /*
     * Enable/disable auto-decrementing preferred lifetime Default: "0"
     * (no auto-decrement).
     * E.g. "eth0.prefix.6to4net=1"
     */
    { "ipnet.inet6.radvd", "AdvPreferredLifetimeDecrement",
      RADVD_IFLIST_PREFIX_ADV_PREFERRED_LIFETIME_DECREMENT },

    { IP_NULL, IP_NULL, IP_NULL }
};

/*
 ****************************************************************************
 * 8                   STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                  PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_radvd_configure
 *===========================================================================
 * Description: Configure IPv6 Router Advertisement Daemon.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipnet_radvd_configure(void)
{
#ifdef INCLUDE_IPRADVD_CMD
    (void)ipcom_shell_add_cmd("radvdconfig", "radvdconfig <if> ...",
                              "IPNET Router Advertisment daemon config",
                              ipnet_cmd_radvd,
                              IPCOM_SHELL_PRIO_SAME,
                              IPCOM_PROC_STACK_SMALL);
#endif /* INCLUDE_IPRADVD_CMD */

    if (ipcom_sysvar_setv(ipnet_radvd_conf_sysvar, 0) == IPCOM_SUCCESS)
    {
        /* now configure the interface-specific sysvars */
        return ipcom_sysvar_ext_setv (ipnet_radvd_conf_sysvar_ext, 0);
    }

    return IPCOM_ERR_INVALID_CONFIG;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
