/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipipsec_ctrl.c,v $ $Revision: 1.49 $
 *   $Source: /home/interpeak/CVSRoot/ipipsec2/src/ipipsec_ctrl.c,v $
 *   $Author: lob $
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
#include "ipipsec_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_type.h>
#include <ipcom_time.h>
#include <ipcom_cstyle.h>
#include <ipcom_clib.h>
#include <ipcom_inet.h>
#include <ipcom_err.h>
#include <ipcom_os.h>
#include <ipcom_sysvar.h>
#include <ipcom_syslog.h>

#include "ipipsec_h.h"
#include "ipipsec_pfkeyv2.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPIPSEC_LEAVE(xerrval)   do { retval = xerrval; goto leave; } while(0)


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

#ifndef IP_PORT_LAS

/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

IP_EXTERN Ipipsec_data  ipipsec;


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_STATIC Ipipsec_sa *
ipipsec_sa_lex_get(Ipipsec_sa_handle *sah, Ipipsec_sa_handle *sah_next);

/*IP_STATIC Ipipsec_flow *ipipsec_flow_lex_get(Ip_u32 id);*/
IP_STATIC Ipipsec_flow *ipipsec_flow_sort_get(Ip_u32 id);


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
 *                    ipipsec_sa_to_handle
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipipsec_sa_to_handle(Ipipsec_sa *sa, Ipipsec_sa_handle *handle)
{
    ipcom_memset(handle, 0, sizeof(Ipipsec_sa_handle));

    if (sa != &ipipsec.sa_bypass)
    {
        ipcom_memcpy(&handle->dst, &sa->key.dst, sa->in_addr_size);
        ipcom_memcpy(&handle->src, &sa->key.src, sa->in_addr_size);
    }

    handle->spi_n     = sa->key.ports.espah_spi_n;
    handle->protocol  = IPIPSEC_SADB_GET_PROTO(sa->sadb_msg_satype);
    handle->direction = sa->key.direction;
    handle->domain    = sa->key.domain;
    handle->satype    = sa->sadb_msg_satype;
    handle->priority  = sa->key.priority;
    handle->dscp      = sa->key.dscp;
}


/*
 *===========================================================================
 *                    ipipsec_handle_to_key
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipipsec_handle_to_key(Ipipsec_sa_handle *sah, Ipipsec_key *key)
{
    ipcom_memset(key, 0, sizeof(Ipipsec_key));

    ipcom_memcpy(&key->dst, &sah->dst, sah->domain == IP_AF_INET ? 4 : 16);
    ipcom_memcpy(&key->src, &sah->src, sah->domain == IP_AF_INET ? 4 : 16);
    key->ports.espah_spi_n = sah->spi_n;
    key->proto             = sah->protocol;
    key->direction         = sah->direction;
    key->domain            = sah->domain;
    /* satype */
    key->priority          = sah->priority;
    key->dscp              = sah->dscp;
}


/*
 *===========================================================================
 *                    ipipsec_sa_lex_get
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ipipsec_sa *
ipipsec_sa_lex_get(Ipipsec_sa_handle *sah, Ipipsec_sa_handle *sah_next)
{
    Ipipsec_sa   *cur, *sa;
    int     sadir, domain_index;

    if (sah->spi_n)
    {
        Ipipsec_key   key;

        ipipsec_handle_to_key(sah, &key);
        cur = ipipsec_sa_lookup_exact(&key);
    }
    else
        cur = IP_NULL;

    /* Scan SA's (IPv4 and input first) */
    for (domain_index = 0; domain_index < 2; domain_index++)
        for (sadir = 0; sadir < 2; sadir++)
            for (sa = IPCOM_LIST_FIRST(&ipipsec.sa_head[domain_index][sadir]);
                 sa != IP_NULL;
                 sa = IPCOM_LIST_NEXT(&sa->sa_list))
                {
                    /* First fast-forward to the current SA */
                    if (cur != IP_NULL)
                    {
                        if (cur == sa)
                            cur = IP_NULL;
                        continue;
                    }

                    /* IP_IPPROTO_ESP, IP_IPPROTO_AH or 0 for any */
                    if (sah_next->protocol != 0
                        && sah_next->protocol != sa->key.proto)
                        continue;

                    /* 0,1 = input/output, 2 = any direction SA */
                    if (sah_next->direction != IPIPSEC_SADIR_ANY
                        && sah_next->direction != sa->key.direction)
                        continue;

                    /* IP_AF_INET or IP_AF_INET6 or 0 for any */
                    if (sah_next->domain
                        && sah_next->domain != sa->key.domain)
                        continue;

                    return sa;
                }

    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipipsec_flow_lex_get
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if 0
IP_STATIC Ipipsec_flow *
ipipsec_flow_lex_get(Ip_u32 id)
{
    Ipipsec_flow   *flow, *best;
    int     flowdir, domain_index;

    best = IP_NULL;

    /* Check flows (IPv4 and input first) */
    for (domain_index = 0; domain_index < 2; domain_index++)
        for (flowdir = 0; flowdir < 2; flowdir++)
            for (flow = IPCOM_LIST_FIRST(&ipipsec.flow_head[domain_index][flowdir]);
                 flow != IP_NULL;
                 flow = IPCOM_LIST_NEXT(&flow->flow_list))
            {
                ip_assert(flow->magic == IPIPSEC_FLOW_MAGIC);
                if ((id == 0 || flow->id > id) && (best == IP_NULL || flow->id < best->id))
                    best = flow;
            }

    return best;
}
#endif


/*
 *===========================================================================
 *                    ipipsec_flow_sort_get
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ipipsec_flow *
ipipsec_flow_sort_get(Ip_u32 id)
{
    Ipipsec_flow   *flow, *next;
    int             flowdir, domain_index;

    /* Check flows (IPv4 and input first) */
    for (domain_index = 0; domain_index < 2; domain_index++)
        for (flowdir = 0; flowdir < 2; flowdir++)
            for (flow = IPCOM_LIST_FIRST(&ipipsec.flow_head[domain_index][flowdir]);
                 flow != IP_NULL;
                 flow = next)
            {
                ip_assert(flow->magic == IPIPSEC_FLOW_MAGIC);

                next = IPCOM_LIST_NEXT(&flow->flow_list);
                if (id == 0)
                    return flow;
                if (id != flow->id)
                    continue;
                if (next)
                    return next;
                else
                    id = 0;
            }

    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipipsec_flow_to_selector
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipipsec_flow_to_selector(Ipipsec_flow *flow, Ipipsec_ctrl_selector *sel)
{
    ipcom_memset(sel, 0, sizeof(Ipipsec_ctrl_selector));

    sel->Index     = flow->id;
    sel->Protocol  = flow->flowsel.key.proto;
    sel->domain    = flow->flowsel.key.domain;
    sel->direction = flow->flowsel.key.direction;
    sel->flowtype  = flow->flowtype;
    sel->flowpri   = flow->flowsel.key.priority;

    switch (sel->Protocol)
    {
    case IP_IPPROTO_TCP:
    case IP_IPPROTO_UDP:
        sel->LocalPort     = flow->flowsel.key.ports.udptcp_srcdst_n[0];
        sel->LocalMaxPort  = ip_htons(flow->flowsel.src_max_port);
        sel->RemotePort    = flow->flowsel.key.ports.udptcp_srcdst_n[1];
        sel->RemoteMaxPort = ip_htons(flow->flowsel.dst_max_port);
        break;
    case IP_IPPROTO_ICMP:
    case IP_IPPROTO_ICMPV6:
        sel->ports[0]      = flow->flowsel.key.ports.icmp_typecode[0];
        sel->ports[1]      = flow->flowsel.key.ports.icmp_typecode[1];
        break;
    case IP_IPPROTO_MH:
        sel->ports[0]      = flow->flowsel.key.ports.mh_type[0];
        break;
    default:
        break;
    }
#ifdef IPCOM_USE_INET
    if (flow->flowsel.key.domain == IP_AF_INET)
    {
        sel->LocalId.in.s_addr     = flow->flowsel.key.src.in.s_addr;
        sel->LocalMaxId.in.s_addr  = flow->flowsel.src_max.in.s_addr;
        sel->RemoteId.in.s_addr    = flow->flowsel.key.dst.in.s_addr;
        sel->RemoteMaxId.in.s_addr = flow->flowsel.dst_max.in.s_addr;
        sel->local_mask.in.s_addr  = flow->flowsel.mask.src.in.s_addr;
        sel->remote_mask.in.s_addr = flow->flowsel.mask.dst.in.s_addr;
    }
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    if (flow->flowsel.key.domain == IP_AF_INET6)
    {
        ipcom_memcpy(&sel->LocalId.in6,     &flow->flowsel.key.src, sizeof(struct Ip_in6_addr));
        ipcom_memcpy(&sel->LocalMaxId.in6,  &flow->flowsel.src_max, sizeof(struct Ip_in6_addr));
        ipcom_memcpy(&sel->RemoteId.in6,    &flow->flowsel.key.dst, sizeof(struct Ip_in6_addr));
        ipcom_memcpy(&sel->RemoteMaxId.in6, &flow->flowsel.dst_max, sizeof(struct Ip_in6_addr));
        ipcom_memcpy(&sel->local_mask,      &flow->flowsel.mask.src, sizeof(struct Ip_in6_addr));
        ipcom_memcpy(&sel->remote_mask,     &flow->flowsel.mask.dst, sizeof(struct Ip_in6_addr));
    }
#endif /* IPCOM_USE_INET6 */

    if (flow->sa)
        ipipsec_sa_to_handle(flow->sa, &sel->sa);
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipipsec_conf_internal
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipipsec_conf_internal(Ip_tag *ctrl_tags)
{
    Ip_tag                  tagp[2];
    Ip_err                  retval;
#ifdef IPIPSEC_USE_DAEMONS
    Ipnet_sig *sig;

    sig = ipipsec_suspend_daemons();
    if (sig == IP_NULL)
        return IPCOM_ERR_NO_MEMORY;
    ipipsec_poll_control_pipe();
#endif

    for(;; ctrl_tags += 2)
    {
        /* Read the TAG value; need to copy since it may
         * be a pointer in another memory space */
        if (ipcom_memcpy(&tagp[0], ctrl_tags, sizeof(Ip_tag)) == IP_NULL)
        {
            IPIPSEC_LEAVE(IPCOM_ERR_INVALID_ARG);
        }

        if (tagp[0] == IP_TAG_END)
            break;

        /* Read data */
        if (ipcom_memcpy(&tagp[1], ctrl_tags + 1, sizeof(Ip_tag)) == IP_NULL)
        {
            IPIPSEC_LEAVE(IPCOM_ERR_INVALID_ARG);
        }

        /* What are we dealing with? */
        switch(tagp[0])
        {
        case IPIPSEC_CONF_GET_ENABLE_OUTFILTER:
            tagp[1] = ipipsec_conf_enable_outfilter;
            break;
        case IPIPSEC_CONF_SET_ENABLE_OUTFILTER:
            ipipsec_conf_enable_outfilter = tagp[1];
            break;
        case IPIPSEC_CONF_GET_ENABLE_INFILTER:
            tagp[1] = ipipsec_conf_enable_infilter;
            break;
        case IPIPSEC_CONF_SET_ENABLE_INFILTER:
            ipipsec_conf_enable_infilter = tagp[1];
            break;
        case IPIPSEC_CONF_GET_ENABLE_ESP:
            tagp[1] = ipipsec_conf_esp_enable;
            break;
        case IPIPSEC_CONF_SET_ENABLE_ESP:
            ipipsec_conf_esp_enable = tagp[1];
            break;
        case IPIPSEC_CONF_GET_ENABLE_AH:
            tagp[1] = ipipsec_conf_ah_enable;
            break;
        case IPIPSEC_CONF_SET_ENABLE_AH:
            ipipsec_conf_ah_enable = tagp[1];
            break;
        case IPIPSEC_CONF_GET_ALLOW_IPIP:
            tagp[1] = ipipsec_conf_ipip_allow;
            break;
        case IPIPSEC_CONF_SET_ALLOW_IPIP:
            ipipsec_conf_ipip_allow = tagp[1];
            break;
        default:
            IPIPSEC_LEAVE(IPCOM_ERR_UNSUPPORTED_TAG);
        }

        /* Restore value */
        ipcom_memcpy(ctrl_tags + 1, &tagp[1], sizeof(Ip_tag));
    }

    retval = IPCOM_SUCCESS;

leave:
#ifdef IPIPSEC_USE_DAEMONS
    ipipsec_resume_daemons(sig);
#endif
    return retval;
}


/*
 *===========================================================================
 *                    ipipsec_ctrl_internal
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipipsec_ctrl_internal(Ip_tag *ctrl_tags)
{
    Ip_tag                   tagp[2];
    Ip_err                   retval;
    Ipipsec_flow            *flow;
    Ipipsec_ctrl_selector    sel;
#ifdef IPIPSEC_USE_DAEMONS
    Ipnet_sig *sig;

    sig = ipipsec_suspend_daemons();
    if (sig == IP_NULL)
        return IPCOM_ERR_NO_MEMORY;
    ipipsec_poll_control_pipe();
#endif

    for(;; ctrl_tags += 2)
    {
        /* Read the TAG value; need to copy
         * since it may be a pointer in another memory space */
        if (ipcom_memcpy(&tagp[0], ctrl_tags, sizeof(Ip_tag)) == IP_NULL)
        {
            IPIPSEC_LEAVE(IPCOM_ERR_INVALID_ARG);
        }

        if (tagp[0] == IP_TAG_END)
            break;

        /* Read data */
        if (ipcom_memcpy(&tagp[1], ctrl_tags + 1, sizeof(Ip_tag)) == IP_NULL)
        {
            IPIPSEC_LEAVE(IPCOM_ERR_INVALID_ARG);
        }

        /* What are we dealing with? */
        switch(tagp[0])
        {
        case IPIPSEC_CTRLT_GET_SELECTOR :
            if (ipcom_memcpy(&sel, (void *)tagp[1], sizeof(sel)) == IP_NULL)
            {
                IPIPSEC_LEAVE(IPCOM_ERR_INVALID_ARG);
            }

            flow = ipipsec_flow_sort_get(sel.Index);
            if (flow == IP_NULL)
            {
                sel.Index = 0;
                ipcom_memcpy((void *)tagp[1], &sel, sizeof(sel));
                break;
            }

            ipipsec_flow_to_selector(flow, &sel);

            ipcom_memcpy((void *)tagp[1], &sel, sizeof(sel));
            break;

#if 0
        case IPIPSEC_CTRLT_SET_DEFAULT_TTL :
            if (tagp[1] > 255)
                IPIPSEC_LEAVE(IPCOM_ERR_INVALID_TAG_VALUE);
            else
                ipipsec.default_ttl = (Ip_u8)tagp[1];
            break;
#endif

#ifdef IPIPSEC_USE_MIB
        case IPIPSEC_CTRLT_GET_ipsecEspCurrentInboundSAs :
            tagp[1] = ipipsec.stats.ipsecEspCurrentInboundSAs;
            break;

        case IPIPSEC_CTRLT_GET_ipsecEspTotalInboundSAs :
            tagp[1] = ipipsec.stats.ipsecEspTotalInboundSAs;
            break;

        case IPIPSEC_CTRLT_GET_ipsecEspCurrentOutboundSAs :
            tagp[1] = ipipsec.stats.ipsecEspCurrentOutboundSAs;
            break;

        case IPIPSEC_CTRLT_GET_ipsecEspTotalOutboundSAs :
            tagp[1] = ipipsec.stats.ipsecEspTotalOutboundSAs;
            break;

        case IPIPSEC_CTRLT_GET_ipsecAhCurrentInboundSAs :
            tagp[1] = ipipsec.stats.ipsecAhCurrentInboundSAs;
            break;

        case IPIPSEC_CTRLT_GET_ipsecAhTotalInboundSAs :
            tagp[1] = ipipsec.stats.ipsecAhCurrentInboundSAs;
            break;

        case IPIPSEC_CTRLT_GET_ipsecAhCurrentOutboundSAs :
            tagp[1] = ipipsec.stats.ipsecAhCurrentOutboundSAs;
            break;

        case IPIPSEC_CTRLT_GET_ipsecAhTotalOutboundSAs :
            tagp[1] = ipipsec.stats.ipsecAhTotalOutboundSAs;
            break;

        case IPIPSEC_CTRLT_GET_ipsecDecryptionErrors :
            tagp[1] = ipipsec.stats.ipsecAhTotalOutboundSAs;
            break;

        case IPIPSEC_CTRLT_GET_ipsecAuthenticationErrors :
            tagp[1] = ipipsec.stats.ipsecAuthenticationErrors;
            break;

        case IPIPSEC_CTRLT_GET_ipsecReplayErrors :
            tagp[1] = ipipsec.stats.ipsecAuthenticationErrors;
            break;

        case IPIPSEC_CTRLT_GET_ipsecPolicyErrors :
            tagp[1] = ipipsec.stats.ipsecAuthenticationErrors;
            break;

        case IPIPSEC_CTRLT_GET_ipsecOtherReceiveErrors :
            tagp[1] = ipipsec.stats.ipsecOtherReceiveErrors;
            break;

        case IPIPSEC_CTRLT_GET_ipsecSendErrors :
            tagp[1] = ipipsec.stats.ipsecSendErrors;
            break;

        case IPIPSEC_CTRLT_GET_ipsecUnknownSpiErrors :
            tagp[1] = ipipsec.stats.ipsecUnknownSpiErrors;
            break;
#endif /* IPIPSEC_USE_MIB */

#ifdef IPIPSEC_USE_TRAPS
            /* trap function */
        case IPIPSEC_CTRLT_SET_TRAP_FUNCTION :
            ipipsec.trap_cb = (Ipipsec_trap_func)tagp[1];
            break;

            /* traps get */
        case IPIPSEC_CTRLT_GET_espAuthFailureTrapEnable :
            tagp[1] = ipipsec.enable_trap[IPIPSEC_TRAPTYPE_espAuthFailureTrap];
            break;

        case IPIPSEC_CTRLT_GET_ahAuthFailureTrapEnable :
            tagp[1] = ipipsec.enable_trap[IPIPSEC_TRAPTYPE_ahAuthFailureTrap];
            break;

        case IPIPSEC_CTRLT_GET_espReplayFailureTrapEnable :
            tagp[1] = ipipsec.enable_trap[IPIPSEC_TRAPTYPE_espReplayFailureTrap];
            break;

        case IPIPSEC_CTRLT_GET_ahReplayFailureTrapEnable :
            tagp[1] = ipipsec.enable_trap[IPIPSEC_TRAPTYPE_ahReplayFailureTrap];
            break;

        case IPIPSEC_CTRLT_GET_espPolicyFailureTrapEnable :
            tagp[1] = ipipsec.enable_trap[IPIPSEC_TRAPTYPE_espPolicyFailureTrap];
            break;

        case IPIPSEC_CTRLT_GET_ahPolicyFailureTrapEnable :
            tagp[1] = ipipsec.enable_trap[IPIPSEC_TRAPTYPE_ahPolicyFailureTrap];
            break;

        case IPIPSEC_CTRLT_GET_invalidSpiTrapEnable :
            tagp[1] = ipipsec.enable_trap[IPIPSEC_TRAPTYPE_invalidSpiTrap];
            break;

        case IPIPSEC_CTRLT_GET_otherPolicyFailureTrapEnable :
            tagp[1] = ipipsec.enable_trap[IPIPSEC_TRAPTYPE_otherPolicyFailureTrap];
            break;

            /* traps set */
        case IPIPSEC_CTRLT_SET_espAuthFailureTrapEnable :
            ipipsec.enable_trap[IPIPSEC_TRAPTYPE_espAuthFailureTrap] = (Ip_u32)tagp[1];
            break;

        case IPIPSEC_CTRLT_SET_ahAuthFailureTrapEnable :
            ipipsec.enable_trap[IPIPSEC_TRAPTYPE_ahAuthFailureTrap] = (Ip_u32)tagp[1];
            break;

        case IPIPSEC_CTRLT_SET_espReplayFailureTrapEnable :
            ipipsec.enable_trap[IPIPSEC_TRAPTYPE_espReplayFailureTrap] = (Ip_u32)tagp[1];
            break;

        case IPIPSEC_CTRLT_SET_ahReplayFailureTrapEnable :
            ipipsec.enable_trap[IPIPSEC_TRAPTYPE_ahReplayFailureTrap] = (Ip_u32)tagp[1];
            break;

        case IPIPSEC_CTRLT_SET_espPolicyFailureTrapEnable :
            ipipsec.enable_trap[IPIPSEC_TRAPTYPE_espPolicyFailureTrap] = (Ip_u32)tagp[1];
            break;

        case IPIPSEC_CTRLT_SET_ahPolicyFailureTrapEnable :
            ipipsec.enable_trap[IPIPSEC_TRAPTYPE_ahPolicyFailureTrap] = (Ip_u32)tagp[1];
            break;

        case IPIPSEC_CTRLT_SET_invalidSpiTrapEnable :
            ipipsec.enable_trap[IPIPSEC_TRAPTYPE_invalidSpiTrap] = (Ip_u32)tagp[1];
            break;

        case IPIPSEC_CTRLT_SET_otherPolicyFailureTrapEnable :
            ipipsec.enable_trap[IPIPSEC_TRAPTYPE_otherPolicyFailureTrap] = (Ip_u32)tagp[1];
            break;
#endif /* IPIPSEC_USE_TRAPS */

        default:
            IPIPSEC_LEAVE(IPCOM_ERR_UNSUPPORTED_TAG);
        }

        /* Restore value */
        ipcom_memcpy(ctrl_tags + 1, &tagp[1], sizeof(Ip_tag));
    }

    retval = IPCOM_SUCCESS;

 leave:
#ifdef IPIPSEC_USE_DAEMONS
    ipipsec_resume_daemons(sig);
#endif
    return retval;
}


/*
 *===========================================================================
 *                    ipipsec_sactrl_internal
 *===========================================================================
 * Context:
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipipsec_sactrl_internal(Ipipsec_sa_handle *sah, Ip_tag *ctrl_tags)
{
    Ip_tag              tagp[5];
    Ipipsec_sa         *sa;
    Ipipsec_sa         *sa_next;
    Ip_err              retval;
    Ipipsec_flow       *flow;
    Ipipsec_sa_handle   sah_next;
    Ip_tag             *next_tag;
#ifdef IPIPSEC_USE_DAEMONS
    Ipnet_sig *sig;

    sig = ipipsec_suspend_daemons();
    if (sig == IP_NULL)
        return IPCOM_ERR_NO_MEMORY;
    ipipsec_poll_control_pipe();
#endif

    /* Just to make certain that its in the memory space */
    if (ipcom_memcpy(&sah_next, sah, sizeof(sah_next)) == IP_NULL)
    {
        IPIPSEC_LEAVE(IPCOM_ERR_INVALID_ARG);
    }

    /* Get SA pointer for SA get/set */
    if (sah_next.spi_n)
    {
        Ipipsec_key  key;

        ipipsec_handle_to_key(&sah_next, &key);
        sa = ipipsec_sa_lookup_exact(&key);
    }
    else
        sa = IP_NULL;

    /* Parse the vtags */
    for (next_tag = ctrl_tags;; next_tag += 2)
    {
        Ip_size_t   szu = sizeof(Ip_tag);

        /* Go to next tag */
        ctrl_tags = next_tag;

        /* Read the TAG value; need to copy since it may
         * be a pointer in another memory space */
        if (ipcom_memcpy(&tagp[0], ctrl_tags, sizeof(Ip_tag)) == IP_NULL)
        {
            IPIPSEC_LEAVE(IPCOM_ERR_INVALID_ARG);
        }

        if (tagp[0] == IP_TAG_END)
            break;


        if (tagp[0] > IPIPSEC_SACTRLT_GET_NEXT_SA && tagp[0] <= IPIPSEC_SACTRLT_GET_LAST)
        {
            if (sa == IP_NULL)
                IPIPSEC_LEAVE(IPCOM_ERR_INVALID_ARG);
        }

        /* Read data */
        if (ipcom_memcpy(&tagp[1], ctrl_tags + 1, sizeof(Ip_tag)) == IP_NULL)
        {
            IPIPSEC_LEAVE(IPCOM_ERR_INVALID_TAG_VALUE);
        }

        switch(tagp[0])
        {
            /* This tag returns immediately after processing since it updates the 'sa' */
        case IPIPSEC_SACTRLT_GET_NEXT_SA:
            if (ipcom_memcpy(&sah_next, (void *)tagp[1], sizeof(sah_next)) == IP_NULL)
            {
                IPIPSEC_LEAVE(IPCOM_ERR_INVALID_ARG);
            }

            sa_next = ipipsec_sa_lex_get(sah, &sah_next);
            if (sa_next != IP_NULL)
                ipipsec_sa_to_handle(sa_next, &sah_next);
            else
                sah_next.spi_n = 0;
            ipcom_memcpy((void *)tagp[1], &sah_next, sizeof(sah_next));
            break;

        case IPIPSEC_SACTRLT_GET_SaSelector:
            {
                Ipcom_list *list_element;

                ip_assert(sa);

                /* In MIB, only first selector is listed. Pick the output one first */
                list_element = IPCOM_LIST_FIRST(&sa->flow_head[IPIPSEC_DIR_OUT]);
                if (list_element == IP_NULL)
                    list_element = IPCOM_LIST_FIRST(&sa->flow_head[IPIPSEC_DIR_IN]);
                flow = IPIPSEC_LIST_ENTRY(list_element, Ipipsec_flow, sa_list);
                ip_assert(flow == IP_NULL || flow->magic == IPIPSEC_FLOW_MAGIC);

                if (flow)
                    tagp[1] = flow->id;
                else
                    tagp[1] = 0;
            }
            break;

        case IPIPSEC_SACTRLT_GET_SaCreator:
            tagp[1] = 1;  /* 0=unknown,1=static,2=ike,3=other */
            break;

        case IPIPSEC_SACTRLT_GET_Encapsulation:
            ip_assert(sa);
            if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_TUNNEL))
                tagp[1] = 1;  /* 0=reserved,1=tunnel,2=transport */
            else
                tagp[1] = 2;
            break;

        case IPIPSEC_SACTRLT_GET_EncAlg:
            ip_assert(sa);
            switch(sa->sadb_sa_encrypt)
            {
            case SADB_EALG_DESCBC:
                if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_HALFIV))
                    tagp[1] = 1;
                else
                    tagp[1] = 2;
                break;
            case SADB_EALG_3DESCBC:
                tagp[1] = 3;
                break;
            case SADB_X_EALG_BLF:
                tagp[1] = 7;
                break;
            case SADB_X_EALG_CAST:
                tagp[1] = 6;
                break;
            case SADB_X_EALG_AESCBC:
                tagp[1] = 12;
                break;
            case SADB_X_EALG_AESCTR:
                tagp[1] = 13;
                break;
            case SADB_EALG_NONE:
            default:
                tagp[1] = 11; /* espNull */
                break;
            }
            break;

        case IPIPSEC_SACTRLT_GET_EncKeyLength:
            ip_assert(sa);
            tagp[1] = sa->encrypt ? (sa->encrypt->bytes << 3) : 0;
            break;

        case IPIPSEC_SACTRLT_GET_AuthAlg:
            switch(sa->sadb_sa_auth)
            {
            case SADB_AALG_MD5HMAC96:
                tagp[1] = 1;
                break;
            case SADB_AALG_SHA1HMAC96:
                tagp[1] = 2;
                break;
            case SADB_X_AALG_RIPEMD160HMAC96:
                tagp[1] = 5;  /*!! non-defined value */
                break;
            case SADB_X_AALG_MD5:
                tagp[1] = 4;
                break;
            case SADB_X_AALG_SHA1:
                tagp[1] = 4;
                break;
            case SADB_X_AALG_AESXCBCMAC96:
                tagp[1] = 9;
                break;
            case SADB_AALG_NONE:
            default:
                tagp[1] = 0;
                break;
            }
            break;

        case IPIPSEC_SACTRLT_GET_AuthKeyLength:
            ip_assert(sa);
            tagp[1] = sa->auth ? (sa->auth->keyLen << 3) : 0;
            break;

        case IPIPSEC_SACTRLT_GET_RepWinSize:
            ip_assert(sa);
            tagp[1] = sa->sadb_sa_replay;
            break;

        case IPIPSEC_SACTRLT_GET_LimitSeconds:
            tagp[1] = sa->lthard.addtime;
            break;

        case IPIPSEC_SACTRLT_GET_LimitKbytes:
            tagp[1] = 0;  /*!!auto*/
            break;

        case IPIPSEC_SACTRLT_GET_AccSeconds:
            if (sa->ltcurrent.usetime)
                tagp[1] = ipcom_time(0) - sa->ltcurrent.addtime;
            else
                tagp[1] = 0;
            break;

        case IPIPSEC_SACTRLT_GET_InAccKbytes:
            tagp[1]  = (IP_U64_GETLO(sa->bytes[IPIPSEC_DIR_IN]) >> 10);
            tagp[1] += (IP_U64_GETHI(sa->bytes[IPIPSEC_DIR_IN]) << 22);
            break;

        case IPIPSEC_SACTRLT_GET_OutAccKbytes:
            tagp[1]  = (IP_U64_GETLO(sa->bytes[IPIPSEC_DIR_OUT]) >> 10);
            tagp[1] += (IP_U64_GETHI(sa->bytes[IPIPSEC_DIR_OUT]) << 22);
            break;

        case IPIPSEC_SACTRLT_GET_InUserOctets:
            ip_assert(sa);
            ipcom_memcpy((void *)tagp[1], &sa->bytes[IPIPSEC_DIR_IN], sizeof(Ip_u64));
            break;

        case IPIPSEC_SACTRLT_GET_OutUserOctets:
            ip_assert(sa);
            ipcom_memcpy((void *)tagp[1], &sa->bytes[IPIPSEC_DIR_OUT], sizeof(Ip_u64));
            break;

        case IPIPSEC_SACTRLT_GET_InPackets:
            ip_assert(sa);
            ipcom_memcpy((void *)tagp[1], &sa->packets[IPIPSEC_DIR_IN], sizeof(Ip_u64));
            break;

        case IPIPSEC_SACTRLT_GET_OutPackets:
            ip_assert(sa);
            ipcom_memcpy((void *)tagp[1], &sa->packets[IPIPSEC_DIR_OUT], sizeof(Ip_u64));
            break;

#ifdef IPIPSEC_USE_MIB
        case IPIPSEC_SACTRLT_GET_DecryptErrors:
            ip_assert(sa);
            tagp[1] = sa->err_in_decrypt;
            break;

        case IPIPSEC_SACTRLT_GET_AuthErrors:
            ip_assert(sa);
            tagp[1] = sa->err_in_auth;
            break;

        case IPIPSEC_SACTRLT_GET_ReplayErrors:
            ip_assert(sa);
            tagp[1] = sa->err_in_replay;
            break;

        case IPIPSEC_SACTRLT_GET_PolicyErrors:
            ip_assert(sa);
            tagp[1] = sa->err_in_policy;
            break;

        case IPIPSEC_SACTRLT_GET_PadErrors:
            ip_assert(sa);
            tagp[1] = sa->err_in_pad;
            break;

        case IPIPSEC_SACTRLT_GET_OtherReceiveErrors:
            ip_assert(sa);
            tagp[1] = sa->err_in_other;
            break;

        case IPIPSEC_SACTRLT_GET_OutSendErrors:
            ip_assert(sa);
            tagp[1] = sa->err_out_send;
            break;
#endif /* IPIPSEC_USE_MIB */

        case IPIPSEC_SACTRLT_GET_Grouped:
            ip_assert(sa);
            if (ipcom_memcpy(&sah_next, (void *)tagp[1], sizeof(sah_next)) == IP_NULL)
            {
                IPIPSEC_LEAVE(IPCOM_ERR_INVALID_ARG);
            }

            if (sa->group_next != IP_NULL)
                ipipsec_sa_to_handle(sa->group_next, &sah_next);
            else
                sah_next.spi_n = 0;
            ipcom_memcpy((void *)tagp[1], &sah_next, sizeof(sah_next));
            break;

        case IPIPSEC_SACTRLT_GET_DstAddr:
            ip_assert(sa);
            tagp[1] = sa->key.dst.in.s_addr;
            break;

        case IPIPSEC_SACTRLT_GET_SrcAddr:
            ip_assert(sa);
            tagp[1] = sa->key.src.in.s_addr;
            break;

        case IPIPSEC_SACTRLT_GET_State:
            ip_assert(sa);
            tagp[1] = sa->sadb_sa_state;
            break;

#ifdef IPCOM_USE_INET6
        case IPIPSEC_SACTRLT_GET_DstAddr6:
        case IPIPSEC_SACTRLT_GET_SrcAddr6:
            ip_assert(sa);
            if (sa->key.domain != IP_AF_INET6)
                IPIPSEC_LEAVE(IPCOM_ERR_FAILED);
            if (*tagp == IPIPSEC_SACTRLT_GET_DstAddr6)
                ipcom_memcpy(&tagp[1], &sa->key.dst, 16);
            else
                ipcom_memcpy(&tagp[1], &sa->key.src, 16);
            /* Advance three more tags */
            next_tag += 3;
            szu      = 16;
            break;
#endif /* IPCOM_USE_INET6 */

        default:
            IPIPSEC_LEAVE(IPCOM_ERR_UNSUPPORTED_TAG);
        }

        /* Restore value */
        ipcom_memcpy(ctrl_tags + 1, &tagp[1], szu);
    }

    retval = IPCOM_SUCCESS;

 leave:
#ifdef IPIPSEC_USE_DAEMONS
    ipipsec_resume_daemons(sig);
#endif
    return retval;
}
#endif /* !IP_PORT_LAS */


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipipsec_sactrl
 *===========================================================================
 * Context:
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipipsec_sactrl(Ipipsec_sa_handle *sah, Ip_tag *ctrl_tags)
{
#if defined(IPNET)
    Ip_fd      pfd = IP_INVALID_SOCKET;
    int        err = 0;
    void      *ptr[2];

    /* The arguments */
    ptr[0] = sah;
    ptr[1] = ctrl_tags;

    /* Get a socket */
    if ((pfd = ipcom_socket(IP_PF_KEY, IP_SOCK_RAW, PF_KEY_V2)) == IP_INVALID_SOCKET)
        return ipcom_errno;

    /* Do the sockopt */
    if (ipcom_socketioctl(pfd, IP_SIOCXIPSEC_SA_CTL, ptr) < 0)
        err = ipcom_errno;

    ipcom_socketclose(pfd);
    return err;
#elif defined(IPLITE)
    Ip_err ret;

    (void)ipcom_ipsec_ioevent(IPIPSEC_EIOXCODE_LOCK, IP_NULL);
    ret = (int)ipipsec_sactrl_internal(sah, ctrl_tags);
    (void)ipcom_ipsec_ioevent(IPIPSEC_EIOXCODE_UNLOCK, IP_NULL);
    return ret;
#else
    return -IP_ERRNO_EINVAL
#endif
}


/*
 *===========================================================================
 *                    ipipsec_ctrl
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipipsec_ctrl(Ip_tag *ctrl_tags)
{
#if defined(IPNET)
    Ip_fd       pfd;
    int         err = 0;
    void       *ptr   = ctrl_tags;

    /* Get a socket to do the ioctl */
    if ((pfd = ipcom_socket(IP_PF_KEY, IP_SOCK_RAW, PF_KEY_V2)) == IP_INVALID_SOCKET)
        return ipcom_errno;

    /* Do the sockopt */
    if (ipcom_socketioctl(pfd, IP_SIOCXIPSEC_CTL, &ptr) < 0)
        err = ipcom_errno;

    ipcom_socketclose(pfd);
    return err;
#elif defined(IPLITE)
    Ip_err ret;

    (void)ipcom_ipsec_ioevent(IPIPSEC_EIOXCODE_LOCK, IP_NULL);
    ret = ipipsec_ctrl_internal(ctrl_tags);
    (void)ipcom_ipsec_ioevent(IPIPSEC_EIOXCODE_UNLOCK, IP_NULL);
    return ret;
#else
    return -IP_ERRNO_EINVAL
#endif
}


/*
 *===========================================================================
 *                    ipipsec_ctrl
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipipsec_conf(Ip_tag *ctrl_tags)
{
#if defined(IPNET)
    Ip_fd    pfd;
    int      err = 0;
    void    *ptr   = ctrl_tags;

    /* Get a socket to do the ioctl */
    if ((pfd = ipcom_socket(IP_PF_KEY, IP_SOCK_RAW, PF_KEY_V2)) == IP_INVALID_SOCKET)
        return ipcom_errno;

    /* Do the sockopt */
    if (ipcom_socketioctl(pfd, IP_SIOCXIPSEC_CONF_CTL, &ptr) < 0)
        err = ipcom_errno;

    ipcom_socketclose(pfd);
    return err;
#elif defined(IPLITE)
    Ip_err ret;

    (void)ipcom_ipsec_ioevent(IPIPSEC_EIOXCODE_LOCK, IP_NULL);
    ret = ipipsec_conf_internal(ctrl_tags);
    (void)ipcom_ipsec_ioevent(IPIPSEC_EIOXCODE_UNLOCK, IP_NULL);
    return ret;
#else
    return -IP_ERRNO_EINVAL
#endif
}


#ifndef IP_PORT_LAS
/*
 *===========================================================================
 *                    ipipsec_sys_ioctl
 *===========================================================================
 * Description: IO control.
 * Parameters:  fd - A socket descriptor.
 *              request - The IO control request.
 *              argp - Request specific data.
 * Returns:     0 = success, -1 = error (error code from ipcom_errno).
 *
 */
IP_PUBLIC int
ipipsec_sys_ioctl(Ip_fd fd, unsigned long request, void *argp)
{
    int                    ret = 0;
    Ipipsec_sa            *sa = IP_NULL;
    Ipipsec_ioctl_sactrl  *isa = argp;

#ifdef IPIPSEC_USE_DAEMONS
    Ipnet_sig *sig;

    sig = ipipsec_suspend_daemons();
    if (sig == IP_NULL)
        return -IP_ERRNO_ENOMEM;
    ipipsec_poll_control_pipe();
#endif

    IPCOM_UNUSED_ARG(fd);

    ip_assert(isa != IP_NULL);

    switch(request)
    {
    case IP_SIOCXIPSEC_Grouped:
    case IP_SIOCXIPSEC_InUserOctets:
    case IP_SIOCXIPSEC_OutUserOctets:
    case IP_SIOCXIPSEC_InUserPackets:
    case IP_SIOCXIPSEC_OutUserPackets:
        /* Get SA pointer for SA get/set */
        if (isa->sa.spi_n)
        {
            Ipipsec_key  key;

            ipipsec_handle_to_key(&isa->sa, &key);
            sa = ipipsec_sa_lookup_exact(&key);
        }
        if (sa == IP_NULL)
        {
            ret = -IP_ERRNO_ESRCH;
            goto leave;
        }
        break;

    default:
        break;
    }

    switch (request)
    {
    case IP_SIOCXIPSEC_SELECTOR:
        {
            Ipipsec_ctrl_selector *sel = argp;
            Ipipsec_flow          *flow;

            flow = ipipsec_flow_sort_get(sel->Index);
            if (flow == IP_NULL)
                sel->Index = 0;
            else
                ipipsec_flow_to_selector(flow, sel);
        }
        break;

    case IP_SIOCXIPSEC_NEXT_SA:
        {
            Ipipsec_sa   *sa_next;

            sa_next = ipipsec_sa_lex_get(&isa->sa, &isa->arg.next);
            if (sa_next != IP_NULL)
                ipipsec_sa_to_handle(sa_next, &isa->arg.next);
            else
                isa->arg.next.spi_n = 0;
        }
        break;

    case IP_SIOCXIPSEC_Grouped:
        ip_assert(sa != IP_NULL);
        if (sa->group_next != IP_NULL)
            ipipsec_sa_to_handle(sa->group_next, &isa->arg.group);
        else
            isa->arg.group.spi_n = 0;
        break;

    case IP_SIOCXIPSEC_InUserOctets:
        ip_assert(sa);
        isa->arg.u64 = sa->bytes[IPIPSEC_DIR_IN];
        break;
    case IP_SIOCXIPSEC_OutUserOctets:
        ip_assert(sa);
        isa->arg.u64 = sa->bytes[IPIPSEC_DIR_OUT];
        break;
    case IP_SIOCXIPSEC_InUserPackets:
        ip_assert(sa);
        isa->arg.u64 = sa->packets[IPIPSEC_DIR_IN];
        break;
    case IP_SIOCXIPSEC_OutUserPackets:
        ip_assert(sa);
        isa->arg.u64 = sa->packets[IPIPSEC_DIR_OUT];
        break;

    default:
        ret = -IP_ERRNO_EINVAL;
        break;
    }

 leave:
#ifdef IPIPSEC_USE_DAEMONS
    ipipsec_resume_daemons(sig);
#endif

    return ret;
}
#endif /* !IP_PORT_LAS */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
