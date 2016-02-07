/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_sysctl.h,v $ $Revision: 1.10 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_sysctl.h,v $
 *   $Author: ulf $ $Date: 2009-06-08 11:04:41 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_SYSCTL_H
#define IPCOM_SYSCTL_H

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

#include "ipcom_config.h"


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

/*
    1:st level
        2:nd level
            3:rd level
                4:th level
                    5:th level
                        6:th level
    IP_CTL_NET
        IP_AF_ROUTE
            0
                0 | IP_AF_INET | IP_AF_INET6 (which af to return, 0=all)
                    IP_NET_RT_DUMP
                        0   (dump all route table entries)
                    IP_NET_RT_FLAGS
                        IPNET_RTF_xxx (dump route entries that has this flag set)
                    IP_NET_RT_IFLIST
                        0       (dump address information for all interfaces)
                        netif   (dump address information for this interface)
        IP_AF_INET
            IP_IPPROTO_IP
                IP_IPCTL_FORWARDING      (forward IPv4 packets between interfaces)
                IP_IPCTL_SENDREDIRECTS   (enable/disable sending of ICMP redirect)
                IP_IPCTL_MTUDISC         (enable/disable path MTU discovery)
            IP_IPPROTO_UDP
                IP_UDPCTL_CHECKSUM       (enable/disable UDP checksum when sending)
        IP_AF_INET6
            IP_IPPROTO_IPV6
                IP_IPV6CTL_FORWARDING    (forward IPv6 packets between interfaces)
                IP_IPV6CTL_ACCEPT_RTADV  (enable/disable router advertisment processing)
                    netif
                IP_IPV6CTL_ENABLED       (enable/disable IPv6 processing on interface)
                    netif
*/

#define IP_CTL_MAXNAME   6   /* Max name length */

#if defined(IP_PORT_VXWORKS) && IP_PORT_VXWORKS >= 65
#define IP_CTL_NET   4   /* Networking */
#else
#define IP_CTL_NET   3   /* Networking */
#endif


/* Networking */
#define IP_NET_RT_DUMP    1   /* Dump all route entries */
#define IP_NET_RT_FLAGS   2   /* Dump all route entries with a specific flag set */
#define IP_NET_RT_IFLIST  3   /* Dump interface address information */
#define IP_NET_RT_MAXID   4

/* IPv4 */
#define IP_IPCTL_FORWARDING      1   /* IPv4 packet forwardning on/off */
#define IP_IPCTL_SENDREDIRECTS   2   /* Send ICMP redirect if in- and out interface is the same */
#define IP_IPCTL_STAT            3   /* IP protocol statistics show or clear*/
#define IP_IPCTL_MTUDISC         9   /* Enable/disable path MTU discovery */

/* IPv6 */
#define IP_IPV6CTL_FORWARDING    1   /* IPv6 packet forwardning on/off */
#define IP_IPV6CTL_ACCEPT_RTADV  2   /* IPv6 router advertisment processing on/off */
#define IP_IPV6CTL_MIP6          3   /* MIP6 sysctl */
#define IP_IPV6CTL_ENABLED       4   /* IPv6 processing enabled/disabled */

/* UDP */
#define IP_UDPCTL_CHECKSUM       1   /* Calculate checksum on outgoing UDP packets */


/*
 *===========================================================================
 *            User library support
 *===========================================================================
 */
#if (!defined(IPCOM_KERNEL) && defined(IPCOM_USRLIB)) || defined(IPCOM_USE_REMOTE_STACK)
#define ipcom_sysctl   ipcom_sysctl_usr
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

IP_PUBLIC int
ipcom_sysctl(int *name, unsigned int namelen,
             void *oldp, Ip_size_t *oldlenp,
             void *newp, Ip_size_t newlen);


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
