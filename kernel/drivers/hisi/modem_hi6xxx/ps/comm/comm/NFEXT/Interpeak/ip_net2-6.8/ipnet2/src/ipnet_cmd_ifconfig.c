/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_cmd_ifconfig.c,v $ $Revision: 1.141.28.1 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_cmd_ifconfig.c,v $
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

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cmd.h>
#include <ipcom_getopt.h>
#include <ipcom_sysctl.h>
#include <ipcom_time.h>

#include "ipnet.h"
#include "ipnet_cmd.h"
#include "ipnet_ctrl.h"
#include "ipnet_h.h"
#include "ipnet_routesock.h"
#ifdef IPCOM_USE_INET6
#include "ipnet_ip6.h"
#endif

#ifdef IPCOM_USE_FORWARDER
#include "ipcom_forwarder.h"
#endif

#ifdef IPPPP_USE_PPPMP
#include "ipppp.h"
#endif /* IPPPP_USE_PPPMP */

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_IFCONFIG_CMD_SHOW_INFO        0   /* Show infornation about interface(s) */
#define IPNET_IFCONFIG_CMD_UP               1   /* Bring the interface up */
#define IPNET_IFCONFIG_CMD_DOWN             2   /* Bring the interface up */
#define IPNET_IFCONFIG_CMD_REPLACE_ADDR     3   /* Replace the primary address (*/
#define IPNET_IFCONFIG_CMD_ADD_ADDR         4   /* Add an address */
#define IPNET_IFCONFIG_CMD_DELETE_ADDR      5   /* Delete an address */
#define IPNET_IFCONFIG_CMD_SET_VR           6   /* Set the VR for an interface */
#define IPNET_IFCONFIG_CMD_SET_MTU          8   /* Set the MTU */
#define IPNET_IFCONFIG_CMD_DETACH           9   /* Detaches a interface from IPNET */
#define IPNET_IFCONFIG_CMD_HELP             10  /* Show the help page */
#define IPNET_IFCONFIG_CMD_ENABLE_DHCP      11  /* Enable dhcp auto config */
#define IPNET_IFCONFIG_CMD_DISABLE_DHCP     12  /* Disable dhcp auto config */
#define IPNET_IFCONFIG_CMD_SET_DSTADDR      13  /* Set destination address (PPP only) */
#define IPNET_IFCONFIG_CMD_SET_LLADDR       14  /* Set driver hardware address. */
#define IPNET_IFCONFIG_CMD_CREATE           15  /* Create a pseudo-deveice */
#define IPNET_IFCONFIG_CMD_DESTROY          16  /* Destroy a pseudo-device */
#define IPNET_IFCONFIG_CMD_VLAN_CONF        17  /* Configure a virtual LAN interface */
#define IPNET_IFCONFIG_CMD_TUNNEL           18  /* Configure tunnel endpoints */
#define IPNET_IFCONFIG_CMD_ENABLE_LINK0     19  /* Enable link0 flag */
#define IPNET_IFCONFIG_CMD_DISABLE_LINK0    20  /* Disable link0 flag */
#define IPNET_IFCONFIG_CMD_ENABLE_LINK1     21  /* Enable link1 flag */
#define IPNET_IFCONFIG_CMD_DISABLE_LINK1    22  /* Disable link1 flag */
#define IPNET_IFCONFIG_CMD_ENABLE_LINK2     23  /* Enable link2 flag */
#define IPNET_IFCONFIG_CMD_DISABLE_LINK2    24  /* Disable link2 flag */
#define IPNET_IFCONFIG_CMD_ENABLE_PROMISC   25  /* Enable link promisc mode */
#define IPNET_IFCONFIG_CMD_DISABLE_PROMISC  26  /* Disable link promisc mode */
#define IPNET_IFCONFIG_CMD_VLANPRI_CONF     27
#define IPNET_IFCONFIG_CMD_VERBOSE          28  /* Verbose output (multicast & anycast addresses) */
#define IPNET_IFCONFIG_CMD_IPV4_ONLY        29  /* Show only IPv4 addresses */
#define IPNET_IFCONFIG_CMD_IPV6_ONLY        30  /* Show only IPv6 addresses */

#define IPNET_IFCONFIG_OPT_NETMASK       1
#define IPNET_IFCONFIG_OPT_PREFIXLEN     2
#define IPNET_IFCONFIG_OPT_TENTATIVE     3
#define IPNET_IFCONFIG_OPT_VALID         4
#define IPNET_IFCONFIG_OPT_PREFERRED     5
#define IPNET_IFCONFIG_OPT_TEMPORARY     6
#define IPNET_IFCONFIG_OPT_ANYCAST       7
#define IPNET_IFCONFIG_OPT_HOMEADDRESS   8
#define IPNET_IFCONFIG_OPT_GRECSUM       9
#define IPNET_IFCONFIG_OPT_CGA          10

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
    int     cmd;            /* The command to perform */
    Ip_u32  ifindex;        /* The interface to operate on (0 == all, valid for show command) */
    int     domain;         /* The domain to operate in (0 == all, valid for show command) */
    Ip_bool verbose;
    Ip_bool ipv4_only;
    Ip_bool ipv6_only;
    union
    {
        struct Ipnet_ifconfig_in_addr  inet;
        struct Ipnet_ifconfig_in6_addr inet6;
        Ip_u16                         vr;
        Ip_u32                         mtu;
        char                          *lladdr;
        struct Ip_ifreq                create;
        Ip_bool                        promisc_mode;
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

IP_GLOBAL void
ipstack_ifconfig_print_info(Ip_fd sock, Ip_u32 ifindex, int af, Ip_bool full_output);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_STATIC Ipcom_cmd_int_str_map ipnet_cmd_map[] =
{
    { IPNET_IFCONFIG_CMD_UP,             "up"      },
    { IPNET_IFCONFIG_CMD_DOWN,           "down"    },
    { IPNET_IFCONFIG_CMD_ADD_ADDR,       "add"     },
    { IPNET_IFCONFIG_CMD_DELETE_ADDR,    "del"     },
    { IPNET_IFCONFIG_CMD_DELETE_ADDR,    "delete"  },
    { IPNET_IFCONFIG_CMD_SET_MTU,        "mtu"     },
    { IPNET_IFCONFIG_CMD_SET_VR,         "vr"      },
    { IPNET_IFCONFIG_CMD_SET_VR,         "rtab"    },
    { IPNET_IFCONFIG_CMD_DETACH,         "detach"  },
#ifndef IP_PORT_LAS
    { IPNET_IFCONFIG_CMD_ENABLE_DHCP,    "dhcp"    },
    { IPNET_IFCONFIG_CMD_DISABLE_DHCP,   "-dhcp"   },
#endif
    { IPNET_IFCONFIG_CMD_SET_DSTADDR,    "dstaddr" },
    { IPNET_IFCONFIG_CMD_SET_LLADDR,     "lladdr"  },
    { IPNET_IFCONFIG_CMD_CREATE,         "create"  },
    { IPNET_IFCONFIG_CMD_DESTROY,        "destroy" },
#ifdef IPNET_USE_VLAN
    { IPNET_IFCONFIG_CMD_VLAN_CONF,      "vlan"    },
    { IPNET_IFCONFIG_CMD_VLAN_CONF,      "vlanif"  },
    { IPNET_IFCONFIG_CMD_VLANPRI_CONF,   "vlanpri"  },
#endif /* IPNET_USE_VLAN */
#ifdef IPNET_USE_TUNNEL
    { IPNET_IFCONFIG_CMD_TUNNEL,         "tunnel"  },
#endif
    { IPNET_IFCONFIG_CMD_ENABLE_LINK0,   "link0"   },
    { IPNET_IFCONFIG_CMD_DISABLE_LINK0,  "-link0"  },
    { IPNET_IFCONFIG_CMD_ENABLE_LINK1,   "link1"   },
    { IPNET_IFCONFIG_CMD_DISABLE_LINK1,  "-link1"  },
    { IPNET_IFCONFIG_CMD_ENABLE_LINK2,   "link2"   },
    { IPNET_IFCONFIG_CMD_DISABLE_LINK2,  "-link2"  },
    { IPNET_IFCONFIG_CMD_ENABLE_PROMISC, "promisc"  },
    { IPNET_IFCONFIG_CMD_DISABLE_PROMISC,"-promisc"  },
    { -1,                                IP_NULL   }
};

IP_STATIC Ipcom_cmd_int_str_map ipnet_switch_map[] =
{
    { IPNET_IFCONFIG_CMD_SHOW_INFO,    "-a"    },
    { IPNET_IFCONFIG_CMD_HELP,         "-h"    },
    { IPNET_IFCONFIG_CMD_VERBOSE,      "-v"    },
    { IPNET_IFCONFIG_CMD_IPV4_ONLY,    "-4"    },
    { IPNET_IFCONFIG_CMD_IPV6_ONLY,    "-6"    },
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
    { IPNET_IFCONFIG_OPT_GRECSUM,      "grecsum" },
#ifdef IPNET_USE_RFC3971
    { IPNET_IFCONFIG_OPT_CGA,          "cga" },
#endif
    { -1,                              IP_NULL     }
};


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipnet_cmd_ifconfig_create_mask
 *===========================================================================
 * Description: Creates a network mask with the 'netbits' leftmost bits set.
 * Parameters:  [in] mask - Pointer to a buffer where the mask should be stored.
 *              [in] netbits - The number of set bits.
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_ifconfig_create_mask(void *mask, int netbits)
{
    int i;

    for (i = 0; i < netbits; i++)
    {
        IP_BIT_SET(((Ip_u16*) mask)[i / 16], ip_htons(0x8000 >> (i % 16)));
    }
}


/*
 *===========================================================================
 *                    ipnet_ifconfig_print_ipv4
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPCOM_USE_INET
IP_STATIC Ip_size_t
ipnet_ifconfig_print_ipv4(Ip_u8 *ifa_first,
                          Ip_size_t buf_len,
                          Ip_bool is_point_to_point,
                          Ip_bool skip,
                          Ip_bool verbose)
{
    char                     str[IP_INET_ADDRSTRLEN];
    struct Ipnet_ifa_msghdr *ifa;
    struct Ip_sockaddr_in   *addrs[IPNET_RTAX_MAX];
    Ip_size_t                offset;

    offset = 0;
    while (offset < buf_len)
    {
        ifa = (struct Ipnet_ifa_msghdr *) &ifa_first[offset];

        ipnet_cmd_init_addrs(ifa + 1, ifa->ifam_addrs, (struct Ip_sockaddr **) addrs);

        if (ifa->ifam_type != IPNET_RTM_NEWADDR || addrs[IPNET_RTAX_IFA]->sin_family != IP_AF_INET)
            break;

        offset += ifa->ifam_msglen;

        if (skip
            || (IP_IN_CLASSD(addrs[IPNET_RTAX_IFA]->sin_addr.s_addr) && !verbose))
            continue;

        ipcom_printf("\tinet %s",
                     ipcom_inet_ntop(IP_AF_INET, &addrs[IPNET_RTAX_IFA]->sin_addr, str, sizeof(str)));

        ipcom_printf("  mask %s",
                     ipcom_inet_ntop(IP_AF_INET, &addrs[IPNET_RTAX_NETMASK]->sin_addr, str, sizeof(str)));

        if (IP_BIT_ISSET(ifa->ifam_addrs, IPNET_RTA_BRD))
        {
            if (is_point_to_point)
            {
                ipcom_printf("  peer %s",
                             ipcom_inet_ntop(IP_AF_INET, &addrs[IPNET_RTAX_BRD]->sin_addr, str, sizeof(str)));
            }
            else
            {
                ipcom_printf("  broadcast %s",
                             ipcom_inet_ntop(IP_AF_INET, &addrs[IPNET_RTAX_BRD]->sin_addr, str, sizeof(str)));
            }
        }

        ipcom_printf(IP_LF);

    }

    return offset;
}
#endif /* IPCOM_USE_INET */


/*
 *===========================================================================
 *                    ipnet_ifconfig_print_ipv6
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPCOM_USE_INET6
IP_STATIC Ip_size_t
ipnet_ifconfig_print_ipv6(Ip_fd fd,
                          Ip_u8 *ifa_first,
                          Ip_size_t buf_len,
                          Ip_bool skip,
                          Ip_bool verbose)
{
    struct Ip_in6_aliasreq   ifareq;
    struct Ipnet_ifa_msghdr *ifa;
    struct Ip_sockaddr_in6  *addrs[IPNET_RTAX_MAX];
    Ip_size_t                offset = 0;
    char                     str[IP_INET6_ADDRSTRLEN];
    int                      i;

    for (i = 0; i < (verbose ? 2 : 1); i++)
    {
        offset = 0;
        /* i==0 print unicast, i==1 print multicast */
        while (offset < buf_len)
        {
            ifa = (struct Ipnet_ifa_msghdr *) &ifa_first[offset];
            ipnet_cmd_init_addrs(ifa + 1, ifa->ifam_addrs, (struct Ip_sockaddr **) addrs);

            if (ifa->ifam_type != IPNET_RTM_NEWADDR || addrs[IPNET_RTAX_IFA]->sin6_family != IP_AF_INET6)
                break;

            offset += ifa->ifam_msglen;

            ipcom_memset(&ifareq, 0, sizeof(ifareq));
            ipcom_if_indextoname(ifa->ifam_index, ifareq.ifra_name);
            ipcom_memcpy(&ifareq.ifra_addr, addrs[IPNET_RTAX_IFA], sizeof(ifareq.ifra_addr));
            (void)ipcom_socketioctl(fd, IP_SIOCXGIFADDR_IN6, &ifareq);

            if (skip || (IP_BIT_ISSET(ifareq.ifra_flags, IP_IN6_IFF_ANYCAST) && !verbose))
                continue;

            if ((i == 0 && IP_IN6_IS_ADDR_MULTICAST(&addrs[IPNET_RTAX_IFA]->sin6_addr) == IP_FALSE)
                || (i == 1 && IP_IN6_IS_ADDR_MULTICAST(&addrs[IPNET_RTAX_IFA]->sin6_addr)))
            {
                ipcom_printf("\tinet6 %s %s",
                             i == 0 ? "unicast" : "multicast",
                             ipcom_inet_ntop(IP_AF_INET6, &addrs[IPNET_RTAX_IFA]->sin6_addr, str, sizeof(str)));
                if (IPNET_IP6_IS_SCOPE_LINK_LOCAL(&addrs[IPNET_RTAX_IFA]->sin6_addr))
                    ipcom_printf("%%%s", ifareq.ifra_name);

                if (IP_BIT_ISSET(ifa->ifam_addrs, IPNET_RTA_BRD))
                {
                    ipcom_printf("  peer %s",
                                 ipcom_inet_ntop(IP_AF_INET6, &addrs[IPNET_RTAX_BRD]->sin6_addr, str, sizeof(str)));
                    if (IP_IN6_IS_ADDR_LINK_LOCAL(&addrs[IPNET_RTAX_BRD]->sin6_addr))
                        ipcom_printf("%%%s", ifareq.ifra_name);
                }

                ipcom_printf("  prefixlen %u",
                             ipcom_sockaddr_to_prefixlen((struct Ip_sockaddr *) addrs[IPNET_RTAX_NETMASK]));

                if (IP_BIT_ISSET(ifareq.ifra_flags, IP_IN6_IFF_TEMPORARY))
                    ipcom_printf("  temporary");
                if (IP_BIT_ISSET(ifareq.ifra_flags, IP_IN6_IFF_TENTATIVE))
                    ipcom_printf("  tentative");
                if (IP_BIT_ISSET(ifareq.ifra_flags, IP_IN6_IFF_HOMEADDRESS))
                    ipcom_printf("  homeaddress");
                if (IP_BIT_ISSET(ifareq.ifra_flags, IP_IN6_IFF_AUTONOMOUS))
                    ipcom_printf("  autonomous");
                if (IP_BIT_ISSET(ifareq.ifra_flags, IP_IN6_IFF_ANYCAST))
                    ipcom_printf("  anycast");
                if (IP_BIT_ISSET(ifareq.ifra_flags, IP_IN6_IFF_DEPRECATED))
                    ipcom_printf("  deprecated");
                if (IP_BIT_ISSET(ifareq.ifra_flags, IP_IN6_IFF_AUTOMATIC))
                    ipcom_printf("  automatic");
#ifdef IPNET_USE_RFC3971
                if (IP_BIT_ISSET(ifareq.ifra_flags, IP_IN6_IFF_CGA))
                    ipcom_printf("  CGA");
#endif

                ipcom_printf(IP_LF);
            }
        }
    }

    return offset;
}
#endif /* IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                    ipnet_ifconfig_print_tunnel
 *===========================================================================
 * Description: Print information specific to tunnel interfaces.
 * Parameters:  ifindex - The tunnle interface to print information for.
 * Returns:
 *
 */
#ifdef IPNET_USE_TUNNEL
IP_STATIC void
ipnet_ifconfig_print_tunnel(Ip_fd fd, Ip_u32 ifindex)
{
    struct Ip_ip_tunnel_param p;
    char                      str[IP_INET6_ADDRSTRLEN];

    ipcom_memset(&p, 0, sizeof(p));
    if (ipcom_if_indextoname(ifindex, p.ifr_name) == IP_NULL
        || ipcom_socketioctl(fd, IP_SIOCGETTUNNEL, &p) < 0)
        return;

    if (p.remote_addr.sa.sa_family != IP_AF_UNSPEC)
    {
        ipcom_printf("  %s %s --> ",
                     p.protocol == IP_IPPROTO_GRE ? "GRE"
                     : p.protocol == IP_IPPROTO_MOBILE ? "Minimal Encap"
                     : p.local_addr.sa.sa_family == IP_AF_INET ? "IPv[4|6]-over-IPv4" : "IPv[4|6]-over-IPv6",
                     ipcom_cmd_sockaddr_to_str(&p.local_addr.sa, IP_FALSE, IP_TRUE, str, sizeof(str)));
        ipcom_printf("%s  ttl:%d",
                     ipcom_cmd_sockaddr_to_str(&p.remote_addr.sa, IP_FALSE, IP_TRUE, str, sizeof(str)),
                     p.ttl);
    }
}
#endif /* IPNET_USE_TUNNEL */


/*
 *===========================================================================
 *                    ipnet_ifconfig_print_ppp_multilink
 *===========================================================================
 * Description: Print information specific to PPP-ML interfaces.
 * Parameters:  ifindex - The PPP interface to print information for.
 * Returns: void
 *
 */
#ifdef IPPPP_USE_PPPMP
IP_STATIC void
ipnet_ifconfig_print_ppp_multilink(Ip_fd fd, Ip_u32 ifindex)
{
    struct Ip_pppreq  pppreq;

    ipcom_memset(&pppreq, 0, sizeof(struct Ip_pppreq));
    if (ipcom_if_indextoname(ifindex, pppreq.pppr_name) == IP_NULL
        || ipcom_socketioctl(fd, IP_SIOCGPPPIFFLAGS, &pppreq) < 0)
        return;

    if (IP_BIT_ISSET(pppreq.pppru.pppflags, IPPPP_PPPIFFLAG_MP_BUNDLE))
        ipcom_printf("\n        PPP Multilink bundle");
    if (IP_BIT_ISSET(pppreq.pppru.pppflags, IPPPP_PPPIFFLAG_MP_MEMBER))
        ipcom_printf("\n        PPP Multilink member link");
}
#endif /* IPPPP_USE_PPPMP */


/*
 *===========================================================================
 *                    ipnet_ifconfig_print_vlan
 *===========================================================================
 * Description: Print information specific to virtual LAN interfaces.
 * Parameters:  ifindex - The VLAN interface to print information for.
 * Returns:
 *
 */
#ifdef IPNET_USE_VLAN
IP_STATIC void
ipnet_ifconfig_print_vlan(Ip_fd fd, Ip_u32 ifindex)
{
    struct Ip_ifreq   ifreq;
    struct Ip_vlanreq vlanreq;

    ipcom_memset(&vlanreq, 0, sizeof(struct Ip_vlanreq));
    ifreq.ip_ifr_data = &vlanreq;
    if (ipcom_if_indextoname(ifindex, ifreq.ifr_name) == IP_NULL
        || ipcom_socketioctl(fd, IP_SIOCGETVLAN, &ifreq) < 0)
        return;

    if (vlanreq.vlr_tag)
        ipcom_printf(IP_LF"\tvlan: %d vlan priority: %d parent: %s ",
                     vlanreq.vlr_tag, vlanreq.vlr_pri, vlanreq.vlr_parent);
}
#endif /* IPNET_USE_VLAN */


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

#ifndef IP_PORT_LAS
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
#endif

/*
 *===========================================================================
 *                    ipnet_ifconfig_if_detach
 *===========================================================================
 * Description: Detaches a network interface from IPNET.
 * Parameters:  fd - A socket descriptor.
 *              ifindex - The interface index for the interface to detach.
 * Returns:
 *
 */
IP_STATIC int
ipnet_ifconfig_if_detach(Ip_fd fd, Ip_u32 ifindex)
{
    struct Ip_ifreq ifreq;

    ipcom_memset(&ifreq, 0, sizeof(struct Ip_ifreq));
    (void)ipcom_if_indextoname(ifindex, ifreq.ifr_name);
    if (ipcom_socketioctl(fd, IP_SIOCXDETACH, &ifreq) < 0)
    {
        int err = ipcom_errno;
        /* Try harder; PPPOE interfaces are a bit delayed */
        if (err == IP_ERRNO_EBUSY)
        {
            struct Ip_ifreq ifreq2;

            ipcom_memset(&ifreq2, 0, sizeof(struct Ip_ifreq));
            (void)ipcom_if_indextoname(ifindex, ifreq2.ifr_name);
            if (ipcom_socketioctl(fd, IP_SIOCGIFFLAGS, &ifreq2) == 0)
            {
                if (IP_BIT_ISFALSE(ifreq2.ifr_ifru.ifru_flags, IP_IFF_UP) &&
                    IP_BIT_ISTRUE(ifreq2.ifr_ifru.ifru_flags, IP_IFF_RUNNING))
                {
                    int i;
                    /* The interface is down; probably just running, waiting
                     * to be stopped (PPPOE) */
                    for (i = 0; i < 10; i++)
                    {
                        ipcom_sleep(1);
                        if (ipcom_socketioctl(fd, IP_SIOCXDETACH, &ifreq) == 0)
                            return 0;
                    }
                }
            }
        }

        ipcom_printf("ifconfig: failed to detach interface %s: %s" IP_LF,
            ifreq.ifr_name, ipcom_strerror(err));
        return -err;
    }
    return 0;
}


#ifdef IPCOM_USE_INET
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
    IPCOM_SA_LEN_SET(in, sizeof(struct Ip_sockaddr_in));
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
    IPCOM_SA_LEN_SET(in_addr, sizeof(struct Ip_sockaddr_in));
    in_addr->sin_addr   = *addr;

    if (netmask != IP_NULL)
    {
        if (!add)
            IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("warning: mask is ignored in delete operations" IP_LF));
        else
        {
            in_mask = (struct Ip_sockaddr_in *) &ifal.ifra_mask;
            in_mask->sin_family = IP_AF_INET;
            IPCOM_SA_LEN_SET(in_mask, sizeof(struct Ip_sockaddr_in));
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
    {
    	int disable;

    	disable = ipcom_sysvar_get_as_int("ipdhcpc.auto_disable", IP_NULL);

    	if (IP_IN_CLASSD(addr->s_addr) == IP_FALSE && disable > 0)
    		/* Unicast address was changed manually. Disable dhcp on the interface */
    		return ipnet_ifconfig_if_change_dhcp(fd, ifindex, IP_FALSE);

    }
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
    IPCOM_SA_LEN_SET(&ifareq.ifra_addr, sizeof(struct Ip_sockaddr_in6));
    ifareq.ifra_addr.sin6_addr   = param->addr;
    if (IP_IN6_IS_ADDR_LINK_LOCAL(&param->addr))
        ifareq.ifra_addr.sin6_scope_id = ifindex;

    ifareq.ifra_prefixmask.sin6_family = IP_AF_INET6;
    IPCOM_SA_LEN_SET(&ifareq.ifra_prefixmask, sizeof(struct Ip_sockaddr_in6));

    ipnet_cmd_ifconfig_create_mask(&ifareq.ifra_prefixmask.sin6_addr, param->prefixlen);

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


/*
 *===========================================================================
 *                    ipnet_ifconfig_ipv6_delete
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_ifconfig_ipv6_delete(Ip_fd fd, Ip_u32 ifindex, struct Ip_in6_addr *addr)
{
    struct Ip_in6_ifreq ifreq;

    ipcom_memset(&ifreq, 0, sizeof(ifreq));
    (void)ipcom_if_indextoname(ifindex, ifreq.ifr_name);
    ifreq.ifr_ifru.ifru_addr.sin6_family = IP_AF_INET6;
    IPCOM_SA_LEN_SET(&ifreq.ifr_ifru.ifru_addr, sizeof(struct Ip_sockaddr_in6));
    ifreq.ifr_ifru.ifru_addr.sin6_addr   = *addr;
    if (IP_IN6_IS_ADDR_LINK_LOCAL(addr))
        ifreq.ifr_ifru.ifru_addr.sin6_scope_id = ifindex;


    if (ipcom_socketioctl(fd, IP_SIOCDIFADDR_IN6, &ifreq) < 0)
    {
        ipcom_printf("ifconfig: ipcom_socketioctl() failed to delete IPv6 address: %s" IP_LF,
            ipcom_strerror(ipcom_errno));
        return -ipcom_errno;
    }
    return 0;
}

#endif /* IPCOM_USE_INET6 */

/*
 *===========================================================================
 *                    ipnet_ifconfig_set_vr
 *===========================================================================
 * Description: Sets the virtual router used by an interface.
 * Parameters:  sock - A socket.
 *              ifindex - The interface index.
 *              vr - The index of the virtual router to set.
 *              silent - Set to IP_TRUE if this command is run
 *              without standard output connected.
 * Returns:
 *
 */
IP_STATIC int
ipnet_ifconfig_set_vr(Ip_fd sock, Ip_u32 ifindex, int vr, Ip_bool silent)
{
    struct Ip_ifreq ifreq;

    ipcom_memset(&ifreq, 0, sizeof(struct Ip_ifreq));
    (void)ipcom_if_indextoname(ifindex, ifreq.ifr_name);
    ifreq.ip_ifr_vr = (Ip_u16) vr;

    if (ipcom_socketioctl(sock, IP_SIOCSIFVR, &ifreq) < 0)
    {
        IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("ifconfig: set vr failed: %s" IP_LF, ipcom_strerror(ipcom_errno)));
        return -ipcom_errno;
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ifconfig_set_mtu
 *===========================================================================
 * Description: Sets the MTU for an interface.
 * Parameters:  sock - A socket.
 *              ifindex - The interface index.
 *              mtu - The new MTU.
 * Returns:
 *
 */
IP_STATIC int
ipnet_ifconfig_set_mtu(Ip_fd sock, Ip_u32 ifindex, int mtu)
{
    struct Ip_ifreq ifreq;

    ipcom_memset(&ifreq, 0, sizeof(struct Ip_ifreq));
    (void)ipcom_if_indextoname(ifindex, ifreq.ifr_name);
    ifreq.ip_ifr_mtu = mtu;

    if (ipcom_socketioctl(sock, IP_SIOCSIFMTU, &ifreq) < 0)
    {
        ipcom_printf("ifconfig: set MTU failed: %s" IP_LF, ipcom_strerror(ipcom_errno));
        return -ipcom_errno;
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ifconfig_set_dstaddr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_ifconfig_set_dstaddr(Ip_fd sock, Ipnet_cmd_ifconfig *cmdp)
{
    if(cmdp->domain != IP_AF_INET)
    {
        ipcom_printf("ifconfig: inet domain only."IP_LF);
        return -IP_ERRNO_EINVAL;
    }
    else
    {
        struct Ip_ifreq ifreq;
        struct Ip_sockaddr_in *in_addr;

        ipcom_memset(&ifreq, 0, sizeof(struct Ip_ifreq));
        (void)ipcom_if_indextoname(cmdp->ifindex, ifreq.ifr_name);
        in_addr = (struct Ip_sockaddr_in *) &ifreq.ip_ifr_dstaddr;
        in_addr->sin_family      = IP_AF_INET;
        IPCOM_SA_LEN_SET(in_addr, sizeof(struct Ip_sockaddr_in));
        in_addr->sin_addr.s_addr = cmdp->param.inet.addr.s_addr;

        if (ipcom_socketioctl(sock, IP_SIOCSIFDSTADDR, &ifreq) < 0)
        {
            ipcom_printf("ifconfig: set destination address failed: %s" IP_LF, ipcom_strerror(ipcom_errno));
            return -ipcom_errno;
        }
    }
    return 0;
}


/*
 *===========================================================================
 *                    iplite_ifconfig_set_lladdr
 *===========================================================================
 * Description: Sets the link layer address for an interface.
 * Parameters:  sock - A socket.
 *              cmdp - command info.
 * Returns:
 *
 */
IP_STATIC int
ipnet_ifconfig_set_lladdr(Ip_fd sock, Ipnet_cmd_ifconfig *cmdp)
{
    struct Ip_ifreq       ifreq;
    struct Ip_sockaddr_dl dl_addr;

    if (ipcom_getsockaddrbyaddr(IP_AF_LINK, cmdp->param.lladdr, (struct Ip_sockaddr *)&dl_addr) != 0)
    {
        ipcom_printf("Set LLADDR failed: invalid address '%s'" IP_LF, cmdp->param.lladdr);
        return -IP_ERRNO_EINVAL;
    }

    ipcom_memset(&ifreq, 0, sizeof(struct Ip_ifreq));
    (void)ipcom_if_indextoname(cmdp->ifindex, ifreq.ifr_name);

    ifreq.ip_ifr_addr.sa_family = IP_AF_LINK;
    IPCOM_SA_LEN_SET(&ifreq.ip_ifr_addr, dl_addr.sdl_alen);
    ipcom_memcpy(ifreq.ip_ifr_addr.sa_data, dl_addr.sdl_data, dl_addr.sdl_alen);

    if (ipcom_socketioctl(sock, IP_SIOCSIFLLADDR, &ifreq) < 0)
    {
        ipcom_printf("Set LLADDR failed: %s" IP_LF, ipcom_strerror(ipcom_errno));
        return -ipcom_errno;
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ifconfig_create
 *===========================================================================
 * Description: Creates a new pseudo-interface.
 * Parameters:  sock - A socket.
 *              cmdp - command info.
 * Returns:
 *
 */
IP_STATIC int
ipnet_ifconfig_create(Ip_fd sock, Ipnet_cmd_ifconfig *cmdp)
{
    if (ipcom_socketioctl(sock, IP_SIOCIFCREATE, &cmdp->param.create) < 0)
    {
        ipcom_printf("Creating %s failed: %s" IP_LF,
                     cmdp->param.create.ifr_name,
                     ipcom_strerror(ipcom_errno));
        return -ipcom_errno;
    }
    return 0;
}

/*
 *===========================================================================
 *                    ipnet_ifconfig_destroy
 *===========================================================================
 * Description: Destroys a pseudo-interface.
 * Parameters:  sock - A socket.
 *              cmdp - command info.
 * Returns:
 *
 */
IP_STATIC int
ipnet_ifconfig_destroy(Ip_fd sock, Ipnet_cmd_ifconfig *cmdp)
{
    struct Ip_ifreq ifreq;

    ipcom_memset(&ifreq, 0, sizeof(ifreq));
    (void)ipcom_if_indextoname(cmdp->ifindex, ifreq.ifr_name);
    if (ipcom_socketioctl(sock, IP_SIOCIFDESTROY, &ifreq) < 0)
    {
        ipcom_printf("Destroying %s failed: %s" IP_LF,
                     ifreq.ifr_name,
                     ipcom_strerror(ipcom_errno));
        return -ipcom_errno;
    }
    return 0;
}


#ifdef IPNET_USE_VLAN
/*
 *===========================================================================
 *                    ipnet_ifconfig_vlan_conf
 *===========================================================================
 * Description: Configures a virtual lan interface.
 * Parameters:  sock - A socket.
 *              cmdp - command info.
 * Returns:
 *
 */
IP_STATIC int
ipnet_ifconfig_vlan_conf(Ip_fd sock, Ipnet_cmd_ifconfig *cmdp)
{
    struct Ip_ifreq ifreq;

    (void)ipcom_if_indextoname(cmdp->ifindex, ifreq.ifr_name);
    ifreq.ip_ifr_data = &cmdp->param.vlan;
    if (ipcom_socketioctl(sock, IP_SIOCSETVLAN, &ifreq) < 0)
    {
        ipcom_printf("Setting VLAN parameters for %s failed: %s" IP_LF,
                     ifreq.ifr_name,
                     ipcom_strerror(ipcom_errno));
        return -ipcom_errno;
    }
    return 0;
}

/*
 *===========================================================================
 *                    ipnet_ifconfig_vlanpri_conf
 *===========================================================================
 * Description: Configures the (optional) vlan priority for a virtual lan
 *              interface.
 * Parameters:  sock - A socket.
 *              cmdp - command info.
 * Returns:
 *
 */
IP_STATIC int
ipnet_ifconfig_vlanpri_conf(Ip_fd sock, Ipnet_cmd_ifconfig *cmdp)
{
    struct Ip_ifreq ifreq;

    (void)ipcom_if_indextoname(cmdp->ifindex, ifreq.ifr_name);
    ifreq.ip_ifr_data = &cmdp->param.vlan;
    if (ipcom_socketioctl(sock, IP_SIOCSETVLANPRI, &ifreq) < 0)
    {
        ipcom_printf("Setting VLAN priority for %s failed: %s" IP_LF,
                     ifreq.ifr_name,
                     ipcom_strerror(ipcom_errno));
        return -ipcom_errno;
    }
    return 0;
}

#endif /* IPNET_USE_VLAN */


#ifdef IPNET_USE_TUNNEL
/*
 *===========================================================================
 *                    ipnet_ifconfig_tunnel_conf
 *===========================================================================
 * Description: Configures a virtual lan interface.
 * Parameters:  sock - A socket.
 *              cmdp - command info.
 * Returns:
 *
 */
IP_STATIC int
ipnet_ifconfig_tunnel_conf(Ip_fd sock, Ipnet_cmd_ifconfig *cmdp)
{
    (void)ipcom_if_indextoname(cmdp->ifindex, cmdp->param.tunnel.ifr_name);
    if (ipcom_socketioctl(sock, IP_SIOCCHGTUNNEL, &cmdp->param.tunnel) < 0)
    {
        ipcom_printf("Setting tunnel parameters for %s failed: %s" IP_LF,
                     cmdp->param.tunnel.ifr_name,
                     ipcom_strerror(ipcom_errno));
        return -ipcom_errno;
    }
    return 0;
}
#endif /* IPNET_USE_TUNNEL */


/*
 *===========================================================================
 *                    ipnet_ifconfig_if_change_link
 *===========================================================================
 * Description: Change the link2 state of a network interface.
 * Parameters:  fd - A socket descriptor.
 *              ifindex - The interface index for the interface to change.
 *              link_en - Set to IP_TRUE if the IP_IFF_LINK2 flag should be set.
 *              link_nr - The link flag number.
 * Returns:
 *
 */

IP_STATIC int
ipnet_ifconfig_if_change_link(Ip_fd fd, Ip_u32 ifindex, Ip_bool link_en, int link_nr)
{
#ifdef IP_PORT_LAS
    int             get_cmd = IP_X_SIOCGIFLINKFLAGS;
    int             set_cmd = IP_X_SIOCSIFLINKFLAGS;
    Ip_u32          link_bit;
    Ip_u32          flags;
#else
    int             get_cmd = IP_SIOCGIFFLAGS;
    int             set_cmd = IP_SIOCSIFFLAGS;
    Ip_u16          link_bit;
    Ip_u16          flags;
#endif
    struct Ip_ifreq ifreq;

    ipcom_memset(&ifreq, 0, sizeof(struct Ip_ifreq));
    (void)ipcom_if_indextoname(ifindex, ifreq.ifr_name);
    if (ipcom_socketioctl(fd, get_cmd, &ifreq) < 0)
    {
        ipcom_printf("ifconfig: failed to get interface flags: %s" IP_LF, ipcom_strerror(ipcom_errno));
        return -ipcom_errno;
    }

    switch (link_nr)
    {
    case 0:
        link_bit = IP_IFF_LINK0;
        break;
    case 1:
        link_bit = IP_IFF_LINK1;
        break;
    default:
        link_bit = IP_IFF_LINK2;
        break;
    }

    flags = ifreq.ip_ifr_flags;
#ifdef IP_PORT_LAS
    flags = flags << 16;
#endif
    if (link_en)
        IP_BIT_SET(flags, link_bit);
    else
        IP_BIT_CLR(flags, link_bit);

#ifdef IP_PORT_LAS
    flags = flags >> 16;
#endif
    ifreq.ip_ifr_flags = (Ip_u16)flags;

    if (ipcom_socketioctl(fd, set_cmd, &ifreq) < 0)
    {
        ipcom_printf("ifconfig: failed to set interface flags: %s"IP_LF, ipcom_strerror(ipcom_errno));
        return -ipcom_errno;
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ifconfig_if_promisc_link
 *===========================================================================
 * Description: Change the promisc mode state of a network interface.
 * Parameters:  fd - A socket descriptor.
 *              ifindex - The interface index for the interface to change.
 *              link_en - Set to IP_TRUE if the promisc mode is to be enabled.
 * Returns:
 *
 */

IP_STATIC int
ipnet_ifconfig_if_promisc_link(Ip_fd fd, Ip_u32 ifindex, Ip_bool link_en)
{
#ifdef IP_PORT_LAS
    (void) fd;
    (void) ifindex;
    (void) link_en;
    ipcom_printf("ipnet_ifconfig_if_promisc_link: operation not supported on this port" IP_LF);
    return -1;
#else
    struct Ip_ifreq ifreq;

    ipcom_memset(&ifreq, 0, sizeof(struct Ip_ifreq));
    (void)ipcom_if_indextoname(ifindex, ifreq.ifr_name);
    ifreq.ifr_ifru.ifru_opt = link_en;

    if (ipcom_socketioctl(fd, IP_SIOCXPROMISC, &ifreq) < 0)
    {
        ipcom_printf("ifconfig: failed to set interface flags: %s"IP_LF, ipcom_strerror(ipcom_errno));
        return -ipcom_errno;
    }
    return 0;
#endif
}


/*
 *===========================================================================
 *                    ipnet_ifconfig_print_usage
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_ifconfig_print_usage(void)
{
    ipcom_printf(IP_LF);
    ipcom_printf("NAME"IP_LF);
    ipcom_printf("   ifconfig - configure network interface parameters"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("SYNOPSIS"IP_LF);
    ipcom_printf("   ifconfig <interface> inet <address> [optional parameters]"IP_LF);
    ipcom_printf("   ifconfig <interface> <family> add <address> [optional parameters]"IP_LF);
    ipcom_printf("   ifconfig <interface> <family> delete <address>"IP_LF);
    ipcom_printf("   ifconfig <interface> <family> dstaddr <address>"IP_LF);
    ipcom_printf("   ifconfig <interface> mtu <value>"IP_LF);
    ipcom_printf("   ifconfig <interface> lladdr <string>"IP_LF);
    ipcom_printf("   ifconfig <interface> up"IP_LF);
    ipcom_printf("   ifconfig <interface> down"IP_LF);
    ipcom_printf("   ifconfig <interface> detach"IP_LF);
    ipcom_printf("   ifconfig <interface> promisc"IP_LF);
    ipcom_printf("   ifconfig <interface> -promisc"IP_LF);
#ifndef IP_PORT_LAS
    ipcom_printf("   ifconfig <interface> dhcp"IP_LF);
    ipcom_printf("   ifconfig <interface> -dhcp"IP_LF);
#endif
    ipcom_printf("   ifconfig <interface> vr <value>"IP_LF);
    ipcom_printf("   ifconfig [<interface> | -a]"IP_LF);
    ipcom_printf("   ifconfig <interface> [create | destroy]"IP_LF);
#ifdef IPNET_USE_VLAN
    ipcom_printf("   ifconfig <interface> vlan <tag> vlanpri <value> vlanif <iface>"IP_LF);
#endif /* IPNET_USE_VLAN */
#ifdef IPNET_USE_TUNNEL
    ipcom_printf("   ifconfig <interface> <family> tunnel <local> <remote> [ttl]"IP_LF);
    ipcom_printf("   ifconfig <interface> link2"IP_LF);
#endif /* IPNET_USE_TUNNEL */
    ipcom_printf(IP_LF);
    ipcom_printf("DESCRIPTION"IP_LF);
    ipcom_printf("   ifconfig is used to assign an address to a network interface and/or"IP_LF);
    ipcom_printf("   configure network interface parameters."IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("COMMANDS"IP_LF);
    ipcom_printf("   -a           List all interfaces."IP_LF);
    ipcom_printf("   -v           Verbose output. NOTE: this switch must be the first"IP_LF);
    ipcom_printf("                switch of all if used."IP_LF);
    ipcom_printf("   inet <a>     Change the primary IPv4 address."IP_LF);
    ipcom_printf("   add <a>      Add 'a' IPv4 or IPv6 address."IP_LF);
    ipcom_printf("   delete <a>   Delete 'a' IPv4 or IPv6 address."IP_LF);
    ipcom_printf("   dstaddr <a>  Set remote address to 'a' (inet and PPP only)."IP_LF);
    ipcom_printf("   mtu <v>      Set interface MTU to 'v'."IP_LF);
    ipcom_printf("   lladdr <a>   Set interface link address to 'a'."IP_LF);
    ipcom_printf("   up           Bring the interface up."IP_LF);
    ipcom_printf("   down         Bring the interface down."IP_LF);
    ipcom_printf("   detach       Detaches interface (note: must be down first)."IP_LF);
    ipcom_printf("   vr <v>       Set virtual router to 'v' (default = 0)."IP_LF);
    ipcom_printf("   dhcp         Enable dhcp auto configuration (inet only)."IP_LF);
    ipcom_printf("   -dhcp        Disable dhcp auto configuration (inet only)"IP_LF);
    ipcom_printf("   link[0-2]    Enable special processing at the link level."IP_LF);
    ipcom_printf("   -link[0-2]   Disable special processing at the link level."IP_LF);
    ipcom_printf("   create       Create the specified network pseudo-device."IP_LF);
    ipcom_printf("   destroy      Destroy the specified network pseudo-device."IP_LF);
#ifdef IPNET_USE_VLAN
    ipcom_printf("   vlan <tag>   The virtual LAN tag (1-4095). Must be set at the"IP_LF);
    ipcom_printf("                same time as vlanif."IP_LF);
    ipcom_printf("   vlanif <iface> The physical Ethernet interface to use for this"IP_LF);
    ipcom_printf("                virtual lan."IP_LF);
    ipcom_printf("   vlanpri <value>   Set vlan class of service to 'value'. This optional"IP_LF);
    ipcom_printf("                     3-bit value is inserted to the vlan tag header"IP_LF);
    ipcom_printf("                     for packets sent from the vlan pseudo interface."IP_LF);
#endif /* IPNET_USE_VLAN */
#ifdef IPNET_USE_TUNNEL
    ipcom_printf("   tunnel <local> <remote> [ttl] If the interface is a gre or gif"IP_LF);
    ipcom_printf("                pseudo-interface, set the tunnel endpoints."IP_LF);
    ipcom_printf("   link0        GRE tunnel: Enable minimal encapsulation (RFC 2002)"IP_LF);
    ipcom_printf("   -link0       GRE tunnel: Use normal GRE tunnel operation"IP_LF);
#endif /* IPNET_USE_TUNNEL */
    ipcom_printf(IP_LF);
    ipcom_printf("VALUES"IP_LF);
    ipcom_printf("   <address>    The IPv4 or IPv6 address."IP_LF);
    ipcom_printf("   <family>     Address family. 'inet' or 'inet6'."IP_LF);
    ipcom_printf("   <interface>  Name of the interface."IP_LF);
    ipcom_printf("   <value>      Numeric value depending on the command."IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("OPTIONAL PARAMETERS"IP_LF);
    ipcom_printf("   netmask <a.b.c.d>  Set interface netmask to 'a.b.c.d' (inet only)"IP_LF);
    ipcom_printf("   prefixlen <value>  Set interface prefix (mask length) length to 'n'"IP_LF);
    ipcom_printf("   temporary          The address is temporary (inet6 only)"IP_LF);
    ipcom_printf("   tentative          Perform DAD before setting address (inet6 only)"IP_LF);
    ipcom_printf("   anycast            The address is anycast (inet6 only)"IP_LF);
    ipcom_printf("   homeaddress        The address is a homeaddress (inet6 only)"IP_LF);
    ipcom_printf("   preferred <value>  Number of seconds this address is preferred (inet6 only)"IP_LF);
    ipcom_printf("   valid <value>      Number of seconds this address is valid (inet6 only)"IP_LF);
#ifdef IPNET_USE_RFC3971
    ipcom_printf("   cga                The address is Cryptographically generated (inet6 only)"IP_LF);
#endif
    ipcom_printf(IP_LF);
}


/*
 *===========================================================================
 *                    ipnet_cmd_ifconfig_parse_arg
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_ifconfig_parse_arg(int argc,
                             int *next_arg,
                             char **argv,
                             Ipnet_cmd_ifconfig *cmd,
                             Ip_bool silent)
{
    int   domain;
    char *str_prefixlen;

    if (*next_arg == argc)
    {
        cmd->cmd = IPNET_IFCONFIG_CMD_SHOW_INFO;
        return 0;
    }

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
        /* BSD and Linux compat 'ifconfig eth0 inet xxx/y' or 'ifconfig eth0 xxx/y
           means change the primary IPv4 the address */
        if (ipcom_strcmp(argv[*next_arg - 1], "inet") == 0
            || ipcom_inet_pton(IP_AF_INET, argv[*next_arg], &cmd->param.inet) == 1)
        {
            cmd->cmd = IPNET_IFCONFIG_CMD_REPLACE_ADDR;
            --*next_arg;
            cmd->domain = IP_AF_INET;
        }
        else
        {
            IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("ifconfig: '%s' unknown command" IP_LF, argv[*next_arg]));
            return -IP_ERRNO_EINVAL;
        }
    }

    ++*next_arg;

    switch (cmd->cmd)
    {
    case IPNET_IFCONFIG_CMD_REPLACE_ADDR :
    case IPNET_IFCONFIG_CMD_ADD_ADDR :
    case IPNET_IFCONFIG_CMD_DELETE_ADDR :
    case IPNET_IFCONFIG_CMD_SET_DSTADDR :
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
                    goto no_more_addr_options;
                }
            }

        no_more_addr_options:
            if (prefixlen != -1 && cmd->param.inet.mask_is_set == IP_FALSE)
            {
                if (prefixlen <= 0 || prefixlen > 32)
                {
                    IPNET_IFCONFIG_CHECK_SILENT(
                        ipcom_printf("ifconfig: inet <prefixlen> must be a value 1 - 32" IP_LF));
                    return -IP_ERRNO_EINVAL;
                }
                cmd->param.inet.mask.s_addr = 0;
                ipnet_cmd_ifconfig_create_mask(&cmd->param.inet.mask, prefixlen);
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
                    ++*next_arg;
                    break;
                case IPNET_IFCONFIG_OPT_PREFERRED:
                    cmd->param.inet6.preferred_lifetime = ipcom_atoi(argv[*next_arg]);
                    ++*next_arg;
                    break;
#ifdef IPNET_USE_RFC3971
                case IPNET_IFCONFIG_OPT_CGA:
                    IP_BIT_SET(cmd->param.inet6.flags, IP_IN6_IFF_CGA);
                    break;
#endif
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

    case IPNET_IFCONFIG_CMD_SET_VR:
        if (*next_arg >= argc)
        {
            IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("ifconfig: missing VR <value>" IP_LF));
            return -IP_ERRNO_EINVAL;
        }
        cmd->param.vr  = (Ip_u16) ipcom_atoi(argv[*next_arg]);
        ++*next_arg;
        break;

    case IPNET_IFCONFIG_CMD_SET_MTU:
        if (*next_arg >= argc)
        {
            IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("ifconfig: missing mtu <value>" IP_LF));
            return -IP_ERRNO_EINVAL;
        }
        cmd->param.mtu = (Ip_u32) ipcom_atoi(argv[*next_arg]);
        ++*next_arg;
        break;

    case IPNET_IFCONFIG_CMD_SET_LLADDR:
        if (*next_arg >= argc)
        {
            IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("A hardware address must follow, e.g. 00:11:22:33:44:55" IP_LF));
            return -IP_ERRNO_EINVAL;
        }
        cmd->param.lladdr = argv[*next_arg];
        ++*next_arg;
        break;

#ifdef IPNET_USE_VLAN
    case IPNET_IFCONFIG_CMD_VLAN_CONF:
        if (argc - *next_arg < 3)
            goto vlan_conf_parse_error;

        --*next_arg;
        if (ipcom_strcmp(argv[*next_arg], "vlanif") == 0)
        {
            ipcom_strncpy(cmd->param.vlan.vlr_parent, argv[++*next_arg], IP_IFNAMSIZ);
            ++*next_arg;
        }

        if (ipcom_strcmp(argv[*next_arg], "vlan"))
            goto vlan_conf_parse_error;
        else
        {
            cmd->param.vlan.vlr_tag = (Ip_u16)ipcom_atoi(argv[++*next_arg]);
            if (cmd->param.vlan.vlr_tag < 1 || cmd->param.vlan.vlr_tag > 4094)
            {
                IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("vlan ID must be 1-4094" IP_LF));
                return -IP_ERRNO_EINVAL;
            }

            if (cmd->param.vlan.vlr_parent[0] == '\0')
            {
                if (ipcom_strcmp(argv[++*next_arg], "vlanif") == 0)
                    ipcom_strncpy(cmd->param.vlan.vlr_parent, argv[++*next_arg], IP_IFNAMSIZ);
                else
                    goto vlan_conf_parse_error;
            }
        }
        ++*next_arg;
        break;

    vlan_conf_parse_error:
        IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("vlan <vid> and vlanif <ethif> must be set at the same time" IP_LF));
        return -IP_ERRNO_EINVAL;

    case IPNET_IFCONFIG_CMD_VLANPRI_CONF:
        cmd->param.vlan.vlr_pri = (Ip_u16)ipcom_atoi(argv[*next_arg]);
        if (cmd->param.vlan.vlr_pri > 7)
        {
            IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("vlan priority must be 0-7" IP_LF));
            return -IP_ERRNO_EINVAL;
        }
        ++*next_arg;
        break;

#endif /* IPNET_USE_VLAN */

#ifdef IPNET_USE_TUNNEL
    case IPNET_IFCONFIG_CMD_TUNNEL:
        {
            struct Ip_ip_tunnel_param *tp = &cmd->param.tunnel;
            if (argc - *next_arg < 2)
            {
                IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("<local> and <remote> must be set at the same time" IP_LF));
                return -IP_ERRNO_EINVAL;
            }
            tp->local_addr.sa.sa_family  = (Ip_sa_family_t)cmd->domain;
            IPCOM_SA_LEN_SET(&tp->local_addr.sa, sizeof(tp->local_addr));
            tp->remote_addr.sa.sa_family = (Ip_sa_family_t)cmd->domain;
            IPCOM_SA_LEN_SET(&tp->remote_addr.sa, sizeof(tp->remote_addr));
#ifdef IPCOM_USE_INET
            if (cmd->domain == IP_AF_INET)
            {
                if (ipcom_inet_pton(cmd->domain, argv[*next_arg], &tp->local_addr.sin.sin_addr) != 1
                    || ipcom_inet_pton(cmd->domain, argv[++*next_arg], &tp->remote_addr.sin.sin_addr) != 1)
                {
                    IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("%s is not a valid IPv4 address"IP_LF, argv[*next_arg]));
                    return -IP_ERRNO_EINVAL;
                }
            }
            else
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
                if (cmd->domain == IP_AF_INET6)
                {
                    if (ipcom_inet_pton(cmd->domain, argv[*next_arg], &tp->local_addr.sin6.sin6_addr) != 1
                        || ipcom_inet_pton(cmd->domain, argv[++*next_arg], &tp->remote_addr.sin6.sin6_addr) != 1)
                    {
                        IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("%s is not a valid IPv6 address"IP_LF, argv[*next_arg]));
                        return -IP_ERRNO_EINVAL;
                    }
                }
                else
#endif /* IPCOM_USE_INET6 */
                {
                    IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("No domain specified"IP_LF));
                    return -IP_ERRNO_EINVAL;
                }
            ++*next_arg;
            if (argc > *next_arg && ipcom_isdigit(argv[*next_arg][0]))
            {
                cmd->param.tunnel.ttl = (Ip_u8)ipcom_atoi(argv[*next_arg]);
                ++*next_arg;
            }
            if (argc > *next_arg && ipcom_strcmp(argv[*next_arg], "csum") == 0)
            {
                IP_BIT_SET(tp->o_flags, IPCOM_TUNNEL_FLAG_GRE_CHECKSUM);
                ++*next_arg;
            }
        }
        break;
#endif /* IPNET_USE_TUNNEL */

    default:
        break;
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_cmd_qc_format_number
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC const char *
ipnet_ifconfig_format_number(char *str_num, int str_len, Ip_u32 num)
{
    if (num / 10000000 > 0)
        ipcom_snprintf(str_num, str_len, "%luM", num / 1000000);
    else if (num / 10000 > 0)
        ipcom_snprintf(str_num, str_len, "%luk", num / 1000);
    else
        ipcom_snprintf(str_num, str_len, "%lu", num);
    return str_num;
}


/*
 *===========================================================================
 *                    ipnet_ifconfig_print_info
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_ifconfig_print_info(Ip_fd fd, Ipnet_cmd_ifconfig *cmd, Ip_bool full_output)
{
    int                     name[6];
    Ip_u8                  *if_data;
    Ip_size_t               if_data_len;
    Ip_size_t               if_data_offset;
    struct Ipnet_if_msghdr *ifm;
    struct Ip_sockaddr     *hwaddr;
    char                    ifname[IP_IFNAMSIZ];
    char                    str[80];
    struct Ipnet_ifqueue    ifq;
    struct Ip_ifreq         ifreq;
    Ip_u32                  ifindex = cmd->ifindex;
    int                     af = cmd->domain;

    ipcom_memset(&ifq, 0, sizeof(&ifq));

    name[0] = IP_CTL_NET;
    name[1] = IP_AF_ROUTE;
    name[2] = 0;    /* Always 0 */
    name[3] = af;
    name[4] = IP_NET_RT_IFLIST;
    name[5] = (int)ifindex;

restart_ifconfig:
    if (ipcom_sysctl(name, 6, IP_NULL, &if_data_len, IP_NULL, 0) < 0)
    {
        ipcom_printf("ifconfig: ipnet_sysctl() failed: %s"IP_LF, ipcom_strerror(ipcom_errno));
        return;
    }

    if_data = ipcom_malloc(if_data_len);
    if (if_data == IP_NULL)
    {
        ipcom_printf("ifconfig: out of memory"IP_LF);
        return;
    }

    if (ipcom_sysctl(name, 6, if_data, &if_data_len, IP_NULL, 0) < 0)
    {
        if (ipcom_errno == IP_ERRNO_ENOMEM)
        {
            ipcom_free(if_data);
            goto restart_ifconfig;
        }

        ipcom_printf("ifconfig: ipnet_sysctl() failed: %s"IP_LF, ipcom_strerror(ipcom_errno));
        goto cleanup;
    }

    if_data_offset = 0;
    while (if_data_len > if_data_offset + sizeof(struct Ipnet_if_msghdr))
    {
        ifm = (struct Ipnet_if_msghdr *) &if_data[if_data_offset];
        hwaddr = (struct Ip_sockaddr *) &if_data[if_data_offset + sizeof(struct Ipnet_if_msghdr)];
        if_data_offset += ifm->ifm_msglen;

        /* Resolve the interface name */
        if (ipcom_if_indextoname(ifm->ifm_index, ifname) == IP_NULL)
            continue;

        if (full_output)
        {
            ipcom_printf("%s\tLink type:%s",
                         ifname,
                         ipnet_cmd_if_type_to_str(ifm->ifm_data.ifi_type, str, sizeof(str)));
            if (((struct Ip_sockaddr_dl *) hwaddr)->sdl_alen != 0)
            {
                ipcom_printf("  HWaddr %s",
                             ipcom_cmd_sockaddr_to_str(hwaddr, IP_FALSE, IP_TRUE, str, sizeof(str)));
            }

            ipcom_strncpy(ifq.ifq_name,
                          ifname,
                          IP_IFNAMSIZ);
            ifq.ifq_id = IP_IFQ_ID_NONE;
            if (ipcom_socketioctl(fd, IPNET_SIOCGIFQUEUE, &ifq) >= 0)
                ipcom_printf("  Queue:%s", ifq.ifq_type);

#ifdef IPCOM_USE_HW_CAPS
            if (ifm->ifm_data.ifi_type == IP_IFT_ETHER)
            {
                ipcom_memset(&ifreq, 0, sizeof(struct Ip_ifreq));
                ipcom_strncpy(ifreq.ifr_name,
                              ifname,
                              IP_IFNAMSIZ);
                if (ipcom_socketioctl(fd, IP_SIOCGIFCAP, &ifreq) >= 0 &&
                    ifreq.ip_ifr_curcap > 0)
                    ipcom_printf(IP_LF"\tcapabilities: %s",
                                ipcom_cmd_if_caps_to_str(ifreq.ip_ifr_curcap,
                                str, sizeof(str)));
            }
#endif /* IPCOM_USE_HW_CAPS */

#ifdef IPNET_USE_TUNNEL
            if (ifm->ifm_data.ifi_type == IP_IFT_TUNNEL)
                ipnet_ifconfig_print_tunnel(fd, ifm->ifm_index);
#endif

#ifdef IPNET_USE_VLAN
            if (ifm->ifm_data.ifi_type == IP_IFT_L2VLAN)
                ipnet_ifconfig_print_vlan(fd, ifm->ifm_index);
#endif

#ifdef IPPPP_USE_PPPMP
            if ((ifm->ifm_data.ifi_type == IP_IFT_PPP) ||
                (ifm->ifm_data.ifi_type == IP_IFT_PPPOE) ||
                (ifm->ifm_data.ifi_type == IP_IFT_PPPL2TP))
                ipnet_ifconfig_print_ppp_multilink(fd, ifm->ifm_index);
#endif /* IPPPP_USE_PPPMP */

            ipcom_printf(IP_LF);
        }

#ifdef IPCOM_USE_INET
        if_data_offset += ipnet_ifconfig_print_ipv4(&if_data[if_data_offset],
                                                    if_data_len - if_data_offset,
                                                    IP_BIT_ISSET(ifm->ifm_flags, IP_IFF_POINTOPOINT),
                                                    cmd->ipv6_only,
                                                    cmd->verbose);

#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
        if_data_offset += ipnet_ifconfig_print_ipv6(fd,
                                                    &if_data[if_data_offset],
                                                    if_data_len - if_data_offset,
                                                    cmd->ipv4_only,
                                                    cmd->verbose);
#endif /* IPCOM_USE_INET6 */

        if (full_output)
        {
            ipcom_memset(&ifreq, 0, sizeof(struct Ip_ifreq));
            ipcom_strcpy(ifreq.ifr_name, ifname);
            if (ipcom_socketioctl(fd, IP_SIOCGIFVR, &ifreq) < 0)
                /* Default to VR 0 if the kernel is lacking VR support */
                ifreq.ip_ifr_vr = 0;
#ifdef IP_PORT_LAS
            {
                Ip_u32 flags = ifm->ifm_flags;
                Ip_u32 lflags = 0;

                if (ipcom_socketioctl(fd, IP_X_SIOCGIFLINKFLAGS, &ifreq) == 0)
                    lflags = ifreq.ip_ifr_flags;

                flags |= lflags << 16;
                ipcom_printf("\t%s"IP_LF, ipcom_cmd_if_flags_to_str((int)flags, str, sizeof(str)));
            }
#else
            ipcom_printf("\t%s"IP_LF, ipcom_cmd_if_flags_to_str(ifm->ifm_flags, str, sizeof(str)));
#endif
            ipcom_printf("\tMTU:%ld  metric:%ld  VR:%u  ifindex:%u",
                         ifm->ifm_data.ifi_mtu ,
                         ifm->ifm_data.ifi_metric,
                         ifreq.ip_ifr_vr,
                         ifm->ifm_index);
            ipcom_printf(IP_LF"\tRX packets:%lu mcast:%lu errors:%lu dropped:%ld" IP_LF,
                         ifm->ifm_data.ifi_ipackets, ifm->ifm_data.ifi_imcasts,
                         ifm->ifm_data.ifi_ierrors, ifm->ifm_data.ifi_iqdrops);
            ipcom_printf("\tTX packets:%lu mcast:%lu errors:%lu" IP_LF,
                         ifm->ifm_data.ifi_opackets, ifm->ifm_data.ifi_omcasts,
                         ifm->ifm_data.ifi_oerrors);
            ipcom_printf("\tcollisions:%lu unsupported proto:%lu" IP_LF,
                         ifm->ifm_data.ifi_collisions, ifm->ifm_data.ifi_noproto);
            ipcom_printf("\tRX bytes:%s",
                         ipnet_ifconfig_format_number(str, sizeof(str), ifm->ifm_data.ifi_ibytes));
            ipcom_printf("  TX bytes:%s",
                         ipnet_ifconfig_format_number(str, sizeof(str), ifm->ifm_data.ifi_obytes));

#ifdef IPCOM_USE_FORWARDER

            {
                int ret;
                struct Ip_fwreq fwreq;

                ipcom_memset(&fwreq, 0, sizeof(struct Ip_fwreq));
                ipcom_strcpy(fwreq.eth_name, ifname);

                ret = ipcom_socketioctl(fd, IP_SIOCGHWSTATS, (struct Ip_ifreq *)&fwreq);

                if (ret ==  0)
                {
                    ipcom_printf(IP_LF"\tHW Forwarder");

                    ipcom_printf(IP_LF"\tTx Packets: %lu, Bytes: %llu",
                                 fwreq.u.ps.tx_packets,
                                 ((Ip_u64)fwreq.u.ps.tx_octets_hw << 32) + (Ip_u64)fwreq.u.ps.tx_octets_lw);


                    ipcom_printf(IP_LF"\tRx Packets: %lu, Bytes: %llu",
                                 fwreq.u.ps.packets,
                                 ((Ip_u64)fwreq.u.ps.octets_hw << 32) + (Ip_u64)fwreq.u.ps.octets_lw);

                    ipcom_printf(IP_LF"\t   Frames size:  64: %lu, 128: %lu, 256: %lu, 512: %lu, 1024: %lu, 1519: %lu",
                                 fwreq.u.ps.len_64_packets,
                                 fwreq.u.ps.len_65_127_packets,
                                 fwreq.u.ps.len_128_255_packets,
                                 fwreq.u.ps.len_256_511_packets,
                                 fwreq.u.ps.len_512_1023_packets,
                                 fwreq.u.ps.len_1024_1518_packets,
                                 fwreq.u.ps.len_1519_max_packets);

                    ipcom_printf(IP_LF"\t   Broadcast: %lu, Multicast: %lu, Dropped: %lu (%lu)",
                                 fwreq.u.ps.broadcast_packets,
                                 fwreq.u.ps.multicast_packets,
                                 fwreq.u.ps.dropped_packets,
                                 fwreq.u.ps.dropped_octets);

                    ipcom_printf(IP_LF);
                }
                else
                {
                    ipcom_printf("ifconfig %s: failed (ret = %d(%x))\n", ifname, ret, ret);
                }
            }
#endif
            ipcom_printf(IP_LF IP_LF);
        }
    }

cleanup:
    ipcom_free(if_data);
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipstack_ifconfig_print_info
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipstack_ifconfig_print_info(Ip_fd fd, Ip_u32 ifindex, int af, Ip_bool full_output)
{
    Ipnet_cmd_ifconfig cmd;

    ipcom_memset(&cmd, 0, sizeof(cmd));
    cmd.ifindex = ifindex;
    cmd.domain  = af;

    ipnet_ifconfig_print_info(fd, &cmd, full_output);
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_cmd_ifconfig
 *===========================================================================
 * Description: ifconfig shell command entry point.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipnet_cmd_ifconfig(int argc, char **argv)
{
    int                next_arg;
    Ip_fd              fd;
    Ipnet_cmd_ifconfig cmd;
    int                err = 0;
    IP_CONST char     *ifname = IP_NULL;
    int                silent = IP_FALSE;
#if IPCOM_VR_MAX > 1
    int  vr      = ipcom_proc_vr_get();
    int  vr_new  = vr;
#endif
    next_arg = 1;

    /* Silent must only be used by the configuration daemon */
    if (argc > next_arg && ipcom_strcmp(argv[next_arg], "-silent") == 0)
    {
        silent = IP_TRUE;
        next_arg++;
    }

#if IPCOM_VR_MAX > 1
    if (argc > next_arg && ipcom_strcmp(argv[next_arg], "-V") == 0)
    {
        next_arg++;
        vr_new = ipcom_atoi(argv[next_arg]);
        next_arg++;
    }

    if (vr != vr_new)
        ipcom_proc_vr_set(vr_new);
#endif

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
        case IPNET_IFCONFIG_CMD_HELP:
            goto print_usage;
        case IPNET_IFCONFIG_CMD_SHOW_INFO:
            ipnet_ifconfig_print_info(fd, &cmd, IP_TRUE);
            goto cleanup;
        case IPNET_IFCONFIG_CMD_VERBOSE:
            cmd.verbose = IP_TRUE;
            next_arg++;
            break;
        case IPNET_IFCONFIG_CMD_IPV4_ONLY:
            cmd.ipv4_only = IP_TRUE;
            next_arg++;
            break;
        case IPNET_IFCONFIG_CMD_IPV6_ONLY:
            cmd.ipv6_only = IP_TRUE;
            next_arg++;
            break;
        default:
            if (argv[next_arg][0] == '-' && cmd.cmd == -1)
                IPNET_IFCONFIG_CHECK_SILENT(ipcom_printf("'%s' is a unknown switch"IP_LF, argv[next_arg]));
            break;
        }
    }

    if (argc == next_arg)
        goto print_usage;

    /* Process all commands */
    while (next_arg < argc && err == 0)
    {
        ipcom_memset(&cmd.param, 0, sizeof(cmd.param));
        if (ifname == IP_NULL)
            ifname = argv[next_arg++];

        if (ipnet_cmd_ifconfig_parse_arg(argc, &next_arg, argv, &cmd, silent) < 0)
            goto errout;

        if (cmd.cmd == IPNET_IFCONFIG_CMD_CREATE)
            ipcom_strncpy(cmd.param.create.ifr_name, ifname, IP_IFNAMSIZ);
        else
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
        case IPNET_IFCONFIG_CMD_SHOW_INFO:
            ipnet_ifconfig_print_info(fd, &cmd, IP_TRUE);
            break;
        case IPNET_IFCONFIG_CMD_UP:
            err = ipnet_ifconfig_if_change_state(fd, cmd.ifindex, IP_TRUE, silent);
            break;
        case IPNET_IFCONFIG_CMD_DOWN:
            err = ipnet_ifconfig_if_change_state(fd, cmd.ifindex, IP_FALSE, silent);
            break;
#ifndef IP_PORT_LAS
        case IPNET_IFCONFIG_CMD_ENABLE_DHCP:
            err = ipnet_ifconfig_if_change_dhcp(fd, cmd.ifindex, IP_TRUE);
            break;
        case IPNET_IFCONFIG_CMD_DISABLE_DHCP:
            err = ipnet_ifconfig_if_change_dhcp(fd, cmd.ifindex, IP_FALSE);
            break;
#endif
        case IPNET_IFCONFIG_CMD_DETACH :
            err = ipnet_ifconfig_if_detach(fd, cmd.ifindex);
            break;
        case IPNET_IFCONFIG_CMD_REPLACE_ADDR:
        case IPNET_IFCONFIG_CMD_ADD_ADDR:
#ifdef IPCOM_USE_INET
            if (cmd.domain == IP_AF_INET)
            {
                if (cmd.cmd == IPNET_IFCONFIG_CMD_REPLACE_ADDR)
                    err = ipnet_ifconfig_ipv4_replace(fd,
                                                      cmd.ifindex,
                                                      cmd.param.inet.addr_is_set ? &cmd.param.inet.addr : IP_NULL,
                                                      cmd.param.inet.mask_is_set ? &cmd.param.inet.mask : IP_NULL,
                                                      silent);
                else
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
        case IPNET_IFCONFIG_CMD_DELETE_ADDR :
#ifdef IPCOM_USE_INET
            if (cmd.domain == IP_AF_INET)
                err = ipnet_ifconfig_ipv4_add_delete(fd,
                                                     IP_FALSE,
                                                     cmd.ifindex,
                                                     &cmd.param.inet.addr,
                                                     IP_NULL,
                                                     silent);
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
            if (cmd.domain == IP_AF_INET6)
                err = ipnet_ifconfig_ipv6_delete(fd, cmd.ifindex, &cmd.param.inet6.addr);
#endif /* IPCOM_USE_INET6 */
            break;
        case IPNET_IFCONFIG_CMD_SET_DSTADDR :
            err = ipnet_ifconfig_set_dstaddr(fd, &cmd);
            break;
        case IPNET_IFCONFIG_CMD_SET_VR :
            err = ipnet_ifconfig_set_vr(fd, cmd.ifindex, cmd.param.vr, silent);
            break;
        case IPNET_IFCONFIG_CMD_SET_MTU:
            err = ipnet_ifconfig_set_mtu(fd, cmd.ifindex, cmd.param.mtu);
            break;
        case IPNET_IFCONFIG_CMD_SET_LLADDR:
            err = ipnet_ifconfig_set_lladdr(fd, &cmd);
            break;
        case IPNET_IFCONFIG_CMD_CREATE :
            err = ipnet_ifconfig_create(fd, &cmd);
            break;
        case IPNET_IFCONFIG_CMD_DESTROY :
            err = ipnet_ifconfig_destroy(fd, &cmd);
            break;
#ifdef IPNET_USE_VLAN
        case IPNET_IFCONFIG_CMD_VLAN_CONF :
            err = ipnet_ifconfig_vlan_conf(fd, &cmd);
            break;
        case IPNET_IFCONFIG_CMD_VLANPRI_CONF :
            err = ipnet_ifconfig_vlanpri_conf(fd, &cmd);
            break;

#endif /* IPNET_USE_VLAN */
#ifdef IPNET_USE_TUNNEL
        case IPNET_IFCONFIG_CMD_TUNNEL :
            err = ipnet_ifconfig_tunnel_conf(fd, &cmd);
            break;
#endif /* IPNET_USE_TUNNEL */
        case IPNET_IFCONFIG_CMD_ENABLE_LINK0:
            err = ipnet_ifconfig_if_change_link(fd, cmd.ifindex, IP_TRUE, 0);
            break;
        case IPNET_IFCONFIG_CMD_DISABLE_LINK0:
            err = ipnet_ifconfig_if_change_link(fd, cmd.ifindex, IP_FALSE, 0);
            break;
        case IPNET_IFCONFIG_CMD_ENABLE_LINK1:
            err = ipnet_ifconfig_if_change_link(fd, cmd.ifindex, IP_TRUE, 1);
            break;
        case IPNET_IFCONFIG_CMD_DISABLE_LINK1:
            err = ipnet_ifconfig_if_change_link(fd, cmd.ifindex, IP_FALSE, 1);
            break;
        case IPNET_IFCONFIG_CMD_ENABLE_LINK2:
            err = ipnet_ifconfig_if_change_link(fd, cmd.ifindex, IP_TRUE, 2);
            break;
        case IPNET_IFCONFIG_CMD_DISABLE_LINK2:
            err = ipnet_ifconfig_if_change_link(fd, cmd.ifindex, IP_FALSE, 2);
            break;
    case IPNET_IFCONFIG_CMD_ENABLE_PROMISC:
            err = ipnet_ifconfig_if_promisc_link(fd, cmd.ifindex, IP_TRUE);
            break;
    case IPNET_IFCONFIG_CMD_DISABLE_PROMISC:
            err = ipnet_ifconfig_if_promisc_link(fd, cmd.ifindex, IP_FALSE);
            break;
        default:
            IP_PANIC();
            break;
        }
    }

cleanup:
    if (fd != IP_INVALID_SOCKET)
        (void)ipcom_socketclose(fd);
#if IPCOM_VR_MAX > 1
    if (vr != vr_new)
        ipcom_proc_vr_set(vr);
#endif
    return err;

print_usage:
    if (!silent)
        ipnet_cmd_ifconfig_print_usage();
errout:
    err = -1;
    goto cleanup;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
