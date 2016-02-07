/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipmip6pm_config.c,v $ $Revision: 1.8 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/ipmip6pm_config.c,v $
 *   $Author: jonas $
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
#include <ipcom_os.h>
#include <ipcom_err.h>
#include <ipcom_shell.h>
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

typedef void (*Ipmip6pm_config_func_t) (void *);
/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_GLOBAL int ipmip6pm_cmd_pm6(int argc, char **argv);

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

#if !defined(INCLUDE_IPMIP6MN)

#define INCLUDE_IPMIP6PM_LINK_FEATURE_LINK_LAYER_ADDRESS       1
#define INCLUDE_IPMIP6PM_LINK_FEATURE_LINK_LOCAL_ADDRESS       1
#define INCLUDE_IPMIP6PM_SERVICE_FEATURE_DHCP_RELAY            1
#define INCLUDE_IPMIP6PM_SERVICE_FEATURE_RADVD                 1
#define INCLUDE_IPMIP6PM_SERVICE_FEATURE_SERVICE_SELECTION     1
#define INCLUDE_IPMIP6PM_SERVICE_FEATURE_VENDOR_EXTENSION      1
#define INCLUDE_IPMIP6PM_DUAL_STACK_DHCP_RELAY_SUPPORT         1
#define INCLUDE_IPMIP6PM_DUAL_STACK_DHCP_SERVER_SUPPORT        1
#define INCLUDE_IPMIP6PM_DUAL_STACK                            1

#define INCLUDE_IPMIP6PM_CMD


/*
 *===========================================================================
 *             Basic MIP6 Mobile Node(mn) sysvar Configuration
 *===========================================================================
 */

IP_STATIC IP_CONST Ipcom_sysvar ipmip6pm_conf_sysvar[] =
{

    /* Configuration delimiter - DO NOT EDIT! */
	{ IP_NULL,                                      IP_NULL         }
};

#else

IP_STATIC IP_CONST Ipcom_sysvar ipmip6pm_conf_sysvar[] =
{
    /* Basic configuration entries */
    { "ipmip6pm.interfaces",                        MIP6PM_INTERFACES           },


    /* Configuration delimiter - DO NOT EDIT! */
	{ IP_NULL,                                      IP_NULL         }
};

/* Interface specific list configurations */
IP_CONST Ipcom_sysvar_ext ipmip6pm_conf_sysvar_ext[] =
{
    /* Configuration delimiter - DO NOT EDIT! */
    { "ipmip6pm.interface", "lifetime",                                        MIP6PM_LIFETIME_ENUM                                            },
    { "ipmip6pm.interface", "InitialBindackTimeoutFirstReg",                   MIP6PM_INITIAL_REGISTRATION_TIMEOUT_ENUM                        },
    { "ipmip6pm.interface", "TimestampBasedApproachInUse",                     MIP6PM_TIMESTAMP_BASED_APPROACH_IN_USE_ENUM                     },
    { "ipmip6pm.interface", "care_of_address",                                 MIP6PM_CARE_OF_ADDRESS_ENUM                                     },
    { "ipmip6pm.interface", "tunnel.gre",                                      MIP6PM_TUNNEL_GRE_ENUM                                          },
    { "ipmip6pm.interface", "tunnel.ip6ip6",                                   MIP6PM_TUNNEL_IP6IP6_ENUM                                       },
    { "ipmip6pm.interface", "tunnel.gre.Preferred",                            MIP6PM_TUNNEL_GRE_PREFERRED_ENUM                                },
    { "ipmip6pm.interface", "tunnel.gre.UseKeys",                              MIP6PM_TUNNEL_GRE_KEYED_ENUM                                    },
    { "ipmip6pm.interface", "HeartBeatEnabled",                                MIP6PM_HEARTBEAT_ENABLED_ENUM                                   },
    { "ipmip6pm.interface", "HeartBeatInterval",                               MIP6PM_HEARTBEAT_INTERVAL_ENUM                                  },
    { "ipmip6pm.interface", "MissingHeartBeatsAllowed",                        MIP6PM_MISSING_HEARTBEATS_ALLOWED_ENUM                          },
    { "ipmip6pm.LMA", IP_NULL,                                                 MIP6PM_LMA_ENUM                                                 },

    { "ipmip6pm.interface", "BRIMaxRetriesNumber",                             MIP6PM_BINDING_REVOCATION_MAX_RETRIES_ENUM                      },
    { "ipmip6pm.interface", "MINDelayBRIs",                                    MIP6PM_BINDING_REVOCATION_MIN_DELAY_ENUM                        },

#if defined(INCLUDE_IPMIP6PM_DUAL_STACK)
    { "ipmip6pm.interface", "inet.care_of_address",                            MIP6PM_INET_CARE_OF_ADDRESS_ENUM                                },
    { "ipmip6pm.interface", "inet.UseIPv4UDPEncapForSignalingMessages",        MIP6PM_INET_USE_IPV4_UDP_ENCAP_FOR_SIGNALING_MESSAGES_ENUM      },
    { "ipmip6pm.interface", "inet.ForceIPv4UDPEncapsulationSupport",           MIP6PM_INET_FORCE_IPV4_UDP_ENCAPSULATION_SUPPORT_ENUM           },
    { "ipmip6pm.interface", "inet.IPv4UDPEncapsulationNATTimeout",             MIP6PM_INET_IPV4UDPENCAPSULATIONNATTIMEOUT_ENUM                 },
    { "ipmip6pm.interface", "inet.tunnel.gre",                                 MIP6PM_INET_TUNNEL_GRE_ENUM                                     },
    { "ipmip6pm.interface", "inet.tunnel.ipip",                                MIP6PM_INET_TUNNEL_IPIP_ENUM                                    },
#endif

#if defined(INCLUDE_IPMIP6PM_DUAL_STACK_DHCP_RELAY_SUPPORT)
    { "ipmip6pm.interface", "inet.dhcp.relay.enabled",                         "enabled"                                                       },
    { "ipmip6pm.interface", "inet.dhcp.relay.server",                          MIP6PM_INET_DHCP_RELAY_SERVER_ENUM                              },
    { "ipmip6pm.interface", "inet.dhcp.relay.server.override",                 MIP6PM_INET_DHCP_RELAY_SERVER_OVERRIDE_ENUM                     },
#else
    { "ipmip6pm.interface", "inet.dhcp.relay.enabled",                         "disabled"                                                      },
#endif

#if defined(INCLUDE_IPMIP6PM_DUAL_STACK_DHCP_SERVER_SUPPORT)
    { "ipmip6pm.interface", "inet.dhcp.server.enabled",                        "enabled"                                                       },
#else
    { "ipmip6pm.interface", "inet.dhcp.server.enabled",                        "disabled"                                                      },
#endif

#if defined(INCLUDE_IPMIP6PM_LINK_FEATURE_LINK_LAYER_ADDRESS)
    { "ipmip6pm.interface", "FixedMAGLinkLayerAddressOnAllAccessLinks",        MIP6PM_FIXED_MAG_LINK_LAYER_ADDRESS_ON_ALL_ACCESS_LINKS_ENUM    },
#endif
#if defined(INCLUDE_IPMIP6PM_LINK_FEATURE_LINK_LOCAL_ADDRESS)
    { "ipmip6pm.interface", "FixedMAGLinkLayerAddressOnAllAccessLinks",        MIP6PM_FIXED_MAG_LINK_LOCAL_ADDRESS_ON_ALL_ACCESS_LINKS_ENUM    },
#endif

#if defined(INCLUDE_IPMIP6PM_SERVICE_FEATURE_DHCP_RELAY)
    { "ipmip6pm.interface", "dhcpv6_relay.mode",                               MIP6PM_DHCP_RELAY_MODE_ENUM                                     },
    { "ipmip6pm.interface", "dhcpv6_relay.server",                             MIP6PM_DHCP_RELAY_SERVER_ENUM                                   },
#endif

#if defined(INCLUDE_IPMIP6PM_SERVICE_FEATURE_RADVD)
    { "ipmip6pm.interface", "radvd.AdvManagedFlag",                            MIP6PM_RADVD_MANAGED_ENUM                                       },
    { "ipmip6pm.interface", "radvd.AdvOtherConfigFlag",                        MIP6PM_RADVD_OTHER_ENUM                                         },
#endif

#if defined(INCLUDE_IPMIP6PM_SERVICE_FEATURE_SERVICE_SELECTION)
    { "ipmip6pm.interface", "service_selection",                               MIP6PM_SERVICE_SELECTION_ENUM                                   },
#endif

#if defined(INCLUDE_IPMIP6PM_SERVICE_FEATURE_VENDOR_EXTENSION)
    { "ipmip6pm.service_selection",     IP_NULL,                                MIP6PM_VENDOR_EXTENSION_ENUM                                    },
#endif

    { IP_NULL, IP_NULL, IP_NULL }
};

#endif

#if defined(INCLUDE_IPMIP6PM_SERVICE_FEATURE_DHCP_RELAY)
extern void ipmip6pm_service_dhcpv6_relay_install(void *);
#endif

#if defined(INCLUDE_IPMIP6PM_SERVICE_FEATURE_RADVD)
extern void ipmip6pm_service_radvd_install(void *);
#endif

#if defined(INCLUDE_IPMIP6PM_SERVICE_FEATURE_SERVICE_SELECTION)
extern void ipmip6pm_service_selection_install(void *);
#endif

#if defined(INCLUDE_IPMIP6PM_SERVICE_FEATURE_VENDOR_EXTENSION)
extern void ipmip6pm_service_vendor_extension_install(void *);
#endif

#if defined(INCLUDE_IPMIP6PM_DUAL_STACK_DHCP_RELAY_SUPPORT) || defined(INCLUDE_IPMIP6PM_DUAL_STACK_DHCP_SERVER_SUPPORT)
extern void ipmip6pm_service_dsmipv6_install(void *);
#endif

#if defined(INCLUDE_IPMIP6PM_SERVICE_FEATURE_EXTENSION)
extern void MIP6PM_SERVICE_FEATURE_INITIALIZATION_HOOK(void *);
#endif


Ipmip6pm_config_func_t      ipmip6pm_service_registrations[] =
{
#if defined(INCLUDE_IPMIP6PM_SERVICE_FEATURE_DHCP_RELAY)
    ipmip6pm_service_dhcpv6_relay_install,
#endif

#if defined(INCLUDE_IPMIP6PM_SERVICE_FEATURE_RADVD)
    ipmip6pm_service_radvd_install,
#endif

#if defined(INCLUDE_IPMIP6PM_SERVICE_FEATURE_SERVICE_SELECTION)
    ipmip6pm_service_selection_install,
#endif
#if defined(INCLUDE_IPMIP6PM_SERVICE_FEATURE_VENDOR_EXTENSION)
    ipmip6pm_service_vendor_extension_install,
#endif
#if defined(INCLUDE_IPMIP6PM_DUAL_STACK_DHCP_RELAY_SUPPORT) || defined(INCLUDE_IPMIP6PM_DUAL_STACK_DHCP_SERVER_SUPPORT)
    ipmip6pm_service_dsmipv6_install,
#endif

#if defined(INCLUDE_IPMIP6PM_SERVICE_FEATURE_EXTENSION)
    MIP6PM_SERVICE_FEATURE_INITIALIZATION_HOOK,
#endif
    IP_NULL
};

#if defined(INCLUDE_IPMIP6PM_LINK_FEATURE_LINK_LAYER_ADDRESS)
extern void ipmip6pm_link_address_install(void *);
#endif

#if defined(INCLUDE_IPMIP6PM_LINK_FEATURE_LINK_LOCAL_ADDRESS)
extern void ipmip6pm_link_local_install(void *);
#endif

#if defined(INCLUDE_IPMIP6PM_LINK_FEATURE_EXTENSION)
extern void MIP6PM_LINK_FEATURE_INITIALIZATION_HOOK(void *);
#endif


Ipmip6pm_config_func_t      ipmip6pm_link_registrations[] =
{
#if defined(INCLUDE_IPMIP6PM_LINK_FEATURE_LINK_LAYER_ADDRESS)
    ipmip6pm_link_address_install,
#endif

#if defined(INCLUDE_IPMIP6PM_LINK_FEATURE_LINK_LOCAL_ADDRESS)
    ipmip6pm_link_local_install,
#endif

#if defined(INCLUDE_IPMIP6PM_LINK_FEATURE_EXTENSION)
    MIP6PM_LINK_FEATURE_INITIALIZATION_HOOK,
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
 *                    ipmip6pm_configure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err ipmip6pm_configure(void);
IP_PUBLIC Ip_err
ipmip6pm_configure(void)
{
	/* Initialize IPMIPMN system variables. */
    /* Initialize IPMIPMN system variables. */
    if (ipcom_sysvar_setv(ipmip6pm_conf_sysvar, 0) == IPCOM_SUCCESS)
    {
        /* now configure the interface-specific sysvars */
#if defined(INCLUDE_IPMIP6MN)
        return ipcom_sysvar_ext_setv (ipmip6pm_conf_sysvar_ext, 0);
#else
        return IPCOM_SUCCESS;
#endif
    }
    ipcom_sysvar_setv(ipmip6pm_conf_sysvar, 0);

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipmip6pm_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err ipmip6pm_init(void);
IP_PUBLIC Ip_err
ipmip6pm_init(void)
{
#if defined(INCLUDE_IPMIP6PM_CMD) && !defined(IP_PORT_LAS)
    ipcom_shell_add_cmd("pm6", "pm6 <command> [modifiers]", "Mobile IPv6 Proxy Mobility",
                        ipmip6pm_cmd_pm6,
                        IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_SMALL);
#endif
    return IPCOM_SUCCESS;
}

/*
 *===========================================================================
 *                    ipmip6pm_exit
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err ipmip6pm_exit(void);
IP_PUBLIC Ip_err
ipmip6pm_exit(void)
{
#if defined(INCLUDE_IPMIP6PM_CMD) && !defined(IP_PORT_LAS)
    /* Remove the shell command */
    ipcom_shell_remove_cmd("pm6", ipmip6pm_cmd_pm6);
#endif
    return IPCOM_SUCCESS;
}

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

