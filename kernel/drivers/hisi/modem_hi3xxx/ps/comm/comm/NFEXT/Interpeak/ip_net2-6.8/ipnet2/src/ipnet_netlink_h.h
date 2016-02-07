
/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_netlink_h.h,v $ $Revision: 1.5 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_netlink_h.h,v $
 *     $Author: jonas $ $Date: 2009-03-02 16:00:14 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Joel Hortelius <joel.hortelius@interpeak.se>
 ******************************************************************************
 */
#ifndef IPNET_NETLINK_H_H
#define IPNET_NETLINK_H_H

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
#include "ipnet_netlink.h"
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

typedef struct Ipnet_netlink_socket_struct
{
    /* Destination data (connected) */
    Ip_pid_t        nl_dst_pid;
    Ip_pid_t        nl_dst_mc_groups;

    /* Bind information */
    Ip_pid_t        nl_src_pid;
    Ip_pid_t        nl_src_mc_groups;
}
Ipnet_netlink_socket;

typedef struct Ipnet_netlink_foreach_cb_data_struct
{
    Ip_u8        status;              /* Message status            */
    Ip_u32       seq;                 /* Message sequence number   */
    Ip_pid_t     pid;                 /* Message receiver pid      */
    int          vr;                  /* Caller specified vr       */
    int          table;               /* Caller specified table    */
    Ip_ptrdiff_t args[4];             /* Various callback data     */
    Ip_size_t    offset;              /* Message buffer offset     */
    Ip_size_t    len;                 /* Message buffer length     */
    Ip_u8       *data;                /* Message buffer            */
}
Ipnet_netlink_foreach_cb_data;


#define IPNET_NETLINK_DUMP_FIRST_ENTRY 0
#define IPNET_NETLINK_DUMP_FINISHED    1
#define IPNET_NETLINK_DUMP_DONT_FIT    2
#define IPNET_NETLINK_DUMP_TRAILER     3

/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */
IP_GLOBAL struct Ip_nlmsghdr*
ipnet_netlink_nlmsg_sz_begin(Ipnet_netlink_mem_t  *mem,
                             Ip_u32               seq,
                             int                  type,
                             Ip_u32               flags);

IP_GLOBAL int
ipnet_netlink_nlmsg_sz_end(Ipnet_netlink_mem_t *mem,
                           struct Ip_nlmsghdr* nlh);

IP_GLOBAL void
ipnet_netlink_pkt_queue(Ipcom_pkt                   **pktp,
                        struct Ipnet_socket_struct  *sock);

IP_GLOBAL Ip_bool
ipnet_netlink_pkt_alloc(Ipcom_pkt                   **pktp,
                        struct Ipnet_socket_struct  *sock,
                        Ip_bool                     non_blocking,
                        Ipnet_netlink_mem_t         *mem,
                        Ip_size_t                   size);
IP_GLOBAL Ipnet_netlink_mem_t *
ipnet_netlink_pkt_mem(Ipcom_pkt                     *pkt,
                      struct Ipnet_socket_struct    *sock,
                      Ipnet_netlink_mem_t           *mem);

IP_GLOBAL int
ipnet_netlink_dump(Ipnet_netlink_dump_t         do_dump,
                   int                          family,
                   struct Ipnet_socket_struct   *sock,
                   struct Ip_nlmsghdr           *nlmsg,
                   Ip_bool                      non_blocking);


IP_GLOBAL char *ipnet_netlink_strdup(struct Ip_nlattr *nla);
IP_GLOBAL void *ipnet_netlink_sz_put(Ipnet_netlink_mem_t *mem, void *data, int len);

IP_GLOBAL void ipnet_netlink_broadcast(Ipcom_pkt *pkt, int proto, int msg_group);

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
