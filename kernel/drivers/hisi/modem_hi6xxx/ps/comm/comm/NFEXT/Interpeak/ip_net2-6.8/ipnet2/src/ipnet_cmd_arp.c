/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_cmd_arp.c,v $ $Revision: 1.48 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_cmd_arp.c,v $
 *     $Author: kenneth $
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

#ifdef IPCOM_USE_INET

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cmd.h>
#include <ipcom_getopt.h>
#include <ipcom_sysctl.h>

#include "ipnet.h"
#include "ipnet_eth.h"
#include "ipnet_cmd.h"
#include "ipnet_routesock.h"


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
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_arp_print_entries
 *===========================================================================
 * Description: Prints an ARP entry.
 * Parameters:  rt - An ARP entry.
 *              addrs - Addresses in the ARP entry.
 * Returns:     0 == success, <0 == error
 *
 */
IP_STATIC int
ipnet_arp_print_entries(Ip_bool silent, struct Ipnet_rt_msghdr *rt, struct Ip_sockaddr *addrs[IPNET_RTAX_MAX])
{
    if (silent == IP_FALSE)
    {
        char str_addr[IPNET_ETH_ADDR_SIZE * 3];
        char ifname[IP_IFNAMSIZ];

        if (ipcom_inet_ntop(IP_AF_INET,
            &((struct Ip_sockaddr_in*) addrs[IPNET_RTAX_DST])->sin_addr,
            str_addr, sizeof(str_addr)) == IP_NULL)
            return -ipcom_errno;

        ipcom_printf("%16s", str_addr);
        ipcom_printf(" at %s ",
            ipcom_cmd_sockaddr_to_str(addrs[IPNET_RTAX_GATEWAY],
                                      IP_FALSE,
                                      IP_TRUE,
                                      str_addr,
                                      sizeof(str_addr)));
        if (rt->rtm_rmx.rmx_expire == IPCOM_ADDR_INFINITE)
            ipcom_printf("permanent ");
        if (IP_BIT_ISSET(rt->rtm_flags, IPNET_RTF_PROTO2))
            ipcom_printf("published ");
        ipcom_printf("on %s"IP_LF,
                     ipcom_if_indextoname(rt->rtm_index, ifname));
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_arp_delete_all_entries
 *===========================================================================
 * Description: Erases all entries matching the ifindex and vr.
 * Parameters:  rt - An ARP entry.
 *              addrs - Addresses in the ARP entry.
 * Returns:     0 == success, <0 == error
 *
 */
IP_STATIC int
ipnet_arp_delete_all_entries(Ip_bool silent, struct Ipnet_rt_msghdr *rt, struct Ip_sockaddr *addrs[IPNET_RTAX_MAX])
{
    struct Ip_arpreq arpreq;
    int              fd;
    int              ret;

    (void) rt;
    (void) silent;

    ipcom_memset(&arpreq, 0, sizeof(arpreq));
    ipcom_memcpy(&arpreq.arp_pa, addrs[IPNET_RTAX_DST], IPCOM_SA_LEN_GET(addrs[IPNET_RTAX_DST]));

    fd = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, 0);
    if (fd < 0)
        goto errout;
    ret = ipcom_socketioctl(fd, IP_SIOCDARP, &arpreq);
    ipcom_socketclose(fd);
    if (ret < 0)
        goto errout;

    return 0;

errout:
    return -ipcom_errno;
}


/*
 *===========================================================================
 *                    ipnet_arp_for_each
 *===========================================================================
 * Description: Lists all ARP entries and call the callback function for each.
 * Parameters:  ifindex - The interface specified at the command prompt.
 *              vr - The route table index specified at the command prompt.
 *              cb - The callback function to call.
 * Returns:     0 == success, <0 == error
 *
 */
IP_STATIC int
ipnet_arp_for_each(Ip_bool silent,
                   Ip_u32 ifindex,
                   int (*cb)(Ip_bool silent, struct Ipnet_rt_msghdr *rt, struct Ip_sockaddr *addrs[IPNET_RTAX_MAX]))
{
    int                     name[6];
    struct Ipnet_rt_msghdr *rt;
    struct Ip_sockaddr     *addrs[IPNET_RTAX_MAX];
    Ip_u8                  *route_buf;
    Ip_size_t               route_buf_len;
    Ip_size_t               offset;
    int                     ret = 0;

    name[0] = IP_CTL_NET;
    name[1] = IP_AF_ROUTE;
    name[2] = 0;
    name[3] = IP_AF_INET;
    name[4] = IP_NET_RT_FLAGS;
    name[5] = IPNET_RTF_LLINFO;

    if (ipcom_sysctl(name, 6, IP_NULL, &route_buf_len, IP_NULL, 0) < 0)
        return -ipcom_errno;

    if (route_buf_len == 0)
        return 0;

    route_buf = ipcom_malloc(route_buf_len);
    if (route_buf == IP_NULL)
        return -IP_ERRNO_ENOMEM;

    if (ipcom_sysctl(name, 6, route_buf, &route_buf_len, IP_NULL, 0) < 0)
    {
        ipcom_free(route_buf);
        return -ipcom_errno;
    }

    for (offset = 0; offset < route_buf_len; offset += rt->rtm_msglen)
    {
        rt = (struct Ipnet_rt_msghdr *) &route_buf[offset];

        ipnet_cmd_init_addrs(rt + 1, rt->rtm_addrs, addrs);

        if ((ifindex == 0 || ifindex == rt->rtm_index)
            && IP_BIT_ISFALSE(rt->rtm_flags, IPNET_RTF_X_MCAST_RO | IPNET_RTF_X_BCAST_RO))
        {
            ret = cb(silent, rt, addrs);
            if (ret < 0)
                break;
        }
    }

    ipcom_free(route_buf);
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_arp_delete_or_probe
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_arp_delete_or_probe(Ip_bool silent,
                          Ip_u32 ifindex,
                          IP_CONST char *str_ip4_addr,
                          Ip_bool is_delete)
{
    struct Ip_arpreq       arpreq;
    struct Ip_sockaddr_in *addr = (struct Ip_sockaddr_in *) &arpreq.arp_pa;
    int                    ret;

    ipcom_memset(&arpreq, 0, sizeof(arpreq));
    IPCOM_SA_LEN_SET(&arpreq.arp_ha, sizeof(arpreq.arp_ha));
    arpreq.arp_ha.sdl_family = IP_AF_LINK;
    arpreq.arp_ha.sdl_type   = IP_IFT_ETHER;
    arpreq.arp_ha.sdl_index  = (Ip_u16) ifindex;

    IPCOM_SA_LEN_SET(&arpreq.arp_pa, sizeof(struct Ip_sockaddr));
    arpreq.arp_pa.sa_family = IP_AF_INET;

    ret = ipcom_inet_pton(IP_AF_INET, str_ip4_addr, &addr->sin_addr);
    if (ret == -1)
        goto errout;
    if (ret == 0)
    {
        if (silent == IP_FALSE)
            ipcom_printf("arp: %s is not a valid IPv4 address."IP_LF, str_ip4_addr);
    }
    else
    {
        int s;

        s = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, 0);
        if (s < 0)
            goto errout;
        ret = ipcom_socketioctl(s, is_delete ? IP_SIOCDARP : IP_SIOCPARP, &arpreq);
        ipcom_socketclose(s);
        if (ret < 0)
            goto errout;
        if (is_delete && silent == IP_FALSE)
            ipcom_printf("arp: %s deleted."IP_LF, str_ip4_addr);
    }
    return 0;

errout:
    return -ipcom_errno;
}


/*
 *===========================================================================
 *                    ipnet_arp_delete_entry
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_arp_delete_entry(Ip_bool silent, Ip_u32 ifindex, IP_CONST char *str_ip4_addr)
{
    return ipnet_arp_delete_or_probe(silent, ifindex, str_ip4_addr, IP_TRUE);
}


/*
 *===========================================================================
 *                    ipnet_arp_force_probe
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_arp_force_probe(Ip_bool silent, Ip_u32 ifindex, IP_CONST char *str_ip4_addr)
{
    return ipnet_arp_delete_or_probe(silent, ifindex, str_ip4_addr, IP_FALSE);
}


/*
 *===========================================================================
 *                    ipnet_arp_add_entry
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_arp_add_entry(Ip_bool silent, IP_CONST char *str_ip4_addr, IP_CONST char *str_link_addr, Ip_u32 ifindex, int arp_flags)
{
    int              ret;
    int              s;
    struct Ip_arpreq arpreq;

    ipcom_memset(&arpreq, 0, sizeof(struct Ip_arpreq));
    arpreq.arp_flags = arp_flags;

    ret = ipcom_getsockaddrbyaddr(IP_AF_LINK, str_link_addr,
        (struct Ip_sockaddr *) &arpreq.arp_ha);
    if (ret < 0 || arpreq.arp_ha.sdl_alen != IPNET_ETH_ADDR_SIZE)
    {
        if (silent == IP_FALSE)
            ipcom_printf("arp: '%s' is not a valid link address."IP_LF, str_link_addr);
        goto errout;
    }
    arpreq.arp_ha.sdl_type  = IP_IFT_ETHER;
    arpreq.arp_ha.sdl_index = (Ip_u16) ifindex;

    ret = ipcom_getsockaddrbyaddr(IP_AF_INET, str_ip4_addr, &arpreq.arp_pa);
    if (ret < 0)
    {
        if (silent == IP_FALSE)
            ipcom_printf("arp: '%s' is not a valid IPv4 address."IP_LF, str_ip4_addr);
        goto errout;
    }

    s = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, 0);
    ret = ipcom_socketioctl(s, IP_SIOCSARP, &arpreq);
    ipcom_socketclose(s);
    if (ret < 0)
        goto errout;

    return 0;

errout:
    return -ipcom_errno;
}


/*
 *===========================================================================
 *                    ipnet_arp_print_usage
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_arp_print_usage(void)
{
    ipcom_printf(IP_LF
                 "NAME"IP_LF
                 "     arp - address resolution display and control"IP_LF
                 IP_LF);
    ipcom_printf("SYNOPSIS"IP_LF
                 "     arp [-silent] [-V <routetab>] [-i <ifname>] -a"IP_LF
                 "     arp [-silent] [-V <routetab>] [-i <ifname>] -A"IP_LF
                 "     arp [-silent] [-V <routetab>] [-i <ifname>] -d <hostaddress>"IP_LF
                 "     arp [-silent] [-V <routetab>] [-i <ifname>] [-p] [-t] -s <hostaddress> <ether_addr>"IP_LF
                 "     arp [-silent] [-V <routetab>] [-i <ifname>] -r <hostaddress>"IP_LF
                 IP_LF);
    ipcom_printf("DESCRIPTION"IP_LF
                 "     The arp program displays and modifies the Internet-to-Ethernet address"IP_LF
                 "     translation tables used by the address resolution protocol."IP_LF
                 IP_LF
                 "     Options:"IP_LF
                 "     -silent       Suppress output. MUST be the first argument."IP_LF
                 "     -i <ifname>   Specify interface. First one if not specified."IP_LF
                 "     -t            Temporary ARP entry."IP_LF
                 "     -p            Public ARP entry (i.e. proxy ARP entry)."IP_LF
                 "     -r            Send an ARP request to the host address."IP_LF
                 "     -V <routetab> Specify route table. 0 if not specified."IP_LF
                 IP_LF);
    ipcom_printf("     Commands:"IP_LF
                 "     -a      Display the currently existing ARP entries."IP_LF
                 "     -A      Delete all the ARP entries."IP_LF
                 "     -d      Delete specified ARP entry."IP_LF
                 "     -s      Register a ARP entry for a node."IP_LF);
}


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_cmd_arp
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipnet_cmd_arp(int argc, char **argv)
{
    int          c, ret = 17;
    Ip_u32       ifindex = 0;
#if IPCOM_VR_MAX > 1
    int          vr = ipcom_proc_vr_get();
    int          vr_new = vr;
#endif
    Ipcom_getopt opt;
    int          arp_flags = IP_ATF_PERM;
    Ip_bool      silent    = IP_FALSE;

    if (argc > 1)
    {
        if (ipcom_strcmp(argv[1], "-silent") == 0)
        {
            silent = IP_TRUE;
            argc--;
            argv++;
        }
    }

    /* Parse options */
    ipcom_getopt_clear_r(&opt);
    while ((c = ipcom_getopt_r(argc, argv, "aAd:si:V:tpgr:", &opt)) != -1)
    {
        switch (c)
        {
#if IPCOM_VR_MAX > 1
        case 'V' :
            vr_new = ipcom_atoi(opt.optarg);
            break;
#endif
        case 'i' :
            ifindex = ipcom_if_nametoindex(opt.optarg);
            if(ifindex == 0)
            {
                if (silent == IP_FALSE)
                    ipcom_printf("arp: %s is not a valid interface."IP_LF, opt.optarg);
                goto done;
            }
            break;
        case 't':
            IP_BIT_CLR(arp_flags, IP_ATF_PERM);
            break;
        case 'p':
            IP_BIT_SET(arp_flags, IP_ATF_PUBL);
            break;
        case 'g':
            IP_BIT_SET(arp_flags, IP_ATF_GRAT);
            break;
        default:
            break;
        }
    }

#if IPCOM_VR_MAX > 1
    if (vr != vr_new)
        ipcom_proc_vr_set(vr_new);
#endif

    /* Parse commands */
    ipcom_getopt_clear_r(&opt);
    while ((c = ipcom_getopt_r(argc, argv, "aAd:si:V:tpgr:", &opt)) != -1)
    {
        switch (c)
        {
        case 'a':
            ret = ipnet_arp_for_each(silent, ifindex, ipnet_arp_print_entries);
            goto done;
        case 'A':
            ret = ipnet_arp_for_each(silent, ifindex, ipnet_arp_delete_all_entries);
            goto done;
        case 'd':
            ret = ipnet_arp_delete_entry(silent, ifindex, opt.optarg);
            goto done;
        case 'r':
            ret = ipnet_arp_force_probe(silent, ifindex, opt.optarg);
            goto done;
        case 's':
            if (argc < opt.optind + 2)
                goto done;
            ret = ipnet_arp_add_entry(silent, argv[opt.optind], argv[opt.optind+1], ifindex, arp_flags);
            goto done;
        default:
            break;
        }
    }

done:
    if (ret == 17 && silent == IP_FALSE)
        ipnet_arp_print_usage();

    if (ret < 0 && silent == IP_FALSE)
        ipcom_printf("arp: operation failed, errno: %s" IP_LF, ipcom_strerror(-ret));

#if IPCOM_VR_MAX > 1
    if (vr != vr_new)
        ipcom_proc_vr_set(vr);
#endif

    return ret;
}

#endif /* IPCOM_USE_INET */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

