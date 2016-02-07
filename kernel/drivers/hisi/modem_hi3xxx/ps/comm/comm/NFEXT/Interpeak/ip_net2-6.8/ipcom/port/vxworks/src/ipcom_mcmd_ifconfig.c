/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_mcmd_ifconfig.c,v $ $Revision: 1.3 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_mcmd_ifconfig.c,v $
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
 * This is a reduced version of ipnet_cmd_ifconfig.c. The file is used to
 * make ifconfig command utility binary scalable.
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
#include <ipcom_sysctl.h>

#include "ipnet.h"
#include "ipnet_cmd.h"
#include "ipnet_ctrl.h"
#include "ipnet_h.h"
#include "ipnet_routesock.h"
#ifdef IPCOM_USE_INET6
#include "ipnet_ip6.h"
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_IFCONFIG_CMD_UP               1   /* Bring the interface up */
#define IPNET_IFCONFIG_CMD_ADD_ADDR         2   /* Add an address */

#define IPNET_IFCONFIG_OPT_NETMASK       1
#define IPNET_IFCONFIG_OPT_PREFIXLEN     2
#define IPNET_IFCONFIG_OPT_TENTATIVE     3
#define IPNET_IFCONFIG_OPT_VALID         4
#define IPNET_IFCONFIG_OPT_PREFERRED     5
#define IPNET_IFCONFIG_OPT_TEMPORARY     6
#define IPNET_IFCONFIG_OPT_ANYCAST       7
#define IPNET_IFCONFIG_OPT_HOMEADDRESS   8

#define IPNET_IFCONFIG_CHECK_SILENT(statement)  do { if (!silent) statement; } while((0))


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

struct Ipnet_ifconfig_in_addr
{
    struct Ip_in_addr   addr;
    struct Ip_in_addr   mask;
    Ip_bool             addr_is_set;
    Ip_bool             mask_is_set;
};

struct Ipnet_ifconfig_in6_addr
{
    struct Ip_in6_addr  addr;
    int                 flags;
    int                 prefixlen;
    Ip_time_t           preferred_lifetime;
    Ip_time_t           valid_lifetime;
};

typedef struct Ipnet_cmd_ifconfig_struct
{
    int    cmd;            /* The command to perform */
    Ip_u32 ifindex;        /* The interface to operate on (0 == all, valid for show command) */
    int    domain;         /* The domain to operate in (0 == all, valid for show command) */
    union
    {
        struct Ipnet_ifconfig_in_addr  inet;
        struct Ipnet_ifconfig_in6_addr inet6;
        Ip_u16                         vr;
        Ip_u32                         mtu;
        char                          *lladdr;
        struct Ip_ifreq                create;
	Ip_bool		promisc_mode;
#ifdef IPNET_USE_VLAN
        struct Ip_vlanreq              vlan;
#endif /* IPNET_USE_VLAN */
#ifdef IPNET_USE_TUNNEL
        struct Ip_ip_tunnel_param      tunnel;
#endif /* IPNET_USE_TUNNEL */
    } param;
}
Ipnet_cmd_ifconfig;


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

IP_STATIC Ipcom_cmd_int_str_map ipnet_cmd_map[] =
{
    { IPNET_IFCONFIG_CMD_UP,             "up"      },
    { IPNET_IFCONFIG_CMD_ADD_ADDR,       "add"     },
#ifdef IPNET_USE_VLAN
#endif /* IPNET_USE_VLAN */
#ifdef IPNET_USE_TUNNEL
#endif
    { -1,                                IP_NULL   }
};

IP_STATIC Ipcom_cmd_int_str_map ipnet_switch_map[] =
{
    { -1,                              IP_NULL }
};

IP_STATIC Ipcom_cmd_int_str_map ipnet_af_map[] =
{
#ifdef IPCOM_USE_INET
    { IP_AF_INET,                   "inet"  },
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    { IP_AF_INET6,                  "inet6" },
#endif /* IPCOM_USE_INET6 */
    { -1,                           IP_NULL }
};

IP_STATIC Ipcom_cmd_int_str_map ipnet_option_map[] =
{
    { IPNET_IFCONFIG_OPT_NETMASK,      "netmask"   },
    { IPNET_IFCONFIG_OPT_PREFIXLEN,    "prefixlen" },
    { IPNET_IFCONFIG_OPT_TENTATIVE,    "tentative" },
    { IPNET_IFCONFIG_OPT_VALID,        "valid"     },
    { IPNET_IFCONFIG_OPT_PREFERRED,    "preferred" },
    { IPNET_IFCONFIG_OPT_TEMPORARY,    "temporary" },
    { IPNET_IFCONFIG_OPT_ANYCAST,      "anycast" },
    { IPNET_IFCONFIG_OPT_HOMEADDRESS,  "homeaddress" },
    { -1,                              IP_NULL     }
};


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_mcmd_ifconfig_create_mask
 *===========================================================================
 * Description: Creates a network mask with the 'netbits' leftmost bits set.
 * Parameters:  [in] mask - Pointer to a buffer where the mask should be stored.
 *              [in] netbits - The number of set bits.
 * Returns:
 *
 */
IP_STATIC void
ipcom_mcmd_ifconfig_create_mask(void *mask, int netbits)
{
    int i;

    for (i = 0; i < netbits; i++)
    {
        IP_BIT_SET(((Ip_u16*) mask)[i / 16], ip_htons(0x8000 >> (i % 16)));
    }
}

/*
 *===========================================================================
 *                    ipnet_ifconfig_if_change_state
 *===========================================================================
 * Description: Change the state of a network interface.
 * Parameters:  fd - A socket descriptor.
 *              ifindex - The interface index for the interface to change.
 *              if_up - Set to IP_TRUE if the interface should be brought up.
 * Returns:
 *
 */
IP_STATIC int
ipnet_ifconfig_if_change_state(Ip_fd fd, Ip_u32 ifindex, Ip_bool if_up, Ip_bool silent)
{
    struct Ip_ifreq ifreq;

    ipcom_memset(&ifreq, 0, sizeof(struct Ip_ifreq));
    (void)ipcom_if_indextoname(ifindex, ifreq.ifr_name);
    if (ipcom_socketioctl(fd, IP_SIOCGIFFLAGS, &ifreq) < 0)
    {
        IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("ifconfig: failed to get interface flags: %s" IP_LF, ipcom_strerror(ipcom_errno)));
        return -ipcom_errno;
    }

    if (if_up)
        IP_BIT_SET(ifreq.ip_ifr_flags, IP_IFF_UP);
    else
        IP_BIT_CLR(ifreq.ip_ifr_flags, IP_IFF_UP);
    if (ipcom_socketioctl(fd, IP_SIOCSIFFLAGS, &ifreq) < 0)
    {
        IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("ifconfig: failed to set interface flags: %s"IP_LF, ipcom_strerror(ipcom_errno)));
        return -ipcom_errno;
    }
    return 0;
}

#ifdef IPCOM_USE_INET
/*
 *===========================================================================
 *                    ipnet_ifconfig_if_change_dhcp
 *===========================================================================
 * Description: Change the dhcp state of a network interface.
 * Parameters:  fd - A socket descriptor.
 *              ifindex - The interface index for the interface to change.
 *              dhcp_en - Set to IP_TRUE if the interface should be configured
 *                        by dhcp.
 * Returns:
 *
 */

IP_STATIC int
ipnet_ifconfig_if_change_dhcp(Ip_fd fd, Ip_u32 ifindex, Ip_bool dhcp_en)
{
    struct Ip_ifreq ifreq;

    ipcom_memset(&ifreq, 0, sizeof(struct Ip_ifreq));
    (void)ipcom_if_indextoname(ifindex, ifreq.ifr_name);

    if (dhcp_en)
        ifreq.ifr_ifru.ifru_opt = 1;
    else
        ifreq.ifr_ifru.ifru_opt = 0;
    if (ipcom_socketioctl(fd, IP_SIOCXSDHCPRUNNING, &ifreq) < 0)
    {
        ipcom_printf("ifconfig: failed to enable/disabled DHCP: %s"IP_LF, ipcom_strerror(ipcom_errno));
        return -ipcom_errno;
    }
    return 0;
}

#if 0
/*
 *===========================================================================
 *                    ipnet_ifconfig_ipv4_replace
 *===========================================================================
 * Description: Replaces the primary IPv4 address.
 * Parameters:  ifindex - The interface index for which the primary address
 *                        should be changed.
 *              addr - The new address.
 *              mask - The new mask or IP_NULL if the mask should not be changed.
 *              silent - IP_TRUE if no nothing should be printed in the case
 *                       on an error.
 * Returns:
 *
 */
IP_STATIC int
ipnet_ifconfig_ipv4_replace(Ip_fd fd,
                            Ip_u32 ifindex,
                            struct Ip_in_addr *addr,
                            struct Ip_in_addr *netmask,
                            Ip_bool silent)
{
    struct Ip_ifreq        ifr;
    struct Ip_sockaddr_in *in;

    if (addr == IP_NULL)
    {
        IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("ifconfig: address is missing" IP_LF));
        return -IP_ERRNO_EINVAL;
    }

    ipcom_memset(&ifr, 0, sizeof(struct Ip_ifreq));
    (void)ipcom_if_indextoname(ifindex, ifr.ifr_name);

    in = (struct Ip_sockaddr_in *) &ifr.ip_ifr_addr;
    in->sin_family = IP_AF_INET;
    in->sin_len    = sizeof(struct Ip_sockaddr_in);
    in->sin_addr   = *addr;
    if (ipcom_socketioctl(fd, IP_SIOCSIFADDR, &ifr) < 0)
    {
        IPNET_IFCONFIG_CHECK_SILENT(
            ipcom_printf("ifconfig: ipcom_socketioctl() failed to set primary IPv4 address: %s" IP_LF,
                         ipcom_strerror(ipcom_errno)));
        return -ipcom_errno;
    }

    if (netmask == IP_NULL)
        return 0;

    in->sin_addr   = *netmask;
    if (ipcom_socketioctl(fd, IP_SIOCSIFNETMASK, &ifr) < 0)
    {
        IPNET_IFCONFIG_CHECK_SILENT(
            ipcom_printf("ifconfig: ipcom_socketioctl() failed to set mask for primary IPv4 address: %s" IP_LF,
                         ipcom_strerror(ipcom_errno)));
        return -ipcom_errno;
    }
    return 0;
}
#endif


/*
 *===========================================================================
 *                    ipnet_ifconfig_ipv4_add_delete
 *===========================================================================
 * Description: Adds or deletes a IPv4 address.
 * Parameters:  add - Set to IP_TRUE to add the address.
 *              ifindex - The interface the address should be added to.
 *              addr - The address to add/delete
 *              netmask - The netmask to add.
 * Returns:
 *
 */
IP_STATIC int
ipnet_ifconfig_ipv4_add_delete(Ip_fd fd,
                               Ip_bool add,
                               Ip_u32 ifindex,
                               struct Ip_in_addr *addr,
                               struct Ip_in_addr *netmask,
                               Ip_bool silent)
{
    struct Ip_ifaliasreq   ifal;
    struct Ip_sockaddr_in *in_addr;
    struct Ip_sockaddr_in *in_mask;

    if (addr == IP_NULL)
    {
        IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("ifconfig: address is missing" IP_LF));
        return -IP_ERRNO_EINVAL;
    }

    ipcom_memset(&ifal, 0, sizeof(struct Ip_ifaliasreq));
    (void)ipcom_if_indextoname(ifindex, ifal.ifra_name);

    /* Adding address and mask. */
    in_addr = (struct Ip_sockaddr_in *) &ifal.ifra_addr;
    in_addr->sin_family = IP_AF_INET;
    in_addr->sin_len    = sizeof(struct Ip_sockaddr_in);
    in_addr->sin_addr   = *addr;

    if (netmask != IP_NULL)
    {
        if (!add)
            IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("warning: mask is ignored in delete operations" IP_LF));
        else
        {
            in_mask = (struct Ip_sockaddr_in *) &ifal.ifra_mask;
            in_mask->sin_family = IP_AF_INET;
            in_mask->sin_len    = sizeof(struct Ip_sockaddr_in);
            in_mask->sin_addr   = *netmask;
        }
    }

    if (ipcom_socketioctl(fd, add ? IP_SIOCAIFADDR : IP_SIOCDIFADDR, &ifal) < 0)
    {
        IPNET_IFCONFIG_CHECK_SILENT(
            ipcom_printf("ifconfig: ipcom_socketioctl() failed to %s IPv4 address: %s" IP_LF,
                add ? "add" : "delete",
                ipcom_strerror(ipcom_errno)));
        return -ipcom_errno;
    }

#ifndef IP_PORT_LAS
    if (IP_IN_CLASSD(addr->s_addr) == IP_FALSE)
        /* Unicast address was changed manually. Disable dhcp on the interface */
        return ipnet_ifconfig_if_change_dhcp(fd, ifindex, IP_FALSE);
#endif
    return 0;
}
#endif /* IPCOM_USE_INET */


#ifdef IPCOM_USE_INET6
/*
 *===========================================================================
 *                    ipnet_ifconfig_ipv6_add
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_ifconfig_ipv6_add(Ip_fd fd,
                        Ip_u32 ifindex,
                        struct Ipnet_ifconfig_in6_addr *param,
                        Ip_bool silent)
{
    struct Ip_in6_aliasreq ifareq;

    ipcom_memset(&ifareq, 0, sizeof(ifareq));
    (void)ipcom_if_indextoname(ifindex, ifareq.ifra_name);

    ifareq.ifra_addr.sin6_family = IP_AF_INET6;
    ifareq.ifra_addr.sin6_len    = sizeof(struct Ip_sockaddr_in6);
    ifareq.ifra_addr.sin6_addr   = param->addr;
    if (IP_IN6_IS_ADDR_LINK_LOCAL(&param->addr))
        ifareq.ifra_addr.sin6_scope_id = ifindex;

    ifareq.ifra_prefixmask.sin6_family = IP_AF_INET6;
    ifareq.ifra_prefixmask.sin6_len    = sizeof(struct Ip_sockaddr_in6);
    ipcom_mcmd_ifconfig_create_mask(&ifareq.ifra_prefixmask.sin6_addr, param->prefixlen);

    ifareq.ifra_flags = param->flags;

    if (param->preferred_lifetime != (Ip_time_t)IPCOM_ADDR_INFINITE)
        ifareq.ifra_lifetime.ia6t_preferred = param->preferred_lifetime + ipcom_time(IP_NULL);
    else
        ifareq.ifra_lifetime.ia6t_preferred = (Ip_time_t)IPCOM_ADDR_INFINITE;

    if (ifareq.ifra_lifetime.ia6t_expire != (Ip_time_t)IPCOM_ADDR_INFINITE)
        ifareq.ifra_lifetime.ia6t_expire = param->valid_lifetime + ipcom_time(IP_NULL);
    else
        ifareq.ifra_lifetime.ia6t_expire = (Ip_time_t)IPCOM_ADDR_INFINITE;

    if (ipcom_socketioctl(fd, IP_SIOCAIFADDR_IN6, &ifareq) < 0)
    {
        IPNET_IFCONFIG_CHECK_SILENT(
            ipcom_printf("ifconfig: ipcom_socketioctl() failed to add IPv6 address: %s" IP_LF,
                ipcom_strerror(ipcom_errno)));
        return -ipcom_errno;
    }
    return 0;
}
#endif /* IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                    ipcom_mcmd_ifconfig_parse_arg
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_mcmd_ifconfig_parse_arg(int argc,
                             int *next_arg,
                             char **argv,
                             Ipnet_cmd_ifconfig *cmd,
                             Ip_bool silent)
{
    int   domain;
    char *str_prefixlen;

    domain = ipcom_cmd_str_to_key(ipnet_af_map, argv[*next_arg]);
    if (domain > 0)
    {
        /* Domain specifed */
        cmd->domain = domain;
        ++*next_arg;
    }

    cmd->cmd = ipcom_cmd_str_to_key(ipnet_cmd_map, argv[*next_arg]);
    if (cmd->cmd < 0)
    {
        IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("ifconfig: '%s' unknown command" IP_LF, argv[*next_arg]));
        return -IP_ERRNO_EINVAL;
    }

    ++*next_arg;

    switch (cmd->cmd)
    {
    case IPNET_IFCONFIG_CMD_ADD_ADDR :
        if (cmd->domain <= 0)
        {
            IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("ifconfig: missing <family>" IP_LF));
            return -IP_ERRNO_EINVAL;
        }
        if (*next_arg >= argc)
        {
            IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("ifconfig: missing <address>" IP_LF));
            return -IP_ERRNO_EINVAL;
        }

#ifdef IPCOM_USE_INET
        if (cmd->domain == IP_AF_INET)
        {
            int prefixlen = -1;

            if (ipcom_inet_pton(IP_AF_INET, argv[*next_arg], &cmd->param.inet.addr) != 1)
            {
                IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("ifconfig: %s is not a valid inet <address>" IP_LF,
                                                         argv[*next_arg]));
                return -IP_ERRNO_EINVAL;
            }

            cmd->param.inet.addr_is_set = IP_TRUE;
            cmd->param.inet.mask_is_set = IP_FALSE;

            str_prefixlen = ipcom_strstr(argv[*next_arg], "/");
            if (str_prefixlen != IP_NULL && ipcom_strlen(str_prefixlen) > 1)
                prefixlen = ipcom_atoi(str_prefixlen + 1);

            ++*next_arg;
            while (*next_arg < argc)
            {
                switch (ipcom_cmd_str_to_key(ipnet_option_map, argv[*next_arg]))
                {
                case IPNET_IFCONFIG_OPT_NETMASK:
                    ++*next_arg;
                    if (ipcom_inet_pton(IP_AF_INET, argv[*next_arg], &cmd->param.inet.mask) != 1)
                    {
                        IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("ifconfig: %s is not a valid inet <netmask>" IP_LF,
                                                                 argv[*next_arg]));
                        return -IP_ERRNO_EINVAL;
                    }
                    ++*next_arg;
                    cmd->param.inet.mask_is_set = IP_TRUE;
                    break;
                case IPNET_IFCONFIG_OPT_PREFIXLEN:
                    ++*next_arg;
                    prefixlen = ipcom_atoi(argv[*next_arg]);
                    ++*next_arg;
                    break;
                default:
                    return 0;
                }
            }

            if (prefixlen != -1 && cmd->param.inet.mask_is_set == IP_FALSE)
            {
                if (prefixlen <= 0 || prefixlen > 32)
                {
                    IPNET_IFCONFIG_CHECK_SILENT(
                        ipcom_printf("ifconfig: inet <prefixlen> must be a value 1 - 32" IP_LF));
                    return -IP_ERRNO_EINVAL;
                }
                cmd->param.inet.mask.s_addr = 0;
                ipcom_mcmd_ifconfig_create_mask(&cmd->param.inet.mask, prefixlen);
                cmd->param.inet.mask_is_set = IP_TRUE;
            }
        }
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
        if (cmd->domain == IP_AF_INET6)
        {
            if (ipcom_inet_pton(IP_AF_INET6, argv[*next_arg], &cmd->param.inet6.addr) != 1)
            {
                IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("ifconfig: %s is not a valid inet6 <address>" IP_LF,
                                                         argv[*next_arg]));
                return -IP_ERRNO_EINVAL;
            }
            cmd->param.inet6.flags              = 0;
            cmd->param.inet6.prefixlen          = 64;
            cmd->param.inet6.preferred_lifetime = (Ip_time_t)IPCOM_ADDR_INFINITE;
            cmd->param.inet6.valid_lifetime     = (Ip_time_t)IPCOM_ADDR_INFINITE;

            str_prefixlen = ipcom_strstr(argv[*next_arg], "/");
            if (str_prefixlen != IP_NULL && ipcom_strlen(str_prefixlen) > 1)
                cmd->param.inet6.prefixlen = ipcom_atoi(str_prefixlen + 1);

            ++*next_arg;
            while (*next_arg < argc)
            {
                switch (ipcom_cmd_str_to_key(ipnet_option_map, argv[(*next_arg)++]))
                {
                case IPNET_IFCONFIG_OPT_TENTATIVE:
                    IP_BIT_SET(cmd->param.inet6.flags, IP_IN6_IFF_TENTATIVE);
                    break;
                case IPNET_IFCONFIG_OPT_TEMPORARY:
                    IP_BIT_SET(cmd->param.inet6.flags, IP_IN6_IFF_TEMPORARY);
                    break;
                case IPNET_IFCONFIG_OPT_ANYCAST:
                    IP_BIT_SET(cmd->param.inet6.flags, IP_IN6_IFF_ANYCAST);
                    break;
                case IPNET_IFCONFIG_OPT_HOMEADDRESS:
                    IP_BIT_SET(cmd->param.inet6.flags, IP_IN6_IFF_HOMEADDRESS);
                    break;
                case IPNET_IFCONFIG_OPT_PREFIXLEN:
                    cmd->param.inet6.prefixlen = ipcom_atoi(argv[*next_arg]);
                    ++*next_arg;
                    break;
                case IPNET_IFCONFIG_OPT_VALID:
                    cmd->param.inet6.valid_lifetime = ipcom_atoi(argv[*next_arg]);
                    break;
                case IPNET_IFCONFIG_OPT_PREFERRED:
                    cmd->param.inet6.preferred_lifetime = ipcom_atoi(argv[*next_arg]);
                    break;
                default:
                    --*next_arg;
                    return 0;
                }
            }
            if (cmd->param.inet6.prefixlen <= 0 || cmd->param.inet6.prefixlen > 128)
            {
                IPNET_IFCONFIG_CHECK_SILENT(
                    ipcom_printf("ifconfig: inet6 <prefixlen> must be a value 1 - 128" IP_LF));
                return -IP_ERRNO_EINVAL;
            }
        }
#endif /* IPCOM_USE_INET6 */
        break;

    default:
        break;
    }
    return 0;
}

/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_mcmd_ifconfig
 *===========================================================================
 * Description: ifconfig shell command entry point.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_mcmd_ifconfig(int argc, char **argv)
{
    int                next_arg;
    Ip_fd              fd;
    Ipnet_cmd_ifconfig cmd;
    int                err = 0;
    IP_CONST char     *ifname = IP_NULL;
    int                silent = IP_FALSE;
    next_arg = 1;

    /* Silent must only be used by the configuration daemon */
    if (argc > next_arg && ipcom_strcmp(argv[next_arg], "-silent") == 0)
    {
        silent = IP_TRUE;
        next_arg++;
    }

#ifdef IPCOM_USE_INET
    fd = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, 0);
#else
    fd = ipcom_socket(IP_AF_INET6, IP_SOCK_DGRAM, 0);
#endif
    if (fd == IP_SOCKERR)
    {
        ipcom_printf("ifconfig: ipcom_socket() failed: %s"IP_LF, ipcom_strerror(ipcom_errno));
        goto errout;
    }

    ipcom_memset(&cmd, 0, sizeof(cmd));

    /* Process all switches */
    while (argc > next_arg && cmd.cmd != -1)
    {
        cmd.cmd = ipcom_cmd_str_to_key(ipnet_switch_map, argv[next_arg]);
        switch (cmd.cmd)
        {
        default:
            if (argv[next_arg][0] == '-' && cmd.cmd == -1)
                IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("'%s' is a unknown switch"IP_LF, argv[next_arg]));
            break;
        }
    }

    if (argc == next_arg)
        goto errout;

    /* Process all commands */
    while (next_arg < argc && err == 0)
    {
        ipcom_memset(&cmd.param, 0, sizeof(cmd.param));
        if (ifname == IP_NULL)
            ifname = argv[next_arg++];

        if (ipcom_mcmd_ifconfig_parse_arg(argc, &next_arg, argv, &cmd, silent) < 0)
            goto errout;

        {
            cmd.ifindex = ipcom_if_nametoindex(ifname);
            if (cmd.ifindex == 0)
            {
                IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("ifconfig: interface %s not found"IP_LF, ifname));
                goto errout;
            }
        }

        switch (cmd.cmd)
        {
        case IPNET_IFCONFIG_CMD_UP :
            err = ipnet_ifconfig_if_change_state(fd, cmd.ifindex, IP_TRUE, silent);
            break;
        case IPNET_IFCONFIG_CMD_ADD_ADDR :
#ifdef IPCOM_USE_INET
            if (cmd.domain == IP_AF_INET)
            {
                err = ipnet_ifconfig_ipv4_add_delete(fd,
                                                     IP_TRUE,
                                                     cmd.ifindex,
                                                     cmd.param.inet.addr_is_set ? &cmd.param.inet.addr : IP_NULL,
                                                     cmd.param.inet.mask_is_set ? &cmd.param.inet.mask : IP_NULL,
                                                     silent);
            }
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
            if (cmd.domain == IP_AF_INET6)
                err = ipnet_ifconfig_ipv6_add(fd, cmd.ifindex, &cmd.param.inet6, silent);
#endif /* IPCOM_USE_INET6 */
            break;
#ifdef IPNET_USE_VLAN
#endif /* IPNET_USE_VLAN */
        default:
            IP_PANIC();
            break;
        }
    }

cleanup:
    if (fd != IP_INVALID_SOCKET)
        (void)ipcom_socketclose(fd);

    return err;

errout:
    err = -1;
    goto cleanup;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
