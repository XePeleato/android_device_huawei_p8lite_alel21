/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipdhcpc_config.c,v $ $Revision: 1.8 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/ipdhcpc_config.c,v $
 *   $Author: gzadoyan $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 ******************************************************************************
 */

/*
modification history
--------------------
01h,04jul09,ggz  Added DHCPC ip address lease time
01g,11oct07,uol  Defect WIND00107491, add DHCP capabilities in bootrom.
01f,22jan07,kch  Added rfc2131_init_delay, rfc2131_exponential_backoff,
                 discover_retries and offer_timeout sysvars (WIND00086146).
01e,03jan07,kch  Renamed DHCPC_IFLIST_REQ_OPTS and DHCPC_IFLIST_CLIENT_ID
                 macros to DHCPC_IF_REQ_OPTS_LIST and DHCPC_IF_CLIENT_ID_LIST.
01d,01dec06,kch  Updated CVS revision log entry.
01c,13sep06,kch  Refresh with Sept 08, 2006 code drop
01b,28aug06,kch  Update based on Aug 22, 2006 code drop.
01a,07aug06,kch  Added ipdhcpc_conf_sysvar_ext[] to support per-interface
                 sysvar configurations.
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
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_sysvar.h>


/*
 ****************************************************************************
 * 3                    DEFINES
 ****************************************************************************
 */
#if defined(BOOTAPP_DHCPC_REQ_OPTS)
#undef DHCPC_REQ_OPTS
#define DHCPC_REQ_OPTS BOOTAPP_DHCPC_REQ_OPTS
#endif

#if defined(BOOTAPP_DHCPC_IF_REQ_OPTS_LIST)
#undef DHCPC_IF_REQ_OPTS_LIST
#define DHCPC_IF_REQ_OPTS_LIST BOOTAPP_DHCPC_IF_REQ_OPTS_LIST
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

/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipdhcpc_configure(void);


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
 *                         ipdhcpc_conf_sysvar
 *===========================================================================
 * DHCP Client Configuration Variables
 */

IP_CONST Ipcom_sysvar ipdhcpc_conf_sysvar[] =
{
	/* DHCP client port (default 68) */
	{"ipdhcpc.client_port", DHCPC_CLIENT_PORT},

	/* DHCP server port (default 67) */
	{"ipdhcpc.server_port", DHCPC_SERVER_PORT},

    /* RFC2131 Initialization Delay identifier                  */
    /* 1 enabled          , 0 disabled                          */
    {"ipdhcpc.rfc2131_init_delay", DHCPC_RFC2131_INIT_DELAY},

    /* RFC2131 Exponential Back-off Delay                       */
    /* 1 enabled, 0 disabled                                    */
    {"ipdhcpc.rfc2131_exponential_backoff", DHCPC_RFC2131_EXP_BACKOFF},

    /* Number of DHCP client retries                            */
    {"ipdhcpc.discover_retries", DHCPC_DISCOVER_RETRIES},

    /* DHCP offer time-out in milliseconds                      */
    {"ipdhcpc.offer_timeout", DHCPC_OFFER_TIMEOUT},
    
    /* DHCP bootp mode                                          */
    /* 1 enabled, 0 disabled                                    */
    {"ipdhcpc.bootp_mode", DHCPC_BOOTP_MODE},

    /* DHCP boot filename                                       */
    {"ipdhcpc.boot_filename", DHCPC_BOOT_FILENAME},    
    
    /* Global server hostname                                   */
    {"ipdhcpc.server_hostname", DHCPC_SERVER_HOSTNAME},    
    
    /* Global hostname option                                   */
    {"ipdhcpc.option.hostname", DHCPC_OPTION_HOSTNAME}, 
    
    /* Global netmask option                                    */
    {"ipdhcpc.option.netmask", DHCPC_OPTION_NETMASK}, 
    
    /* IP address lease time option                             */
    {"ipdhcpc.option.lease_time", DHCPC_OPTION_IPADDRESS_LEASE_TIME}, 

    /* Global vendor class identifier option                    */
    {"ipdhcpc.option.vendor_class_id", DHCPC_OPTION_VENDOR_CLASS_ID}, 
    
    /* Global vendor identifying vendor options option          */
    {"ipdhcpc.option.vendor_identifying_vendor_class", DHCPC_OPTION_VI_VENDOR_CLASS}, 
    
	/* Global list of requested DHCP options  */
	{"ipdhcpc.option.requested_options", DHCPC_REQ_OPTS},

	/* Global client identifier */
	{"ipdhcpc.option.client_identifier", DHCPC_CLIENT_ID},
	
	/* Global user specified options */
	{"ipdhcpc.option.user_specified", DHCPC_OPTION_USER_SPECIFIED},
	
	/* Remove existing unicast addresses before negotiation begins */
	{"ipdhcpc.remove_existing_addresses", DHCPC_REMOVE_EXISTING_ADDRESSES},
	
	/* Remove negotiated addresses from interface when lease expires */
	{"ipdhcpc.remove_leased_address", DHCPC_REMOVE_LEASED_ADDRESS},
	
	/* Disable DHCP on the interface if a static address is manually applied */
	{"ipdhcpc.auto_disable", DHCPC_AUTO_DISABLE},
	
	/* Global DHCP maximum message size */
	{"ipdhcpc.option.max_message_size", DHCPC_OPTION_MAX_MESSAGE_SIZE},
	
	/* Global Time To Live (TTL) for DHCP packets */
	{"ipdhcpc.ttl", DHCPC_TTL},
    
    /*
     * Interface specific list of sysvar configurations are defined in
     * ipdhcpc_conf_sysvar_ext[]
     */
    { IP_NULL, IP_NULL }
};

/* Interface specific list configurations */
IP_CONST Ipcom_sysvar_ext ipdhcpc_conf_sysvar_ext[] =
{
    /* 
     * Interface specific list of requested DHCP options with interface name
     * followed by comma separated list of DHCP options numbers that the
     * DHCP client wants the server to provide in the format <ifparam>=<opts>.
     * Each pair of <ifparam>=<opts> is semicolon separated, e.g.
     * "eth0=1,2,3,4;eth1=2,3,4,5".
     */
    {"ipdhcpc", "option.requested_options", DHCPC_IF_REQ_OPTS_LIST},
    
    /* 
     * Interface specific client identifier with a string of characters that
     * the DHCP client wants the server to identify it by in the format
     * <ifparam>=<id>. Each pair of <ifparam>=<id> is semicolon separated, e.g.
     * "eth0=my_red_computer_eth0" or 
     * "eth0=my_red_computer_eth0;eth1=my_blue_computer_eth1".
     */
    {"ipdhcpc", "option.client_identifier", DHCPC_IF_CLIENT_ID_LIST},
    
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
 *                    ipdhcpc_configure
 *===========================================================================
 * Description: Configure DHCP client.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipdhcpc_configure(void)
{
    if (ipcom_sysvar_setv(ipdhcpc_conf_sysvar, 0) == IPCOM_SUCCESS)
    {
        /* now configure the interface-specific sysvars */
        return ipcom_sysvar_ext_setv (ipdhcpc_conf_sysvar_ext, 0);
    }

    return IPCOM_ERR_INVALID_CONFIG;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
