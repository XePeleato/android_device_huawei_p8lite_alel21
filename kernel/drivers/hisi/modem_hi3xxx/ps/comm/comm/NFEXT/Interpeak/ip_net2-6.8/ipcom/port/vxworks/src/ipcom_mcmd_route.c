/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_mcmd_route.c,v $ $Revision: 1.3 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_mcmd_route.c,v $
 *   $Author: kenneth $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * This is a reduced version of ipnet_cmd_route.c. The file is used to
 * make route command utility binary scalable.
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

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_netif.h"
#include "ipnet_routesock.h"
#include "ipnet_cmd.h"

#ifdef IPMPLS
#include "ipnet_mpls.h"
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define CMD_LEAVE(retval)   do { ret = retval; goto leave; } while((0))

#define IPNET_CMD_ROUTE_BUF_SIZE   1024    /* Buffer size when reading messages */

#define IPNET_ROUTE_INDENT     "    "

#define IPNET_CMD_ROUTE_ADD        0   /* Add a route */
#define IPNET_CMD_ROUTE_FLUSH      1   /* Remove all routes */
#define IPNET_CMD_ROUTE_SHOW       5   /* Print out the route table similar to "netstat -r" */

#define IPNET_CMD_ROUTE_NAME_BUF   256

/* generic */
#define IPNET_CMD_ROUTE_SWITCH_EOL          -1
#define IPNET_CMD_ROUTE_SWITCH_NO_DNS       -2
#define IPNET_CMD_ROUTE_SWITCH_DEVICE       -3
#define IPNET_CMD_ROUTE_SWITCH_SILENT       -100
#define IPNET_CMD_ROUTE_SWITCH_NO_LLINFO    -101
/* IP domain */
#define IPNET_CMD_ROUTE_SWITCH_INET         -11
#define IPNET_CMD_ROUTE_SWITCH_INET6        -12
/* Mask */
#define IPNET_CMD_ROUTE_SWITCH_NETMASK      -21
#define IPNET_CMD_ROUTE_SWITCH_PREFIXLEN    -22
/* Address type */
#define IPNET_CMD_ROUTE_SWITCH_HOST         -31
#define IPNET_CMD_ROUTE_SWITCH_NET          -32
/* Route flags */
#define IPNET_CMD_ROUTE_SWITCH_CLONING      -41
#define IPNET_CMD_ROUTE_SWITCH_IFACE        -42
#define IPNET_CMD_ROUTE_SWITCH_STATIC       -43
#define IPNET_CMD_ROUTE_SWITCH_NOSTATIC     -44
#define IPNET_CMD_ROUTE_SWITCH_REJECT       -45
#define IPNET_CMD_ROUTE_SWITCH_BLACKHOLE    -46
#define IPNET_CMD_ROUTE_SWITCH_LLINFO       -47
#define IPNET_CMD_ROUTE_SWITCH_XRESOLVE     -48
#define IPNET_CMD_ROUTE_SWITCH_PROTO1       -49
#define IPNET_CMD_ROUTE_SWITCH_PROTO2       -50
#define IPNET_CMD_ROUTE_SWITCH_PREF         -51
#define IPNET_CMD_ROUTE_SWITCH_TUNNELEND    -52
#define IPNET_CMD_ROUTE_SWITCH_MBINDING     -53
/* Metric flags */
#define IPNET_CMD_ROUTE_SWITCH_RTT          -61
#define IPNET_CMD_ROUTE_SWITCH_RTTVAR       -62
#define IPNET_CMD_ROUTE_SWITCH_MTU          -63
#define IPNET_CMD_ROUTE_SWITCH_HOPCOUNT     -64
#define IPNET_CMD_ROUTE_SWITCH_EXPIRE       -65
/* Virtual router flags */
#define IPNET_CMD_ROUTE_SWITCH_VR_BYNAME    -74
/* Policy routing tables */
#define IPNET_CMD_ROUTE_SWITCH_RT_TABLE     -80
/* MPLS shortcut routes */
#define IPNET_CMD_ROUTE_SWITCH_MPLS_PW      -90

#define IPNET_CMD_ROUTE_CHECK_SILENT(statement)  do { if (!silent) statement; } while((0))

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

#ifdef IPCOM_MCMD_ROUTE_SHOW
IP_STATIC Ipcom_cmd_int_str_map ipnet_rtm_map[] =
{
    { IPNET_RTM_ADD,        "RTM_ADD" },
    { IPNET_RTM_DELETE,     "RTM_DELETE" },
    { IPNET_RTM_CHANGE,     "RTM_CHANGE" },
    { IPNET_RTM_GET,        "RTM_GET" },
    { IPNET_RTM_LOOSING,    "RTM_LOOSING" },
    { IPNET_RTM_REDIRECT,   "RTM_REDIRECT" },
    { IPNET_RTM_MISS,       "RTM_MISS" },
    { IPNET_RTM_LOCK,       "RTM_LOCK" },
    { IPNET_RTM_RESOLVE,    "RTM_RESOLVE" },
    { IPNET_RTM_NEWADDR,    "RTM_NEWADDR" },
    { IPNET_RTM_DELADDR,    "RTM_DELADDR" },
    { IPNET_RTM_IFINFO,     "RTM_IFINFO" },
    { IPNET_RTM_IFANNOUNCE, "RTM_IFANNOUNCE" },
    { IPNET_RTM_NEWVR,      "RTM_NEWVR" },
    { IPNET_RTM_DELVR,      "RTM_DELVR" },
    { -1, IP_NULL }
};

IP_STATIC Ipcom_cmd_int_str_map ipnet_rta_map[] =
{
    { IPNET_RTAX_DST,      "RTA_DST" },
    { IPNET_RTAX_GATEWAY,  "RTA_GATEWAY" },
    { IPNET_RTAX_NETMASK,  "RTA_NETMASK" },
    { IPNET_RTAX_GENMASK,  "RTA_GENMASK" },
    { IPNET_RTAX_IFP,      "RTA_IFP" },
    { IPNET_RTAX_IFA,      "RTA_IFA" },
    { IPNET_RTAX_AUTHOR,   "RTA_AUTHOR" },
    { IPNET_RTAX_BRD,      "RTA_BRD" },
    { -1, IP_NULL }
};

IP_STATIC Ipcom_cmd_int_str_map ipnet_if_type_map[] =
{
    { IP_IFT_OTHER,       "Other" },
    { IP_IFT_ETHER,       "Ethernet" },
    { IP_IFT_PPP,         "PPP" },
    { IP_IFT_LOOP,        "Loopback" },
    { IP_IFT_PPPOE,       "PPPoE" },
    { IP_IFT_IEEE80211,   "802.11" },
    { IP_IFT_TUNNEL,      "Tunnel" },
    { IP_IFT_L2VLAN,      "VLAN" },
    { IP_IFT_MPLS_TUNNEL, "MPLS"},
    { -1, IP_NULL }
};
#endif /* IPCOM_MCMD_ROUTE_SHOW */


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_cmd_route_create_mask
 *===========================================================================
 * Description: Creates a network mask with the 'netbits' leftmost bits set.
 * Parameters:  [in] mask - Pointer to a buffer where the mask should be stored.
 *              [in] netbits - The number of set bits.
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_route_create_mask(void *mask, int netbits)
{
    int i;

    for (i = 0; i < netbits; i++)
    {
        IP_BIT_SET(((Ip_u16*) mask)[i / 16], ip_htons(0x8000 >> (i % 16)));
    }
}


/*
 *===========================================================================
 *                    ipnet_cmd_route_set_mask_from_prefixlen
 *===========================================================================
 * Description: Sets the route mask from prefixlen.
 * Parameters:  cmd - The command structure.
 *              max_prefixlen - The maximum value the prefixlen can have.
 *              prefixlen - The number of leading ones in the mask.
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_route_set_mask_from_prefixlen(Ipnet_cmd_route *cmd,
                                        int max_prefixlen,
                                        const char *str_prefixlen)
{
    int prefixlen;

    prefixlen = ipcom_atoi(str_prefixlen);
    if (prefixlen < 0 || prefixlen > max_prefixlen)
        return -IP_ERRNO_EINVAL;

    cmd->netmask.sa.sa_len    = sizeof(union Ipnet_cmd_route_addr);
    cmd->netmask.sa.sa_family = (Ip_sa_family_t) cmd->domain;
#ifdef IPCOM_USE_INET
    if (cmd->domain == IP_AF_INET)
        ipnet_cmd_route_create_mask(&cmd->netmask.sa_in.sin_addr, prefixlen);
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    if (cmd->domain == IP_AF_INET6)
        ipnet_cmd_route_create_mask(&cmd->netmask.sa_in6.sin6_addr, prefixlen);
#endif /* IPCOM_USE_INET6 */

    IP_BIT_SET(cmd->rt_flags, IPNET_RTF_MASK);
    return 0;
}


#ifdef IPCOM_MCMD_ROUTE_SHOW
/*
 *===========================================================================
 *                    ipnet_cmd_route_print_addresses
 *===========================================================================
 * Description: Prints the addresses.
 * Parameters:  addrs - Number of addresses.
 *              first - The first address in the array.
 * Returns:     The string represenstion.
 *
 */
IP_STATIC void
ipnet_cmd_route_print_addresses(Ip_bool no_dns, int addrs, struct Ip_sockaddr *first)
{
    struct Ip_sockaddr *addr_ptr;
    int                 i;
    char                addr_str[IPNET_CMD_ROUTE_NAME_BUF];
    int                 sa_len;

    addr_ptr = first;
    for (i = 0; i < IPNET_RTAX_MAX; i++)
    {
        if (IP_BIT_ISSET(addrs, 1 << i))
        {
            if (ipcom_cmd_sockaddr_to_str(addr_ptr, IP_FALSE, no_dns, addr_str, sizeof(addr_str)) == IP_NULL)
            {
                /* Invalid address */
                IP_PANIC();
                continue;
            }
            ipcom_printf(IPNET_ROUTE_INDENT "%-20s: %s" IP_LF,
                         ipcom_cmd_key_to_str(ipnet_rta_map, i, IP_NULL, 0),
                         addr_str);
            sa_len = addr_ptr->sa_len ? IP_ROUNDUP(addr_ptr->sa_len, sizeof(Ip_u32)) : sizeof(Ip_u32);
            addr_ptr = (struct Ip_sockaddr *)((Ip_u8 *)addr_ptr + sa_len);
        }
    }
}


/*
 *===========================================================================
 *                    ipnet_cmd_route_print_metrics
 *===========================================================================
 * Description: Prints route metrics.
 * Parameters:  rmx - Pointer to the route metrucs.
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_route_print_metrics(struct Ipnet_rt_metrics *rmx)
{
    ipcom_printf("%10s%10s%10s%10s%10s" IP_LF,
        "rtt,msec", "rttvar", "hopcount", "MTU", "expire");
    ipcom_printf("%10ld%10ld%10ld%10ld%10ld" IP_LF, rmx->rmx_rtt,
        rmx->rmx_rttvar, rmx->rmx_hopcount, rmx->rmx_mtu, rmx->rmx_expire);
}


/*
 *===========================================================================
 *                    ipnet_cmd_route_print_if
 *===========================================================================
 * Description: Print generic network interface information.
 * Parameters:  ifindex - The interface to print information for.
 *              metric - Interface metric.
 *              flags - Interface flags.
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_route_print_if(Ip_u32 ifindex, int metric, int flags)
{
    char  ifbuf[IP_IFNAMSIZ];
    char *ifname;
    char  flagstr[40];

    ifname = ipcom_if_indextoname(ifindex, ifbuf),

    ipcom_printf(IPNET_ROUTE_INDENT "%s: ifindex %ld, metric %d, flags <%s>" IP_LF,
                 ifname ? ifname : "",
                 ifindex,
                 metric,
                 ipcom_cmd_if_flags_to_str(flags, flagstr, sizeof(flagstr)));
}

/*
 *===========================================================================
 *                    ipnet_cmd_route_print_msghdr
 *===========================================================================
 * Description: Print generic route socket information.
 * Parameters:  msghdr - The route message header.
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_route_print_msghdr(struct Ipnet_rs_msghdr *msghdr)
{
    ipcom_printf("%s: ver: %d, len: %d" IP_LF,
                 ipcom_cmd_key_to_str(ipnet_rtm_map, msghdr->type, IP_NULL, 0),
                 msghdr->version, msghdr->msglen);
}


/*
 *===========================================================================
 *                    ipnet_cmd_route_print_routeinfo
 *===========================================================================
 * Description: Print route information.
 * Parameters:  no_dns - Set to IP_TRUE if DNS lookup should be skipped.
 *              rtm - The route information.
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_route_print_routeinfo(Ip_bool no_dns, struct Ipnet_rt_msghdr *rtm)
{
    char str[IP_INET6_ADDRSTRLEN];


    if (rtm->rtm_index != 0)
    {
        char *ifname;

        ifname = ipcom_if_indextoname(rtm->rtm_index, str);
        ipcom_printf(IPNET_ROUTE_INDENT "%s, ", ifname ? ifname : "");
    }
    else
        ipcom_printf(IPNET_ROUTE_INDENT "* Unspecified interface *, ");
    ipcom_printf("pid %lu, seq %lu, errno %ld, flags <%s>" IP_LF,
                 rtm->rtm_pid, rtm->rtm_seq, rtm->rtm_errno,
                 ipnet_cmd_rt_flags_to_str(rtm->rtm_flags, str, sizeof(str)));
    ipnet_cmd_route_print_addresses(no_dns, rtm->rtm_addrs, (struct Ip_sockaddr *) (rtm + 1));
}


/*
 *===========================================================================
 *                    ipnet_cmd_route_print_addrinfo
 *===========================================================================
 * Description: Print interface address information.
 * Parameters:  no_dns - Set to IP_TRUE if DNS lookup should be skipped.
 *              ifam - The interface address information.
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_route_print_addrinfo(Ip_bool no_dns, struct Ipnet_ifa_msghdr *ifam)
{
    ipnet_cmd_route_print_if(ifam->ifam_index, ifam->ifam_metric, ifam->ifam_flags);
    ipnet_cmd_route_print_addresses(no_dns, ifam->ifam_addrs, (struct Ip_sockaddr *) (ifam + 1));
}


/*
 *===========================================================================
 *                    ipnet_cmd_route_print_ifinfo
 *===========================================================================
 * Description: Print interface information.
 * Parameters:  no_dns - Set to IP_TRUE if DNS lookup should be skipped.
 *              ifm - The interface information.
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_route_print_ifinfo(Ip_bool no_dns, struct Ipnet_if_msghdr *ifm)
{
    ipnet_cmd_route_print_if(ifm->ifm_index, ifm->ifm_data.ifi_metric, ifm->ifm_flags);
    ipcom_printf(IPNET_ROUTE_INDENT "type: %s" IP_LF,
                 ipcom_cmd_key_to_str(ipnet_if_type_map, ifm->ifm_data.ifi_type, IP_NULL, 0));
    ipcom_printf(IPNET_ROUTE_INDENT "link addr len: %d   link hdr len: %d" IP_LF,
                 ifm->ifm_data.ifi_addrlen, ifm->ifm_data.ifi_hdrlen);
    ipcom_printf(IPNET_ROUTE_INDENT "MTU: %ld   baudrate: %ld" IP_LF,
                 ifm->ifm_data.ifi_mtu, ifm->ifm_data.ifi_baudrate);
    ipcom_printf(IPNET_ROUTE_INDENT "received pkts: %ld   receive errors: %ld" IP_LF,
                 ifm->ifm_data.ifi_ipackets, ifm->ifm_data.ifi_ierrors);
    ipcom_printf(IPNET_ROUTE_INDENT "sent pkts: %ld   send errors: %ld" IP_LF,
                 ifm->ifm_data.ifi_opackets, ifm->ifm_data.ifi_oerrors);
    ipcom_printf(IPNET_ROUTE_INDENT "collisions: %ld" IP_LF, ifm->ifm_data.ifi_collisions);
    ipcom_printf(IPNET_ROUTE_INDENT "received bytes: %ld   sent bytes: %ld" IP_LF,
                 ifm->ifm_data.ifi_ibytes, ifm->ifm_data.ifi_obytes);
    ipcom_printf(IPNET_ROUTE_INDENT "received multicast pkts: %ld   sent multicast pkts: %ld" IP_LF,
                 ifm->ifm_data.ifi_imcasts, ifm->ifm_data.ifi_omcasts);
    ipcom_printf(IPNET_ROUTE_INDENT "dropped pkts: %ld   dropped pkts (no proto): %ld" IP_LF,
                 ifm->ifm_data.ifi_iqdrops, ifm->ifm_data.ifi_noproto);
    ipnet_cmd_route_print_addresses(no_dns, ifm->ifm_addrs, (struct Ip_sockaddr *) (ifm + 1));
}


/*
 *===========================================================================
 *                    ipnet_cmd_route_print_ifannounce
 *===========================================================================
 * Description: Print interface announce information.
 * Parameters:  no_dns - Set to IP_TRUE if DNS lookup should be skipped.
 *              ifan - The interface announce information.
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_route_print_ifannounce(Ip_bool no_dns, struct Ipnet_if_announcemsghdr *ifan)
{
    (void)no_dns;
    ipcom_printf(IPNET_ROUTE_INDENT "%s: ifindex %ld, what: %s" IP_LF,
                 ifan->ifan_name,
                 ifan->ifan_index,
                 ifan->ifan_what == IPNET_IFAN_DEPARTURE ? "departure" : "arrival");
}
#endif /* IPCOM_MCMD_ROUTE_SHOW */


/*
 *===========================================================================
 *                    ipnet_cmd_route_issue_request
 *===========================================================================
 * Description: Issue a IPNET_RTM_GET message.
 * Parameters:  sock - The route socket.
 *              cmd - The command parameters.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_cmd_route_issue_request(Ip_fd sock, Ipnet_cmd_route *cmd,
                              struct Ipnet_rt_msghdr **rtm_reply, Ip_bool silent)
{
    int                     err;
    Ip_pid_t                pid;
    Ip_u32                  seq;
    int                     size;
    Ip_u16                  bufsize;
    struct Ipnet_rt_msghdr *rtm;
    struct Ip_sockaddr     *sa;
    int                     addrs;
    Ip_u8                   type;

    ip_assert(rtm_reply && *rtm_reply == IP_NULL);

    addrs = 0;
    size = sizeof(struct Ipnet_rt_msghdr);
    if (cmd->dst.sa.sa_len == 0)
    {
        IPNET_CMD_ROUTE_CHECK_SILENT(ipcom_printf("No destination address specified." IP_LF));
        return -IP_ERRNO_EINVAL;
    }

    if (cmd->dst.sa.sa_len != 0)
    {
        IP_BIT_SET(addrs, IPNET_RTA_DST);
        size += cmd->dst.sa.sa_len;
    }

    if (cmd->gateway.sa.sa_len != 0)
    {
        IP_BIT_SET(addrs, IPNET_RTA_GATEWAY);
        size += cmd->gateway.sa.sa_len;
    }
    else
    {
        if (IP_BIT_ISSET(cmd->rt_flags, IPNET_RTF_GATEWAY | IPNET_RTF_TUNNELEND)
            && cmd->command == IPNET_CMD_ROUTE_ADD)
        {
            IPNET_CMD_ROUTE_CHECK_SILENT(ipcom_printf("No gateway address specified." IP_LF));
            return -IP_ERRNO_EINVAL;
        }
    }

    if (cmd->netmask.sa.sa_len != 0)
    {
        IP_BIT_SET(addrs, IPNET_RTA_NETMASK);
        size += cmd->netmask.sa.sa_len;
    }

    bufsize = sizeof(struct Ipnet_rt_msghdr) + 8 * sizeof(struct Ip_sockaddr_in6);
    rtm = ipcom_malloc(bufsize);
    if (rtm == IP_NULL)
        return -IP_ERRNO_ENOMEM;
    *rtm_reply = rtm;

    pid = ipcom_getpid();
    seq = (int) ipcom_random();

    ipcom_memset(rtm, 0, sizeof(struct Ipnet_rt_msghdr));
    rtm->rtm_msglen  = (Ip_u16) size;
    rtm->rtm_version = IPNET_RTM_VERSION;
    rtm->rtm_addrs   = addrs;
    rtm->rtm_pid     = pid;
    rtm->rtm_seq     = seq;
    rtm->rtm_table   = cmd->table;
    switch (cmd->command)
    {
    case IPNET_CMD_ROUTE_ADD :
        type = IPNET_RTM_ADD;
        rtm->rtm_flags = cmd->rt_flags;
        break;
    default:
        IP_PANIC2();
        return -IP_ERRNO_EINVAL;
    }

    rtm->rtm_type  = type;
    rtm->rtm_index = cmd->ifindex;

    rtm->rtm_inits            = cmd->initis;
    rtm->rtm_rmx.rmx_rtt      = cmd->rtt;
    rtm->rtm_rmx.rmx_rttvar   = cmd->rttvar;
    rtm->rtm_rmx.rmx_mtu      = cmd->mtu;
    rtm->rtm_rmx.rmx_hopcount = cmd->hopcount;
    rtm->rtm_rmx.rmx_expire   = cmd->expire;

    sa = (struct Ip_sockaddr *) (rtm + 1);
    ipcom_memcpy(sa, &cmd->dst, cmd->dst.sa.sa_len);

    if (cmd->gateway.sa.sa_len)
    {
        sa = (struct Ip_sockaddr *) ((Ip_u8 *) sa + sa->sa_len);
        ipcom_memcpy(sa, &cmd->gateway, cmd->gateway.sa.sa_len);
    }
    if (cmd->netmask.sa.sa_len)
    {
        sa = (struct Ip_sockaddr *) ((Ip_u8 *) sa + sa->sa_len);
        ipcom_memcpy(sa, &cmd->netmask, cmd->netmask.sa.sa_len);
    }

    if (ipcom_setsockopt(sock, IP_SOL_SOCKET, IP_SO_X_VR, &cmd->vr_new, sizeof(cmd->vr_new)) != 0)
        return -ipcom_errno;

    err = (int)ipcom_send(sock, rtm, rtm->rtm_msglen, 0);
    if (err < 0)
        return -ipcom_errno;

    do
    {
        size = ipcom_recv(sock, rtm, bufsize, 0);
        if (size < 0)
            return -ipcom_errno;
        ip_assert(rtm->rtm_msglen == size);
    } while (rtm->rtm_type != type || rtm->rtm_seq != seq || rtm->rtm_pid != pid);

    return - (int) rtm->rtm_errno;
}

/*
 *===========================================================================
 *                    ipnet_cmd_route_parse_arg
 *===========================================================================
 * Description: Parse the commandline arguments.
 * Parameters:  argc - The orginal argc.
 *              argv - The orginal argv.
 *              cmd - Will be updated with information parsed from the commandline.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipnet_cmd_route_parse_arg(int argc, char **argv, Ipnet_cmd_route *cmd)
{
    Ip_bool iface = IP_FALSE;
    Ipcom_cmd_int_str_map cmd_map[] = {
        { IPNET_CMD_ROUTE_ADD,              "add" },
        /* Generic flags */
        { IPNET_CMD_ROUTE_SWITCH_NO_DNS,    "-n" },
        { IPNET_CMD_ROUTE_SWITCH_SILENT,    "-silent" },
        { IPNET_CMD_ROUTE_SWITCH_DEVICE,    "-dev" },
        { IPNET_CMD_ROUTE_SWITCH_NO_LLINFO, "-nollinfo" },
        /* IP domain */
        { IPNET_CMD_ROUTE_SWITCH_INET,      "-inet" },
        { IPNET_CMD_ROUTE_SWITCH_INET6,     "-inet6" },
        /* Mask */
        { IPNET_CMD_ROUTE_SWITCH_NETMASK,   "-netmask" },
        { IPNET_CMD_ROUTE_SWITCH_PREFIXLEN, "-prefixlen" },
        /* IP domain */
        { IPNET_CMD_ROUTE_SWITCH_HOST,      "-host" },
        { IPNET_CMD_ROUTE_SWITCH_NET,       "-net" },
        /* Route flags */
        { IPNET_CMD_ROUTE_SWITCH_CLONING,   "-cloning" },
        { IPNET_CMD_ROUTE_SWITCH_IFACE,     "-iface" },
        { IPNET_CMD_ROUTE_SWITCH_STATIC,    "-static" },
        { IPNET_CMD_ROUTE_SWITCH_NOSTATIC,  "-nostatic" },
        { IPNET_CMD_ROUTE_SWITCH_REJECT,    "-reject" },
        { IPNET_CMD_ROUTE_SWITCH_BLACKHOLE, "-blackhole" },
        { IPNET_CMD_ROUTE_SWITCH_LLINFO,    "-llinfo" },
        { IPNET_CMD_ROUTE_SWITCH_XRESOLVE,  "-xresolve" },
        { IPNET_CMD_ROUTE_SWITCH_PROTO1,    "-proto1" },
        { IPNET_CMD_ROUTE_SWITCH_PROTO2,    "-proto2" },
        { IPNET_CMD_ROUTE_SWITCH_PREF,      "-pref" },
        { IPNET_CMD_ROUTE_SWITCH_TUNNELEND, "-tunnelend" },
        { IPNET_CMD_ROUTE_SWITCH_MBINDING,  "-mbinding" },
        /* Metric flags */
        { IPNET_CMD_ROUTE_SWITCH_RTT,       "-rtt" },
        { IPNET_CMD_ROUTE_SWITCH_RTTVAR,    "-rttvar" },
        { IPNET_CMD_ROUTE_SWITCH_MTU,       "-mtu" },
        { IPNET_CMD_ROUTE_SWITCH_HOPCOUNT,  "-hopcount" },
        { IPNET_CMD_ROUTE_SWITCH_EXPIRE,    "-expire" },
        /* Table index */
        { IPNET_CMD_ROUTE_SWITCH_RT_TABLE,  "-T" },
        /* MPLS shortcut route */
#ifdef IPMPLS
        { IPNET_CMD_ROUTE_SWITCH_MPLS_PW,   "-mpls" },
#endif
        { IPNET_CMD_ROUTE_SWITCH_EOL, IP_NULL },
    };
    int     max_prefixlen;
    int     sock_addr_len;
    int     i;
    int     arg;
    Ip_bool silent = IP_FALSE;

    if (argc == 1)
    {
        return -1;
    }

    ipcom_memset(cmd, 0, sizeof(Ipnet_cmd_route));
    cmd->command  = -1;
    cmd->domain   = IP_AF_INET;
    cmd->expire   = IPCOM_ADDR_INFINITE;
    cmd->rt_flags = IPNET_RTF_UP | IPNET_RTF_DONE | IPNET_RTF_STATIC;
    cmd->table    = IPCOM_ROUTE_TABLE_DEFAULT;

    sock_addr_len = sizeof(struct Ip_sockaddr_in);
    max_prefixlen = 32;

    /* Parse command and options */
    arg = 1;
    for (i = 0; cmd_map[i].str != IP_NULL; i++)
    {
        if (ipcom_strcmp(argv[arg], cmd_map[i].str) == 0)
        {
            if (IPNET_CMD_ROUTE_ADD == cmd_map[i].key)
                cmd->command = cmd_map[i].key;
            else
            {
                switch (cmd_map[i].key)
                {
                case IPNET_CMD_ROUTE_SWITCH_NO_DNS: /* -n */
                    cmd->no_dns = IP_TRUE;
                    break;
                case IPNET_CMD_ROUTE_SWITCH_DEVICE: /* -dev ifname */
                    if (++arg >= argc)
                    {
                        IPNET_CMD_ROUTE_CHECK_SILENT(ipcom_printf("ifname must follow %s" IP_LF, cmd_map[i].str));
                        return -IP_ERRNO_EINVAL;
                    }
                    cmd->ifindex = ipcom_if_nametoindex(argv[arg]);
                    if (cmd->ifindex == 0)
                    {
                        IPNET_CMD_ROUTE_CHECK_SILENT(ipcom_printf("%s is not a valid interface name" IP_LF, argv[arg]));
                        return -IP_ERRNO_EINVAL;
                    }
                    break;
                case IPNET_CMD_ROUTE_SWITCH_NO_LLINFO: /* -nollinfo */
                    cmd->no_llinfo = IP_TRUE;
                    break;
                case IPNET_CMD_ROUTE_SWITCH_INET:   /* -inet */
                    cmd->show_domain = IP_AF_INET;
                    break;
                case IPNET_CMD_ROUTE_SWITCH_INET6:  /* -inet6 */
                    cmd->domain = IP_AF_INET6;
                    cmd->show_domain = IP_AF_INET6;
                    sock_addr_len = sizeof(struct Ip_sockaddr_in6);
                    max_prefixlen = 128;
                    break;
                case IPNET_CMD_ROUTE_SWITCH_NETMASK:    /* -netmask x */
                    if (++arg >= argc)
                    {
                        IPNET_CMD_ROUTE_CHECK_SILENT(
                                                     ipcom_printf("A IPv4 style netmask must follow %s" IP_LF, cmd_map[i].str));
                        return -IP_ERRNO_EINVAL;
                    }
                    i = ipcom_getsockaddrbyaddrname(cmd->domain, cmd->no_dns,
                                                    argv[arg], &cmd->netmask.sa);
                    if (i < 0)
                    {
                        IPNET_CMD_ROUTE_CHECK_SILENT(
                                                     ipcom_printf("%s is an invalid IPv4 netmask format." IP_LF, argv[arg]));
                        return i;
                    }
                    IP_BIT_SET(cmd->rt_flags, IPNET_RTF_MASK);
                    break;
                case IPNET_CMD_ROUTE_SWITCH_PREFIXLEN:    /* -prefixlen x */
                    if (++arg >= argc)
                    {
                        IPNET_CMD_ROUTE_CHECK_SILENT(ipcom_printf("An integer between 0-%d must follow %s" IP_LF,
                                                                  max_prefixlen, cmd_map[i].str));
                        return -IP_ERRNO_EINVAL;
                    }

                    if ((i = ipnet_cmd_route_set_mask_from_prefixlen(cmd, max_prefixlen, argv[arg])) < 0)
                    {
                        IPNET_CMD_ROUTE_CHECK_SILENT(
                                                     ipcom_printf("An integer between 0-%d must follow '-prefixlen'" IP_LF,
                                                                  max_prefixlen));
                        return i;
                    }
                    break;
                case IPNET_CMD_ROUTE_SWITCH_HOST:    /* -host */
                    IP_BIT_SET(cmd->rt_flags, IPNET_RTF_HOST);
                    break;
                case IPNET_CMD_ROUTE_SWITCH_NET:    /* -net */
                    IP_BIT_CLR(cmd->rt_flags, IPNET_RTF_HOST);
                    break;
                case IPNET_CMD_ROUTE_SWITCH_CLONING:   /* -cloning */
                    IP_BIT_SET(cmd->rt_flags, IPNET_RTF_CLONING);
                    break;
                case IPNET_CMD_ROUTE_SWITCH_IFACE:   /* -iface */
                    iface = IP_TRUE;
                    break;
                case IPNET_CMD_ROUTE_SWITCH_STATIC:   /* -static */
                    IP_BIT_SET(cmd->rt_flags, IPNET_RTF_STATIC);
                    break;
                case IPNET_CMD_ROUTE_SWITCH_NOSTATIC:   /* -nostatic */
                    IP_BIT_CLR(cmd->rt_flags, IPNET_RTF_STATIC);
                    break;
                case IPNET_CMD_ROUTE_SWITCH_REJECT:   /* -reject */
                    IP_BIT_SET(cmd->rt_flags, IPNET_RTF_REJECT);
                    break;
                case IPNET_CMD_ROUTE_SWITCH_BLACKHOLE:   /* -blackhole */
                    IP_BIT_SET(cmd->rt_flags, IPNET_RTF_BLACKHOLE);
                    break;
                case IPNET_CMD_ROUTE_SWITCH_LLINFO:   /* -llinfo */
                    IP_BIT_SET(cmd->rt_flags, IPNET_RTF_LLINFO);
                    break;
                case IPNET_CMD_ROUTE_SWITCH_XRESOLVE:   /* -xresolve */
                    IP_BIT_SET(cmd->rt_flags, IPNET_RTF_XRESOLVE);
                    break;
                case IPNET_CMD_ROUTE_SWITCH_PROTO1:   /* -proto1 */
                    IP_BIT_SET(cmd->rt_flags, IPNET_RTF_PROTO1);
                    break;
                case IPNET_CMD_ROUTE_SWITCH_PROTO2:   /* -proto2 */
                    IP_BIT_SET(cmd->rt_flags, IPNET_RTF_PROTO2);
                    break;
                case IPNET_CMD_ROUTE_SWITCH_PREF:     /* -pref */
                    IP_BIT_SET(cmd->rt_flags, IPNET_RTF_PREF);
                    break;
                case IPNET_CMD_ROUTE_SWITCH_TUNNELEND:  /* -tunnelend */
                    IP_BIT_SET(cmd->rt_flags, IPNET_RTF_TUNNELEND);
                    IP_BIT_CLR(cmd->rt_flags, IPNET_RTF_GATEWAY);
                    break;
                case IPNET_CMD_ROUTE_SWITCH_MBINDING: /* -mbinding */
                    IP_BIT_SET(cmd->rt_flags, IPNET_RTF_MBINDING);
                    break;
                case IPNET_CMD_ROUTE_SWITCH_VR_BYNAME:
                    cmd->vr_name = argv[++arg];
                    cmd->add_vr  = IP_TRUE;
                    break;
                case IPNET_CMD_ROUTE_SWITCH_RTT:        /* -rtt x */
                case IPNET_CMD_ROUTE_SWITCH_RTTVAR:     /* -rttvar x */
                case IPNET_CMD_ROUTE_SWITCH_MTU:        /* -mtu x */
                case IPNET_CMD_ROUTE_SWITCH_HOPCOUNT:   /* -hopcount x */
                case IPNET_CMD_ROUTE_SWITCH_EXPIRE:     /* -expire x */
                case IPNET_CMD_ROUTE_SWITCH_RT_TABLE:   /* -T */
                    if (++arg >= argc)
                    {
                        IPNET_CMD_ROUTE_CHECK_SILENT(ipcom_printf("A integer must follow %s." IP_LF,
                                                                  cmd_map[i].str));
                        return -IP_ERRNO_EINVAL;
                    }
                    {
                        int val;
                        val = ipcom_atoi(argv[arg]);
                        if (val < 0)
                        {
                            IPNET_CMD_ROUTE_CHECK_SILENT(
                                                         ipcom_printf("A positive integer must follow %s." IP_LF, cmd_map[i].str));
                            return -IP_ERRNO_EINVAL;
                        }
                        switch (cmd_map[i].key)
                        {
                        case IPNET_CMD_ROUTE_SWITCH_RTT:        /* -rtt x */
                            IP_BIT_SET(cmd->initis, IPNET_RTV_RTT);
                            cmd->rtt = val;
                            break;
                        case IPNET_CMD_ROUTE_SWITCH_RTTVAR:     /* -rttvar x */
                            IP_BIT_SET(cmd->initis, IPNET_RTV_RTTVAR);
                            cmd->rttvar = val;
                            break;
                        case IPNET_CMD_ROUTE_SWITCH_MTU:        /* -mtu x */
                            IP_BIT_SET(cmd->initis, IPNET_RTV_MTU);
                            cmd->mtu = val;
                            break;
                        case IPNET_CMD_ROUTE_SWITCH_HOPCOUNT:   /* -hopcount x */
                            IP_BIT_SET(cmd->initis, IPNET_RTV_HOPCOUNT);
                            cmd->hopcount = val;
                            break;
                        case IPNET_CMD_ROUTE_SWITCH_EXPIRE:     /* -expire x */
                            IP_BIT_SET(cmd->initis, IPNET_RTV_EXPIRE);
                            cmd->expire = val;
                            break;
                        case IPNET_CMD_ROUTE_SWITCH_RT_TABLE:
                            cmd->table = (Ip_u32)val;
                            break;
#ifdef IPMPLS
                        case IPNET_CMD_ROUTE_SWITCH_MPLS_PW:
                            cmd->gateway.sa_mpls.smpls_len    = sizeof(struct Ip_sockaddr_mpls);
                            cmd->gateway.sa_mpls.smpls_family = IP_AF_MPLS;
                            cmd->gateway.sa_mpls.smpls_key    = val;
                            break;
#endif
                        default:
                            break;
                        }
                    }
                    break;
                case IPNET_CMD_ROUTE_SWITCH_SILENT:
                    cmd->silent = IP_TRUE;
                    silent = IP_TRUE;
                    break;
                default:
                    IP_PANIC();
                    return -IP_ERRNO_EINVAL;
                }
            }

            arg++;
            if (arg >= argc)
                break;
            i = -1;
        }
    }

    if (cmd->command == -1)
    {
        if (arg == argc)
            IPNET_CMD_ROUTE_CHECK_SILENT(ipcom_printf("No command specified" IP_LF));
        else
            IPNET_CMD_ROUTE_CHECK_SILENT(ipcom_printf("%s is an unknown command." IP_LF, argv[arg]));
        return -1;
    }

    if (arg < argc)
    {
        if (ipcom_strcmp(argv[arg], "default") == 0)
        {
            IP_BIT_CLR(cmd->rt_flags, IPNET_RTF_HOST);
            IP_BIT_SET(cmd->rt_flags, IPNET_RTF_MASK);
            cmd->dst.sa.sa_family = (Ip_sa_family_t)cmd->domain;
            cmd->dst.sa.sa_len    = (Ip_u8) sock_addr_len;
            cmd->netmask.sa.sa_family = (Ip_sa_family_t)cmd->domain;
            cmd->netmask.sa.sa_len    = (Ip_u8) sock_addr_len;
        }
        else
        {
            char *str_prefixlen;

            str_prefixlen = ipcom_strstr(argv[arg], "/");
            if (str_prefixlen != IP_NULL && ipcom_strlen(str_prefixlen) > 1)
            {
                *str_prefixlen = '\0';
                ipnet_cmd_route_set_mask_from_prefixlen(cmd, max_prefixlen, str_prefixlen + 1);
            }

            /* Read destination */
            i = ipcom_getsockaddrbyaddrname(cmd->domain, cmd->no_dns, argv[arg], &cmd->dst.sa);
            if (i < 0)
            {
                IPNET_CMD_ROUTE_CHECK_SILENT(ipcom_printf("%s is an invalid destination." IP_LF,
                                                          argv[arg]));
                return i;
            }
#ifdef IPCOM_USE_INET6
            if (cmd->domain == IP_AF_INET6 && IPNET_IP6_IS_SCOPE_LINK_LOCAL(&cmd->dst.sa_in6.sin6_addr))
                cmd->netmask.sa_in6.sin6_scope_id = 0xffffffff;
#endif /* IPCOM_USE_INET6 */
        }
        arg++;
    }

    if (cmd->command == IPNET_CMD_ROUTE_ADD
        && IP_BIT_ISFALSE(cmd->rt_flags, IPNET_RTF_HOST)
        && cmd->netmask.sa.sa_len == 0)
    {
        IPNET_CMD_ROUTE_CHECK_SILENT(ipcom_printf("No network mask was specified." IP_LF));
        return -IP_ERRNO_EINVAL;
    }

    if (arg < argc)
    {
        /* Read gateway */
        i = ipcom_getsockaddrbyaddrname(IP_BIT_ISSET(cmd->rt_flags, IPNET_RTF_LLINFO) ? IP_AF_LINK : cmd->domain,
                                        IP_BIT_ISSET(cmd->rt_flags, IPNET_RTF_LLINFO) ? IP_TRUE : cmd->no_dns,
                                        argv[arg],
                                        &cmd->gateway.sa);
        if (i < 0)
        {
            IPNET_CMD_ROUTE_CHECK_SILENT(ipcom_printf("%s is an invalid gateway." IP_LF,
                                                      argv[arg]));
            return i;
        }
        if (!iface && IP_BIT_ISFALSE(cmd->rt_flags, IPNET_RTF_TUNNELEND))
            IP_BIT_SET(cmd->rt_flags, IPNET_RTF_GATEWAY);
    }

    return 0;
}

/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_mcmd_route_add
 *===========================================================================
 * Description: Issue a IPNET_RTM_ADD message.
 * Parameters:  sock - The route socket.
 *              cmd - The command parameters.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipcom_mcmd_route_add(Ip_fd sock, Ipnet_cmd_route *cmd)
{
    char                    str[IPNET_CMD_ROUTE_NAME_BUF];
    int                     err;
    struct Ipnet_rt_msghdr *rtm = IP_NULL;

    cmd->command = IPNET_CMD_ROUTE_ADD;
    err = ipnet_cmd_route_issue_request(sock, cmd, &rtm, cmd->silent);
    if (err < 0)
        goto cleanup;

    if (!cmd->silent)
    {
        if (IP_BIT_ISSET(cmd->rt_flags, IPNET_RTF_HOST))
            ipcom_printf(IPNET_ROUTE_INDENT "add host ");
        else
            ipcom_printf(IPNET_ROUTE_INDENT "add net ");
        ipcom_printf("%s", ipcom_cmd_sockaddr_to_str(&cmd->dst.sa, IP_FALSE, cmd->no_dns, str, sizeof(str)));

        if (cmd->netmask.sa.sa_len != 0)
        {
            ipcom_printf(": netmask %s",
                ipcom_cmd_sockaddr_to_str(&cmd->netmask.sa, IP_FALSE, cmd->no_dns, str, sizeof(str)));
        }

        if (cmd->gateway.sa.sa_len != 0)
        {
            ipcom_printf(": gateway %s",
                ipcom_cmd_sockaddr_to_str(&cmd->gateway.sa, IP_FALSE, cmd->no_dns, str, sizeof(str)));
        }
        ipcom_printf(IP_LF);
    }

cleanup:
    if (rtm != IP_NULL)
        ipcom_free(rtm);
    return err;
}

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_mcmd_route
 *===========================================================================
 * Description: The entry point for the 'route' shell command. Only the "add"
 *              command is supported.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_mcmd_route(int argc, char **argv)
{
    Ipnet_cmd_route cmd;
    int             ret;
    int             val;
    Ip_fd           sock = IP_INVALID_SOCKET;

    ret = ipnet_cmd_route_parse_arg(argc, argv, &cmd);
    if (ret < 0)
        return -ret;

    if (cmd.command == IPNET_CMD_ROUTE_ADD)
    {
        sock = ipcom_socket(IP_AF_ROUTE, IP_SOCK_RAW, 0);
        if (sock == IP_INVALID_SOCKET)
        {
            ipcom_printf("Failed to create routing socket : %s (%d)" IP_LF,
                         ipcom_strerror(ipcom_errno), ipcom_errno);
            ret = ipcom_errno;
            goto leave;
        }

        val = 0xffff;
        ret = ipcom_setsockopt(sock, IP_SOL_SOCKET, IP_SO_RCVBUF, &val, sizeof(val));
        if (ret < 0)
        {
            ipcom_printf("Failed to set the recv buffer size : %s (%d)" IP_LF,
                         ipcom_strerror(ret), ret);
            ret = ipcom_errno;
            goto leave;
        }
    }

    switch (cmd.command)
    {
    case IPNET_CMD_ROUTE_ADD:
        ret = ipcom_mcmd_route_add(sock, &cmd);
        break;
    default:
        IP_PANIC();
        ret = -1;
    }
    if (ret < 0)
    {
        ret = -ret;
        if (!cmd.silent)
            ipcom_printf("route: failed: %s (%d)" IP_LF, ipcom_strerror(ret), ret);
    }

 leave:
    if (sock != IP_INVALID_SOCKET)
        ipcom_socketclose(sock);
    return ret;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

