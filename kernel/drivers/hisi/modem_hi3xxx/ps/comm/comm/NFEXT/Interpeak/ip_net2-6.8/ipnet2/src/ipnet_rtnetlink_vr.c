/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_rtnetlink_vr.c,v $ $Revision: 1.6 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_rtnetlink_vr.c,v $
 *   $Author: jonas $
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

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */
typedef struct Ipnet_rtnetlink_vr
{
    struct
    {
        Ip_ptrdiff_t    *vr;
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
Ipnet_rtnetlink_vr_t;

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
IP_STATIC void ipnet_netlink_generate_vrtab_event(Ip_u16 vr, Ip_u32 table,
                                                  Ip_u8 event);

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
 *                    ipnet_rtnetlink_ip4_neigh_fill_info
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
ipnet_rtnetlink_vr_fill_info(Ipnet_netlink_mem_t *mem,
                             int                 vr,
                             int                 seq,
                             Ip_u32              table,
                             Ip_u8               event,
                             Ip_u32              eflags)
{
    struct Ip_nlmsghdr        *nlh;
    struct Ip_vrmsg           *vrm;
    struct Ip_rtattr          *rta;
#ifdef IPNET_USE_ROUTE_TABLE_NAMES
    char                      name[IPCOM_RTNAMSIZ];
#endif /* IPNET_USE_ROUTE_TABLE_NAMES */
    unsigned                  num = 1;
    Ip_u32                    table_id;
    Ip_u32                    *table_ids = &table_id;
    unsigned                  i;
    int                       ret = -IP_ERRNO_ENOMEM;
    Ipnet_netif               *netif;

    nlh = ipnet_netlink_nlmsg_sz_begin(mem, seq, event, eflags);
    if (!nlh)
        return -1;

    vrm = ipnet_netlink_sz_put(mem, IP_NULL, sizeof (*vrm));
    if (!vrm)
        return -1;

    /* Setup the neigh header */
    vrm->vr_family   = IP_AF_PACKET;
    vrm->vr_vr       = vr;

    /* Do the tables */
    (void)ipnet_route_get_table_ids(vr, table_ids, &num);
    if (num != 1)
    {
        /* More than one table in this VR, need to dynamically
           allocate a buffer that can hold all FIBs. */
        table_ids = ipcom_calloc(num, sizeof(*table_ids));
        if (table_ids == IP_NULL)
            goto exit_out;

        (void) ipnet_route_get_table_ids(vr, table_ids, &num);
    }

    if (num > 0)
    {
        rta = ipnet_rtnetlink_rta_sz_nested_begin(mem, IP_VR_TABLES);
        if (!rta)
            goto exit_out;

        for (i = 0; i < num; i++)
        {
            /**/
            if (table != IPCOM_ROUTE_TABLE_UNSPEC && table_ids[i] != table)
                continue;

            /* Put route table index */
            if (!ipnet_rtnetlink_rta_sz_put(mem, IP_VRX_TABLE, sizeof(table_ids[i]), &table_ids[i]))
                goto exit_out;
#ifdef IPNET_USE_ROUTE_TABLE_NAMES
            /* Put table name */
            if (ipnet_route_get_name(vr, table_ids[i], name))
            {
                if (!ipnet_rtnetlink_rta_sz_put(mem, IP_VRX_TABLE_NAME, ipcom_strlen(name) + 1, name))
                    goto exit_out;
            }
#endif /* IPNET_USE_ROUTE_TABLE_NAMES */

        }

        /* Terminate the nested RTA */
        ipnet_rtnetlink_rta_sz_nested_end(mem, rta);
    }

    rta = IP_NULL;
    IPNET_NETIF_FOR_EACH_ON_VR(netif, vr, i)
    {
        if (rta == IP_NULL)
        {
            rta = ipnet_rtnetlink_rta_sz_nested_begin(mem, IP_VR_IFNAMES);
            if (!rta)
                goto exit_out;
        }

        if (!ipnet_rtnetlink_rta_sz_put(mem, IP_VRX_IFNAME, ipcom_strlen(netif->ipcom.name) + 1, netif->ipcom.name))
            goto exit_out;

    }
    /* Terminate the nested RTA */
    if (rta)
        ipnet_rtnetlink_rta_sz_nested_end(mem, rta);


    ret = ipnet_netlink_nlmsg_sz_end(mem, nlh);

exit_out:
    if (table_ids && table_ids != &table_id)
        ipcom_free(table_ids);
    return ret;
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
ipnet_rtnetlink_vr_foreach_vr(Ipcom_route *rtab, Ipnet_rtnetlink_vr_t *param)
{
    Ip_u16             vr       = IPNET_ROUTE_GET_VR(rtab);

    /* This traversal is already aborted */
    if (param->state.aborted)
        return;

    /* Only handle rtabs of selected address family */
#ifdef IPCOM_USE_INET
    if (IPNET_ROUTE_GET_FAMILY(rtab) != IP_AF_INET)
        return;
#elif IPCOM_USE_INET6
    if (IPNET_ROUTE_GET_FAMILY(rtab) != IP_AF_INET6)
        return;
#else
    return
#endif

    /* Go for the defaults to get current VR's */
    if (IPNET_ROUTE_GET_TABLE(rtab) != IPCOM_ROUTE_TABLE_DEFAULT)
        return;

    /*
     * If this is a trailer message buffer, make sure it
     * starts dumping where the last message ended
     */
    if (param->state.cont)
    {
        /* Verify current state */
        if (*param->state.vr != vr)
            return;
    }


    /* Reset the continue variable; we're on the move again */
    param->state.cont = 0;

    /* Time to start dump info */
    if (ipnet_rtnetlink_vr_fill_info(param->mem, vr, param->nlmsg->nlmsg_seq, IPCOM_ROUTE_TABLE_UNSPEC, IP_RTM_GETVR, IP_NLM_F_MULTI) >= 0)
        return;

    param->state.aborted = 1;
    *param->state.vr     = (Ip_ptrdiff_t)vr;
}

/*
 *===========================================================================
 *                    ipnet_netlink_generate_vrtab_event
 *===========================================================================
 * Description: Creates a NETLINK route message and passes it to the NETLINK
 *              subsystem.
 * Parameters:  vr    - virtual route table
 *              table - route table
 *              event - Event type
 * Returns:     -
 *
 */
IP_STATIC void
ipnet_netlink_generate_vrtab_event(Ip_u16 vr, Ip_u32 table, Ip_u8 event)
{
    Ipcom_pkt               *pkt    = IP_NULL;
    Ipnet_netlink_mem_t     mem;
    int                     ret     = -1;

    if (!ipnet_netlink_pkt_alloc(&pkt, IP_NULL, IP_TRUE, &mem, 512))
        return;

    /* Must explicitly switch to the correct VR here */
    pkt->vr_index = mem.vr = vr;

    /* Time to start dump info */
    ret = ipnet_rtnetlink_vr_fill_info(&mem,
                                       vr,
                                       0,
                                       table,
                                       event,
                                       0);
    if (ret < 0)
        ipcom_pkt_free(pkt);
    else
    {
        pkt->end += ret;

        /* We're done; do report this pkt */
        ipnet_rtnetlink_broadcast(pkt, IP_RTNLGRP_RTAB);
    }
}

/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */
/*
 *===========================================================================
 *                    ipnet_netlinksock_vrtab_add
 *===========================================================================
 * Description: Called by the stack when a new virtual router / table is
 *              created.
 * Parameters:  vr    - The virtual router index
 *              table - The table index
 * Returns:     -
 *
 */
IP_GLOBAL void
ipnet_netlinksock_vrtab_add(Ip_u16 vr, Ip_u32 table)
{
    ipnet_netlink_generate_vrtab_event(vr, table, IP_RTM_NEWVR);
}


/*
 *===========================================================================
 *                    ipnet_netlinksock_vrtab_del
 *===========================================================================
 * Description: Called by the stack when a virtual router / table is
 *              deleted.
 * Parameters:  vr    - The virtual router index
 *              table - The table index
 * Returns:     -
 *
 */
IP_GLOBAL void
ipnet_netlinksock_vrtab_del(Ip_u16 vr, Ip_u32 table)
{
    ipnet_netlink_generate_vrtab_event(vr, table, IP_RTM_DELVR);
}


/*
 *===========================================================================
 *                    ipnet_rtnetlink_vr_newvr
 *===========================================================================
 * Description: Add a new route table or interface to route table mapping
 *              via NETLINK
 * Parameters:  data - NETLINK message payload area.
 *              len  - Length of message.
 *              nlh  - NETLINK message header
 *              arg  - Reference to attributes
 *              s_vr - Callers VR context.
 *
 * Returns:     0 - Success
 *             <0 - Failure
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_vr_newvr_family(Ipnet_netlink_mem_t *mem,
                                struct Ip_nlmsghdr  *nlh,
                                struct Ip_rtattr    **rta,
                                int                 family)
{
    Ip_u32              table = IPCOM_ROUTE_TABLE_DEFAULT;
    int                 ret   = 0;
    struct Ip_vrmsg     *vrm;
    Ip_u16              vr;

    (void)family;

    vrm = IP_NLMSG_DATA(nlh);

    vr = vrm->vr_vr;

    /* Current vr must be valid */
    if (mem->vr == IPCOM_VR_ANY || vr == IPCOM_VR_ANY)
        return -IP_ERRNO_EINVAL;


    /* Create the virtual router if it doesn't exist */
    if (!ipnet_route_is_virtual_router_valid(vrm->vr_vr))
    {
        /* Create virtual router */
        ret = ipnet_route_add_table(vr, table, &vr, &table);
        if (ret != 0 && ret != -IP_ERRNO_EEXIST)
            return ret;
    }

    if (rta[IP_VR_TABLE - 1])
    {
        table = IP_GET_32ON8(IP_RTA_DATA(rta[IP_VR_TABLE - 1]));

        /* Create virtual router */
        ret = ipnet_route_add_table(vr, table, &vr, &table);
        if (ret != 0 && ret != -IP_ERRNO_EEXIST)
            return ret;

#ifdef IPNET_USE_ROUTE_TABLE_NAMES
        if (rta[IP_VR_TABLE_NAME -1])
        {
            /* Table now exists, but has no name. Set it. */
            ret = ipnet_route_set_name(vr, table, IP_RTA_DATA(rta[IP_VR_TABLE_NAME -1]));
            if (ret != 0 && ret != -IP_ERRNO_EEXIST)
                return ret;
        }
#endif /* IPNET_USE_ROUTE_TABLE_NAMES */
    }

    return ret;
}

/*
 *===========================================================================
 *                    ipnet_rtnetlink_vr_changevr
 *===========================================================================
 * Description: Add a new route table or interface to route table mapping
 *              via NETLINK
 * Parameters:  data - NETLINK message payload area.
 *              len  - Length of message.
 *              nlh  - NETLINK message header
 *              arg  - Reference to attributes
 *              s_vr - Callers VR context.
 *
 * Returns:     0 - Success
 *             <0 - Failure
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_vr_changevr_family(Ipnet_netlink_mem_t  *mem,
                                   struct Ip_nlmsghdr   *nlh,
                                   struct Ip_rtattr     **rta,
                                   int                  family)
{
    Ipnet_netif         *netif = IP_NULL;
    struct Ip_vrmsg     *vrm;
    Ip_u16              vr;
    Ip_u16              ifvr;

    (void)family;

    vrm = IP_NLMSG_DATA(nlh);

    vr      = vrm->vr_vr;
    ifvr    = IPCOM_VR_ANY;

    /* Current vr must be valid */
    if (mem->vr == IPCOM_VR_ANY || vr == IPCOM_VR_ANY)
        return -IP_ERRNO_EINVAL;

    if (rta[IP_VR_IFNAME - 1] == IP_NULL)
        return -IP_ERRNO_EINVAL;

    /* Extract destination virtual router index */
    if (rta[IP_VR_VR - 1])
        ifvr = (Ip_u16)IP_GET_32ON8(IP_RTA_DATA(rta[IP_VR_VR - 1]));

    /* Move interface from current vr to destination vr */
    netif = ipnet_if_nametonetif(ifvr, IP_RTA_DATA(rta[IP_VR_IFNAME - 1]));
    if (netif == IP_NULL)
        return -IP_ERRNO_ENXIO;

    if (IP_BIT_ISSET(netif->ipcom.flags,IP_IFF_UP))
    {
        IPCOM_LOG3(NOTICE, "Failed to move interface %s from VR %u to VR %u since the interface is UP",
                   netif->ipcom.name,
                   netif->vr_index,
                   vr);
        return -IP_ERRNO_EPERM;
    }

    return ipnet_ioctl_move_if_rt_to_vr(netif, vr);
}

/*
 *===========================================================================
 *                    ipnet_rtnetlink_vr_changevr
 *===========================================================================
 * Description: Add a new route table or interface to route table mapping
 *              via NETLINK
 * Parameters:  data - NETLINK message payload area.
 *              len  - Length of message.
 *              nlh  - NETLINK message header
 *              arg  - Reference to attributes
 *              s_vr - Callers VR context.
 *
 * Returns:     0 - Success
 *             <0 - Failure
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_vr_getvr_family(Ipnet_netlink_mem_t *mem,
                                struct Ip_nlmsghdr  *nlh,
                                struct Ip_rtattr    **rta,
                                int                 family)
{
    struct Ip_vrmsg     *vrm;
    Ip_u32              table   = IPCOM_ROUTE_TABLE_UNSPEC;
    Ip_u16              vr;

    (void)family;

    vrm = IP_NLMSG_DATA(nlh);

    vr = vrm->vr_vr;

#ifdef IPNET_USE_ROUTE_TABLE_NAMES
    if (rta[IP_NDA_TABLE_NAME -1])
    {
        /* Extract router and table from name */
        if (ipnet_route_vr_and_table_from_name(IP_RTA_DATA(rta[IP_VR_TABLE_NAME - 1]),
                                               &vr,
                                               &table) < 0)
            return -IP_ERRNO_ESRCH;
    }
#endif /* IPNET_USE_ROUTE_TABLE_NAMES */

    /* Current vr must be valid */
    if (mem->vr == IPCOM_VR_ANY || vr == IPCOM_VR_ANY)
        return -IP_ERRNO_EINVAL;

    if (!ipnet_route_is_virtual_router_valid(vr))
        return -IP_ERRNO_ESRCH;

    /* Time to start dump info */
    return ipnet_rtnetlink_vr_fill_info(mem, vr, nlh->nlmsg_seq, table, IP_RTM_GETVR, 0);
}


/*
 *===========================================================================
 *                    ipnet_rtnetlink_vr_delvr
 *===========================================================================
 * Description: Delete a route table rule via NETLINK
 * Parameters:  data - NETLINK message payload area
 *              len  - Message length
 *              nlh  - NETLINK message header
 *              arg  - Reference to attributes
 *              s_vr - Callers VR context.
 *
 * Returns:     0 - Success
 *             <0 - Failure
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_vr_delvr_family(Ipnet_netlink_mem_t *mem,
                                struct Ip_nlmsghdr  *nlh,
                                struct Ip_rtattr    **rta,
                                int                 family)
{
    Ip_u32              table = IPCOM_ROUTE_TABLE_DEFAULT;
    struct Ip_vrmsg     *vrm;
    Ip_u16              vr;
    int                 ret = 0;

    (void)family;

    vrm = IP_NLMSG_DATA(nlh);

    vr      = vrm->vr_vr;

    /* Current vr must be valid */
    if (mem->vr == IPCOM_VR_ANY || vr == IPCOM_VR_ANY)
        return -IP_ERRNO_EINVAL;

    /* Extract route table id */
    if (rta[IP_VR_TABLE - 1])
        table = IP_GET_32ON8(IP_RTA_DATA(rta[IP_VR_TABLE - 1]));
#ifdef IPNET_USE_ROUTE_TABLE_NAMES
    /* Extract table by name */
    if (rta[IP_VR_TABLE_NAME - 1])
    {
        char  *name = IP_RTA_DATA(rta[IP_VR_TABLE_NAME - 1]);
        Ip_u16 tmp_vr = 0;
        Ip_u32 tmp_table = IPCOM_ROUTE_TABLE_DEFAULT;

        if (ipnet_route_vr_and_table_from_name(name , &tmp_vr, &tmp_table) < 0)
            return -IP_ERRNO_ESRCH;

        if (rta[IP_VR_TABLE - 1])
        {
            if ((tmp_vr != vr) || (tmp_table != table))
                return -IP_ERRNO_ESRCH;

            ipnet_route_delete_name(name);
            return 0; /* Success */
        }

        table = tmp_table;
        vr    = tmp_vr;
    }
#endif /* IP_USE_ROUTE_TABLE_NAMES */

    /* Delete vr:table */
    if (table != IPCOM_ROUTE_TABLE_DEFAULT)
        ret = ipnet_route_delete_table(vr, table);
    else
    {
        table = IPCOM_ROUTE_TABLE_ANY;
        ret = ipnet_route_delete_vr(vr);
    }
#ifdef IP_USE_ROUTE_COOKIES
    /* Delete any cookie that belongs to this vr/table */
    if (ret == 0)
        ipnet_route_delete_cookie(vr, table);
#endif
    return ret;
}


/*
 *==========================================================================
 *                    ipnet_rtnetlink_vr_dump
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
ipnet_rtnetlink_vr_dump_family(Ipnet_netlink_mem_t  *mem,
                               struct Ip_nlmsghdr   *nlmsg,
                               int                  family,
                               Ip_ptrdiff_t         *args)
{
    Ip_ptrdiff_t        *done           = &args[0];
    Ip_ptrdiff_t        *s_vr           = &args[1];
    Ip_ptrdiff_t        *s_continue     = &args[2];
    Ipnet_rtnetlink_vr_t    param;

    (void)family;

    param.nlmsg     = nlmsg;
    param.mem       = mem;

    if (*done)
        goto exit_out;

    param.state.aborted = 0;
    param.state.done    = done;
    param.state.vr      = s_vr;
    param.state.cont    = *s_continue;

    /* */
    ipcom_hash_for_each(ipnet->rtabs,
                        (Ipcom_hash_foreach_cb_func)ipnet_rtnetlink_vr_foreach_vr,
                        &param);

    /* Are we aborted? */
    *s_continue = param.state.aborted;

    /* If not aborted; we're done */
    if (!param.state.aborted)
        *done = 1;

exit_out:
    return param.mem->used;
}


IP_GLOBAL void
ipnet_rtnetlink_vr_init(void)
{
    /* AF_UNSPEC */
    ipnet_rtnetlink_register_family_event(IP_AF_UNSPEC, IP_RTM_DELVR,
                                          IP_VR_MAX, sizeof(struct Ip_vrmsg),
                                          ipnet_rtnetlink_vr_delvr_family,
                                          IP_NULL);
    ipnet_rtnetlink_register_family_event(IP_AF_UNSPEC, IP_RTM_NEWVR,
                                          IP_VR_MAX, sizeof(struct Ip_vrmsg),
                                          ipnet_rtnetlink_vr_newvr_family,
                                          IP_NULL);
    ipnet_rtnetlink_register_family_event(IP_AF_UNSPEC, IP_RTM_CHANGEVR,
                                          IP_VR_MAX, sizeof(struct Ip_vrmsg),
                                          ipnet_rtnetlink_vr_changevr_family,
                                          IP_NULL);
    ipnet_rtnetlink_register_family_event(IP_AF_UNSPEC, IP_RTM_GETVR,
                                          IP_VR_MAX, sizeof(struct Ip_vrmsg),
                                          ipnet_rtnetlink_vr_getvr_family,
                                          ipnet_rtnetlink_vr_dump_family);
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

