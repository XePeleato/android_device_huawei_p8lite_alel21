/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_radvd.h,v $ $Revision: 1.6.22.1 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_radvd.h,v $
 *   $Author: ulf $ $Date: 2010-07-02 13:21:15 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPNET_RADVD_H
#define IPNET_RADVD_H

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

#include "ipnet_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_type.h>
#include <ipcom_cstyle.h>


#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_RADVD_IPD_NAME "ipnet_radvd"

#define IPNET_RADVD_SYSVAR_NAME_LEN 80

/* Bits in Ipnet_radvd_if_data::override_bitmask */
#define IPNET_RADVD_OVERRIDE_MIN_RTR_ADV_INTERVAL           (1UL << 0)
#define IPNET_RADVD_OVERRIDE_MAX_RTR_ADV_INTERVAL           (1UL << 1)
#define IPNET_RADVD_OVERRIDE_MIN_DELAY_BETWEEN_RAS          (1UL << 2)
#define IPNET_RADVD_OVERRIDE_DEFAULT_LIFETIME               (1UL << 3)
#define IPNET_RADVD_OVERRIDE_REACHABLE_TIME                 (1UL << 4)
#define IPNET_RADVD_OVERRIDE_RETRANS_TIMER                  (1UL << 5)
#define IPNET_RADVD_OVERRIDE_LINK_MTU                       (1UL << 6)
#define IPNET_RADVD_OVERRIDE_ADV_HOME_AGENT_OPT_PREFERENCE  (1UL << 7)
#define IPNET_RADVD_OVERRIDE_ADV_HOME_AGENT_OPT_LIFETIME    (1UL << 8)
#define IPNET_RADVD_OVERRIDE_HOPLIMIT                       (1UL << 9)
#define IPNET_RADVD_OVERRIDE_UNICAST_ONLY                   (1UL << 10)
#define IPNET_RADVD_OVERRIDE_MANAGED                        (1UL << 11)
#define IPNET_RADVD_OVERRIDE_OTHER_CONFIG                   (1UL << 12)
#define IPNET_RADVD_OVERRIDE_HOME_AGENT                     (1UL << 13)
#define IPNET_RADVD_OVERRIDE_ADV_INTERVAL_OPT               (1UL << 14)
#define IPNET_RADVD_OVERRIDE_ADV_HOME_AGENT_OPT             (1UL << 15)
#define IPNET_RADVD_OVERRIDE_PRIVATE_EXPERIMENTION_0        (1UL << 16)
#define IPNET_RADVD_OVERRIDE_PRIVATE_EXPERIMENTION_1        (1UL << 17)


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/* Prefix option configuration parameters (used in API) */
typedef struct Ipnet_radvd_if_prefix_data_struct
{
    struct Ip_in6_addr prefix;
    int                prefixlen;
    Ip_u32             valid_lifetime;
    Ip_bool            valid_lifetime_decrement;
    Ip_u32             preferred_lifetime;
    Ip_bool            preferred_lifetime_decrement;
    Ip_bool            on_link;
    Ip_bool            autonomous;
    Ip_bool            router_address;
}
Ipnet_radvd_if_prefix_data;

/* Network interface configuration parameters (used in API) */
typedef struct Ipnet_radvd_if_data_struct
{
    char        ifname[IP_IFNAMSIZ];
    Ip_u32      override_bitmask;
    Ip_u32      min_rtr_adv_interval;
    Ip_u32      max_rtr_adv_interval;
    Ip_u32      min_delay_between_ras;
    Ip_u32      default_lifetime;
    Ip_u32      reachable_time;
    Ip_u32      retrans_timer;
    Ip_u32      link_mtu;
    Ip_u32      adv_home_agent_opt_preference;
    Ip_u32      adv_home_agent_opt_lifetime;
    Ip_u8       hoplimit;
    Ip_bool     unicast_only;
    Ip_bool     managed;
    Ip_bool     other_config;
    Ip_bool     home_agent;
    Ip_bool     adv_interval_opt;
    Ip_bool     adv_home_agent_opt;
    Ip_bool     disable_accept_rtadv;
    /* Flags from RFC 5175, the RA message will contain the "Flag
       Expansion Option" if any of these flags are set */
    Ip_bool     private_experimentation_0;
    Ip_bool     private_experimentation_1;
}
Ipnet_radvd_if_data;

/* Client-side handles */
typedef void *Ipnet_radvd_if_handle;
typedef void *Ipnet_radvd_if_prefix_handle;


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipnet_radvd_if_open
 *===========================================================================
 * Description: Adds configuration entry for the specifed interface.
 * Parameters:  handle - Pointer to returned handle.
 *              data -  Configuration data for the interface.
 * Returns:     IPCOM error code.
 *
 */
IP_PUBLIC Ip_err
ipnet_radvd_if_open(Ipnet_radvd_if_handle *handle,
                    const Ipnet_radvd_if_data *data);

/*
 *===========================================================================
 *                    ipnet_radvd_if_close
 *===========================================================================
 * Description: Removes configuration entry for the specifed interface.
 * Parameters:  handle - Configuration handle.
 * Returns:     IPCOM error code.
 *
 */
IP_PUBLIC Ip_err
ipnet_radvd_if_close(Ipnet_radvd_if_handle handle);


/*
 *===========================================================================
 *                    ipnet_radvd_if_start
 *===========================================================================
 * Description: Initiates communication on the provided handle
 * Parameters:  handle - Configuration handle.
 * Returns:     No return value
 *
 */
IP_PUBLIC void
ipnet_radvd_if_start(Ipnet_radvd_if_handle handle);


/*
 *===========================================================================
 *                    ipnet_radvd_if_get
 *===========================================================================
 * Description: Get configuration data for the specifed handle.
 * Parameters:  handle - Configuration handle.
 *              data -   Pointer to returned configuration data.
 * Returns:     IPCOM error code.
 *
 */
IP_PUBLIC Ip_err
ipnet_radvd_if_get(Ipnet_radvd_if_handle handle,
                   Ipnet_radvd_if_data *data);

/*
 *===========================================================================
 *                    ipnet_radvd_if_set
 *===========================================================================
 * Description: Set configuration data for the specifed handle.
 * Parameters:  handle - Configuration handle.
 *              data -   Pointer to new configuration data.
 * Returns:     IPCOM error code.
 *
 */
IP_PUBLIC Ip_err
ipnet_radvd_if_set(Ipnet_radvd_if_handle handle,
                   const Ipnet_radvd_if_data *data);

/*
 *===========================================================================
 *                    ipnet_radvd_if_active_get
 *===========================================================================
 * Description: Get currently active configuration data for the specifed if.
 * Parameters:  ifname - Interface name.
 *              data - Pointer to returned configuration data.
 * Returns:     IPCOM error code.
 *
 */
IP_PUBLIC Ip_err
ipnet_radvd_if_active_get(const char *ifname,
                          Ipnet_radvd_if_data *data);

/*
 *===========================================================================
 *                    ipnet_radvd_if_prefix_open
 *===========================================================================
 * Description: Adds prefix configuration entry for the specifed interface handle.
 * Parameters:  handle - Pointer to returned handle.
 *              prefix_handle - Pointer to returned handle.
 *              data - Prefix configuration data for the interface.
 * Returns:     IPCOM error code.
 *
 */
IP_PUBLIC Ip_err
ipnet_radvd_if_prefix_open(Ipnet_radvd_if_handle handle,
                           Ipnet_radvd_if_prefix_handle *prefix_handle,
                           const Ipnet_radvd_if_prefix_data *data);

/*
 *===========================================================================
 *                    ipnet_radvd_if_prefix_close
 *===========================================================================
 * Description: Removes prefix configuration entry.
 * Parameters:  handle - Prefix configuration handle.
 * Returns:     IPCOM error code.
 *
 */
IP_PUBLIC Ip_err
ipnet_radvd_if_prefix_close(Ipnet_radvd_if_prefix_handle handle);

/*
 *===========================================================================
 *                    ipnet_radvd_if_prefix_get
 *===========================================================================
 * Description: Get prefix configuration data for the specifed handle.
 * Parameters:  handle - Prefix configuration handle.
 *              data -   Pointer to returned configuration data.
 * Returns:     IPCOM error code.
 *
 */
IP_PUBLIC Ip_err
ipnet_radvd_if_prefix_get(Ipnet_radvd_if_prefix_handle handle,
                          Ipnet_radvd_if_prefix_data *data);

/*
 *===========================================================================
 *                    ipnet_radvd_if_prefix_set
 *===========================================================================
 * Description: Set prefix configuration data for the specifed handle.
 * Parameters:  handle - Prefix configuration handle.
 *              data -   Pointer to new configuration data.
 * Returns:     IPCOM error code.
 *
 */
IP_PUBLIC Ip_err
ipnet_radvd_if_prefix_set(Ipnet_radvd_if_prefix_handle handle,
                          const Ipnet_radvd_if_prefix_data *data);


/*
 *===========================================================================
 *                    ipnet_radvd_main
 *===========================================================================
 * Description: RADVD main function
 * Parameters:  do_ipd - Setup IPD; mainly used for LAS
 * Returns:     No return value
 *
 */
IP_GLOBAL void ipnet_radvd_main(Ip_bool do_ipd);


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
