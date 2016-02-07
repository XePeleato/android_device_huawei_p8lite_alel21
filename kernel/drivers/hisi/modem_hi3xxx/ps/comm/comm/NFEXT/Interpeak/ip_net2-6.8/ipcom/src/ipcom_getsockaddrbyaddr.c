/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_getsockaddrbyaddr.c,v $ $Revision: 1.13.20.1 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_getsockaddrbyaddr.c,v $
 *   $Author: ulf $
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
#include "ipcom_inet.h"
#include "ipcom_sock2.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* The delimiter between the address and the interface for scoped addresses
 * ex FE80::200:30FF:FE40:5060%eth0
 */
#define IPCOM_IF_DELIMITER  '%'


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
 * Description:  Convert the presentation form of a address to a socket address.
 * Parameters:   domain - Address domain (IP_AF_LINK, IP_AF_INET or IP_AF_INET6).
 *               straddr - The presentation form of the address.
 *               sa - Will hold the socket address.
 * Returns:      0 for success else negative errno code (ipcom_perrno.h).
 *
 */
IP_PUBLIC int
ipcom_getsockaddrbyaddr(int domain, IP_CONST char *straddr, struct Ip_sockaddr *sa)
{
    int ret;
#if defined(IPCOM_USE_INET6) || defined(IPCOM_USE_PACKET)
    const char *ifname = IP_NULL;
#endif

    switch (domain)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        ipcom_memset(sa, 0, sizeof(struct Ip_sockaddr_in));
        IPCOM_SA_LEN_SET(sa, sizeof(struct Ip_sockaddr_in));
        sa->sa_family = IP_AF_INET;
        if (straddr != IP_NULL)
        {
            ret = ipcom_inet_pton(IP_AF_INET, straddr, &((struct Ip_sockaddr_in *) sa)->sin_addr);
            if (ret <= 0)
                return -IP_ERRNO_EINVAL;
        }
        break;
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        {
            struct Ip_sockaddr_in6 *in6 = (struct Ip_sockaddr_in6 *) sa;

            ipcom_memset(in6, 0, sizeof(struct Ip_sockaddr_in6));
            IPCOM_SA_LEN_SET(sa, sizeof(struct Ip_sockaddr_in6));
            sa->sa_family = IP_AF_INET6;
            if (straddr != IP_NULL)
            {
                ret = ipcom_inet_pton(IP_AF_INET6, straddr, &in6->sin6_addr);
                if (ret <= 0)
                    return -IP_ERRNO_EINVAL;

                ifname = ipcom_strchr(straddr, IPCOM_IF_DELIMITER);
            }

            if (ifname != IP_NULL && *ifname++ != '\0')
            {
                in6->sin6_scope_id = ipcom_if_nametoindex((IP_CONST char *)ifname);

                if (in6->sin6_scope_id == 0 && ipcom_isdigit(*ifname))
                    in6->sin6_scope_id = (Ip_u32)ipcom_atoi(ifname);

                if (in6->sin6_scope_id == 0)
                    return -IP_ERRNO_ENXIO;
            }
        }
        break;
#endif /* IPCOM_USE_INET6 */

#ifdef IPCOM_USE_PACKET
    case IP_AF_PACKET:
        {
            struct Ip_sockaddr_ll *ll = (struct Ip_sockaddr_ll *) sa;

            ipcom_memset(ll, 0, sizeof(struct Ip_sockaddr_ll));

            ll->sll_family = IP_AF_PACKET;
            IPCOM_SA_LEN_SET(ll, sizeof(struct Ip_sockaddr_ll));

            if (straddr != IP_NULL)
            {
                char        *endp  = IP_NULL;
                unsigned    data_index;

                ifname = ipcom_strchr(straddr, IPCOM_IF_DELIMITER);
                if (ifname != IP_NULL && *ifname != '\0')
                    ll->sll_ifindex = ipcom_if_nametoindex(ifname+1);

                if (ll->sll_ifindex == 0)
                    return -IP_ERRNO_ENXIO;

                for (data_index = 0; *straddr != IPCOM_IF_DELIMITER; straddr = endp)
                {
                    long  b      = ipcom_strtol(straddr, &endp, 16);

                    /* If endp equals straddr; we've failed to parse anything in base 16 */
                    if (endp == straddr)
                        return -IP_ERRNO_EINVAL;

                    ll->sll_addr[data_index++] = (char) b;

                    /* Are we at the delimiter? If so; exit this loop */
                    if (*endp++ == IPCOM_IF_DELIMITER)
                        break;

                }
                /* Number of assigned elements */
                ll->sll_halen = (Ip_u8)data_index;
            }
        }
        break;
#endif /* IPCOM_USE_PACKET */

    case IP_AF_LINK:
        {
            struct Ip_sockaddr_dl *dl = (struct Ip_sockaddr_dl *) sa;
            char                  *hwaddr = (char *) straddr;

            ipcom_memset(dl, 0, sizeof(struct Ip_sockaddr_dl));
            for (;;)
            {
                unsigned long b;
                char         *endptr;

                b = ipcom_strtoul(hwaddr, &endptr, 16);
                if (hwaddr == endptr)
                    /* Not a valid hex digit */
                    return -IP_ERRNO_EINVAL;

                if (b > 0xff)
                    /* Too large number */
                    return -IP_ERRNO_EINVAL;

                dl->sdl_data[dl->sdl_alen++] = (char) b;
                if (*endptr == '\0')
                    /* Done */
                    break;

                if (*endptr != ':' && *endptr != '-')
                    /* Invalid separator */
                    return -IP_ERRNO_EINVAL;
                hwaddr = endptr + 1;
            }
            dl->sdl_family = IP_AF_LINK;
            IPCOM_SA_LEN_SET(sa, (ip_offsetof(struct Ip_sockaddr_dl, sdl_data)
                                  + IP_MAX(sizeof(dl->sdl_data), dl->sdl_alen)));
        }
        break;

    case IP_AF_UNSPEC:
        sa->sa_family = IP_AF_UNSPEC;
        IPCOM_SA_LEN_SET(sa, sizeof(*sa));
        break;

    default:
        return -IP_ERRNO_EAFNOSUPPORT;
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipcom_getsockaddrbyaddrname
 *===========================================================================
 * Description:  Resolves the 'addr_or_name' to IP address stored in socket
 *               address.
 * Parameters:   domain - IP_AF_INET or IP_AF_INET6.
 *               no_dns - IP_TRUE if address should be resolved using DNS.
 *               addr_or_name - Must be a IP address or a DNS name.
 *               sa - Socket address structure to hold the IP address.
 * Returns:      0 for success else negative errno code (ipcom_perrno.h).
 *
 */
IP_PUBLIC int
ipcom_getsockaddrbyaddrname(int domain, Ip_bool no_dns, IP_CONST char *addr_or_name,
                            struct Ip_sockaddr *sa)
{
    struct Ip_addrinfo  hints;
    struct Ip_addrinfo *res = IP_NULL;
    int                 ret;

    if (no_dns == IP_TRUE)
        ret = ipcom_getsockaddrbyaddr(domain, addr_or_name, sa);
    else
    {
        ipcom_memset(&hints, 0, sizeof(hints));
        hints.ai_family = domain;
        ret = ipcom_getaddrinfo(addr_or_name, IP_NULL, &hints, &res);
        if(ret != 0)
        {
            ret = -IP_ERRNO_ESRCH;
            goto exit;
        }
        if (res->ai_family != domain)
        {
            IP_PANIC();
            ret = -IP_ERRNO_ESRCH;
            goto exit;
        }

#ifdef IPCOM_USE_INET
        if (domain == IP_AF_INET)
            ipcom_memcpy(sa, res->ai_addr, IP_MIN(res->ai_addrlen, sizeof(struct Ip_sockaddr_in)));
        else
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
        if (domain == IP_AF_INET6)
            ipcom_memcpy(sa, res->ai_addr, IP_MIN(res->ai_addrlen, sizeof(struct Ip_sockaddr_in6)));
        else
#endif /* IPCOM_USE_INET6 */
        {
            ret = -IP_ERRNO_EAFNOSUPPORT;
        }
    }

exit:
    if(res != IP_NULL)
        ipcom_freeaddrinfo(res);
    if(ret < 0)
        ipcom_errno = -ret;
    return ret;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

