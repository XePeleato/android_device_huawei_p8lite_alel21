/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_gethostby.c,v $ $Revision: 1.62 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_gethostby.c,v $
 *   $Author: rboden $
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

#define IPCOM_SKIP_NATIVE_SOCK_API

#include "ipcom_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#if defined(IP_PORT_OSE5)
#if !defined(softOSE) && defined(IP_TARGET_SOFT)
#define softOSE /* to use sfk_netdb.h */
#endif
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#ifdef s6_addr
#undef s6_addr
#endif
#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_clib.h"
#include "ipcom_sock.h"
#include "ipcom_sock2.h"

#elif defined(IPDNSC)
#define IPCOM_USE_CLIB_PROTO
#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_clib.h"
#include "ipcom_sock.h"
#include "ipcom_sock2.h"
#include <ipdnsc.h>

#elif defined(IP_PORT_VXWORKS)
#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_clib.h"
#include "ipcom_sock.h"
#include "ipcom_sock2.h"
#undef s6_addr /* Remove the ipcom definition, vxWorks 6.0 has a conflicting macro definition */
#undef ifa_dstaddr /* The ifa_dstaddr #define in ipcom_sock2.h breaks code in target/h/.../ifvar.h */
#undef ifa_broadaddr /* The ifa_broadaddr #define in ipcom_sock2.h breaks code in target/h/.../ifvar.h */

#include <vxWorks.h>
#include <string.h>

#ifndef WRS_IPNET
#include <config.h>
#ifdef INCLUDE_DNS_RESOLVER
#include <resolvLib.h>
#endif
#endif /* WRS_IPNET */

#include <hostLib.h>

#elif defined(IP_PORT_WIN32) && IPCOM_USE_SOCK == IPCOM_SOCK_NATIVE
#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_clib.h"
#include "ipcom_sock.h"
#include "ipcom_sock2.h"
#include <winsock2.h>

#elif defined(IP_PORT_OSE) && IPCOM_USE_SOCK == IPCOM_SOCK_NATIVE
#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_clib.h"
#include "ipcom_sock.h"
#include "ipcom_sock2.h"
#ifdef USE_OSEDEF_H
#undef USE_OSEDEF_H
#endif
#include <netdb.h>
#include <string.h>

#elif (defined(IP_PORT_UNIX) || defined(IP_PORT_LAS)) && IPCOM_USE_SOCK == IPCOM_SOCK_NATIVE
#include "ipcom_clib.h"
#include "ipcom_cstyle.h"
#include "ipcom_sock.h"
#include "ipcom_sock2.h"
#include "ipcom_type.h"
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#ifdef s6_addr
#undef s6_addr
#endif

#else
#define IPCOM_USE_CLIB_PROTO
#include "ipcom_clib.h"
#include "ipcom_cstyle.h"
#include "ipcom_sock.h"
#include "ipcom_sock2.h"
#include "ipcom_type.h"
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_GETHOST_MAXNAME  256

/* Size of buffer used in ipcom_gethostbyname() */
#define IPCOM_HOSTENT_SIZE     512


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

struct Ip_hostent_buffer
{
    char  *addr_list[2];
    char  *alias_list[1];
    char   host_name[IPCOM_GETHOST_MAXNAME];
    char   space[IPCOM_HOSTENT_SIZE - sizeof(union Ip_in_addr_union) - 3*sizeof(char *) - IPCOM_GETHOST_MAXNAME];
};


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

#if defined(IP_PORT_VXWORKS) && (IP_PORT_VXWORKS >= 65)
struct hostent;
extern void hostentFree(struct hostent *);
extern int hostTblInit(void);  /*!!FIXME - should not be called here!*/
IP_EXTERN struct Ip_hostent *getlhostbyname(const char *name, int family);
IP_EXTERN struct Ip_hostent *getlhostbyaddr(const char *addr, int len, int af);
#endif

#ifdef IPDNSC
IP_EXTERN int ipcom_dns_init_ok;
IP_EXTERN Ipdnsc_api ipcom_dns;
#endif


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

#if defined(IP_PORT_VXWORKS) && (IP_PORT_VXWORKS >= 65)
IP_PUBLIC struct Ip_hostent *ipcom_gethostbynamelocal(const char *name,
                                                      int family,
                                                      Ip_u32 *ifindexp,
                                                      struct Ip_hostent *result,
                                                      char *buf, int buflen);
IP_PUBLIC struct Ip_hostent *ipcom_gethostbyaddrlocal(const char *addr,
                                                      int len, int type,
                                                      struct Ip_hostent *result,
                                                      char *buf, int buflen);
#endif


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_PUBLIC int ipcom_h_errno;

static struct Ip_hostent         ipcom_he_result;
static struct Ip_hostent_buffer  ipcom_he_buffer;


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_gethostip
 *===========================================================================
 * Description: Get the first IPv4 address. Avoid loopback if possible.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC struct Ip_hostent *
ipcom_gethostip(char *name,
                struct Ip_hostent *result,
                char *buf,
                int buflen)
{
    Ip_u32 host_addr_n = 0;
    struct Ip_hostent_buffer *buffer;

#if defined(IPCOM_USE_INET) && defined(IPCOM_SOCK)
    struct Ip_if_nameindex *ifrs;

    ifrs = ipcom_if_nameindex();
    if (ifrs)
    {
        struct Ip_if_nameindex *oifrs;
        Ip_fd fd;

        oifrs = ifrs;
        fd = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, IP_IPPROTO_UDP);
        if (fd != IP_INVALID_SOCKET)
        {
            struct Ip_ifreq  ifreq;

            for (; ifrs && ifrs->if_name != IP_NULL; ifrs++)
            {
                ipcom_strcpy(ifreq.ifr_name, ifrs->if_name);
                if (ipcom_socketioctl(fd, IP_SIOCGIFADDR, &ifreq) >= 0)
                {
                    host_addr_n = ((struct Ip_sockaddr_in *)&ifreq.ip_ifr_addr)->sin_addr.s_addr;
                    if (ipcom_strcmp(ifrs->if_name, "lo0"))
                        break;
                }
            }
            (void)ipcom_socketclose(fd);
        }
        ipcom_if_freenameindex(oifrs);
    }
#endif /* IPCOM_USE_INET && IPCOM_SOCK */

    (void) buflen; /* Avoid compiler warning */

    /* Set hostent */
    buffer = IP_PTR_ALIGN(buf, sizeof(void*));

    result->h_name         = buffer->host_name;
    result->h_aliases      = buffer->alias_list;
    result->h_aliases[0]   = IP_NULL;
    result->h_addrtype     = IP_AF_INET;
    result->h_length       = sizeof(host_addr_n);
    result->h_addr_list    = buffer->addr_list;
    result->h_addr_list[0] = IP_PTR_ALIGN(buffer->space, sizeof(void*));
    result->h_addr_list[1] = IP_NULL;

    /* Set host addr, addr_list and alias_list */
    ipcom_memcpy(result->h_addr_list[0], &host_addr_n, result->h_length);

    /* Set hostname */
    ipcom_strncpy(result->h_name, name, sizeof(buffer->host_name)-1);

    return result;
}


/*
 *===========================================================================
 *                    ipcom_gethost_static
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPDNSC) || (defined(IP_PORT_WIN32) && IPCOM_USE_SOCK == IPCOM_SOCK_NATIVE) \
|| (defined(IP_PORT_VXWORKS) && IP_PORT_VXWORKS >= 65)

IP_STATIC struct Ip_hostent *
ipcom_gethost_static(struct Ip_hostent *he,
                     struct Ip_hostent *result,
                     char *buf,
                     int buflen)
{
    struct Ip_hostent_buffer *buffer;
#ifdef IPDNSC
    struct Ip_hostent *port_he = he;
#else
    struct hostent    *port_he = (struct hostent *)he;
#endif

    if (port_he == IP_NULL)
        return IP_NULL;

    if (result == IP_NULL)
    {
        result = &ipcom_he_result;
        buf = (char *)&ipcom_he_buffer;
        buflen = sizeof (ipcom_he_buffer);
    }

    if (buflen < sizeof(union Ip_in_addr_union) + (3 * sizeof(char *)) + ipcom_strlen(port_he->h_name) + 1)
    {
        /* Yes the reentrant versions set errno and not h_errno for this error */
        (void)ipcom_errno_set(IP_ERRNO_ERANGE);
        return IP_NULL;
    }

    buffer = IP_PTR_ALIGN(buf, sizeof(void*));

    /* Set hostent */
    result->h_name         = buffer->host_name;
    result->h_aliases      = buffer->alias_list;
    result->h_aliases[0]   = IP_NULL;
    result->h_addrtype     = port_he->h_addrtype;
    result->h_length       = port_he->h_length;
    result->h_addr_list    = buffer->addr_list;
    result->h_addr_list[0] = IP_PTR_ALIGN(buffer->space, sizeof(void*));
    result->h_addr_list[1] = IP_NULL;

    /* Set hostname */
    ipcom_strcpy(result->h_name, port_he->h_name);

    /* Set host addr */
    ipcom_memcpy(result->h_addr_list[0],
                 port_he->h_addr_list[0],
                 port_he->h_length);


    return result;
}
#endif


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_gethostbyname_r
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC struct Ip_hostent *
ipcom_gethostbyname_r(const char *name,
                      struct Ip_hostent *result,
                      char *buffer,
                      int buflen,
                      int *h_errnop)
{
    char hostname[IPCOM_GETHOST_MAXNAME];

    if (name == IP_NULL || result == IP_NULL)
        return IP_NULL;

    /* Check local hostname first */
    if (ipcom_gethostname(hostname, sizeof(hostname)) == 0 && ipcom_strcmp(name, hostname) == 0)
        return ipcom_gethostip(hostname, result, buffer, buflen);

    /* VxWorks static host table check */
#if defined(IP_PORT_VXWORKS) && IP_PORT_VXWORKS >= 65
    {
        struct Ip_hostent *he;
        he = ipcom_gethostbynamelocal(name, IP_AF_INET, IP_NULL, result, buffer, buflen);
        if (he != IP_NULL)
            return he;
    }
#endif /* IP_PORT_VXWORKS && IP_PORT_VXWORKS >= 65 */

    /* Interpeak DNS lookup */
#if defined(IPDNSC)
    {
        struct Ip_hostent *he, *resultp;

        if (ipcom_dns_init_ok == 0)
            return IP_NULL;
        he = ipcom_dns.getipnodebyname(name, IP_AF_INET, 0, h_errnop);
        if (he == IP_NULL)
            return IP_NULL;
        resultp = ipcom_gethost_static(he, result, buffer, buflen);
        ipcom_dns.freehostent(he);
        return resultp;
    }

#elif defined(IP_PORT_VXWORKS) && defined(INCLUDE_DNS_RESOLVER)
    (void)h_errnop;
    return (struct Ip_hostent *)resolvGetHostByName((char *)name,
                                                    buffer,
                                                    buflen);
#else
    (void)h_errnop;
#endif

    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipcom_gethostbyname
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC struct Ip_hostent *
ipcom_gethostbyname(const char *name)
{
    char hostname[IPCOM_GETHOST_MAXNAME];

    if (name == IP_NULL)
        return IP_NULL;

    /* Check local hostname first */
    if (ipcom_gethostname(hostname, sizeof(hostname)) == 0 && ipcom_strcmp(name, hostname) == 0)
        return ipcom_gethostip(hostname, &ipcom_he_result, (char *)&ipcom_he_buffer, sizeof(ipcom_he_buffer));

    /* VxWorks static host table check */
#if defined(IP_PORT_VXWORKS) && IP_PORT_VXWORKS >= 65
    {
        struct Ip_hostent *he;
        he = ipcom_gethostbynamelocal(name, IP_AF_INET, IP_NULL, IP_NULL, IP_NULL, 0);
        if (he != IP_NULL)
            return he;
    }
#endif /* IP_PORT_VXWORKS && IP_PORT_VXWORKS >= 65 */

#if defined(IP_PORT_OSE5)
    return (struct Ip_hostent *)gethostbyname(name);

#elif defined(IP_PORT_LKM)
    return IP_NULL;

    /* Interpeak DNS client */
#elif defined(IPDNSC)
    {
        struct Ip_hostent *he, *resultp;

        if (ipcom_dns_init_ok == 0)
            return IP_NULL;
        he = ipcom_dns.getipnodebyname(name, IP_AF_INET, 0, &ipcom_h_errno);
        if (he == IP_NULL)
            return IP_NULL;
        resultp = ipcom_gethost_static(he, &ipcom_he_result,
                                       (char *)&ipcom_he_buffer,
                                       sizeof(ipcom_he_buffer));
        ipcom_dns.freehostent(he);
        return resultp;
    }

    /* Port specific lookups */
#elif defined(IP_PORT_VXWORKS) && defined(INCLUDE_DNS_RESOLVER)
    return (struct Ip_hostent *)resolvGetHostByName((char *)name,
                                                    (char *)&ipcom_he_buffer,
                                                    sizeof(ipcom_he_buffer));

#elif (defined(IP_PORT_OSE) || defined(IP_PORT_UNIX) || defined(IP_PORT_LAS)) && IPCOM_USE_SOCK == IPCOM_SOCK_NATIVE
    return (struct Ip_hostent *)gethostbyname(name);

#elif defined(IP_PORT_WIN32) && IPCOM_USE_SOCK == IPCOM_SOCK_NATIVE
    return ipcom_gethost_static((struct Ip_hostent *)gethostbyname(name),
                                &ipcom_he_result,
                                (char *)&ipcom_he_buffer,
                                sizeof(ipcom_he_buffer));

#else
    return IP_NULL;
#endif
}


/*
 *===========================================================================
 *                    ipcom_gethostbyaddr_r
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC struct Ip_hostent *
ipcom_gethostbyaddr_r(const char *addr, int len, int type,
                      struct Ip_hostent *result,
                      char *buffer, int buflen, int *h_errnop)
{
    if (addr == IP_NULL || result == IP_NULL)
        return IP_NULL;

    /* VxWorks static host table check */
#if defined(IP_PORT_VXWORKS) && IP_PORT_VXWORKS >= 65
    {
        struct Ip_hostent *he;
        he = ipcom_gethostbyaddrlocal(addr, len, type, result, buffer, buflen);
        if (he != IP_NULL)
            return he;
    }
#endif /* IP_PORT_VXWORKS && IP_PORT_VXWORKS >= 65 */

    /* Interpeak DNS client */
#if defined(IPDNSC)
    {
        struct Ip_hostent *he, *resultp;

        if (ipcom_dns_init_ok == 0)
            return IP_NULL;
        he = ipcom_dns.getipnodebyaddr(addr, (Ip_size_t)len, type, h_errnop);
        if (he == IP_NULL)
            return IP_NULL;
        resultp = ipcom_gethost_static(he, result, buffer, buflen);
        ipcom_dns.freehostent(he);
        return resultp;
    }

    /* Port specific lookups */
#elif defined(IP_PORT_VXWORKS) && defined(INCLUDE_DNS_RESOLVER)
    return (struct Ip_hostent *)resolvGetHostByAddr((char *)addr,
                                                    buffer,
                                                    buflen);

#else
    (void)addr;
    (void)len;
    (void)type;
    (void)result;
    (void)buffer;
    (void)buflen;
    (void)h_errnop;
    return IP_NULL;
#endif
}


/*
 *===========================================================================
 *                    ipcom_gethostbyaddr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC struct Ip_hostent *
ipcom_gethostbyaddr(const char *addr, int len, int type)
{
    /* Check the static host table first */
#if defined(IP_PORT_VXWORKS) && IP_PORT_VXWORKS >= 65
    {
        struct Ip_hostent *he;
        he = ipcom_gethostbyaddrlocal(addr, len, type, IP_NULL, IP_NULL, 0);
        if (he != IP_NULL)
            return he;
    }
#endif /* IP_PORT_VXWORKS && IP_PORT_VXWORKS >= 65 */

    /* Lookup the address with the appropriate function */
#if defined(IP_PORT_OSE5)
    return (struct Ip_hostent *)gethostbyaddr(addr, len, type);

    /* Interpeak DNS client */
#elif defined(IPDNSC)
    {
        struct Ip_hostent *he, *resultp;

        if (addr == IP_NULL || ipcom_dns_init_ok == 0)
            return IP_NULL;
        he = ipcom_dns.getipnodebyaddr(addr, (Ip_size_t)len, type, &ipcom_h_errno);
        if (he == IP_NULL)
            return IP_NULL;
        resultp = ipcom_gethost_static(he, &ipcom_he_result,
                                       (char *)&ipcom_he_buffer,
                                       sizeof(ipcom_he_buffer));
        ipcom_dns.freehostent(he);
        return resultp;
    }

    /* Port specific lookups */
#elif defined(IP_PORT_VXWORKS) && defined(INCLUDE_DNS_RESOLVER)
    return (struct Ip_hostent *)resolvGetHostByAddr((char *)addr,
                                                    (char *)&ipcom_he_buffer,
                                                    sizeof(ipcom_he_buffer));

#elif (defined(IP_PORT_OSE) || defined(IP_PORT_UNIX) || defined(IP_PORT_LAS)) && IPCOM_USE_SOCK == IPCOM_SOCK_NATIVE
    return (struct Ip_hostent *)gethostbyaddr(addr, len, type);

#elif defined(IP_PORT_WIN32) && IPCOM_USE_SOCK == IPCOM_SOCK_NATIVE
    return ipcom_gethost_static((struct Ip_hostent *)gethostbyaddr(addr, len, type),
                                &ipcom_he_result,
                                (char *)&ipcom_he_buffer,
                                sizeof(ipcom_he_buffer));

#else
    (void)addr;
    (void)len;
    (void)type;
    return IP_NULL;
#endif
}


#if defined(IP_PORT_VXWORKS) && (IP_PORT_VXWORKS >= 65)
/*
 *===========================================================================
 *                    ipcom_gethostbynamelocal
 *===========================================================================
 * Description: Gets host info from local host table given the host name
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC struct Ip_hostent *
ipcom_gethostbynamelocal(const char *name, int family, Ip_u32 *ifindexp,
                         struct Ip_hostent *result, char *buf, int buflen)
{
    struct Ip_hostent *he  = IP_NULL;
    struct Ip_hostent *hes = IP_NULL;

    if (hostTblInit() != 0)
      return IP_NULL;

#ifdef IPCOM_HOST_FROM_LOCALTBL
    he = (struct Ip_hostent *)getlhostbyname(name, family);
#endif
    if (he != IP_NULL)
    {
#ifdef IPCOM_USE_INET6
        if (ifindexp != IP_NULL && he->h_addrtype == IP_AF_INET6)
            *ifindexp = *(Ip_u32 *)(he->h_addr + 16);
#endif
        hes = ipcom_gethost_static(he, result, buf, buflen);
        hostentFree((struct hostent *)he);
    }

    return hes;
}


/*
 *===========================================================================
 *                    ipcom_gethostbyaddrlocal
 *===========================================================================
 * Description: Gets host info from local host table given the host address
 * Parameters:
 * Returns: IP_NULL or a pointer to struct Ip_hostent.
 *
 */
IP_PUBLIC struct Ip_hostent *
ipcom_gethostbyaddrlocal(const char *addr, int len, int family,
                         struct Ip_hostent *result, char *buf, int buflen)
{
    struct Ip_hostent *he  = IP_NULL;
    struct Ip_hostent *hes = IP_NULL;

    if (hostTblInit() != 0)
      return IP_NULL;

#ifdef IPCOM_HOST_FROM_LOCALTBL
    he = (struct Ip_hostent *)getlhostbyaddr(addr, len, family);
#endif
    if (he != IP_NULL)
    {
        hes = ipcom_gethost_static(he, result, buf, buflen);
        hostentFree((struct hostent *)he);
    }

    return hes;
}
#endif /* defined(IP_PORT_VXWORKS) && (IP_PORT_VXWORKS >= 65) */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
