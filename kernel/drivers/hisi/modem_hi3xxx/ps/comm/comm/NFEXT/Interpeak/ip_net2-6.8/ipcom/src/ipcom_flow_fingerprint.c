/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_flow_fingerprint.c,v $ $Revision: 1.3 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_flow_fingerprint.c,v $
 *   $Author: kenneth $
 *   $State: Exp $ $Locker:  $
 *
 * INTERPEAK_COPYRIGHT_STRING
 * Design and implementation by Kenneth Jonsson <kenneth.jonsson@windriver.com>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */
/*
  Creates a flow finger print for each packet. Packets that belong to the
  same flow will have the same finger print.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_hash.h>
#include <ipcom_inet.h>
#include <ipcom_netif.h>
#include <ipcom_pkt.h>

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

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_flow_fingerprint
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC unsigned
ipcom_flow_fingerprint(Ipcom_netif *netif, Ipcom_pkt *pkt)
{
    Ipcom_ip_header *ipv4;

    ipv4 = (Ipcom_ip_header *) &pkt->data[pkt->start + netif->link_hdr_size];

    switch (ipv4->vers_len & 0xf0)
    {
#ifdef IPCOM_USE_INET
    case 0x40:
        return ipcom_hash_update(&ipv4->src, 8, ipv4->protocol + (ipv4->tos << 8));
#endif
#ifdef IPCOM_USE_INET6
    case 0x60:
        {
            Ipcom_ip6_header *ipv6;
            ipv6 = (Ipcom_ip6_header *) &pkt->data[pkt->start + netif->link_hdr_size];
            return ipcom_hash_update(&ipv6->src, 32, ipv6->ver_class_flow);
        }
#endif
    default:
        return ipcom_hash_update(&pkt->data[pkt->start], netif->link_hdr_size, 0);
    }
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

