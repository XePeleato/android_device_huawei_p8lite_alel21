/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipmip6ha_config.c,v $ $Revision: 1.9 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/ipmip6ha_config.c,v $
 *   $Author: jhorteli $
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
typedef void (*Ipmip6ha_config_func_t) (void *);

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

IP_GLOBAL int ipmip6ha_cmd_ha6(int argc, char **argv);

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */



/*
 *===========================================================================
 *             Basic MIP6 Home Agent (ha) sysvar Configuration
 *===========================================================================
 */

#ifndef INCLUDE_IPMIP6HA

#define INCLUDE_IPMIP6HA_LMA_DUAL_STACK_DHCP_SERVER     1
#define INCLUDE_IPMIP6HA_LMA_DHCP_SERVER                1

#if defined(IPTESTENGINE)
#define INCLUDE_IPMIP6HA_FEATURE_EXTENSION              1
#define MIP6HA_FEATURE_INITIALIZATION_HOOK              ipmip6ha_service_testext_install
#endif

#if !defined(IP_PORT_LAS)
#define INCLUDE_IPMIP6HA_CMD                            1
#endif


IP_STATIC IP_CONST Ipcom_sysvar ipmip6ha_conf_sysvar[] =
{
    /* Configuration delimiter - DO NOT EDIT! */
	{ IP_NULL,                                      IP_NULL         }
};

#else

IP_STATIC IP_CONST Ipcom_sysvar ipmip6ha_conf_sysvar[] =
{
    { "ipmip6ha.interfaces",                             MIP6HA_INTERFACES                        },

    /* DSMIPv6 */
#if defined (INCLUDE_IPMIP6HA_DUAL_STACK)
    { "ipmip6ha.inet.topology.reserved",                 MIP6HA_INET_TOPOLOGY_ADDRESS_RESERVATION },
#endif

    /* Configuration delimiter - DO NOT EDIT! */
	{ IP_NULL,                                      IP_NULL         }
};


/* Interface specific list configurations */
IP_CONST Ipcom_sysvar_ext ipmip6ha_conf_sysvar_ext[] =
{
    /* Basic Configuration Parameters */
    { "ipmip6ha.interface", "homeagent",                                       MIP6HA_INTERFACE_ADDRESS_ENUM                                   },
    { "ipmip6ha.interface", "lifetime",                                        MIP6HA_LIFETIME_ENUM                                            },
    { "ipmip6ha.interface", "refresh",                                         MIP6HA_REFRESH_ENUM                                             },
    { "ipmip6ha.interface", "tunnel.ip6ip6",                                   MIP6HA_TUNNEL_IP6IP6_ENUM                                       },

    { "ipmip6ha.interface", "BRIMaxRetriesNumber",                             MIP6HA_BINDING_REVOCATION_MAX_RETRIES_ENUM                      },
    { "ipmip6ha.interface", "MINDelayBRIs",                                    MIP6HA_BINDING_REVOCATION_MIN_DELAY_ENUM                        },

#if defined(INCLUDE_IPMIP6HA_DUAL_STACK)
    /* DSMIPv6 */
    { "ipmip6ha.interface", "inet.homeagent",                                  MIP6HA_INET_INTERFACE_ADDRESS_ENUM                              },
    { "ipmip6ha.interface", "inet.AcceptForcedIPv4UDPEncapsulationRequest",    MIP6HA_INET_ACCEPT_FORCED_IPV4_UDP_ENCAPSULATION_REQUEST_ENUM   },
    { "ipmip6ha.interface", "inet.ForceIPv4UDPEncapsulation",                  MIP6HA_INET_FORCE_IPV4_UDP_ENCAPSULATION_ENUM                   },
    { "ipmip6ha.interface", "inet.IPv4UDPEncapsulationNATTimeout",             MIP6HA_INET_IPV4_UDP_ENCAPSULATION_NAT_TIMEOUT                  },
    { "ipmip6ha.interface", "inet.tunnel.ipip",                                MIP6HA_INET_TUNNEL_IPIP_ENUM                                    },    
    { "ipmip6ha.inet.pool", IP_NULL,                                            MIP6HA_INET_POOL_ENUM                                           },    
    { "ipmip6ha.inet.acl.homeaddress", IP_NULL,                                 MIP6HA_INET_ACL_HOMEADDRESS_ENUM                                },
    { "ipmip6ha.inet.acl.limit.homeaddress", IP_NULL,                           MIP6HA_INET_ACL_LIMIT_HOMEADDRESS_ENUM                          },
    { "ipmip6ha.inet.acl.limit.prefix_delegation", IP_NULL,                     MIP6HA_INET_ACL_LIMIT_PD_ENUM                                   },
    { "ipmip6ha.inet.acl.pool", IP_NULL,                                        MIP6HA_INET_ACL_POOL_ENUM                                       },
#endif

    /* NEMO */
#if defined(INCLUDE_IPMIP6HA_MOBILE_ROUTER)
    { "ipmip6ha.mobile_router.acl.prefix_delegation", IP_NULL,                  MIP6HA_MOBILE_ROUTER_ACL_PD_ENUM                                },
#endif

#if defined(INCLUDE_IPMIP6HA_MOBILE_ROUTER_DUAL_STACK)
    /* DSMIPv6 NEMO */
    { "ipmip6ha.mobile_router.inet.acl.prefix_delegation", IP_NULL,             MIP6HA_MOBILE_ROUTER_INET_ACL_PD_ENUM                           },
#endif

#if defined(INCLUDE_IPMIP6HA_LMA)
    /* Proxy Mobility*/
    { "ipmip6ha.LMA.pool", IP_NULL,                                             MIP6HA_LMA_POOL_ENUM                                            },
    { "ipmip6ha.LMA.acl.prefix_delegation", IP_NULL,                            MIP6HA_LMA_ACL_PD_ENUM                                          },
    { "ipmip6ha.LMA.acl.pool", IP_NULL,                                         MIP6HA_LMA_ACL_POOL_ENUM                                        },
    { "ipmip6ha.LMA.acl.limit", IP_NULL,                                        MIP6HA_LMA_ACL_LIMIT_ENUM                                       },
    { "ipmip6ha.interface", "LMA.BindingRevocation",                           MIP6HA_LMA_REVOKE_PREVIOUS_MAG_ENUM                        },
    { "ipmip6ha.interface", "LMA.tunnel.ip6ip6",                               MIP6HA_LMA_TUNNEL_IP6IP6_ENUM                                   },
    { "ipmip6ha.interface", "LMA.tunnel.gre",                                  MIP6HA_LMA_TUNNEL_GRE_ENUM                                     },
    { "ipmip6ha.interface", "LMA.MinDelayBeforeBCEDelete",                     MIP6HA_LMA_MIN_DELAY_BEFORE_BCE_DELETE_ENUM                     },
    { "ipmip6ha.interface", "LMA.MaxDelayBeforeNewBCEAssign",                  MIP6HA_LMA_MAX_DELAY_BEFORE_NEW_BCE_ASSIGN_ENUM                 },
    { "ipmip6ha.interface", "LMA.TimestampValidityWindow",                     MIP6HA_LMA_TIMESTAMP_VALIDITY_WINDOW_ENUM                       },
    { "ipmip6ha.interface", "LMA.MobileNodeGeneratedTimestampInUse",           MIP6HA_LMA_MOBILE_NODE_GENERATED_TIMESTAMP_IN_USE_ENUM          },
    { "ipmip6ha.interface", "LMA.FixedMAGLinkLocalAddressOnAllAccessLinks",    MIP6HA_LMA_FIXED_MAG_LINKLOCAL_ADDRESS_ON_ALL_ACCESS_LINKS_ENUM },
    { "ipmip6ha.interface", "LMA.HeartBeatEnabled",                            MIP6HA_LMA_HEARTBEAT_ENABLED_ENUM                               },
    { "ipmip6ha.interface", "LMA.HeartBeatInterval",                           MIP6HA_LMA_HEARTBEAT_INTERVAL_ENUM                              },
    { "ipmip6ha.interface", "LMA.MissingHeartBeatsAllowed",                    MIP6HA_LMA_MISSING_HEARTBEATS_ALLOWED_ENUM                      },
    { "ipmip6ha.LMA.MAG", IP_NULL,                                              MIP6HA_LMA_MAG_ENUM                                             },
#endif

#if defined(INCLUDE_IPMIP6HA_LMA_DHCP_SERVER)
    { "ipmip6ha.interface", "LMA.dhcp.server",                                 MIP6HA_LMA_DHCP_SERVER_SERVER_ENUM                              },
    { "ipmip6ha.interface", "LMA.dhcp.server.preferred",                       MIP6HA_LMA_DHCP_SERVER_SERVER_PREFERRED_ENUM                    },
    { "ipmip6ha.interface", "LMA.dhcp.server.valid",                           MIP6HA_LMA_DHCP_SERVER_SERVER_VALID_ENUM                        },    
#endif

#if defined(INCLUDE_IPMIP6HA_LMA_DUAL_STACK)
    /* DSMIPv6 Proxy Mobility */
    { "ipmip6ha.LMA.inet.topology.dhcp.option", IP_NULL,                        MIP6HA_LMA_INET_TOPOLOGY_DHCP_OPTION_ENUM                       },
    { "ipmip6ha.LMA.inet.acl.homeaddress", IP_NULL,                             MIP6HA_LMA_INET_ACL_HOMEADDRESS_ENUM                            },
    { "ipmip6ha.LMA.inet.acl.pool", IP_NULL,                                    MIP6HA_LMA_INET_ACL_POOL_ENUM                                   },
    
    { "ipmip6ha.interface", "LMA.inet.tunnel.ipip",                            MIP6HA_LMA_INET_TUNNEL_IPIP_ENUM                                },    
    { "ipmip6ha.interface", "LMA.inet.tunnel.gre",                             MIP6HA_LMA_INET_TUNNEL_GRE_ENUM                                 },    
#endif

#if defined(INCLUDE_IPMIP6HA_LMA_DUAL_STACK_DHCP_SERVER)
    { "ipmip6ha.interface", "LMA.inet.dhcp.server",                            MIP6HA_LMA_INET_DHCP_SERVER_ENUM                                },    
#endif
   
    
    /* Configuration delimiter - DO NOT EDIT! */
    { IP_NULL, IP_NULL, IP_NULL }
};

#endif


#if defined(INCLUDE_IPMIP6HA_LMA_DUAL_STACK_DHCP_SERVER)
extern void ipmip6ha_pmipv6_dhcpv4_install(void *);
#endif

#if defined(INCLUDE_IPMIP6HA_LMA_DHCP_SERVER)
extern void ipmip6ha_pmipv6_dhcpv6_install(void *);
#endif

extern void ipmip6ha_pmipv6_linklocal_install(void *);

#if defined(INCLUDE_IPMIP6HA_FEATURE_EXTENSION)
extern void MIP6HA_FEATURE_INITIALIZATION_HOOK(void *);
#endif

Ipmip6ha_config_func_t      ipmip6ha_service_registrations[] =
{
#if defined(INCLUDE_IPMIP6HA_LMA_DUAL_STACK_DHCP_SERVER)
    ipmip6ha_pmipv6_dhcpv4_install,
#endif
#if defined(INCLUDE_IPMIP6HA_LMA_DHCP_SERVER)
    ipmip6ha_pmipv6_dhcpv6_install,
#endif

    ipmip6ha_pmipv6_linklocal_install,

#if defined(INCLUDE_IPMIP6HA_FEATURE_EXTENSION)
    MIP6HA_FEATURE_INITIALIZATION_HOOK,
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
 *                    ipmip6ha_configure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err ipmip6ha_configure(void);
IP_PUBLIC Ip_err
ipmip6ha_configure(void)
{
	/* Initialize IPMIPMN system variables. */
    /* Initialize IPMIPMN system variables. */
    if (ipcom_sysvar_setv(ipmip6ha_conf_sysvar, 0) == IPCOM_SUCCESS)
    {
        /* now configure the interface-specific sysvars */
#ifdef INCLUDE_IPMIP6HA
        return ipcom_sysvar_ext_setv (ipmip6ha_conf_sysvar_ext, 0);
#else
        return IPCOM_SUCCESS;
#endif
    }

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipmip6ha_init
 *===========================================================================
 * Description: 
 * Parameters:  
 * Returns:     
 *
 */
IP_PUBLIC Ip_err ipmip6ha_init(void);
IP_PUBLIC Ip_err
ipmip6ha_init(void)
{
#if defined(INCLUDE_IPMIP6HA_CMD) && !defined(IP_PORT_LAS)
    ipcom_shell_add_cmd("ha6", "ha6 <command> [modifiers]", "Mobile IPv6 Home Agent",
                        ipmip6ha_cmd_ha6,
                        IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_SMALL);
#endif
    return IPCOM_SUCCESS;
}

/*
 *===========================================================================
 *                    ipmip6ha_exit
 *===========================================================================
 * Description: 
 * Parameters:  
 * Returns:     
 *
 */
IP_PUBLIC Ip_err ipmip6ha_exit(void);
IP_PUBLIC Ip_err
ipmip6ha_exit(void)
{
#if defined(INCLUDE_IPMIP6HA_CMD) && !defined(IP_PORT_LAS)
    /* Remove the shell command */
    ipcom_shell_remove_cmd("ha6", ipmip6ha_cmd_ha6);
#endif
    return IPCOM_SUCCESS;
}

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

