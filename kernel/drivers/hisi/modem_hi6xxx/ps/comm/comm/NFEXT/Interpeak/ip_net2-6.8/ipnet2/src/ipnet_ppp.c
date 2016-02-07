/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_ppp.c,v $ $Revision: 1.116 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_ppp.c,v $
 *   $Author: tneale $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
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

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#include "ipnet_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef IPPPP

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_type.h>

#include <ipppp.h>
#include <ipppp_ipstack.h>

#include "ipnet_h.h"
#include "ipnet_netif.h"
#include "ipnet_pkt_queue.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Number of bits in the host part (EUI-64) when using the
 * PPP to create a IPv6 address (RFC 2464?)
 */
#define IPPPP_IP6_ID_BITS     64


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

IP_STATIC int ipnet_ppp_input(struct Ipnet_netif_struct *netif, Ipcom_pkt *pkt);
IP_STATIC int ipnet_ppp_ioctl(struct Ipnet_netif_struct *netif, Ip_u32 command, void *data);
IP_STATIC int ipnet_ppp_ioevent(Ipcom_netif *netif, int command, void *data, int fcflags);

#ifdef IPCOM_USE_INET
IP_STATIC int ipnet_ppp_ip4_output(struct Ipnet_netif_struct *netif, struct Ipnet_route_entry_struct *rt, Ipcom_pkt *pkt);
#endif

#ifdef IPCOM_USE_INET6
IP_STATIC int ipnet_ppp_ip6_output(struct Ipnet_netif_struct *netif, struct Ipnet_route_entry_struct *rt, Ipcom_pkt *pkt);
#endif

IP_PUBLIC int ipppp_if_init2(void *voidif);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

Ipcom_once_t ipnet_ppp_ioevent_once = IPCOM_ONCE_INIT;

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_ppp_input
 *===========================================================================
 * Description: Input handler for ethernet frames. pkt->start is the offset
 *              to the ethernet header.
 * Parameters:  pkt - The ethernet frame.
 * Returns:     0 = success, <0 error code
 *
 */
IP_STATIC int
ipnet_ppp_input(struct Ipnet_netif_struct *netif, Ipcom_pkt *pkt)
{
    ip_assert(ipnet_is_stack_task());

    return ipppp_input(&netif->ipcom, pkt);
}


/*
 *===========================================================================
 *                    ipnet_ppp_ioctl
 *===========================================================================
 * Description: PPP IO control.
 * Parameters:  netif - The interface on which the multicast address will
 *              be used.
 *              command - ioctl command.
 *              data - Depends on command.
 * Returns:     0 = success, < 0 = error code
 *
 */
IP_STATIC int
ipnet_ppp_ioctl(struct Ipnet_netif_struct *netif, Ip_u32 command, void *data)
{
    ip_assert(ipnet_is_stack_task());

    return ipppp_ioctl(&netif->ipcom, command, data);
}


/*
 *===========================================================================
 *                    ipnet_ppp_ip4_output
 *===========================================================================
 * Description:  Outputs a IPv4 packet.
 * Parameters:
 * Returns:      0 = success, <0 = error code.
 *
 */
#ifdef IPCOM_USE_INET
IP_STATIC int
ipnet_ppp_ip4_output(struct Ipnet_netif_struct *netif, struct Ipnet_route_entry_struct *rt, Ipcom_pkt *pkt)
{
    IPCOM_UNUSED_ARG(rt);
    ip_assert(ipnet_is_stack_task());

    return ipppp_ip4_output(&netif->ipcom, pkt);
}
#endif


#ifdef IPCOM_USE_INET6
/*
 *===========================================================================
 *                    ipnet_ppp_ip6_output
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_ppp_ip6_output(struct Ipnet_netif_struct *netif, struct Ipnet_route_entry_struct *rt, Ipcom_pkt *pkt)
{
    IPCOM_UNUSED_ARG(rt);
    ip_assert(ipnet_is_stack_task());

    return ipppp_ip6_output(&netif->ipcom, pkt);
}


/*
 *===========================================================================
 *                    ipnet_ppp_ip6_create_addr
 *===========================================================================
 * Description: Creates an unique address based on the passed prefix.
 * Parameters:  netif - The interface to which this address will be bound.
 *              prefix - The network identifier part of the address.
 *              in6_addr - Will hold a unique address if the function is
 *              successful.
 * Returns:     0 = succeess. <0 = error code.
 *
 */
IP_STATIC int
ipnet_ppp_ip6_create_addr(Ipnet_netif *netif,
                          IP_CONST struct Ip_in6_addr *prefix,
                          struct Ip_in6_addr *in6_addr)
{
    struct Ip_in6_addr mask;
    struct Ip_in6_addr host_id;
    unsigned           i;

    /* Create the EUI-64 ideaantifier for the PPP interface */
    ip_assert(ipnet_is_stack_task());

    if (netif->inet6_addr_list != IP_NULL
        && IP_IN6_IS_ADDR_MULTICAST(netif->inet6_addr_list) == IP_FALSE)
        /* Host ID already generated, reuse that host ID */
        IPNET_IP6_SET_ADDR(&host_id, &netif->inet6_addr_list->addr);
    else
    {
        /* Generate a random host ID */
        long pid_tick;

        pid_tick = (long)ipcom_getpid() + (long)ipcom_time(IP_NULL);

        /* Let's create a random number */
        host_id.in6.addr32[0] = (Ip_u32) (ipcom_random() + pid_tick);
        host_id.in6.addr32[1] = (Ip_u32) (ipcom_random() + pid_tick);
        host_id.in6.addr32[2] = (Ip_u32) (ipcom_random() + pid_tick);
        host_id.in6.addr32[3] = (Ip_u32) (ipcom_random() + pid_tick);

        /* The universal/local bit must be 0 in this case */
        IP_BIT_CLR(in6_addr->in6.addr16[4], IP_SWAP16(0x0200));
    }

    /* Create an inverted mask */
    ipcom_memset(&mask, 0, sizeof(mask));
    ipnet_route_create_mask(&mask, netif->inet6_if_id_bits);
    mask.in6.addr32[0] = ~mask.in6.addr32[0];
    mask.in6.addr32[1] = ~mask.in6.addr32[1];
    mask.in6.addr32[2] = ~mask.in6.addr32[2];
    mask.in6.addr32[3] = ~mask.in6.addr32[3];
    ipnet_route_apply_mask(&host_id, &mask, netif->inet6_if_id_bits);

    IPNET_IP6_SET_ADDR(in6_addr, prefix);
    for (i = 0; i < sizeof(host_id) / sizeof(host_id.in6.addr16[0]); i++)
        in6_addr->in6.addr16[i] |= host_id.in6.addr16[i];

    return 0;
}
#endif /* IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                         ipnet_ppp_ioevent
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_ppp_ioevent(Ipcom_netif *netif, int command, void *data, int fcflags)
{
    int retval = 0;

    IPCOM_UNUSED_ARG(fcflags);

    if (ipnet_is_stack_task())
        retval = ipppp_ioevent(netif, command, data);
    else
    {
    }

    return retval;
}


/*
 *===========================================================================
 *                       ipnet_ppp_ioevent_init
 *===========================================================================
 * Description: Creates the ppp code mutex.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipnet_ppp_ioevent_init(Ipcom_mutex *ppp_code_mutex)
{
    return ipcom_mutex_create(ppp_code_mutex);
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
 *                    ipppp_if_init2
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipppp_if_init2(void *voidif)
{
    Ipnet_netif *netif = voidif;
    int          ret;
    Ip_bool      is_stack_task = ipnet_is_stack_task();

    if (!is_stack_task)
        ipcom_ppp_ioevent(voidif, IPPPP_EIOXCODE_LOCK, IP_NULL);

    /* Setup link functions for PPP use. */
    netif->link_input           = ipnet_ppp_input;
    netif->link_ioctl           = ipnet_ppp_ioctl;
    netif->ipcom.link_ioevent   = ipnet_ppp_ioevent;
#ifdef IPCOM_USE_INET
    netif->link_ip4_output      = ipnet_ppp_ip4_output;
#endif
#ifdef IPCOM_USE_INET6
    netif->inet6_if_id_bits     = IPPPP_IP6_ID_BITS;
    netif->link_ip6_output      = ipnet_ppp_ip6_output;
    netif->link_ip6_create_addr = ipnet_ppp_ip6_create_addr;
#endif

    ret = ipppp_if_init(&netif->ipcom);

    if (!is_stack_task)
        ipcom_ppp_ioevent(voidif, IPPPP_EIOXCODE_UNLOCK, IP_NULL);

    return ret;
}


/*
 *===========================================================================
 *                    ipcom_ppp_ioevent
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_ppp_ioevent(Ipcom_netif *netif, int command, void *data)
{
    struct Ipnet_route_add_param param;
    struct Ipnet_rt_metrics      metrics;
    Ip_u32                       netmask_n;
    Ipnet_netif                 *ipnet_netif = (Ipnet_netif *)netif;
    int                          ret = 0;
    static Ipcom_mutex           ppp_code_mutex = IPCOM_MUTEX_INVALID;
    static void                 *suspend_handle = IP_NULL;

    switch(command)
    {
    case IPPPP_EIOXBLOCKOK:
        if (ipnet_is_stack_task() == IP_FALSE)
            ret = 1;      /* user process -> block is ok. */
        break;

    case IPPPP_EIOXDRVIOCTL:
        {
            Ipppp_eio_ioctl   *eio_ioctl = data;

            ip_assert(netif);
            ip_assert(ipnet_is_stack_task());

            ret = netif->drv_ioctl(netif, eio_ioctl->command, eio_ioctl->data);
            break;
        }

    case IPPPP_EIOXOUTPUT_IF:
        ip_assert(ipnet_netif);
        ip_assert(ipnet_is_stack_task());
        ret = ipnet_if_output(ipnet_netif, data);
        break;

    case IPPPP_EIOXINDEXTONETIF:
        {
            Ipnet_netif *ptr;

            ip_assert(ipnet_is_stack_task());
            ptr = ipnet_if_indextonetif(IPCOM_VR_ANY, (Ip_u32)data);
            return (int)ptr;
        }

    case IPPPP_EIOXCODE_LOCK:
        if (ipcom_once(&ipnet_ppp_ioevent_once,
                       (Ip_err(*)(void*)) ipnet_ppp_ioevent_init,
                       (void *) &ppp_code_mutex)
            != IPCOM_SUCCESS)
        {
            IP_PANIC();
            return -1;
        }

        (void)ipcom_mutex_lock(ppp_code_mutex);
        suspend_handle = ipnet_suspend_stack();
        break;

    case IPPPP_EIOXCODE_UNLOCK:
        ipnet_resume_stack(suspend_handle);
        (void)ipcom_mutex_unlock(ppp_code_mutex);
        break;

    case IPPPP_EIOXIF_LOCK:
        ip_assert(netif);
        ip_assert(ipnet_is_stack_task());
        IPNET_IF_LOCK(ipnet_netif);
        break;

    case IPPPP_EIOXIF_UNLOCK:
        ip_assert(netif);
        ip_assert(ipnet_is_stack_task());
        IPNET_IF_UNLOCK(ipnet_netif);
        break;

#ifdef IPCOM_USE_INET
    case IPPPP_EIOXINPUT_IN:
        ip_assert(ipnet_netif);
        ip_assert(ipnet_is_stack_task());
        ret = ipnet_ip4_input(ipnet_netif, data);
        break;

    case IPPPP_EIOXUP_IN:
        {
            Ipppp_eio_in  *eio_in = data;

            ip_assert(netif);
            ip_assert(ipnet_is_stack_task());

            /* Add local IPv4 address to this interface. */
            ip_assert(ipnet_netif->inet4_addr_list == IP_NULL
                      || ipnet_netif->inet4_addr_list->type != IPNET_ADDR_TYPE_UNICAST);
            netmask_n = 0xffffffff;
            ret = ipnet_ip4_add_addr(ipnet_netif, eio_in->local_addr_n, netmask_n,
                                     IP_TRUE, IPNET_ADDR_TYPE_UNICAST);
            if (ret < 0)
            {
                IP_PANIC2();
                return ret;
            }

            /* Add a host route to the peer. */
            ipcom_memset(&param, 0, sizeof(struct Ipnet_route_add_param));
            ipcom_memset(&metrics, 0, sizeof(struct Ipnet_rt_metrics));
            param.domain = IP_AF_INET;
            param.vr     = ipnet_netif->vr_index;
            param.table  = IPCOM_ROUTE_TABLE_DEFAULT;
            param.netif  = ipnet_netif;
            param.key    = &eio_in->peer_addr_n;
            param.flags  = IPNET_RTF_UP | IPNET_RTF_HOST | IPNET_RTF_DONE;
            if (ipppp_sysvar_get_conf_string(netif, IP_NULL, "ipppp.flags", "proxyarp"))
                IP_BIT_SET(param.flags, IPNET_RTF_PROTO2);
            param.metrics = &metrics;
            metrics.rmx_hopcount = netif->metric;
            metrics.rmx_expire   = IPCOM_ADDR_INFINITE;
            ret = ipnet_route_add(&param);
            if (ret < 0)
            {
                IP_PANIC2();
                return ret;
            }

            /* Add/Change default gateway to peer. */
            if (ipppp_sysvar_get_conf_string(netif, IP_NULL, "ipppp.flags", "defaultroute"))
            {
                struct Ip_sockaddr_in  gw;
                Ip_u32  inaddr_any = 0;

                (void)ipnet_route_delete(IP_AF_INET, ipnet_netif->vr_index, &inaddr_any);

                ipcom_memset(&gw, 0, sizeof(struct Ip_sockaddr_in));
                gw.sin_family = IP_AF_INET;
                IPCOM_SA_LEN_SET(&gw, sizeof(struct Ip_sockaddr_in));
                gw.sin_addr.s_addr = eio_in->peer_addr_n;

                param.flags = IPNET_RTF_UP | IPNET_RTF_GATEWAY | IPNET_RTF_DONE;
                param.gateway = (struct Ip_sockaddr *)&gw;
                param.key = &inaddr_any;
                param.netmask = &inaddr_any;
                ret = ipnet_route_add(&param);
                if (ret < 0)
                {
                    IP_PANIC2();
                    return ret;
                }
            }
        }
        break;

    case IPPPP_EIOXDOWN_IN:
        ip_assert(ipnet_netif);
        ip_assert(ipnet_is_stack_task());
        ipnet_if_clean(ipnet_netif, IP_TRUE, IP_FALSE);
        break;

    case IPPPP_EIOXDSTADDR_IN:
        {
            struct Ip_ifreq  ifreq;
            struct Ip_sockaddr_in   *inaddr = (struct Ip_sockaddr_in *)&ifreq.ip_ifr_addr;
            Ip_u32 dstaddr = ((struct Ip_in_addr *)data)->s_addr;

            Ipnet_netif *tmpif;
            unsigned     i;

            ip_assert(ipnet_is_stack_task());

            dstaddr = ip_htonl(dstaddr);
            IPNET_NETIF_FOR_EACH(tmpif, i)
            {
                if (IP_BIT_ISFALSE(tmpif->ipcom.flags, IP_IFF_POINTOPOINT))
                    continue;
                (void)ipnet_if_link_ioctl(tmpif, IP_SIOCGIFDSTADDR, &ifreq);
                if (inaddr->sin_addr.s_addr == dstaddr)
                    return 1;
            }
        }
        return 0;

#endif /* ifdef IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    case IPPPP_EIOXINPUT_IN6:
        ip_assert(ipnet_netif);
        ip_assert(ipnet_is_stack_task());
        ret = ipnet_ip6_input(ipnet_netif, data);
        break;

    case IPPPP_EIOXUP_IN6:
        {
            struct Ipnet_ipv6_key rt6[2];
            Ipppp_eio_in6  *eio_in6 = data;

            ip_assert(netif);
            ip_assert(ipnet_is_stack_task());

            /* Add IPv6 address. */
            ret = ipnet_ip6_add_addr(ipnet_netif, &eio_in6->local_addr, 0,
                                     IPCOM_ADDR_INFINITE, IPCOM_ADDR_INFINITE, 128);
            if (ret < 0)
                return ret;

            /* Add cloning route for link local net. */
            ipcom_memset(&param, 0, sizeof(struct Ipnet_route_add_param));
            ipcom_memset(&metrics, 0, sizeof(struct Ipnet_rt_metrics));
            ipcom_memset(rt6, 0, sizeof(struct Ipnet_ipv6_key) * 2);
            rt6[0].scope_id = netif->ifindex;
            rt6[0].addr.in6.addr8[0] = 0xfe;
            rt6[0].addr.in6.addr8[1] = 0x80;
            rt6[1].scope_id = 0xffffffff;
            rt6[1].addr.in6.addr32[0] = 0xffffffff;
            rt6[1].addr.in6.addr32[1] = 0xffffffff;
            param.domain  = IP_AF_INET6;
            param.vr      = ipnet_netif->vr_index;
            param.table   = IPCOM_ROUTE_TABLE_DEFAULT;
            param.netif   = ipnet_netif;
            param.key     = &rt6[0];
            param.netmask = &rt6[1];
            param.flags   = IPNET_RTF_UP | IPNET_RTF_MASK | IPNET_RTF_CLONING | IPNET_RTF_DONE;
            param.metrics = &metrics;
            metrics.rmx_hopcount = netif->metric;
            metrics.rmx_expire   = IPCOM_ADDR_INFINITE;
            ret = ipnet_route_add(&param);
            if (ret < 0)
            {
                IP_PANIC2();
                return ret;
            }

            /* Add link local multicast route. */
            rt6[0].addr.in6.addr8[0] = 0xff;
            rt6[0].addr.in6.addr8[1] = 0x02;
            param.flags = IPNET_RTF_UP  | IPNET_RTF_MASK | IPNET_RTF_DONE;
            ret = ipnet_route_add(&param);
            if (ret < 0)
            {
                IP_PANIC2();
                return ret;
            }

            /* Add IPv6 host route to peer. */
            ipcom_memcpy(&rt6[0].addr, &eio_in6->peer_addr, sizeof(struct Ip_in6_addr));
            param.netmask = IP_NULL;
            param.flags = IPNET_RTF_UP | IPNET_RTF_HOST | IPNET_RTF_DONE;
            ret = ipnet_route_add(&param);
            if (ret < 0)
            {
                IP_PANIC2();
                return ret;
            }
        }
        break;

    case IPPPP_EIOXDOWN_IN6:
        ip_assert(ipnet_netif);
        ip_assert(ipnet_is_stack_task());
        ipnet_if_clean(ipnet_netif, IP_FALSE, IP_TRUE);
        break;
#endif /* ifdef IPCOM_USE_INET6 */

    default:
        ip_assert(ipnet_netif);
        ip_assert(ipnet_is_stack_task());
        ipnet_kioevent(ipnet_netif, command, data, 0);
        break;
    }

    return ret;
}

#endif /* IPPPP */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

