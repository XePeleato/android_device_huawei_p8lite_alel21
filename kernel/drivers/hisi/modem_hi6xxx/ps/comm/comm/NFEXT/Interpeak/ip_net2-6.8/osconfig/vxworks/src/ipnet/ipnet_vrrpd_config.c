/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_vrrpd_config.c,v $ $Revision: 1.3 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/ipnet_vrrpd_config.c,v $
 *   $Author: n $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */

/*
modification history
--------------------
01e,03jan07,kch  Updated CVS revision log entry for Jan 03 code drop.
01d,01dec06,kch  Updated CVS revision log entry.
01c,13sep06,kch  Refresh with Sept 08, 2006 code drop
01b,28aug06,kch  Update based on Aug 22, 2006 code drop.
01a,12aug06,kch  Modified to use configuration macros. Also added
                 ipnet_vrrpd_conf_sysvar_ext[] to support per-interface
                 sysvar configurations.
*/

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Configuration for the IPNET VRRP deamon.
 * The system variables have base names that matches the names in RFC3768
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

/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipnet_vrrpd_configure(void);


/*
 ****************************************************************************
 * 7                    DATA
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         ipnet_vrrp_conf_sysvar
 *===========================================================================
 * Router Advertisment Daemon Configuration Variables
 */
IP_CONST Ipcom_sysvar ipnet_vrrpd_conf_sysvar[] =
{
    /*
     * List of interfaces where this router should do VRRP operation. The
     * format is a space-separated list of interface names, e.g. "eth0 eth1".
     * Default: empty list (i.e. the daemon will just shutdown if it is
     * started).
     */
    { "ipnet.vrrp.interfaces", VRRP_IFNAME_LIST},

    /*
     * Interface specific list of sysvar configurations are defined in
     * ipnet_vrrpd_conf_sysvar_ext[]
     */
	{ IP_NULL, IP_NULL }
};

/* Interface specific list configurations */
IP_CONST Ipcom_sysvar_ext ipnet_vrrpd_conf_sysvar_ext[] =
{
    /*
     * List of virtual router identifiers defined on this interface. Must
     * be >= 1 and >= 255. The vrids list is specified using the format
     * <ifparam>=<value>. Each pair of <ifparam>=<value> is semicolon
     * separated, e.g. "eth0=1;eth1=2".
     */
    {"ipnet.vrrp", "vrids", VRRP_IFLIST_VRIDS},

    /*
     * Priority value to be used in Master election for this virtual router.
     * 1-254 is available for VRRP routers backing up the virtual router.
     * 255 means that this router owns the address and start as MASTER
     * Default value is 100. The priority list is specified using the format
     * <ifparam>=<value>. Each pair of <ifparam>=<value> is semicolon
     * separated, e.g. "eth0.1=100;eth1.2=255".
     */
    { "ipnet.vrrp", "Priority", VRRP_IFLIST_VRIDS_PRIORITY},

    /*
     * One or more IP addresses associated with this virtual router for each
     * interface/VRID pair. The IP Address list is specified using the format
     * <ifparam>=<value>. Each pair of <ifparam>=<value> is semicolon
     * separated, e.g. "eth0.1=10.130.2.254;eth1.2=10.130.3.254"
     */
    { "ipnet.vrrp", "IP_Addresses", VRRP_IFLIST_VRIDS_IPADDR},

    /*
     * Time interval (in seconds) between VRRP advertisement messages for each
     * interface/VRID pair. Default: 1. The advertisement interval list is
     * specified using the format <ifparam>=<value>. Each pair of
     * <ifparam>=<value> is semicolon separated, e.g. "eth0.1=1;eth1.2=1".
     */
    { "ipnet.vrrp", "Advertisement_Interval", VRRP_IFLIST_VRIDS_ADV_INTERVAL},

    /*
     * Controls whether a higher priority Backup router preempts a lower
     * priority master for each interface/VRID pair. Default: 1 (i.e. true).
     * The preempt mode list is specified using the format <ifparam>=<value>.
     * Each pair of <ifparam>=<value> is semicolon separated, e.g.
     * "eth0.1=1;eth1.2=1".
     */
    { "ipnet.vrrp", "Preempt_Mode", VRRP_IFLIST_VRIDS_PREEMPT_MODE},

    /*
     * Simple text password authentication. Default=0 is no authentication.
     */
    { "ipnet.vrrp", "Authentication_Type", VRRP_AUTH_TYPE },
    { "ipnet.vrrp", "Authentication_Data", VRRP_AUTH_DATA },

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
 *                    ipnet_vrrpd_configure
 *===========================================================================
 * Description: Configure the IPNET VRRP deamon.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipnet_vrrpd_configure(void)
{
	if (ipcom_sysvar_setv(ipnet_vrrpd_conf_sysvar, 0) == IPCOM_SUCCESS)
    {
        /* now configure the interface-specific sysvars */
        return ipcom_sysvar_ext_setv (ipnet_vrrpd_conf_sysvar_ext, 0);
    }

    return IPCOM_ERR_INVALID_CONFIG;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
