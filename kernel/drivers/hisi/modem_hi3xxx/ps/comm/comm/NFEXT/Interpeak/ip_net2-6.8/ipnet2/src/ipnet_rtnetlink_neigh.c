/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_rtnetlink_neigh.c,v $ $Revision: 1.9 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_rtnetlink_neigh.c,v $
 *   $Author: ulf $
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
 * Contains netlink handlers for managing Ipv4 and Ipv6 addresses.
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
#ifdef IPCOM_USE_INET6
#include "ipcom_sock6.h"
#include "ipnet_ip6.h"
#endif

#ifdef IP_PORT_LKM
#include "ipcom_lkm.h" /* Needed for Linux PPP interface config */
#endif /* IP_PORT_LKM */

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
typedef struct Ipnet_rtnetlink_neigh_family
{
    struct
    {
        Ip_ptrdiff_t    *vr;
        Ip_ptrdiff_t    *rt;
        Ip_ptrdiff_t    *done;
        int             aborted;
        int             cont;
    } state;

    /* Requested */
    struct Ip_nlmsghdr  *nlmsg;
    Ip_u8               family;

    /* Buffering */
    Ipnet_netlink_mem_t *mem;
}
Ipnet_rtnetlink_neigh_family_t;

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
 *===========================================================================
 *                    ipnet_rtnetlink_route_foreach_vr
 *===========================================================================
 * Description: Hash table foreach callback. Called once for each virtual
 *              router that exists in the system.
 * Parameters:  rtab  - route table
 *              param - callback parameters.
 * Returns:     -
 *
 */
IP_STATIC void
ipnet_rtnetlink_neigh_foreach_vr_family(Ipcom_route *rtab, Ipnet_rtnetlink_neigh_family_t *param)
{
    Ip_u16             vr       = IPNET_ROUTE_GET_VR(rtab);
    Ipnet_route_entry  *rt      = IP_NULL;

    /* This traversal is already aborted */
    if (param->state.aborted)
        return;

    /* Only handle rtabs of selected address family */
    if (param->family != IP_AF_UNSPEC && IPNET_ROUTE_GET_FAMILY(rtab) != param->family)
        return;

    /* We only have neigh values in the default table */
    if (IPNET_ROUTE_GET_TABLE(rtab) != IPCOM_ROUTE_TABLE_DEFAULT)
        return;

    /* Check if specific VR shall be dumped */
    if (IP_BIT_ISFALSE(param->nlmsg->nlmsg_flags, IP_NLM_F_VR_UNSPEC) && (param->mem->vr != IPCOM_VR_ANY) && (param->mem->vr != vr))
        return;

    /*
     * If this is a trailer message buffer, make sure it
     * starts dumping where the last message ended
     */
    if (param->state.cont)
    {
        Ipnet_route_entry  *rtt = (Ipnet_route_entry*)(*param->state.rt);

        /* Verify current state */
        if (*param->state.vr != vr)
            return;

        if (rtt != IP_NULL)
        {
            rt = ipnet_route_first_entry(IPNET_ROUTE_GET_FAMILY(rtab),
                                         vr,
                                         IPNET_ROUTE_GET_TABLE(rtab));
            while (rt != IP_NULL)
            {
                if (rt == rtt)
                    break;
                rt = ipnet_route_next_entry(rt);
            }
        }
    }


    /* Reset the continue variable; we're on the move again */
    param->state.cont = 0;

    /* Get first unless we've found one to continue from */
    if (rt == IP_NULL)
        rt = ipnet_route_first_entry(IPNET_ROUTE_GET_FAMILY(rtab),
                                     vr,
                                     IPNET_ROUTE_GET_TABLE(rtab));

    /* */
    while (rt)
    {
        do
        {
            if (ipnet_rtnetlink_neigh_is(rt))
                break;
            rt = ipnet_route_next_entry(rt);
        } while (rt != IP_NULL);

        if (rt != IP_NULL)
        {
            int ret = 0;
            /* Verify this particular NC entry */
#ifdef IPCOM_USE_INET
            if (IPNET_ROUTE_GET_FAMILY(rtab) == IP_AF_INET)
                ret = ipnet_rtnetlink_ip4_neigh_fill_info(param->mem, rt, param->nlmsg->nlmsg_seq, IP_RTM_NEWNEIGH, IP_NLM_F_MULTI);
#endif
#ifdef IPCOM_USE_INET6
            if (IPNET_ROUTE_GET_FAMILY(rtab) == IP_AF_INET6)
                ret = ipnet_rtnetlink_ip6_neigh_fill_info(param->mem, rt, param->nlmsg->nlmsg_seq, IP_RTM_NEWNEIGH, IP_NLM_F_MULTI);
#endif
            if (ret < 0)
                goto aborted;

            /* Get next */
            rt = ipnet_route_next_entry(rt);
        }
    }

    return;

aborted:
    param->state.aborted = 1;
    *param->state.rt     = (Ip_ptrdiff_t)rt;
    *param->state.vr     = (Ip_ptrdiff_t)vr;
}


/*
 *===========================================================================
 *                    ipnet_rtnetlink_ip6_addr_event
 *===========================================================================
 * Description: Status of a network interface has changed, create a IFINFO
 *              event on NETLNK..
 * Parameters:  netif - The interface.
 * Returns:     -
 *
 */
IP_STATIC void
ipnet_rtnetlink_neigh_event(struct Ipnet_route_entry_struct *rt, int event)
{
    Ipcom_pkt               *pkt    = IP_NULL;
    Ipnet_netlink_mem_t     mem;
    int                     ret     = -1;

    if (!ipnet_netlink_pkt_alloc(&pkt, IP_NULL, IP_TRUE, &mem, 512))
        return;

    /* Must explicitly switch to the correct VR here */
    pkt->vr_index = mem.vr = IPNET_ROUTE_GET_VR(rt->head);

#ifdef IPCOM_USE_INET
    if (IPNET_ROUTE_GET_FAMILY(rt->head) == IP_AF_INET)
    {
        ret = ipnet_rtnetlink_ip4_neigh_fill_info (&mem,
                                                   rt,
                                                   0,
                                                   event,
                                                   0);
    }
#endif
#ifdef IPCOM_USE_INET6
    if (IPNET_ROUTE_GET_FAMILY(rt->head) == IP_AF_INET6)
    {
        ret = ipnet_rtnetlink_ip6_neigh_fill_info (&mem,
                                                   rt,
                                                   0,
                                                   event,
                                                   0);
    }
#endif

    if (ret < 0)
        ipcom_pkt_free(pkt);
    else
    {
        pkt->end += ret;

        /* We're done; do report this pkt */
        ipnet_rtnetlink_broadcast(pkt, IP_RTNLGRP_NEIGH);
    }
}

/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *==========================================================================
 *                    ipnet_rtnetlink_neigh_dump_family
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
IP_GLOBAL int
ipnet_rtnetlink_neigh_dump_family(Ipnet_netlink_mem_t             *mem,
                                  struct Ip_nlmsghdr              *nlmsg,
                                  int                             family,
                                  Ip_ptrdiff_t                    *args)
{
    Ipnet_rtnetlink_neigh_family_t  param;
    Ip_ptrdiff_t                    *done       = &args[0];
    Ip_ptrdiff_t                    *s_vr       = &args[1];
    Ip_ptrdiff_t                    *s_rtm      = &args[2];
    Ip_ptrdiff_t                    *s_continue = &args[3];

    param.nlmsg     = nlmsg;
    param.family    = family;
    param.mem       = mem;

    if (*done)
        goto exit_out;

    param.state.aborted = 0;
    param.state.done    = done;
    param.state.rt      = s_rtm;
    param.state.vr      = s_vr;
    param.state.cont    = *s_continue;

    /* */
    ipcom_hash_for_each(ipnet->rtabs,
                        (Ipcom_hash_foreach_cb_func)ipnet_rtnetlink_neigh_foreach_vr_family,
                        &param);

    /* Are we aborted? */
    *s_continue = param.state.aborted;

    /* If not aborted; we're done */
    if (!param.state.aborted)
        *done = 1;

exit_out:
    return param.mem->used;
}

/*
 *===========================================================================
 *                    ipnet_rtnetlink_neigh_delneigh_family
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_neigh_delneigh_family(Ipnet_netlink_mem_t  *mem,
                                      struct Ip_nlmsghdr   *nlmsg,
                                      struct Ip_rtattr     **rta,
                                      int                  family)
{
    /**/
    Ip_u16                vr;
    Ip_u32                table = IPCOM_ROUTE_TABLE_DEFAULT;
    Ipnet_netif           *netif;
    int                   ret = -IP_ERRNO_EAFNOSUPPORT;
    struct Ip_ndmsg       *ndm = IP_NLMSG_DATA(nlmsg);
    Ipnet_rtnetlink_key_t ukey;

    /* Destination address required */
    if (rta[IP_NDA_DST-1] == IP_NULL)
        return -IP_ERRNO_EINVAL;

#ifdef IPNET_USE_ROUTE_TABLE_NAMES
    if (rta[IP_NDA_TABLE_NAME -1])
    {
        /* Extract router and table from name */
        if (ipnet_route_vr_and_table_from_name(IP_RTA_DATA(rta[IP_NDA_TABLE_NAME - 1]),
                                               &vr,
                                               &table) < 0)
            return -IP_ERRNO_ESRCH;
    }
    else
#endif
    {
        if (mem->vr == IPCOM_VR_ANY)
            return -IP_ERRNO_EINVAL;

        vr = ipnet_rtnetlink_vr(rta[IP_NDA_VR - 1], mem->vr);

        if (rta[IP_NDA_TABLE - 1])
            table = IP_GET_32ON8(IP_RTA_DATA(rta[IP_NDA_TABLE - 1]));

    }

    netif = ipnet_if_indextonetif(vr, ndm->ndm_ifindex);

    if (netif && (netif->vr_index != vr))
        return -IP_ERRNO_EINVAL;

    /* */
    if ((ret = ipnet_rtnetlink_route_key_setup(family, &ukey, netif? netif->ipcom.ifindex : 0, IP_NULL, -1, rta[IP_NDA_DST-1])) < 0)
        return ret;

    /* Delete neighbor */
    ret = ipnet_route_delete2(family,
                              vr,
                              table,
                              &ukey,
                              IP_NULL,
                              IP_NULL,
                              netif ? netif->ipcom.ifindex : 0,
                              0,
                              0,
                              0);

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_rtnetlink_neigh_newneigh_family
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_neigh_newneigh_family(Ipnet_netlink_mem_t  *mem,
                                      struct Ip_nlmsghdr   *nlmsg,
                                      struct Ip_rtattr     **rta,
                                      int                  family)
{
    Ipnet_netif                  *netif;
    Ipnet_route_entry            *rt;
    int                          nd_state;
    int                          ret = -IP_ERRNO_EAFNOSUPPORT;
    Ip_u16                       vr;
    Ip_u32                       table = IPCOM_ROUTE_TABLE_DEFAULT;
    struct Ip_sockaddr_dl        dl;
    struct Ip_ndmsg              *ndm = IP_NLMSG_DATA(nlmsg);
    void                         *link_addr = IP_NULL;
    Ipnet_rtnetlink_key_t        ukey;

     /* Destination address required */
    if (rta[IP_NDA_DST-1] == IP_NULL)
        return -IP_ERRNO_EINVAL;

#ifdef IPNET_USE_ROUTE_TABLE_NAMES
    if (rta[IP_NDA_TABLE_NAME -1])
    {
        /* Extract router and table from name */
        if (ipnet_route_vr_and_table_from_name(IP_RTA_DATA(rta[IP_NDA_TABLE_NAME - 1]),
                                               &vr,
                                               &table) < 0)
            return -IP_ERRNO_ESRCH;
    }
    else
#endif /* IPNET_USE_ROUTE_TABLE_NAMES */
    {
        if (mem->vr == IPCOM_VR_ANY)
            return -IP_ERRNO_EINVAL;

        vr = ipnet_rtnetlink_vr(rta[IP_NDA_VR - 1], mem->vr);

        if (rta[IP_NDA_TABLE - 1])
            table = IP_GET_32ON8(IP_RTA_DATA(rta[IP_NDA_TABLE - 1]));
    }

    netif = ipnet_if_indextonetif(vr, ndm->ndm_ifindex);

    if (netif && (netif->vr_index != vr))
         return -IP_ERRNO_EINVAL;

    if ((ret = ipnet_rtnetlink_route_key_setup(family, &ukey, netif? netif->ipcom.ifindex : 0, IP_NULL, -1, rta[IP_NDA_DST-1])) < 0)
        return ret;

    if (rta[IP_NDA_LLADDR-1])
        /* Neighbors link layer address */
        link_addr = IP_RTA_DATA(rta[IP_NDA_LLADDR-1]);
    else if (IP_BIT_ISSET(ndm->ndm_flags, IP_NTF_PROXY))
    {
        /* Proxy ARP entry. Use Link layer address of local netif */
        if (netif == IP_NULL)
            return -IP_ERRNO_EINVAL;

        link_addr = netif->ipcom.link_addr;
    }

    /* Check if neighbor route exists */
    ret = ipnet_route_lookup(family,
                             vr,
                             table,
                             IPNET_RTL_FLAG_LINK_LOCAL | IPNET_RTL_FLAG_DONTCLONE,
                             &ukey,
                             netif ? netif->ipcom.ifindex : 0,
                             netif ? netif->ipcom.ifindex : 0,
                             &rt);

    if ((ret == IPNET_ROUTE_PERFECT_MATCH)
        && (rt->netif == netif)
        && IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_HOST)
        && IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_LLINFO))
    {
        if (rt->data == IP_NULL)
            return -IP_ERRNO_EINVAL;

        /* Get the neighbour state */
        nd_state = *(Ipnet_nd_state_t *)rt->data;

        if (((ndm->ndm_state == IP_NUD_INCOMPLETE)
             && (nd_state != IPNET_ND_UNINITIALIZED)
             && (nd_state != IPNET_ND_INCOMPLETE))
            || ((ndm->ndm_state == IP_NUD_DELAY)
                && (nd_state != IPNET_ND_STALE)))
        {
            return -IP_ERRNO_EINVAL;
        }
    }
    else
    {
        struct Ipnet_rt_metrics      metrics;
        struct Ipnet_route_add_param param;

        /* Create new neighbor entry */
        if (ndm->ndm_state == IP_NUD_DELAY)
            ndm->ndm_state = IP_NUD_INCOMPLETE;

        ipcom_memset(&param, 0, sizeof(struct Ipnet_route_add_param));
        ipcom_memset(&dl, 0, sizeof(struct Ip_sockaddr_dl));

        param.domain     = family;
        param.vr         = vr;
        param.table      = table;
        param.key        = &ukey;
        param.netif      = netif;
        param.flags      = IPNET_RTF_UP | IPNET_RTF_HOST | IPNET_RTF_LLINFO | IPNET_RTF_DONE;
        param.gateway    = (struct Ip_sockaddr*) &dl;

        if (ndm->ndm_state != IP_NUD_PERMANENT)
        {
            ipcom_memset(&metrics, 0, sizeof(metrics));
            /* The correct timeout will be set when the ND state is set,
               but must be != infinite for now */
            metrics.rmx_expire = 1;
            param.metrics = &metrics;
        }

        IPCOM_SA_LEN_SET(&dl, sizeof(struct Ip_sockaddr_dl));
        dl.sdl_family    = IP_AF_LINK;
        dl.sdl_index     = (Ip_u16)(netif ? netif->ipcom.ifindex : 0);
        dl.sdl_alen      = 6;
        dl.sdl_type      = IP_IFT_ETHER;

        if (link_addr)
            ipcom_memcpy(&dl.sdl_data[0], link_addr,dl.sdl_alen);

        if (netif)
            IPNET_IF_LOCK(netif);

        /* Add new neighbor */
        ret = ipnet_route_add(&param);

        if (netif)
            IPNET_IF_UNLOCK(netif);

        if (ret < 0)
            return ret;

        /* Get the new neighbor route entry */
        ret = ipnet_route_lookup(family,
                                 vr,
                                 table,
                                 IPNET_RTL_FLAG_LINK_LOCAL | IPNET_RTL_FLAG_DONTCLONE,
                                 &ukey,
                                 netif ? netif->ipcom.ifindex : 0,
                                 netif ? netif->ipcom.ifindex : 0,
                                 &rt);
        if (ret < 0)
            return ret;
    }

#ifdef IPNET_COMPENSATE_UNRELIABLE_NETLINK
    /* Set neighbor acknowledge bit */
    IP_BIT_SET(rt->hdr.flags, IPNET_RTF_X_NEIGH_ACK);
#endif

    if (IP_BIT_ISSET(ndm->ndm_flags,IP_NTF_PROXY))
        /* Proxy ARP entry */
        IP_BIT_SET(rt->hdr.flags, IPNET_RTF_PROTO2);

    if (IP_BIT_ISSET(ndm->ndm_state, IP_NUD_PERMANENT))
        ipnet_route_set_lifetime(rt, IPCOM_ADDR_INFINITE);

    nd_state = ipnet_rtnetlink_neigh_nud2nc(ndm->ndm_state);

#ifdef IPCOM_USE_INET
    if (family == IP_AF_INET)
        ret = ipnet_rtnetlink_ip4_neigh_setup(&ukey, ndm, rt, nd_state, link_addr, rta);
    else
#endif
#ifdef IPCOM_USE_INET6
    if (family == IP_AF_INET6)
        ret = ipnet_rtnetlink_ip6_neigh_setup(&ukey, ndm, rt, nd_state, link_addr, rta);
    else
#endif
        return -IP_ERRNO_EAFNOSUPPORT;

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_rtnetlink_neigh_is
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_bool
ipnet_rtnetlink_neigh_is(struct Ipnet_route_entry_struct    *rt)
{
    /* Find the first NC entry */
    if (IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_LLINFO)
        && IP_BIT_ISSET(rt->hdr.flags, IPNET_RTF_HOST)
        && IP_BIT_ISFALSE(rt->hdr.flags, IPNET_RTF_PROTO2)
        && rt->gateway
        && rt->gateway->sa_family == IP_AF_LINK)
    {
        /* Found a NC entry */
        return IP_TRUE;
    }
    return IP_FALSE;
}


/*
 *===========================================================================
 *                   ipnet_rtnetlink_neigh_nc2nud
 *===========================================================================
 * Description: Converts neighbor states from IPNET to NETLINK standard.
 * Parameters:  nc_state - IPNET neighbor state
 * Returns:     NETLINK neighbor state
 *
 */
IP_GLOBAL Ip_u16
ipnet_rtnetlink_neigh_nc2nud(int nc_state, Ip_u32 expire)
{
    Ip_u16 flags    = 0;

    if (expire == IPCOM_ADDR_INFINITE)
        flags |= IP_NUD_PERMANENT;

    switch(nc_state)
    {
    case IPNET_ND_INCOMPLETE:
    case IPNET_ND_UNINITIALIZED:
        flags |= IP_NUD_INCOMPLETE;
        break;
    case IPNET_ND_REACHABLE:
        flags |= IP_NUD_REACHABLE;
        break;
    case IPNET_ND_STALE:
        flags |= IP_NUD_STALE;
        break;
    case IPNET_ND_DELAY:
        flags |= IP_NUD_DELAY;
        break;
    case IPNET_ND_PROBE:
        flags |= IP_NUD_PROBE;
        break;
    case IPNET_ND_PROXY:
        flags |= IP_NUD_NONE;
        break;
    default:
        flags |= IP_NUD_FAILED;
        break;
    }

    return flags;
}

/*
 *===========================================================================
 *                   ipnet_rtnetlink_neigh_nud2nc
 *===========================================================================
 * Description: Converts neighbor states from NETLINK standard to IPNET
 * Parameters:  ndm_state - NETLINK neighbor state
 * Returns:     IPNET neighbor state.
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_neigh_nud2nc(Ip_u16 ndm_state)
{
    if (IP_BIT_ISSET(ndm_state, IP_NUD_PERMANENT|IP_NUD_REACHABLE))
        return IPNET_ND_REACHABLE;
    else if (IP_BIT_ISSET(ndm_state, IP_NUD_STALE))
        return IPNET_ND_STALE;
    else if (IP_BIT_ISSET(ndm_state, IP_NUD_PROBE))
        return IPNET_ND_PROBE;
    else if (IP_BIT_ISSET(ndm_state, IP_NUD_DELAY))
        return IPNET_ND_DELAY;
    else if (IP_BIT_ISSET(ndm_state, IP_NUD_INCOMPLETE))
        return IPNET_ND_INCOMPLETE;
    else
        return IPNET_ND_UNINITIALIZED;
}


IP_GLOBAL void
ipnet_rtnetlink_neigh_add(Ipnet_route_entry *rt)
{
    ipnet_rtnetlink_neigh_event(rt, IP_RTM_NEWNEIGH);
}

IP_GLOBAL void
ipnet_rtnetlink_neigh_change(Ipnet_route_entry *rt)
{
    ipnet_rtnetlink_neigh_event(rt, IP_RTM_NEWNEIGH);
}

IP_GLOBAL void
ipnet_rtnetlink_neigh_del(Ipnet_route_entry *rt)
{
    ipnet_rtnetlink_neigh_event(rt, IP_RTM_DELNEIGH);
}

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */


#endif /* IPNET_USE_NETLINK */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
