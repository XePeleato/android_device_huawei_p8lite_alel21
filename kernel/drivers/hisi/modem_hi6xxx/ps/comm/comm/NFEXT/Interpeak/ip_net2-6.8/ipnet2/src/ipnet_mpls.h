/*
 ******************************************************************************
 *                     INTERPEAK HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_mpls.h,v $ $Revision: 1.11 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_mpls.h,v $
 *   $Author: jonas $ $Date: 2009-03-12 11:56:19 $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright Interpeak AB 2000-2005 <www.interpeak.se>. All rights reserved.
 *     Design and implementation by Jonas Rendel <Jonas.Rendel@interpeak.se>
 ******************************************************************************
 */
#ifndef IPNET_MPLS_H
#define IPNET_MPLS_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_type.h>
#include "ipnet_config.h"
#include "ipnet_route.h"
#include "ipnet_eth.h"

#include <ipmpls_ipstack.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IP_IFT_MPLS_FIBIF  0x384  /* Internal MPLS to FIB map interface */

#define IPNET_ETHFRAME_MPLS_UNICAST    ip_htons(IPNET_ETH_P_MPLS_UNICAST)
#define IPNET_ETHFRAME_MPLS_MULTICAST  ip_htons(IPNET_ETH_P_MPLS_MULTICAST)

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

IP_GLOBAL void ipnet_mpls_route_table_notify(Ip_u16 vrtab_index, Ip_bool is_added);
IP_GLOBAL int ipnet_mpls_shortcut_route_setup(Ip_u16 vr,
                                        Ipnet_route_entry *rt,
                                        Ipnet_netif **pnetif);
IP_GLOBAL void ipnet_mpls_shortcut_route_clear(Ipnet_route_entry *rt);
IP_GLOBAL int ipnet_mpls_if_init(Ipnet_netif *netif);
IP_GLOBAL int ipnet_mpls_init(void);
IP_GLOBAL int ipnet_mpls_input(struct Ipnet_netif_struct *netif, Ipcom_pkt *pkt);
IP_GLOBAL void ipnet_mpls_probe_reply(Ipnet_netif *netif, Ipcom_pkt *pkt,
                                      struct Ip_sockaddr *dst);
IP_GLOBAL IP_CONST void *ipnet_mpls_get_src_addr(Ip_sa_family_t sa_family,
                                                 Ipnet_netif *fibif, void* dst);

#ifdef __cplusplus
}
#endif

#endif
