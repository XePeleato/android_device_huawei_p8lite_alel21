/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipmipmn_config.c,v $ $Revision: 1.4 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/ipmipmn_config.c,v $
 *   $Author: jhorteli $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
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
#define INCLUDE_IPMIPMN_CMD 1
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
IP_PUBLIC int ipmipmn_cmd_mn(int argc, char **argv);

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipmipmn_configure(void);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 *===========================================================================
 *             Mobile Node sysvar Configuration
 *===========================================================================
 */

#if defined(INCLUDE_IPMIPMN)

IP_STATIC IP_CONST Ipcom_sysvar ipmipmn_conf_sysvar[] =
{
    /* 
     * Active MIP interface. There's only one interface configured at 
     * any time in the mobile node. 
     */
    { "ipmipmn.interface", MIPMN_IFNAME},

    /* Home agent IP address */
    { "ipmipmn.homeagent", MIPMN_HOME_AGENT},

    /* Reduce the MTU of tunneled routes; incase of VPN tunneling to
     * reduce fragmentation 
     */
    { "ipmipmn.mtu_reduction", MIPMN_MTU_REDUCTION },

    /* Home address and netmask. 0.0.0.0 -> read IP address/netmask at start. */
    { "ipmipmn.homeaddress", MIPMN_HOME_ADDRESS},
    { "ipmipmn.homenetmask", MIPMN_HOME_MASK},

    /* Set default gateway on home network if this sysvar is set. */
    { "ipmipmn.homegateway", MIPMN_HOME_GATEWAY},

    /* Destination address for outgoing router solicitation messages. Set to 0 to disable. */
    { "ipmipmn.sol_address", MIPMN_SOL_ADDRESS},

    /* Home agent shared secret and Security Parameter Index (SPI). */
    { "ipmipmn.haauthsecret", MIPMN_HA_AUTH_SECRET},
    { "ipmipmn.haauthspi", MIPMN_HA_AUTH_SPI},
    { "ipmipmn.haauthmethod", MIPMN_HA_AUTH_METHOD},

    /* Whether to use a foreign agent ("fa") or a co-located ("co") address. */
    { "ipmipmn.runmode", MIPMN_RUN_MODE},

    /* Foreign agent shared secret and Security Parameter Index (SPI).
     * This is only needed if ipmipmn.runmode is set to "fa".
     * Note: not setting "ipmipmn.faauthsecret" means mobile-foreign
     * authentication is disabled.
     */
    { "ipmipmn.faauthsecret", MIPMN_FA_AUTH_SECRET},
    { "ipmipmn.faauthspi", MIPMN_FA_AUTH_SPI},
    { "ipmipmn.faauthmethod", MIPMN_FA_AUTH_METHOD},

    /* AAA */
    { "ipmipmn.aaa.secret", MIPMN_AAA_SECRET},
    { "ipmipmn.aaa.spi", MIPMN_AAA_SPI},
    { "ipmipmn.aaa.method", MIPMN_AAA_METHOD},

    /* Registration lifetime in seconds. 0 = infinite. */
    { "ipmipmn.reg_lifetime", MIPMN_REG_LIFETIME},

    /* Receive broadcast packets from home network. */
    { "ipmipmn.receivebroadcasts",MIPMN_RECV_BROADCASTS},

    /* "ipip" = IP over IP (gif), "gre" = GRE, "min" = minimal encapsulation. */
    { "ipmipmn.simbind", MIPMN_SIMBIND},

    /* Tunnel reordering */
    { "ipmipmn.reordering", MIPMN_TUNNEL_REORDERING},

    /* Revocation support */
    { "ipmipmn.revocation", MIPMN_REVOCATION},

    /* "ipip" = IP over IP (gif), "gre" = GRE, "min" = minimal encapsulation. */
    { "ipmipmn.tunneltype", MIPMN_TUNNEL_TYPE},

    /* Enable(1)/disable(0) reverse tunneling. */
    { "ipmipmn.reversetunneling", MIPMN_REVERSE_TUNNELING},

    /* NAI. Network Access identifiers */
    { "ipmipmn.nai.mn", MIPMN_NAI},

    /* DHAA */
    { "ipmipmn.dhaa",    MIPMN_DYNAMIC_HOME_AGENT_ASSIGNMENT },

    /* 
     * Movement and solicitations; Always solicit on L2-LU (wont work
     * well with POST LLH) 
     */
    { "ipmipmn.solicit", MIPMN_SOLICIT},

    /* ipsec protected CoA's only */
    { "ipmipmn.ipipsec.protected", MIPMN_IPSEC_PROTECTED},
    
    /* Force reconfiguration on move */
    { "ipmipmn.ipike.reconfigure", MIPMN_IPIKE_RECONFIGURE },

    /* only use secure addresses (IKE allocated) */
    { "ipmipmn.ipike.secure", MIPMN_IPIKE_SECURE},

    /* enable mobike on movement (IKE allocated) */
    { "ipmipmn.ipike.mobike", MIPMN_IPIKE_MOBIKE },
    
    /* NAT-T enabled on this interface */
    { "ipmipmn.nat_t.enabled", MIPMN_NAT_T_ENABLED},

    /* NAT-T enabled on this interface */
    { "ipmipmn.nat_t.forced", MIPMN_NAT_T_FORCED},

    /* NAT-T keepalive; In seconds */
    { "ipmipmn.nat_t.keepalive", MIPMN_NAT_T_KEEPALIVE},

    /* NAT-T forced tunnel type; ipip, gre, min or empty (use ordinary spec) */
    { "ipmipmn.nat_t.tunneltype",  MIPMN_NAT_T_TUNNEL_TYPE},
     
    /* Configuration delimiter - DO NOT EDIT! */
	{ IP_NULL, IP_NULL }
};

IP_STATIC IP_CONST Ipcom_sysvar_ext ipmipmn_conf_sysvar_ext[] =
{
    /* Extended security association selection and creation */
    { "ipmipmn.fa.spi", "secret", MIPMN_FA_SPI_SECRET_LIST},
    { "ipmipmn.fa.spi", "method", MIPMN_FA_SPI_METHOD_LIST},
    { "ipmipmn.ha.spi", "secret", MIPMN_HA_SPI_SECRET_LIST },
    { "ipmipmn.ha.spi", "method", MIPMN_HA_SPI_METHOD_LIST},

    { "ipmipmn.ha.sa.address", IP_NULL, MIPMN_HA_SA_ADDRESS_LIST},
    { "ipmipmn.fa.sa.address", IP_NULL, MIPMN_FA_SA_ADDRESS_LIST},

    /* Configuration delimiter - DO NOT EDIT! */
	{ IP_NULL, IP_NULL, IP_NULL }
};

#else /* WRS_IPNET */


IP_STATIC IP_CONST Ipcom_sysvar ipmipmn_conf_sysvar[] =
{
    /* Configuration delimiter - DO NOT EDIT! */
	{ IP_NULL, IP_NULL }
};

#if defined(IPTESTENGINE)
#   define INCLUDE_IPMIPMN_FEATURE_EXTENSION   1
#   define MIPMN_FEATURE_INITIALIZATION_HOOK   ipmipmn_regext_test_install
#endif

#endif /* WRS_IPNET */


#if defined(INCLUDE_IPMIPMN_FEATURE_EXTENSION)
extern void MIPMN_FEATURE_INITIALIZATION_HOOK(void);
#endif

typedef void (*Ipmipmn_config_func_t) (void);
Ipmipmn_config_func_t      ipmipmn_function_registrations[] =
{
#if defined(INCLUDE_IPMIPMN_FEATURE_EXTENSION)
    MIPMN_FEATURE_INITIALIZATION_HOOK,
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
 *                    ipmipmn_configure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipmipmn_configure(void)
{
    Ip_err err;
    /* Initialize IPMIPMN system variables. */
    if ((err = ipcom_sysvar_setv(ipmipmn_conf_sysvar, 0)) != IPCOM_SUCCESS)
        return err;

#if defined(INCLUDE_IPMIPMN)
    if ((err = ipcom_sysvar_ext_setv(ipmipmn_conf_sysvar_ext, 0)) != IPCOM_SUCCESS)
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
IP_PUBLIC Ip_err ipmipmn_init(void);
IP_PUBLIC Ip_err
ipmipmn_init(void)
{
#if defined(INCLUDE_IPMIPMN_CMD) && !defined(IP_PORT_LAS)
    (void)ipcom_shell_add_cmd("mn", 
                              "mn <command> [modifiers]", "Mobile IP Mobile Node",
                              ipmipmn_cmd_mn,
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
IP_PUBLIC Ip_err ipmipmn_exit(void);
IP_PUBLIC Ip_err
ipmipmn_exit(void)
{
#if defined(INCLUDE_IPMIPMN_CMD) && !defined(IP_PORT_LAS)
    (void)ipcom_shell_remove_cmd("mn", ipmipmn_cmd_mn);
#endif
    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

