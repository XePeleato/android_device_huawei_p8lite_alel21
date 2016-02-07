/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_rtnetlink_link.c,v $ $Revision: 1.11 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_rtnetlink_link.c,v $
 *   $Author: rime $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation Jonas Rendel <Jonas.Rendel@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Contains netlink handlers for managing interfaces
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

#ifdef IPNET_USE_NETLINK
#define IPCOM_USE_CLIB_PROTO
#include <ipcom.h>
#include <ipcom_type.h>
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_sock.h>
#include <ipcom_sock6.h>
#include <ipcom_sock2.h>
#include <ipcom_route.h>
#include <ipcom_errno.h>

#include "ipnet_h.h"
#include "ipnet_ctrl.h"
#include "ipnet_route.h"
#include "ipnet_netlink_h.h"
#include "ipnet_rtnetlink_h.h"
#include "ipcom_sock.h"

#ifdef IP_PORT_LKM
#include "ipcom_lkm.h" /* Needed for Linux PPP interface config */
#endif /* IP_PORT_LKM */

#ifdef IPMIP
#include <private/ipmip_netlink_kern.h>
#endif

#include "ipnet_eth.h"

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
IP_STATIC int
ipnet_rtnetlink_link_dump_all(Ipnet_netlink_mem_t *mem,
                              struct Ip_nlmsghdr  *nlmsg,
                              int                 family,
                              Ip_ptrdiff_t        *args)
{
    Ip_ptrdiff_t    *done    = &args[0];
    Ip_ptrdiff_t    *cfamily = &args[1];
    int             i;

    (void)family;

    for (i = *cfamily; i < IP_AF_MAX; i++)
    {
        Ipnet_rtnetlink_table_t **tbl;

        if (i == IP_AF_PACKET || i == IP_AF_UNSPEC)
            continue;

        if ((tbl = ipnet->rtnetlink_links[i]) != IP_NULL)
        {
            /* Continue until termination */
            if (tbl[nlmsg->nlmsg_type - IP_RTM_BASE] && tbl[nlmsg->nlmsg_type - IP_RTM_BASE]->nl_func_table.nl_dump)
            {
                (*tbl[nlmsg->nlmsg_type - IP_RTM_BASE]->nl_func_table.nl_dump) (mem, nlmsg, i, &args[2]);

                /* Done pointer */
                if (!args[2])
                    goto exit_out;
            }
        }

        /* Clear the args area */
        ipcom_memset(&args[2], 0, sizeof(Ip_ptrdiff_t) * 6);
    }

    /* All done */
    *done = 1;

exit_out:
    *cfamily = i;
    return mem->used;
}


#ifdef IPNET_USE_NETLINKSOCK
/*
 *==========================================================================
 *                    ipnet_rtnetlink_link_sz_fillifinfo
 *===========================================================================
 * Description: Get interface info via NETLINK
 * Parameters:  data - Message payload.
 *              len  - Length of message.
 *              cb   - NETLINK callback entry
 *
 * Returns:     0 - Success
 *             <0 - Failure
 *
 */
IP_STATIC int
ipnet_rtnetlink_link_sz_fillifinfo(Ipnet_netlink_mem_t *mem,
                                   int                 event,
                                   int                 eflags,
                                   Ip_u32              seq,
                                   Ip_u32              change,
                                   Ipnet_netif         *netif)
{
    int     ifname_len = ipcom_strlen(netif->ipcom.name);
    struct Ip_ifinfomsg  *ifinfo;
    Ip_u32               tmp32;
    struct Ip_nlmsghdr  *nlh;

    nlh = ipnet_netlink_nlmsg_sz_begin(mem, seq, event, eflags);
    if (!nlh)
        return -1;

    /* Setup the IFINFO header */
    ifinfo = ipnet_netlink_sz_put(mem, IP_NULL, sizeof(*ifinfo));
    if (!ifinfo)
        return -1;

    ifinfo->ifi_family = IP_AF_UNSPEC;
    ifinfo->ifi_type   = (Ip_u16)IPCOM_ARPHRD_TYPE(&netif->ipcom);
    ifinfo->ifi_index  = netif->ipcom.ifindex;
    ifinfo->ifi_flags  = netif->ipcom.flags; /* Shouldn't this be mapped? */
    ifinfo->ifi_change = change;

    if (netif->ipcom.link_addr_size)
        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_IFLA_ADDRESS, netif->ipcom.link_addr_size, netif->ipcom.link_addr))
            return -1;

    /* Ethernet specific 'patch' */
    if (ifinfo->ifi_type == IP_IFT_ETHER)
    {
        Ip_u16 bcast[3];
        IPNET_SET_ETHADDR_BROADCAST(bcast);
        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_IFLA_BROADCAST, sizeof(bcast), bcast))
            return -1;
    }

    if (!ipnet_rtnetlink_rta_sz_put(mem, IP_IFLA_IFNAME, ifname_len + 1, netif->ipcom.name))
        return -1;
    tmp32 = netif->ipcom.mtu;
    if (!ipnet_rtnetlink_rta_sz_put(mem, IP_IFLA_MTU, sizeof(tmp32), &tmp32))
        return -1;

    tmp32 = netif->vr_index;
    if (!ipnet_rtnetlink_rta_sz_put(mem, IP_IFLA_VR, sizeof(tmp32), &tmp32))
        return -1;

    /* Do the statistics */
    {
        struct Ip_rtattr          *rtas;
        struct Ip_rtnl_link_stats *devstats;
        rtas = ipnet_rtnetlink_rta_sz_put(mem, IP_IFLA_STATS, sizeof(*devstats), IP_NULL);
        if (!rtas)
            return -1;

        ipnet_rtnetlink_link_stats_get(netif, (struct Ip_rtnl_link_stats *)IP_RTA_DATA(rtas));
    }

    if (netif->rtnetlink_ops != IP_NULL)
    {
        int                 ret;
        struct Ip_rtattr    *rta;

        if (netif->rtnetlink_ops->fill_link)
        {
            ret = (*netif->rtnetlink_ops->fill_link) (mem, netif);
            if (ret < 0)
                return -1;
        }

        rta = ipnet_rtnetlink_rta_sz_nested_begin(mem, IP_IFLA_LINKINFO);

        if (!rta)
            return -1;

        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_IFLA_INFO_KIND, ipcom_strlen(netif->rtnetlink_ops->kind) + 1, netif->rtnetlink_ops->kind))
            return -1;

        if (netif->rtnetlink_ops->fill_info != IP_NULL)
        {
            struct Ip_rtattr    *info = ipnet_rtnetlink_rta_sz_nested_begin(mem, IP_IFLA_INFO_DATA);
            if (!info)
                return -1;

            ret = (*netif->rtnetlink_ops->fill_info) (mem, netif);
            if (ret < 0)
                return -1;

            ipnet_rtnetlink_rta_sz_nested_end(mem, info);
        }

        ipnet_rtnetlink_rta_sz_nested_end(mem, rta);
    }

    return ipnet_netlink_nlmsg_sz_end(mem, nlh);
}

/*
 *==========================================================================
 *                    ipnet_rtnetlink_link_dump
 *===========================================================================
 * Description: Get interface info via NETLINK
 * Parameters:  data - Message payload.
 *              len  - Length of message.
 *              cb   - NETLINK callback entry
 *
 * Returns:     0 - Success
 *             <0 - Failure
 *
 */
IP_STATIC int
ipnet_rtnetlink_link_dump_family(Ipnet_netlink_mem_t *mem,
                                 struct Ip_nlmsghdr  *nlmsg,
                                 int                 family,
                                 Ip_ptrdiff_t        *args)
{
    Ip_ptrdiff_t            *done   = &args[0];
    Ip_ptrdiff_t            *ifx    = &args[1];
    unsigned int            i;
    Ipnet_netif             *netif;
    unsigned int            sindex = *ifx;
    Ip_u16                  vr  = IPCOM_VR_ANY;

    (void)family;

    if (*done)
        goto exit_out;

    if (IP_BIT_ISFALSE(nlmsg->nlmsg_flags, IP_NLM_F_VR_UNSPEC))
        vr = mem->vr;

    IPNET_NETIF_FOR_EACH_ON_VR(netif, vr, i)
    {
        int ret;

        if (sindex != 0 && sindex != netif->ipcom.ifindex)
            continue;

        sindex  = 0;
        *ifx    = netif->ipcom.ifindex;

        /* */
        IPNET_IF_LOCK(netif);
        ret = ipnet_rtnetlink_link_sz_fillifinfo(mem,
                                                 IP_RTM_NEWLINK,
                                                 IP_NLM_F_MULTI,
                                                 nlmsg->nlmsg_seq,
                                                 0xFFFFFFFF,
                                                 netif);
        IPNET_IF_UNLOCK(netif);

        if (ret < 0)
            goto exit_out;
    }

    /* We're done */
    *done = 1;

exit_out:
    return mem->used;
}


/*
 *===========================================================================
 *                    ipnet_rtnetlink_link_setlink
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_rtnetlink_link_setlink_family(Ipnet_netlink_mem_t *mem,
                                    struct Ip_nlmsghdr  *nlmsg,
                                    struct Ip_rtattr    **rta,
                                    int                 family)
{
    /**/
    struct Ip_ifinfomsg     *ifm;
    char                    *ifname = IP_NULL;
    Ipnet_netif             *netif  = IP_NULL;
    struct Ip_ifreq         ifreq;
    int                     ret     = 0;
    Ip_u16                  vr;

    (void) family;

    /* Copy or zero */
    ifname = ipnet_netlink_strdup((struct Ip_nlattr *)rta[IP_IFLA_IFNAME - 1]);

    ifm = IP_NLMSG_DATA(nlmsg);

    vr = ipnet_rtnetlink_vr(rta[IP_IFLA_VR - 1], mem->vr);

    if (ifm->ifi_index > 0)
        netif = ipnet_if_indextonetif(vr, ifm->ifi_index);
    else if (ifname)
        netif = ipnet_if_nametonetif(vr, ifname);

    /* Didnt find the device? */
    if (netif == IP_NULL)
    {
        ret = -IP_ERRNO_ENODEV;
        goto done;
    }

    ipcom_strcpy(ifreq.ifr_name, netif->ipcom.name);

    /* Device address */
    if (rta[IP_IFLA_ADDRESS - 1])
    {
        if (IP_RTA_PAYLOAD(rta[IP_IFLA_ADDRESS - 1]) != netif->ipcom.link_addr_size)
        {
            ret = -IP_ERRNO_EINVAL;
            goto done;
        }

        ipcom_memcpy(ifreq.ifr_ifru.ifru_addr.sa_data,
                     IP_RTA_DATA(rta[IP_IFLA_ADDRESS - 1]),
                     IP_RTA_PAYLOAD(rta[IP_IFLA_ADDRESS - 1]));

        ret = ipnet_if_link_ioctl(netif, IP_SIOCSIFLLADDR, &ifreq);
        if (ret < 0)
            goto done;
    }

    if (rta[IP_IFLA_MTU - 1])
        netif->ipcom.mtu = (Ip_u32) IP_GET_32ON8(IP_RTA_DATA(rta[IP_IFLA_MTU - 1]));

    if (ifm->ifi_flags || ifm->ifi_change)
    {
        ret = ipnet_netif_change_flags(&netif, ifm->ifi_flags, ifm->ifi_change);
        if (!netif)
            goto done;
    }

done:
    if (ifname)
        ipcom_free(ifname);
    return ret;
}

/*
 *===========================================================================
 *                    ipnet_rtnetlink_link_setlink
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_rtnetlink_link_newlink_family(Ipnet_netlink_mem_t *mem,
                                    struct Ip_nlmsghdr  *nlmsg,
                                    struct Ip_rtattr    **rta,
                                    int                 family)
{
    /**/
    struct Ip_ifinfomsg     *ifm;
    char                    *ifname     = IP_NULL;
    Ipnet_netif             *netif      = IP_NULL;
    struct Ip_rtattr        **linkinfo  = IP_NULL;
    struct Ip_rtattr        **linkdata  = IP_NULL;
    int                     ret         = 0;
    struct Ip_rtnl_link_ops *ops        = IP_NULL;
    Ipnet_netif             *nif        = IP_NULL;
    Ip_u16                  vr;

    /* Copy or zero */
    ifname = ipnet_netlink_strdup((struct Ip_nlattr *)rta[IP_IFLA_IFNAME - 1]);

    ifm = IP_NLMSG_DATA(nlmsg);

    vr = ipnet_rtnetlink_vr(rta[IP_IFLA_VR - 1], mem->vr);

    if (ifm->ifi_index > 0)
        netif = ipnet_if_indextonetif(vr, ifm->ifi_index);
    else if (ifname)
        netif = ipnet_if_nametonetif(vr, ifname);

    /* Got a LINKINFO? */
    if (rta[IP_IFLA_LINKINFO - 1] != IP_NULL)
    {
        char *kind;
        /* Need the 'KIND*' */
        ret = ipnet_rtnetlink_parse(IP_IFLA_INFO_MAX, IP_RTA_DATA(rta[IP_IFLA_LINKINFO - 1]), IP_RTA_PAYLOAD(rta[IP_IFLA_LINKINFO - 1]), &linkinfo);
        if (ret < 0)
            goto done;

        /* Got the linkinfo */
        if ((kind = ipnet_netlink_strdup((struct Ip_nlattr *)linkinfo[IP_IFLA_INFO_KIND - 1])) != IP_NULL)
        {
            ops = ipnet_rtnetlink_get_ops(kind);
            ipcom_free(kind);
        }

        /* Parse link data, if we have it */
        if (ops != IP_NULL)
        {
            if (ops->maxtype != 0 && linkinfo[IP_IFLA_INFO_DATA - 1] != IP_NULL)
            {
                ret = ipnet_rtnetlink_parse(ops->maxtype, IP_RTA_DATA(linkinfo[IP_IFLA_INFO_DATA - 1]), IP_RTA_PAYLOAD(linkinfo[IP_IFLA_INFO_DATA - 1]), &linkdata);
                if (ret < 0)
                    goto done;
            }

            if (ops->validate)
            {
                ret = (*ops->validate) ( (struct Ip_nlattr**) rta, (struct Ip_nlattr**) linkdata);
                if (ret < 0)
                    goto done;
            }
        }
    }

    /* Didnt find the device? */
    if (netif != IP_NULL)
    {
        if (nlmsg->nlmsg_flags & IP_NLM_F_EXCL)
        {
            ret  = -IP_ERRNO_EEXIST;
            goto done;
        }

        if (nlmsg->nlmsg_flags & IP_NLM_F_REPLACE)
        {
            ret = -IP_ERRNO_EOPNOTSUPP;
            goto done;
        }

        if (linkdata)
        {
            if (ops == IP_NULL || ops->changelink == IP_NULL)
            {
                ret = -IP_ERRNO_EOPNOTSUPP;
                goto done;
            }

            ret = (*ops->changelink) (netif, (struct Ip_nlattr**)rta, (struct Ip_nlattr**)linkdata);
            if (ret < 0)
                goto done;
        }

        /* Do setlink */
        ret = ipnet_rtnetlink_link_setlink_family(mem, nlmsg, rta, family);
        goto done;
    }
    else
    {
        if (ops == IP_NULL || ops->newlink == IP_NULL)
        {
            ret = -IP_ERRNO_EOPNOTSUPP;
            goto done;
        }

        if (!(nlmsg->nlmsg_flags & IP_NLM_F_CREATE) || ifm->ifi_index || ifm->ifi_flags || ifm->ifi_change)
        {
            ret = -IP_ERRNO_ENODEV;
            goto done;
        }

        /* OK - We need to create the correct interface here */
        nif = (Ipnet_netif *)ipcom_if_malloc(ops->ift_type);

        if (!nif)
        {
            ret = -IP_ERRNO_ENOMEM;
            goto done;
        }

        /**/
        nif->vr_index = vr;

        /* Find a valid interface name */
        if (!ifname)
        {
            char tmpif[IP_IFNAMSIZ + 1];
            int  i;

            /**/
            for (i = 0; i < 1024; i++)
            {
                ipcom_snprintf(tmpif, sizeof(tmpif), "%s%d", ops->kind, i);
                if (!ipnet_if_nametonetif(vr, tmpif))
                {
                    ifname = ipcom_strdup(tmpif);
                    break;
                }
            }
        }

        if (!ifname)
        {
            ret = -IP_ERRNO_ENOMEM;
            goto done;
        }

        /* Store name */
        ipcom_strncpy(nif->ipcom.name, ifname, IP_IFNAMSIZ);

        /* Do we have the setup callback? */
        if (ops->setup)
        {
            /* We've gotten an interface; invoke setup routine */
            (*ops->setup) (nif);
        }

        /* Store MTU, if specified */
        if (rta[IP_IFLA_MTU - 1])
            nif->ipcom.mtu = (Ip_u32) IP_GET_32ON8(IP_RTA_DATA(rta[IP_IFLA_MTU - 1]));

        /* Successful creation? Dont allow the cleanup routine to destroy nif */
        if ((ret = (*ops->newlink) (nif, (struct Ip_nlattr**)rta, (struct Ip_nlattr**)linkdata)) >= 0)
            nif = IP_NULL;

        goto done;
    }

    /* We dont do newlink yet */
    return -IP_ERRNO_EOPNOTSUPP;

done:
    if (ifname)
        ipcom_free(ifname);
    if (linkinfo)
        ipcom_free(linkinfo);
    if (linkdata)
        ipcom_free(linkdata);
    if (nif)
        IPNET_IF_UNLOCK(nif);
    return ret;
}

/*
 *===========================================================================
 *                    ipnet_rtnetlink_link_setlink
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_rtnetlink_link_getlink_family(Ipnet_netlink_mem_t *mem,
                                    struct Ip_nlmsghdr  *nlmsg,
                                    struct Ip_rtattr    **rta,
                                    int                 family)
{
    /**/
    struct Ip_ifinfomsg     *ifm;
    char                    *ifname = IP_NULL;
    Ipnet_netif             *netif  = IP_NULL;
    int                     ret     = 0;
    Ip_u16                  vr;

    (void)family;

    ifm = IP_NLMSG_DATA(nlmsg);

    vr = ipnet_rtnetlink_vr(rta[IP_IFLA_VR - 1], mem->vr);

    if (ifm->ifi_index > 0)
        netif = ipnet_if_indextonetif(vr, ifm->ifi_index);
    else if ((ifname = ipnet_netlink_strdup((struct Ip_nlattr *)rta[IP_IFLA_IFNAME - 1])) != IP_NULL)
        netif = ipnet_if_nametonetif(vr, ifname);

    /* Didnt find the device? */
    if (netif == IP_NULL)
    {
        ret = -IP_ERRNO_ENODEV;
        goto done;
    }
            /* */
    IPNET_IF_LOCK(netif);
    ret = ipnet_rtnetlink_link_sz_fillifinfo(mem,
                                             IP_RTM_NEWLINK,
                                             0,
                                             nlmsg->nlmsg_seq,
                                             0xFFFFFFFF,
                                             netif);
    IPNET_IF_UNLOCK(netif);

done:
    if (ifname)
        ipcom_free(ifname);
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_rtnetlink_link_dellink_family
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_rtnetlink_link_dellink_family(Ipnet_netlink_mem_t *mem,
                                    struct Ip_nlmsghdr  *nlmsg,
                                    struct Ip_rtattr    **rta,
                                    int                 family)
{
    /**/
    struct Ip_ifinfomsg     *ifm;
    char                    *ifname = IP_NULL;
    Ipnet_netif             *netif  = IP_NULL;
    int                     ret     = 0;
    Ip_u16                  vr;

    (void) family;

    /* Copy or zero */
    ifname = ipnet_netlink_strdup((struct Ip_nlattr *)rta[IP_IFLA_IFNAME - 1]);

    ifm = IP_NLMSG_DATA(nlmsg);

    vr = ipnet_rtnetlink_vr(rta[IP_IFLA_VR - 1], mem->vr);

    if (ifm->ifi_index > 0)
        netif = ipnet_if_indextonetif(vr, ifm->ifi_index);
    else if (ifname)
        netif = ipnet_if_nametonetif(vr, ifname);

    /* Didnt find the device? */
    if (netif == IP_NULL)
    {
        ret = -IP_ERRNO_ENODEV;
        goto done;
    }

    if (netif->rtnetlink_ops == IP_NULL || netif->rtnetlink_ops->dellink == IP_NULL)
    {
        ret = -IP_ERRNO_EOPNOTSUPP;
        goto done;
    }

    (*netif->rtnetlink_ops->dellink) (netif);

done:
    if (ifname)
        ipcom_free(ifname);
    return ret;
}

#endif /* IPNET_USE_NETLINKSOCK */

/*
 *===========================================================================
 *                    ipnet_rtnetlink_link_if_change
 *===========================================================================
 * Description: Status of a network interface has changed, create a IFINFO
 *              event on NETLNK..
 * Parameters:  netif - The interface.
 * Returns:     -
 *
 */
IP_STATIC void
ipnet_rtnetlink_link_if_event(Ipnet_netif *netif, int event)
{
#ifdef IPNET_USE_NETLINKSOCK
    Ipcom_pkt               *pkt    = IP_NULL;
    Ipnet_netlink_mem_t     mem;
    int                     ret;

    if (!ipnet_netlink_pkt_alloc(&pkt, IP_NULL, IP_TRUE, &mem, 512))
        return;

    /* Must explicitly switch to the correct VR here */
    pkt->vr_index = mem.vr = netif->vr_index;

    ret = ipnet_rtnetlink_link_sz_fillifinfo(&mem,
                                             event,
                                             0,
                                             0,
                                             netif->rtnetlink_last_iflags ^ netif->ipcom.flags,
                                             netif);
    if (ret < 0)
        ipcom_pkt_free(pkt);
    else
    {
        pkt->end += ret;

        /* Remember current link flags */
        netif->rtnetlink_last_iflags = netif->ipcom.flags;

        /* We're done; do report this pkt */
        ipnet_rtnetlink_broadcast(pkt, IP_RTNLGRP_LINK);
    }
#else
    (void)netif;
#endif
}
/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

IP_GLOBAL void
ipnet_rtnetlink_link_change(struct Ipnet_netif_struct *netif)
{
    ipnet_rtnetlink_link_if_event(netif, IP_RTM_NEWLINK);
}

IP_GLOBAL void
ipnet_rtnetlink_link_add(struct Ipnet_netif_struct *netif)
{
    ipnet_rtnetlink_link_if_event(netif, IP_RTM_NEWLINK);
}

IP_GLOBAL void
ipnet_rtnetlink_link_delete(struct Ipnet_netif_struct *netif)
{
    ipnet_rtnetlink_link_if_event(netif, IP_RTM_DELLINK);
}

/*
 *===========================================================================
 *                    ipnet_rtnetlink_link_stats_get
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL struct Ip_rtnl_link_stats *
ipnet_rtnetlink_link_stats_get(Ipnet_netif                *netif,
                               struct Ip_rtnl_link_stats  *devstats)
{
    /* Map our internal statistics to the rtnl_link_stats */
    ipcom_memset(devstats, 0, sizeof(*devstats));
    devstats->rx_bytes      = netif->ibytes;
    devstats->rx_packets    = netif->ipackets;
    devstats->rx_errors     = netif->ierrors;
    devstats->rx_dropped    = netif->iqdrops;

    devstats->collisions    = netif->collisions;
    devstats->multicast     = netif->imcasts;
    devstats->tx_bytes      = netif->obytes;
    devstats->tx_packets    = netif->opackets;
    devstats->tx_errors     = netif->oerrors;

    return devstats;
}


IP_GLOBAL void
ipnet_rtnetlink_link_init(void)
{
#ifdef IPNET_USE_NETLINKSOCK
    /* AF_UNSPEC */
    ipnet_rtnetlink_register_family_event(IP_AF_UNSPEC, IP_RTM_DELLINK,
                                          IP_IFLA_MAX, sizeof(struct Ip_ifinfomsg),
                                          ipnet_rtnetlink_link_dellink_family,
                                          IP_NULL);
    ipnet_rtnetlink_register_family_event(IP_AF_UNSPEC, IP_RTM_NEWLINK,
                                          IP_IFLA_MAX, sizeof(struct Ip_ifinfomsg),
                                          ipnet_rtnetlink_link_newlink_family,
                                          IP_NULL);
    ipnet_rtnetlink_register_family_event(IP_AF_UNSPEC, IP_RTM_SETLINK,
                                          IP_IFLA_MAX, sizeof(struct Ip_ifinfomsg),
                                          ipnet_rtnetlink_link_setlink_family,
                                          IP_NULL);
    ipnet_rtnetlink_register_family_event(IP_AF_UNSPEC, IP_RTM_GETLINK,
                                          IP_IFLA_MAX, sizeof(struct Ip_ifinfomsg),
                                          ipnet_rtnetlink_link_getlink_family,
                                          ipnet_rtnetlink_link_dump_family);
#endif /* IPNET_USE_NETLINKSOCK */

    ipnet_rtnetlink_register_family_event(IP_AF_UNSPEC, IP_RTM_GETADDR,
                                          IP_IFA_MAX, sizeof(struct Ip_ifaddrmsg),
                                          IP_NULL,
                                          ipnet_rtnetlink_link_dump_all);
    ipnet_rtnetlink_register_family_event(IP_AF_UNSPEC, IP_RTM_GETROUTE,
                                          IP_RTA_MAX, sizeof(struct Ip_rtmsg),
                                          IP_NULL,
                                          ipnet_rtnetlink_link_dump_all);
    ipnet_rtnetlink_register_family_event(IP_AF_UNSPEC, IP_RTM_GETRULE,
                                          IP_RTA_MAX, sizeof(struct Ip_rtmsg),
                                          IP_NULL,
                                          ipnet_rtnetlink_link_dump_all);
    ipnet_rtnetlink_register_family_event(IP_AF_UNSPEC, IP_RTM_GETNEIGH,
                                          IP_NDA_MAX, sizeof(struct Ip_ndmsg),
                                          IP_NULL,
                                          ipnet_rtnetlink_link_dump_all);

#if defined(IPNET_USE_VLAN) && defined(IPNET_USE_NETLINKSOCK)
    ipnet_vlan_rtnetlink_init();
#endif /* IPNET_USE_VLAN && IPNET_USE_NETLINKSOCK */
    /* Init the neigh module */
    ipnet_rtnetlink_vr_init();

#ifdef IPMIP
    ipmip_rtnetlink_init();
#endif
}

#endif /* IPNET_USE_NETLINK */

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */



/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
