/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_configd.c,v $ $Revision: 1.70 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_configd.c,v $
 *   $Author: rime $
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
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_netif.h>
#include <ipcom_sysctl.h>
#include <ipcom_syslog.h>
#include <ipcom_type.h>

#include "ipnet.h"
#include "ipnet_cmd.h"
#include "ipnet_h.h"


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

union Ipnet_addr
{
#ifdef IPCOM_USE_INET
    struct Ip_in_addr   inet;
#endif
#ifdef IPCOM_USE_INET6
    struct Ip_in6_addr  inet6;
#endif
};

#if defined(WRS_IPNET)
typedef int (*ipcom_cmd_func_type)(int argc, char ** argv);
#endif

/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */
#if defined(WRS_IPNET)
IP_EXTERN ipcom_cmd_func_type ipnet_cmd_route_hook;
IP_EXTERN ipcom_cmd_func_type ipnet_cmd_ifconfig_hook;
IP_EXTERN ipcom_cmd_func_type ipnet_cmd_qc_hook;
IP_EXTERN int (*ipnet_qc_unif_config_hook) (const char * ifname);
#endif

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
 *===========================================================================
 *                    ipnet_config_cmd_ifconfig
 *===========================================================================
 * Description: Provides runtime binding for the ipnet_cmd_ifconfig() function.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_config_cmd_ifconfig(int argc, char **argv)
{
#if defined(WRS_IPNET)
    /*
     * ipnet_cmd_ifconfig_hook should never be NULL.
     * It should always be dfined one way are another; checking just in case.
     */
    if (ipnet_cmd_ifconfig_hook)
        return ipnet_cmd_ifconfig_hook(argc, argv);
    else
        return -IP_ERRNO_EINVAL;
#else
    return ipnet_cmd_ifconfig(argc, argv);
#endif
}


/*
 *===========================================================================
 *                    ipnet_config_cmd_qc
 *===========================================================================
 * Description: Provides runtime binding for the ipnet_cmd_qc() function.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_config_cmd_qc(int argc, char **argv)
{
#if defined(WRS_IPNET)
    /*
     * ipnet_cmd_qc_hook() will be NULL unless the standard qc command is
     * included in the image.
     */
    if (ipnet_cmd_qc_hook)
        return ipnet_cmd_qc_hook(argc, argv);
    else
        return -IP_ERRNO_EINVAL;
#else
    return ipnet_cmd_qc(argc, argv);
#endif
}

/*
 *===========================================================================
 *                    ipnet_config_cmd_route
 *===========================================================================
 * Description: Provides runtime binding for the ipnet_cmd_route() function.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_config_cmd_route(int argc, char **argv)
{
#if defined(WRS_IPNET)
    /*
     * ipnet_cmd_route_hook should never be NULL.
     * It should always be dfined one way are another; checking just in case.
     */
    if (ipnet_cmd_route_hook)
        return ipnet_cmd_route_hook(argc, argv);
    else
        return -IP_ERRNO_EINVAL;
#else
    return ipnet_cmd_route(argc, argv);
#endif
}


/*
 *===========================================================================
 *                    ipnet_config_add_route
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_config_add_route(int domain, char *addr, int prefix_len, char *gateway)
{
    char *argv[] = {
        "route",
        "add",
        "-silent",
        IP_NULL,
        IP_NULL,
        IP_NULL,
        IP_NULL,
        IP_NULL,
        IP_NULL,
        IP_NULL,
        IP_NULL,
        IP_NULL,
        IP_NULL,
    };
    int argc = 4;
    int max_prefix_len;
    union Ipnet_addr mask;
    union Ipnet_addr dst_addr;
    char dst_str[IP_INET6_ADDRSTRLEN];
    char prefix_len_str[4];
    int ret;

#ifdef IPCOM_USE_INET
    if (domain == IP_AF_INET)
    {
        max_prefix_len = 32;
        argv[3] = "-inet";
    }
    else
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    if (domain == IP_AF_INET6)
    {
        max_prefix_len = 128;
        argv[3] = "-inet6";
    }
    else
#endif /* IPCOM_USE_INET6 */
    {
        /* Unsupported domain */
        IPCOM_LOG1(ERR, "unsupported domain: %d", domain);
        return -IP_ERRNO_EAFNOSUPPORT;
    }

    if (prefix_len == -1)
        /* Host route */
        prefix_len = max_prefix_len;

    if (addr == IP_NULL || gateway == IP_NULL || prefix_len < 0 || prefix_len > max_prefix_len)
        return -IP_ERRNO_EINVAL;

    if (ipcom_inet_pton(domain, addr, &dst_addr) != 1)
    {
        IPCOM_LOG2(ERR, "invalid %s address format: %s",
            (domain == IP_AF_INET) ? "inet" : "inet6", addr);
        return -IP_ERRNO_EINVAL;
    }

    /* Apply mask */
    ipcom_memset(&mask, 0, sizeof(mask));
    ipnet_route_create_mask(&mask, prefix_len);
    ipnet_route_apply_mask(&dst_addr, &mask, max_prefix_len);
    (void) ipcom_inet_ntop(domain, &dst_addr, dst_str, sizeof(dst_str));

    argv[argc++] = "-static";

    if (prefix_len == max_prefix_len)
        argv[argc++] = "-host";
    else
    {
        argv[argc++] = "-net";
        argv[argc++] = "-prefixlen";
        ipcom_sprintf(prefix_len_str, "%d", prefix_len);
        argv[argc++] = prefix_len_str;
    }

    argv[argc++] = dst_str;
    argv[argc++] = gateway;

    ret = ipnet_config_cmd_route(argc, argv);
    if (ret == IP_ERRNO_EEXIST)
        return 0;
    return ret;
}


#ifdef IPCOM_USE_INET
/*
 *===========================================================================
 *                    ipnet_config_add_inet_addr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_config_add_inet_addr(Ip_fd fd, char *ifname, char *option)
{
    char *inet_addr;
    char *prefix_len;
    char *argv[] = {
        "ifconfig",
        "-silent",
        IP_NULL,
        "inet",
        "add",
        IP_NULL,
        IP_NULL,
        IP_NULL,
        IP_NULL
    };
    int argc = 5;
    char inet_addr_str[IP_INET_ADDRSTRLEN];
    char inet_prefix_len_str[8];

    argv[2] = ifname;

    inet_addr = ipcom_strtok_r(option, " \t/", &option);
    if (inet_addr == IP_NULL)
    {
        IPCOM_LOG0(ERR, "inet address is missing or format is invalid");
        return -IP_ERRNO_EINVAL;
    }

    if (ipcom_strcmp(inet_addr, "dhcp") == 0)
    {
        struct Ip_ifreq  ifreq;

    use_dhcp:
        ipcom_strcpy(ifreq.ifr_name, ifname);
        ifreq.ifr_ifru.ifru_opt = 1;

		if (ipcom_socketioctl(fd, IP_SIOCXSDHCPRUNNING, &ifreq) < 0)
        {
            IPCOM_LOG1(ERR, "Failed to enable DHCP on %s", ifname);
            return ipcom_errno;
        }
        return 0;
    }

#ifdef IPNET_USE_RARP
    if (ipcom_strcmp(inet_addr, "rarp") == 0)
    {
        struct Ip_ethreq  ethreq;

        ipcom_strcpy(ethreq.ethr_name, ifname);
        ethreq.ethru.rarp = -1;

		if (ipcom_socketioctl(fd, IP_SIOCXETHSRARP, &ethreq) < 0)
        {
            IPCOM_LOG1(ERR, "Failed to enable RARPP on %s", ifname);
            return ipcom_errno;
        }
        return 0;
    }
#endif /* IPNET_USE_RARP */

    if (ipcom_strcmp(inet_addr, "driver") == 0)
    {
        /* Get the IPv4 address to use from the driver */
        struct Ip_ethreq ethreq;

        ipcom_strcpy(ethreq.ethr_name, ifname);
        if (ipcom_socketioctl(fd, IP_SIOCXETHGINET, &ethreq) < 0)
        {
            IPCOM_LOG1(ERR, "Failed to read the IPv4 address from the driver for %s", ifname);
            return ipcom_errno;
        }
        if (ethreq.ethru.inet.addr.s_addr == 0xffffffff)
            goto use_dhcp;
        inet_addr = inet_addr_str;
        (void) ipcom_inet_ntop(IP_AF_INET, &ethreq.ethru.inet.addr,
            inet_addr_str, sizeof(inet_addr_str));

        prefix_len = inet_prefix_len_str;
        ipcom_snprintf(inet_prefix_len_str, sizeof(inet_prefix_len_str), "%d",
            ipcom_mask_to_prefixlen(&ethreq.ethru.inet.mask, 32));
    }
    else
    {
        prefix_len = ipcom_strtok_r(option, " \t/", &option);
        if (prefix_len == IP_NULL)
        {
            IPCOM_LOG0(ERR, "prefix len is missing or format is invalid");
            return -IP_ERRNO_EINVAL;
        }
    }

    argv[argc++] = inet_addr;
    argv[argc++] = "prefixlen";
    argv[argc++] = prefix_len;

    return ipnet_config_cmd_ifconfig(argc, argv);
}


/*
 *===========================================================================
 *                    ipnet_config_add_gateway
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_config_add_gateway(Ip_fd fd, char *ifname, char *option)
{
    char gw_str[IP_INET_ADDRSTRLEN];
    char *gw;

    gw = ipcom_strtok_r(option, " \t", &option);
    if (gw == IP_NULL)
    {
        IPCOM_LOG1(ERR,
                   "Wrong gateway format specified for interface %s, must be 'gateway <driver|gw_address>",
                   ifname);
        IP_PANIC();
        return -IP_ERRNO_EINVAL;
    }

    if (ipcom_strcmp(gw, "driver") == 0)
    {
        /* Get the IPv4 gateway address to use from the driver */
        struct Ip_ethreq ethreq;

        ipcom_strcpy(ethreq.ethr_name, ifname);
        if (ipcom_socketioctl(fd, IP_SIOCXETHGINET, &ethreq) < 0)
        {
            IPCOM_LOG1(ERR, "Failed to read the IPv4 gateway address "
                       "from the driver for %s", ifname);
            return ipcom_errno;
        }
        if (ethreq.ethru.inet.gateway.s_addr == 0xffffffff)
            return 0; /* using dhcp, ignore gateway */
        if (ethreq.ethru.inet.gateway.s_addr == 0)
        {
            IPCOM_LOG1(NOTICE, "No IPv4 gateway address set in driver for %s", ifname);
            return 0;
        }

        (void) ipcom_inet_ntop(IP_AF_INET, &ethreq.ethru.inet.gateway,
                               gw_str, sizeof(gw_str));
        gw = gw_str;
    }

    return ipnet_config_add_route(IP_AF_INET, "0.0.0.0", 0, gw);
}
#endif /* IPCOM_USE_INET */


#ifdef IPCOM_USE_INET6
/*
 *===========================================================================
 *                    ipnet_config_add_inet6_addr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_config_add_inet6_addr(Ip_fd fd, char *ifname, char *option)
{
    char *opt;
    char *inet6_addr;
    char *prefix_len;
    char *argv[] = {
        "ifconfig",
        "-silent",
        IP_NULL,
        "inet6",
        "add",
        IP_NULL,
        IP_NULL,
        IP_NULL,
        IP_NULL,
        IP_NULL,
        IP_NULL
    };
    int argc = 6;
    char inet6_addr_str[IP_INET6_ADDRSTRLEN];
    char prefixlen_str[4];

    argv[2] = ifname;

    for (;;)
    {
        /* Parse options */
        opt = ipcom_strtok_r(option, " \t/", &option);
        if (opt == IP_NULL)
        {
            IPCOM_LOG0(ERR, "too few arguments");
            return -IP_ERRNO_EINVAL;
        }
        if (ipcom_strcmp(opt, "tentative") == 0)
            argv[argc++] = opt;
        else
            break;
    }

    inet6_addr = opt;
    if (ipcom_strcmp(inet6_addr, "driver") == 0)
    {
        /* Get the IPv6 address to use from the driver */
        struct Ip_ethreq ethreq;

        ipcom_strcpy(ethreq.ethr_name, ifname);
        if (ipcom_socketioctl(fd, IP_SIOCXETHGINET6, &ethreq) < 0)
        {
            IPCOM_LOG1(ERR, "Failed to read the IPv6 address from the driver for %s", ifname);
            return ipcom_errno;
        }
        inet6_addr = inet6_addr_str;
        (void) ipcom_inet_ntop(IP_AF_INET6, &ethreq.ethru.inet6.addr,
                               inet6_addr_str, sizeof(inet6_addr_str));

        ipcom_sprintf(prefixlen_str, "%u", ethreq.ethru.inet6.prefixlen);
        prefix_len = prefixlen_str;
    }
    else
    {
        prefix_len = ipcom_strtok_r(option, " \t/", &option);
        if (prefix_len == IP_NULL)
        {
            IPCOM_LOG0(ERR, "prefix len is missing or format is invalid");
            return -IP_ERRNO_EINVAL;
        }
    }

    argv[5] = inet6_addr;
    argv[argc++] = "prefixlen";
    argv[argc++] = prefix_len;

    return ipnet_config_cmd_ifconfig(argc, argv);
}


/*
 *===========================================================================
 *                    ipnet_config_add_gateway6
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_config_add_gateway6(char *ifname, char *option)
{
    char *gw;

    gw = ipcom_strtok_r(option, " \t", &option);
    if (gw == IP_NULL || ipcom_strcmp(gw, "::") == 0)
        return ipnet_config_add_route(IP_AF_INET6, "::", 0, (char *) ifname);
    return ipnet_config_add_route(IP_AF_INET6, "::", 0, gw);
}
#endif /* IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                    ipnet_config_bring_up_if
 *===========================================================================
 * Description: Brings up the specified interface.
 * Parameters:  ifname - The interface to bring up.
 * Returns:
 *
 */
IP_STATIC int
ipnet_config_bring_up_if(char *ifname)
{
    char *argv[] = {
        "ifconfig",
        "-silent",
        IP_NULL,
        "up",
        IP_NULL
    };

    argv[2] = ifname;
    return ipnet_config_cmd_ifconfig(4, argv);
}


/*
 *===========================================================================
 *                    ipnet_config_interface
 *===========================================================================
 * Description: Configures one interface if it exists and is UP.
 * Parameters:  fd - A socket descriptor
 *              ifname - The name of the interface to configure
 *              conf - The configuration to apply to the interface
 *              conf_len - Number of valid entries in the "conf" array
 *              pass - First pass (1) will add all addresses
 *                     second pass (2) will add other things like gateways
 * Returns:     0 = success
 *             <0 = error code
 *
 */
IP_STATIC int
ipnet_config_interface(Ip_fd fd, char *ifname, char *conf[], int conf_len, int pass)
{
    char *option;
    char *option_org;
    char *token;
    int   i;
    int   err;

    ip_assert(pass == 1 || pass == 2);

    if (ipcom_if_nametoindex(ifname) == 0)
    {
        /* Interface does not exist */
        IPCOM_LOG1(INFO, "%s not attached - skipping", ifname);
        return 0;
    }

    if (0 > (err = ipnet_config_bring_up_if(ifname)))
    {
        IPCOM_LOG1(INFO, "Failed to bring UP %s", ifname);
        return 0;
    }

    for (i = 0; i < conf_len; i++)
    {
        option_org = option = ipcom_strdup(conf[i]);
        if (option == IP_NULL)
            return -IP_ERRNO_ENOMEM;

        token = ipcom_strtok_r(option, " \t/", &option);

        if (token == IP_NULL || ipcom_strcmp(token, "ifname") == 0)
        {
            /* Ignore */
            err = 0;
        }
#ifdef IPCOM_USE_INET
        else if (ipcom_strcmp(token, "inet") == 0)
        {
            if (pass == 1)
                err = ipnet_config_add_inet_addr(fd, ifname, option);
        }
        else if (ipcom_strcmp(token, "gateway") == 0)
        {
            if (pass == 2)
                err = ipnet_config_add_gateway(fd, ifname, option);
        }
#else
        else if (ipcom_strcmp(token, "inet") == 0
                 || ipcom_strcmp(token, "gateway") == 0)
            IP_NOOP;
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
        else if (ipcom_strcmp(token, "inet6") == 0)
        {
            if (pass == 1)
                err = ipnet_config_add_inet6_addr(fd, ifname, option);
        }
        else if (ipcom_strcmp(token, "gateway6") == 0)
        {
            if (pass == 2)
                err = ipnet_config_add_gateway6(ifname, option);
        }
#else
        else if (ipcom_strcmp(token, "inet6") == 0
                 || ipcom_strcmp(token, "gateway6") == 0)
            IP_NOOP;
#endif /* IPCOM_USE_INET6 */
        else if (ipcom_strcmp(token, "devname") != 0)
            /* Invalid configuration */
            err = -IP_ERRNO_EINVAL;

        ipcom_free(option_org);
        if (err != 0)
        {
            if (err < 0)
                err = -err;
            IPCOM_LOG4(ERR, "failed on '%s' when configuring %s: %s (%d)",
                conf[i], ifname, ipcom_strerror(err), err);
            IP_PANIC();
            break;
        }
    }

    return err;
}


/*
 *===========================================================================
 *                    ipnet_config_run_boot_cmd
 *===========================================================================
 * Description: Configures one route entry.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_config_run_boot_cmd(const char *conf)
{
    char *option_org;
    char *option;
    char *argv[20];
    char *arg;
    int   argc = 0;
    int   ret = -IP_ERRNO_EINVAL;

    option_org = option = ipcom_strdup(conf);
    if (option == IP_NULL)
        return -IP_ERRNO_ENOMEM;

    while (IP_NULL != (arg = ipcom_strtok_r(option, " \t", &option)))
    {
        if (argc == 1)
            argv[argc++] = "-silent";
        argv[argc++] = arg;

        if (argc >= (int) (sizeof(argv) / sizeof(*argv)))
        {
            IPCOM_LOG1(ERR, "Too many arguments in command %s", conf);
            IP_PANIC();
        }
    }
    argv[argc] = IP_NULL;

    if (argc > 2)
    {
        if (ipcom_strcmp(argv[0], "route") == 0)
            ret = ipnet_config_cmd_route(argc, argv);
        else if (ipcom_strcmp(argv[0], "ifconfig") == 0)
            ret = ipnet_config_cmd_ifconfig(argc, argv);
        else if (ipcom_strcmp(argv[0], "qc") == 0)
            ret = ipnet_config_cmd_qc(argc, argv);
#if defined(IPNET_USE_NAT) && !defined(IP_PORT_LKM) && !defined(IP_PORT_VXWORKS)
        else if (ipcom_strcmp(argv[0], "nat") == 0)
            ret = ipnet_cmd_nat(argc, argv);
#endif
    }

    ipcom_free(option_org);
    return ret;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipnet_boot_config
 *===========================================================================
 * Description: Adds the initial configuration to the stack.
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipnet_boot_config()
{
    const char *key;
    const char *ifname = IP_NULL;
    int         begin_index;
    int         end_index;
    int         err = 0;
    Ip_fd       fd;

#ifdef IPCOM_USE_INET
    fd = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, 0);
#else
    fd = ipcom_socket(IP_AF_INET6, IP_SOCK_DGRAM, 0);
#endif
    if (fd == IP_SOCKERR)
    {
        IP_PANIC2();
        err = -ipcom_errno;
        goto leave;
    }

    /* Configure interfaces. */
    begin_index = 0;
    while (ipnet_conf_interfaces[begin_index] != IP_NULL)
    {
        end_index = begin_index;
        while (ipnet_conf_interfaces[end_index] != IP_NULL)
        {
            key = ipcom_strstr((const char *)ipnet_conf_interfaces[end_index], "ifname");
            if (key != IP_NULL)
            {
                ifname = key + 6;
                while(*ifname && (*ifname == ' ' || *ifname == '\t'))
                    ifname++;
            }
            end_index++;
        }
        ip_assert(ifname != IP_NULL);
        err = ipnet_config_interface(fd,
                                     (char *)ifname,
                                     (char **) &ipnet_conf_interfaces[begin_index],
                                     end_index - begin_index,
                                     1);
        if (err >= 0)
        {
            /* Run all pending ioctls since the addresses are not added until that is done
               which means that adding of gateway will fail since it cannot be reached */
            err = ipnet_config_interface(fd,
                                         (char *)ifname,
                                         (char **) &ipnet_conf_interfaces[begin_index],
                                         end_index - begin_index,
                                         2);
        }
#if defined (WRS_IPNET)
        if (err >= 0 && ipnet_qc_unif_config_hook != IP_NULL)
        {
	    err = ipnet_qc_unif_config_hook (ifname);
        }
#endif /* defined (WRS_IPNET) */
        if (err < 0)
        {
            IP_PANIC2();
            goto leave;
        }
        begin_index = end_index + 1;
    }

    /* Run boot configuration commands. */
    for (begin_index = 0; ipnet_conf_boot_cmd[begin_index] != IP_NULL; begin_index++)
    {
        err = ipnet_config_run_boot_cmd(ipnet_conf_boot_cmd[begin_index]);
        if (err < 0)
        {
            IPCOM_LOG1(ERR, "Configuration failed to run '%s'", ipnet_conf_boot_cmd[begin_index]);
            IP_PANIC2();
            goto leave;
        }
    }

 leave:
    if (fd != IP_INVALID_SOCKET)
        (void)ipcom_socketclose(fd);
    return err;
}


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
