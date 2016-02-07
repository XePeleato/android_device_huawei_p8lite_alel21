/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_inet.c,v $ $Revision: 1.18 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_inet.c,v $
 *     $Author: jhorteli $ $Date: 2009-02-25 14:52:24 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */

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

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_inet.h"
#include "ipcom_sock.h"
#include "ipcom_clib.h"


/*
 ****************************************************************************
 * 4                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 5                    DEFINES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 6                    TYPES
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
IP_PUBLIC IP_CONST struct Ip_in_addr ip_inaddr_any      = { IP_INADDR_ANY };

/*
 ****************************************************************************
 * 9                    FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_ipproto_name
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC char *
ipcom_ipproto_name(Ip_u8 protocol)
{
    static char ipprot[10];

    switch(protocol)
    {
    case 0:                 return "any";
    case IP_IPPROTO_ICMP:   return "ICMP";
    case IP_IPPROTO_IGMP:   return "IGMP";
    case IP_IPPROTO_IPIP:   return "IPIP";
    case IP_IPPROTO_TCP:    return "TCP ";
    case IP_IPPROTO_UDP:    return "UDP ";
    case IP_IPPROTO_IPV6:   return "IPV6";
    case IP_IPPROTO_RSVP:   return "RSVP";
    case IP_IPPROTO_GRE:    return "GRE ";
    case IP_IPPROTO_ESP:    return "ESP ";
    case IP_IPPROTO_AH:     return "AH  ";
    case IP_IPPROTO_ICMPV6: return "ICMPV6";
    case IP_IPPROTO_PIM:    return "PIM ";
    case IP_IPPROTO_RAW:    return "RAW ";
    case IP_IPPROTO_MH:     return "MH  ";
    default:
        break;
    }

    ipcom_sprintf(ipprot, "#%ld ", (Ip_s32)protocol);
    return (char *)ipprot;
}


/*
 *===========================================================================
 *                    ipcom_cmsg_nxthdr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC struct Ip_cmsghdr *
ipcom_cmsg_nxthdr(IP_CONST struct Ip_msghdr *msgp, struct Ip_cmsghdr *cmsgp)
{
    Ip_ptrdiff_t clen;
    Ip_ptrdiff_t mlen;

    clen = (Ip_ptrdiff_t) cmsgp + IP_CMSG_ALIGN(cmsgp->cmsg_len) + IP_CMSG_ALIGN(sizeof(struct Ip_cmsghdr));
    mlen = (Ip_ptrdiff_t) msgp->msg_control + msgp->msg_controllen;

    if (clen >= mlen)
        return IP_NULL;
    else
        return (struct Ip_cmsghdr *)((Ip_ptrdiff_t)(cmsgp) + IP_CMSG_ALIGN(cmsgp->cmsg_len));
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */


