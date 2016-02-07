/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_wlan.h,v $ $Revision: 1.4 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_wlan.h,v $
 *     $Author: kenneth $ $Date: 2005-09-06 10:47:05 $
 *     $State: Exp $ $Locker:  $
 *
 *   Copyright Interpeak AB 2000-2004 <www.interpeak.se>. All rights reserved.
 *      Design and implementation by Niklas Lundberg <n@interpeak.se>
 ******************************************************************************
 */
#ifndef IPNET_WLAN_H
#define IPNET_WLAN_H

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

#include <ipcom_hash.h>
#include "ipnet_netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Frame Values
 *===========================================================================
 * Note: Also used for 3:rd argument to when creating packet sockets.
 */
#define IPNET_WLAN_P_ALL        0
#define IPNET_WLAN_P_IP         0x0800
#define IPNET_WLAN_P_ARP        0x0806
#define IPNET_WLAN_P_RARP       0x8035
#define IPNET_WLAN_P_IPV6       0x86DD
#define IPNET_WLAN_P_PPPOE_DIS  0x8863
#define IPNET_WLAN_P_PPPOE_SES  0x8864
#define IPNET_WLAN_P_8021Q_VLAN 0x8100

/* Ethernet MTU/MRU. */
#define IPNET_WLAN_MTU          1500

/* Size of MAC address */
#define IPNET_WLAN_ADDR_SIZE    6

/* Size of WLAN link header. (only applies to data frames) */
#define IPNET_WLAN_HDR_SIZE     30

/* Size of Ethernet link trailer (not used by IP layers) */
#define IPNET_WLAN_TRL_SIZE     6 /*!! FCS=4 */

/* Number of bits in the host part (EUI-64) when using the
   MAC addr to create a IPv6 address (RFC 2464) */
#define IPNET_WLAN_IP6_ID_BITS  64


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipnet_netif_wlan_vlan
 *===========================================================================
 */
typedef struct Ipnet_netif_wlan_vlan_struct
{
    Ipnet_netif *parent; /* Parent WLAN interface */
    int          id;     /* Virtual LAN ID 1-4095 */
}
Ipnet_netif_wlan_vlan;


/*
 *===========================================================================
 *                         Ipnet_netif_wlan
 *===========================================================================
 * WLAN specific data
 */
typedef struct Ipnet_netif_wlan_struct
{
#ifdef IPNET_USE_VLAN
    union
    {
        Ipcom_hash            *ifs;    /* All virtual interfaces that has this interface as parent */
        Ipnet_netif_wlan_vlan  param;  /* Virtual LAN parameters */
    } vlan;
#endif

    int (*eth_link_ioctl)(Ipnet_netif *, Ip_u32, void *);
    int (*aux_link_ioctl)(Ipnet_netif *, Ip_u32, void *);

    /* NIKLAS INTERFACE SPECIFIKA DATA HERE: */
    /* 4 nycklar, kanal, etc */
}
Ipnet_netif_wlan;


/*
 ****************************************************************************
 * 6                    PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC int ipnet_wlan_if_init(Ipnet_netif *netif);

#ifdef __cplusplus
}
#endif

#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
