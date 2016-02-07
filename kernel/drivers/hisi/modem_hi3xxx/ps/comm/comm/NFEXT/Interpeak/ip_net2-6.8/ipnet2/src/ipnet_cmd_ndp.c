/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_cmd_ndp.c,v $ $Revision: 1.45.22.2 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_cmd_ndp.c,v $
 *     $Author: ulf $
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

#ifdef IPCOM_USE_INET6

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cmd.h>
#include <ipcom_getopt.h>

#include "ipnet.h"
#include "ipnet_ctrl.h"
#include "ipnet_h.h"
#include "ipnet_cmd.h"


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
 *                    ipnet_ndp_expire_time_str
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC IP_CONST char*
ipnet_ndp_expire_time_str(Ip_time_t expires, char *str_expire)
{
    if (expires == (Ip_time_t)IPCOM_ADDR_INFINITE)
        ipcom_sprintf(str_expire, "permanent");
    else
        ipcom_sprintf(str_expire, "%ld s", expires);
    return str_expire;
}


/*
 *===========================================================================
 *                    ipnet_ndp_state_str
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char
ipnet_ndp_state_str(Ipnet_nd_state_t status)
{
    switch (status)
    {
    case IPNET_ND_INCOMPLETE:
        return 'I';
    case IPNET_ND_REACHABLE:
        return 'R';
    case IPNET_ND_STALE:
        return 'S';
    case IPNET_ND_DELAY:
        return 'D';
    case IPNET_ND_PROBE:
        return 'P';
    default:
        break;
    }
    return ' ';
}


/*
 *===========================================================================
 *                    ipnet_ndp_print_neighbor_cache_entries
 *===========================================================================
 * Description: Prints all neighbor cache entries.
 * Parameters:  vr_index - The virtual router to use.
 * Returns:     0 = success, -1 = fail.
 *
 */
IP_STATIC int
ipnet_ndp_print_neighbor_cache_entries(Ip_bool silent, Ip_u16 vr_index)
{
    if (silent == IP_FALSE)
    {
        struct Ipnet_tag_nc_entry tag;
        char                      str[256];
        int                       ret;

        ipcom_printf("%-35s %-18s %-9s %-7s %-4s\n", "Neighbor", "Linklayer Address", "Netif", "Expire", "St");
        tag.hdr.type = IPNET_CTRL_NEW_ENUM_NC_ENTRY;
        tag.hdr.size = sizeof(tag);
        tag.vr_index = vr_index;
        for (;;)
        {
            ret = ipnet_ctrl(&tag.hdr);
            if (ret == IPNET_CTRL_ENUM_DONE)
                break;
            if (ret < 0)
            {
                ipcom_printf("ipnet_ctrl() failed to get ND cache entry: %s" IP_LF, ipcom_strerror(ipcom_errno));
                return -1;
            }
            tag.hdr.type = IPNET_CTRL_CONT_ENUM_NC_ENTRY;

            ipcom_printf("%-35s ", ipcom_cmd_sockaddr_to_str(
                (struct Ip_sockaddr *) &tag.addr, IP_FALSE, IP_TRUE, str, sizeof(str)));
            ipcom_printf("%-18s ", ipcom_cmd_sockaddr_to_str(
                (struct Ip_sockaddr *) &tag.link_addr, IP_FALSE, IP_TRUE, str, sizeof(str)));
            ipcom_printf("%-9s ", ipcom_if_indextoname(tag.ifindex, str));
            ipcom_printf("%-7s ", ipnet_ndp_expire_time_str(tag.expires_sec, str));
            ipcom_printf("%c", ipnet_ndp_state_str(tag.state));
            ipcom_printf("\n");
        }
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ndp_erase_all_neighbor_cache_entries
 *===========================================================================
 * Description: Deletes all neighbor cache entries.
 * Parameters:  vr_index - The route table to use.
 * Returns:     0 = success, -1 = fail.
 *
 */
IP_STATIC int
ipnet_ndp_erase_all_neighbor_cache_entries(Ip_bool silent, Ip_u16 vr_index)
{
    struct Ipnet_tag_nc_entry tag;
    int ret;

    for (;;)
    {
        tag.hdr.type = IPNET_CTRL_NEW_ENUM_NC_ENTRY;
        tag.hdr.size = sizeof(tag);
        tag.vr_index = vr_index;
        ret = ipnet_ctrl(&tag.hdr);
        if (ret == IPNET_CTRL_ENUM_DONE)
            break;
        if (ret < 0)
        {
            if (silent == IP_FALSE)
                ipcom_printf("ipnet_ctrl() failed to enumerate ND cache entry: %s"IP_LF,
                             ipcom_strerror(ipcom_errno));
            return 0;
        }

        tag.hdr.type = IPNET_CTRL_DEL_NC_ENTRY;
        if (ipnet_ctrl(&tag.hdr) < 0)
        {
            if (silent == IP_FALSE)
                ipcom_printf("ipnet_ctrl() failed to delete ND cache entry: %s"IP_LF,
                             ipcom_strerror(ipcom_errno));
            return 0;
        }
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ndp_remove_neighbor_cache_entry
 *===========================================================================
 * Description: Removes the specified ND entry from the ND cache.
 * Parameters:  vr_index - The virtual router to use.
 *              str_ip6_addr - A string representation of the IPv6 address
 *              of the entry to remove.
 * Returns:
 *
 */
IP_STATIC int
ipnet_ndp_remove_neighbor_cache_entry(Ip_bool silent,
                                      Ip_u16 vr_index,
                                      IP_CONST char *str_ip6_addr,
                                      Ip_u32 ifindex)
{
    struct Ipnet_tag_nc_entry tag;

    if (ipcom_getsockaddrbyaddrname(IP_AF_INET6, IP_TRUE, str_ip6_addr,
        (struct Ip_sockaddr *) &tag.addr) < 0)
    {
        if (silent == IP_FALSE)
            ipcom_printf("%s is not a valid IPv6 address\n", str_ip6_addr);
        return -1;
    }

    tag.hdr.type = IPNET_CTRL_DEL_NC_ENTRY;
    tag.hdr.size = sizeof(tag);
    tag.vr_index = vr_index;
    tag.ifindex  = ifindex;
    if (ipnet_ctrl(&tag.hdr) < 0)
    {
        if (silent == IP_FALSE)
            ipcom_printf("Failed to delete %s: %s", str_ip6_addr, ipcom_strerror(ipcom_errno));
        return -1;
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ndp_print_default_router_entries
 *===========================================================================
 * Description: Prints all default routers.
 * Parameters:  vr_index - The route table to use.
 * Returns:     0 = success, -1 = fail.
 *
 */
IP_STATIC int
ipnet_ndp_print_prefix_entries(Ip_bool silent, Ip_u16 vr_index)
{
    if (silent == IP_FALSE)
    {
        struct Ipnet_tag_prefix_entry tag;
        char str[44];
        int ret;

        ipcom_printf("%-40s%-10s%-8s\n", "Prefix", "Expire", "Netif");
        tag.hdr.type = IPNET_CTRL_NEW_ENUM_PREFIX_ENTRY;
        tag.hdr.size = sizeof(tag);
        tag.ifindex  = 0;
        tag.vr_index = vr_index;
        for (;;)
        {
            ret = ipnet_ctrl(&tag.hdr);
            if (ret == IPNET_CTRL_ENUM_DONE)
                break;
            if (ret < 0)
            {
                ipcom_printf("ipnet_ctrl() failed to enumerate prefix entry: %s"IP_LF,
                    ipcom_strerror(ipcom_errno));
                return -1;
            }
            tag.hdr.type = IPNET_CTRL_CONT_ENUM_PREFIX_ENTRY;

            (void)ipcom_cmd_sockaddr_to_str((struct Ip_sockaddr *) &tag.prefix,
                                            IP_FALSE, IP_FALSE, str, sizeof(str));
            ipcom_sprintf(&str[ipcom_strlen(str)], "/%d", tag.bits);
            ipcom_printf("%-40s", str);
            ipcom_printf("%-10s", ipnet_ndp_expire_time_str(tag.expires_sec, str));
            ipcom_printf("%-8s", ipcom_if_indextoname(tag.ifindex, str));
            ipcom_printf("\n");
        }
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ndp_print_default_router_entries
 *===========================================================================
 * Description: Prints all routers that has announced themself.
 * Parameters:  vr_index - The virtual router to use.
 * Returns:     0 = success, -1 = fail.
 *
 */
IP_STATIC int
ipnet_ndp_print_default_router_entries(Ip_bool silent, Ip_u16 vr_index)
{
    if (silent == IP_FALSE)
    {
        struct Ipnet_tag_router_entry tag;
        char                          str[IP_INET6_ADDRSTRLEN];
        int                           ret;

        ipcom_printf("%-36s%-10s%-8s\n", "Router address", "Expire", "Netif");
        tag.hdr.type = IPNET_CTRL_NEW_ENUM_ROUTER_ENTRY;
        tag.hdr.size = sizeof(tag);
        tag.vr_index = vr_index;
        for (;;)
        {
            ret = ipnet_ctrl(&tag.hdr);
            if (ret == IPNET_CTRL_ENUM_DONE)
                break;
            if (ret < 0)
            {
                ipcom_printf("ipnet_ctrl() failed to get router entry: %s"IP_LF,
                    ipcom_strerror(ipcom_errno));
                return -1;
            }
            tag.hdr.type = IPNET_CTRL_CONT_ENUM_ROUTER_ENTRY;

            ipcom_printf("%-36s", ipcom_inet_ntop(IP_AF_INET6, &tag.addr, str, sizeof(str)));
            ipcom_printf("%-10s", ipnet_ndp_expire_time_str(tag.expires_sec, str));
            ipcom_printf("%-8s", ipcom_if_indextoname(tag.ifindex, str));
            ipcom_printf("\n");
        }
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ndp_remove_all_default_router_entries
 *===========================================================================
 * Description: Remove all default router entries.
 * Parameters:  vr_index - The virtual router.
 * Returns:     0 = success, -1 = fail.
 *
 */
IP_STATIC int
ipnet_ndp_remove_all_default_router_entries(Ip_bool silent, Ip_u16 vr_index)
{
    struct Ipnet_tag_router_entry tag;
    int                           ret;

    tag.hdr.size = sizeof(tag);
    tag.vr_index = vr_index;
    for (;;)
    {
        tag.hdr.type = IPNET_CTRL_NEW_ENUM_ROUTER_ENTRY;
        ret = ipnet_ctrl(&tag.hdr);
        if (ret == IPNET_CTRL_ENUM_DONE)
            break;
        if (ret < 0)
        {
            if (silent == IP_FALSE)
                ipcom_printf("ipnet_ctrl() failed to get router entry: %s"IP_LF,
                             ipcom_strerror(ipcom_errno));
            return -1;
        }

        tag.hdr.type = IPNET_CTRL_DEL_ROUTER_ENTRY;
        if (ipnet_ctrl(&tag.hdr) < 0)
        {
            if (silent == IP_FALSE)
                ipcom_printf("ipnet_ctrl() failed to delete router entry: %s"IP_LF,
                             ipcom_strerror(ipcom_errno));
            return -1;
        }
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ndp_add_entry
 *===========================================================================
 * Description: Adds a neighbor cache entry.
 * Parameters:  ifname - The name of the interface to a NC entry.
 *              str_ip6_addr - The address of the entry.
 *              str_link_addr - The link layer address.
 * Returns:     0 = success, -1 = fail.
 *
 */
IP_STATIC int
ipnet_ndp_add_entry(Ip_bool silent, Ip_u16 vr_index, Ip_u32 ifindex,
                    IP_CONST char *str_ip6_addr, IP_CONST char *str_link_addr)
{
    struct Ipnet_tag_nc_entry tag;
    char ifname[IP_IFNAMSIZ];

    if (ifindex)
        ipcom_if_indextoname(ifindex, ifname);
    else
        strncpy(ifname, "*unspecified*", IP_IFNAMSIZ);

    if (ipcom_getsockaddrbyaddrname(IP_AF_INET6, IP_TRUE, str_ip6_addr,
        (struct Ip_sockaddr *) &tag.addr) < 0)
    {
        if (silent == IP_FALSE)
            ipcom_printf("%s is not a valid IPv6 address\n", str_ip6_addr);
        goto errout;
    }

    tag.hdr.type    = IPNET_CTRL_ADD_NC_ENTRY;
    tag.hdr.size    = sizeof(tag);
    tag.ifindex     = ifindex;
    tag.is_router   = IP_FALSE;
    tag.expires_sec = IPCOM_ADDR_INFINITE;
    tag.vr_index    = vr_index;
    if (ipcom_getsockaddrbyaddr(IP_AF_LINK, str_link_addr, (struct Ip_sockaddr *) &tag.link_addr) < 0)
    {
        if (silent == IP_FALSE)
            ipcom_printf("%s is not a valid link address for %s\n", str_link_addr, ipcom_if_indextoname(ifindex, ifname)? ifname : "<unknown>");
        return -1;
    }
    if (ipnet_ctrl(&tag.hdr) < 0)
    {
        if (silent == IP_FALSE)
            ipcom_printf("Failed to add NC entry to %s\n", ipcom_if_indextoname(ifindex, ifname)? ifname : "<unknown>");
        goto errout;
    }

    return 0;

errout:
    return -1;
}


/*
 *===========================================================================
 *                    ipnet_ndp_print_usage
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_ndp_print_usage(void)
{
    ipcom_printf(IP_LF);
    ipcom_printf("NAME" IP_LF);
    ipcom_printf("     ndp - control/diagnose IPv6 neighbor discovery protocol" IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("SYNOPSIS" IP_LF);
    ipcom_printf("     ndp [-silent] [-V <routetab>] -a" IP_LF);
    ipcom_printf("     ndp [-silent] [-V <routetab>] -A" IP_LF);
    ipcom_printf("     ndp [-silent] [-V <routetab>] -c" IP_LF);
    ipcom_printf("     ndp [-silent] [-V <routetab>] [-i <ifname>] -d hostaddress" IP_LF);
    ipcom_printf("     ndp [-silent] [-V <routetab>] -p" IP_LF);
    ipcom_printf("     ndp [-silent] [-V <routetab>] -r" IP_LF);
    ipcom_printf("     ndp [-silent] [-V <routetab>] -R" IP_LF);
    ipcom_printf("     ndp [-silent] [[-V <routetab>] | [-i <ifname>]] -s nodeaddress ether_addr" IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("DESCRIPTION" IP_LF);
    ipcom_printf("     The ndp command manipulates the address mapping table used by Neighbor" IP_LF);
    ipcom_printf("     Discovery Protocol (NDP). It can take up to 1 minute before deleted" IP_LF);
    ipcom_printf("     entries is removed, but they should be shown as 'expired' immediately." IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     Options:"IP_LF);
    ipcom_printf("     -silent       Suppress output. MUST be first argument"IP_LF);
    ipcom_printf("     -i <ifname>   Specify interface. Selected by the stack otherwise."IP_LF);
    ipcom_printf("     -V <routetab> Specify route table. <routetab> = 0 if not specified."IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     Commands:" IP_LF);
    ipcom_printf("     -a      Dump the currently existing NDP entries." IP_LF);
    ipcom_printf("     -A      Erase all the NDP entries." IP_LF);
    ipcom_printf("     -d      Delete specified NDP entry." IP_LF);
    ipcom_printf("     -p      Show prefix list." IP_LF);
    ipcom_printf("     -r      Show default router list." IP_LF);
    ipcom_printf("     -R      Flush all the entries in the default router list." IP_LF);
    ipcom_printf("     -s      Register a NDP entry for a node. The entry will be permanent." IP_LF);
    ipcom_printf("             Specify -V or -i, not both." IP_LF);
    ipcom_printf("" IP_LF);
}


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_cmd_ndp
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipnet_cmd_ndp(int argc, char **argv)
{
#if IPCOM_VR_MAX > 1
    Ip_u16 vr_index = (Ip_u16) ipcom_proc_vr_get();
#else
    Ip_u16 vr_index = 0;
#endif
    Ip_u32 ifindex = 0;
    Ip_bool silent = IP_FALSE;
    int c;
    int ret = 17;
    Ipcom_getopt  opt;

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
    while ((c = ipcom_getopt_r(argc, argv, "AHPRad:hprsV:i:", &opt)) != -1)
    {
        switch (c)
        {
        case 'V' :
            vr_index = (Ip_u16) ipcom_atoi(opt.optarg);
            break;
        case 'i' :
            ifindex = ipcom_if_nametoindex(opt.optarg);
            if(ifindex == 0)
            {
                if (silent == IP_FALSE)
                    ipcom_printf("%s is not a valid interface."IP_LF, opt.optarg);
                goto done;
            }
            break;
        default:
            break;
        }
    }

    ipcom_getopt_clear_r(&opt);
    while ((c = ipcom_getopt_r(argc, argv, "AHRad:hprsV:i:", &opt)) != -1)
    {
        switch (c)
        {
        case 'a':
            ret = ipnet_ndp_print_neighbor_cache_entries(silent, vr_index);
            break;
        case 'A':
            ret = ipnet_ndp_erase_all_neighbor_cache_entries(silent, vr_index);
            break;
        case 'd':
            ret = ipnet_ndp_remove_neighbor_cache_entry(silent, vr_index, opt.optarg, ifindex);
            break;
        case 'p':
            ret = ipnet_ndp_print_prefix_entries(silent, vr_index);
            break;
        case 'R':
            ret = ipnet_ndp_remove_all_default_router_entries(silent, vr_index);
            break;
        case 'r':
            ret = ipnet_ndp_print_default_router_entries(silent, vr_index);
            break;
        case 's':
            if (argc < opt.optind + 2)
                goto done;
            ret = ipnet_ndp_add_entry(silent, vr_index, ifindex, argv[opt.optind], argv[opt.optind+1]);
            break;
        default:
            break;
        }
    }
done:
    if (ret == 17 && silent == IP_FALSE)
        ipnet_ndp_print_usage();

    if (silent == IP_FALSE)
        ipcom_printf(IP_LF);
    return ret;
}

#endif /* IPCOM_USE_INET6 */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

