/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipmipha_config.c,v $ $Revision: 1.4 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/ipmipha_config.c,v $
 *   $Author: jhorteli $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Niklas Lundberg <n@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_sysvar.h>


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

/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC int ipmipha_cmd_ha(int argc, char **argv);


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */
IP_PUBLIC Ip_err ipmipha_init(void);
IP_PUBLIC Ip_err ipmipha_exit(void);
IP_PUBLIC Ip_err ipmipha_configure(void);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         SYSVAR CONFIGURATION
 *===========================================================================
 */

#if defined(INCLUDE_IPMIPHA)

IP_STATIC IP_CONST Ipcom_sysvar ipmipha_conf_sysvar[] =
{
    /* Space-separated list of interfaces to use. e.g. "vlan0 eth1 eth2" */
	{ "ipmipha.interfaces", MIPHA_IFNAME_LIST},

    /* Registration lifetime in seconds. 0 = infinite. */
    { "ipmipha.reg_lifetime", MIPHA_REG_LIFETIME},

    /* Maximum number of seconds a registration request timestamp may differ
     * from the home agent's clock. Default is 7. 0 = no check is done.
     */
    { "ipmipha.timestampreplayprotectionmaxdiff", MIPHA_REPLAY_PROTECTION},

    /* NAI (Network Access Identifier) for this HA,
     * i.e. FQDN, Fully Qualified Domain Name
     */
    { "ipmipha.nai.ha", MIPHA_HA_NAI},

    /* Reverse lookup required? */
    { "ipmipha.fa.sa.lookup.required", MIPHA_FA_SA_LOOKUP_REQUIRE},
    { "ipmipha.mn.sa.lookup.required", MIPHA_MN_SA_LOOKUP_REQUIRE},

    /* Lookup order */
    { "ipmipha.mn.sa.lookup.order", MIPHA_MN_SA_LOOKUP_ORDER },

    /* Dynamic home address */
    { "ipmipha.mn.hoa.dynamic.required", MIPHA_MN_DYNAMIC_HOA_REQUIRED },

    /* Require NAI */
    { "ipmipha.mn.nai.required", MIPHA_MN_NAI_REQUIRED },


#ifdef INCLUDE_IPMIPHA_AAA_RADIUS
    { "ipmipha.aaa.radius.enable", "yes"},
    { "ipmipha.aaa.radius.access",                  MIPHA_AAA_RADIUS_ACCESS },
    { "ipmipha.aaa.radius.access.required",         MIPHA_AAA_RADIUS_ACCESS_REQUIRE },
    { "ipmipha.aaa.radius.access.address",          MIPHA_AAA_RADIUS_ACCESS_ADDRESS },
    { "ipmipha.aaa.radius.access.port",             MIPHA_AAA_RADIUS_ACCESS_PORT },
    { "ipmipha.aaa.radius.access.secret",           MIPHA_AAA_RADIUS_ACCESS_SECRET },
    { "ipmipha.aaa.radius.access.time_interval",    MIPHA_AAA_RADIUS_ACCESS_TIME_INTERVAL },

    { "ipmipha.aaa.radius.accounting",              MIPHA_AAA_RADIUS_ACCOUNTING },
    { "ipmipha.aaa.radius.accounting.address",      MIPHA_AAA_RADIUS_ACCOUNTING_ADDRESS },
    { "ipmipha.aaa.radius.accounting.port",         MIPHA_AAA_RADIUS_ACCOUNTING_PORT },
    { "ipmipha.aaa.radius.accounting.secret",       MIPHA_AAA_RADIUS_ACCOUNTING_SECRET },

    { "ipmipha.aaa.radius.nai",                     MIPHA_AAA_RADIUS_NAI },
    { "ipmipha.aaa.radius.local.address",           MIPHA_AAA_RADIUS_LOCAL_ADDRESS },
#else
    { "ipmipha.aaa.radius.enable", "no"},
#endif

#ifdef INCLUDE_IPMIPHA_AAA_DIAMETER
    { "ipmipha.aaa.diameter.enable", "yes"},
    { "ipmipha.aaa.diameter.access",                MIPHA_AAA_DIAMETER_ACCESS },
    { "ipmipha.aaa.diameter.access.required",       MIPHA_AAA_DIAMETER_ACCESS_REQUIRE },
    { "ipmipha.aaa.diameter.access.address",        MIPHA_AAA_DIAMETER_ACCESS_ADDRESS },
    { "ipmipha.aaa.diameter.access.port",           MIPHA_AAA_DIAMETER_ACCESS_PORT },
    { "ipmipha.aaa.diameter.access.time_interval",  MIPHA_AAA_DIAMETER_ACCESS_TIME_INTERVAL },
    { "ipmipha.aaa.diameter.access.realm",          MIPHA_AAA_DIAMETER_ACCESS_REALM },
    { "ipmipha.aaa.diameter.access.hostname",       MIPHA_AAA_DIAMETER_ACCESS_HOSTNAME },
#else
    { "ipmipha.aaa.diameter.enable", "no"},
#endif

    /*
     * Interface specific list of sysvar configurations are defined in
     * ipmipha_conf_sysvar_ext[]
     */

    /* End of configuration - DO NOT REMOVE! */
	{ IP_NULL, IP_NULL }
};

/* Interface specific list configurations */
IP_CONST Ipcom_sysvar_ext ipmipha_conf_sysvar_ext[] =
{
    /*
     * Security Parameter Index (SPI) list. Home agents can configure multiple
     * SPIs for use with many different home agents. Each SPI have exactly one
     * secret. The secret length may be up to 16 bytes and need not be
     * printable. The SPI is specifed in the form of "<SPI>=<value>", each
     * separated from the other by a semicolon. E.g. "1000=test0",
     * "1000=test0;1001=test1".
     */
    { "ipmipha.spi", "secret", MIPHA_SPI_LIST},

    /* Home Agent IP address. If set to "0", no RtAdv will be sent. */
    /* The IPv4 addresses of the home agent in the form of
     * "<ifname>=<address>", each separated from the other by a semicolon.
     * This address must be reachable from all foreign nets the mobile node
     * wishes to visit. If this value is set "0.0.0.0" for an interface,
     * the interface will not act home agent on the subnet it is configured
     * for, but only sniff incoming packets for possible forwarding to
     * proxied mobile nodes. This is typically useful if the home agent is
     * being run on a router. E.g., "eth0=0.0.0.0".
     */
    { "ipmipha.if", "homeagent", MIPHA_IF_HOME_ADDRESS_LIST},

    /*
     * Foreign - Home authentication.
     * List of SPIs to use between FA and HA. This parameter can be specified
     * for each FA in the form of "<FA address>=<SPI>", each separated from
     * the other by a semicolon. Set "<FA address>" to "any" to enable
     * foreign-home authentication to all foreign agents. If the FA IP address
     * is not found and no "any" entry exists, no foreign-home authentication
     * will be used. For example, setting "10.1.2.42=1002" enables
     * foreign-home authentication using SPI 1002 between this FA and HA
     * using IP Address 10.1.2.42. Setting "any=1002" enables foreign-home
     * authentication to all foreign agents using SPI 1002.
     */
    { "ipmipha.fa.sa.address", IP_NULL, MIPHA_FA_SA_ADDRESS_LIST},

    /*
     * Security associations to use between MN and HA. This parameter
	 * is specified for each MN in the form of "<MN address>[/<prefix>]=<SPI>",
     * each separated from the other by a semicolon. Set "<MN address>" to
     * "any" to enable mobile-home authentication to all mobile nodes. If the
     * MN IP address is not found and no "any" entry exists, no communication
     * will be valid if the lookup require primitive is specified and no AAA
     * key generation mechanisms have been used. For example, setting
     * "10.1.2.42=1002" enables mobile-home authentication using SPI 1002
     * between this MN and HA using Home Address 10.1.2.42. Setting "any=1002"
     * enables mobile-home authentication to all mobile nodes using SPI 1002.
     * Setting "10.1.2.0/24=1004" indicates that all mobile nodes in the
     * 10.1.2.0/24 network should be using SPI 1004.
     */
    { "ipmipha.mn.sa.address", IP_NULL, MIPHA_MN_SA_ADDRESS_LIST},

    /*
     * Security associations to use between MN and HA. This parameter
     * is specified for each MN in the form of "user@domain=<SPI>" or
     * "@domain=<SPI>" each separated from the other by a semicolon.For
     * example,setting "@windriver.com=1002" enables mobile-home authentication
     * using SPI 1002 between a mobile node using a NAI in the windriver.com
     * domain.
     */
    { "ipmipha.mn.sa.nai", IP_NULL, MIPHA_MN_SA_NAI_LIST},

    /* NAI to HoA mapping */
    { "ipmipha.mn.nai", "hoa.address", MIPHA_MN_NAI_HOA_ADDRESS_LIST},

    /* Home netmask. */
    /* The home agent's IPv4 netmask(s) in the form of "<ifname>=<mask>",
     * each separated from the other by a semicolon. E.g. "eth0=255.255.0.0".
     */
    { "ipmipha.if", "homenetmask", MIPHA_IF_HOME_MASK_LIST},

    /* Specifies if Foreign Agent to Home Agent security association is
     * required or not. Enabled(1), Disabled(0). This parameter is
     * configurable on a per-interface basis in the form of
     * "<ifname>=<value>", each separated from the other by a semicolon.
     * E.g. "eth0=1".
     */
    { "ipmipha.if", "fa_auth", MIPHA_IF_FA_AUTH_LIST},

    /* RtAdv interval in seconds. */
    /* The interval at which the home agent will send ICMP router
     * advertisements on this interface specified in the form of
     * "<ifname>=<value>", each separated from the other by a semicolon. If
     * set to 0, the home agent will not send any router advertisements, also,
     * none of the other adv_ sysvars need to be set. The default value of
     * 3 is good for testing purposes, whereas the RFC suggests about a
     * value of a third of the ICMP router advertisement's lifetime. This
     * value can be set high if mobile nodes are configured to use
     * solicitations upon entering new subnets. E.g. "eth0=10".
     */
    { "ipmipha.if", "adv_interval", MIPHA_IF_ADV_INTERVAL_LIST},

    /* RtAdv destination IP address. */
    /* The destination address of outgoing router advertisement messages
     * from the home agent. This parameter is configurable on a per-interface
     * basis in the form of "<ifname>=<value>". Each pair of
     * "<ifname>=<IP Address>" is semicolon separated. E.g. "eth0=224.0.0.11".
     */
    { "ipmipha.if", "adv_address", MIPHA_IF_ADV_ADDRESS_LIST},

    /* RtAdv lifetime in seconds. */
    /* The lifetime of advertisement messages from the home agent, in seconds.
     * This parameter is configurable on a per-interface basis in the form of
     * "<ifname>=<value>", each separated from the other by a semicolon.
     * E.g. "eth0=300".
     */
    { "ipmipha.if", "adv_lifetime", MIPHA_IF_ADV_LIFETIME_LIST},

    /* NAT-T; mode is 0: disabled, 1: enabled  */
    /* Enables or disables NAT Traversal according to RFC3519. This parameter
     * is configurable on a per-interface basis in the form of
     * "<ifname>=<value>", each separated from the other by a semicolon.
     * E.g. "eth0=1".
     */
    { "ipmipha.if", "nat_t.enabled", MIPHA_IF_NAT_T_ENABLED_LIST},

    /* NAT-T; forced keepalive; 0, use node's configured values. */
    /* Decides the keepalive timeout (in seconds) to use for NAT Traversal
     * ICMP keepalive messages in order to maintain a NAT device's UDP port
     * mapping. This parameter is configurable on a per-interface basis in the
     * form of "<ifname>=<timeout>", each separated from the other by a
     * semicolon. E.g. "eth0=120".
     */
    { "ipmipha.if", "nat_t.keepalive",  MIPHA_IF_NAT_T_KEEPALIVE_LIST},


    /* Simultaneous bindings support */
    { "ipmipha.if", "simbind.enable",    MIPHA_IF_SIMBIND_ENABLED_LIST },
    { "ipmipha.if", "simbind.max",       MIPHA_IF_SIMBIND_MAX_LIST },

    /*
     * Interface Tunneling Reordering
     * Enable packet reordering to guarantee in-order delivery. Currently
     * only supported by GRE by use of sequence numbers.
     */
    { "ipmipha.if", "tunnel.reordering", MIPHA_IF_TUNNEL_REORDERING_LIST},

    /*
     * Interface Reverse Tunneling
     * Disabled(0)/Optional(1)/Required(2) usage of reverse tunneling. If
     * it's disabled, any node trying to request reverse tunneling will be
     * disallowed. If optional, the node itself will be allowed to decide.
     * If required the reverse tunneling bit must be set. This parameter
     * is configurable on a per-interface basis in the form of
     * "<ifname>=<value>", each separated from the other by a semicolon.
     * E.g. "eth0=1"
     */
    { "ipmipha.if", "tunnel.reverse", MIPHA_IF_TUNNEL_REVERSE_LIST},


    /* Interface IPIP tunneling
     * Enable(1)/Disable(0) usage of IPIP tunneling for an interface. This
     * parameter is configurable on a per-interface basis in the form of
     * "<ifname>=<value>", each separated from the other by a semicolon.
     * E.g. "eth0=1".
     */
    { "ipmipha.if", "tunnel.ipip", MIPHA_IF_IPIP_TUNNEL_ENABLED_LIST},

    /* Interface GRE tunneling
     * Enable(1)/Disable(0) usage of GRE tunneling for an interface. This
     * parameter is configurable on a per-interface basis in the form of
     * "<ifname>=<value>", each separated from the other by a semicolon.
     * E.g. "eth0=1".
     */
    { "ipmipha.if", "tunnel.gre", MIPHA_IF_GRE_TUNNEL_ENABLED_LIST},

    /* Interface MIN Encap tunneling
     * Enable(1)/Disable(0) usage of Minimal encapsulation tunneling for an
     * interface. This parameter is configurable on a per-interface basis
     * in the form of "<ifname>=<value>", each separated from the other by a
     * semicolon. E.g. "eth0=1".
     */
    { "ipmipha.if", "tunnel.minenc", MIPHA_IF_MINENC_TUNNEL_ENABLED_LIST},

    /* Revocation support     */
    { "ipmipha.if", "revocation", MIPHA_IF_REVOCATION_ENABLED_LIST},

    /* End of configuration - DO NOT REMOVE! */
	{ IP_NULL, IP_NULL, IP_NULL }
};

#else /* WRS_IPNET */

#define INCLUDE_IPMIPHA_CMD

IP_STATIC IP_CONST Ipcom_sysvar ipmipha_conf_sysvar[] =
{
    /* End of configuration - DO NOT REMOVE! */
	{ IP_NULL, IP_NULL }
};

#if defined(IPTESTENGINE)
#   define INCLUDE_IPMIPHA_FEATURE_EXTENSION   1
#   define MIPHA_FEATURE_INITIALIZATION_HOOK   ipmipha_regext_test_install
#endif

#define INCLUDE_IPMIPHA_AAA_RADIUS
#define INCLUDE_IPMIPHA_AAA_DIAMETER 

#endif /* WRS_IPNET */

#if defined(INCLUDE_IPMIPHA_FEATURE_EXTENSION)
extern void MIPHA_FEATURE_INITIALIZATION_HOOK(void);
#endif

#ifdef INCLUDE_IPMIPHA_AAA_RADIUS
IP_GLOBAL void ipmipha_aaa_radius_install(void);
#endif

#ifdef INCLUDE_IPMIPHA_AAA_DIAMETER
IP_GLOBAL void ipmipha_aaa_diameter_install(void);
#endif

IP_GLOBAL void ipmipha_regext_dhaa_install(void);
IP_GLOBAL void ipmipha_regext_hoa_install(void);


typedef void (*Ipmipha_config_func_t) (void);
Ipmipha_config_func_t      ipmipha_function_registrations[] =
{
#ifdef INCLUDE_IPMIPHA_AAA_RADIUS
    ipmipha_aaa_radius_install,
#endif    
#ifdef INCLUDE_IPMIPHA_AAA_DIAMETER
    ipmipha_aaa_diameter_install,
#endif    
#if defined(INCLUDE_IPMIPHA_FEATURE_EXTENSION)
    MIPHA_FEATURE_INITIALIZATION_HOOK,
#endif

    ipmipha_regext_dhaa_install,
    ipmipha_regext_hoa_install,

    IP_NULL
};

/*
 ****************************************************************************
 * 9                    PUBLIC FUNCTIONS
 ****************************************************************************
 */



/*
 *===========================================================================
 *                    ipmipha_configure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipmipha_configure(void)
{
    Ip_err err;

    /* Initialize IPMIPHA system variables. */
    /* Initialize IPMIPHA system variables. */
    err = ipcom_sysvar_setv(ipmipha_conf_sysvar, 0);
    if (err != IPCOM_SUCCESS)
        return err;

#if defined(WRS_IPNET) || defined(PRJ_BUILD)
    /* now configure the interface-specific sysvars */
    err = ipcom_sysvar_ext_setv(ipmipha_conf_sysvar_ext, 0);
    if (err != IPCOM_SUCCESS)
        return err;
#endif

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipmipha_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipmipha_init(void)
{
#if defined(INCLUDE_IPMIPHA_CMD) && !defined(IP_PORT_LAS)
    (void)ipcom_shell_add_cmd("ha", "ha nodes", "Mobile IP Home Agent",
                        ipmipha_cmd_ha,
                        IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_SMALL);
#endif

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipmipha_exit
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipmipha_exit(void)
{
#if defined(INCLUDE_IPMIPHA_CMD) && !defined(IP_PORT_LAS)
    (void)ipcom_shell_remove_cmd("ha", ipmipha_cmd_ha);
#endif
    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

