/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_sock2.c,v $ $Revision: 1.15 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_sock2.c,v $
 *     $Author: ulf $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *      Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Old socket functions (do not use for new code).
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
#include "ipcom_sock2.h"

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
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_inet_ntoa
 *===========================================================================
 * Description: Converts the Internet host address 'in' given in network
 *              byte order to a string in dotted quad format.
 *              The string returned is a statically allocated buffer.
 * Parameters:  in - Host address in network byte order.
 * Returns:     A string version of the address.
 *
 */
IP_PUBLIC char *
ipcom_inet_ntoa(const struct Ip_in_addr in)
{
    static char cp[16];

    (void)ipcom_inet_ntop(IP_AF_INET, (IP_CONST void *)&in, cp, 16);
    return cp;
}


/*
 *===========================================================================
 *                    ipcom_inet_addr
 *===========================================================================
 * Description: Converts the Internet host address 'cp' from dotted quad
 *              notation into binary data.
 * Parameters:  cp - A text representation of an Internet address.
 * Returns:     The address is network byte order or IP_INADDR_NONE
 *              if the input is invalid.
 *
 */
IP_PUBLIC unsigned long
ipcom_inet_addr(const char *cp)
{
    unsigned long in;

    if (ipcom_inet_pton(IP_AF_INET, (IP_CONST char *)cp, &in) <= 0)
        return IP_INADDR_NONE;
    return in;
}


/*
 *===========================================================================
 *                    ipcom_inet_aton
 *===========================================================================
 * Description: Converts the Internet host address 'cp' from dotted quad
 *              notation into binary data and stores in in the structure that
 *              'addr' points to.
 * Parameters:  cp - A text representation of an Internet address.
 *              addr - A pointer to where the binary address should be stored.
 * Returns:     >0 success, 0 fail
 *
 */
IP_PUBLIC int
ipcom_inet_aton(const char *cp, struct Ip_in_addr *addr)
{
    return ipcom_inet_pton(IP_AF_INET, (IP_CONST char *)cp, addr);
}


/*
 *===========================================================================
 *                    ipcom_getprotobyname
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC struct Ip_protoent *
ipcom_getprotobyname(const char *name)
{
    IPCOM_UNUSED_ARG(name);

    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipcom_getprotobynumber
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC struct Ip_protoent *
ipcom_getprotobynumber(int proto)
{
    IPCOM_UNUSED_ARG(proto);

    return IP_NULL;
}

#if !defined(IP_PORT_LKM) && (!defined(IP_PORT_VXWORKS) || defined(IPCOM_KERNEL))
/*
 *===========================================================================
 *                    ipcom_socketpair
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_socketpair(int domain, int type, int protocol, int sv[2])
{
    struct Ip_addrinfo *res = IP_NULL;
    struct Ip_addrinfo  hints;
    int                 ret;
    int                 i;
    Ip_socklen_t        namelen;

    sv[0] = -1;
    sv[1] = -1;

    ipcom_memset(&hints, 0, sizeof(hints));
    hints.ai_family   = domain;
    hints.ai_socktype = type;
    hints.ai_protocol = protocol;

    ret = ipcom_getaddrinfo(IP_NULL, "0", &hints, &res);
    if (ret)
    {
        ipcom_errno_set(IP_ERRNO_EINVAL);
        goto errout;
    }

    for (i = 0; i < 2; i++)
    {
        sv[i] = ipcom_socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sv[i] < 0)
            goto errout;
    }

    /* Bind to an ephemeral port */
    if (ipcom_bind(sv[0], res->ai_addr, res->ai_addrlen) < 0)
        goto errout;

    /* Figure out which ephemeral port the stack selected */
    namelen = res->ai_addrlen;
    if (ipcom_getsockname(sv[0], res->ai_addr, &namelen) < 0)
        goto errout;

    if (res->ai_socktype == IP_SOCK_STREAM
        && ipcom_listen(sv[0], 1) < 0)
        goto errout;

    if (ipcom_connect(sv[1], res->ai_addr, res->ai_addrlen) < 0)
        goto errout;

    if (res->ai_socktype == IP_SOCK_STREAM)
    {
        int l = sv[0];

        sv[0] = ipcom_accept(l, IP_NULL, IP_NULL);
        ipcom_socketclose(l);
        if (sv[0] < 0)
            goto errout;
    }
    else
    {
        union Ip_sockaddr_union name_sv1;
        Ip_socklen_t            name_sv1_len = sizeof(name_sv1);

        if (ipcom_getsockname(sv[1],
                              (struct Ip_sockaddr *) &name_sv1,
                              &name_sv1_len) < 0)
            goto errout;

        if (ipcom_connect(sv[0],
                          (struct Ip_sockaddr *) &name_sv1,
                          res->ai_addrlen) < 0)
            goto errout;
    }

    ipcom_freeaddrinfo(res);
    return 0;

 errout:
    if (res != IP_NULL)
        ipcom_freeaddrinfo(res);
    for (i = 0; i < 2; i++)
        if (sv[i] >= 0)
            ipcom_socketclose(sv[i]);
    return -1;

}
#endif

#ifndef IPCOM_USE_SA_LEN
/*
 *===========================================================================
 *                    ipcom_sa_get_len
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_u8
ipcom_sa_get_len(const struct Ip_sockaddr *sa)
{
    switch (sa->sa_family)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        return sizeof(struct Ip_sockaddr_in);
#endif
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        return sizeof(struct Ip_sockaddr_in6);
#endif
    case IP_AF_MPLS:
        return sizeof(struct Ip_sockaddr_mpls);
#ifdef IPCOM_USE_PACKET
    case IP_AF_PACKET:
        return sizeof(struct Ip_sockaddr_ll);
#endif
    case IP_AF_NETLINK:
        return sizeof(struct Ip_sockaddr_nl);
    case IP_AF_LINK:
        {
            struct Ip_sockaddr_dl *sdl = (struct Ip_sockaddr_dl *)sa;
            return
                ip_offsetof(struct Ip_sockaddr_dl, sdl_data) +
                IP_MAX(sizeof(sdl->sdl_data), (sdl->sdl_alen + sdl->sdl_nlen));
        }
    default:
    case IP_AF_UNSPEC:
        return sizeof(struct Ip_sockaddr);
    }
}
#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

