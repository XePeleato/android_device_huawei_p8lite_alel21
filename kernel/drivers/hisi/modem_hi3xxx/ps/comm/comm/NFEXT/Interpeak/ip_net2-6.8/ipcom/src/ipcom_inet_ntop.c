/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_inet_ntop.c,v $ $Revision: 1.9 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_inet_ntop.c,v $
 *   $Author: kenneth $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
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
#include "ipcom_clib.h"
#include "ipcom_sock.h"
#include "ipcom_sock6.h"


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
 *===========================================================================
 *                    ipcom_char_append
 *===========================================================================
 * Description: Appends a character to a string and verifies that the string
 *              is long enough.
 * Parameters:  tkn - The character to append.
 *              str - The string to append to, the string will be point to
 *              the next character beyond the appended one.
 *              len - Must contain the current length of 'str' when calling,
 *              will be updated with the new length.
 * Returns:     IP_TRUE if the character was appended.
 *
 */
IP_STATIC Ip_bool
ipcom_char_append(char tkn, char **str, Ip_size_t *len)
{
    if (*len <= 1)
        return IP_FALSE;
    **str = tkn;
    (*str)++;
    **str = '\0';
    (*len)--;
    return IP_TRUE;
}


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
 *                    ipcom_inet_ntop
 *===========================================================================
 * Description: Converts the network address into a character string.
 * Parameters:  family - The address family for the address to convert.
 *              strptr - A pointer to the buffer that will receive the
 *              string representation of the address.
 *              len - The length of the strptr buffer.
 * Returns:     The strptr value if the call was successful or IP_NULL
 *              if failed (get error code from ipcom_errno).
 *
 */
IP_PUBLIC IP_CONST char *
ipcom_inet_ntop(int family, IP_CONST void *addrptr, char *strptr, Ip_size_t len)
{
    char *str = strptr;

#ifdef IPCOM_USE_INET
    if (family == IP_AF_INET)
    {
        struct Ip_in_addr in_val;
        int octet;
        int b, s;

        ipcom_memcpy(&in_val.s_addr, addrptr, sizeof(in_val));
        for (b = 24; b >= 0; b -= 8)
        {
            octet = (IP_SWAP32(in_val.s_addr) >> b) & 0xFF;
            for (s = 100; s > 0; s /= 10)
                if (octet >= s || s == 1)
                {
                    if (IP_FALSE == ipcom_char_append(
                        (char)('0' + (octet % (s * 10)) / s), &str, &len))
                        goto errout;
                }
            if (b != 0 && ipcom_char_append('.', &str, &len) == IP_FALSE)
                goto errout;
        }
        return strptr;
    }
    else
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    if (family == IP_AF_INET6)
    {
        struct Ip_in6_addr in6_val;
        int n, w, word;
        char nibble;
        Ip_bool dci = IP_FALSE; /* is double colon inserted */
        Ip_bool dcc = IP_FALSE; /* is double colon being inserted */

        ipcom_memcpy(&in6_val, addrptr, sizeof(in6_val));
        if (IP_IN6_IS_ADDR_V4COMPAT(&in6_val))
        {
            /* Compat addresses is formatted like this ::x.y.z.w */
            if (ipcom_char_append(':', &str, &len) == IP_FALSE
                || ipcom_char_append(':', &str, &len) == IP_FALSE)
                goto errout;
            if (ipcom_inet_ntop(IP_AF_INET, &in6_val.in6.addr16[6], str, len) == IP_NULL)
                return IP_NULL;
            return strptr;
        }

        if (IP_IN6_IS_ADDR_V4MAPPED(&in6_val))
        {
            /* Ipv6 mapped Ipv4 addresses is formatted like this ::ffff:x.y.z.w */
            if (8 >= len)
                goto errout;
            ipcom_strcpy(strptr, "::ffff:");
            if (ipcom_inet_ntop(IP_AF_INET, &in6_val.in6.addr16[6], strptr + 7, len - 7) == IP_NULL)
                return IP_NULL;
            return strptr;
        }

        for (w = 0; w < 8; w++)
        {
            word = IP_SWAP16(in6_val.in6.addr16[w]);
            if (word == 0 && dci == IP_FALSE)
            {
                dcc = IP_TRUE;
                if (w == 0 && ipcom_char_append(':', &str, &len) == IP_FALSE)
                    goto errout;
                continue;
            }

            if (dci == IP_FALSE && dcc == IP_TRUE)
            {
                if (ipcom_char_append(':', &str, &len) == IP_FALSE)
                    goto errout;
                dci = IP_TRUE;
            }

            for (n = 12; n >= 0; n -= 4)
            {
                /* Skip leading zeros */
                if (n == 0 || word >= (0x1 << n))
                {
                    nibble = (char) ((word >> n) & 0xF);
                    if (IP_FALSE == ipcom_char_append(
                        (char)(nibble > 9 ? 'a' + nibble - 10 : '0' + nibble), &str, &len))
                        goto errout;
                }
            }
            if (w != 7 && ipcom_char_append(':', &str, &len) == IP_FALSE)
                goto errout;
        }
        if (dci == IP_FALSE && dcc == IP_TRUE)
            if (ipcom_char_append(':', &str, &len) == IP_FALSE)
                goto errout;

        return strptr;
    }
    else
#endif /* IPCOM_USE_INET6 */
    {
        (void)ipcom_errno_set(IP_ERRNO_EAFNOSUPPORT);
        return IP_NULL;
    }

errout:
    (void)ipcom_errno_set(IP_ERRNO_ENOSPC);
    return IP_NULL;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

