/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_getinfo.c,v $ $Revision: 1.35 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_getinfo.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Markus Carlstedt <markus@interpeak.se>
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

#if defined(IP_PORT_OSE5)
#include "osarch.h" /* Will set softOSE appropriately */
#include "errno.h"
#include "netdb.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#ifdef s6_addr
#undef s6_addr
#endif
#include "ipcom_cstyle.h"
#include "ipcom_clib.h"
#include "ipcom_sock.h"
#include "ipcom_sock2.h"

#else

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_cstyle.h"
#include "ipcom_clib.h"
#include "ipcom_sock.h"
#include "ipcom_sock2.h"

#endif /* IP_PORT_OSE5 */


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#if !defined(IP_PORT_VXWORKS) || defined(_WRS_KERNEL)
#define IPCOM_USE_GETINFO
#endif


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

#ifdef IPCOM_USE_GETINFO
IP_PUBLIC int
ipcom_getnameinfo2(IP_CONST struct Ip_sockaddr *sa, Ip_socklen_t salen,
                   char *host, Ip_size_t hostlen,
                   char *serv, Ip_size_t servlen,
                   int flags);

IP_PUBLIC int
ipcom_getaddrinfo2(IP_CONST char *hostname,
                   IP_CONST char *servname,
                   IP_CONST struct Ip_addrinfo *hints,
                   struct Ip_addrinfo **res);
#endif /* IPCOM_USE_GETINFO */

IP_PUBLIC void
ipcom_freeaddrinfo2(struct Ip_addrinfo *ai);


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

#if defined(IP_PORT_OSE5)
/* Start by assuming that getaddrinfo is supported.
 * The first call to ipcom_getaddrinfo() will test if getaddrinfo()
 * is supported. If it is not the fallback flag is set, and
 * ipcom_getaddrinfo2() is called.
 * A subsequent call to ipcom_freeaddrinfo() will use ipcom_freeaddrinfo2().
 * We will never run in into a situation where we have a addrinfo struct
 * from CRT with fallback flag set, since no addrinfo struct is returned
 * when getaddrinfo() is found to be unsupported.
 */
static int ipcom_getaddrinfo_fallback = 0;
#endif


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#ifdef IPCOM_USE_GETINFO
/*
 *===========================================================================
 *                    ipcom_getnameinfo
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_getnameinfo(IP_CONST struct Ip_sockaddr *sa,
                  Ip_socklen_t salen,
                  char *host,
                  Ip_size_t hostlen,
                  char *serv,
                  Ip_size_t servlen,
                  int flags)
{
#if defined(IP_PORT_OSE5)
    int ret;

    ret = getnameinfo((const struct sockaddr *)sa, (socklen_t)salen,
                      host, (socklen_t)hostlen,
                      serv, (socklen_t)servlen,
                      flags);
    if (ret != 0 && errno == EOPNOTSUPP)
    {
        /* Fall back to ipcom_getnameinfo2() */
        ret = ipcom_getnameinfo2(sa, salen, host, hostlen, serv, servlen, flags);
    }

    return ret;
#else
    return ipcom_getnameinfo2(sa, salen, host, hostlen, serv, servlen, flags);
#endif /* IP_PORT_OSE5 */
}


/*
 *===========================================================================
 *                    ipcom_getaddrinfo
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_getaddrinfo(IP_CONST char *hostname,
                  IP_CONST char *servname,
                  IP_CONST struct Ip_addrinfo *hints,
                  struct Ip_addrinfo **res)
{
#if defined(IP_PORT_OSE5)
    int ret;

    if (ipcom_getaddrinfo_fallback == 0)
    {
        ret = getaddrinfo("127.0.0.1", IP_NULL, IP_NULL, IP_NULL);
        if (ret == EAI_SYSTEM && errno == EOPNOTSUPP)
            /* Fall back to ipcom_getnameinfo2(), but set ipcom_getaddrinfo_fallback
             * flag first to steer ipcom_freeaddrinfo() via ipcom_freeaddrinfo2().
             */
            ipcom_getaddrinfo_fallback = 2;
        else
            ipcom_getaddrinfo_fallback = 1;
    }

    if (ipcom_getaddrinfo_fallback == 1)
        ret = getaddrinfo((const char *)hostname, (const char *)servname,
                          (const struct addrinfo *)hints,
                          (struct addrinfo **)res);
    else
        /* Use fallback solution via ipcom_getaddrinfo2() */
        ret = ipcom_getaddrinfo2(hostname, servname, hints, res);
    return ret;
#else
    return ipcom_getaddrinfo2(hostname, servname, hints, res);
#endif /* IP_PORT_OSE5 */
}
#endif /* IPCOM_USE_GETINFO */


/*
 *===========================================================================
 *                    ipcom_freeaddrinfo
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_freeaddrinfo(struct Ip_addrinfo *ai)
{
#if defined(IP_PORT_OSE5)
    ip_assert(ipcom_getaddrinfo_fallback > 0);
    if (ipcom_getaddrinfo_fallback == 1)
        freeaddrinfo((struct addrinfo *)ai);
    else
        ipcom_freeaddrinfo2(ai);
#else
    ipcom_freeaddrinfo2(ai);
#endif
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
