/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_waitif.c,v $
 *     $Author: uolofsso $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * ipcom_waitif.c contains an example of how to wait for an interface to
 * acquire an IP address, e.g. when DHCP is used.
 *
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"

#ifdef IPCOM_USE_WAITIF

#define IPCOM_SYSLOG_FACILITY IPCOM_LOG_IPCOM
#ifdef IP_DEBUG
#define IPCOM_SYSLOG_PRIORITY IPCOM_LOG_DEBUG
#else
#define IPCOM_SYSLOG_PRIORITY IPCOM_LOG_WARNING
#endif


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_inet.h>
#include <ipcom_syslog.h>
#include <ipnet_routesock.h>
#include <ipcom_waitif.h>


/*
 ****************************************************************************
 * 5                    DEFINES
 ****************************************************************************
 */

/* Max # of interfaces to poll */
#define IPNET_WAITIF_MAXIF 16

/* Max timeout to wait for an address */
#define IPNET_WAITIF_TIMEOUT 60

/*
 ****************************************************************************
 * 9                    FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_waitif_gifaddr
 *===========================================================================
 * Description: Get interface address.
 * Parameters:  fd - socket descriptor
 *              ifindex - Interface index.
 *              domain - The address domain.
 * Returns:     0 = No address available, 1 = No address available, -1 = error.
 *
 */
IP_STATIC int
ipcom_waitif_gifaddr(Ip_fd fd, int ifindex, int domain)
{
    int ret = -1;

    switch (domain)
    {
    case IP_AF_INET:
    {
        struct Ip_ifreq ifreq;

        /* Get if name */
        ipcom_memset(&ifreq, 0, sizeof(ifreq));
        (void)ipcom_if_indextoname(ifindex, ifreq.ifr_name);

        /* Get main address */
        if (ipcom_socketioctl(fd, IP_SIOCGIFADDR, &ifreq) < 0)
        {
            if (ipcom_errno != IP_ERRNO_EADDRNOTAVAIL && ipcom_errno != IP_ERRNO_ENXIO)
            {
                IPCOM_LOG1(ERR, "ipcom_waitif :: ipcom_socketioctl(SIOCGIFADDR) failed, errno = %d", ipcom_errno);
            }
        }
        else
        {
            struct Ip_in_addr network = ((struct Ip_sockaddr_in *)&ifreq.ip_ifr_addr)->sin_addr;

            if (network.s_addr != IP_INADDR_DEFAULT)
            {
                ret = 0;
            }
        }
        break;
    }

#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
    {
        int len = IPNET_WAITIF_MAXIF * sizeof(struct Ip_ifreq);
        char ifname[IP_IFNAMSIZ];
        struct Ip_ifconf ifc;
        char *buf = IP_NULL;
        char *ptr;

        /* Get if name */
        (void)ipcom_if_indextoname(ifindex, ifname);

        /* Get if data buffer */
        if ((buf = ipcom_malloc(IPNET_WAITIF_MAXIF * sizeof(struct Ip_ifreq))) == IP_NULL)
        {
            IPCOM_LOG0(ERR, "ipcom_waitif :: Out of memory");
            goto leave;
        }

        /* Enter buf in ifconf struct */
        ifc.ifc_len = len;
        ifc.ip_ifc_buf = buf;

        if (ipcom_socketioctl(fd, IP_SIOCGIFCONF, &ifc) < 0)
        {
            IPCOM_LOG1(ERR, "ipcom_socketioctl(SIOCGIFCONF) failed, errno = %d\n", ipcom_errno);
            goto leave;
        }

        /* Loop for all interfaces */
        for (ptr = buf; ptr < buf + ifc.ifc_len; )
        {
            struct Ip_ifreq *ifr = (struct Ip_ifreq *)ptr;
            char *cptr;

            /* Calculate pointer to next if */
            len = IP_MAX(sizeof(struct Ip_sockaddr_in), ifr->ip_ifr_addr.sa_len);
            ptr += sizeof(ifr->ifr_name) + len;

            if ((cptr = ipcom_strchr(ifr->ifr_name, ':')) != IP_NULL)
            {
                *cptr = '\0';
            }

            /* Only for v6 addresses */
            if (ipcom_strcmp(ifr->ifr_name, ifname) == 0 &&
                ifr->ip_ifr_addr.sa_family == IP_AF_INET6)
            {
                struct Ip_sockaddr_in6 *in6 = (struct Ip_sockaddr_in6 *)&ifr->ip_ifr_addr;

                if (!IP_IN6_IS_ADDR_UNSPECIFIED(&in6->sin6_addr))
                {
                    ret = 0;
                    break;
                }
            }
        }

leave:
        if (buf != IP_NULL)
        {
            ipcom_free(buf);
        }
        break;
    }
#endif /* IPCOM_USE_INET6 */
    }

    return ret;
}


/*
 *===========================================================================
 *                    ipcom_waitif
 *===========================================================================
 * Description: Wait for interface to get an IP address assigned.
 * Parameters:  ifindex - Index of the interface to wait for.
 *                        0 = any interface except loopback.
 *              domain - The address domain.
 * Returns:     0 = success, -1 = error, and sets errno.
 *
 */
IP_PUBLIC int
ipcom_waitif(int ifindex, int domain)
{
    struct Ipnet_rs_msghdr rtmsg;
    struct Ip_timeval stop;
    struct Ip_timeval tv;
    Ip_fd fd = IP_INVALID_SOCKET;
    int ret = -1;

    /* Check domain */
    if (domain != IP_AF_INET
#ifdef IPCOM_USE_INET6
        && domain != IP_AF_INET6
#endif /* IPCOM_USE_INET6 */
       )
    {
        IPCOM_LOG1(ERR, "ipcom_waitif :: invalid domain: %d", domain);
        ipcom_errno_set(IP_ERRNO_EINVAL);
        goto leave;
    }

    /* Get routing socket */
    fd = ipcom_socket(IP_AF_ROUTE, IP_SOCK_RAW, 0);
    if (fd == IP_INVALID_SOCKET)
    {
        IPCOM_LOG1(ERR, "ipcom_waitif :: ipcom_socket(ROUTE, RAW) failed, errno = %d", ipcom_errno);
        goto leave;
    }

    /* Get stop time */
    ipcom_microtime(&tv);
    stop.tv_sec = tv.tv_sec + IPNET_WAITIF_TIMEOUT;

    /* Loop and wait for address */
    for (;;)
    {
        Ip_ssize_t length;

        /* Check if address available */
        if (ifindex == 0)
        {
            int i;

            /* Any if will do */
            for (i = 2; i < IPNET_WAITIF_MAXIF; i++)
            {
                if (ipcom_waitif_gifaddr(fd, i, domain) == 0)
                {
                    ret = 0;
                    goto leave;
                }
            }
        }
        else if (ipcom_waitif_gifaddr(fd, ifindex, domain) == 0)
        {
            ret = 0;
            goto leave;
        }

again:
        /* Get timeout time */
        ipcom_microtime(&tv);
        tv.tv_sec = stop.tv_sec - tv.tv_sec;
        tv.tv_usec = 0;
        if (tv.tv_sec <= 0)
        {
            ipcom_errno_set(IP_ERRNO_ETIMEDOUT);
            goto leave;
        }

        /* Set receive timeout */
        if (ipcom_setsockopt(fd, IP_SOL_SOCKET, IP_SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
        {
            IPCOM_LOG1(ERR, "ipcom_waitif :: ipcom_setsockopt(RCVTIMEO) failed, errno = %d", ipcom_errno);
            goto leave;
        }

        /* Read routing message headers only (truncated read). */
        length = ipcom_recv(fd, &rtmsg, sizeof(rtmsg), 0);
        if (length == IP_SOCKERR)
        {
            if (ipcom_errno != IP_ERRNO_ETIMEDOUT && ipcom_errno != IP_ERRNO_EWOULDBLOCK)
            {
                IPCOM_LOG1(ERR, "ipcom_waitif :: ipcom_recv() failed, errno = %d", ipcom_errno);
            }

            goto leave;
        }

        /* Only bother about newaddr */
        if (rtmsg.type != IPNET_RTM_NEWADDR)
        {
            goto again;
        }
    }

leave:
    if (fd != IP_INVALID_SOCKET)
    {
        ipcom_socketclose(fd);
    }

    return ret;
}

#else
int ipcom_waitif_empty_file;
#endif /* IPCOM_USE_WAITIF */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
