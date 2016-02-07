/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_ioctl.c,v $ $Revision: 1.239.6.1.4.2 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_ioctl.c,v $
 *     $Author: ulf $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * ipcom_socketioctl() function.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#define IPCOM_SKIP_NATIVE_SOCK_API

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#include <ipnet_config.h>


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_err.h>
#include <ipcom_syslog.h>

#include "ipnet.h"
#include "ipnet_classifier.h"
#include "ipnet_h.h"
#include "ipnet_loopback.h"
#include "ipnet_route.h"
#include "ipnet_usr_sock.h"

#ifdef IPNET_USE_TUNNEL
#include "ipnet_tunnel.h"
#endif

#ifdef IPNET_USE_VLAN
#include "ipnet_eth.h"
#endif

#ifdef IPNET_USE_PCAP
#include "ipnet_pcap.h"
#endif

#ifdef IPTCP
#include <iptcp.h>
#endif

#ifdef IPMPLS
#include <ipmpls.h>
#endif

#ifdef IPMIP4
#include <private/ipmip4_ipnet.h>
#endif

#ifdef IPIPSEC2
#include <ipipsec.h>
#include <ipipsec_pfkeyv2.h>
#endif

#ifdef IPCOM_WV_INSTRUMENTATION
#include <ipcom_windview.h>
#endif

#ifdef IPROHC
#include <iprohc.h>
#endif


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

struct Ipnet_ifconf_entry
{
    char    if_name[IP_IFNAMSIZ];

    union ifconf_addr
    {
        struct Ip_sockaddr     sa;
        struct Ip_sockaddr_dl  sdl;
        struct Ip_sockaddr_in  sin;
#ifdef IPCOM_USE_INET6
        struct Ip_sockaddr_in6 sin6;
#endif
    } s;
};


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

#ifdef IPIPSEC2
IP_GLOBAL Ip_err ipipsec_conf_internal(Ip_tag *ctrl_tags);
IP_GLOBAL Ip_err ipipsec_ctrl_internal(Ip_tag *ctrl_tags);
IP_GLOBAL Ip_err ipipsec_sactrl_internal(Ipipsec_sa_handle *sah, Ip_tag *ctrl_tags);
#endif


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
 *                      ipnet_ioctl_sigalarm
 *===========================================================================
 * Description: Raises EINTR error on all socket owned by the task with PID
 *              equal to *ppid.
 * Parameters:  ppid - Points to the PID of the application to interrupt
 *                     or IP_NULL if the EINTR should be cleared.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ioctl_sigalarm(Ip_pid_t *ppid)
{
    Ipnet_socket *sock;
    int           i;
    Ip_pid_t      pid;

    if (ppid == IP_NULL)
        pid = ipnet_get_effective_pid();
    else
        pid = *ppid;

    for (i = 0; i < ipnet_conf_max_sockets; i++)
    {
        sock = ipnet->sockets[i];
        if (sock != IP_NULL
            && sock->ipcom.pid == pid
            && (sock->ipcom.so_errno == 0 || sock->ipcom.so_errno == IP_ERRNO_EINTR))
        {
            if (ppid == IP_NULL)
                sock->ipcom.so_errno = 0;
            else
                ipnet_sock_data_avail(sock, IP_ERRNO_EINTR, 0);
        }
    }

    ipcom_free(ppid);
}


/*
 *===========================================================================
 *                    ipnet_ioctl_if_is_rt_affected
 *===========================================================================
 * Description: Checks if the specified route is affected when the interface
 *              change route table.
 * Parameters:  netif - The network interface.
 *              rt - The route to test.
 * Returns:     IP_TRUE - Route is affected.
 *              IP_FALSE - Route is not affected.
 *
 */
IP_STATIC Ip_bool
ipnet_ioctl_if_is_rt_affected(Ipnet_netif *netif, Ipnet_route_entry *rt)
{
    if (rt->netif == IP_NULL)
        return IP_FALSE;

    if (rt->netif->ipcom.ifindex == netif->ipcom.ifindex)
        return IP_TRUE;

#ifdef IPCOM_USE_INET
    if (IPNET_ROUTE_GET_FAMILY(rt->head) == IP_AF_INET)
    {
        struct Ipnet_ipv4_key *ipv4_key = (struct Ipnet_ipv4_key *) rt->hdr.key;

        if (ipnet_ip4_get_addr_type2(ipv4_key->addr.s_addr, netif) == IPNET_ADDR_TYPE_UNICAST)
            return IP_TRUE;
    }
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    if (IPNET_ROUTE_GET_FAMILY(rt->head) == IP_AF_INET6)
    {
        struct Ipnet_ipv6_key *ipv6_key = (struct Ipnet_ipv6_key *) rt->hdr.key;

        if (ipnet_ip6_get_addr_type2(&ipv6_key->addr, netif) == IPNET_ADDR_TYPE_UNICAST
            && (ipnet_ip6_is_scoped_addr(&ipv6_key->addr) == IP_FALSE
                || ipv6_key->scope_id == ipnet_ip6_get_zone(netif->ipcom.ifindex,
                                                            &ipv6_key->addr)))
            return IP_TRUE;
    }
#endif /* IPCOM_USE_INET6 */

    return IP_FALSE;
}


/*
 *===========================================================================
 *                    ipnet_ioctl_if_remove_routes
 *===========================================================================
 * Description: Removes all routes that uses the specified interface.
 * Parameters:  netif - The network interface.
 *              vr - The route table from which the routes should be removed.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ioctl_if_remove_routes(Ipnet_netif *netif, Ip_u16 vr)
{
    Ipnet_route_entry *current;
    int                domain[2] = { 0, 0 };
    unsigned           num_domains = 0;
    unsigned           i, t;
    unsigned           num_tables = 0;
    Ip_u32            *table_ids;

#ifdef IPCOM_USE_INET
    domain[num_domains++] = IP_AF_INET;
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    domain[num_domains++] = IP_AF_INET6;
#endif /* IPCOM_USE_INET */

    if (ipnet_route_get_table_ids(vr, IP_NULL, &num_tables) < 0)
    {
        /* Can only happen if there is a bug in the code */
        IP_PANIC();
        return;
    }

    table_ids = ipcom_calloc(num_tables, sizeof(*table_ids));
    if (table_ids == IP_NULL)
    {
        IP_PANIC2();
        return;
    }

    if (ipnet_route_get_table_ids(vr, table_ids, &num_tables) < 0)
    {
        /* Can only happen if there is a bug in the code */
        IP_PANIC();
        return;
    }

    for (i = 0; i < num_domains; i++)
        for (t = 0; t < num_tables; t++)
        {
            current = ipnet_route_first_entry(domain[i], vr, table_ids[t]);

            while (current != IP_NULL)
            {
                if (ipnet_ioctl_if_is_rt_affected(netif, current) == IP_FALSE)
                {
                    current = ipnet_route_next_entry(current);
                    continue;
                }
                (void)ipnet_route_delete2(domain[i],
                                          vr,
                                          IPNET_ROUTE_GET_TABLE(current->head),
                                          current->hdr.key,
                                          current->hdr.mask,
                                          current->gateway,
                                          current->netif->ipcom.ifindex,
                                          0,
                                          0,
                                          IP_FALSE);
                current = ipnet_route_first_entry(domain[i], vr, table_ids[t]);
            }
        }
}


/*
 *===========================================================================
 *                    ipnet_ioctl_if_queue
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_ioctl_if_queue(unsigned long request,
                     struct Ipnet_ifqueue *ifq,
                     Ipnet_netif *netif)
{
    Ipnet_pkt_queue *q;

    switch (request)
    {
    case IPNET_SIOCGIFQUEUE:
        if (netif->snd_queue == IP_NULL)
        {
            ifq->ifq_id        = IP_IFQ_ID_DEFAULT;
            ifq->ifq_parent_id = IP_IFQ_ID_NONE;
            ipcom_strncpy(ifq->ifq_type, "none", sizeof(ifq->ifq_type));
        }
        else
        {
            q = netif->snd_queue;
            if (ifq->ifq_id != IP_IFQ_ID_NONE && ifq->ifq_id != q->id)
            {
                if (q->c_ops == IP_NULL)
                    return -IP_ERRNO_ESRCH;

                q = q->c_ops->q_get(q, ifq->ifq_id);
                if (q == IP_NULL)
                    return -IP_ERRNO_ESRCH;
            }
            ifq->ifq_id        = q->id;
            ifq->ifq_parent_id = q->parent_id;
            ipcom_strncpy(ifq->ifq_type, q->type, sizeof(ifq->ifq_type));
            q->dump(q, &ifq->ifq_data);
            ifq->ifq_count = q->count(q);
        }
        break;

    case IPNET_SIOCSIFQUEUE:
        ipnet_if_clean_snd_queue(netif);
        if (ipcom_strcmp(ifq->ifq_type, "none") == 0)
        {
            ipnet_pkt_queue_delete(netif->snd_queue);
            netif->snd_queue = IP_NULL;
        }
        else
        {
            int ret;

            ret = ipnet_pkt_queue_new(ifq, netif, &q);
            if (ret < 0)
                return ret;
        }
        break;

    default:
        IP_PANIC();
        return -IP_ERRNO_EINVAL;
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ioctl_if_queue_filter
 *===========================================================================
 * Description: Controls IPNET interface queue filters for classfull interface
 *              queues.
 * Parameters:  request - request code
 *              f - filter parameters
 *              netif - the interface the queue is installed at
 * Returns:
 *
 */
IP_STATIC int
ipnet_ioctl_if_queue_filter(unsigned long request,
                            struct Ipnet_ifqueue_filter *f,
                            Ipnet_netif *netif)
{
    Ipnet_pkt_queue     *q = netif->snd_queue;
    Ipnet_pkt_queue     *child_q;

    if (q == IP_NULL || q->c_ops == IP_NULL)
    {
        IPCOM_LOG3(WARNING, "ioctl: Could not set filter since queue %s:%d at %s does not support that operation",
                   q ? q->type : "none", q ? q->id : 0, netif->ipcom.name);
        return -IP_ERRNO_EINVAL;
    }

    q = q->c_ops->q_get(q, f->filter_queue_id);
    if (q == IP_NULL)
    {
        IPCOM_LOG2(WARNING, "ioctl: did not find a queue with ID: %d at interface %s",
                   f->filter_queue_id, netif->ipcom.name);
        return -IP_ERRNO_ESRCH;
    }
    if (q->c_ops == IP_NULL)
    {
        IPCOM_LOG3(WARNING, "ioctl: queue %s:%d at %s is not a container",
                   q->type, q->id, netif->ipcom.name);
        return -IP_ERRNO_EINVAL;
    }

    switch (request)
    {
    case IPNET_SIOCXAIFQFILTER:
        child_q = q->c_ops->q_get(q, f->filter_child_queue_id);
        if (child_q == IP_NULL || child_q->parent_id != q->id)
        {
            IPCOM_LOG3(WARNING, "ioctl: queue %s:%d has no child with ID %d",
                       q->type, q->id, f->filter_child_queue_id);
            return -IP_ERRNO_EINVAL;
        }

        return q->c_ops->f_insert(q, f->filter_id, &f->filter_rule, f->filter_child_queue_id);

    case IPNET_SIOCXDIFQFILTER:
        return q->c_ops->f_remove(q, f->filter_id);

    default:
        IP_PANIC();
        break;
    }
    return -IP_ERRNO_EINVAL;
}


/*
 *===========================================================================
 *                    ipnet_ioctl_if_siocgifconf
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_ioctl_if_siocgifconf(Ipnet_socket *sock, struct Ip_ifconf *ifconf)
{
    Ipnet_netif *netif;
    int len, ifc_len;
    Ip_u32 i;
    struct Ipnet_ifconf_entry *entry;

    entry = (struct Ipnet_ifconf_entry *)ifconf->ip_ifc_buf;
    ifc_len = 0;

    IPNET_NETIF_FOR_EACH_ON_VR(netif, sock->vr_index, i)
    {
        /* sockaddr_dl */
        len = sizeof(entry->if_name) + sizeof(struct Ip_sockaddr_dl);
        if ((ifconf->ifc_len - ifc_len) < len)
            break;
        ifc_len += len;
        ipcom_memset(entry, 0, len);
        ipcom_strcpy(entry->if_name, netif->ipcom.name);
        IPCOM_SA_LEN_SET(&entry->s.sdl, sizeof(struct Ip_sockaddr_dl));
        entry->s.sdl.sdl_family = IP_AF_LINK;
        entry->s.sdl.sdl_index  = (Ip_u16) netif->ipcom.ifindex;
        entry->s.sdl.sdl_type   = (Ip_u8) netif->ipcom.type;
        switch (netif->ipcom.type)
        {
#ifdef IPCOM_USE_ETHERNET
        case IP_IFT_ETHER:
            entry->s.sdl.sdl_type = IP_IFT_ETHER;
            entry->s.sdl.sdl_alen = 6;
            ipcom_memcpy(entry->s.sdl.sdl_data, netif->ipcom.link_addr, 6);
            break;
#endif
#ifdef IPNET_USE_WLAN
        case IP_IFT_IEEE80211:
            entry->s.sdl.sdl_type = IP_IFT_IEEE80211;
            entry->s.sdl.sdl_alen = 6;
            ipcom_memcpy(entry->s.sdl.sdl_data, netif->ipcom.link_addr, 6);
            break;
#endif
        default:
            break;
        }
        entry = (struct Ipnet_ifconf_entry *)((char*)entry + len);

        /* sockaddr_in */
#ifdef IPCOM_USE_INET
        {
            Ipnet_ip4_addr_entry *inet4_addr;    /* List of IPv4 addresses */
            for (inet4_addr = netif->inet4_addr_list; inet4_addr; inet4_addr = inet4_addr->next)
            {
                if (inet4_addr->type != IPNET_ADDR_TYPE_UNICAST)
                    continue;

                len = sizeof(entry->if_name) + sizeof(struct Ip_sockaddr_in);
                if ((ifconf->ifc_len - ifc_len) < len)
                    break;
                /* Add the unicast address */
                ifc_len += len;
                ipcom_memset(entry, 0, len);
                ipcom_strcpy(entry->if_name, netif->ipcom.name);
                entry->s.sin.sin_family = IP_AF_INET;
                IPCOM_SA_LEN_SET(&entry->s.sin, sizeof(struct Ip_sockaddr_in));
                entry->s.sin.sin_addr.s_addr = inet4_addr->ipaddr_n;
                entry = (struct Ipnet_ifconf_entry *)((char*)entry + len);
            }
        }
#endif /* IPCOM_USE_INET */

        /* sockaddr_in6 */
#ifdef IPCOM_USE_INET6
        {
            Ipnet_ip6_addr_entry *inet6_addr;    /* List of IPv6 addresses */
            for (inet6_addr = netif->inet6_addr_list; inet6_addr; inet6_addr = inet6_addr->next)
            {
                if (IP_IN6_IS_ADDR_MULTICAST(&inet6_addr->addr) || IP_BIT_ISSET(inet6_addr->flags, IPNET_IP6_ADDR_FLAG_ANYCAST))
                    continue;
                len = sizeof(entry->if_name) + sizeof(struct Ip_sockaddr_in6);
                if ((ifconf->ifc_len - ifc_len) < len)
                    break;
                ifc_len += len;
                ipcom_memset(entry, 0, len);
                ipcom_strcpy(entry->if_name, netif->ipcom.name);
                entry->s.sin.sin_family = IP_AF_INET6;
                IPCOM_SA_LEN_SET(&entry->s.sin6, sizeof(struct Ip_sockaddr_in6));
                if(IP_IN6_IS_ADDR_LINK_LOCAL(&inet6_addr->addr))
                    entry->s.sin6.sin6_scope_id = netif->ipcom.ifindex;
                ipcom_memcpy(&entry->s.sin6.sin6_addr, &inet6_addr->addr, sizeof(struct Ip_in6_addr));
                entry = (struct Ipnet_ifconf_entry *)((char*)entry + len);
            }
        }
#endif /* IPCOM_USE_INET6 */

    }

    ifconf->ifc_len = ifc_len;
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ioctl_rtab
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_ioctl_rtab(unsigned long request, void *argp)
{
    Ip_u16                     *vr_ptr = argp;
    struct Ip_sioc_route_table *srt = argp;
    int                         ret;

    switch (request)
    {
    case IP_SIOCADDVR:
        ret = ipnet_route_add_table(*vr_ptr, IPCOM_ROUTE_TABLE_DEFAULT, vr_ptr, IP_NULL);
        return ret;

    case IP_SIOCDELVR:
        return ipnet_route_delete_vr(*vr_ptr);

    case IP_SIOCADDROUTETAB:
        return ipnet_route_add_table(srt->vr, srt->table, &srt->vr, &srt->table);

    case IP_SIOCDELROUTETAB:
        if (IPCOM_ROUTE_TABLE_IS_DEFAULT(srt->table))
        {
            IPCOM_LOG0(WARNING, "Use IP_SIOCDELVR to delete a virtual router");
            return -IP_ERRNO_EACCES;
        }
        return ipnet_route_delete_table(srt->vr, srt->table);

#ifdef IPNET_USE_ROUTE_TABLE_NAMES
    case IP_SIOCGETROUTETAB:
        if (ipnet_route_vr_and_table_from_name(srt->name, &srt->vr, &srt->table) == 0)
            return 0;

        ret = ipnet_ioctl_rtab(IP_SIOCADDROUTETAB, srt);
        if (ret < 0)
            return ret;
        return ipnet_ioctl_rtab(IP_SIOCSROUTETABNAME, srt);

    case IP_SIOCSROUTETABNAME:
        return ipnet_route_set_name(srt->vr, srt->table, srt->name);

    case IP_SIOCGROUTETABNAME:
        return ipnet_route_get_name(srt->vr, srt->table, srt->name) == IP_NULL ? -IP_ERRNO_ESRCH : 0;
#endif /* IPNET_USE_ROUTE_TABLE_NAMES */

    default:
        return - IP_ERRNO_EINVAL;
    }
}


/*
 *===========================================================================
 *                    ipnet_ioctl_if
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_ioctl_if(Ipnet_socket *sock, unsigned long request, struct Ip_ifreq *ifreq)
{
    Ipnet_netif *netif;
    int          ret = 0;

    if (request != IP_SIOCGIFCONF)
        ip_assert(IP_IOCPARM_LEN(request) >= IP_IFNAMSIZ);

    switch (request)
    {
    case IP_SIOCSIFNAME:
    case IP_SIOCSIFHWBROADCAST:
        return -IP_ERRNO_EPERM;

    case IP_SIOCGIFCONF:
        return ipnet_ioctl_if_siocgifconf(sock, (struct Ip_ifconf *)ifreq);

    case IP_SIOCIFCREATE:
        if (ipnet_if_nametonetif(sock->vr_index, ifreq->ifr_name))
            return -IP_ERRNO_EEXIST;
#ifdef IPNET_USE_LOOPBACK
        if (ipcom_strncmp(ifreq->ifr_name, "lo", 2) == 0)
        {
            ret = ipnet_loopback_attach(sock->vr_index, ifreq->ifr_name);
            return ret;
        }
#endif
#ifdef IPNET_USE_VLAN
        if (ipcom_strncmp(ifreq->ifr_name, "vlan", 4) == 0)
            return ipnet_vlan_create(sock, ifreq);

        if (ipcom_strchr(ifreq->ifr_name, '.') != IP_NULL)
            return ipnet_vlan_compact_create(sock, ifreq);
#endif
#ifdef IPPPP_USE_PPPOE
        if (ipcom_strncmp(ifreq->ifr_name, "pppoe", 5) == 0)
        {
            if (ipnet->ppp.pppoe_create == IP_NULL)
                return -IP_ERRNO_EINVAL;
            return ipnet->ppp.pppoe_create(ifreq);
        }
#endif
#ifdef IPPPP_USE_PPPL2TP
        if (ipcom_strncmp(ifreq->ifr_name, "pppl2tp", 7) == 0)
        {
            if (ipnet->ppp.pppl2tp_create == IP_NULL)
                return -IP_ERRNO_EINVAL;
            return ipnet->ppp.pppl2tp_create(ifreq);
        }
#endif
#ifdef IPPPP
        if (ipcom_strncmp(ifreq->ifr_name, "ppp", 3) == 0)
        {
            if (ipnet->ppp.if_create == IP_NULL)
                return -IP_ERRNO_EINVAL;
            return ipnet->ppp.if_create(ifreq);
        }
#endif
#ifdef IPNET_USE_TUNNEL
        return ipnet_tunnel_create_bsd(sock, ifreq);
#else
        return -IP_ERRNO_EINVAL;
#endif
    case IP_X_SIOCADDTUNNEL:
#ifdef IPNET_USE_TUNNEL
        /**/
        return ipnet_tunnel_create_linux(sock, ifreq);
#else
        return -IP_ERRNO_EINVAL;
#endif

    case IP_SIOCGIFNAME:
        netif = ipnet_if_indextonetif(sock->vr_index, ifreq->ifr_ifru.ifru_ifindex);
        if (netif == IP_NULL)
            return -IP_ERRNO_ENXIO;
        ipcom_strncpy(ifreq->ifr_name, netif->ipcom.name, sizeof(ifreq->ifr_name));
        return ret;
    default:
        break;
    }

    netif = ipnet_if_nametonetif(sock->vr_index, ifreq->ifr_name);
    if (netif == IP_NULL)
        return -IP_ERRNO_ENXIO;

    switch (request)
    {
    case IP_SIOCGIFINDEX:
        ifreq->ifr_ifru.ifru_ifindex = netif->ipcom.ifindex;
        break;
    case IP_SIOCGIFMTU:
        ifreq->ifr_ifru.ifru_mtu = (int)netif->ipcom.mtu;
        break;
    case IP_SIOCSIFMTU:
        ret = ipnet_if_link_ioctl(netif, request, ifreq);
        if (ret == 0
            || ret == -IP_ERRNO_EOPNOTSUPP
            || ret == -IP_ERRNO_EINVAL)
        {
            /*
             * ioctl either successful, or not supported by the link
             * layer (IP_ERRNO_EOPNOSUPP) or by the driver
             * (IP_ERRNO_EINVAL). The 2 errors are accepted since that
             * is how setting the MTU used to work, i.e. no
             * notification to the driver at all.
             * The driver has to be notifed on VxWorks to enable
             * Ethernet jumbograms (9k frames).
             */
            netif->ipcom.mtu = (Ip_u32) ifreq->ifr_ifru.ifru_mtu;
            ret = 0;
        }
        break;
    case IP_SIOCGIFFLAGS:
        ifreq->ifr_ifru.ifru_flags = (Ip_u16)netif->ipcom.flags;
        break;
#ifdef IP_PORT_LKM
    case IP_X_SIOCGIFLINKFLAGS:
        /* LKM/LAS specific ioctl used for setting the link 0|1|2 flags */
        {
            Ip_u32 link_flags = netif->ipcom.flags;

            link_flags &= IP_IFF_LINK0 | IP_IFF_LINK1 | IP_IFF_LINK2;
            ifreq->ifr_ifru.ifru_flags = (Ip_u16)(link_flags >> 16);
        }
        break;
#endif
#ifdef IPCOM_USE_HW_CAPS
    case IP_SIOCGIFCAP:
        ifreq->ip_ifr_reqcap = 0;   /* currently not supported */
        ifreq->ip_ifr_curcap = netif->ipcom.drv_capabilities;
        break;
#endif /* IPCOM_USE_HW_CAPS */
    case IP_SIOCGIFMETRIC:
        ifreq->ifr_ifru.ifru_metric = netif->ipcom.metric;
        break;
    case IP_SIOCSIFMETRIC:
        netif->ipcom.metric = ifreq->ifr_ifru.ifru_metric;
        break;
    case IP_SIOCSIFVR:
        ret = ipnet_ioctl_move_if_rt_to_vr(netif, ifreq->ip_ifr_vr);
        break;
    case IP_SIOCGIFVR:
        ifreq->ip_ifr_vr = netif->vr_index;
        break;
    case IP_SIOCSIFFLAGS:
        ret = ipnet_netif_change_flags(&netif, ifreq->ifr_ifru.ifru_flags, 0);
        break;
#ifdef IP_PORT_LKM
    case IP_X_SIOCSIFLINKFLAGS:
        /* LKM/LAS specific ioctl used for setting the link 0|1|2 flags */
        {
            Ip_u32 link_flags = ifreq->ifr_ifru.ifru_flags << 16;

            link_flags &= IP_IFF_LINK0 | IP_IFF_LINK1 | IP_IFF_LINK2;
            IP_BIT_CLR(netif->ipcom.flags, IP_IFF_LINK0 | IP_IFF_LINK1 | IP_IFF_LINK2);
            IP_BIT_SET(netif->ipcom.flags, link_flags);
        }
        break;
#endif
    case IPNET_SIOCGIFQUEUE:
    case IPNET_SIOCSIFQUEUE:
        return ipnet_ioctl_if_queue(request, (struct Ipnet_ifqueue *) ifreq, netif);

    case IPNET_SIOCXAIFQFILTER:
    case IPNET_SIOCXDIFQFILTER:
        return ipnet_ioctl_if_queue_filter(request, (struct Ipnet_ifqueue_filter *) ifreq, netif);

    case IP_SIOCXDETACH:
        if (!ipnet_if_can_detach(&netif->ipcom))
            return -IP_ERRNO_EBUSY;

        /* Detach IPNET interface */
        ret = ipnet_if_detach(netif);
        if (ret < 0)
            return ret;

        /* Call link layer detach */
        ret = ipnet_if_link_ioctl(netif, IP_SIOCXDETACH, IP_NULL);
        if (ret < 0)
            return ret;

        /* One extra interface unlock is the same as calling ipcom_if_free() */
        IPNET_IF_UNLOCK(netif);
        break;

    case IP_SIOCXRESETSTAT:
        ipcom_memset((Ip_u8 *) netif + ip_offsetof(Ipnet_netif, ipackets),
                     0,
                     ip_offsetof(Ipnet_netif, noproto) - ip_offsetof(Ipnet_netif, ipackets));
        break;

    case IP_SIOCXGDHCPRUNNING:
        ifreq->ifr_ifru.ifru_opt = IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_X_DHCPRUNNING) ? 1 : 0;
        break;

    case IP_SIOCXSDHCPRUNNING:
        if (ifreq->ifr_ifru.ifru_opt == 1)
            IP_BIT_SET(netif->ipcom.flags, IP_IFF_X_DHCPRUNNING);
        else
            IP_BIT_CLR(netif->ipcom.flags, IP_IFF_X_DHCPRUNNING);
        ipnet_kioevent(netif, IP_EIOXCHANGE, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        break;

#ifdef IPNET_USE_PCAP
    case IP_SIOCXPCAP:
        return ipnet_pcap_ioctl(netif, request, ifreq);
#endif

    case IP_SIOCGIFTXQLEN:
#if !defined(IP_PORT_LKM)
        ifreq->ifr_ifru.ifru_ivalue = 0;
        break;
#endif
    case IP_SIOCSIFTXQLEN:
        return -IP_ERRNO_EPERM;
    default:
        /* Remaining ioctl()'s to to link layer (including ETH, PPP etc). */
        ret = ipnet_if_link_ioctl(netif, request, ifreq);
        break;
    }
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_ioctl_wlan
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPCOM_USE_WLAN
IP_STATIC int
ipnet_ioctl_wlan(Ipnet_socket *sock, unsigned long request, struct Ip_ifreq *ifreq)
{
    Ipcom_netif      *netif;
    IpcomWlanBssReq  *bssreq;
    IpcomWlanBss     *bsslist;
    int               size;
    IpcomWlanStatus   ret;
    int               i;

    netif = (Ipcom_netif *) ipnet_if_nametonetif(sock->vr_index, ifreq->ifr_name);
    if (netif == IP_NULL || netif->wlan == IP_NULL)
        return IP_ERRNO_ENODEV;

    switch (request)
    {
    case IP_SIOCXGWLANLIST:
        bssreq = (IpcomWlanBssReq  *) ifreq->ip_ifr_data;
        ret = ipcom_wlan_get_bsslist(netif, &bsslist, &size);
        if (ret != IPCOM_WLAN_STATUS_SUCCESS)
            return IPCOM_ERR_FAILED;

        /* What about user/kernel space copying here? */
        size = IP_MIN(size, IPCOM_WLAN_MAX_BSS);
        for (i = 0; i < size; i++)
            bssreq->bsslist[i] = bsslist[i];
        bssreq->num_bss = size;
        break;

    default:
        break;
    }
}
#endif /* IPCOM_USE_WLAN */


/*
 *===========================================================================
 *                    ipnet_ioctl_sock
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_ioctl_sock(Ipnet_socket *sock, unsigned long request, void *argp)
{
    int      *iargp = (int*) argp;
    Ip_pid_t *pid;

    switch (request)
    {
    case IP_SIOCSPGRP:
        sock->ipcom.pid = *iargp;
        break;

    case IP_SIOCGPGRP:
        *iargp = sock->ipcom.pid;
        break;

    case IP_FIONBIO:
        if (*iargp)
            IP_BIT_SET(sock->flags, IPNET_SOCKET_FLAG_NONBLOCKING);
        else
            IP_BIT_CLR(sock->flags, IPNET_SOCKET_FLAG_NONBLOCKING);
        break;

    case IP_X_FIOGNBIO:
        *iargp = IP_BIT_ISSET(sock->flags, IPNET_SOCKET_FLAG_NONBLOCKING) ? 1 : 0;
        break;

    case IP_FIONREAD:
        if (sock->proto != IP_IPPROTO_TCP)
        {
            /* Returns the size of the next pending datagram in the
               integer in bytes, or 0 when no datagram is pending. */
            *iargp = (sock->rcv_head == IP_NULL ? 0 : sock->rcv_head->end - sock->rcv_head->start);
            break;
        }
        /* else, fall through */
    case IP_SIOCINQ:
        /* Returns the total size of all queued packet. */
        *iargp = sock->ipcom.rcv_bytes;
        break;

    case IP_SIOCOUTQ:
        *iargp = ipcom_atomic_get(&sock->snd_bytes);
        break;

    case IP_X_SIOCSINTR:
        if (!sock->ipcom.pid)
        {
            return -IP_ERRNO_EINVAL;
        }
        pid = ipcom_malloc(sizeof(*pid));
        if (!pid)
        {
            return -IP_ERRNO_ENOMEM;
        }
        *pid = sock->ipcom.pid;

        if (ipnet_timeout_schedule((Ip_u32) *iargp * 1000,
                                   (Ipnet_timeout_handler) ipnet_ioctl_sigalarm,
                                   pid,
                                   IP_NULL) != 0)
        {
            ipcom_free(pid);
            return -IP_ERRNO_ENOMEM;
        }
        break;

    case IP_X_SIOCGINTR:
        ipnet_ioctl_sigalarm(IP_NULL);
        break;

#ifdef IP_X_SIOCSDEBUG
    case IP_X_SIOCSDEBUG:
        ipnet->debug = *iargp;
        break;
#endif

#ifdef IP_X_SIOCGDEBUG
    case IP_X_SIOCGDEBUG:
        *iargp = ipnet->debug;
        break;
#endif

    default:
        return -IP_ERRNO_EINVAL;
    }

    return 0;
}


#ifdef IPCOM_USE_INET
/*
 *===========================================================================
 *                    ipnet_ioctl_netif_from_in_addr
 *===========================================================================
 * Description: Returns the interface for which
 * Parameters:  request - IP_SIOCADDRT or IP_SIOCDELRT for add or delete a
 *              IPv4 route.
 * Returns:     0 = success, <0 = error
 *
 */
IP_STATIC Ipnet_netif *
ipnet_ioctl_netif_from_in_addr(Ipnet_socket *sock, Ip_u32 in_addr_n, Ip_u32 in_mask_n)
{
    Ip_u32                i;
    Ipnet_ip4_addr_entry *addr;
    Ipnet_netif          *netif;

    IPNET_NETIF_FOR_EACH_ON_VR(netif, sock->vr_index, i)
    {
        for (addr = netif->inet4_addr_list; addr != IP_NULL; addr = addr->next)
        {
            if (((addr->ipaddr_n ^ in_addr_n) & in_mask_n) == 0)
                return netif;
        }
    }
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_ioctl_ip4_bsd43_compat
 *===========================================================================
 * Description: Adds or deletes route. Provides compatibility with application
 *              that still uses the old BSD 4.3 API.
 * Parameters:  request - IP_SIOCADDRT or IP_SIOCDELRT for add or delete a
 *              IPv4 route.
 * Returns:     0 = success, <0 = error
 *
 */
IP_STATIC int
ipnet_ioctl_ip4_bsd43_compat(Ipnet_socket *sock, unsigned long request, struct Ip_ortentry *ort)
{
    struct Ipnet_route_add_param param;
    struct Ipnet_ipv4_key        key;
    struct Ipnet_ipv4_key        mask;
    struct Ipnet_ipv4_key        gw;
    Ipnet_route_entry           *re;

    if (ort->rt_dst.sa_family != IP_AF_INET)
        return -IP_ERRNO_EINVAL;

    key.addr.s_addr = ((struct Ip_sockaddr_in *) &ort->rt_dst)->sin_addr.s_addr;

    if (request == IP_SIOCDELRT)
        return ipnet_route_delete2(IP_AF_INET,
                                   sock->vr_index,
                                   IPCOM_ROUTE_TABLE_DEFAULT,
                                   &key,
                                   IP_NULL,
                                   IP_NULL,
                                   0,
                                   ipnet_get_effective_pid(),
                                   0,
                                   IP_FALSE);

    mask.addr.s_addr = IP_NETMASK(key.addr.s_addr);

    ipcom_memset(&param, 0, sizeof(param));
    param.domain = IP_AF_INET;
    param.table  = IPCOM_ROUTE_TABLE_DEFAULT;
    param.flags  = ort->rt_flags;
    param.vr     = sock->vr_index;

    if (IP_BIT_ISSET(param.flags, IPNET_RTF_GATEWAY))
    {
        if (ort->rt_gateway.sa_family != IP_AF_INET)
            return -IP_ERRNO_EINVAL;

        gw.addr.s_addr = ((struct Ip_sockaddr_in *) &ort->rt_gateway)->sin_addr.s_addr;
        param.gateway = &ort->rt_gateway;

        if (ipnet_route_raw_lookup(IP_AF_INET, sock->vr_index, IPCOM_ROUTE_TABLE_DEFAULT,
                                   IPNET_RTL_FLAG_DONTCLONE, &gw, 0, 0, &re) < 0)
            return -IP_ERRNO_ENETUNREACH;

        if (IP_BIT_ISSET(re->netif->ipcom.flags, IP_IFF_LOOPBACK))
            param.netif = ipnet_ioctl_netif_from_in_addr(sock, gw.addr.s_addr, 0xffffffff);
        else
            param.netif = re->netif;
    }
    else
    {
        /* Try to get the interface from the key */
        param.netif = ipnet_ioctl_netif_from_in_addr(sock, key.addr.s_addr, mask.addr.s_addr);

    }

    param.key = &key;
    if (IP_BIT_ISSET(param.flags, IPNET_RTF_HOST))
    {
        if (IP_BIT_ISFALSE(param.flags, IPNET_RTF_GATEWAY))
            /* Must be link local host since no gateway was specified */
            IP_BIT_SET(param.flags, IPNET_RTF_LLINFO);
    }
    else
    {
        param.netmask = &mask;
        if (IP_BIT_ISFALSE(param.flags, IPNET_RTF_GATEWAY))
            /* Must be link local net since no gateway was specified */
            IP_BIT_SET(param.flags, IPNET_RTF_CLONING);
    }

    param.no_ref_count = IP_TRUE;
    return ipnet_route_add(&param);
}


/*
 *===========================================================================
 *                    ipnet_ioctl_x_route
 *===========================================================================
 * Description: Adds or deletes route. Provides an API that is easily
 *              mappable to the one used in Linux.
 * Parameters:  request - IP_SIOCXADDRT or IP_SIOCXDELRT for add or delete a
 *              IPv4/Ipv6 route.
 * Returns:     0 = success, <0 = error
 *
 */
IP_STATIC int
ipnet_ioctl_x_route(Ipnet_socket *sock,
                    unsigned long request,
                    struct Ip_rtxmsg *rtmsg)
{
    Ip_sa_family_t               family = rtmsg->rt_dst.sa_family;
    struct Ipnet_route_add_param param;
    union Ipnet_route_key        key;
    union Ipnet_route_key        mask;

    ipcom_memset(&param, 0, sizeof(param));
    ipcom_memset(&key, 0, sizeof(key));
    ipcom_memset(&mask, 0, sizeof(mask));

    /* Determine address family */
    switch (family)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        {
            struct Ip_sockaddr_in *sin = (struct Ip_sockaddr_in*)&rtmsg->rt_dst;

            ipcom_memcpy(&key.ipv4.addr, &sin->sin_addr, sizeof(struct Ip_in_addr));
        }
        break;
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        {
            struct Ip_sockaddr_in6 *sin6 = (struct Ip_sockaddr_in6*)&rtmsg->rt_dst;

            ipcom_memcpy(&key.ipv6.addr, &sin6->sin6_addr, sizeof(struct Ip_in6_addr));

            if (IPCOM_IN6_IS_ADDR_SCOPED(&key.ipv6.addr))
                key.ipv6.scope_id = rtmsg->rt_ifindex;
        }
        break;
#endif
    default:
        return -IP_ERRNO_EINVAL;
    }

    ipnet_route_create_mask(&mask, rtmsg->rt_dst_prefixlen);

    param.domain       = family;
    param.vr           = sock->vr_index;
    param.netif        = ipnet_if_indextonetif(sock->vr_index,
                                               rtmsg->rt_ifindex);;
    param.key          = &key;
    param.netmask      = &mask;
    param.flags        = rtmsg->rt_flags;
    param.no_ref_count = IP_TRUE;

    if (IP_BIT_ISSET(param.flags, IPNET_RTF_GATEWAY))
        param.gateway = &rtmsg->rt_gateway;


    if (request == IP_SIOCXDELRT)
        return ipnet_route_delete2(param.domain,
                                   param.vr,
                                   IPCOM_ROUTE_TABLE_DEFAULT,
                                   param.key,
                                   param.netmask,
                                   param.gateway,
                                   0,
                                   ipnet_get_effective_pid(),
                                   0,
                                   IP_FALSE);

    if (IP_BIT_ISSET(param.flags, IPNET_RTF_HOST))
    {
        if (IP_BIT_ISFALSE(param.flags, IPNET_RTF_GATEWAY))
            /* Must be link local host since no gateway was specified */
            IP_BIT_SET(param.flags, IPNET_RTF_LLINFO);
    }
    else
    {
        if (IP_BIT_ISFALSE(param.flags, IPNET_RTF_GATEWAY))
            /* Must be link local net since no gateway was specified */
            IP_BIT_SET(param.flags, IPNET_RTF_CLONING);
    }

    return ipnet_route_add(&param);
}


/*
 *===========================================================================
 *                    ipnet_ioctl_ip4
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_ioctl_ip4(Ipnet_socket *sock, unsigned long request, struct Ip_ifreq *ifreq)
{
    Ipnet_netif           *netif;
    Ipnet_ip4_addr_entry  *addr;
    struct Ip_sockaddr_in *in_addr;
    struct Ip_ifaliasreq  *ifalias;
    Ip_u32                 ipaddr_n;
    Ip_u32                 netmask_n;
    Ip_u32                 subbrd_n;
    int                    ret      = 0;
    int                    flags    = 0;
    struct Ip_sockaddr_dl  dl;
    Ip_u32                 netaddr_n;
    struct Ipnet_route_add_param param;

#ifdef IPMIP4
    if (request == IP_SIOCMIP4)
        return ipmip4_ipnet_ioctl(sock->vr_index, request, (void *)ifreq);
#endif

    if (request == IP_SIOCADDRT || request == IP_SIOCDELRT)
        return ipnet_ioctl_ip4_bsd43_compat(sock, request, (struct Ip_ortentry *) ifreq);

    ip_assert(IP_IOCPARM_LEN(request) >= IP_IFNAMSIZ);

    netif = ipnet_if_nametonetif(sock->vr_index, ifreq->ifr_name);
    if (netif == IP_NULL)
        return -IP_ERRNO_ENXIO;

#ifdef IPPPP
    if (netif->ipcom.type == IP_IFT_PPP || netif->ipcom.type == IP_IFT_PPPOE)
        /* PPP special case. */
        return ipnet_if_link_ioctl(netif, request, ifreq);
#endif

    ifalias = (struct Ip_ifaliasreq *)ifreq;

    switch (request)
    {
    case IP_SIOCGIFADDR:
    case IP_SIOCSIFADDR:
    case IP_SIOCDIFADDR:
    case IP_SIOCGIFBRDADDR:
    case IP_SIOCSIFBRDADDR:
    case IP_SIOCGIFNETMASK:
    case IP_SIOCSIFNETMASK:
    case IP_SIOCGIFDSTADDR:
    case IP_SIOCSIFDSTADDR:
        in_addr = (struct Ip_sockaddr_in *) &ifreq->ifr_ifru.ifru_addr;
        break;

#ifdef IP_SIOCAHOMEADDR
    case IP_SIOCAHOMEADDR:
#endif
    case IP_SIOCAIFADDR:
        in_addr = (struct Ip_sockaddr_in *) &ifalias->ifra_addr;
        break;
    default:
        in_addr = IP_NULL;
        break;
    }

    if (in_addr != IP_NULL && IP_BIT_ISSET(IP_IOCF_W, request))
    {
#if defined(IPNET_COMPAT_OLDSOCK) && defined(IPCOM_USE_SA_LEN)
        if (in_addr->sin_family == 0 && in_addr->sin_len < IP_AF_MAX)
            ((struct Ip_sockaddr *)in_addr)->sa_family = in_addr->sin_len;
        ((struct Ip_sockaddr *)in_addr)->sa_len = sizeof(struct Ip_sockaddr_in);
#endif

        if (in_addr->sin_family != IP_AF_INET
            || IPCOM_SA_LEN_GET(in_addr) < sizeof(struct Ip_sockaddr_in))
            return -IP_ERRNO_EINVAL;
    }

    /* Get interface primary IPv4 address. */
    if (request != IP_SIOCDIFADDR)
    {
        addr = netif->inet4_addr_list;
        if (addr != IP_NULL && addr->type != IPNET_ADDR_TYPE_UNICAST)
            addr = IP_NULL;
    }
    else
    {
        addr = ipnet_ip4_get_addr_entry(in_addr->sin_addr.s_addr,
                                        sock->vr_index,
                                        netif);
        if (addr == IP_NULL)
            return -IP_ERRNO_EADDRNOTAVAIL;
    }

    if (addr != IP_NULL)
    {
        ipaddr_n  = addr->ipaddr_n;
        netmask_n = addr->netmask_n;
        subbrd_n  = (addr->ipaddr_n & addr->netmask_n) | ~addr->netmask_n;
    }
    else
    {
        ipaddr_n  = 0;
        netmask_n = 0;
        subbrd_n  = 0;
    }

    switch (request)
    {
    case IP_SIOCGIFADDR:
    case IP_SIOCGIFBRDADDR:
    case IP_SIOCGIFNETMASK:
    case IP_SIOCGIFDSTADDR:
        ipcom_memset(in_addr, 0, sizeof(struct Ip_sockaddr_in));
        in_addr->sin_family = IP_AF_INET;
        IPCOM_SA_LEN_SET(in_addr, sizeof(struct Ip_sockaddr_in));
        break;
#ifdef IP_SIOCAHOMEADDR
    case IP_SIOCAHOMEADDR:
#endif
    case IP_SIOCAIFADDR:
        netmask_n = ((struct Ip_sockaddr_in *)&ifalias->ifra_mask)->sin_addr.s_addr;
        /* Fall though */
    case IP_SIOCSIFADDR:
        ipaddr_n = in_addr->sin_addr.s_addr;
#ifndef IP_PORT_LKM
        if (ipaddr_n == IP_INADDR_ANY)
            return -IP_ERRNO_EINVAL;
#endif
        break;
    default:
        break;
    }

    switch (request)
    {
    case IP_SIOCGIFADDR:
        if (!ipaddr_n)
            return -IP_ERRNO_EADDRNOTAVAIL;
        in_addr->sin_addr.s_addr = ipaddr_n;
        break;
    case IP_SIOCGIFBRDADDR:
        in_addr->sin_addr.s_addr = subbrd_n;
        break;
    case IP_SIOCGIFNETMASK:
        in_addr->sin_addr.s_addr = netmask_n;
        break;
    case IP_SIOCSIFNETMASK:
        if (addr == IP_NULL)
            return -IP_ERRNO_EINVAL;
        ret = ipnet_ip4_change_addr_mask(addr, in_addr->sin_addr.s_addr);
        break;
    case IP_SIOCSIFADDR:
        netmask_n = IP_NETMASK(in_addr->sin_addr.s_addr);
        if (addr != IP_NULL)
        {
            /* Remove the previous primary address */
            ret = ipnet_ip4_remove_addr(netif, addr->ipaddr_n);
            if (ret < 0)
                return ret;
        }
#ifdef IP_PORT_LKM
        /*
         * Setting the address 0.0.0.0 is interpreted
         * as remove primary address on Linux.
         */
        if (in_addr->sin_addr.s_addr == 0)
            break;
#endif /*IP_PORT_LKM*/

        flags |= IPNET_IP4_ADDR_FLAG_PRIMARY;

        /* Fall through */
    case IP_SIOCAIFADDR:
#ifdef IP_SIOCAHOMEADDR
    case IP_SIOCAHOMEADDR:
        if (request == IP_SIOCAHOMEADDR)
            flags |= IPNET_IP4_ADDR_FLAG_HOMEADDRESS | IPNET_IP4_ADDR_FLAG_PREFERRED;
#endif

        ret = ipnet_ip4_add_addr2(netif,
                                  in_addr->sin_addr.s_addr,
                                  netmask_n,
                                  flags,
                                  IP_IN_CLASSD(in_addr->sin_addr.s_addr) ? IPNET_ADDR_TYPE_MULTICAST : IPNET_ADDR_TYPE_UNICAST);
        break;
    case IP_SIOCSIFBRDADDR:
        if (addr != IP_NULL && subbrd_n != in_addr->sin_addr.s_addr)
        {

        if (0 != (subbrd_n = ipnet_ip4_netif_get_subbrd(netif)))
            {
                /* Remove the old */
                netaddr_n = subbrd_n;
                (void) ipnet_route_delete2(IP_AF_INET,
                                           netif->vr_index,
                                           IPCOM_ROUTE_TABLE_DEFAULT,
                                           &netaddr_n,
                                           IP_NULL,
                                           IP_NULL,
                                           netif->ipcom.ifindex,
                                           0,
                                           0,
                                           IP_FALSE);
                ipnet_ip4_remove_addr(netif,subbrd_n);
            }
        /* Add the new broadcast address */
        ipnet_ip4_add_addr(netif,
                           in_addr->sin_addr.s_addr,
                           0,
                           IP_FALSE,
                           IPNET_ADDR_TYPE_NETBROADCAST);

        netmask_n = 0xffffffff;
        /* Add the new network interface route */

        IPCOM_SA_LEN_SET(&dl, sizeof(struct Ip_sockaddr_dl));
        dl.sdl_family = IP_AF_LINK;
        dl.sdl_index  = (Ip_u16)netif->ipcom.ifindex;
        dl.sdl_type   = (Ip_u8)netif->ipcom.type;

        ipcom_memset(&param, 0, sizeof(struct Ipnet_route_add_param));
        param.domain  = IP_AF_INET;
        param.vr      = netif->vr_index;
        param.table   = IPCOM_ROUTE_TABLE_DEFAULT;
        param.netif   = netif;
        netaddr_n     = in_addr->sin_addr.s_addr;
        param.gateway = (struct Ip_sockaddr *)&dl;
        param.key     = &netaddr_n;
        param.netmask = &netmask_n;
        param.flags   = IPNET_RTF_UP | IPNET_RTF_HOST | IPNET_RTF_DONE | IPNET_RTF_LLINFO | IPNET_RTF_PREF ;
        ret = ipnet_route_add(&param);

        }
        else
            return -IP_ERRNO_EINVAL;

        break;
    case IP_SIOCDIFADDR:
        ret = ipnet_ip4_remove_addr(netif, in_addr->sin_addr.s_addr);
        break;

    default:
        return ipnet_if_link_ioctl(netif, request, ifreq);
    }

    return ret;
}
#endif /* IPCOM_USE_INET */


/*
 *===========================================================================
 *                    ipnet_ioctl_ip6
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPCOM_USE_INET6
IP_STATIC int
ipnet_ioctl_ip6(Ipnet_socket *sock, unsigned long request, void *data)
{
    Ipnet_netif            *netif;
    struct Ip_in6_ifreq    *ifreq = (struct Ip_in6_ifreq *) data;
    struct Ip_in6_aliasreq *ifareq = (struct Ip_in6_aliasreq *) data;
    Ip_u32                  preferred_lifetime = IPCOM_ADDR_INFINITE;
    Ip_u32                  valid_lifetime = IPCOM_ADDR_INFINITE;
    Ip_u32                  flags = 0;

    ip_assert(IP_IOCPARM_LEN(request) >= IP_IFNAMSIZ);

    netif = ipnet_if_nametonetif(sock->vr_index, ifreq->ifr_name);
    if (netif == IP_NULL)
        return -IP_ERRNO_ENXIO;

#ifdef IPPPP
    if (netif->ipcom.type == IP_IFT_PPP || netif->ipcom.type == IP_IFT_PPPOE)
        /* PPP special case. */
        return ipnet_if_link_ioctl(netif, request, ifreq);
#endif

    /* Sanity check */
    switch (request)
    {
    case IP_SIOCAIFADDR_IN6:
    case IP_SIOCXGIFADDR_IN6:
        if (ifareq->ifra_addr.sin6_family != IP_AF_INET6
            || IPCOM_SA_LEN_GET(&ifareq->ifra_addr) < sizeof(struct Ip_sockaddr_in6))
            return -IP_ERRNO_EINVAL;
        break;
    case IP_SIOCDIFADDR_IN6:
    case IP_SIOCGIFPREFIX_IN6:
        if (ifreq->ifr_ifru.ifru_addr.sin6_family != IP_AF_INET6
            || IPCOM_SA_LEN_GET(&ifreq->ifr_ifru.ifru_addr) < sizeof(struct Ip_sockaddr_in6))
            return -IP_ERRNO_EINVAL;
        break;
    case IP_SIOCGIFDSTADDR_IN6:
        if (!IP_BIT_ISSET(netif->ipcom.flags, IP_IFF_POINTOPOINT))
            return -IP_ERRNO_EINVAL;
        break;
    default:
        return -IP_ERRNO_EINVAL;
    }

    switch (request)
    {
    case IP_SIOCAIFADDR_IN6:
        {
            Ip_u32 ifindex = netif->ipcom.ifindex;
            int ret = 0;

            if (ifareq->ifra_lifetime.ia6t_preferred != (Ip_time_t)IPCOM_ADDR_INFINITE)
                preferred_lifetime = ifareq->ifra_lifetime.ia6t_preferred - ipcom_time(IP_NULL);
            if (ifareq->ifra_lifetime.ia6t_expire != (Ip_time_t)IPCOM_ADDR_INFINITE)
                valid_lifetime = ifareq->ifra_lifetime.ia6t_expire - ipcom_time(IP_NULL);

            if (IP_BIT_ISTRUE(ifareq->ifra_flags, IP_IN6_IFF_TEMPORARY))
                flags |= IPNET_IP6_ADDR_FLAG_TEMPORARY;
            if (IP_BIT_ISTRUE(ifareq->ifra_flags, IP_IN6_IFF_TENTATIVE))
                flags |= IPNET_IP6_ADDR_FLAG_TENTATIVE;
            if (IP_BIT_ISTRUE(ifareq->ifra_flags, IP_IN6_IFF_HOMEADDRESS))
                flags |= IPNET_IP6_ADDR_FLAG_HOMEADDRESS;
            if (IP_BIT_ISTRUE(ifareq->ifra_flags, IP_IN6_IFF_AUTONOMOUS))
                flags |= IPNET_IP6_ADDR_FLAG_AUTONOMOUS;
            if (IP_BIT_ISTRUE(ifareq->ifra_flags, IP_IN6_IFF_ANYCAST))
                flags |= IPNET_IP6_ADDR_FLAG_ANYCAST;
#ifdef IPNET_USE_RFC3971
            if (IP_BIT_ISTRUE(ifareq->ifra_flags, IP_IN6_IFF_CGA))
                flags |= IPNET_IP6_ADDR_FLAG_CGA;
#endif
            ret = ipnet_ip6_add_addr(netif,
                                     &ifareq->ifra_addr.sin6_addr,
                                     flags,
                                     preferred_lifetime, valid_lifetime,
                                     ipcom_sockaddr_to_prefixlen((struct Ip_sockaddr *) &ifareq->ifra_prefixmask));
            netif = ipnet_if_indextonetif(IPCOM_VR_ANY, ifindex);

            /* Check if peer address specified */
            if (ret == 0
                && netif != IP_NULL
                && ifareq->ifra_dstaddr.sin6_family == IP_AF_INET6
                && IPCOM_SA_LEN_GET(&ifareq->ifra_dstaddr) == sizeof(struct Ip_sockaddr_in6))
            {
                Ipnet_ppp_peer *peer = netif->private_data;

                /* Check if interface supports it */
                if (peer != IP_NULL)
                    ret = ipnet_if_set_ipv6_ppp_peer(peer, &ifareq->ifra_dstaddr.sin6_addr);
                else
                    ret = -IP_ERRNO_EINVAL;

                if (ret != 0)
                    (void)ipnet_ip6_remove_addr(netif, &ifareq->ifra_addr.sin6_addr);
            }

            return ret;
        }
    case IP_SIOCDIFADDR_IN6:
        return ipnet_ip6_remove_addr(netif, &ifreq->ifr_ifru.ifru_addr.sin6_addr);
    case IP_SIOCGIFDSTADDR_IN6:
        return ipnet_if_link_ioctl(netif, IP_SIOCGIFDSTADDR_IN6, &ifreq->ifr_ifru.ifru_addr.sin6_addr);
    case IP_SIOCXGIFADDR_IN6:
        {
            Ipnet_ip6_addr_entry *address = (Ipnet_ip6_addr_entry *)netif->inet6_addr_list;
            while (address != IP_NULL)
            {
                if (IP_IN6_ARE_ADDR_EQUAL(&address->addr, &ifareq->ifra_addr.sin6_addr))
                {
                    ifareq->ifra_flags                      = 0;
                    if (IP_BIT_ISSET(address->flags, IPNET_IP6_ADDR_FLAG_TEMPORARY))
                        ifareq->ifra_flags |= IP_IN6_IFF_TEMPORARY;
                    if (IP_BIT_ISSET(address->flags, IPNET_IP6_ADDR_FLAG_TENTATIVE))
                        ifareq->ifra_flags |= IP_IN6_IFF_TENTATIVE;
                    if (IP_BIT_ISSET(address->flags, IPNET_IP6_ADDR_FLAG_HOMEADDRESS))
                        ifareq->ifra_flags |= IP_IN6_IFF_HOMEADDRESS;
                    if (IP_BIT_ISSET(address->flags, IPNET_IP6_ADDR_FLAG_AUTONOMOUS))
                        ifareq->ifra_flags |= IP_IN6_IFF_AUTONOMOUS;
                    if (IP_BIT_ISSET(address->flags, IPNET_IP6_ADDR_FLAG_ANYCAST))
                        ifareq->ifra_flags |= IP_IN6_IFF_ANYCAST;
                    if (IP_BIT_ISSET(address->flags, IPNET_IP6_ADDR_FLAG_DEPRECATED))
                        ifareq->ifra_flags |= IP_IN6_IFF_DEPRECATED;
                    if (IP_BIT_ISSET(address->flags, IPNET_IP6_ADDR_FLAG_AUTOMATIC))
                        ifareq->ifra_flags |= IP_IN6_IFF_AUTOMATIC;
#ifdef IPNET_USE_RFC3971
                    if (IP_BIT_ISSET(address->flags, IPNET_IP6_ADDR_FLAG_CGA))
                        ifareq->ifra_flags |= IP_IN6_IFF_CGA;
#endif
                    ifareq->ifra_lifetime.ia6t_expire       = address->expires_sec;
                    ifareq->ifra_lifetime.ia6t_preferred    = address->preferred_sec;
                    ifareq->ifra_lifetime.ia6t_pltime       = 0;
                    ifareq->ifra_lifetime.ia6t_vltime       = 0;
                    ipcom_memset(&ifareq->ifra_prefixmask, 0, sizeof(ifareq->ifra_prefixmask));
                    ipnet_route_create_mask(&ifareq->ifra_prefixmask.sin6_addr, address->prefixlen);
                    return 0;
               }

                /* traverses the next address list */
                address = (Ipnet_ip6_addr_entry *)address->next;
            }

            return -IP_ERRNO_EINVAL;
        }
    case IP_SIOCGIFPREFIX_IN6:
        {
            Ipnet_ip6_addr_entry *address;

            address = (Ipnet_ip6_addr_entry *)netif->inet6_addr_list;
            while (address != IP_NULL) {
                if (IP_IN6_ARE_ADDR_EQUAL(&address->addr, &ifreq->ifr_ifru.ifru_addr.sin6_addr)) {
		    ifreq->ifr_ifru.ifru_prefixlen = address->prefixlen;
                    return(address->prefixlen);
                }
                /* traverses the next address list */
                address = (Ipnet_ip6_addr_entry *)address->next;
            }
            return -IP_ERRNO_EINVAL;
        }
    default:
        IP_PANIC();
        return -IP_ERRNO_EINVAL;
    }
}
#endif /* IPCOM_USE_INET6 */


/*
 ****************************************************************************
 * 10                    GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_ioctl_move_if_rt_to_vr
 *===========================================================================
 * Description: Moves all routes that are using the interface to the
 *              new routing table.
 * Parameters:  netif - The network interface that change route table.
 *              vr - New virtual router index.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_GLOBAL int
ipnet_ioctl_move_if_rt_to_vr(Ipnet_netif *netif, Ip_u16 vr)
{
    Ipnet_route_entry           *rt;
    struct Ipnet_route_add_param rt_add_param;
    int                          n;
    Ip_size_t                    d;
    int                          domains[] = {
#ifdef IPCOM_USE_INET
        IP_AF_INET,
#endif
#ifdef IPCOM_USE_INET6
        IP_AF_INET6
#endif /* IPCOM_USE_INET */
    };

    if (vr == netif->vr_index)
        /* The interface is already using the specified route table */
        return 0;

    if (ipnet_route_is_virtual_router_valid(vr) == IP_FALSE)
    {
        IPCOM_LOG2(WARNING, "%s cannot be assigned to virtual router %u since it does not exist",
                   netif->ipcom.name, vr);
        return -IP_ERRNO_ENXIO;
    }

#ifdef IP_PORT_LKM
    /* Don't allow multiple interfaces with identical names in the same VR */
    if (ipnet_if_nametonetif(vr, netif->ipcom.name) != IP_NULL)
        return -IP_ERRNO_EPERM;
#endif /* IP_PORT_LKM */

    /* Copy all routes that uses this interface to the new route table
     * Note: Two passes necessary because cloning routes need to be added first!
     */
    for (d = 0; d < sizeof(domains) / sizeof(domains[0]); d++)
    {
        for (n = 0; n < 2; n++)
        {
            for (rt = ipnet_route_first_entry(domains[d], netif->vr_index, IPCOM_ROUTE_TABLE_DEFAULT);
                 rt != IP_NULL;
                 rt = ipnet_route_next_entry(rt))
            {
                int ret;

                if (n == 0 && IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_CLONING))
                    continue;

                if (n == 1 && IP_BIT_ISTRUE(rt->hdr.flags, IPNET_RTF_CLONING))
                    continue;

                if (ipnet_ioctl_if_is_rt_affected(netif, rt) == IP_FALSE)
                    continue;

                if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_X_MCAST_RO | IPNET_RTF_X_BCAST_RO))
                {
                    rt->netif = IP_NULL;
                    IP_BIT_CLR(rt->hdr.flags, IPNET_RTF_UP);
                    continue;
                }

                ipcom_memset(&rt_add_param, 0, sizeof(rt_add_param));
                rt_add_param.domain     = domains[d];
                rt_add_param.flags      = rt->hdr.flags;
                rt_add_param.gateway    = rt->gateway;
                rt_add_param.key        = rt->hdr.key;
                rt_add_param.metrics    = &rt->metrics;
                if (IP_BIT_ISSET(rt->netif->ipcom.flags, IP_IFF_LOOPBACK))
                    rt_add_param.netif  = ipnet_loopback_get_netif(vr);
                else
                    rt_add_param.netif  = rt->netif;
                rt_add_param.netmask    = rt->hdr.mask;
                rt_add_param.pid        = rt->pid;
                rt_add_param.seq        = rt->seq;
                rt_add_param.vr         = vr;
                rt_add_param.table      = IPCOM_ROUTE_TABLE_DEFAULT;
                ret = ipnet_route_add(&rt_add_param);
                ip_assert2(ret == 0 || ret == -IP_ERRNO_EEXIST);
                (void)ret;
            }
        }
    }

    /* Remove all route that are using this interface from the old route table */
    ipnet_ioctl_if_remove_routes(netif, netif->vr_index);

#ifdef IPCOM_USE_INET
    {
        Ipnet_ip4_addr_entry *addr;
        for (addr = netif->inet4_addr_list; addr != IP_NULL; addr = addr->next)
            ipnet_ip4_remove_addr_cache(addr);
    }
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    {
        Ipnet_ip6_addr_entry *addr;
        for (addr = netif->inet6_addr_list; addr != IP_NULL; addr = addr->next)
            ipnet_ip6_remove_addr_cache(addr);
    }
#endif /* IPCOM_USE_INET */

#ifdef IPNET_USE_FAST_IFNAME_LOOKUP
    /* Remove from interface name hash table */
    (void)ipcom_hash_remove(ipnet->ifname_tab,netif);
#endif /* IPNET_USE_IFNAME_LOOKUP */

    /* Change the route tab */
    netif->vr_index = vr;

#ifdef IPCOM_USE_INET
    {
        Ipnet_ip4_addr_entry *addr;
        for (addr = netif->inet4_addr_list; addr != IP_NULL; addr = addr->next)
            if (ipnet_ip4_insert_addr_cache(addr))
                IP_PANIC();
    }
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    {
        Ipnet_ip6_addr_entry *addr;
        for (addr = netif->inet6_addr_list; addr != IP_NULL; addr = addr->next)
            if (ipnet_ip6_insert_addr_cache(addr))
                IP_PANIC();
    }
#endif /* IPCOM_USE_INET */

#ifdef IPNET_USE_FAST_IFNAME_LOOKUP
    /* Update interface name hash table with new VR index */
    (void)ipcom_hash_add(ipnet->ifname_tab, netif);
#endif /* IPNET_USE_FAST_IFNAME_LOOKUP */

#ifdef IP_PORT_LKM
    ipcom_netif_assign_vr(&netif->ipcom, netif->vr_index);
#endif /* IP_PORT_LKM */

    return 0;
}


/*
 ****************************************************************************
 * 11                    PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_sys_socketioctl
 *===========================================================================
 * Description: IO control.
 * Parameters:  fd - A socket descriptor.
 *              request - The IO control request.
 *              argp - Request specific data.
 * Returns:     0 = success, -1 = error (error code from ipcom_errno).
 *
 */
IP_PUBLIC int
ipnet_sys_socketioctl(Ip_fd fd, unsigned long request, void *argp)
{
    Ipnet_socket *sock;
    int           i;
    /* This union is only used to avoid the gcc warning
       "dereferencing type-punned pointer will break strict-aliasing rules" */
    union {
        Ipcom_if_eventcb  cb;
        void             *ptr;
    } event_u;

    IPNET_FD_TO_OPEN_SOCKET(fd, sock);

    if (sock->ops->ioctl != IP_NULL)
    {
        i = sock->ops->ioctl(sock, request, argp);
        if (i == 0 || i != -IP_ERRNO_EOPNOTSUPP)
            return i;
        /* else: try the IPNET ioctl handlers */
    }

    switch (IP_IOC_GROUP(request))
    {
    case IP_IOCG_SOCK:
        return ipnet_ioctl_sock(sock, request, argp);

    case IP_IOCG_RTAB:
        return ipnet_ioctl_rtab(request, argp);

    case IP_IOCG_POLICY_RT:
        return ipnet_route_policy_ioctl(ipnet_get_effective_pid(),
                                        sock->vr_index,
                                        request,
                                        (struct Ipnet_policy_routing_rule *) argp);

    case IP_IOCG_NETIF:
    case IP_IOCG_ETH:
#ifdef IPPPP
    case IP_IOCG_PPP:
#endif
#ifdef IPNET_USE_WLAN
    case IP_IOCG_WLAN:
#endif
        return ipnet_ioctl_if(sock, request, (struct Ip_ifreq *) argp);

#ifdef IPCOM_USE_INET
    case IP_IOCG_INET:
        return ipnet_ioctl_ip4(sock, request, (struct Ip_ifreq *) argp);

#ifdef IPNET_USE_MCAST_ROUTING
    case IP_IOCG_MCAST:
        return ipnet_ip4_mcast_ioctl(sock, request, argp);
#endif

#ifdef IPCOM_USE_ETHERNET
    case IP_IOCG_ARP:
        return ipnet_arp_ioctl(sock, request, (struct Ip_arpreq *) argp);
#endif
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    case IP_IOCG_INET6:
        return ipnet_ioctl_ip6(sock, request, argp);

#ifdef IPNET_USE_MCAST_ROUTING
    case IP_IOCG_MCAST_IN6:
        return ipnet_ip6_mcast_ioctl(sock, request, argp);
#endif
#endif /* IPCOM_USE_INET6 */

#ifdef IPMPLS
    case IP_IOCG_MPLS:
        return ipmpls_ioctl(request, argp);
#endif

#ifdef IPNET_USE_DIFFSERV
    case IP_IOCG_DS:
        return ipnet_diffserv_ioctl(request, argp);
#endif
    default:
        break;
    }

    switch (request)
    {
#ifdef IPCOM_USE_INET6
    case IP_SIOCXGETGW_IN6:
        /* Get default IPv6 gateway. */
        return -IP_ERRNO_ENOSUPPORT;

    case IP_SIOCXSETGW_IN6:
        {
            /* Set default IPv6 gateway (including its optional scope id). */
            struct Ipnet_route_add_param param;
            struct Ipnet_ipv6_key        rt6;
            struct Ip_sockaddr_in6       gw6;
            Ipnet_route_entry           *rt;

            ipcom_memset(&param, 0, sizeof(struct Ipnet_route_add_param));
            ipcom_memset(&rt6, 0, sizeof(struct Ipnet_ipv6_key));
            ipcom_memset(&gw6, 0, sizeof(gw6));
            param.domain  = IP_AF_INET6;
            param.flags   = IPNET_RTF_UP | IPNET_RTF_GATEWAY | IPNET_RTF_DONE | IPNET_RTF_STATIC;
            /* Key == :: */
            param.key     = &rt6;
            /* Netmask == /0 */
            param.netmask = &rt6;
            /* Gateway */
            param.gateway = (struct Ip_sockaddr *)&gw6;
            IPCOM_SA_LEN_SET(&gw6, sizeof(struct Ip_sockaddr_in6));
            gw6.sin6_family = IP_AF_INET6;
            ipcom_memcpy(&gw6.sin6_addr, &((struct Ip_sockaddr_in6 *)argp)->sin6_addr, sizeof(struct Ip_in6_addr));
            gw6.sin6_scope_id = ((struct Ip_sockaddr_in6 *)argp)->sin6_scope_id;
            if (ipnet_route_raw_lookup(IP_AF_INET6, sock->vr_index,
                                       IPCOM_ROUTE_TABLE_DEFAULT, IPNET_RTL_FLAG_DONTCLONE,
                                       &gw6.sin6_addr, gw6.sin6_scope_id, 0, &rt) < 0)
                return -IP_ERRNO_ENETUNREACH;
            ip_assert(rt);
            ip_assert(rt->netif);
            param.netif = rt->netif;
            param.vr    = sock->vr_index;
            param.table = IPCOM_ROUTE_TABLE_DEFAULT;
            return ipnet_route_add(&param);
        }
#endif /* IPCOM_USE_INET6 */

    case IP_SIOCXSIFFEVENTCB:
        /* Set interface event callback. */
        for (i = 0; i < IPNET_MAX_IF_EVENTCB; i++)
            if (ipnet->if_eventcb[i] == IP_NULL)
            {
                event_u.ptr = argp;
                ipnet->if_eventcb[i] = event_u.cb;
                return 0;
            }
        return -IP_ERRNO_EINVAL;

    case IP_SIOCXDIFFEVENTCB:
        /* Delete interface event callback. */
        event_u.ptr = argp;
        for (i = 0; i < IPNET_MAX_IF_EVENTCB; i++)
            if (ipnet->if_eventcb[i] == event_u.cb)
            {
                ipnet->if_eventcb[i] = IP_NULL;
                return 0;
            }
        return -IP_ERRNO_ENOENT;

#ifdef IPIPSEC2
    case IP_SIOCXIPSEC_SELECTOR:
    case IP_SIOCXIPSEC_NEXT_SA:
    case IP_SIOCXIPSEC_Grouped:
    case IP_SIOCXIPSEC_InUserOctets:
    case IP_SIOCXIPSEC_OutUserOctets:
    case IP_SIOCXIPSEC_InUserPackets:
    case IP_SIOCXIPSEC_OutUserPackets:
        return ipipsec_sys_ioctl(fd, request, argp);
#ifdef IPIPSEC_USE_CTRL_API
    case IP_SIOCXIPSEC_CTL:
        return ipipsec_ctrl_internal(((Ip_tag **)argp)[0]);
    case IP_SIOCXIPSEC_SA_CTL:
        return ipipsec_sactrl_internal(((void **)argp)[0], ((void **)argp)[1]);
    case IP_SIOCXIPSEC_CONF_CTL:
        return ipipsec_conf_internal(((void **)argp)[0]);
#endif
#endif /* IPIPSEC2 */

#ifdef IPCOM_USE_INET
    case IP_SIOCXADDRT:
    case IP_SIOCXDELRT:
        return ipnet_ioctl_x_route(sock, request, argp);
#endif /* IPCOM_USE_INET */

#ifdef IPROHC
    case IP_SIOCSROHCSTATUS:
    case IP_SIOCROHCADDIF:
    case IP_SIOCROHCDELIF:
    case IP_SIOCROHCADDIFPROFILE:
    case IP_SIOCROHCDELIFPROFILE:
    case IP_SIOCSROHCCID:
	return iprohc_ioctl(request, (struct Ip_rohcreq *) argp);
#endif /* IPROHC */

    default:
        break;
    }

    return -IP_ERRNO_EINVAL;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

