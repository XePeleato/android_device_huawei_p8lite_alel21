/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_vrrp.h,v $ $Revision: 1.2 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_vrrp.h,v $
 *   $Author: kenneth $ $Date: 2005-03-22 10:04:05 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPNET_VRRP_H
#define IPNET_VRRP_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Implementation of RFC3768 - Virtual Router Redundancy Protocol (VRRP)
 * This file contains helper functions used within the stack.
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

#include <ipcom_cstyle.h>
#include <ipcom_type.h>


#ifdef __cplusplus
extern "C" {
#endif

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

/* Structure that holds VRID/network interface to address(es) mapping */
typedef struct Ipnet_vrrp_addr_struct
{
    Ipnet_netif *netif;         /* The interface the virtual addresses are assigned to */
    Ip_u8        vrid;          /* The virtual router ID of the addresses */
    Ip_u8        num_addrs;     /* Number of addresses must be >=1 */
    struct Ip_in_addr addrs[1]; /* One or more addresses assigned to this VRID */
}
Ipnet_vrrp_addr_t;


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

IP_GLOBAL Ipnet_vrrp_addr_t * ipnet_vrrp_get_addr_entry(Ipnet_netif *netif, Ip_u8 vrid);
IP_GLOBAL Ip_bool ipnet_vrrp_owns_addr(Ipnet_netif *netif, Ip_u8 vrid, struct Ip_in_addr addr);
IP_GLOBAL int ipnet_vrrp_add_addr(Ipnet_netif *netif, Ip_u8 vrid, struct Ip_in_addr addr);
IP_GLOBAL int ipnet_vrrp_del_addr(Ipnet_netif *netif, Ip_u8 vrid, struct Ip_in_addr addr);

#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
