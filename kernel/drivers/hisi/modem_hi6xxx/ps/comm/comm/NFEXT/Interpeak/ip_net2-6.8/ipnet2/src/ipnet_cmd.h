/*
 ******************************************************************************
 *                     INTERPEAK HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_cmd.h,v $ $Revision: 1.15 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_cmd.h,v $
 *   $Author: kenneth $ $Date: 2009-02-02 13:20:23 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPNET_CMD_H
#define IPNET_CMD_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Functions used by IPNET shell commands.
 */

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipnet_config.h"

#include <ipcom_inet.h>
#include <ipcom_sock.h>
#include <ipcom_sock2.h>
#include <ipcom_cmd.h>

#ifdef __cplusplus
extern "C" {
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

/*
 *===========================================================================
 *                         Ipnet_cmd_route_addr
 *===========================================================================
 */
union Ipnet_cmd_route_addr
{
    struct Ip_sockaddr      sa;
#ifdef IPCOM_USE_INET
    struct Ip_sockaddr_in   sa_in;
#endif
#ifdef IPCOM_USE_INET6
    struct Ip_sockaddr_in6  sa_in6;
#endif
#ifdef IPMPLS
    struct Ip_sockaddr_mpls sa_mpls;
#endif
};


/*
 *===========================================================================
 *                         Ipnet_cmd_route
 *===========================================================================
 */
typedef struct Ipnet_cmd_route_struct
{
    int       command;                     /* IPNET_ROUTE_CMD_xxx command */
    Ip_bool   no_dns;                      /* Do not try to look up the names */
    int       domain;                      /* IP_AF_INET or IP_AF_INET6 */
    int       show_domain;                 /* IP_AF_INET or IP_AF_INET6 */
    int       rt_flags;                    /* IPNET_RTF_xxx flags */
    Ip_u32    ifindex;                     /* Route interface index, 0 not specified. */
    union Ipnet_cmd_route_addr dst;        /* Host or network address */
    union Ipnet_cmd_route_addr netmask;    /* The dst netmask */
    union Ipnet_cmd_route_addr gateway;    /* The gateway for the route */

    int       initis;                      /* IPNET_RTV_xxx flags */
    int       rtt;
    int       rttvar;
    int       mtu;
    int       hopcount;
    Ip_u32    expire;
#if IPCOM_VR_MAX > 1
    int       vr;
    int       vr_new;
#endif
    Ip_bool   add_vr;
    Ip_u32    table;
    char     *vr_name;

    Ip_bool   silent;
    Ip_bool   no_llinfo;
}
Ipnet_cmd_route;


/* Forward declaration */
struct Ipnet_rt_msghdr;


/*
 *===========================================================================
 *                        Ipnet_cmd_ping_stat
 *===========================================================================
 */
typedef struct Ipnet_cmd_ping_stat_struct
{
    unsigned transmitted; /* Number of transmitted echo requests */
    unsigned received;    /* Number of requests that got at least one answer */
    unsigned duplicates;  /* Number of duplicate echo responses */
    unsigned rtt_min;     /* Minimum round-tripp time in milliseconds */
    unsigned rtt_max;     /* Maximum round-tripp time in milliseconds */
    unsigned rtt_sum;     /* Sum of all round-tripp times in milliseconds */
    unsigned start_time;  /* Time when the ping command started */
}
Ipnet_cmd_ping_stat_t;


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

/* Shell command helper functions */
IP_GLOBAL IP_CONST char *
ipnet_cmd_if_type_to_str(int type, char *str, Ip_size_t len);

IP_GLOBAL IP_CONST char *
ipnet_cmd_rt_flags_to_str(Ip_u32 flags, char *str, Ip_size_t len);

IP_GLOBAL IP_CONST char *
ipnet_cmd_rt_flags_to_str_short(Ip_u32 flags, Ip_bool full);

IP_GLOBAL void
ipnet_cmd_init_addrs(void *msg, Ip_u32 addrs, struct Ip_sockaddr **sas);

IP_GLOBAL unsigned
ipnet_cmd_msec_since(struct Ip_timeval *base);

IP_GLOBAL void
ipnet_bit_array_clr(void *bit_array, unsigned bit);

IP_GLOBAL Ip_bool
ipnet_bit_array_test_and_set(void *bit_array, unsigned bit);

IP_GLOBAL int
ipnet_cmd_route_add(Ip_fd sock, Ipnet_cmd_route *cmd);

IP_GLOBAL int
ipnet_cmd_route_delete(Ip_fd sock, Ipnet_cmd_route *cmd);

IP_GLOBAL void
ipnet_ping_stat_init(Ipnet_cmd_ping_stat_t *stat);

IP_GLOBAL void
ipnet_ping_stat_transmit(Ipnet_cmd_ping_stat_t *stat);

IP_GLOBAL void
ipnet_ping_stat_receive(Ipnet_cmd_ping_stat_t *stat,
                        unsigned elapsed_time,
                        Ip_bool duplicate);

IP_GLOBAL void
ipnet_ping_stat_report(Ipnet_cmd_ping_stat_t *stat,
                       IP_CONST char *peer);

#ifdef __cplusplus
}
#endif

#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
