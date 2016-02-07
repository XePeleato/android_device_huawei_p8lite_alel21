
/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_rtnetlink.c,v $ $Revision: 1.8 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_rtnetlink.c,v $
 *   $Author: jonas $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Joel Hortelius <joel.hortelis@interpeak.se>
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
#define IPCOM_USE_CLIB_PROTO
#include <ipcom.h>
#include <ipcom_sock.h>
#include <ipcom_route.h>
#include <ipcom_errno.h>

#include "ipnet_h.h"
#include "ipnet_ctrl.h"
#include "ipnet_route.h"
#include "ipnet_netlink_h.h"
#include "ipnet_rtnetlink_h.h"
#include "ipcom_sock.h"
#include "ipnet_eth.h"

#ifdef IPNET_USE_NETLINK

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

/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_netlink_register_ops
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_rtnetlink_register_family_event(int                             family,
                                      int                             cmd,
                                      Ip_size_t                       maxattr,
                                      Ip_size_t                       min_size,
                                      Ipnet_rtnetlink_do_t            doit,
                                      Ipnet_netlink_dump_t           dumpit)
{
    Ipnet_rtnetlink_table_t   *table;

    /**/
    if (ipnet->rtnetlink_links[family] == IP_NULL)
    {
        ipnet->rtnetlink_links[family] = ipcom_calloc(IP_RTM_NR_MSGTYPES, sizeof(Ipnet_rtnetlink_table_t *));
        ip_assert(ipnet->rtnetlink_links[family] != IP_NULL);
    }

    ip_assert(ipnet->rtnetlink_links[family][cmd - IP_RTM_BASE] == IP_NULL);

    ipnet->rtnetlink_links[family][cmd - IP_RTM_BASE] = ipcom_calloc(1, sizeof(Ipnet_rtnetlink_table_t));

    ip_assert(ipnet->rtnetlink_links[family][cmd - IP_RTM_BASE] != IP_NULL);

    table = ipnet->rtnetlink_links[family][cmd - IP_RTM_BASE];

    table->nl_cmd                       = cmd;
    table->nl_func_table.nl_rta_max     = maxattr;
    table->nl_func_table.nl_size_min    = IP_NLMSG_LENGTH(min_size);
    table->nl_func_table.nl_do          = doit;
    table->nl_func_table.nl_dump        = dumpit;
}


/*
 *===========================================================================
 *                    ipnet_rtnetlink_parse
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_parse(int                 maxattr,
                      struct Ip_rtattr    *attr,
                      Ip_size_t           attrlen,
                      struct Ip_rtattr    ***rtap)
{
    struct Ip_rtattr **rta = IP_NULL;

    if (maxattr == 0)
        attrlen = 0;
    else
    {
        rta = ipcom_calloc(sizeof(struct Ip_rtattr *), maxattr + 1);
        if (!rta)
            return -IP_ERRNO_ENOMEM;
    }

    while (IP_RTA_OK(attr, attrlen))
    {
        Ip_u16 flavor = attr->rta_type;
        if (flavor)
        {
            if (flavor > maxattr)
            {
                ipcom_free(rta);
                return -IP_ERRNO_EINVAL;
            }

            rta[flavor-1] = attr;
        }
        attr = IP_RTA_NEXT(attr, attrlen);
    }

    *rtap = rta;
    return 0;
}

#ifdef IPNET_USE_NETLINKSOCK
/*
 *===========================================================================
 *                    ipnet_rtnetlink_register_ops
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_rtnetlink_register_ops(struct Ip_rtnl_link_ops *ops)
{
    ops->next = ipnet->rtnetlink_ops;
    ipnet->rtnetlink_ops = ops;
}

/*
 *===========================================================================
 *                    ipnet_rtnetlink_get_ops
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL struct Ip_rtnl_link_ops *
ipnet_rtnetlink_get_ops(const char *kind)
{
    struct Ip_rtnl_link_ops *ops    = ipnet->rtnetlink_ops;

    while (ops != IP_NULL)
    {
        if (ipcom_strcmp(ops->kind, kind) == 0)
            return ops;
        ops = ops->next;
    }

    return IP_NULL;
}
#endif /* IPNET_USE_NETLINK_SOCK */


/*
 *===========================================================================
 *                   ipnet_rtnetlink_rta_sz_put
 *===========================================================================
 * Description: Add a NETLINK route attribute to a NETLINK message.
 * Parameters:  nlh      - Current NLMSG
 *              sz       - Total size
 *              attrtype - Attribute type
 *              attrlen  - Lenght of attribute
 *              data     - Attribute data
 *
 * Returns:
 *
 */
IP_GLOBAL void *
ipnet_rtnetlink_rta_sz_put(Ipnet_netlink_mem_t *mem, int attrtype, int attrlen, IP_CONST void *data)
{
    struct Ip_rtattr *rta = ipnet_netlink_sz_put(mem, IP_NULL, IP_RTA_LENGTH(attrlen));

    if (!rta)
        return IP_NULL;

    rta->rta_type = (Ip_u16) attrtype;
    rta->rta_len  = (Ip_u16) IP_RTA_LENGTH (attrlen);

    if (data)
        ipcom_memcpy(IP_RTA_DATA(rta), data, attrlen);

    return rta;
}

/*
 *===========================================================================
 *                    ipnet_rtnetlink_rta_sz_put_u32
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void *
ipnet_rtnetlink_rta_sz_put_u32(Ipnet_netlink_mem_t  *mem,
                               int                  atype,
                               Ip_u32               val)
{
    /* Reinvoke */
    return ipnet_rtnetlink_rta_sz_put(mem, atype, sizeof(val), &val);
}


/*
 *===========================================================================
 *                   ipnet_rtnetlink_rta_sz_nested_begin
 *===========================================================================
 * Description: Begin a nested NETLINK route attribute to a NETLINK message.
 * Parameters:  nlh      - Current NLMSG
 *              sz       - Total size
 *              attrtype - Attribute type
 *
 * Returns:
 *
 */
IP_GLOBAL void *
ipnet_rtnetlink_rta_sz_nested_begin(Ipnet_netlink_mem_t *mem, int attrtype)
{
    struct Ip_rtattr *rta = ipnet_netlink_sz_put(mem, IP_NULL, IP_RTA_LENGTH(0));

    if (!rta)
        return IP_NULL;

    rta->rta_type = (Ip_u16) attrtype;
    rta->rta_len  = (Ip_u16) IP_RTA_LENGTH (0);

    return rta;
}


/*
 *===========================================================================
 *                   ipnet_rtnetlink_rta_sz_nested_end
 *===========================================================================
 * Description: Terminates a nested RTA.
 * Parameters:  nlh      - Current NLMSG
 *              rta      - Total size
 *              attrtype - Attribute type
 *
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_rtnetlink_rta_sz_nested_end(Ipnet_netlink_mem_t *mem, void *nested)
{
    char                *eptr        = &mem->ptr[mem->used + mem->offset];
    struct Ip_rtattr    *rta         = nested;

    rta->rta_len    = (Ip_size_t) (eptr - (char *)rta);
}


IP_GLOBAL Ipnet_rtnetlink_func_table_t *
ipnet_rtnetlink_family_get_do(struct Ipnet_rtnetlink_table    ***table,
                              int                             family,
                              int                             cmd)
{
    if (family < IP_AF_MAX && cmd >= IP_RTM_BASE && cmd-IP_RTM_BASE <= IP_RTM_NR_MSGTYPES)
    {
        Ipnet_rtnetlink_table_t   **tbl = table[family];
        if (tbl)
        {
            if (tbl[cmd - IP_RTM_BASE] && tbl[cmd - IP_RTM_BASE]->nl_func_table.nl_do)
                return &tbl[cmd - IP_RTM_BASE]->nl_func_table;
        }

        if (family != IP_AF_UNSPEC)
            return ipnet_rtnetlink_family_get_do(table, IP_AF_UNSPEC, cmd);
    }

    return IP_NULL;
}

IP_GLOBAL Ipnet_rtnetlink_func_table_t *
ipnet_rtnetlink_family_get_dump(struct Ipnet_rtnetlink_table    ***table,
                                int                             family,
                                int                             cmd)
{
    if (family < IP_AF_MAX && cmd >= IP_RTM_BASE && cmd-IP_RTM_BASE <= IP_RTM_NR_MSGTYPES)
    {
        Ipnet_rtnetlink_table_t   **tbl = table[family];
        if (tbl)
        {
            if (tbl[cmd - IP_RTM_BASE] && tbl[cmd - IP_RTM_BASE]->nl_func_table.nl_dump)
                return &tbl[cmd - IP_RTM_BASE]->nl_func_table;
        }

        if (family != IP_AF_UNSPEC)
            return ipnet_rtnetlink_family_get_dump(table, IP_AF_UNSPEC, cmd);
    }

    return IP_NULL;
}


#ifdef IPNET_USE_NETLINKSOCK
/*
 *===========================================================================
 *                    ipnet_netlink_dump
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_rtnetlink_do(Ipnet_rtnetlink_do_t       do_it,
                   int                        family,
                   Ipnet_socket               *sock,
                   struct Ip_nlmsghdr         *nlmsg,
                   Ip_bool                    non_blocking,
                   struct Ip_rtattr           **rta)
{
    Ipcom_pkt               *pkt    = IP_NULL;
    Ipnet_netlink_mem_t     mem;
    int                     ret;

    /* Is it a 'get'? */
    if (((nlmsg->nlmsg_type - IP_RTM_BASE) & 0x3) == 2)
    {
        /**/
        if (!ipnet_netlink_pkt_alloc(&pkt, sock, non_blocking, &mem, 512))
            return 0;
    }
    else
    {
        /**/
        ipnet_netlink_pkt_mem(IP_NULL, sock, &mem);
    }

    /* */
    ret = (*do_it) (&mem, nlmsg, rta, family);

    /* */
    if (pkt != IP_NULL && ret > 0)
    {
        pkt->end += ret;
        ipnet_netlink_pkt_queue(&pkt, sock);
    }

    if (pkt)
        ipcom_pkt_free(pkt);
    return ret < 0? ret : 0;
}

/*
 *===========================================================================
 *                    ipnet_rtnetlink_sock_send
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_sock_send(Ipnet_socket                    *sock,
                          Ip_bool                         non_blocking,
                          struct Ip_nlmsghdr              *nlmsg)
{
	struct Ip_rtattr                **rta   = IP_NULL;
    int                             ret = 0;
    Ipnet_rtnetlink_func_table_t    *tbl;
    int                             family;

    /* Need to retrieve the family byte */
    if (nlmsg->nlmsg_len < IP_NLMSG_LENGTH(sizeof(struct Ip_rtgenmsg)))
        return 0;

    /* These messages are to be ignored */
    if (nlmsg->nlmsg_type < IP_RTM_BASE)
        return 0;

    /**/
    family = ((struct Ip_rtgenmsg*)IP_NLMSG_DATA(nlmsg))->rtgen_family;

    /* Dump */
    if (IP_BIT_ISSET(nlmsg->nlmsg_flags, IP_NLM_F_DUMP) && ((nlmsg->nlmsg_type - IP_RTM_BASE) & 0x3) == 2)
    {
        ret = -IP_ERRNO_EAFNOSUPPORT;
        tbl = ipnet_rtnetlink_family_get_dump(ipnet->rtnetlink_links, family, nlmsg->nlmsg_type);
        if (tbl == IP_NULL)
            goto error_out;

        ret = ipnet_netlink_dump(tbl->nl_dump, family, sock, nlmsg, non_blocking);
        goto error_out;
    }

    tbl = ipnet_rtnetlink_family_get_do(ipnet->rtnetlink_links, family, nlmsg->nlmsg_type);
    if (tbl == IP_NULL)
    {
        ret = -IP_ERRNO_EAFNOSUPPORT;
        goto error_out;
    }
    else
    {
        struct Ip_rtattr *attr      = IP_NULL;
        int              attrlen    = 0;

        /* Check minimum length. */
        if (nlmsg->nlmsg_len < tbl->nl_size_min)
        {
            ret = -IP_ERRNO_EINVAL;
            goto error_out;
        }

        /* */
        if (nlmsg->nlmsg_len > tbl->nl_size_min)
        {
            attrlen = nlmsg->nlmsg_len - IP_NLMSG_ALIGN(tbl->nl_size_min);
            attr = (struct Ip_rtattr*)((int)nlmsg + IP_NLMSG_ALIGN(tbl->nl_size_min));
        }

        ret = ipnet_rtnetlink_parse(tbl->nl_rta_max, attr, attrlen, &rta);
        if (ret < 0)
            goto error_out;
    }

    /* DO */
    ret = ipnet_rtnetlink_do(tbl->nl_do, family, sock, nlmsg, non_blocking, (void *)rta);

error_out:
    if (rta)
        ipcom_free(rta);
    return IP_MIN(ret, 0);
}
#endif /* IPNET_USE_NETLINK_SOCK */


/*
 *===========================================================================
 *                      ipnet_netlink_broadcast
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_rtnetlink_broadcast(Ipcom_pkt *pkt, int msg_group)
{
    ipnet_netlink_broadcast(pkt, IP_NETLINK_ROUTE, msg_group);
}



/*
 *===========================================================================
 *                    ipnet_rtnetlink_vr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_u16
ipnet_rtnetlink_vr(struct Ip_rtattr *rta, Ip_u16 vr)
{
    if (rta)
        return (Ip_u16)IP_GET_32ON8(IP_RTA_DATA(rta));
    return vr;
}


/*
 *===========================================================================
 *                    ipnet_rtnetlink_dump_family
 *===========================================================================
 * Description: Generic netlink address dumper.
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_addr_dump_family(Ipnet_netlink_mem_t *mem,
                                 struct Ip_nlmsghdr  *nlh,
                                 int                 family,
                                 Ip_ptrdiff_t        *args)
{

    int                     req_type;

    /**/
    switch (nlh->nlmsg_type)
    {
    case IP_RTM_GETMULTICAST:
        req_type = IPNET_ADDR_TYPE_MULTICAST;
        break;
    case IP_RTM_GETANYCAST:
        req_type = IPNET_ADDR_TYPE_ANYCAST;
        break;
    default:
        req_type = IPNET_ADDR_TYPE_UNICAST;
        break;
    }

    switch (family)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        return ipnet_rtnetlink_ip4_addr_dump(mem, nlh, req_type, args);
#endif
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        return ipnet_rtnetlink_ip6_addr_dump(mem, nlh, req_type, args);
#endif
    default:
        return -IP_ERRNO_EAFNOSUPPORT;

    }
}

/*
 *===========================================================================
 *                    ipnet_rtnetlink_addr_newaddr_family
 *===========================================================================
 * Description: Generic netlink address add handler
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_addr_newaddr_family(Ipnet_netlink_mem_t *mem,
                                    struct Ip_nlmsghdr  *nlh,
                                    struct Ip_rtattr    **rta,
                                    int                 family)
{
    switch (family)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        return ipnet_rtnetlink_ip4_newaddr(mem, nlh, rta);
#endif
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        return ipnet_rtnetlink_ip6_newaddr(mem, nlh, rta);
#endif
    default:
        return -IP_ERRNO_EAFNOSUPPORT;;
    }
}


/*
 *===========================================================================
 *                    ipnet_rtnetlink_addr_deladdr_family
 *===========================================================================
 * Description: Generic netlink address delete handler
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_rtnetlink_addr_deladdr_family(Ipnet_netlink_mem_t *mem,
                                    struct Ip_nlmsghdr  *nlh,
                                    struct Ip_rtattr    **rta,
                                    int                 family)
{
    switch (family)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        return ipnet_rtnetlink_ip4_deladdr(mem, nlh, rta);
#endif
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        return ipnet_rtnetlink_ip6_deladdr(mem, nlh, rta);
#endif
    default:
        return -IP_ERRNO_EAFNOSUPPORT;
    }
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
