/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_ifaddrs.c,v $ $Revision: 1.15.20.1 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_ifaddrs.c,v $
 *     $Author: ulf $ $Date: 2010-07-02 13:20:56 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Markus Carlstedt <markus@interpeak.se>
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
#include "ipcom_h.h"
#include "ipcom_sock2.h"
#include "ipcom_clib.h"
#include "ipcom_syslog.h"
#include "ipcom_sysctl.h"

#if defined(IPNET) || (defined(IP_PORT_VXWORKS) && !defined(_WRS_KERNEL))
#include <ipnet_routesock.h>
#include <ipnet_cmd.h>


/*
 ****************************************************************************
 * 4                    EXTERN PROTOTYPES
 ****************************************************************************
 */

#if defined(IP_PORT_VXWORKS) && !defined(_WRS_KERNEL)

#define ipcom_free		free
#define ipcom_calloc		calloc
#define ipcom_strdup		strdup
#define ipcom_malloc		malloc
#define ipcom_memcpy		memcpy
#define ipcom_memset		memset

#ifdef  ipcom_socketioctl
#undef  ipcom_socketioctl
#define ipcom_socketioctl 	ioctl
#endif  /* ipcom_socketioctl */
#define ipcom_sysctl		sysctl

#ifdef  ipcom_socket
#undef  ipcom_socket
#define ipcom_socket		socket
#endif  /* ipcom_socket */

#ifdef  ipcom_socketclose	
#undef  ipcom_socketclose	
#define ipcom_socketclose	close
#endif  /* ipcom_socketclose */

#define ipcom_getifaddrs	getifaddrs
#define ipcom_freeifaddrs	freeifaddrs

#ifdef  IPCOM_LOG1
#undef  IPCOM_LOG1
#define IPCOM_LOG1(x, fm, a)    printf(fm, a)
#endif  /* IPCOM_LOG1 */

#define  Ip_size_t		size_t


extern int socket
    (
    int domain,    /* address family (AF_xxx)     */
    int type,      /* socket type (SOCK_xxx)      */
    int protocol   /* socket protocol (usually 0) */
    );

extern int  sysctl (int *, u_int, void *, size_t *, void *, size_t);
extern int  ioctl (int fd, int function, ...);
extern int  close (int);

#endif


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
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

IP_STATIC void
ipcom_ifaddrs_free(struct Ip_ifaddrs *ifa)
{
    if(ifa->ifa_name != IP_NULL)
        ipcom_free(ifa->ifa_name);
    if(ifa->ifa_addr != IP_NULL)
        ipcom_free(ifa->ifa_addr);
    if(ifa->ifa_netmask != IP_NULL)
        ipcom_free(ifa->ifa_netmask);
    if(ifa->ifa_broadaddr != IP_NULL)
        ipcom_free(ifa->ifa_broadaddr);

    ipcom_free(ifa);
}


#if defined(IP_PORT_VXWORKS) && !defined(_WRS_KERNEL)
IP_STATIC void
ipnet_cmd_init_addrs_rtp(void *msg, Ip_u32 addrs, struct Ip_sockaddr **sas)
{
    int       i;
    Ip_size_t offset = 0;

    for (i = 0; i < IPNET_RTAX_MAX; i++)
    {
        if (IP_BIT_ISSET(addrs, 1 << i))
        {
            sas[i] = (struct Ip_sockaddr *) &((Ip_u8 *) msg)[offset];
            offset += IPCOM_SA_LEN_GET(sas[i]);
        }
        else
            sas[i] = IP_NULL;
    }
}
#endif


/*
 *===========================================================================
 *                    ipcom_create_ifaddrs_entry
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC struct Ip_ifaddrs *
ipcom_create_ifaddrs_entry(const char                *ifname,
                           struct Ipnet_if_msghdr    *ifm,
                           struct Ipnet_ifa_msghdr   *ifa,
                           struct Ip_sockaddr        **addrs)
{
    /**/
    struct Ip_ifaddrs        *nifa = ipcom_calloc(1, sizeof(*nifa));

    if (nifa == IP_NULL)
        return IP_NULL;

    /* Interface name */
    nifa->ifa_name = ipcom_strdup(ifname);
    if(nifa->ifa_name == IP_NULL)
        goto fail;

    /* Interface flags */
    nifa->ifa_flags = ifm->ifm_flags;

    /* Interface address */
    nifa->ifa_addr = ipcom_malloc(sizeof(union Ip_sockaddr_union));
    if(nifa->ifa_addr == IP_NULL)
        goto fail;

    ipcom_memcpy(nifa->ifa_addr, addrs[IPNET_RTAX_IFA], IPCOM_SA_LEN_GET(addrs[IPNET_RTAX_IFA]));

    nifa->ifa_netmask = ipcom_malloc(sizeof(union Ip_sockaddr_union));
    if(nifa->ifa_netmask == IP_NULL)
        goto fail;

    ipcom_memcpy(nifa->ifa_netmask, addrs[IPNET_RTAX_NETMASK], IPCOM_SA_LEN_GET(addrs[IPNET_RTAX_IFA]));


    if (IP_BIT_ISSET(ifa->ifam_addrs, IPNET_RTA_BRD))
    {
        nifa->ifa_broadaddr = ipcom_malloc(sizeof(union Ip_sockaddr_union));
        if(nifa->ifa_broadaddr == IP_NULL)
            goto fail;

        ipcom_memcpy(nifa->ifa_broadaddr, addrs[IPNET_RTAX_BRD], IPCOM_SA_LEN_GET(addrs[IPNET_RTAX_BRD]));
    }

    return nifa;

fail:
    ipcom_ifaddrs_free(nifa);
    return IP_NULL;
}

/*
 *===========================================================================
 *                    ipnet_ifconfig_print_ipv4
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPCOM_USE_INET
IP_STATIC Ip_size_t
ipcom_create_ifaddrs_inet(Ip_fd                     fd,
                          const char               *ifname,
                          Ip_u8                     *ifa_first,
                          Ip_size_t                 buf_len,
                          struct Ipnet_if_msghdr    *ifm,
                          struct Ip_ifaddrs         ***tail)
{
    struct Ipnet_ifa_msghdr *ifa;
    struct Ip_sockaddr_in   *addrs[IPNET_RTAX_MAX];
    Ip_size_t                offset;

    (void)fd;

    offset = 0;
    while (offset < buf_len)
    {
        struct Ip_ifaddrs        *nifa;

        ifa = (struct Ipnet_ifa_msghdr *) &ifa_first[offset];

#if defined(IP_PORT_VXWORKS) && !defined(_WRS_KERNEL)
        ipnet_cmd_init_addrs_rtp(ifa + 1, ifa->ifam_addrs, (struct Ip_sockaddr **) addrs);
#else
        ipnet_cmd_init_addrs(ifa + 1, ifa->ifam_addrs, (struct Ip_sockaddr **) addrs);
#endif

        if (ifa->ifam_type != IPNET_RTM_NEWADDR || addrs[IPNET_RTAX_IFA]->sin_family != IP_AF_INET)
            break;

        offset += ifa->ifam_msglen;

        if (IP_IN_CLASSD(addrs[IPNET_RTAX_IFA]->sin_addr.s_addr))
            continue;

        nifa = ipcom_create_ifaddrs_entry(ifname, ifm, ifa, (struct Ip_sockaddr **)addrs);
        if (nifa != IP_NULL)
        {
            **tail = nifa;
            *tail  = &nifa->ifa_next;
        }
    }

    return offset;
}
#endif /* IPCOM_USE_INET */


#ifdef IPCOM_USE_INET6
IP_STATIC Ip_size_t
ipcom_create_ifaddrs_inet6(Ip_fd                    fd,
                           const char               *ifname,
                           Ip_u8                    *ifa_first,
                           Ip_size_t                buf_len,
                           struct Ipnet_if_msghdr   *ifm,
                           struct Ip_ifaddrs        ***tail)
{
    struct Ip_in6_aliasreq   ifareq;
    struct Ipnet_ifa_msghdr *ifa;
    struct Ip_sockaddr_in6  *addrs[IPNET_RTAX_MAX];
    Ip_size_t                offset = 0;

    offset = 0;
    while (offset < buf_len)
    {
        struct Ip_ifaddrs        *nifa;

        ifa = (struct Ipnet_ifa_msghdr *) &ifa_first[offset];
#if defined(IP_PORT_VXWORKS) && !defined(_WRS_KERNEL)
        ipnet_cmd_init_addrs_rtp(ifa + 1, ifa->ifam_addrs, (struct Ip_sockaddr **) addrs);
#else
        ipnet_cmd_init_addrs(ifa + 1, ifa->ifam_addrs, (struct Ip_sockaddr **) addrs);
#endif

        if (ifa->ifam_type != IPNET_RTM_NEWADDR || addrs[IPNET_RTAX_IFA]->sin6_family != IP_AF_INET6)
            break;

        offset += ifa->ifam_msglen;

        ipcom_memset(&ifareq, 0, sizeof(ifareq));
        ipcom_memcpy(ifareq.ifra_name, ifname, IP_IFNAMSIZ);
        ipcom_memcpy(&ifareq.ifra_addr, addrs[IPNET_RTAX_IFA], sizeof(ifareq.ifra_addr));
        (void)ipcom_socketioctl(fd, IP_SIOCXGIFADDR_IN6, &ifareq);

        if (IP_BIT_ISSET(ifareq.ifra_flags, IP_IN6_IFF_ANYCAST))
            continue;

        if (IP_IN6_IS_ADDR_MULTICAST(&addrs[IPNET_RTAX_IFA]->sin6_addr))
            continue;

        nifa = ipcom_create_ifaddrs_entry(ifname, ifm, ifa, (struct Ip_sockaddr **)addrs);
        if (nifa != IP_NULL)
        {
            **tail = nifa;
            *tail  = &nifa->ifa_next;
        }
    }

    return offset;
}
#endif /* IPCOM_USE_INET6 */

/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_getifaddrs
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_getifaddrs(struct Ip_ifaddrs **ifa)
{
    int                     name[6];
    Ip_u8                   *if_data;
    Ip_size_t               if_data_len;
    Ip_size_t               if_data_offset;
    struct Ipnet_if_msghdr  *ifm;
    struct Ip_ifaddrs       **tail  = ifa;
    struct Ip_sockaddr_dl   *dl;
    struct Ipnet_rt_msghdr  *rtm;
    Ip_fd                   fd = IP_INVALID_SOCKET;
    char                    ifname[IP_IFNAMSIZ];
    Ip_size_t               if_name_len;

    if(ifa == IP_NULL)
        return -1;
    *ifa = IP_NULL;


    name[0] = IP_CTL_NET;
    name[1] = IP_AF_ROUTE;
    name[2] = 0;    /* Always 0 */
    name[3] = 0;
    name[4] = IP_NET_RT_IFLIST;
    name[5] = 0;

redo_alloc:
    if (ipcom_sysctl(name, 6, IP_NULL, &if_data_len, IP_NULL, 0) < 0)
    {
        IPCOM_LOG1(ERR, "ipcom_getifaddrs() :: ipcom_sysctl(%d) failed!"IP_LF, ipcom_errno);
        return -1;
    }

    if_data = ipcom_malloc(if_data_len);
    if (if_data == IP_NULL)
    {
        IPCOM_LOG1(ERR, "ipcom_getifaddrs() :: ipcom_malloc(%d) failed!"IP_LF, if_data_len);
        return -1;
    }

    if (ipcom_sysctl(name, 6, if_data, &if_data_len, IP_NULL, 0) < 0)
    {
        if (ipcom_errno == IP_ERRNO_ENOMEM)
        {
            ipcom_free(if_data);
            goto redo_alloc;
        }

        IPCOM_LOG1(ERR, "ipcom_getifaddrs() :: ipcom_sysctl(%d) failed!"IP_LF, ipcom_errno);
        goto cleanup;
    }

#if defined(IPCOM_USE_INET)
    fd = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, IP_IPPROTO_UDP);
#elif defined(IPCOM_USE_INET6)
    fd = ipcom_socket(IP_AF_INET6, IP_SOCK_DGRAM, IP_IPPROTO_UDP);
#endif
    if (fd == IP_INVALID_SOCKET)
        goto cleanup;

    if_data_offset = 0;
    while (if_data_len > if_data_offset + sizeof(struct Ipnet_if_msghdr))
    {
        ifm = (struct Ipnet_if_msghdr *) &if_data[if_data_offset];
        if_data_offset += ifm->ifm_msglen;
        rtm = (struct Ipnet_rt_msghdr *)(void *)ifm;
        switch (rtm->rtm_type) {
			case IPNET_RTM_IFINFO:
				if (ifm->ifm_addrs & IPNET_RTA_IFP) {
					dl = (struct Ip_sockaddr_dl *)(void *)(ifm + 1);
					ipcom_memset(ifname,0,IP_IFNAMSIZ);
					ipcom_memcpy(ifname, dl->sdl_data,(Ip_size_t)dl->sdl_nlen);
					break; /* Got the name! */
				}
			default:
				continue; /* No name, continue */
        }        
        
#ifdef IPCOM_USE_INET
        if_data_offset += ipcom_create_ifaddrs_inet(fd,
                                                    ifname,
                                                    &if_data[if_data_offset],
                                                    if_data_len - if_data_offset,
                                                    ifm,
                                                    &tail);
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
        if_data_offset += ipcom_create_ifaddrs_inet6(fd,
                                                     ifname,
                                                     &if_data[if_data_offset],
                                                     if_data_len - if_data_offset,
                                                     ifm,
                                                     &tail);
#endif /* IPCOM_USE_INET6 */


    }

cleanup:
    if (fd != IP_INVALID_SOCKET)
        ipcom_socketclose(fd);
    ipcom_free(if_data);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_freeifaddrs
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_freeifaddrs(struct Ip_ifaddrs *ifa)
{
    struct Ip_ifaddrs *next;

    if(ifa == IP_NULL)
        return;
    do
    {
        /* Clean the entry */
        next = ifa->ifa_next;
        ipcom_ifaddrs_free(ifa);
        ifa = next;
    }
    while(ifa != IP_NULL);
}

#else
int ipcom_ifaddrs_empty_file = 0;
#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
