/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_loopback.h,v $ $Revision: 1.10 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_loopback.h,v $
 *     $Author: kenneth $ $Date: 2005-11-10 10:05:24 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPNET_LOOPBACK_H
#define IPNET_LOOPBACK_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Loopback interface declarations.
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

#include "ipnet_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#ifdef IP_PORT_LKM
#define IPNET_IF_LOOPBACK_NAME       "lo"  /* Loopback name */
#else
#define IPNET_IF_LOOPBACK_NAME       "lo0" /* Loopback name */
#endif


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

IP_PUBLIC int ipnet_loopback_if_init(struct Ipnet_netif_struct *netif);
IP_GLOBAL int ipnet_loopback_attach(Ip_u16 vr, char *name);
IP_GLOBAL struct Ipnet_netif_struct *ipnet_loopback_get_netif(Ip_u16 vr);
IP_GLOBAL Ip_bool ipnet_is_loopback(Ip_u16 vr, Ip_u32 ifindex);

#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
