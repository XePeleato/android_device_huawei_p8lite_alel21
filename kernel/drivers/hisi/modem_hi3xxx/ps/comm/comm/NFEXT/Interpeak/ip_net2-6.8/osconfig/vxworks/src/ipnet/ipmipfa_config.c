/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipmipfa_config.c,v $ $Revision: 1.7 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/ipmipfa_config.c,v $
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
typedef void (*Ipmipfa_config_func_t) (void);

/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC int ipmipfa_cmd_fa(int argc, char **argv);


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipmipfa_configure(void);
IP_PUBLIC Ip_err ipmipfa_init(void);
IP_PUBLIC Ip_err ipmipfa_exit(void);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

#if defined(INCLUDE_IPMIPFA)

/*
 *===========================================================================
 *                         SYSVAR CONFIGURATION
 *===========================================================================
 */

IP_STATIC IP_CONST Ipcom_sysvar ipmipfa_conf_sysvar[] =
{
    /* Space-separated list of interfaces to use. e.g. "eth0 eth1 eth2" */
    { "ipmipfa.interfaces", MIPFA_IFNAME_LIST},

    /* Registration lifetime in seconds. 0 = infinite. */
    { "ipmipfa.reg_lifetime", MIPFA_REG_LIFETIME},

    /* Reverse lookup required? */
    { "ipmipfa.ha.sa.lookup.required", MIPFA_HA_SA_LOOKUP_REQUIRE},
    /*
     * Interface specific list of sysvar configurations are defined in
     * ipmipfa_conf_sysvar_ext[]
     */

#ifdef INCLUDE_IPMIPFA_AAA_RADIUS
    { "ipmipfa.aaa.radius.enable", "yes"},
    { "ipmipfa.aaa.radius.access",                  MIPFA_AAA_RADIUS_ACCESS },
    { "ipmipfa.aaa.radius.access.required",         MIPFA_AAA_RADIUS_ACCESS_REQUIRE },
    { "ipmipfa.aaa.radius.access.address",          MIPFA_AAA_RADIUS_ACCESS_ADDRESS },
    { "ipmipfa.aaa.radius.access.port",             MIPFA_AAA_RADIUS_ACCESS_PORT },
    { "ipmipfa.aaa.radius.access.secret",           MIPFA_AAA_RADIUS_ACCESS_SECRET },
    { "ipmipfa.aaa.radius.access.time_interval",    MIPFA_AAA_RADIUS_ACCESS_TIME_INTERVAL },

    { "ipmipfa.aaa.radius.accounting",              MIPFA_AAA_RADIUS_ACCOUNTING },
    { "ipmipfa.aaa.radius.accounting.address",      MIPFA_AAA_RADIUS_ACCOUNTING_ADDRESS },
    { "ipmipfa.aaa.radius.accounting.port",         MIPFA_AAA_RADIUS_ACCOUNTING_PORT },
    { "ipmipfa.aaa.radius.accounting.secret",       MIPFA_AAA_RADIUS_ACCOUNTING_SECRET },

    { "ipmipfa.aaa.radius.local.address",           MIPFA_AAA_RADIUS_LOCAL_ADDRESS },
#else
    { "ipmipfa.aaa.radius.enable", "no"},
#endif

#ifdef INCLUDE_IPMIPFA_AAA_DIAMETER
    { "ipmipfa.aaa.diameter.enable", "yes"},
    { "ipmipfa.aaa.diameter.access",                MIPFA_AAA_DIAMETER_ACCESS },
    { "ipmipfa.aaa.diameter.access.required",       MIPFA_AAA_DIAMETER_ACCESS_REQUIRE },
    { "ipmipfa.aaa.diameter.access.address",        MIPFA_AAA_DIAMETER_ACCESS_ADDRESS },
    { "ipmipfa.aaa.diameter.access.port",           MIPFA_AAA_DIAMETER_ACCESS_PORT },
    { "ipmipfa.aaa.diameter.access.time_interval",  MIPFA_AAA_DIAMETER_ACCESS_TIME_INTERVAL },
    { "ipmipfa.aaa.diameter.access.realm",          MIPFA_AAA_DIAMETER_ACCESS_REALM },
    { "ipmipfa.aaa.diameter.access.hostname",       MIPFA_AAA_DIAMETER_ACCESS_HOSTNAME },
    { "ipmipfa.aaa.diameter.access.fa_ha_spi",      MIPFA_AAA_DIAMETER_ACCESS_FA_HA_SPI },
#else
    { "ipmipfa.aaa.diameter.enable", "no"},
#endif

#ifdef INCLUDE_IPMIPFA_PMIPV4
    { "ipmipfa.pmipv4.enable", "yes"},
#else
    { "ipmipfa.pmipv4.enable", "no"},
#endif

#ifdef INCLUDE_IPMIPFA_PMIPV4_DHCP
    { "ipmipfa.pmipv4.dhcp.enable", "yes"},
#else
    { "ipmipfa.pmipv4.dhcp.enable", "no"},
#endif

    /* End of configuration - DO NOT REMOVE! */
	{ IP_NULL, IP_NULL }
};

/* Interface specific list configurations */
IP_CONST Ipcom_sysvar_ext ipmipfa_conf_sysvar_ext[] =
{
    /*
     * Foreign - Home authentication.
     * List of SPIs to use between FA and HA. This parameter can be specified
     * for each HA in the form of "<HA address>=<SPI>", each separated from
     * the other by a semicolon. Set "<HA address>" to "any" to enable
     * foreign-home authentication to all home agents. If the HA IP address
     * is not found and no "any" entry exists, no foreign-home authentication
     * will be used. For example, setting "10.1.2.42=1002" enables
     * foreign-home authentication using SPI 1002 between this FA and HA
     * using IP Address 10.1.2.42. Setting "any=1002" enables foreign-home
     * authentication to all home agents using SPI 1002.
     */
    { "ipmipfa.ha.sa.address", IP_NULL, MIPFA_HA_SA_ADDRESS_LIST},

    /*
     * Foreign - Foreign authentication.
     * List of SPIs to use between FA and FA. This parameter can be
     * specified for each FA in the form of "<FA address>=<SPI>", each
     * separated from the other by a semicolon. Set "<FA address>" to
     * "any" to enable foreign-foreign authentication to all foreign agents.
     * If the FA IP address is not found and no "any" entry exists, no
     * foreign-foreign authentication will be used. For example, setting
     * "10.1.2.42=1002" enable foreign-foreign authentication using SPI
     * 1002 between this FA and FA using IP Address 10.1.2.42. Setting
     * "any=1002" enables foreign-foreign authentication to all foreign
     * agents using SPI 1002.
     */
    { "ipmipfa.fa.sa.address", IP_NULL, MIPFA_FA_SA_ADDRESS_LIST},


    /*
     * Security Parameter Index (SPI) list. Foreign agents can configure
     * multiple SPIs for use with many different home agents. Each SPI have
     * exactly one secret. The secret length may be up to 16 bytes and need
     * not be printable. The SPI is specifed in the form of "<SPI>=<value>",
     * each separated from the other by a semicolon. E.g. "1000=test0",
     * "1000=test0;1001=test1".
     */
    { "ipmipfa.spi", "secret", MIPFA_SPI_SECRET_LIST},

    /*
     * LLH Neighbor specification. Specifies the neighbors and their
     * corresponding solicitation/advertisement timeout values.
     * This parameter is configurable on network or address basis.
     * Multiple neighbors may be specified, separated by a semi-colon.
     * E.g. 10.1.2.45=10/20;10.2.2.0/24=20/40
     */
    { "ipmipfa.llh.neighbor.ip", IP_NULL, MIPFA_LLH_NBR_IP_LIST },

    /* List of LLH neighbor BSSID's known to this foreign agent. Multiple
     * neighbors may be specified separated by a semicolon, and for
     * each neighbor you must specify IP address this particular BSSID
     * corresponds to. <bssid>=<ip address>. E.g."1111=10.1.2.45;1112=10.2.2.1"
     */
    { "ipmipfa.llh.neighbor.ap", IP_NULL, MIPFA_LLH_NBR_BSSID_LIST },

    /*
     * Enable pre-registration
     * Enables(1) or Disables(0) PRE-REGISTRATION Low Latency Handoffs for an
     * interface. This parameter is configurable on a per-interface basis
     * in the form of "<ifname>=<value>", each separated from the other by a
     * semicolon. E.g. "eth0=1".
     */
    { "ipmipfa.if", "llh.pre.enable", MIPFA_IF_LLH_PRE_ENABLED_LIST},

    /*
     * Enable post-registration
     * Enables(1) or Disables(0) POST-REGISTRATION Low Latency Handoffs for
     * an interface. This parameter is configurable on a per-interface
     * basis in the form of "<ifname>=<value>", each separated from the other
     * by a semicolon. E.g. "eth0=0".
     */
    { "ipmipfa.if", "llh.post.enable", MIPFA_IF_LLH_POST_ENABLED_LIST},

    /*
     * BET registration lifetime
     * Specifies the default value of the BET lifetime when POST registration
     * is in effect. The BET lifetime is the value the FA will try to
     * negotiate for the BET tunnel, and it is also used as the default value
     * when the tunnel lifetime is being extended. This parameter can be
     * configured on a per-interface basis in the form of "<ifname>=<value>",
     * each separated from the other by a semicolon. E.g. "eth0=5"
     */
    { "ipmipfa.if", "llh.post.bet_lifetime", MIPFA_IF_LLH_BET_LIFETIME_LIST},

    /*
     * Interface FA address in the form of "<ifname>=<address>", each
     * separated from the other by a semicolon. Set to "0.0.0.0" to
     * read IPv4 address at boot. If set to "255.255.255.255", no RtAdv will
     * be sent but registration replies will be processed if matching a sent
     * request. E.g., "eth0=0.0.0.0"
     */
    { "ipmipfa.if", "foreignagent", MIPFA_IF_ADDRESS_LIST},

    /*
     * Interface Mobile Node authentication
     * Enable(1)/Disable(0) forcing of mobile nodes to use Mobile-Foreign
     * authentication. This parameter is configurable on a per-interface
     * basis in the form of "<ifname>=<value>", each separated from the other
     * by a semicolon. E.g. "eth0=0".
     */
    { "ipmipfa.if", "mn_auth",  MIPFA_IF_MN_AUTH_ENABLED_LIST},

    /*
     * RtAdv interval in seconds.
     * The interval at which the agent will send router advertisements on this
     * interface specified in the form of "<ifname>=<value>", each separated
     * from the other by a semicolon. If set to 0, the agent will not send
     * any router advertisements, also, none of the other adv_ sysvars need
     * to be set. The default value of 10 is good for testing purposes whereas
     * the RFC suggests about a value of a third of the ICMP router
     * advertisement's lifetime). This value can be set to 0 or high if mobile
     * nodes have other means of detecting entry to new subnets, such as
     * wireless AP indication. E.g. "eth0=10"
     */
    { "ipmipfa.if", "adv_interval", MIPFA_IF_ADV_INTERVAL_LIST},

    /*
     * RtAdv destination IP address.
     * Specifies the destination address of outgoing router advertisement
     * messages from the agent. This parameter is configurable on a
     * per-interface basis in the form of "<ifname>=<value>", each separated
     * from the other by a semicolon. E.g. "eth0=224.0.0.11".
     */
    { "ipmipfa.if", "adv_address", MIPFA_IF_ADV_ADDRESS_LIST},

    /*
     * RtAdv lifetime in seconds.
     * Specifies the lifetime (in seconds) of advertisement messages from the
     * agent. This parameter is configurable on a per-interface basis in
     * the form of "<ifname>=<value>", each separated from the other by a
     * semicolon. E.g. "eth0=300"
     */
    { "ipmipfa.if", "adv_lifetime", MIPFA_IF_ADV_LIFETIME_LIST},

    /*
     * Interface Challenge
     * Enable(1)/disable(0) RFC3012 challenges on this interface. This
     * parameter is configurable on a per-interface basis in the form of
     * "<ifname>=<value>", each separated from the other by a semicolon.
     * E.g. "eth0=1"
     */
    { "ipmipfa.if", "challenge", MIPFA_IF_CHALLENGE_LIST},

    /*
     * NAT-T enabled on this interface
     * Enables or disables NAT Traversal according to RFC3519. This parameter
     * is configurable on a per-interface basis in the form of
     * "<ifname>=<value>", each separated from the other by a semicolon.
     * E.g. "eth0=1".
     */
    { "ipmipfa.if", "nat_t.enabled", MIPFA_IF_NAT_T_ENABLED_LIST},

    /*
     * NAT-T enabled on this interface
     * Enables forcing of NAT traversal (RFC 3519), regardless what the home
     * agent NAT detection decides.  This parameter is configurable on a
     * per-interface basis in the form of "<ifname>=<value>", each separated
     * from the other by a semicolon. E.g. "eth0=0"
     */
    { "ipmipfa.if", "nat_t.forced", MIPFA_IF_FORCED_NAT_T_ENABLED_LIST},

    /*
     * NAT-T keepalive
     * Decides the keepalive timeout (in seconds) to use for NAT Traversal
     * ICMP keepalive messages in order to maintain a NAT device's UDP port
     * mapping. This parameter is configurable on a per-interface basis in
     * the form of "<ifname>=<timeout>", each separated from the other by a
     * semicolon. E.g. "eth0=120"
     */
    { "ipmipfa.if", "nat_t.keepalive", MIPFA_IF_NAT_T_KEEPALIVE_LIST},

    /* NAT-T forced tunnel type; ipip, gre, min or empty (use ordinary spec) */
    { "ipmipfa.if", "nat_t.tunnel_type", MIPFA_IF_NAT_T_TUNNEL_TYPE_LIST},

    /*
     * Interface Tunneling Reordering
     * Enable packet reordering to guarantee in-order delivery. Currently
     * only supported by GRE by use of sequence numbers.
     */
    { "ipmipfa.if", "tunnel.reordering", MIPFA_IF_TUNNEL_REORDERING_LIST},

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
    { "ipmipfa.if", "tunnel.reverse", MIPFA_IF_TUNNEL_REVERSE_LIST},

    /* Interface IPIP tunneling
     * Enable(1)/Disable(0) usage of IPIP tunneling for an interface. This
     * parameter is configurable on a per-interface basis in the form of
     * "<ifname>=<value>", each separated from the other by a semicolon.
     * E.g. "eth0=1".
     */
    { "ipmipfa.if", "tunnel.ipip", MIPFA_IF_IPIP_TUNNEL_ENABLED_LIST},

    /* Interface GRE tunneling
     * Enable(1)/Disable(0) usage of GRE tunneling for an interface. This
     * parameter is configurable on a per-interface basis in the form of
     * "<ifname>=<value>", each separated from the other by a semicolon.
     * E.g. "eth0=1".
     */
    { "ipmipfa.if", "tunnel.gre", MIPFA_IF_GRE_TUNNEL_ENABLED_LIST},

    /* Interface MIN Encap tunneling
     * Enable(1)/Disable(0) usage of Minimal encapsulation tunneling for an
     * interface. This parameter is configurable on a per-interface basis
     * in the form of "<ifname>=<value>", each separated from the other by a
     * semicolon. E.g. "eth0=1".
     */
    { "ipmipfa.if", "tunnel.minenc", MIPFA_IF_MINENC_TUNNEL_ENABLED_LIST},

    /* Revocation support     */
    { "ipmipfa.if", "revocation", MIPFA_IF_REVOCATION_ENABLED_LIST},
    { "ipmipfa.if", "revocation.inform", MIPFA_IF_REVOCATION_INFORM_LIST},


    /* Assign a specific link layer adddress */
#if 0
    { "ipmipfa.if", "link.layer", MIPFA_IF_LINK_LAYER},
#endif

#ifdef INCLUDE_IPMIPFA_PMIPV4_DHCP
    { "ipmipfa.if", "pmipv4.dhcp.port", MIPFA_PMIPV4_DHCP_PORT},
    { "ipmipfa.if", "pmipv4.dhcp.client.port", MIPFA_PMIPV4_DHCP_CLIENT_PORT},
    { "ipmipfa.if", "pmipv4.dhcp.client.address", MIPFA_PMIPV4_DHCP_CLIENT_ADDRESS},
    { "ipmipfa.if", "pmipv4.dhcp.home.agent", MIPFA_PMIPV4_DHCP_HOME_AGENT_ADDRESS},
    { "ipmipfa.if", "pmipv4.dhcp.relay.server", MIPFA_PMIPV4_DHCP_SERVER},
    { "ipmipfa.if", "pmipv4.dhcp.relay.server.override", MIPFA_PMIPV4_DHCP_SERVER_OVERRIDE},
    { "ipmipfa.if", "pmipv4.dhcp.spi.mn.ha", MIPFA_PMIPV4_DHCP_SPI_MN_HA},
    { "ipmipfa.if", "pmipv4.dhcp.spi.mn.ha.secret", MIPFA_PMIPV4_DHCP_SPI_MN_HA_SECRET},
    { "ipmipfa.if", "pmipv4.dhcp.domain", MIPFA_PMIPV4_DHCP_DOMAIN},
#endif

    /* End of configuration - DO NOT REMOVE! */
    { IP_NULL, IP_NULL, IP_NULL }
};


#else /* WRS_IPNET */

#define INCLUDE_IPMIPFA_CMD

#if defined(IPTESTENGINE)
#   define INCLUDE_IPMIPFA_FEATURE_EXTENSION   1
#   define MIPFA_FEATURE_INITIALIZATION_HOOK   ipmipfa_regext_test_install
#endif

#define INCLUDE_IPMIPFA_AAA_RADIUS
#define INCLUDE_IPMIPFA_AAA_DIAMETER
#define INCLUDE_IPMIPFA_PMIPV4
#define INCLUDE_IPMIPFA_PMIPV4_DHCP

IP_STATIC IP_CONST Ipcom_sysvar ipmipfa_conf_sysvar[] =
{
    /* End of configuration - DO NOT REMOVE! */
	{ IP_NULL, IP_NULL }
};

#endif /* WRS_IPNET */


#if defined(INCLUDE_IPMIPFA_FEATURE_EXTENSION)
extern void MIPFA_FEATURE_INITIALIZATION_HOOK(void);
#endif

#ifdef INCLUDE_IPMIPFA_AAA_RADIUS
IP_GLOBAL void ipmipfa_aaa_radius_install(void);
#endif

#ifdef INCLUDE_IPMIPFA_AAA_DIAMETER
IP_GLOBAL void ipmipfa_aaa_diameter_install(void);
#endif

#ifdef INCLUDE_IPMIPFA_PMIPV4
IP_GLOBAL void ipmipfa_pmipv4_install(void);
#endif
#ifdef INCLUDE_IPMIPFA_PMIPV4_DHCP
IP_GLOBAL void ipmipfa_pmipv4_dhcp_install(void);
#endif
Ipmipfa_config_func_t      ipmipfa_function_registrations[] =
{
#ifdef INCLUDE_IPMIPFA_AAA_RADIUS
    ipmipfa_aaa_radius_install,
#endif    
#ifdef INCLUDE_IPMIPFA_AAA_DIAMETER
    ipmipfa_aaa_diameter_install,
#endif    
#ifdef INCLUDE_IPMIPFA_PMIPV4
    ipmipfa_pmipv4_install,
#endif
#ifdef INCLUDE_IPMIPFA_PMIPV4_DHCP
    ipmipfa_pmipv4_dhcp_install,
#endif
#if defined(INCLUDE_IPMIPFA_FEATURE_EXTENSION)
    MIPFA_FEATURE_INITIALIZATION_HOOK,
#endif
    IP_NULL
};


/*
 ****************************************************************************
 * 9                    PUBLIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipmipfa_configure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipmipfa_configure(void)
{
    if (ipcom_sysvar_setv(ipmipfa_conf_sysvar, 0) == IPCOM_SUCCESS)
    {
#if defined(INCLUDE_IPMIPFA)
        /* now configure the interface-specific sysvars */
        return ipcom_sysvar_ext_setv (ipmipfa_conf_sysvar_ext, 0);
#else /* WRS_IPNET */
        return IPCOM_SUCCESS;
#endif
    }

    return IPCOM_ERR_INVALID_CONFIG;
}


/*
 *===========================================================================
 *                    ipmipfa_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipmipfa_init(void)
{
#if defined(INCLUDE_IPMIPFA_CMD) && !defined(IP_PORT_LAS)
    (void)ipcom_shell_add_cmd("fa", "fa <command> [modifiers]", "Mobile IP Foreign Agent",
                        ipmipfa_cmd_fa,
                        IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_SMALL);
#endif
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipmipfa_exit
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipmipfa_exit(void)
{
#if defined(INCLUDE_IPMIPFA_CMD) && !defined(IP_PORT_LAS)
    (void)ipcom_shell_remove_cmd("fa", ipmipfa_cmd_fa);
#endif
    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

