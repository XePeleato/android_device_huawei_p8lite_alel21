/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_rtnetlink_rule.c,v $ $Revision: 1.12 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_rtnetlink_rule.c,v $
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
 * Contains netlink handlers for managing virtual routers, tables and
 * interface rules.
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
#include "ipnet_netlink.h"
#include "ipnet_netlink_h.h"
#include "ipnet_loopback.h"
#include "ipcom_sock.h"
#ifdef IPCOM_USE_INET6
#include "ipcom_sock6.h"
#include "ipnet_ip6.h"
#endif


#include "ipnet_eth.h"

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */
#define IPNET_RTNETLINK_RULE_RTPRIO_TO_PRIO(prio)   (Ip_s16)IP_MIN(IPNET_PR_PRIO_MAX, (Ip_s32) ((IPNET_PR_PRIO_MIN + (prio))))
#define IPNET_RTNETLINK_RULE_PRIO_TO_RTPRIO(prio)   (Ip_u32)(-IPNET_PR_PRIO_MIN + (prio))

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */
typedef struct Ipnet_rtnetlink_rule
{
    struct
    {
        Ip_ptrdiff_t    *vr;
        Ip_ptrdiff_t    *done;
        Ip_ptrdiff_t    *rule;
        int             aborted;
        int             cont;
    } state;

    /* Requested */
    struct Ip_nlmsghdr  *nlmsg;
    Ip_u8               family;

    /* Buffering */
    Ipnet_netlink_mem_t *mem;
}
Ipnet_rtnetlink_rule_t;

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
IP_STATIC Ip_u32
ipnet_rtnetlink_pktflags_to_rtflags(Ip_u32  pktflags)
{
    Ip_u32 rtflags = 0;
    if (IP_BIT_ISSET(pktflags, IPCOM_PKT_FLAG_TUNNELED))
        rtflags |= IP_FRA_PKT_TUNNELED;
    if (IP_BIT_ISSET(pktflags, IPCOM_PKT_FLAG_FORWARDED))
        rtflags |= IP_FRA_PKT_FORWARDED;
    return rtflags;
}

IP_STATIC Ip_u32
ipnet_rtnetlink_rtflags_to_pktflags(Ip_u32  rtflags)
{
    Ip_u32 pktflags = 0;
    if (IP_BIT_ISSET(rtflags, IP_FRA_PKT_TUNNELED))
        pktflags |= IPCOM_PKT_FLAG_TUNNELED;
    if (IP_BIT_ISSET(rtflags, IP_FRA_PKT_FORWARDED))
        pktflags |= IPCOM_PKT_FLAG_FORWARDED;
    return pktflags;
}

IP_STATIC int
ipnet_rtnetlink_rule_rtrule_to_rule(struct Ip_nlmsghdr               *nlmsg,
                                    Ip_u16                           *vr,
                                    Ip_u8                            family,
                                    struct Ip_rtattr                 **rta,
                                    struct Ipnet_policy_routing_rule *mpr)
{
    struct Ip_fib_rule_hdr              *fib    = IP_NLMSG_DATA(nlmsg);
    Ip_size_t                           addrlen;

#ifdef IPCOM_USE_INET
    if (family == IP_AF_INET)
        addrlen = sizeof(struct Ip_in_addr);
    else
#endif
#ifdef IPCOM_USE_INET6
    if (family == IP_AF_INET6)
        addrlen = sizeof(struct Ip_in6_addr);
    else
#endif
    {
        return -IP_ERRNO_EINVAL;
    }

    ipcom_memset(mpr, 0, sizeof(*mpr));

    mpr->af              = family;
    mpr->daddr_prefixlen = fib->dst_len;
    mpr->saddr_prefixlen = fib->src_len;

    if (*vr == IPCOM_VR_ANY)
        return -IP_ERRNO_EINVAL;

    *vr = ipnet_rtnetlink_vr(rta[IP_FRA_VR - 1], *vr);

    /* Do table */
    if (rta[IP_FRA_TABLE - 1])
        mpr->table = IP_GET_32ON8(IP_RTA_DATA(rta[IP_FRA_TABLE - 1]));
    else
        mpr->table = fib->table;

    if (rta[IP_FRA_IFNAME - 1])
    {
        Ipnet_netif *netif = ipnet_if_nametonetif(*vr, IP_RTA_DATA(rta[IP_FRA_IFNAME - 1]));
        if (!netif)
            return -IP_ERRNO_EINVAL;

        mpr->ifindex = netif->ipcom.ifindex;
        IP_BIT_SET(mpr->mask, IPNET_PR_RULE_IFINDEX);
    }

    if (rta[IP_FRA_PRIORITY - 1])
    {
        Ip_s32  tmp = (Ip_s32)IP_GET_32ON8(IP_RTA_DATA(rta[IP_FRA_PRIORITY - 1]));
        mpr->prio = IPNET_RTNETLINK_RULE_RTPRIO_TO_PRIO(tmp);
    }

    if (rta[IP_FRA_PROTOCOL - 1])
    {
        IP_BIT_SET(mpr->mask, IPNET_PR_RULE_PROTO);
        mpr->proto   = *(Ip_u8 *)IP_RTA_DATA(rta[IP_FRA_PROTOCOL - 1]);
    }

    if (rta[IP_FRA_SRC - 1])
    {
        ipcom_memcpy(&mpr->saddr, IP_RTA_DATA(rta[IP_FRA_SRC - 1]), addrlen);
        IP_BIT_SET(mpr->mask, IPNET_PR_RULE_SADDR);
    }

    if (rta[IP_FRA_DST - 1])
    {
        ipcom_memcpy(&mpr->daddr, IP_RTA_DATA(rta[IP_FRA_DST - 1]), addrlen);
        IP_BIT_SET(mpr->mask, IPNET_PR_RULE_DADDR);
    }

    if (rta[IP_FRA_FLOW - 1])
    {
        mpr->flow = IP_GET_32ON8(IP_RTA_DATA(rta[IP_FRA_FLOW - 1]));
        IP_BIT_SET(mpr->mask, IPNET_PR_RULE_FLOW);
    }

#ifdef IPCOM_USE_INET6
    if (rta[IP_FRA_SCOPE - 1])
    {
        mpr->scope = ipnet_rtnetlink_ip6_rtscope_to_scope(IP_GET_32ON8(IP_RTA_DATA(rta[IP_FRA_SCOPE - 1])));
        IP_BIT_SET(mpr->mask, IPNET_PR_RULE_SCOPE);
    }
#endif

    if (fib->tos)
    {
        mpr->ds = fib->tos;
        IP_BIT_SET(mpr->mask, IPNET_PR_RULE_DS);
    }

    if (rta[IP_FRA_FLOW - 1])
    {
        mpr->flow = IP_GET_32ON8(IP_RTA_DATA(rta[IP_FRA_FLOW - 1]));
        IP_BIT_SET(mpr->mask, IPNET_PR_RULE_FLOW);
    }

    /**/
    if (rta[IP_FRA_ID - 1])
        mpr->id = IP_GET_32ON8(IP_RTA_DATA(rta[IP_FRA_ID - 1]));

    if (rta[IP_FRA_PKTRULE - 1] && rta[IP_FRA_PKTMASK - 1])
    {
        mpr->pkt_mask   = ipnet_rtnetlink_rtflags_to_pktflags(IP_GET_32ON8(IP_RTA_DATA(rta[IP_FRA_PKTMASK - 1])));
        mpr->pkt_result = ipnet_rtnetlink_rtflags_to_pktflags(IP_GET_32ON8(IP_RTA_DATA(rta[IP_FRA_PKTRULE - 1])));
        IP_BIT_SET(mpr->mask, IPNET_PR_RULE_PKTFLAGS);
    }

    if (rta[IP_FRA_EXTACT - 1])
    {
        Ip_u32 extact = IP_GET_32ON8(IP_RTA_DATA(rta[IP_FRA_EXTACT - 1]));
        if (IP_BIT_ISSET(extact, IP_FRA_EXT_RT_SRC_ADDR))
            IP_BIT_SET(mpr->mask, IPNET_PR_RULE_RTKEYSADDR);
    }

    /* Sanitize */
    mpr->table = IPCOM_ROUTE_TABLE_GET(mpr->table);

    return 0;
}



/*
 *===========================================================================
 *                    ipnet_rtnetlink_ip4_rule_fill_info
 *===========================================================================
 * Description: Fill a NETLINK message with info about a route.
 * Parameters:  data - Message buffer.
 *              rt   - IPNET route entry.
 *              pid  - NETLINK caller pid.
 *              len  - Available buffer length
 *
 * Returns:     0 - Success
 *             <0 - Failure
 *
 */
IP_STATIC int
ipnet_rtnetlink_rule_fill_info(Ipnet_netlink_mem_t              *mem,
                               Ip_u16                           vr,
                               Ip_u8                            family,
                               struct Ipnet_policy_routing_rule *rule,
                               int                              seq,
                               Ip_u8                            event,
                               Ip_u32                           eflags)
{
    struct Ip_nlmsghdr        *nlh;
    Ip_u32                    tmp32;
    Ip_size_t                 addrlen;
    struct Ip_fib_rule_hdr     *fib;

    nlh = ipnet_netlink_nlmsg_sz_begin(mem, seq, event, eflags);
    if (!nlh)
        return -1;

    fib = ipnet_netlink_sz_put(mem, IP_NULL, sizeof (*fib));
    if (!fib)
        return -1;

#ifdef IPCOM_USE_INET
    if (family == IP_AF_INET)
        addrlen = sizeof(struct Ip_in_addr);
    else
#endif
#ifdef IPCOM_USE_INET6
    if (family == IP_AF_INET6)
        addrlen = sizeof(struct Ip_in6_addr);
    else
#endif
    {
        return 0;
    }


    ipcom_memset(fib, 0, sizeof(*fib));

    /* Add route message header */
    fib->family         = family;

    /* Only have unicast right now */
    fib->action         = IP_FR_ACT_TO_TBL;
    fib->table          = rule->table;

    /* Convert to 0 ... MAX */
    {
        tmp32 = (Ip_u32)(-IPNET_PR_PRIO_MIN + rule->prio);
        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_FRA_PRIORITY, sizeof(tmp32), &tmp32))
            return -1;
    }

    tmp32 = vr;
    if (!ipnet_rtnetlink_rta_sz_put(mem, IP_FRA_VR, sizeof(tmp32), &tmp32))
        return -1;

    if (!ipnet_rtnetlink_rta_sz_put(mem, IP_FRA_ID, sizeof(rule->id), &rule->id))
        return -1;

    /* Store the source & destination */
    if (IP_BIT_ISSET(rule->mask, IPNET_PR_RULE_DS))
        fib->tos = rule->ds;

    /* Store the source & destination */
    if (IP_BIT_ISSET(rule->mask, IPNET_PR_RULE_DADDR))
    {
        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_FRA_DST, addrlen, &rule->daddr))
            return -1;

        fib->dst_len = rule->daddr_prefixlen;
    }

    if (IP_BIT_ISSET(rule->mask, IPNET_PR_RULE_SADDR))
    {
        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_FRA_SRC, addrlen, &rule->saddr))
            return -1;

        fib->src_len = rule->saddr_prefixlen;
    }

    /* Store table */
    if (!ipnet_rtnetlink_rta_sz_put(mem, IP_FRA_TABLE, sizeof(rule->table), &rule->table))
        return -1;

    /**/
    if (IP_BIT_ISSET(rule->mask, IPNET_PR_RULE_PROTO))
    {
        /* Put destination address */
        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_FRA_PROTOCOL, sizeof(rule->proto), &rule->proto))
            return -1;
    }

    if (IP_BIT_ISSET(rule->mask, IPNET_PR_RULE_IFINDEX))
    {
        /* Put destination address */
        Ipnet_netif *netif = ipnet_if_indextonetif(vr, rule->ifindex);
        if (netif)
            if (!ipnet_rtnetlink_rta_sz_put(mem, IP_FRA_IFNAME, ipcom_strlen(netif->ipcom.name) + 1, netif->ipcom.name))
                return -1;
    }

    if (IP_BIT_ISSET(rule->mask, IPNET_PR_RULE_FLOW))
    {
        /* Put destination address */
        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_FRA_FLOW, sizeof(rule->flow), &rule->flow))
            return -1;
    }

#ifdef IPCOM_USE_INET6
    if (family == IP_AF_INET6 && IP_BIT_ISSET(rule->mask, IPNET_PR_RULE_SCOPE))
    {
        tmp32 = ipnet_rtnetlink_ip6_scope_to_rtscope(rule->scope);
        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_FRA_SCOPE, sizeof(tmp32), &tmp32))
            return -1;
    }
#endif

    /**/
    if (IP_BIT_ISSET(rule->mask, IPNET_PR_RULE_PKTFLAGS))
    {
        tmp32 = ipnet_rtnetlink_pktflags_to_rtflags(rule->pkt_result);
        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_FRA_PKTRULE, sizeof(Ip_u32), &tmp32))
            return -1;
        tmp32 = ipnet_rtnetlink_pktflags_to_rtflags(rule->pkt_mask);
        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_FRA_PKTMASK, sizeof(Ip_u32), &tmp32))
            return -1;
    }

    /**/
    if (IP_BIT_ISSET(rule->mask, IPNET_PR_RULE_RTKEYSADDR))
    {
        Ip_u32  extact = 0;
        if (IP_BIT_ISSET(rule->mask, IPNET_PR_RULE_RTKEYSADDR))
            extact |= IP_FRA_EXT_RT_SRC_ADDR;
        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_FRA_EXTACT, sizeof(Ip_u32), &extact))
            return -1;
    }

    /* Done */
    return ipnet_netlink_nlmsg_sz_end(mem, nlh);
}


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
ipnet_rtnetlink_rule_foreach_vr(Ipcom_route *rtab, Ipnet_rtnetlink_rule_t *param)
{
    Ip_u16                          vr       = IPNET_ROUTE_GET_VR(rtab);
    Ipnet_route_policy_rule         *r       = IP_NULL;

    /* This traversal is already aborted */
    if (param->state.aborted)
        return;

    /* Only handle rtabs of selected address family */
    if (IPNET_ROUTE_GET_FAMILY(rtab) != param->family)
        return;

    /* Go for the defaults to get current VR's */
    if (IPNET_ROUTE_GET_TABLE(rtab) != IPCOM_ROUTE_TABLE_DEFAULT)
        return;

    /*
     * If this is a trailer message buffer, make sure it
     * starts dumping where the last message ended
     */
    if (param->state.cont)
    {
        Ipnet_route_policy_rule         *rtmp     = (Ipnet_route_policy_rule*)(*param->state.rule);
        /* Verify current state */
        if (*param->state.vr != vr)
            return;

        r = ipnet_route_policy_get_rule_chain(param->family, vr);
        /**/
        if (r)
        {
            while (r != IP_NULL && r != rtmp)
                r = IPCOM_LIST_NEXT(&r->list);
        }
    }
    else
    {
        r = ipnet_route_policy_get_rule_chain(param->family, vr);
    }

    /* Reset the continue variable; we're on the move again */
    param->state.cont = 0;

    if (r == IP_NULL)
        return;

    do
    {
        /* Time to start dump info */
        if (ipnet_rtnetlink_rule_fill_info(param->mem, vr, param->family, &r->rule, param->nlmsg->nlmsg_seq, IP_RTM_NEWRULE, IP_NLM_F_MULTI) < 0)
            goto aborted;
    } while ((r = IPCOM_LIST_NEXT(&r->list)) != IP_NULL);

    return;

aborted:
    param->state.aborted = 1;
    *param->state.rule   = (Ip_ptrdiff_t)r;
    *param->state.vr     = (Ip_ptrdiff_t)vr;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_rtnetlink_rule_event
 *===========================================================================
 * Description: Status of a network rule has changed, create a rule
 *              event on NETLNK..
 * Parameters:  netif - The interface.
 * Returns:     -
 *
 */
IP_GLOBAL void
ipnet_rtnetlink_rule_event(struct Ipnet_policy_routing_rule *mpr,
                           Ip_u16                           vr,
                           Ip_pid_t                         pid,
                           int                              event)
{
    Ipcom_pkt               *pkt    = IP_NULL;
    Ipnet_netlink_mem_t     mem;
    int                     ret     = -1;
    int                     group;

    if (!ipnet_netlink_pkt_alloc(&pkt, IP_NULL, IP_TRUE, &mem, 512))
        return;

    /* Must explicitly switch to the correct VR here */
    pkt->vr_index = mem.vr = vr;
    mem.pid = pid;

    ret = ipnet_rtnetlink_rule_fill_info(&mem,
                                         vr,
                                         mpr->af,
                                         mpr,
                                         0,
                                         event,
                                         0);
#ifdef IPCOM_USE_INET
    if (mpr->af == IP_AF_INET)
        group = IP_RTNLGRP_IPV4_RULE;
#endif
#ifdef IPCOM_USE_INET
    if (mpr->af == IP_AF_INET6)
        group = IP_RTNLGRP_IPV6_RULE;
#endif

    if (ret < 0)
        ipcom_pkt_free(pkt);
    else
    {
        pkt->end += ret;

        /* We're done; do report this pkt */
        ipnet_rtnetlink_broadcast(pkt, group);
    }
}




/*
 *==========================================================================
 *                    ipnet_rtnetlink_rule_dump
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
ipnet_rtnetlink_rule_dump_family(Ipnet_netlink_mem_t *mem,
                                 struct Ip_nlmsghdr  *nlmsg,
                                 int                 family,
                                 Ip_ptrdiff_t        *args)

{
    Ip_ptrdiff_t            *done       = &args[0];
    Ip_ptrdiff_t            *s_vr       = &args[1];
    Ip_ptrdiff_t            *s_rule     = &args[2];
    Ip_ptrdiff_t            *s_continue = &args[3];
    Ipnet_rtnetlink_rule_t  param;

    param.nlmsg     = nlmsg;
    param.mem       = mem;

    if (*done)
        goto exit_out;

    param.family        = family;
    param.state.aborted = 0;
    param.state.done    = done;
    param.state.vr      = s_vr;
    param.state.rule    = s_rule;
    param.state.cont    = *s_continue;

    /* */
    ipcom_hash_for_each(ipnet->rtabs,
                        (Ipcom_hash_foreach_cb_func)ipnet_rtnetlink_rule_foreach_vr,
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
 *                    ipnet_rtnetlink_rule_newrule_family
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_rule_newrule_family(Ipnet_netlink_mem_t *mem,
                                    struct Ip_nlmsghdr  *nlmsg,
                                    struct Ip_rtattr    **rta,
                                    int                 family)
{
    struct Ipnet_policy_routing_rule    mpr;
    struct Ip_fib_rule_hdr              *fib    = IP_NLMSG_DATA(nlmsg);
    Ip_u16                              vr;
    int                                 ret;

    vr = mem->vr;

    /* We only do unicast for now */
    if (fib->action != IP_RTN_UNICAST)
        return -IP_ERRNO_EINVAL;

    /**/
    ret = ipnet_rtnetlink_rule_rtrule_to_rule(nlmsg, &vr, family, rta, &mpr);
    if (ret < 0)
        return ret;

    /* Add this rule */
    return ipnet_route_policy_ioctl(mem->pid, vr, IPNET_SIOCSPRRULE, &mpr);
}

/*
 *===========================================================================
 *                    ipnet_rtnetlink_rule_newrule_family
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_rule_delrule_family(Ipnet_netlink_mem_t *mem,
                                    struct Ip_nlmsghdr  *nlmsg,
                                    struct Ip_rtattr    **rta,
                                    int                 family)
{
    struct Ipnet_policy_routing_rule    mpr;
    Ip_u16                              vr;
    int                                 ret;

    vr = mem->vr;

    /**/
    ret = ipnet_rtnetlink_rule_rtrule_to_rule(nlmsg, &vr, family, rta, &mpr);
    if (ret < 0)
        return ret;

    if (mpr.id == IPNET_PR_ID_NONE)
    {
        Ipnet_route_policy_rule         *r;

        /* Try find this rule */
        r = ipnet_route_policy_get_rule_chain(family, vr);
        for (; r != IP_NULL; r = IPCOM_LIST_NEXT(&r->list))
        {
            mpr.id = r->rule.id;

            /* Not the same table */
            if (mpr.table != r->rule.table)
                continue;

            /* Do item for item compare */
            if (IP_BIT_ISSET(mpr.mask, IPNET_PR_RULE_DS))
            {
                if (IP_BIT_ISFALSE(r->rule.mask, IPNET_PR_RULE_DS) || r->rule.ds != mpr.ds)
                    continue;
            }

            /* Do item for item compare */
            if (IP_BIT_ISSET(mpr.mask, IPNET_PR_RULE_PROTO))
            {
                if (IP_BIT_ISFALSE(r->rule.mask, IPNET_PR_RULE_PROTO) || r->rule.proto != mpr.proto)
                    continue;
            }

            /* Do item for item compare */
            if (IP_BIT_ISSET(mpr.mask, IPNET_PR_RULE_SADDR))
            {
                if (IP_BIT_ISFALSE(r->rule.mask, IPNET_PR_RULE_SADDR) ||
                    r->rule.saddr_prefixlen != mpr.saddr_prefixlen ||
                    !ipnet_route_key_cmp(&r->rule.saddr, &mpr.saddr, r->rule.saddr_prefixlen))
                    continue;
            }

            /* Do item for item compare */
            if (IP_BIT_ISSET(mpr.mask, IPNET_PR_RULE_DADDR))
            {
                if (IP_BIT_ISFALSE(r->rule.mask, IPNET_PR_RULE_DADDR) ||
                    r->rule.daddr_prefixlen != mpr.daddr_prefixlen ||
                    !ipnet_route_key_cmp(&r->rule.daddr, &mpr.daddr, r->rule.daddr_prefixlen))
                    continue;
            }

            /* Do item for item compare */
            if (IP_BIT_ISSET(mpr.mask, IPNET_PR_RULE_RTKEYSADDR))
            {
                if (IP_BIT_ISFALSE(r->rule.mask, IPNET_PR_RULE_RTKEYSADDR))
                    continue;
            }

            /* Do item for item compare */
            if (IP_BIT_ISSET(mpr.mask, IPNET_PR_RULE_IFINDEX))
            {
                if (IP_BIT_ISFALSE(r->rule.mask, IPNET_PR_RULE_IFINDEX) || r->rule.ifindex != mpr.ifindex)
                    continue;
            }

            /* Do item for item compare */
            if (IP_BIT_ISSET(mpr.mask, IPNET_PR_RULE_PKTFLAGS))
            {
                if (IP_BIT_ISFALSE(r->rule.mask, IPNET_PR_RULE_PKTFLAGS) || r->rule.pkt_mask != mpr.pkt_mask || r->rule.pkt_result != mpr.pkt_result)
                    continue;
            }

            /* Do item for item compare */
            if (IP_BIT_ISSET(mpr.mask, IPNET_PR_RULE_FLOW))
            {
                if (IP_BIT_ISFALSE(r->rule.mask, IPNET_PR_RULE_FLOW) || r->rule.flow != mpr.flow)
                    continue;
            }

            /* Do item for item compare */
            if (IP_BIT_ISSET(mpr.mask, IPNET_PR_RULE_SCOPE))
            {
                if (IP_BIT_ISFALSE(r->rule.mask, IPNET_PR_RULE_SCOPE) || r->rule.scope != mpr.scope)
                    continue;
            }

            /* Passed all */
            break;
        }

        if (!r)
            return -IP_ERRNO_ENOENT;
    }

    /* Add this rule */
    return ipnet_route_policy_ioctl(mem->pid, vr, IPNET_SIOCDPRRULE, &mpr);
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

