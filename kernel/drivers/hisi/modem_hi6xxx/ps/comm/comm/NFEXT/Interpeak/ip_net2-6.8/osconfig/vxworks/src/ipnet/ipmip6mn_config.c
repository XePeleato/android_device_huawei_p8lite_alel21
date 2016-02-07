/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipmip6mn_config.c,v $ $Revision: 1.15.24.1 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/ipmip6mn_config.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Joel Hortelius <joel.hortelius@interpeak.se>
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


/* #include "ipmip6mn_cmd_mn6.h" */
/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#ifndef INCLUDE_IPMIP6MN

/* Define some parameters that we need */
#if !defined(IP_PORT_LAS)
#define INCLUDE_IPMIP6MN_CMD
#endif

#define INCLUDE_IPMIP6MN_DUAL_STACK_NEMO_DHCP_SERVER        1
#define INCLUDE_IPMIP6MN_NEMO                               1
#define INCLUDE_IPMIP6MN_MOBILE_PREFIX_RENUMBER_SUPPORT     1
#define INCLUDE_IPMIP6MN_DUAL_STACK                         1
#define INCLUDE_IPMIP6MN_IPSEC                              1
#define INCLUDE_IPMIP6MN_NEMO_RADVD                         1
#define INCLUDE_IPMIP6MN_SERVICE_SELECTION                  1
#define INCLUDE_IPMIP6MN_SERVICE_VENDOR_EXTENSION           1

#endif

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */
typedef void (*Ipmip6mn_config_func_t) (void *);

/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */
IP_GLOBAL int ipmip6mn_cmd_mn6(int argc, char **argv);


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
 *             Basic MIP6 Mobile Node(mn) sysvar Configuration
 *===========================================================================
 */

#ifndef INCLUDE_IPMIP6MN

IP_STATIC IP_CONST Ipcom_sysvar ipmip6_mn_conf_sysvar[] =
{
    /* Configuration delimiter - DO NOT EDIT! */
	{ IP_NULL,                                      IP_NULL         }
};

#else

IP_STATIC IP_CONST Ipcom_sysvar ipmip6_mn_conf_sysvar[] =
{
    /* Enumeration of the Mobile Node configurations     */
    { "ipmip6mn.enum.0", MIP6MN_NETWORK_ENUM},

    /* Home agents */
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".homeagent", MIP6MN_HOMEAGENTS},
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".interfaces", MIP6MN_INTERFACES},

    /* Home address and netmask */
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".homeaddress", MIP6MN_HOME_ADDRESS},
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".lifetime",  MIP6MN_LIFETIME},
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".registration_timeout", MIP6MN_REG_TIMEOUT},
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".nai.mn",MIP6MN_NAI_MN},

#if defined(INCLUDE_IPMIP6MN_IPSEC)
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".ipsec.sa.authalg", MIP6MN_SA_AUTH_MODE},
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".ipsec.sa.authkey",MIP6MN_SA_AUTH_KEY},
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".ipsec.sa.encalg", MIP6MN_SA_ENC_MODE},
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".ipsec.sa.enckey", MIP6MN_SA_ENC_KEY},

    /* Enable security for MH signalling 1 == enable */
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".ipsec.mh", MIP6MN_IPSEC_MH_ENABLED},
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".ipsec.mh.spi.in", MIP6MN_IPSEC_MH_SPI_IN},
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".ipsec.mh.spi.out",MIP6MN_IPSEC_MH_SPI_OUT},

    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".ipsec.payload",MIP6MN_IPSEC_PAYLOAD_ENABLED},
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".ipsec.payload.spi.in", MIP6MN_IPSEC_PAYLOAD_SPI_IN },
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".ipsec.payload.spi.out", MIP6MN_IPSEC_PAYLOAD_SPI_OUT},
#endif

#if defined(INCLUDE_IPMIP6MN_NEMO)
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".mobile_router.mode", MIP6MN_NEMO_MODE},
#endif


#if defined(INCLUDE_IPMIP6MN_DUAL_STACK)
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".inet", "enabled" },
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".inet.homeaddress", MIP6MN_INET_HOMEADDRESS},
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".inet.ForceIPv4UDPEncapsulation", MIP6MN_INET_FORCEIPV4UDPENCAPSULATION },
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".inet.IPv4UDPEncapsulationNATTimeout",MIP6MN_INET_IPV4UDPENCAPSULATIONNATTIMEOUT },
#else
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".inet", "disabled" }
#endif

#if defined(INCLUDE_IPMIP6MN_DUAL_STACK_NEMO_DHCP_SERVER)
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".inet.mobile_router.dhcp.server", "enabled" },
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".inet.mobile_router.dhcp.server.offer.timeout", MIP6MN_NEMO_INET_DHCP_SERVER_OFFER_TIMEOUT },
#else
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".inet.mobile_router.dhcp.server", "disabled" },
#endif

#if defined(INCLUDE_IPMIP6MN_SERVICE_SELECTION)
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".service_selection", MIP6MN_SERVICE_SELECTION },
#endif

#if defined(INCLUDE_IPMIP6MN_NEMO_RADVD)
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".mobile_router.radvd", "enabled" },
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".mobile_router.radvd.AdvManagedFlag", MIP6MN_NEMO_RADVD_MANAGED },
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".mobile_router.radvd.AdvOtherConfigFlag", MIP6MN_NEMO_RADVD_OTHER },
#else
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".mobile_router.radvd", "disabled" },
#endif

#if defined(INCLUDE_IPMIP6MN_MOBILE_PREFIX_RENUMBER_SUPPORT)
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".mobile_prefix.renumber.derive_from_configured", MIP6MN_MOBILE_PREFIX_RENUMBER_FROM_CONFIGURED },
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".mobile_prefix.renumber.derive_from_interface", MIP6MN_MOBILE_PREFIX_RENUMBER_FROM_INTERFACE },
#endif

    /* Configuration delimiter - DO NOT EDIT! */
	{ IP_NULL, IP_NULL }
};


/* Interface specific list configurations */
IP_CONST Ipcom_sysvar_ext ipmip6_mn_conf_sysvar_ext[] =
{
#if defined(INCLUDE_IPMIP6MN_IPSEC)
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".ipsec.sa", "authalg", MIP6MN_SA_AUTH_MODE_LIST  },
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".ipsec.sa", "authkey", MIP6MN_SA_AUTH_KEY_LIST},
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".ipsec.sa", "encalg", MIP6MN_SA_ENC_MODE_LIST},
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".ipsec.sa", "enckey", MIP6MN_SA_ENC_KEY_LIST},
#endif

#if defined(INCLUDE_IPMIP6MN_NEMO)
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".mobile_router.ingress", IP_NULL, MIP6MN_NEMO_INGRESS},
#endif

#if defined(INCLUDE_IPMIP6MN_DUAL_STACK_NEMO)
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".inet.mobile_router.ingress", IP_NULL, MIP6MN_NEMO_INET_INGRESS},
#endif

#if defined(INCLUDE_IPMIP6MN_DUAL_STACK_NEMO_DHCP_SERVER)
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".inet.mobile_router.dhcp.server.option", IP_NULL, MIP6MN_NEMO_INET_DHCP_SERVER_OPTION },
#endif

#if defined(INCLUDE_IPMIP6MN_SERVICE_VENDOR_EXTENSION)
    { "ipmip6mn." MIP6MN_NETWORK_ENUM ".vendor_extension", IP_NULL, MIP6MN_SERVICE_VENDOR_EXTENSION_ENUM },
#endif

    /* Configuration delimiter - DO NOT EDIT! */
    { IP_NULL, IP_NULL, IP_NULL }
};


#endif

#if defined(INCLUDE_IPMIP6MN_DUAL_STACK_NEMO_DHCP_SERVER)
extern void ipmip6mn_service_dsmipv6_install(void *);
#endif

#if defined(INCLUDE_IPMIP6MN_MOBILE_PREFIX_RENUMBER_SUPPORT)
extern void ipmip6mn_service_mpdisc_install(void *);
#endif

#if defined(INCLUDE_IPMIP6MN_NEMO_RADVD)
extern void ipmip6mn_service_radvd_install(void *);
#endif

#if defined(INCLUDE_IPMIP6MN_SERVICE_SELECTION)
extern void ipmip6mn_service_selection_install(void *);
#endif

#if defined(INCLUDE_IPMIP6MN_SERVICE_VENDOR_EXTENSION)
extern void ipmip6mn_service_vendor_extension_install(void *);
#endif

#if defined(INCLUDE_IPMIP6MN_FEATURE_EXTENSION)
extern void MIP6MN_FEATURE_INITIALIZATION_HOOK(void *);
#endif

Ipmip6mn_config_func_t      ipmip6mn_service_registrations[] =
{
#if defined(INCLUDE_IPMIP6MN_DUAL_STACK_NEMO_DHCP_SERVER)
    ipmip6mn_service_dsmipv6_install,
#endif
#if defined(INCLUDE_IPMIP6MN_MOBILE_PREFIX_RENUMBER_SUPPORT)
    ipmip6mn_service_mpdisc_install,
#endif
#if defined(INCLUDE_IPMIP6MN_NEMO_RADVD)
    ipmip6mn_service_radvd_install,
#endif
#if defined(INCLUDE_IPMIP6MN_SERVICE_SELECTION)
    ipmip6mn_service_selection_install,
#endif
#if defined(INCLUDE_IPMIP6MN_SERVICE_VENDOR_EXTENSION)
    ipmip6mn_service_vendor_extension_install,
#endif
#if defined(INCLUDE_IPMIP6MN_FEATURE_EXTENSION)
    MIP6MN_FEATURE_INITIALIZATION_HOOK,
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
 *                    ipmip6_configure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err ipmip6mn_configure(void);
IP_PUBLIC Ip_err
ipmip6mn_configure(void)
{
    /* Initialize IPMIPMN system variables. */
    if (ipcom_sysvar_setv(ipmip6_mn_conf_sysvar, 0) == IPCOM_SUCCESS)
    {
#ifdef INCLUDE_IPMIP6MN
        /* now configure the interface-specific sysvars */
        return ipcom_sysvar_ext_setv (ipmip6_mn_conf_sysvar_ext, 0);
#endif
    }

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipmip6mn_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err ipmip6mn_init(void);
IP_PUBLIC Ip_err
ipmip6mn_init(void)
{
#ifdef INCLUDE_IPMIP6MN_CMD
    ipcom_shell_add_cmd("mn6", "mn6 <command> [modifiers]", "Mobile IPv6 Mobile Node",
                        ipmip6mn_cmd_mn6,
                        IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_SMALL);
#endif
    return IPCOM_SUCCESS;
}

/*
 *===========================================================================
 *                    ipmip6mn_exit
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err ipmip6mn_exit(void);
IP_PUBLIC Ip_err
ipmip6mn_exit(void)
{
#ifdef INCLUDE_IPMIP6MN_CMD
    /* Remove the shell command */
    ipcom_shell_remove_cmd("mn6", ipmip6mn_cmd_mn6);
#endif
    return IPCOM_SUCCESS;
}

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

