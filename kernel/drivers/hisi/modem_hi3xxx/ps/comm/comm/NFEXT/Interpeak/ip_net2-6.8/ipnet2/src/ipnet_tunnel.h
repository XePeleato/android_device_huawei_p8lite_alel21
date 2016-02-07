/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_tunnel.h,v $ $Revision: 1.11 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_tunnel.h,v $
 *   $Author: jhorteli $ $Date: 2009-02-25 14:32:58 $
 *   $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPNET_TUNNEL_H
#define IPNET_TUNNEL_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/* IP over IP tunnel network interface for IPNET */

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
#include <ipcom_sock.h>

#include "ipnet_h.h"

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

#ifdef IPNET_USE_GRE
#define IPNET_GRE_MAX_HDR_SIZE     16 /* GRE header, checksum, key and sequence number */
#endif

/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

IP_GLOBAL Ip_u16 ipnet_tunnel_type(struct Ipnet_netif_struct   *netif);
IP_GLOBAL Ip_u16 ipnet_tunnel_get_basetype(int vr, char *name, Ip_size_t *sz);
IP_GLOBAL char *ipnet_tunnel_get_basename(int vr, Ip_u16 basetype, char *name);

IP_GLOBAL Ip_u16 ipnet_tunnel_get_type(char *name);
IP_GLOBAL int ipnet_tunnel_create_bsd(struct Ipnet_socket_struct *sock, struct Ip_ifreq *ifreq);
IP_GLOBAL int ipnet_tunnel_create_linux(struct Ipnet_socket_struct *sock, struct Ip_ifreq *ifreq);
IP_GLOBAL int ipnet_tunnel_add(struct Ipnet_socket_struct *sock, const char *name, Ip_u16 type, struct Ipnet_netif_struct **netifp);
IP_GLOBAL int ipnet_tunnel_setup(struct Ipnet_netif_struct *netif, int tunnel_type);
IP_GLOBAL int ipnet_tunnel_ip4_input(Ipcom_pkt *pkt);
IP_GLOBAL int ipnet_tunnel_ip6_input(Ipcom_pkt *pkt);
IP_GLOBAL int ipnet_tunnel_ip4_rfc3519_input(Ipcom_pkt *pkt);
IP_GLOBAL int ipnet_tunnel_dsmipv6_input(Ipcom_pkt *pkt);

IP_PUBLIC int ipnet_tunnel_if_init(struct Ipnet_netif_struct *netif);


/*
 *===========================================================================
 *                         GRE FUNCTIONS
 *===========================================================================
 *
 */

#ifdef IPNET_USE_GRE
IP_GLOBAL int ipnet_gre_setup(Ipnet_netif *netif);
IP_GLOBAL void ipnet_gre_destroy(Ipnet_netif *netif);
IP_GLOBAL int ipnet_gre_input(Ipnet_netif *netif, Ipcom_pkt *pkt);
IP_GLOBAL int ipnet_gre_output_append(Ipnet_netif *netif, struct Ip_ip_tunnel_param *tunnel_param, Ipcom_pkt *pkt);
#endif

#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
