/*
 ******************************************************************************
 *                     INTERPEAK INTERNAL API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_send.h,v $ $Revision: 1.1 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_send.h,v $
 *   $Author: rboden $ $Date: 2008-06-09 09:21:55 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Roger Boden
 ******************************************************************************
 */
#ifndef IPNET_SEND_H
#define IPNET_SEND_H

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
#include <ipcom_pkt.h>

#include "ipnet_netif.h"
#include "ipnet_route.h"
#include "ipnet_ip6.h"


#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_SEND_PKT_NOT_CONSUMED 1
#define IPNET_SEND_PKT_CONSUMED     2

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
IP_GLOBAL int
ipnet_send_sign_verify(Ipnet_netif *netif,
                       IP_CONST struct Ip_in6_addr *src,
                       int hlim,
                       Ipcom_pkt* icmp6_pkt);
IP_GLOBAL int
ipnet_send_sign_create(Ipnet_netif *netif,
                       Ipnet_route_entry* rt_next_hop,
                       Ipcom_pkt* icmp_pkt);


IP_GLOBAL void
ipnet_send_log_hex_buffer(Ip_u8* buf, int len);

#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
