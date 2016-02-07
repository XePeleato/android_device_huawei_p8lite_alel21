/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_cmd_netstat.c,v $ $Revision: 1.82.16.2 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_cmd_netstat.c,v $
 *     $Author: edahlber $
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
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifdef IPCOM_KERNEL
#undef IPCOM_KERNEL
#endif
#include "ipnet_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cmd.h>
#include <ipcom_getopt.h>
#include <ipcom_sock.h>
#include <ipcom_sysctl.h>

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_ctrl.h"
#include "ipnet_route.h"
#include "ipnet_routesock.h"
#include "ipnet_cmd.h"
#include "ipnet_usr_sock.h"

#include "../../InterpeakMntn/include/InterpeakMntn.h"

#ifdef IPSCTP
#include <ipsctp.h>
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */
#define IPNET_NETSTAT_STRLEN   256

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

IP_STATIC Ipcom_cmd_int_str_map af_map[] =
{
    { IP_AF_INET,  "INET"  },
    { IP_AF_INET6, "INET6" },
    { -1, IP_NULL          }
};

IP_STATIC Ipcom_cmd_int_str_map proto_map[] =
{
    { IP_IPPROTO_TCP,        "TCP"     },
    { IP_IPPROTO_UDP,        "UDP"     },
    { IP_IPPROTO_ICMP,       "ICMP"    },
    { IP_IPPROTO_IGMP,       "IGMP"    },
    { IP_IPPROTO_ICMPV6,     "ICMP6"   },
    { IP_IPPROTO_IP,         "IP"      },
    { IP_IPPROTO_IPV4,       "IPv4"    },
    { IP_IPPROTO_IPV6,       "IPv6"    },
    { IP_IPPROTO_OSPFIGP,    "OSPFIGP" },
    { IP_IPPROTO_RSVP,       "RSVP"    },
    { IP_IPPROTO_PIM,        "PIM"     },
    { IP_IPPROTO_SCTP,       "SCTP"    },
    { -1, IP_NULL }
};

#if defined(IPTCP) && !defined(IP_PORT_LAS)
IP_STATIC IP_CONST char *tcp_state_list[] =
{
    "",
    "CLOSED",
    "LISTEN",
    "SYN_SENT",
    "SYN_RCVD",
    "ESTABLISHED",
    "CLOSE_WAIT",
    "FIN_WAIT_1",
    "CLOSING",
    "LAST_ACK",
    "FIN_WAIT_2",
    "TIME_WAIT"
};
#endif /* IPTCP */

#if defined(IPSCTP) && !defined(IP_PORT_LAS)
IP_STATIC IP_CONST char *netstat_sctp_state_list[] =
{
    "LISTEN",
    "CLOSED",
    "COOKIE_WAIT",
    "COOKIE_ECHOED",
    "ESTABLISHED",
    "SHUTDOWN_PENDING",
    "SHUTDOWN_SENT",
    "SHUTDOWN_RECEIVED",
    "SHUTDOWN_ACK_SENT",
    "UNKNOWN"
};
#endif /* IPSCTP */


/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipnet_cmd_netstat_is_contiguous
 *===========================================================================
 * Description: Checks if the specified mask is contiguous.
 * Parameters:  mask - a networks mask.
 * Returns:     IP_TRUE if the mask is contiguous.
 *
 */
IP_STATIC Ip_bool
ipnet_cmd_netstat_is_contiguous(struct Ip_sockaddr *mask)
{
    Ip_bool is_contiguous = IP_TRUE;

#ifdef IPCOM_USE_INET
    if (mask->sa_family == IP_AF_INET)
    {
        /* Only AF_INET masks can be non-contiguous */
        Ip_u32 contiguous_mask = 0;

        ipnet_route_create_mask(&contiguous_mask,
                                ipcom_sockaddr_to_prefixlen(mask));
        is_contiguous = (ipcom_memcmp(&contiguous_mask,
                                      &((struct Ip_sockaddr_in *)mask)->sin_addr,
                                      sizeof(contiguous_mask))
                         == 0);
    }
#endif /* IPCOM_USE_INET */

    return is_contiguous;
}


/*
 *===========================================================================
 *                    ipnet_netstat_print_routes
 *===========================================================================
 * Description: Prints the content of the routing tables (FIB).
 * Parameters:  af - Address family (0 for all).
 *              rtab_index - The route table to print.
 *              policy - The policy index of the router.
 *              no_dns - IP_TRUE if the addresses should not be resolved to
 *              there DNS names.
 * Returns:
 *
 */
IP_STATIC void
ipnet_netstat_print_routes(int af, Ip_u32 table, Ip_bool no_dns, Ip_bool show_llinfo)
{
    char                    str[IPNET_NETSTAT_STRLEN];
    char                    str2[16];
    int                     name[6];
    Ip_u8                  *route_buf;
    Ip_size_t               route_buf_len;
    Ip_size_t               offset;
    int                     current_proto = 0;
    struct Ipnet_rt_msghdr *rt;
    struct Ip_sockaddr     *addrs[IPNET_RTAX_MAX];

    name[0] = IP_CTL_NET;
    name[1] = IP_AF_ROUTE;
    name[2] = 0;
    name[3] = af;
    name[4] = IP_NET_RT_DUMP;
    name[5] = 0;

    if (ipcom_sysctl(name, 6, IP_NULL, &route_buf_len, IP_NULL, 0) < 0)
    {
        ipcom_printf("Failed to get the length needed for the route buffer: %s",
                     ipcom_strerror(ipcom_errno));
        return;
    }

    if (route_buf_len == 0)
    {
        ipcom_printf("table=%u on VR=%u does not exists or is empty"IP_LF,
                     (unsigned)table,
                     ipcom_proc_vr_get());
        return;
    }

    route_buf = ipcom_malloc(route_buf_len);
    if (route_buf == IP_NULL)
    {
        ipcom_printf("Out of memory");
        return;
    }

    if (ipcom_sysctl(name, 6, route_buf, &route_buf_len, IP_NULL, 0) < 0)
    {
        ipcom_printf("Failed to get the route entries: %s", ipcom_strerror(ipcom_errno));
        goto leave;
    }

    for (offset = 0; offset < route_buf_len; offset += rt->rtm_msglen)
    {
        rt = (struct Ipnet_rt_msghdr *) &route_buf[offset];

        if (show_llinfo == IP_FALSE && IP_BIT_ISSET(rt->rtm_flags, IPNET_RTF_LLINFO))
            continue;

        ipnet_cmd_init_addrs(rt + 1, rt->rtm_addrs, addrs);

        if (rt->rtm_table != table)
            continue;

        if (addrs[IPNET_RTAX_DST]->sa_family != current_proto)
        {
            struct Ip_sioc_route_table sioc_rt;
            int s;

            /* First entry in this address family, print header */
            current_proto = addrs[IPNET_RTAX_DST]->sa_family;

            sioc_rt.vr    = (Ip_u16) ipcom_proc_vr_get();
            sioc_rt.table = table;
            s = ipcom_socket(current_proto, IP_SOCK_DGRAM, 0);

            ipcom_printf("\n");
            ipcom_printf("%s route table - vr: %u, table: %u",
                         ipcom_cmd_key_to_str(af_map, current_proto, str, sizeof(str)),
                         sioc_rt.vr,
                         (unsigned)sioc_rt.table);
            if (ipcom_socketioctl(s, IP_SIOCGROUTETABNAME, &sioc_rt) == 0)
                ipcom_printf(", name: %s", sioc_rt.name);

            ipcom_printf("\n%-32s%-30s%-9s%-6s%-6s%-6s\n", "Destination", "Gateway", "Flags", "Use", "If", "Metric");
            ipcom_socketclose(s);
        }

        (void)ipcom_cmd_sockaddr_to_str(addrs[IPNET_RTAX_DST], IP_FALSE, no_dns, str, sizeof(str));
        if (addrs[IPNET_RTAX_NETMASK] != IP_NULL)
        {
            if (ipnet_cmd_netstat_is_contiguous(addrs[IPNET_RTAX_NETMASK]))
            {
                ipcom_strcat(str, "/");
                ipcom_sprintf(str2, "%d", ipcom_sockaddr_to_prefixlen(addrs[IPNET_RTAX_NETMASK]));
            }
            else
            {
                ipcom_strcat(str, ":");
                ipcom_cmd_sockaddr_to_str(addrs[IPNET_RTAX_NETMASK],
                                          IP_FALSE,
                                          IP_TRUE,
                                          str2,
                                          sizeof(str2));
            }

            ipcom_strcat(str, str2);
        }
        ipcom_printf("%-31s ", str);

        if (addrs[IPNET_RTAX_GATEWAY])
        {
            ipcom_printf("%-29s ", ipcom_cmd_sockaddr_to_str(
                addrs[IPNET_RTAX_GATEWAY], IP_FALSE, no_dns, str, sizeof(str)));
        }
        else if (rt->rtm_index != 0)
        {
            ipcom_sprintf(str2, "link#%ld", rt->rtm_index);
            ipcom_printf("%-29s ", str2);
        }
        else
            ipcom_printf("%-29s ", "*");

        ipcom_printf("%-9s",
                     ipnet_cmd_rt_flags_to_str_short(rt->rtm_flags, IP_FALSE));

        ipcom_printf("%-6ld", rt->rtm_use);

        if (rt->rtm_index != 0)
            ipcom_printf("%-6s", ipcom_if_indextoname(rt->rtm_index, str));
        else
            ipcom_printf("%-6s", "*");
        ipcom_printf(" %-5ld", rt->rtm_rmx.rmx_hopcount);
        ipcom_printf("\n");
    }
 leave:
    ipcom_free(route_buf);
}


/*
 *===========================================================================
 *                    ipnet_netstat_print_entries
 *===========================================================================
 * Description: Prints the status of IPNET sockets.
 * Parameters:  af - The address family to print status for or -1 for all
 *              address family types.
 *              proto - The protocol to print status for or -1 for all
 *              protocols.
 *              show_unconnected - IP_TRUE if both connected and unconnected
 *              sockets status should be printed.
 *              no_dns - IP_TRUE if the addresses should not be resolved to
 *              there DNS names.
 *              show_procs - IP_TRUE if the PID owning a socket should be
 *              displayed.
 *              sock_data - buffer where information about all sockets
 *              should be stored.
 * Returns:
 *
 */
#ifndef IP_PORT_LAS
IP_STATIC void
ipnet_netstat_print_entries(int af,
                            int vr,
                            int proto,
                            Ip_bool show_unconnected,
                            Ip_bool no_dns,
                            Ip_bool show_procs,
                            struct Ipnet_tag_sock_data sock_data[])
{
    int                        p, x, a, y, fd;
    char                       str[IPNET_NETSTAT_STRLEN];
    struct Ipnet_tag_sock_data tag;
    int                        ret;
    Ip_u16                     dport;

    ipcom_memset(&tag, 0, sizeof(tag));
    tag.hdr.type = IPNET_CTRL_NEW_ENUM_SOCK_DATA;
    tag.hdr.size = sizeof(tag);
    for (fd = 0; ; fd++)
    {
        ret = ipnet_ctrl(&tag.hdr);
        if (ret == IPNET_CTRL_ENUM_DONE)
            break;
        if (ret < 0)
        {
            ipcom_printf("ipnet_ctrl() failed (%d)\n", -ret);
            return;
        }
        tag.hdr.type = IPNET_CTRL_CONT_ENUM_SOCK_DATA;
        sock_data[fd] = tag;
    }
    /* Create a end of socket list entry */
    sock_data[fd].fd = -1;

    for (y = 0; af_map[y].key != -1; y++)
    {
        if (af != -1)
            a = af;
        else
            a = af_map[y].key;
        ipcom_printf("%s sockets\n", ipcom_cmd_key_to_str(af_map, a, str, sizeof(str)));
        ipcom_printf("%-6s", "Prot");
#ifndef IP_PORT_VXWORKS
        ipcom_printf("%-5s", "Fd");
#endif
        ipcom_printf("%-7s%-7s", "Recv-Q", "Send-Q");
        ipcom_printf("%-30s%-30s%-12s",
            "Local Address", "Foreign Address", "State");
        if (show_procs)
#ifdef IP_PORT_VXWORKS
            ipcom_printf("TID");
#else
            ipcom_printf("PID");
#endif

        ipcom_printf("\n");
        for (x = 0; x < IP_IPPROTO_MAX ; x++)
        {
            if (proto == -1)
                p = x;
            else
                p = proto;

            for (fd = 0; sock_data[fd].fd >= 0; fd++)
            {
                tag = sock_data[fd];

                if (tag.protocol != p || tag.domain != a)
                    continue;

                if (tag.vr_index != vr)
                    continue;

                dport = ip_ntohs(((struct Ip_sockaddr_in *) &tag.daddr)->sin_port);
                if (!show_unconnected && dport == 0)
                    /* Show only connected sockets */
                    continue;

                ipcom_printf("%-5s ", ipcom_cmd_key_to_str(proto_map, tag.protocol, str, sizeof(str)));
#ifndef IP_PORT_VXWORKS
                ipcom_printf("%-4d ", tag.fd);
#endif
                ipcom_printf("%-6d %-6d ", tag.rcv_bytes, tag.snd_bytes);
#ifdef IPSCTP
                if ((IP_IPPROTO_SCTP == tag.protocol)
                    && (IP_NULL != tag.priv_data))
                {
                    Ipsctp_netstatinfo     *netstatinfo  = IP_NULL;
                    Ipsctp_netstat_tcbinfo *tcbinfo      = IP_NULL;
                    Ip_bool                 first        = IP_TRUE;
                    Ip_bool                 new_tcb      = IP_TRUE;
                    Ip_u32                  diff_count   = 0;
                    Ip_u32                  min_count    = 0;
                    Ip_u32                  which_big    = 0; /* 1:local,2:remote */
                    Ip_u32                  i            = 0;
                    Ip_u32                  j            = 0;

                    netstatinfo = tag.priv_data;

                    for (i = 0; i < netstatinfo->tcb_count; i++)
                    {
                        new_tcb = IP_TRUE;
                        tcbinfo = &netstatinfo->tcb[i];
                        if (tcbinfo->raddr_count <= netstatinfo->laddr_count)
                        {
                            which_big  = 1;
                            min_count  = tcbinfo->raddr_count;
                            diff_count = netstatinfo->laddr_count - tcbinfo->raddr_count;
                        }
                        else
                        {
                            which_big  = 2;
                            min_count  = netstatinfo->laddr_count;
                            diff_count = tcbinfo->raddr_count - netstatinfo->laddr_count;
                        }
                        for (j = 0; j < min_count; j++)
                        {
                            if (IP_TRUE == first)
                                first = IP_FALSE;
                            else
                                ipcom_printf("%-20s", " ");

                            ipcom_printf("%-29s ",
                                         ipcom_cmd_sockaddr_to_str(&netstatinfo->laddr[j].sa,
                                                                   IP_TRUE,
                                                                   no_dns,
                                                                   str,
                                                                   sizeof(str)));
                            ipcom_printf("%-29s ",
                                         ipcom_cmd_sockaddr_to_str(&tcbinfo->raddr[j].sa,
                                                                   IP_TRUE,
                                                                   no_dns,
                                                                   str,
                                                                   sizeof(str)));
                            if (IP_TRUE == new_tcb)
                            {
                                new_tcb = IP_FALSE;
                                /* 12 bytes is not enough */
                                ipcom_printf("%-12s", netstat_sctp_state_list[tcbinfo->state]);
                            }
                            ipcom_printf("\n");
                        }
                        for (; j < diff_count; j++)
                        {
                            switch (which_big)
                            {
                            case 1: /* local address count is bigger */
                                ipcom_printf("%-20s%-29s",
                                             " ",
                                             ipcom_cmd_sockaddr_to_str(&netstatinfo->laddr[j].sa,
                                                                       IP_TRUE,
                                                                       no_dns,
                                                                       str,
                                                                       sizeof(str)));
                                break;
                            case 2: /* remote address count is bigger */
                                ipcom_printf("%-50s%-29s",
                                             " ",
                                             ipcom_cmd_sockaddr_to_str(&tcbinfo->raddr[j].sa,
                                                                       IP_TRUE,
                                                                       no_dns,
                                                                       str,
                                                                       sizeof(str)));
                                break;
                            default:
                                break;
                            }
                            ipcom_printf("\n");
                        }
                    }

                    continue;
                }

#endif /* IPSCTP */
                ipcom_printf("%-29s ",
                    ipcom_cmd_sockaddr_to_str(&tag.saddr.addr, IP_TRUE, no_dns, str, sizeof(str)));
                ipcom_printf("%-29s ",
                    ipcom_cmd_sockaddr_to_str(&tag.daddr.addr, IP_TRUE, no_dns, str, sizeof(str)));

#ifdef IPTCP
                if (tag.protocol == IP_IPPROTO_TCP)
                    ipcom_printf("%-12s", tcp_state_list[tag.tcp_state]);
                else
#endif /* IPTCP */
                {
#ifdef IPSCTP
                    if (IP_IPPROTO_SCTP == tag.protocol)
                        ipcom_printf("%-12s", "CLOSED");
                    else
#endif /* IPSCTP */
                        ipcom_printf("%-12s", "");
                }

                if (show_procs)
#ifdef IP_PORT_VXWORKS
                    ipcom_printf("%x", tag.pid);
#else
                    ipcom_printf("%u", tag.pid);
#endif

                ipcom_printf("\n");
            }

            if (proto != -1)
                break;
        }
        ipcom_printf("\n");
        if (af != -1)
            break;
    }

#ifdef IPSCTP
    /* free SCTP private data */
    for (fd = 0; sock_data[fd].fd >= 0; fd++)
    {
        Ipsctp_netstatinfo *netstatinfo = IP_NULL;
        Ip_u32              i           = 0;

        if ((IP_IPPROTO_SCTP == sock_data[fd].protocol)
            && (IP_NULL != (netstatinfo = sock_data[fd].priv_data))) /* now, only SCTP use this */
        {
            /* free the memory allocate in ipnetd task */
            for (i = 0; i < netstatinfo->tcb_count; i++)
            {
                if (netstatinfo->tcb[i].raddr_count > 0)
                {
                    ipcom_free(netstatinfo->tcb[i].raddr);
                }
            }

            if (netstatinfo->laddr_count > 0)
            {
                ipcom_free(netstatinfo->laddr);
            }
            ipcom_free(netstatinfo->tcb);
            ipcom_free(netstatinfo);
        }
    }
#endif
}
#endif /* !IP_PORT_LAS */

/*=========================================================================== 
 *                      ipnet_netstat_init_stat_sig   
 *===========================================================================   
 * Description: Initializes the statistics signal   
 * Parameters:  sig - signal to initialize   
 *              proto - protocol to fetch information for.   
 * Returns:   
 *   
 */   
  #ifndef IP_PORT_LAS   
  IP_STATIC void   
  ipnet_netstat_init_stat_sig(Ipnet_sig *sig, int proto)   
  {   
      ipnet_usr_sock_init_sig(sig, IPNET_SIG_STATS);   
      sig->d.stats.proto = proto;   
  }   
  #endif   
     


/*
 *===========================================================================
 *                    ipnet_netstat_print_stats
 *===========================================================================
 * Description: Prints IPNET statistics.
 * Parameters:  proto - The protocol to display statistics for or IP_NULL
 *                      for all protocols.
 * Returns:
 *
 */
#ifndef IP_PORT_LAS

#define IPNET_NETSTAT_PRINT_IP          0x80
#define IPNET_NETSTAT_PRINT_ICMP        0x40
#define IPNET_NETSTAT_PRINT_TCP         0x20
#define IPNET_NETSTAT_PRINT_UDP         0x10
#define IPNET_NETSTAT_PRINT_SCTP        0x08
#define IPNET_NETSTAT_PRINT_IP6         0x04
#define IPNET_NETSTAT_PRINT_ICMP6       0x02

#define IPNET_NETSTAT_PRINT_RAW (IPNET_NETSTAT_PRINT_IP|IPNET_NETSTAT_PRINT_ICMP)
#define IPNET_NETSTAT_PRINT_ALL (~0)

IP_STATIC void
ipnet_netstat_print_stats(const char *proto)
{
    unsigned char mask;
    Ipnet_sig    *sig;


    sig = ipcom_calloc(1, sizeof(*sig));
    if (sig == IP_NULL)
    {
        ipcom_printf("Out of memory\n");
        return;
    }

    if (proto == IP_NULL)
        mask = IPNET_NETSTAT_PRINT_ALL;
    else if (ipcom_strcasecmp(proto, "raw") == 0)
        mask = IPNET_NETSTAT_PRINT_RAW;
    else if (ipcom_strcasecmp(proto, "ip") == 0)
        mask = IPNET_NETSTAT_PRINT_IP;
    else if (ipcom_strcasecmp(proto, "icmp") == 0)
        mask = IPNET_NETSTAT_PRINT_ICMP;
    else if (ipcom_strcasecmp(proto, "tcp") == 0)
        mask = IPNET_NETSTAT_PRINT_TCP;
    else if (ipcom_strcasecmp(proto, "udp") == 0)
        mask = IPNET_NETSTAT_PRINT_UDP;
#ifdef IPSCTP
    else if (ipcom_strcasecmp(proto, "sctp") == 0)
        mask = IPNET_NETSTAT_PRINT_SCTP;
#endif
    else if (ipcom_strcasecmp(proto, "ip6") == 0)
        mask = IPNET_NETSTAT_PRINT_IP6;
    else if (ipcom_strcasecmp(proto, "icmp6") == 0)
        mask = IPNET_NETSTAT_PRINT_ICMP6;
    else
        mask = 0;

    /* Print IP and ICMP stats */
    if (IP_BIT_ISSET(mask,IPNET_NETSTAT_PRINT_IP))
    {
        ipnet_netstat_init_stat_sig(sig, IP_IPPROTO_IP);
        ipcom_pipe_send(ipnet_usr_sock_get_pipe(), sig);
        ipcom_pipe_recv(sig->response_pipe, (void **)&sig);
        ip_assert(sig != IP_NULL);
        if (sig->d.stats.sig_errno == 0)
        {
            ipcom_printf("Ip:\n");
            ipcom_printf("    %u total packets received\n", sig->d.stats.p.ip.in_receives);
            if (sig->d.stats.p.ip.in_hdr_errors)
                ipcom_printf("    %u with invalid headers\n", sig->d.stats.p.ip.in_hdr_errors);
            if (sig->d.stats.p.ip.in_addr_errors)
                ipcom_printf("    %u with invalid addresses\n", sig->d.stats.p.ip.in_addr_errors);
            ipcom_printf("    %u forwarded\n", sig->d.stats.p.ip.forw_datagrams);
            if (sig->d.stats.p.ip.in_unknown_protos)
                ipcom_printf("    %u with unknown protocol\n", sig->d.stats.p.ip.in_unknown_protos);
            ipcom_printf("    %u incoming packets discarded\n", sig->d.stats.p.ip.in_discards);
            ipcom_printf("    %u incoming packets delivered\n", sig->d.stats.p.ip.in_delivers);
            ipcom_printf("    %u requests sent out\n", sig->d.stats.p.ip.out_requests);
            if (sig->d.stats.p.ip.out_discards)
                ipcom_printf("    %u outgoing packets dropped\n", sig->d.stats.p.ip.out_discards);
            if (sig->d.stats.p.ip.out_no_routes)
                ipcom_printf("    %u dropped because of missing route\n", sig->d.stats.p.ip.out_no_routes);
            if (sig->d.stats.p.ip.reasm_timeout)
                ipcom_printf("    %u fragments dropped after timeout\n", sig->d.stats.p.ip.reasm_timeout);
            if (sig->d.stats.p.ip.reasm_reqds)
                ipcom_printf("    %u reassemblies required\n", sig->d.stats.p.ip.reasm_reqds);
            if (sig->d.stats.p.ip.reasm_oks)
                ipcom_printf("    %u packets reassembled ok\n", sig->d.stats.p.ip.reasm_oks);
            if (sig->d.stats.p.ip.reasm_fails)
                ipcom_printf("    %u packet reassembles failed\n", sig->d.stats.p.ip.reasm_fails);
            if (sig->d.stats.p.ip.frag_oks)
                ipcom_printf("    %u fragments received ok\n", sig->d.stats.p.ip.frag_oks);
            if (sig->d.stats.p.ip.frag_fails)
                ipcom_printf("    %u fragments failed\n", sig->d.stats.p.ip.frag_fails);
            if (sig->d.stats.p.ip.frag_creates)
                ipcom_printf("    %u fragments created\n", sig->d.stats.p.ip.frag_creates);
        }
        else if (sig->d.stats.sig_errno != IP_ERRNO_EPROTONOSUPPORT)
            goto out;

    }

    if (IP_BIT_ISSET(mask,IPNET_NETSTAT_PRINT_ICMP))
    {
        ipnet_netstat_init_stat_sig(sig, IP_IPPROTO_ICMP);
        ipcom_pipe_send(ipnet_usr_sock_get_pipe(), sig);
        ipcom_pipe_recv(sig->response_pipe, (void **)&sig);
        ip_assert(sig != IP_NULL);

        if (sig->d.stats.sig_errno == 0)
        {
            ipcom_printf("Icmp:\n");
            ipcom_printf("    %u ICMP messages received\n", sig->d.stats.p.icmp.in_msgs);
            ipcom_printf("    %u input ICMP message failed\n", sig->d.stats.p.icmp.in_errors);
            ipcom_printf("    ICMP input histogram:\n");
            if (sig->d.stats.p.icmp.in_dest_unreachs)
                ipcom_printf("        destination unreachable: %u\n", sig->d.stats.p.icmp.in_dest_unreachs);
            if (sig->d.stats.p.icmp.in_time_excds)
                ipcom_printf("        timeout in transit: %u\n", sig->d.stats.p.icmp.in_time_excds);
            if (sig->d.stats.p.icmp.in_parm_probs)
                ipcom_printf("        wrong parameters: %u\n", sig->d.stats.p.icmp.in_parm_probs);
            if (sig->d.stats.p.icmp.in_src_quenchs)
                ipcom_printf("        source quenches: %u\n", sig->d.stats.p.icmp.in_src_quenchs);
            if (sig->d.stats.p.icmp.in_redirects)
                ipcom_printf("        redirects: %u\n", sig->d.stats.p.icmp.in_redirects);
            if (sig->d.stats.p.icmp.in_echos)
                ipcom_printf("        echo requests: %u\n", sig->d.stats.p.icmp.in_echos);
            if (sig->d.stats.p.icmp.in_echo_reps)
                ipcom_printf("        echo replies: %u\n", sig->d.stats.p.icmp.in_echo_reps);
            if (sig->d.stats.p.icmp.in_timestamps)
                ipcom_printf("        timestamp request: %u\n", sig->d.stats.p.icmp.in_timestamps);
            if (sig->d.stats.p.icmp.in_timestamp_reps)
                ipcom_printf("        timestamp reply: %u\n", sig->d.stats.p.icmp.in_timestamp_reps);
            if (sig->d.stats.p.icmp.in_addr_masks)
                ipcom_printf("        address mask request: %u\n", sig->d.stats.p.icmp.in_addr_masks);
            if (sig->d.stats.p.icmp.in_addr_mask_reps)
                ipcom_printf("        address mask replies: %u\n", sig->d.stats.p.icmp.in_addr_mask_reps);
            ipcom_printf("    %u ICMP messages sent\n", sig->d.stats.p.icmp.out_msgs);
            ipcom_printf("    %u ICMP messages failed\n", sig->d.stats.p.icmp.out_errors);
            ipcom_printf("    ICMP output histogram:\n");
            if (sig->d.stats.p.icmp.out_dest_unreachs)
                ipcom_printf("        destination unreachable: %u\n", sig->d.stats.p.icmp.out_dest_unreachs);
            if (sig->d.stats.p.icmp.out_time_excds)
                ipcom_printf("        time exceeded: %u\n", sig->d.stats.p.icmp.out_time_excds);
            if (sig->d.stats.p.icmp.out_parm_probs)
                ipcom_printf("        wrong parameters: %u\n", sig->d.stats.p.icmp.out_parm_probs);
            if (sig->d.stats.p.icmp.out_src_quenchs)
                ipcom_printf("        source quench: %u\n", sig->d.stats.p.icmp.out_src_quenchs);
            if (sig->d.stats.p.icmp.out_redirects)
                ipcom_printf("        redirect: %u\n", sig->d.stats.p.icmp.out_redirects);
            if (sig->d.stats.p.icmp.out_echos)
                ipcom_printf("        echo request: %u\n", sig->d.stats.p.icmp.out_echos);
            if (sig->d.stats.p.icmp.out_echo_reps)
                ipcom_printf("        echo replies: %u\n", sig->d.stats.p.icmp.out_echo_reps);
            if (sig->d.stats.p.icmp.out_timestamps)
                ipcom_printf("        timestamp requests: %u\n", sig->d.stats.p.icmp.out_timestamps);
            if (sig->d.stats.p.icmp.out_timestamp_reps)
                ipcom_printf("        timestamp replies: %u\n", sig->d.stats.p.icmp.out_timestamp_reps);
            if (sig->d.stats.p.icmp.out_addr_masks)
                ipcom_printf("        address mask requests: %u\n", sig->d.stats.p.icmp.out_addr_masks);
            if (sig->d.stats.p.icmp.out_addr_mask_reps)
                ipcom_printf("        address mask replies: %u\n", sig->d.stats.p.icmp.out_addr_mask_reps);
        }
        else if (sig->d.stats.sig_errno != IP_ERRNO_EPROTONOSUPPORT)
            goto out;
    }

    /* Print TCP stats */
    if (IP_BIT_ISSET(mask,IPNET_NETSTAT_PRINT_TCP))
    {
        ipnet_netstat_init_stat_sig(sig, IP_IPPROTO_TCP);
        ipcom_pipe_send(ipnet_usr_sock_get_pipe(), sig);
        ipcom_pipe_recv(sig->response_pipe, (void **)&sig);
        ip_assert(sig != IP_NULL);
        if (sig->d.stats.sig_errno == 0)
        {
            ipcom_printf("Tcp:\n");
            ipcom_printf("    %u active connection openings\n", sig->d.stats.p.tcp.active_opens);
            ipcom_printf("    %u passive connection openings\n", sig->d.stats.p.tcp.passive_opens);
            ipcom_printf("    %u failed connection attempts\n", sig->d.stats.p.tcp.attempt_fails);
            ipcom_printf("    %u connection resets received\n", sig->d.stats.p.tcp.estab_resets);
            ipcom_printf("    %u connections established\n", sig->d.stats.p.tcp.curr_estab);
            ipcom_printf("    %u segments received\n", sig->d.stats.p.tcp.in_segs);
            ipcom_printf("    %u segments send out\n", sig->d.stats.p.tcp.out_segs);
            ipcom_printf("    %u segments retransmitted\n", sig->d.stats.p.tcp.retrans_segs);
            ipcom_printf("    %u bad segments received\n", sig->d.stats.p.tcp.in_errs);
            ipcom_printf("    %u resets sent\n", sig->d.stats.p.tcp.out_rsts);
        }
        else if (sig->d.stats.sig_errno != IP_ERRNO_EPROTONOSUPPORT)
            goto out;

    }

    /* Print UDP stats */
    if (IP_BIT_ISSET(mask,IPNET_NETSTAT_PRINT_UDP))
    {
        ipnet_netstat_init_stat_sig(sig, IP_IPPROTO_UDP);
        ipcom_pipe_send(ipnet_usr_sock_get_pipe(), sig);
        ipcom_pipe_recv(sig->response_pipe, (void **)&sig);
        ip_assert(sig != IP_NULL);
        if (sig->d.stats.sig_errno == 0)
        {
            ipcom_printf("Udp:\n");
            ipcom_printf("    %u packets received\n", sig->d.stats.p.udp.in_datagrams);
            ipcom_printf("    %u packets to unknown port received\n", sig->d.stats.p.udp.no_ports);
            ipcom_printf("    %u packet receive errors\n", sig->d.stats.p.udp.in_errors);
            ipcom_printf("    %u packets sent\n", sig->d.stats.p.udp.out_datagrams);
        }
        else if (sig->d.stats.sig_errno != IP_ERRNO_EPROTONOSUPPORT)
            goto out;
    }

    if (IP_BIT_ISSET(mask,IPNET_NETSTAT_PRINT_IP6))
    {
        ipnet_netstat_init_stat_sig(sig, IP_IPPROTO_IPV6);
        ipcom_pipe_send(ipnet_usr_sock_get_pipe(), sig);
        ipcom_pipe_recv(sig->response_pipe, (void **)&sig);
        ip_assert(sig != IP_NULL);

        if (sig->d.stats.sig_errno == 0)
        {
            ipcom_printf("Ip6:\n");
            ipcom_printf("    %u  total packets received\n", sig->d.stats.p.ip6.ip6_input);
            ipcom_printf("    %u  packets received with error\n", sig->d.stats.p.ip6.ip6_input_err);
            ipcom_printf("    %u  with header Truncation\n", sig->d.stats.p.ip6.ip6_input_hdr_trunc);
            ipcom_printf("    %u  with invalid data length \n", sig->d.stats.p.ip6.ip6_input_inval_len);
            ipcom_printf("    %u  with invalid extend header\n", sig->d.stats.p.ip6.ip6_input_inval_exthdr);
            ipcom_printf("    %u  packets received not to me\n", sig->d.stats.p.ip6.ip6_input_not_to_me);
            ipcom_printf("    %u  with data size too big\n", sig->d.stats.p.ip6.ip6_input_too_big);
            ipcom_printf("    %u  packets received with time exceeded\n", sig->d.stats.p.ip6.ip6_input_time_exceeded);
            ipcom_printf("    %u  with data size too short\n", sig->d.stats.p.ip6.ip6_input_too_short);
            ipcom_printf("    %u  with tcp multicast\n", sig->d.stats.p.ip6.ip6_input_tcp_multicast);
            ipcom_printf("    %u  with protocol error\n", sig->d.stats.p.ip6.ip6_input_prot_err);
            ipcom_printf("    %u  with header no next\n", sig->d.stats.p.ip6.ip6_input_hdr_no_next);
            ipcom_printf("    %u  packets received for raw\n", sig->d.stats.p.ip6.ip6_input_raw);
            ipcom_printf("    %u  total packets sent out\n", sig->d.stats.p.ip6.ip6_output);
            ipcom_printf("    %u  packets sent with error\n", sig->d.stats.p.ip6.ip6_output_err);
            ipcom_printf("    %u  packets sent with black hole\n", sig->d.stats.p.ip6.ip6_output_blackhole);
            ipcom_printf("    %u  packets sent with header space\n", sig->d.stats.p.ip6.ip6_output_hdr_space);
            ipcom_printf("    %u  packets sent with no route\n", sig->d.stats.p.ip6.ip6_output_noroute);
            ipcom_printf("    %u  packets sent with no memory\n", sig->d.stats.p.ip6.ip6_output_nomem);
            ipcom_printf("    %u  packets sent with no enough buffer\n", sig->d.stats.p.ip6.ip6_output_enobufs);
            ipcom_printf("    %u  packets sent with error probe\n", sig->d.stats.p.ip6.ip6_output_err_probe);
            ipcom_printf("    %u  packets sent with link error\n", sig->d.stats.p.ip6.ip6_output_link_err);
        }
        else if (sig->d.stats.sig_errno != IP_ERRNO_EPROTONOSUPPORT)
            goto out;

    }

    if (IP_BIT_ISSET(mask,IPNET_NETSTAT_PRINT_ICMP6))
    {
        ipnet_netstat_init_stat_sig(sig, IP_IPPROTO_ICMPV6);
        ipcom_pipe_send(ipnet_usr_sock_get_pipe(), sig);
        ipcom_pipe_recv(sig->response_pipe, (void **)&sig);
        ip_assert(sig != IP_NULL);
        if (sig->d.stats.sig_errno == 0)
        {
            ipcom_printf("icmp6\n");
            ipcom_printf("    %u  packets received\n", sig->d.stats.p.icmp6.icmp6_input);
            ipcom_printf("    %u  packets received with error\n", sig->d.stats.p.icmp6.icmp6_input_err);
            ipcom_printf("    %u  packets received with bad checksums\n", sig->d.stats.p.icmp6.icmp6_input_badchksum);
            ipcom_printf("    %u  packets received with no enough memory\n", sig->d.stats.p.icmp6.icmp6_input_nomem);
            ipcom_printf("    %u  no enough memory to send\n", sig->d.stats.p.icmp6.icmp6_send_nomem);
        }
        else if (sig->d.stats.sig_errno != IP_ERRNO_EPROTONOSUPPORT)
            goto out;  
    }

    if (IP_BIT_ISSET(mask,IPNET_NETSTAT_PRINT_IP6))
    {
        ipnet_netstat_init_stat_sig(sig, IP_IPPROTO_IPV6);
        ipcom_pipe_send(ipnet_usr_sock_get_pipe(), sig);
        ipcom_pipe_recv(sig->response_pipe, (void **)&sig);
        ip_assert(sig != IP_NULL);

        if (sig->d.stats.sig_errno == 0)
        {
            ipcom_printf("Ip6:\n");
            ipcom_printf("    %u  total packets received\n", sig->d.stats.p.ip6.ip6_input);
            ipcom_printf("    %u  packets received with error\n", sig->d.stats.p.ip6.ip6_input_err);
            ipcom_printf("    %u  with header Truncation\n", sig->d.stats.p.ip6.ip6_input_hdr_trunc);
            ipcom_printf("    %u  with invalid data length \n", sig->d.stats.p.ip6.ip6_input_inval_len);
            ipcom_printf("    %u  with invalid extend header\n", sig->d.stats.p.ip6.ip6_input_inval_exthdr);
            ipcom_printf("    %u  packets received not to me\n", sig->d.stats.p.ip6.ip6_input_not_to_me);
            ipcom_printf("    %u  with data size too big\n", sig->d.stats.p.ip6.ip6_input_too_big);
            ipcom_printf("    %u  packets received with time exceeded\n", sig->d.stats.p.ip6.ip6_input_time_exceeded);
            ipcom_printf("    %u  with data size too short\n", sig->d.stats.p.ip6.ip6_input_too_short);
            ipcom_printf("    %u  with tcp multicast\n", sig->d.stats.p.ip6.ip6_input_tcp_multicast);
            ipcom_printf("    %u  with protocol error\n", sig->d.stats.p.ip6.ip6_input_prot_err);
            ipcom_printf("    %u  with header no next\n", sig->d.stats.p.ip6.ip6_input_hdr_no_next);
            ipcom_printf("    %u  packets received for raw\n", sig->d.stats.p.ip6.ip6_input_raw);
            ipcom_printf("    %u  total packets sent out\n", sig->d.stats.p.ip6.ip6_output);
            ipcom_printf("    %u  packets sent with error\n", sig->d.stats.p.ip6.ip6_output_err);
            ipcom_printf("    %u  packets sent with black hole\n", sig->d.stats.p.ip6.ip6_output_blackhole);
            ipcom_printf("    %u  packets sent with header space\n", sig->d.stats.p.ip6.ip6_output_hdr_space);
            ipcom_printf("    %u  packets sent with no route\n", sig->d.stats.p.ip6.ip6_output_noroute);
            ipcom_printf("    %u  packets sent with no memory\n", sig->d.stats.p.ip6.ip6_output_nomem);
            ipcom_printf("    %u  packets sent with no enough buffer\n", sig->d.stats.p.ip6.ip6_output_enobufs);
            ipcom_printf("    %u  packets sent with error probe\n", sig->d.stats.p.ip6.ip6_output_err_probe);
            ipcom_printf("    %u  packets sent with link error\n", sig->d.stats.p.ip6.ip6_output_link_err);
        }
        else if (sig->d.stats.sig_errno != IP_ERRNO_EPROTONOSUPPORT)
            goto out;

    }

    if (IP_BIT_ISSET(mask,IPNET_NETSTAT_PRINT_ICMP6))
    {
        ipnet_netstat_init_stat_sig(sig, IP_IPPROTO_ICMPV6);
        ipcom_pipe_send(ipnet_usr_sock_get_pipe(), sig);
        ipcom_pipe_recv(sig->response_pipe, (void **)&sig);
        ip_assert(sig != IP_NULL);
        if (sig->d.stats.sig_errno == 0)
        {
            ipcom_printf("icmp6\n");
            ipcom_printf("    %u  packets received\n", sig->d.stats.p.icmp6.icmp6_input);
            ipcom_printf("    %u  packets received with error\n", sig->d.stats.p.icmp6.icmp6_input_err);
            ipcom_printf("    %u  packets received with bad checksums\n", sig->d.stats.p.icmp6.icmp6_input_badchksum);
            ipcom_printf("    %u  packets received with no enough memory\n", sig->d.stats.p.icmp6.icmp6_input_nomem);
            ipcom_printf("    %u  no enough memory to send\n", sig->d.stats.p.icmp6.icmp6_send_nomem);
        }
        else if (sig->d.stats.sig_errno != IP_ERRNO_EPROTONOSUPPORT)
            goto out;  
    }

#if defined(IPSCTP) && defined(IPSCTP_MIB)
    /* Print SCTP stats */
    if (IP_BIT_ISSET(mask,IPNET_NETSTAT_PRINT_SCTP))
    {
        ipnet_netstat_init_stat_sig(sig, IP_IPPROTO_SCTP);
        ipcom_pipe_send(ipnet_usr_sock_get_pipe(), sig);
        ipcom_pipe_recv(sig->response_pipe, (void **)&sig);
        ip_assert(sig != IP_NULL);
        if (sig->d.stats.sig_errno == 0)
        {
            ipcom_printf("Sctp:\n");
            ipcom_printf("    %u current established associations\n", sig->d.stats.p.sctp.sctp_Curr_Estab);
            ipcom_printf("    %u active established associations\n", sig->d.stats.p.sctp.sctp_Active_Estabs);
            ipcom_printf("    %u passive established associations\n", sig->d.stats.p.sctp.sctp_Passive_Estabs);
            ipcom_printf("    %u aborted associations\n", sig->d.stats.p.sctp.sctp_Aborteds);
            ipcom_printf("    %u shutdown associations\n", sig->d.stats.p.sctp.sctp_Shutdowns);
            ipcom_printf("    %u out of the blue packets received\n", sig->d.stats.p.sctp.sctp_OutOfBlues);
            ipcom_printf("    %u invalid checksum packets received\n", sig->d.stats.p.sctp.sctp_Checksum_Errors);
            ipcom_printf("    %llu control chunks sent\n", sig->d.stats.p.sctp.sctp_Out_Ctrl_Chunks);
            ipcom_printf("    %llu ordered data chunks sent\n", sig->d.stats.p.sctp.sctp_Out_Order_Chunks);
            ipcom_printf("    %llu unordered data chunks sent\n", sig->d.stats.p.sctp.sctp_Out_Unorder_Chunks);
            ipcom_printf("    %llu control chunks received\n", sig->d.stats.p.sctp.sctp_In_Ctrl_Chunks);
            ipcom_printf("    %llu ordered data chunks received\n", sig->d.stats.p.sctp.sctp_In_Order_Chunks);
            ipcom_printf("    %llu unordered data chunks received\n", sig->d.stats.p.sctp.sctp_In_Unorder_Chunks);
            ipcom_printf("    %llu fragmented user messages\n", sig->d.stats.p.sctp.sctp_Frag_UsrMsgs);
            ipcom_printf("    %llu reassembled user messages\n", sig->d.stats.p.sctp.sctp_Reasm_UsrMsgs);
            ipcom_printf("    %llu packets sent\n", sig->d.stats.p.sctp.sctp_Out_SCTP_Packs);
            ipcom_printf("    %llu packets received\n", sig->d.stats.p.sctp.sctp_In_SCTP_Packs);
            ipcom_printf("    %u discontinuity time\n", sig->d.stats.p.sctp.sctp_Discontinuity_Time);
        }
        else if (sig->d.stats.sig_errno != IP_ERRNO_EPROTONOSUPPORT)
            goto out;  
    }
#endif

out:
    if (sig->d.stats.sig_errno && mask)
    {
        ipcom_printf("%s\n", ipcom_strerror(sig->d.stats.sig_errno));
        if (sig->d.stats.sig_errno == IP_ERRNO_ENOENT)
            ipcom_printf("Enable MIB-2 support to collect networking statistics.\n");
    }
    ipcom_free(sig);
}
#endif /* !IP_PORT_LAS */


/*
 *===========================================================================
 *                    ipnet_netstat_print_usage
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_netstat_print_usage(void)
{
    ipcom_printf(IP_LF
                 "NAME"IP_LF
                 "     netstat - show network status"IP_LF
                 IP_LF
                 "SYNOPSIS"IP_LF
                 "     netstat -h"IP_LF
                 "     netstat [-an] [-f address_family] [-V vr] [-p protocol]"IP_LF
                 "     netstat -r [-nl] [-f address_family] [-V vr]"IP_LF
                 "     netstat -s [raw|ip|icmp|sctp|udp|tcp|ip6|icmp6]"IP_LF
                 IP_LF
                 "DESCRIPTION"IP_LF
                 "     The netstat command symbolically displays the contents of various"IP_LF
                 "     network-related data structures."IP_LF
                 IP_LF
                 "     -a    Show the state of all sockets; normally sockets used by server"IP_LF
                 "           processes are not shown."IP_LF
                 "     -f <address_family>"IP_LF
                 "           Limit statistics or address control block reports to those of the"IP_LF
                 "           specified address family.  The following address families are"IP_LF
                 "           recognized: inet, for IP_AF_INET; inet6, for IP_AF_INET6."IP_LF
                 "     -h    Help, this usage menu"IP_LF
                 "     -l    Do not show routes where the LLINFO flag is set."IP_LF
                 "     -n    Show network addresses as numbers. Default is to lookup symbolic"IP_LF
                 "           names for the addresses."IP_LF
                 "     -p <protocol>"IP_LF
                 "           Show statistics about protocol, which is either a well-known name"IP_LF
                 "           for a protocol or an alias for it."IP_LF
                 "     -r    Show the routing tables"IP_LF
                 "     -P    Show which process that owns the socket."IP_LF
                 "     -T <table> Specify route table ID. %d if not specified."IP_LF, IPCOM_ROUTE_TABLE_DEFAULT);
    ipcom_printf("     -V <vr> Specify virtual router index. 0 if not specified."IP_LF
                 "     -s    Display networking statistics."IP_LF
                 IP_LF);
}


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_cmd_netstat
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipnet_cmd_netstat(int argc, char **argv)
{
    int          i, c;
    int          proto = -1;
    int          af = -1;
    const char   *proto_name = IP_NULL;
    const char   *stats_name = IP_NULL;
#if IPCOM_VR_MAX > 1
    int          vr = ipcom_proc_vr_get();
    int          vr_new = vr;
#else
    int          vr_new = 0;
#endif
    int          table = IPCOM_ROUTE_TABLE_DEFAULT;
    Ip_bool      show_unconnected = IP_FALSE;
    Ip_bool      no_dns = IP_FALSE;
    Ip_bool      show_routes = IP_FALSE;
    Ip_bool      show_llinfo = IP_TRUE;
    Ip_bool      show_processes = IP_FALSE;
    Ip_bool      show_stats     = IP_FALSE;
    Ipcom_getopt opt;

    ipcom_getopt_clear_r(&opt);
    while ((c = ipcom_getopt_r(argc, argv, "?hHaf:p:PT:nrV:ls", &opt)) != -1)
    {
        switch (c)
        {
        case '?':
        case 'h':
        case 'H':
            goto errout;
        case 'a':
            show_unconnected = IP_TRUE;
            break;
        case 'f':
            for (i = 0; af_map[i].key != -1; i++)
                if (ipcom_strcasecmp(opt.optarg, af_map[i].str) == 0)
                {
                    af = af_map[i].key;
                    break;
                }
            if (af_map[i].key == -1)
                goto errout;
            break;
        case 'p':
            for (i = 0; proto_map[i].key != -1; i++)
                if (ipcom_strcasecmp(opt.optarg, proto_map[i].str) == 0)
                {
                    proto       = proto_map[i].key;
                    proto_name  = opt.optarg;
                    break;
                }
            if (proto_map[i].key == -1)
                goto errout;
            break;
        case 'P':
            show_processes = IP_TRUE;
            break;
        case 'T':
            table = ipcom_atoi(opt.optarg);
            break;
        case 'r':
            show_routes = IP_TRUE;
            break;
        case 'n':
            no_dns = IP_TRUE;
            break;
#if IPCOM_VR_MAX > 1
        case 'V':
            vr_new = ipcom_atoi(opt.optarg);
            break;
#endif
        case 'l':
            show_llinfo = IP_FALSE;
            break;
        case 's':
            show_stats = IP_TRUE;

            /* Is there a next argument, and is it a non-switch? */
            if (argv[opt.optind] != IP_NULL && argv[opt.optind][0] != '-')
                stats_name = argv[opt.optind];
            break;
        default:
            break;
        }
    }

    if (show_stats)
    {
#ifndef IP_PORT_LAS
        ipnet_netstat_print_stats(stats_name? stats_name : proto_name);
#endif
        return 0;
    }

#if IPCOM_VR_MAX > 1
    if (vr != vr_new)
        ipcom_proc_vr_set(vr_new);
#endif

#ifdef IP_PORT_LAS
    ipnet_netstat_print_routes(af == -1 ? 0 : af, table, no_dns, show_llinfo);
#else
    if (show_routes)
        ipnet_netstat_print_routes(af == -1 ? 0 : af, table, no_dns, show_llinfo);
    else
    {
        struct Ipnet_tag_sock_data *tsd;

        tsd = ipcom_calloc(ipnet_conf_max_sockets + 1, sizeof(struct Ipnet_tag_sock_data));
        if (tsd == IP_NULL)
            ipcom_printf("Out of memory"IP_LF);
        else
            ipnet_netstat_print_entries(af,
                                        vr_new,
                                        proto,
                                        show_unconnected,
                                        no_dns,
                                        show_processes,
                                        tsd);
        ipcom_free(tsd);
    }
#endif

#if IPCOM_VR_MAX > 1
    if (vr != vr_new)
        ipcom_proc_vr_set(vr);
#endif

    ipcom_printf("\n");
    return 0;

errout:
    ipnet_netstat_print_usage();
    return 1;
}

/*****************************************************************************
 函 数 名  : IPS_MNTN_CCORE_GetNetstatEntries
 功能描述  : 获取连接状态信息
 输入参数  : pucData        -   数据指针
             pulTotalLen    -   总信息长度
 输出参数  : 无
 返 回 值  : OK         -   获取系统信息成功
             ERROR      -   获取系统信息失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月9日
    作    者   : 冯家麟
    修改内容   : 新生成函数
  2.日    期   : 2013年7月30日
    作    者   : caikai
    修改内容   : VoLTE特性需要将其移植至V9

*****************************************************************************/
IP_PUBLIC int IPS_MNTN_CCORE_GetNetstatEntries
(
    Ip_u8                              *pucData,
    Ip_u32                             *pulTotalLen,
    Ip_u16                              usType
)
{
    struct Ipnet_tag_sock_data  stTag;
    Ip_u8                      *pucLenCur;
    Ip_u32                      ulSysDataLen;
    Ip_u32                      ulLen;
    int                         lRet;
    int                         lIndex;
    int                         lDomain;
    int                         iLoop;

    /*填充消息类型*/
    ipcom_memcpy(pucData, &usType, IPS_MNTN_CCORE_INFO_TYPE_LEN);
    pucData                 +=  IPS_MNTN_CCORE_INFO_TYPE_OFFSET;
    (*pulTotalLen)          +=  IPS_MNTN_CCORE_INFO_TYPE_OFFSET;

    /*记录填充信息长度的游标地址*/
    pucLenCur               =   pucData;
    pucData                 +=  IPS_MNTN_CCORE_INFO_LEN_OFFSET;
    (*pulTotalLen)          +=  IPS_MNTN_CCORE_INFO_LEN_OFFSET;

    ulSysDataLen            =   0;

    for (lIndex = 0; af_map[lIndex].key != -1; lIndex++)
    {
        lDomain = af_map[lIndex].key;
        stTag.hdr.type = IPNET_CTRL_NEW_ENUM_SOCK_DATA;
        stTag.hdr.size = sizeof(stTag);
        for (iLoop = 0; iLoop < IPS_MNTN_CCORE_MAX_LOOP_CNT; iLoop++)
        {
            /*获取连接状态信息*/
            lRet = ipnet_ctrl(&stTag.hdr);
            if (lRet == IPNET_CTRL_ENUM_DONE)
                break;
            if (lRet < 0)
            {
                ipcom_printf("IPS_MNTN_CCORE_GetNetstatEntries: ipnet_ctrl() failed (%d)\n", -lRet);
                return -1;
            }
            stTag.hdr.type = IPNET_CTRL_CONT_ENUM_SOCK_DATA;

            if ((stTag.protocol < 0) || (stTag.protocol >= IP_IPPROTO_MAX))
                continue;

            if (stTag.domain != lDomain)
                continue;

            if (stTag.vr_index != 0)
                continue;


            /*如果加入该项信息，可维可测信息总长度超预期，则获取信息失败*/
            ulLen = *pulTotalLen + ulSysDataLen + sizeof(stTag);
            if(IPS_MNTN_CCORE_INFO_LEN < ulLen)
            {
                ipcom_memcpy(pucLenCur, &ulSysDataLen, IPS_MNTN_CCORE_INFO_LEN_OFFSET);
                (*pulTotalLen)         +=   ulSysDataLen;
                ipcom_printf("IPS_MNTN_CCORE_GetNetstatEntries: len is over expect. len =%d"IP_LF,ulLen);
                return -1;
            }

            /*填充连接状态信息*/
            ipcom_memcpy(pucData, &stTag, sizeof(stTag));
            pucData         +=  sizeof(stTag);
            ulSysDataLen    +=  sizeof(stTag);
        }
    }

    /*填充连接状态总长度*/
    ipcom_memcpy(pucLenCur, &ulSysDataLen, IPS_MNTN_CCORE_INFO_LEN_OFFSET);
    (*pulTotalLen)         +=   ulSysDataLen;

    return 0;
}

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

