
/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_rtnetlink_h.h,v $ $Revision: 1.6 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_rtnetlink_h.h,v $
 *     $Author: jonas $ $Date: 2009-03-02 16:01:15 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Joel Hortelius <joel.hortelius@interpeak.se>
 ******************************************************************************
 */
#ifndef IPNET_RTNETLINK_H_H
#define IPNET_RTNETLINK_H_H

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

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */
#include "ipnet_neigh.h"
#include "ipnet_rtnetlink.h"

#ifdef __cplusplus
extern "C" {
#endif

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
struct Ipnet_socket_struct;
struct Ipnet_netif_struct;
struct Ipnet_rt_metrics;
struct Ipnet_route_entry_struct;
struct Ipnet_ip6_addr_entry_struct;
struct Ipnet_ip4_addr_entry_struct;
struct Ipnet_policy_routing_rule;


typedef union Ipnet_rtnetlink_key
{
#ifdef IPCOM_USE_INET
    struct
    {
        struct Ipnet_ipv4_key         key;
        struct Ipnet_ipv4_key         mask;
    } v4;
#endif
#ifdef IPCOM_USE_INET6
    struct
    {
        struct Ipnet_ipv6_key         key;
        struct Ipnet_ipv6_key         mask;
    } v6;
#endif
} Ipnet_rtnetlink_key_t;

/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */
IP_GLOBAL void  ipnet_rtnetlink_register_family_event(int                           family,
                                                      int                           cmd,
                                                      Ip_size_t                     maxattr,
                                                      Ip_size_t                     nl_min_size,
                                                      Ipnet_rtnetlink_do_t          doit,
                                                      Ipnet_netlink_dump_t          dumpit);

#ifdef IPCOM_USE_INET
IP_GLOBAL void ipnet_rtnetlink_ip4_init(void);
IP_GLOBAL int ipnet_rtnetlink_ip4_route_fill_info(Ipnet_netlink_mem_t *mem,
                                                  Ipnet_route_entry   *rt,
                                                  int                 seq,
                                                  Ip_u8               event,
                                                  Ip_u32              eflags);
IP_GLOBAL int ipnet_rtnetlink_ip4_neigh_fill_info(Ipnet_netlink_mem_t *mem,
                                                  Ipnet_route_entry   *rt,
                                                  int                 seq,
                                                  Ip_u8               event,
                                                  Ip_u32              eflags);
IP_GLOBAL int ipnet_rtnetlink_ip4_route_key_setup(Ipnet_rtnetlink_key_t   *ukey,
                                                  Ip_u32                  ifindex,
                                                  void                    **maskp,
                                                  int                     mask_len,
                                                  struct Ip_rtattr        *dst);
IP_GLOBAL int ipnet_rtnetlink_ip4_route_gw_setup(Ip_u32                     ifindex,
                                                 union Ip_sockaddr_union    *ugw,
                                                 struct Ip_sockaddr         **gw,
                                                 struct Ip_rtattr           *rta);
IP_GLOBAL int ipnet_rtnetlink_ip4_neigh_setup(Ipnet_rtnetlink_key_t   *ukey,
                                              struct Ip_ndmsg         *ndm,
                                              Ipnet_route_entry       *rt,
                                              Ipnet_nd_state_t        nd_state,
                                              void                    *link_addr,
                                              struct Ip_rtattr        **rta);
IP_GLOBAL void ipnet_rtnetlink_ip4_addr_add(struct Ipnet_netif_struct *netif,
                                            struct Ipnet_ip4_addr_entry_struct *addr);
IP_GLOBAL void ipnet_rtnetlink_ip4_addr_del(struct Ipnet_netif_struct *netif,
                                            struct Ipnet_ip4_addr_entry_struct *addr);
#endif

#ifdef IPCOM_USE_INET6
IP_GLOBAL void ipnet_rtnetlink_ip6_init(void);
IP_GLOBAL int ipnet_rtnetlink_ip6_route_fill_info(Ipnet_netlink_mem_t *mem,
                                                  Ipnet_route_entry   *rt,
                                                  int                 seq,
                                                  Ip_u8               event,
                                                  Ip_u32              eflags);
IP_GLOBAL int ipnet_rtnetlink_ip6_neigh_fill_info(Ipnet_netlink_mem_t *mem,
                                                  Ipnet_route_entry   *rt,
                                                  int                 seq,
                                                  Ip_u8               event,
                                                  Ip_u32              eflags);
IP_GLOBAL int ipnet_rtnetlink_ip6_route_key_setup(Ipnet_rtnetlink_key_t   *ukey,
                                                  Ip_u32                  ifindex,
                                                  void                    **maskp,
                                                  int                     mask_len,
                                                  struct Ip_rtattr        *dst);
IP_GLOBAL int ipnet_rtnetlink_ip6_route_gw_setup(Ip_u32                     ifindex,
                                                 union Ip_sockaddr_union    *ugw,
                                                 struct Ip_sockaddr         **gw,
                                                 struct Ip_rtattr           *rta);
IP_GLOBAL int ipnet_rtnetlink_ip6_neigh_setup(Ipnet_rtnetlink_key_t *ukey,
                                              struct Ip_ndmsg       *ndm,
                                              Ipnet_route_entry     *rt,
                                              Ipnet_nd_state_t      nd_state,
                                              void                  *link_addr,
                                              struct Ip_rtattr      **rta);
IP_GLOBAL void ipnet_rtnetlink_ip6_addr_add(struct Ipnet_netif_struct *netif,
                                            struct Ipnet_ip6_addr_entry_struct *addr);
IP_GLOBAL void ipnet_rtnetlink_ip6_addr_del(struct Ipnet_netif_struct *netif,
                                            struct Ipnet_ip6_addr_entry_struct *addr);
#endif

IP_GLOBAL int ipnet_rtnetlink_route_sz_fill_metrics(Ipnet_netlink_mem_t       *mem,
                                                    struct Ipnet_rt_metrics   *metrics,
                                                    Ip_u32                    mtu);



IP_GLOBAL int ipnet_rtnetlink_route_key_setup(int                   family,
                                              Ipnet_rtnetlink_key_t *ukey,
                                              Ip_u32                ifindex,
                                              void                  **maskp,
                                              int                   mask_len,
                                              struct Ip_rtattr      *dst);

IP_GLOBAL int ipnet_rtnetlink_route_gw_setup(int                        family,
                                             Ip_u32                     ifindex,
                                             union Ip_sockaddr_union    *ugw,
                                             struct Ip_sockaddr         **gw,
                                             struct Ip_rtattr           *rta);
IP_GLOBAL void ipnet_rtnetlink_route_add(Ipnet_route_entry *rt);
IP_GLOBAL void ipnet_rtnetlink_route_del(Ipnet_route_entry *rt);
IP_GLOBAL void ipnet_rtnetlink_route_change(Ipnet_route_entry *rt);
IP_GLOBAL void ipnet_rtnetlink_route_resolve(Ipnet_route_entry *rt);
IP_GLOBAL void ipnet_rtnetlink_route_miss(Ipcom_route *rtab, IP_CONST void *key);
IP_GLOBAL void ipnet_rtnetlink_route_redirect(Ipcom_route *rtab, Ip_u32 ifindex,
                                           IP_CONST void *dst, IP_CONST void *new_gw);
IP_GLOBAL void ipnet_rtnetlink_rule_event(struct Ipnet_policy_routing_rule *mpr,
                                          Ip_u16                           vr,
                                          Ip_pid_t                         pid,
                                          int                              event);

IP_GLOBAL void ipnet_rtnetlink_link_init(void);
IP_GLOBAL void ipnet_rtnetlink_vr_init(void);


IP_GLOBAL Ip_bool ipnet_rtnetlink_neigh_is(struct Ipnet_route_entry_struct *rt);
IP_GLOBAL void ipnet_rtnetlink_neigh_add(Ipnet_route_entry *rt);
IP_GLOBAL void ipnet_rtnetlink_neigh_del(Ipnet_route_entry *rt);
IP_GLOBAL void ipnet_rtnetlink_neigh_change(Ipnet_route_entry *rt);


IP_GLOBAL void *ipnet_rtnetlink_rta_sz_put(Ipnet_netlink_mem_t  *mem,
                                           int                  atype,
                                           int                  alen,
                                           IP_CONST void        *data);
IP_GLOBAL void *ipnet_rtnetlink_rta_sz_put_u32(Ipnet_netlink_mem_t  *mem,
                                               int                  atype,
                                               Ip_u32               val);
IP_GLOBAL void *ipnet_rtnetlink_rta_sz_nested_begin(Ipnet_netlink_mem_t *mem,
                                                    int                 attrtype);
IP_GLOBAL void ipnet_rtnetlink_rta_sz_nested_end(Ipnet_netlink_mem_t    *mem,
                                                 void                   *nested);
IP_GLOBAL int ipnet_rtnetlink_parse(int                 maxattr,
                                    struct Ip_rtattr    *atttr,
                                    Ip_size_t           attrlen,
                                    struct Ip_rtattr    ***rtap);

IP_GLOBAL int
ipnet_rtnetlink_sock_send(struct Ipnet_socket_struct    *sock,
                          Ip_bool                       non_blocking,
                          struct Ip_nlmsghdr            *nlmsg);

IP_GLOBAL void
ipnet_rtnetlink_broadcast(Ipcom_pkt *pkt, int msg_group);

/* Notifications */
IP_GLOBAL void ipnet_rtnetlink_link_change(struct Ipnet_netif_struct *netif);
IP_GLOBAL void ipnet_rtnetlink_link_add(struct Ipnet_netif_struct *netif);
IP_GLOBAL void ipnet_rtnetlink_link_delete(struct Ipnet_netif_struct *netif);

IP_GLOBAL void ipnet_netlinksock_vrtab_add(Ip_u16 vr, Ip_u32 table);
IP_GLOBAL void ipnet_netlinksock_vrtab_del(Ip_u16 vr, Ip_u32 table);


IP_GLOBAL struct Ip_nlmsghdr* ipnet_netlink_nlmsg_put(void *data, Ip_pid_t pid,
                                                         Ip_u32 seq, int type, int len, Ip_u32 flags);

IP_GLOBAL int ipnet_netlink_rta_put(void *buf, int attrtype,
                                    int attrlen, IP_CONST void *data);


IP_GLOBAL Ip_u16 ipnet_rtnetlink_vr(struct Ip_rtattr *rta, Ip_u16 vr);

#endif

#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
