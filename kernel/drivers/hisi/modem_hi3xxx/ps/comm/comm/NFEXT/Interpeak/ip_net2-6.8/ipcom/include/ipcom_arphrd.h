
/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_arphrd.h,v $ $Revision: 1.2 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_arphrd.h,v $
 *     $Author: jhorteli $ $Date: 2009-04-23 15:57:05 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Joel Hortelius <joel.hortelius@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_ARPHRD_H
#define IPCOM_ARPHRD_H

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

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IP_ARPHRD_ETHER         IP_IFT_ETHER    /* Ethernet 10Mbps              */
#define IP_ARPHRD_TUNNEL        IP_IFT_TUNNEL   /* IPIP tunnel                  */
#define IP_ARPHRD_LOOPBACK      IP_IFT_LOOP     /* Loopback device              */
#define IP_ARPHRD_VOID          IP_IFT_OTHER    /* Void type, nothing is known */
#define IP_ARPHRD_PPP           IP_IFT_PPP
#define IP_ARPHRD_IEEE80211     IP_IFT_IEEE80211

/* Compatible with Linux */
#define IP_ARPHRD_SIT           776             /* sit0 device - IPv6-in-IPv4   */
#define IP_ARPHRD_TUNNEL6       769             /* IP6IP6 tunnel                */
#define IP_ARPHRD_IPGRE         778             /* GRE over IP                  */
#define IP_ARPHRD_NONE          0xfffe

/* Compatible with Linux; WR only */
#define IP_ARPHRD_TUNNEL6TO4    900
#define IP_ARPHRD_TUNNEL6OVER4  901
#define IP_ARPHRD_IPGRE6        902

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

#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
