/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_getinfo2.c,v $ $Revision: 1.24 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_getinfo2.c,v $
 *   $Author: kenneth $
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

#if defined(IPDNSC)
#define IPCOM_USE_DNS
#endif


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_clib.h"
#include "ipcom_sock.h"
#include "ipcom_sock2.h"
#include "ipcom_sock6.h"
#include "ipcom_inet.h"

#ifdef IPCOM_USE_DNS
#include <ipdnsc.h>
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#if !defined(IP_PORT_VXWORKS) || defined(_WRS_KERNEL)
#define IPCOM_USE_GETINFO
#endif

#if defined(IPCOM_USE_DNS) && defined(IP_PORT_OSE5)
#define ipcom_getnameinfo2  ipcom_getnameinfo3
#define ipcom_getaddrinfo2  ipcom_getaddrinfo3
#define ipcom_freeaddrinfo2 ipcom_freeaddrinfo3
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

#if defined(IP_PORT_VXWORKS) && IP_PORT_VXWORKS >= 65
IP_EXTERN struct Ip_hostent *ipcom_gethostbynamelocal(const char *name,
                                                      int family,
                                                      Ip_u32 *ifindex,
                                                      struct Ip_hostent *result,
                                                      char *buf, int buflen);
IP_EXTERN struct Ip_hostent *ipcom_gethostbyaddrlocal(const char *addr,
                                                      int len, int family,
                                                      struct Ip_hostent *result,
                                                      char *buf, int buflen);
#endif

#ifdef IPCOM_USE_DNS
IP_EXTERN int ipcom_dns_init_ok;
IP_EXTERN Ipdnsc_api ipcom_dns;
#endif


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
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
 * 8                    DATA
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

#ifdef IPCOM_USE_GETINFO
IP_STATIC struct Ip_addrinfo *
ipcom_getaddrinfo_alloc(void)
{
    struct Ip_addrinfo *ai;

    /* Allocate the ai structure */
    ai = ipcom_malloc(sizeof(*ai));
    if (ai == IP_NULL)
        return IP_NULL;
    ipcom_memset(ai, 0, sizeof(*ai));

    /* Allocate the sockaddr structure */
    ai->ai_addr = ipcom_malloc(sizeof(union Ip_sockaddr_union));
    if (ai->ai_addr == IP_NULL)
    {
        ipcom_free(ai);
        return IP_NULL;
    }
    ipcom_memset(ai->ai_addr, 0, sizeof(union Ip_sockaddr_union));

    return ai;
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_getnameinfo2
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_getnameinfo2(IP_CONST struct Ip_sockaddr *sa, Ip_socklen_t salen,
                  char *host, Ip_size_t hostlen,
                  char *serv, Ip_size_t servlen,
                  int flags)
{
    union Ip_sockaddr_union *su;
    struct Ip_servent *se;
    char numserv[8];
    int family = 0;
	Ip_u16 port_n = 0;
    char *addr_n = IP_NULL;
    void *real_addr_n = IP_NULL;
#ifdef IPCOM_USE_INET6
    Ip_u32 ipv4addr;
    char numaddr[IP_INET6_ADDRSTRLEN];
#else
    char numaddr[IP_INET_ADDRSTRLEN];
#endif
    char zone[16];
    struct Ip_hostent *he;
    struct Ip_hostent result;
    int  h_errno;
    char buffer[300];
#ifdef IPCOM_USE_DNS
    Ip_bool hes = IP_FALSE;
#endif

    /* Check arguments */
    if (sa == IP_NULL || IPCOM_SA_LEN_GET(sa) != salen)
		return IP_EAI_FAIL;
	if ((serv == IP_NULL || servlen == 0) && (host == IP_NULL || hostlen == 0))
		return IP_EAI_FAIL;

    su = (union Ip_sockaddr_union *)sa;
#ifdef IPCOM_USE_INET
    if (sa->sa_family == IP_AF_INET)
    {
        family = IP_AF_INET;
        port_n = su->sin.sin_port;
        addr_n = (char *)(&su->sin.sin_addr);
        real_addr_n = addr_n;
    }
#endif
#ifdef IPCOM_USE_INET6
    if (sa->sa_family == IP_AF_INET6)
    {
        family = IP_AF_INET6;
        port_n = su->sin.sin_port;
        addr_n = (char *)(&su->sin6.sin6_addr);
        real_addr_n = addr_n;
        if (IP_IN6_IS_ADDR_V4COMPAT(&su->sin6.sin6_addr) ||
            IP_IN6_IS_ADDR_V4MAPPED(&su->sin6.sin6_addr))
        {
            family = IP_AF_INET;
            ipv4addr = su->sin6.sin6_addr.in6.addr32[3];
            addr_n = (char *)&ipv4addr;
        }
    }
#endif

    if (real_addr_n == IP_NULL)
        /* Invalid family */
        return IP_EAI_FAIL;

    /* Translate the socket address to a node name */
    if (host != IP_NULL && hostlen != 0)
    {
        if (!(flags & IP_NI_NUMERICHOST))
        {
#ifdef IPCOM_USE_DNS
#if defined(IP_PORT_VXWORKS) && (IP_PORT_VXWORKS >= 65)
            he = ipcom_gethostbyaddrlocal(addr_n,
                                          family == IP_AF_INET ? 4 : 16,
                                          family,
                                          &result, buffer, sizeof(buffer));
            if (he)
               hes = IP_TRUE;
            else
#endif
            if (ipcom_dns_init_ok == 0)
                he = IP_NULL;
            else
                he = ipcom_dns.getipnodebyaddr(addr_n,
                                               family == IP_AF_INET ? 4 : 16,
                                               family, &h_errno);
#else
            if (family == IP_AF_INET)
                he = ipcom_gethostbyaddr_r(addr_n, 4, IP_AF_INET, &result, buffer, sizeof(buffer), &h_errno);
            else
                he = IP_NULL;
#endif /* IPCOM_USE_DNS */

            if (he != IP_NULL)
            {
			    if (ipcom_strlen(he->h_name) + 1 > hostlen)
                {
#ifdef IPCOM_USE_DNS
                    if (hes == IP_FALSE)
                        ipcom_dns.freehostent(he);
#endif
				    return IP_EAI_MEMORY;
                }
			    ipcom_strcpy(host, he->h_name);
#ifdef IPCOM_USE_DNS
                if (hes == IP_FALSE)
                    ipcom_dns.freehostent(he);
#endif
            }
            else
            {
                /* Try numeric */
                flags |= IP_NI_NUMERICHOST;
			}
        }

        if (flags & IP_NI_NUMERICHOST)
        {
		    if (flags & IP_NI_NAMEREQD)
			    return IP_EAI_NONAME;

            if (ipcom_inet_ntop(sa->sa_family, real_addr_n, numaddr, sizeof(numaddr)) == IP_NULL)
                return IP_EAI_SYSTEM;
#ifdef IPCOM_USE_INET6
            if (sa->sa_family == IP_AF_INET6
                && ((struct Ip_sockaddr_in6 *)sa)->sin6_scope_id != 0)
            {
                zone[0] = '%';
                ipcom_sprintf(&zone[1], "%u", ((struct Ip_sockaddr_in6 *)sa)->sin6_scope_id);
            }
            else
#endif
            {
                /* ipv4 or unscoped ipv6 */
                zone[0] = '\0';
            }
            if (ipcom_strlen(numaddr) + ipcom_strlen(zone) + 1 > hostlen)
				return IP_EAI_MEMORY;
			ipcom_strcpy(host, numaddr);
			ipcom_strcat(host, zone);
		}
    }

    /* Translate the socket address to a service */
	if (serv != IP_NULL && servlen != 0)
    {
		if (flags & IP_NI_NUMERICSERV)
			se = IP_NULL;
		else
			se = ipcom_getservbyport((int)port_n, (flags & IP_NI_DGRAM) ? "udp" : "tcp");

        if (se)
        {
			if (ipcom_strlen(se->s_name) + 1 > servlen)
				return IP_EAI_MEMORY;
		    ipcom_strcpy(serv, se->s_name);
		}
        else
        {
			ipcom_snprintf(numserv, sizeof(numserv), "%u", ip_ntohs(port_n));
			if (ipcom_strlen(numserv) + 1 > servlen)
				return IP_EAI_MEMORY;
			ipcom_strcpy(serv, numserv);
		}
    }

	return 0;
}


/*
 *===========================================================================
 *                    ipcom_getaddrinfo2
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
/*lint -efunc(429,ipcom_getaddrinfo2) */
/*lint -efunc(429,ipcom_getsockaddrbyaddr) */
IP_PUBLIC int
ipcom_getaddrinfo2(IP_CONST char *hostname,
                  IP_CONST char *servname,
                  IP_CONST struct Ip_addrinfo *hints,
                  struct Ip_addrinfo **res)
{
    struct Ip_addrinfo *ai = IP_NULL;
    struct Ip_addrinfo *tmp;
    int family = 0, flags = 0, socktype = 0, protocol = 0, ret;
    struct Ip_addrinfo lhints;
    struct Ip_hostent result;
    int  h_errno;
    char buffer[300];

    /* Check arguments */
    if (hostname == IP_NULL && servname == IP_NULL)
    {
        ret = IP_EAI_NONAME;
		goto fail;
    }
    if (hints != IP_NULL)
    {
        if (hints->ai_addrlen || hints->ai_canonname || hints->ai_addr || hints->ai_next)
        {
            ret = IP_EAI_NONAME;
			goto fail;
        }
        if (hints->ai_flags & ~(IP_AI_PASSIVE | IP_AI_CANONNAME | IP_AI_NUMERICHOST | IP_AI_NUMERICSERV
                                | IP_AI_V4MAPPED | IP_AI_ALL | IP_AI_ADDRCONFIG))
        {
            ret = IP_EAI_BADFLAGS;
			goto fail;
        }
        if (hints->ai_family != IP_AF_UNSPEC
            && hints->ai_family != IP_AF_INET
            && hints->ai_family != IP_AF_INET6)
        {
            ret = IP_EAI_FAMILY;
            goto fail;
        }
        if (hints->ai_socktype != 0
            && hints->ai_socktype != IP_SOCK_STREAM
            && hints->ai_socktype != IP_SOCK_DGRAM
            && hints->ai_socktype != IP_SOCK_RAW)
        {
            ret = IP_EAI_SOCKTYPE;
			goto fail;
        }

        if (hints->ai_protocol != 0
            && hints->ai_socktype != IP_SOCK_RAW
            && hints->ai_protocol != IP_IPPROTO_UDP
#ifdef IPSCTP
            && hints->ai_protocol != IP_IPPROTO_SCTP
#endif
            && hints->ai_protocol != IP_IPPROTO_TCP)
        {
            ret = IP_EAI_NONAME;
			goto fail;
        }

        if ((hints->ai_socktype == IP_SOCK_STREAM && hints->ai_protocol == IP_IPPROTO_UDP)
            || (hints->ai_socktype == IP_SOCK_DGRAM && hints->ai_protocol == IP_IPPROTO_TCP))
        {
            ret = IP_EAI_NONAME;
			goto fail;
        }

        if (hints->ai_socktype == IP_SOCK_RAW && servname != IP_NULL)
        {
            ret = IP_EAI_SERVICE;
            goto fail;
        }

        flags = hints->ai_flags;
        family = hints->ai_family;
        socktype = hints->ai_socktype;
        protocol = hints->ai_protocol;
	}

    if (family == IP_AF_UNSPEC)
    {
        int ret1;

        if (hints == IP_NULL)
            ipcom_memset(&lhints, 0, sizeof(lhints));
        else
            ipcom_memcpy(&lhints, hints, sizeof(lhints));
        lhints.ai_family = IP_AF_INET6;
        ret = ipcom_getaddrinfo2(hostname, servname, &lhints, res);
        lhints.ai_family = IP_AF_INET;
        tmp = *res;
        if (*res != IP_NULL)
            while(tmp->ai_next != IP_NULL)
                tmp = tmp->ai_next;
        ret1 = ipcom_getaddrinfo2(hostname, servname, &lhints, *res == IP_NULL ? res : &tmp->ai_next);
        return ret == 0 || ret1 == 0 ? 0 : ret;
    }

    /* Allocate initial addrinfo structure */
    ai = ipcom_getaddrinfo_alloc();
    if (ai == IP_NULL)
    {
        ret = IP_EAI_MEMORY;
        goto fail;
    }

    /* Explore hostname */
    if (hostname == IP_NULL)
    {
        char *addr;

        if (flags & IP_AI_PASSIVE)
            addr = family == IP_AF_INET ? "0.0.0.0" : "::";
        else
            addr = family == IP_AF_INET ? "127.0.0.1" : "::1";
        if (ipcom_getsockaddrbyaddr(family, addr, ai->ai_addr))
        {
            ret = IP_EAI_NONAME;
            goto fail;
        }
    }
    else
    {
        int literal_v4 = 0, literal_v6 = 0;
        Ip_u32 ipaddr[4];
        struct Ip_hostent *he;
#if defined(IP_PORT_VXWORKS) && IP_PORT_VXWORKS >= 65
        Ip_u32 ifindex = 0;
#endif
#ifdef IPCOM_USE_INET6
        /* The extra 12 bytes is for a numeric %ifindex scope id string */
        char hostaddr[IP_INET6_ADDRSTRLEN+16];
#else
        char hostaddr[16];
#endif
#ifdef IPCOM_USE_DNS
        Ip_bool hes = IP_FALSE;
        int dns_flags = 0;
#endif

        ipcom_memset(ipaddr, 0, sizeof(ipaddr));
#ifdef IPCOM_USE_INET
        if (family == IP_AF_INET)
            literal_v4 = ipcom_inet_pton(family, (char *) hostname, ipaddr);
#endif
#ifdef IPCOM_USE_INET6
        if (literal_v4 == 0 && family == IP_AF_INET6)
            literal_v6 = ipcom_inet_pton(family, hostname, ipaddr);
#endif
        if ((literal_v4 == 0) && (literal_v6 == 0))
        {
            if (flags & IP_AI_NUMERICHOST)
            {
                ret = IP_EAI_NONAME;
                goto fail;
            }
            else
            {
#ifdef IPCOM_USE_DNS
                if (family == IP_AF_INET6)
                {
                    dns_flags |= flags & IP_AI_V4MAPPED ? IPDNSC_AI_V4MAPPED : 0;
                    dns_flags |= (flags & IP_AI_V4MAPPED) && (flags & IP_AI_ALL) ? IPDNSC_AI_ALL : 0;
                    dns_flags |= flags & IP_AI_ADDRCONFIG ? IPDNSC_AI_ADDRCONFIG : 0;
                }
#if defined(IP_PORT_VXWORKS) && IP_PORT_VXWORKS >= 65
                he = ipcom_gethostbynamelocal(hostname, family, &ifindex, &result, buffer, sizeof(buffer));
                if (he)
                   hes = IP_TRUE;
                else
#endif
                if (ipcom_dns_init_ok == 0)
                    he = IP_NULL;
                else
                    he = ipcom_dns.getipnodebyname(hostname, family, dns_flags, &h_errno);
#else
                if (family == IP_AF_INET)
                    he = ipcom_gethostbyname_r(hostname, &result, buffer, sizeof(buffer), &h_errno);
                else
                    he = IP_NULL;
#endif
                if (he == IP_NULL)
                {
                    ret = IP_EAI_NONAME;
                    goto fail;
                }
                else
                {
                    int i = 0;
                    union Ip_sockaddr_union sa;
                    struct Ip_addrinfo *prev = IP_NULL;

                    while (he->h_addr_list[i] != IP_NULL)
                    {
                        if (ipcom_inet_ntop(he->h_addrtype, he->h_addr_list[i], hostaddr, sizeof(hostaddr)) == IP_NULL
#if defined(IP_PORT_VXWORKS) && IP_PORT_VXWORKS >= 65
                            || (ifindex != 0 && (ipcom_sprintf(ipcom_strchr(hostaddr, '\0'), "%%%u", ifindex) <= 0))
#endif
                            || ipcom_getsockaddrbyaddr(he->h_addrtype, hostaddr, &sa.sa) != 0)
                        {
#ifdef IPCOM_USE_DNS
                            if (hes == IP_FALSE)
                                ipcom_dns.freehostent(he);
#endif
                            ret = IP_EAI_NONAME;
                            goto fail;
                        }

                        if (i == 0)
                            tmp = ai;
                        else
                        {
                            /* Allocate additional addrinfo structure for optional alias address */
                            tmp = ipcom_getaddrinfo_alloc();
                            if (tmp == IP_NULL)
                            {
#ifdef IPCOM_USE_DNS
                                if (hes == IP_FALSE)
                                    ipcom_dns.freehostent(he);
#endif
                                ret = IP_EAI_MEMORY;
                                goto fail;
                            }
                        }

                        /* Copy socket address */
                        ipcom_memcpy(tmp->ai_addr, &sa, sizeof(union Ip_sockaddr_union));

                        /* Get the name */
                        if (flags & IP_AI_CANONNAME)
                        {
                            tmp->ai_canonname = ipcom_strdup(he->h_name);
                            if (tmp->ai_canonname == IP_NULL)
                            {
#ifdef IPCOM_USE_DNS
                                if (hes == IP_FALSE)
                                    ipcom_dns.freehostent(he);
#endif
                                ret = IP_EAI_MEMORY;
                                goto fail;
                            }
                        }

                        if (prev != IP_NULL)
                            prev->ai_next = tmp;
                        prev = tmp;
                        i++;
                    }
#ifdef IPCOM_USE_DNS
                    if (hes == IP_FALSE)
                        ipcom_dns.freehostent(he);
#endif
                }
            }
        }
        else
        {
#ifdef IPCOM_USE_INET
            if (literal_v4 == 1)
                family = IP_AF_INET;
            else
#endif
                family = IP_AF_INET6;

            if (!(flags & IP_AI_NUMERICHOST) && (flags & IP_AI_CANONNAME))
            {
#ifdef IPCOM_USE_DNS
#if defined(IP_PORT_VXWORKS) && (IP_PORT_VXWORKS >= 65)
                he = ipcom_gethostbyaddrlocal((const char *)ipaddr,
                                              family == IP_AF_INET ? 4 : 16,
                                              family,
                                              &result, buffer, sizeof(buffer));
                if (he)
                   hes = IP_TRUE;
                else
#endif
                if (ipcom_dns_init_ok == 0)
                    he = IP_NULL;
                else
                    he = ipcom_dns.getipnodebyaddr(ipaddr,
                                                   family == IP_AF_INET ? 4 : 16,
                                                   family, &h_errno);
#else
                if (family == IP_AF_INET)
                    he = ipcom_gethostbyaddr_r((char *)ipaddr, 4, IP_AF_INET, &result, buffer, sizeof(buffer), &h_errno);
                else
                    he = IP_NULL;
#endif
                if (he != IP_NULL)
                {
                    ai->ai_canonname = ipcom_strdup(he->h_name);
#ifdef IPCOM_USE_DNS
                    if (hes == IP_FALSE)
                        ipcom_dns.freehostent(he);
#endif
                }
                else
                {
                    ai->ai_canonname = ipcom_strdup(hostname);
                }

                if (ai->ai_canonname == IP_NULL)
                {
                    ret = IP_EAI_MEMORY;
                    goto fail;
                }
            }
            if (ipcom_getsockaddrbyaddr(family, hostname, ai->ai_addr))
            {
                ret = IP_EAI_NONAME;
                goto fail;
            }
        }
    }

    /* Explore servname */
    if (servname != IP_NULL)
    {
        long val;
        char *proto = IP_NULL;
        char *suff;
        struct Ip_servent *se = IP_NULL;
        Ip_in_port_t port_n;
        int i = 0, numericserv = 0;

        if (flags & IP_AI_NUMERICSERV)
        {
            /* Allow only numeric service specification */
            numericserv = 1;
            while (servname[i] != '\0')
            {
                if (servname[i] < '0' || servname[i] > '9')
                {
                    ret = IP_EAI_NONAME;
                    goto fail;
                }
                i++;
            }
        }

        if (socktype == IP_SOCK_DGRAM || protocol == IP_IPPROTO_UDP)
            proto = "udp";
#ifdef IPSCTP
        else if (socktype == IP_SOCK_STREAM && protocol == IP_IPPROTO_SCTP)
            proto = "sctp";
#endif
        else if (socktype == IP_SOCK_STREAM || protocol == IP_IPPROTO_TCP)
            proto = "tcp";
        if (numericserv == 0)
            se = ipcom_getservbyname((char *)servname, proto);
        if (se == IP_NULL)
        {
            val = ipcom_strtol((char *)servname, &suff, 10);
            if (val < 0 || val > 0xffff || *suff != '\0' || suff == servname)
            {
                ret = IP_EAI_SERVICE;
                goto fail;
            }
            else
            {
                if (proto == IP_NULL)
                    proto = "udp";
                port_n = (Ip_in_port_t)ip_htons((short)val);
            }
        }
        else
        {
            proto = se->s_proto;
            port_n = (Ip_in_port_t)se->s_port;
        }

        socktype = (ipcom_strcmp(proto, "tcp") == 0 || ipcom_strcmp(proto, "sctp") == 0) ?
                    IP_SOCK_STREAM : IP_SOCK_DGRAM;
#ifdef IPSCTP
        if (protocol != IP_IPPROTO_SCTP)
#endif
            protocol = socktype == IP_SOCK_STREAM ? IP_IPPROTO_TCP : IP_IPPROTO_UDP;
        tmp = ai;
        while (tmp != IP_NULL)
        {
#ifdef IPCOM_USE_INET
            if (family == IP_AF_INET)
                ((union Ip_sockaddr_union *)tmp->ai_addr)->sin.sin_port = port_n;
#endif
#ifdef IPCOM_USE_INET6
            if (family == IP_AF_INET6)
                ((union Ip_sockaddr_union *)tmp->ai_addr)->sin6.sin6_port = port_n;
#endif
            tmp = tmp->ai_next;
        }
    }

    tmp = ai;
    while (tmp != IP_NULL)
    {
        tmp->ai_flags = 0;
        tmp->ai_family = family;
        tmp->ai_socktype = socktype;
        tmp->ai_protocol = protocol;
        tmp->ai_addrlen = IPCOM_SA_LEN_GET(tmp->ai_addr);
        tmp = tmp->ai_next;
    }

    *res = ai;
    ip_assert(*res != IP_NULL && (*res)->ai_addr != IP_NULL);
    return 0;

fail:
    if (ai != IP_NULL)
        ipcom_freeaddrinfo2(ai);
    *res = IP_NULL;
    return ret;
}
#endif /* IPCOM_USE_GETINFO */


/*
 *===========================================================================
 *                    ipcom_freeaddrinfo2
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_freeaddrinfo2(struct Ip_addrinfo *ai)
{
	struct Ip_addrinfo *next;

    while (ai != IP_NULL)
    {
		next = ai->ai_next;
		if (ai->ai_canonname != IP_NULL)
			ipcom_free(ai->ai_canonname);
        if (ai->ai_addr != IP_NULL)
		    ipcom_free(ai->ai_addr);
		ipcom_free(ai);
		ai = next;
    }
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
