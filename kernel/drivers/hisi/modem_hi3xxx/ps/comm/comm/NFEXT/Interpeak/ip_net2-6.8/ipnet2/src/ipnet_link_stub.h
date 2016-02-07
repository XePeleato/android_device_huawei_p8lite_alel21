/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_link_stub.h,v $ $Revision: 1.1 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_link_stub.h,v $
 *     $Author: braghava $ $Date: 2008-09-03 10:12:55 $
 *     $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *      Design and implementation by Balaji Raghavan(braghava@windriver.com)
 ******************************************************************************
 */
#ifndef IPNET_LINK_STUB_H
#define IPNET_LINK_STUB_H

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
#include "ipnet_h.h"
#include "ipnet_neigh.h"
#include "ipnet_timeout.h"

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
#define IPNET_LINK_STUB_P_ALL            0
#define IPNET_LINK_STUB_P_IP             0x0800
#define IPNET_LINK_STUB_P_IPV6           0x86DD


/* Link stub dummy types: */
#define IPNET_LINK_STUBFRAME_IP              ip_htons(IPNET_LINK_STUB_P_IP)
#define IPNET_LINK_STUBFRAME_IPV6            ip_htons(IPNET_LINK_STUB_P_IPV6)


/* Link stub dummy MTU/MRU. */
/* !! Have to set the MTU and LINK address sizes for IPNET to function
   properly. Also set IP_LINKNAMSIZ in ipcom/include/ipcom_netif.h !!*/

#define IPNET_LINK_STUB_MTU          1500


/* Size of Link stub dummy address */
#define IPNET_LINK_STUB_ADDR_SIZE    6

/* Size of Link stub dummy link header. */
#define IPNET_LINK_STUB_HDR_SIZE     0

/* Number of bits in the host part (EUI-64) when using the
   ethernet MAC to create a IPv6 address (RFC 2464) */
#define IPNET_LINK_STUB_IP6_ID_BITS  64


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/* Forward declarations */
struct Ipnet_route_entry_struct;
struct Ipnet_netif_struct;


/*
 *===========================================================================
 *                         Ipnet_pkt_eth
 *===========================================================================
 */


/*
 *===========================================================================
 *                         Ipnet_netif_link_stub
 *===========================================================================
 * Link layer specific data if required.
 */


/*
****************************************************************************
 * 6                    PROTOTYPES
 ****************************************************************************
 */


IP_PUBLIC int ipnet_link_stub_if_init(struct Ipnet_netif_struct *netif);
IP_GLOBAL int ipnet_link_stub_add_hdr(struct Ipnet_netif_struct *netif, Ipcom_pkt *pkt,
                                struct Ip_sockaddr *dst, Ip_u16 type);

#ifdef __cplusplus
}
#endif

#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
