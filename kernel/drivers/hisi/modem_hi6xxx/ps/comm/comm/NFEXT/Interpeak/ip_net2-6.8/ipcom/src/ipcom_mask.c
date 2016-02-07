/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_mask.c,v $ $Revision: 1.2 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_mask.c,v $
 *     $Author: lob $ $Date: 2004-11-09 17:29:43 $
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


/*
 ****************************************************************************
 * 9                    FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_mask_to_prefixlen
 *===========================================================================
 * Description: Converts binary representation of a mask to the corresponding
 *              prefix len value (i.e. 255.255.0.0 -> 16).
 * Parameters:  mask - The mask in binary form
 *              mask_bit_len - The size of the mask in bits
 * Returns:     The prefix length value for the specified mask.
 *
 */
IP_PUBLIC int
ipcom_mask_to_prefixlen(IP_CONST void *mask, int mask_bit_len)
{
    int prefix_len;
    IP_CONST Ip_u8 *bm;
    Ip_u8 b;
    int i;

    bm = (IP_CONST Ip_u8 *) mask;
    for (prefix_len = 0; bm && (prefix_len < mask_bit_len); prefix_len += 8)
    {
        b = bm[prefix_len >> 3];
        if (b != 0xff)
        {
            for (i = 0; (0x80 >> i) & b; i++)
                prefix_len++;
            return prefix_len;
        }
    }
    return mask_bit_len;
}


/*
 *===========================================================================
 *                    ipcom_sockaddr_to_prefixlen
 *===========================================================================
 * Description: Returns the number of leading '1' in the mask.
 * Parameters:  sa - The mask.
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_sockaddr_to_prefixlen(struct Ip_sockaddr *sa)
{
    int af;

    af = sa->sa_family;
#ifdef IPCOM_USE_INET
    if (af == IP_AF_INET)
    {
        return ipcom_mask_to_prefixlen(
            &((struct Ip_sockaddr_in *) sa)->sin_addr,
            sizeof(struct Ip_in_addr) * 8);
    }
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    if (af == IP_AF_INET6)
    {
        return ipcom_mask_to_prefixlen(
            &((struct Ip_sockaddr_in6 *) sa)->sin6_addr,
            sizeof(struct Ip_in6_addr) * 8);
    }
#endif /* IPCOM_USE_INET6 */

    IP_PANIC2();
    return -IP_ERRNO_EAFNOSUPPORT;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */


