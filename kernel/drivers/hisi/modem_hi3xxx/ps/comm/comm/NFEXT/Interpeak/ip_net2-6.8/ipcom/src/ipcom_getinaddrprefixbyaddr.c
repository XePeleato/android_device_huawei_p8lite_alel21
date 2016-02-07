/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_getinaddrprefixbyaddr.c,v $ $Revision: 1.4 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_getinaddrprefixbyaddr.c,v $
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
#include "ipcom_type.h"
#include "ipcom_clib.h"
#include "ipcom_cstyle.h"
#include "ipcom_inet.h"

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
 *                    ipcom_getsockaddrbyaddr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_getinaddrprefixbyaddr(int domain, IP_CONST char *straddr, void *inprefix, int *prefixlen)
{
    char *str_prefix;
    char *str_len;
    int   max_len = 0;
    int   ret = IP_ERRNO_EINVAL;

#ifdef IPCOM_USE_INET
    if (domain == IP_AF_INET)
        max_len = 31;
#endif
#ifdef IPCOM_USE_INET6
    if (domain == IP_AF_INET6)
        max_len = 127;
#endif

    str_len = str_prefix = ipcom_strdup(straddr);
    if (str_prefix == IP_NULL)
    {
        ret = IP_ERRNO_ENOMEM;
        goto cleanup;
    }

    while (*str_len != '/')
    {
        if (*str_len == '\0')
            goto cleanup;
        str_len++;
    }
    *str_len++ = '\0';
    if (ipcom_inet_pton(domain, str_prefix, inprefix) != 1)
        goto cleanup;

    *prefixlen = ipcom_atoi(str_len);
    if (*prefixlen < 1 || *prefixlen > max_len)
        goto cleanup;

    ret = 0;
 cleanup:
    ipcom_free(str_prefix);
    (void)ipcom_errno_set(ret);
    return ret == 0 ? 0 : -1;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

