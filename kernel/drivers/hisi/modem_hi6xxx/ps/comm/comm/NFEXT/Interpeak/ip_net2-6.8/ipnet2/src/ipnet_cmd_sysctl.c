/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_cmd_sysctl.c,v $ $Revision: 1.11 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_cmd_sysctl.c,v $
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
#include <ipcom_shell.h>
#include <ipcom_sysctl.h>

#include "ipnet.h"
#include "ipnet_cmd.h"
#include "ipnet_ctrl.h"
#include "ipnet_h.h"
#include "ipnet_routesock.h"
#ifdef IPCOM_USE_INET6
#include "ipnet_ip6.h"
#endif

#include "ipnet_sysctl.h"

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */
#define IPNET_CMD_SYSCTL_NETIF_NO       (1 << 0)
#define IPNET_CMD_SYSCTL_NETIF_YES      (1 << 1)

#define IPNET_CMD_SYSCTL_TYPE_TRUNK     (0)
#define IPNET_CMD_SYSCTL_TYPE_LEAF_INT  (1)


#define IPNET_CMD_SYSCTL_CTL_END        (-1)

#if defined(IPNET_DEBUG) || defined(IPNET_STATISTICS)
#define IPNET_CMD_SYSCTL_VALUE_SIZE     sizeof(Ipnet_stats)
#elif defined (IPCOM_USE_MIB2)
#define IPNET_CMD_SYSCTL_VALUE_SIZE     sizeof(Ipcom_mib2_stats)
#else
#define IPNET_CMD_SYSCTL_VALUE_SIZE     128
#endif

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

IP_STATIC Ip_bool
ipnet_cmd_sysctl_strcmp(const Ipnet_cmd_sysctl  *ctl,
                        const char              *what,
                        int                     *name,
                        int                     *namelen,
                        char                    **names,
                        int                     *nameslen);

IP_STATIC Ip_bool
ipnet_cmd_sysctl_ival(Ip_bool        set,
                      int            *name,
                      int            namelen,
                      char           **value);

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */


#ifdef IPCOM_USE_INET
IP_STATIC Ipnet_cmd_sysctl ipnet_sysctl_inet_table[] =
{
    { "forwarding",
      IPNET_CMD_SYSCTL_TYPE_LEAF_INT,
      IPNET_CMD_SYSCTL_NETIF_NO|IPNET_CMD_SYSCTL_NETIF_YES,
      IP_NULL,
      { IP_IPPROTO_IP, IP_IPCTL_FORWARDING, IPNET_CMD_SYSCTL_CTL_END },
    },
#ifdef IP_UDPCTL_CHECKSUM
    { "udp_checksum",
      IPNET_CMD_SYSCTL_TYPE_LEAF_INT,
      IPNET_CMD_SYSCTL_NETIF_NO,
      IP_NULL,
      { IP_IPPROTO_UDP, IP_UDPCTL_CHECKSUM, IPNET_CMD_SYSCTL_CTL_END },
    },
#endif
#ifdef IP_IPCTL_MTUDISC
    { "mtu_discovery",
      IPNET_CMD_SYSCTL_TYPE_LEAF_INT,
      IPNET_CMD_SYSCTL_NETIF_NO,
      IP_NULL,
      { IP_IPPROTO_IP, IP_IPCTL_MTUDISC, IPNET_CMD_SYSCTL_CTL_END },
    },
#endif
#ifdef IP_IPCTL_SENDREDIRECTS
    { "send_redirects",
      IPNET_CMD_SYSCTL_TYPE_LEAF_INT,
      IPNET_CMD_SYSCTL_NETIF_NO,
      IP_NULL,
      { IP_IPPROTO_IP, IP_IPCTL_SENDREDIRECTS, IPNET_CMD_SYSCTL_CTL_END },
    },
#endif
    { IP_NULL,
      IPNET_CMD_SYSCTL_TYPE_TRUNK,
      IPNET_CMD_SYSCTL_NETIF_NO,
      IP_NULL,
      { IPNET_CMD_SYSCTL_CTL_END },
    }
};
#endif


#ifdef IPCOM_USE_INET6
IP_STATIC Ipnet_cmd_sysctl ipnet_sysctl_inet6_table[] =
{
    { "forwarding",
      IPNET_CMD_SYSCTL_TYPE_LEAF_INT,
      IPNET_CMD_SYSCTL_NETIF_NO|IPNET_CMD_SYSCTL_NETIF_YES,
      IP_NULL,
      { IP_IPPROTO_IPV6, IP_IPV6CTL_FORWARDING, IPNET_CMD_SYSCTL_CTL_END },
    },
#ifdef IP_IPV6CTL_ACCEPT_RTADV
    { "accept_rtadv",
      IPNET_CMD_SYSCTL_TYPE_LEAF_INT,
      IPNET_CMD_SYSCTL_NETIF_YES,
      IP_NULL,
      { IP_IPPROTO_IPV6, IP_IPV6CTL_ACCEPT_RTADV, IPNET_CMD_SYSCTL_CTL_END },
    },
#endif
#ifdef IP_IPV6CTL_ENABLED
    { "enabled",
      IPNET_CMD_SYSCTL_TYPE_LEAF_INT,
      IPNET_CMD_SYSCTL_NETIF_YES,
      IP_NULL,
      { IP_IPPROTO_IPV6, IP_IPV6CTL_ENABLED, IPNET_CMD_SYSCTL_CTL_END },
    },
#endif
    { IP_NULL,
      IPNET_CMD_SYSCTL_TYPE_TRUNK,
      IPNET_CMD_SYSCTL_NETIF_NO,
      IP_NULL,
      { IPNET_CMD_SYSCTL_CTL_END },
    }
};
#endif


#ifdef IP_PORT_VXWORKS
/*
 * This table is only used by vxworks CLI parsing and sysctl registration.
 * See file target/src/wrn/coreip/wrapper/libc/netSysctl.c for details.
 * Note that it's hidden from -a display by setting argument to 0.
 */
IP_STATIC Ipnet_cmd_sysctl ipnet_sysctl_route_table[] =
{
    { "rt_dump",
      IPNET_CMD_SYSCTL_TYPE_LEAF_INT,
      0,
      IP_NULL,
      { 0, IP_NET_RT_DUMP, IPNET_CMD_SYSCTL_CTL_END },
    },
    { "rt_flags",
      IPNET_CMD_SYSCTL_TYPE_LEAF_INT,
      0,
      IP_NULL,
      { 0, IP_NET_RT_FLAGS, IPNET_CMD_SYSCTL_CTL_END },
    },
    { "rt_iflist",
      IPNET_CMD_SYSCTL_TYPE_LEAF_INT,
      0,
      IP_NULL,
      { 0, IP_NET_RT_IFLIST, IPNET_CMD_SYSCTL_CTL_END },
    },
    { IP_NULL,
      IPNET_CMD_SYSCTL_TYPE_TRUNK,
      IPNET_CMD_SYSCTL_NETIF_NO,
      IP_NULL,
      { IPNET_CMD_SYSCTL_CTL_END },
    }
};
#endif


IP_STATIC Ipnet_cmd_sysctl ipnet_sysctl_net_table[] =
{
#ifdef IPCOM_USE_INET
    { "inet",
      IPNET_CMD_SYSCTL_TYPE_TRUNK,
      IPNET_CMD_SYSCTL_NETIF_NO,
      ipnet_sysctl_inet_table,
      { IP_AF_INET, IPNET_CMD_SYSCTL_CTL_END },
    },
#endif
#ifdef IPCOM_USE_INET6
    { "inet6",
      IPNET_CMD_SYSCTL_TYPE_TRUNK,
      IPNET_CMD_SYSCTL_NETIF_NO,
      ipnet_sysctl_inet6_table,
      { IP_AF_INET6, IPNET_CMD_SYSCTL_CTL_END },
    },
#endif
#ifdef IP_PORT_VXWORKS
    { "route",
      IPNET_CMD_SYSCTL_TYPE_TRUNK,
      IPNET_CMD_SYSCTL_NETIF_NO,
      ipnet_sysctl_route_table,
      { IP_AF_ROUTE, IPNET_CMD_SYSCTL_CTL_END },
    },
#endif
    { IP_NULL,
      IPNET_CMD_SYSCTL_TYPE_TRUNK,
      IPNET_CMD_SYSCTL_NETIF_NO,
      IP_NULL,
      { IPNET_CMD_SYSCTL_CTL_END },
    }
};


IP_STATIC Ipnet_cmd_sysctl ipnet_sysctl_base_table[] =
{
    { "net",
      IPNET_CMD_SYSCTL_TYPE_TRUNK,
      IPNET_CMD_SYSCTL_NETIF_NO,
      ipnet_sysctl_net_table,
      { IP_CTL_NET, IPNET_CMD_SYSCTL_CTL_END },
    },
    { IP_NULL,
      IPNET_CMD_SYSCTL_TYPE_TRUNK,
      IPNET_CMD_SYSCTL_NETIF_NO,
      IP_NULL,
      { IPNET_CMD_SYSCTL_CTL_END },
    }
};


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipnet_cmd_sysctl_get
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_cmd_sysctl_get(int            *name,
                     int            namelen,
                     void           **oldp,
                     Ip_size_t      *oldlenp)
{
    *oldlenp = 0;
    if (ipcom_sysctl(name, namelen, IP_NULL, oldlenp, IP_NULL, 0) < 0)
        return IP_FALSE;

    *oldp = ipcom_malloc(*oldlenp);
    if (*oldp == IP_NULL)
        return IP_FALSE;

    if (ipcom_sysctl(name, namelen, *oldp, oldlenp, IP_NULL, 0) < 0)
    {
        ipcom_free(*oldp);
        return IP_FALSE;
    }

    return IP_TRUE;
}


/*
 *===========================================================================
 *                    ipnet_cmd_sysctl_get
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_cmd_sysctl_getint(int            *name,
                        int            namelen,
                        int            *value)
{
    Ip_size_t vallen = sizeof(*value);
    if (ipcom_sysctl(name, namelen, value, &vallen, IP_NULL, 0) < 0)
        return IP_FALSE;

    return IP_TRUE;
}



/*
 *===========================================================================
 *                    ipnet_cmd_sysctl_set
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_cmd_sysctl_set(int            *name,
                     int            namelen,
                     void           *newp,
                     Ip_size_t      newlenp)
{
    if (ipcom_sysctl(name, namelen, IP_NULL, IP_NULL, newp, newlenp) < 0)
        return IP_FALSE;

    return IP_TRUE;
}

/*
 *===========================================================================
 *                    ipnet_cmd_sysctl_strcmp
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_cmd_sysctl_strcmp(const Ipnet_cmd_sysctl  *ctl,
                        const char              *what,
                        int                     *name,
                        int                     *namelen,
                        char                    **names,
                        int                     *nameslen)
{
    if (ipcom_strcasecmp(what, ctl->comp) == 0)
    {
        int i;
        for (i = 0; ctl->ctl[i] != -1; i++)
        {
            /* Add the CTL values */
            name[(*namelen)++] = ctl->ctl[i];
        }
        /* Store name */
        names[(*nameslen)++] = (char *)ctl->comp;
        return IP_TRUE;
    }

    return IP_FALSE;
}

/*
 *===========================================================================
 *                    ipnet_cmd_sysctl_dump_netif
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char **
ipnet_cmd_sysctl_dump_netif(void)
{
    int                     name[6];
    Ip_u8                  *if_data;
    Ip_size_t               if_data_len;
    Ip_size_t               if_data_offset;
    struct Ipnet_if_msghdr *ifm;
    char                    ifname[IP_IFNAMSIZ];
    int                     sz = 0;
    char                    **names = IP_NULL;

    name[0] = IP_CTL_NET;
    name[1] = IP_AF_ROUTE;
    name[2] = 0;    /* Always 0 */
    name[3] = 0;
    name[4] = IP_NET_RT_IFLIST;
    name[5] = 0;

    if (ipnet_cmd_sysctl_get(name, 6, (void **)&if_data, &if_data_len) == IP_FALSE)
    {
        ipcom_printf("sysctl: netif dump failed"IP_LF);
        return IP_NULL;
    }

    if_data_offset = 0;
    while (if_data_len > if_data_offset + sizeof(struct Ipnet_if_msghdr))
    {
        ifm = (struct Ipnet_if_msghdr *) &if_data[if_data_offset];
        if_data_offset += ifm->ifm_msglen;

        if (ifm->ifm_type != IPNET_RTM_IFINFO)
            continue;

        sz++;
    }

    if (sz == 0)
        goto done;

    names = ipcom_calloc(sz + 1, sizeof(char *));
    if (names == IP_NULL)
    {
        ipcom_printf("sysctl: unable to allocate memory"IP_LF);
        goto done;
    }

    if_data_offset = 0;
    sz = 0;
    while (if_data_len > if_data_offset + sizeof(struct Ipnet_if_msghdr))
    {
        ifm = (struct Ipnet_if_msghdr *) &if_data[if_data_offset];
        if_data_offset += ifm->ifm_msglen;

        if (ifm->ifm_type != IPNET_RTM_IFINFO)
            continue;

        ipcom_if_indextoname(ifm->ifm_index, ifname);
        names[sz++] = ipcom_strdup(ifname);
    }

    names[sz++] = IP_NULL;

done:
    ipcom_free(if_data);
    return names;
}

/*
 *===========================================================================
 *                    ipnet_cmd_sysctl_ival
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_cmd_sysctl_ival(Ip_bool        set,
                      int            *name,
                      int            namelen,
                      char           **value)
{
    if (set)
    {
        int val = ipcom_atoi(*value);
        return ipnet_cmd_sysctl_set(name, namelen, &val, sizeof(val));
    }
    else
    {
        int ival;
        if (!ipnet_cmd_sysctl_getint(name, namelen, &ival))
            return IP_FALSE;

        /* Create string representation */
        ipcom_snprintf(*value, IPNET_CMD_SYSCTL_VALUE_SIZE, "%d", ival);
    }

    return IP_TRUE;
}


/*
 *===========================================================================
 *                    ipnet_cmd_sysctl_print_key
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_sysctl_print_key(char **names,
                           int  nameslen)
{
    int i;
    for (i = 0; i < nameslen; i++)
    {
        ipcom_printf("%s%s", i != 0? "." : "", names[i]);
    }
}

/*
 *===========================================================================
 *                    ipnet_cmd_sysctl_print_value
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_sysctl_print_value(char *val)
{
    ipcom_printf("=%s"IP_LF, val);
}


/*
 *===========================================================================
 *                    ipnet_cmd_sysctl_exec
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_cmd_sysctl_exec(const Ipnet_cmd_sysctl    *ctl,
                      Ip_bool                   set,
                      int                       *name,
                      int                       namelen,
                      char                      **names,
                      int                       nameslen,
                      char                      *value)
{
    if (set)
    {
        switch (ctl->type)
        {
        case IPNET_CMD_SYSCTL_TYPE_LEAF_INT:
            if (ipnet_cmd_sysctl_ival(set, name, namelen, &value))
            {
                ipnet_cmd_sysctl_print_key(names, nameslen);
                ipnet_cmd_sysctl_print_value(value);
                return IP_TRUE;
            }
        default:
            break;
        }
    }
    else
    {
        char vbuf[IPNET_CMD_SYSCTL_VALUE_SIZE];
        value = vbuf;

        switch (ctl->type)
        {
        case IPNET_CMD_SYSCTL_TYPE_LEAF_INT:
            if (ipnet_cmd_sysctl_ival(set, name, namelen, &value))
            {
                ipnet_cmd_sysctl_print_key(names, nameslen);
                ipnet_cmd_sysctl_print_value(value);
                return IP_TRUE;
            }
        default:
            break;
        }
    }

    return IP_FALSE;
}


/*
 *===========================================================================
 *                    ipnet_cmd_sysctl_print_ctl
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_sysctl_print_ctl(const Ipnet_cmd_sysctl  *ctl,
                           int                     *name,
                           int                     namelen,
                           char                    **names,
                           int                     nameslen,
                           const Ipnet_cmd_sysctl  *skip)
{
    int cnamelen  = namelen;
    int cnameslen = nameslen;

    (void) skip;
    while (ctl->comp != IP_NULL)
    {
        if (IP_BIT_ISSET(ctl->argument, IPNET_CMD_SYSCTL_NETIF_NO))
        {
            /* Resolve this particular node */
            if (ipnet_cmd_sysctl_strcmp(ctl, ctl->comp, name, &cnamelen, names, &cnameslen))
            {
                if (ctl->type == IPNET_CMD_SYSCTL_TYPE_TRUNK)
                {
                    /* Go to the next table */
                    ipnet_cmd_sysctl_print_ctl(ctl->ntable, name, cnamelen, names, cnameslen, IP_NULL);
                }
                else
                {
                    if (ipnet_cmd_sysctl_exec(ctl, IP_FALSE, name, cnamelen, names, cnameslen, IP_NULL) == IP_FALSE)
                    {
                        ipcom_printf("failed to get: ");
                        ipnet_cmd_sysctl_print_key(names, cnameslen);
                        ipcom_printf(IP_LF);
                    }
                }
            }
        }

        /* Do the netifs */
        if (IP_BIT_ISSET(ctl->argument, IPNET_CMD_SYSCTL_NETIF_YES))
        {
            /* Dump the arguments */
            char **argv = ipnet_cmd_sysctl_dump_netif();

            if (argv)
            {
                int  i;

                for (i = 0; argv[i] != IP_NULL; i++)
                {
                    cnamelen    = namelen;
                    cnameslen   = nameslen;

                    names[cnameslen++] = argv[i];
                    if (ipnet_cmd_sysctl_strcmp(ctl, ctl->comp, name, &cnamelen, names, &cnameslen))
                    {
                        name[cnamelen++] = ipcom_if_nametoindex(argv[i]);

                        if (ctl->type == IPNET_CMD_SYSCTL_TYPE_TRUNK)
                        {
                            /* Go to the next table */
                            ipnet_cmd_sysctl_print_ctl(ctl->ntable, name, cnamelen, names, cnameslen, IP_NULL);
                        }
                        else
                        {
                            if (ipnet_cmd_sysctl_exec(ctl, IP_FALSE, name, cnamelen, names, cnameslen, IP_NULL) == IP_FALSE)
                            {
                                ipcom_printf("failed to get: ");
                                ipnet_cmd_sysctl_print_key(names, cnameslen);
                                ipcom_printf(IP_LF);
                            }
                        }
                    }

                    ipcom_free(argv[i]);
                }

                ipcom_free(argv);
            }
        }

        ctl++;
        cnamelen    = namelen;
        cnameslen   = nameslen;
    }
}


/*
 *===========================================================================
 *                    ipnet_cmd_sysctl_do
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:     0 on success, -1 on syntax error. > 0 unknown key
 *
 */
IP_STATIC int
ipnet_cmd_sysctl_do(Ip_bool         set,
                    const char      *var)
{
    char                    *dupval = ipcom_strdup(var);
    char                    *tok;
    char                    *ntok;
    char                    *last;
    char                    *value      = IP_NULL;
    char                    *names[12];
    int                     nameslen    = 0;
    int                     name[12];
    int                     namelen     = 0;
    const Ipnet_cmd_sysctl  *current    = ipnet_sysctl_base_table;
    const Ipnet_cmd_sysctl  *ctl        = IP_NULL;
    int                     err         = -1;
    const char              *netif      = IP_NULL;
    int                     argbit      = IPNET_CMD_SYSCTL_NETIF_NO;

    if (set)
    {
        tok = ipcom_strtok_r(dupval, "=", &last);
        if (tok == IP_NULL)
            goto done;
        value = ipcom_strtok_r(IP_NULL, "=", &last);
        if (value == IP_NULL)
            goto done;
    }

    err = 1;
    tok = ipcom_strtok_r(dupval, ".", &last);
    while (tok != IP_NULL)
    {
        ntok = ipcom_strtok_r(IP_NULL, ".", &last);
        ctl = current;

        /* */
        while (ctl->comp != IP_NULL)
        {
            if (IP_BIT_ISSET(ctl->argument, argbit))
            {
                if (ipnet_cmd_sysctl_strcmp(ctl, tok, name, &namelen, names, &nameslen) == IP_TRUE)
                {
                    if (ctl->type == IPNET_CMD_SYSCTL_TYPE_TRUNK)
                    {
                        current = ctl->ntable;
                        break;
                    }
                    else
                    {
                        /* EOL */
                        if (ntok)
                            goto done;

                        /* Check any cached netif's */
                        if (netif)
                        {
                            name[namelen++] = ipcom_if_nametoindex(netif);
                            netif = IP_NULL;
                        }

                        /* We're done */
                        if (ipnet_cmd_sysctl_exec(ctl, set, name, namelen, names, nameslen, value))
                            err = 0;

                        /**/
                        goto done;
                    }
                }
            }

            ctl++;
        }

        /* We've failed */
        if (ctl->comp == IP_NULL)
        {
            /* Check interface */
            if (argbit == IPNET_CMD_SYSCTL_NETIF_NO && ipcom_if_nametoindex(tok) != 0)
            {
                netif               = tok;
                names[nameslen++]   = tok;
                argbit = IPNET_CMD_SYSCTL_NETIF_YES;
            }
            else
            {
                ipcom_printf("sysctl: Failed to parse subkey %s"IP_LF, tok);
                goto done;
            }
        }
        else
        {
            argbit  = IPNET_CMD_SYSCTL_NETIF_NO;
            netif   = IP_NULL;
        }

        /* Store next */
        tok     = ntok;
    }

    /* Do dump */
    if (set == IP_FALSE)
    {
        ipnet_cmd_sysctl_print_ctl(current,
                                   name,
                                   namelen,
                                   names,
                                   nameslen,
                                   IP_NULL);
        err = 0;
    }

done:
    ipcom_free(dupval);
    return err;
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
ipnet_cmd_sysctl_usage(void)
{
    ipcom_printf(IP_LF);
    ipcom_printf("NAME"IP_LF);
    ipcom_printf("   sysctl - Get or set sysctl values"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("SYNOPSIS"IP_LF);
    ipcom_printf("   sysctl -w variable=value"IP_LF);
    ipcom_printf("   sysctl -a"IP_LF);
    ipcom_printf("   sysctl variable"IP_LF);
    ipcom_printf("DESCRIPTION"IP_LF);
    ipcom_printf("   sysctl is used to get or set system parameters."IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("COMMANDS"IP_LF);
    ipcom_printf("   -a                 List all sysctl parameters."IP_LF);
    ipcom_printf("   -w variable=value  Change value of variable to value"IP_LF);
    ipcom_printf("   variable           Get value of variable"IP_LF);
    ipcom_printf(IP_LF);
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
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
ipnet_cmd_sysctl(int argc, char **argv)
{
    int                next_arg = 1;
    Ip_bool            set      = IP_FALSE;

    if (argc < 2)
    {
        ipnet_cmd_sysctl_usage();
        return IPCOM_SHELL_ERR_USAGE;
    }

    /* Silent must only be used by the configuration daemon */
    if (argc > next_arg && ipcom_strcmp(argv[next_arg], "-a") == 0)
    {
        char                    *names[12];
        int                     name[12];

        ipnet_cmd_sysctl_print_ctl(ipnet_sysctl_base_table,
                                   name,
                                   0,
                                   names,
                                   0,
                                   IP_NULL);
        return IPCOM_SHELL_SUCCESS;
    }

    /* Silent must only be used by the configuration daemon */
    if (argc > next_arg && ipcom_strcmp(argv[next_arg], "-w") == 0)
    {
        set = IP_TRUE;
        next_arg++;
    }

    /* Process all switches */
    while (argc > next_arg)
    {
        int ret;
        if (argv[next_arg][0] == '-')
        {
            ipnet_cmd_sysctl_usage();
            return IPCOM_SHELL_ERR_USAGE;
        }

        ret = ipnet_cmd_sysctl_do(set, argv[next_arg]);
        if (ret > 0)
        {
            ipcom_printf("sysctl: unknown key: %s"IP_LF, argv[next_arg]);
            return IPCOM_SHELL_ERR_USAGE;
        }
        else if (ret < 0)
        {
            ipcom_printf("sysctl: syntax error: %s"IP_LF, argv[next_arg]);
            return IPCOM_SHELL_ERR_USAGE;
        }

        next_arg++;
    }

    return IPCOM_SHELL_SUCCESS;
}

/*
 *===========================================================================
 *                    ipnet_sysctl_get_base
 *===========================================================================
 * Description: This routine exposes the IPNET sysctl tree root node.
 * Parameters:  None.
 * Returns:     A pointer to Ipnet_cmd_sysctl
 *
 */
IP_PUBLIC Ipnet_cmd_sysctl *
ipnet_sysctl_get_base(void)
{
    return ipnet_sysctl_base_table;
}


/*
 *===========================================================================
 *                           netsysctlname2oid
 *===========================================================================
 * Description:
 * Parameters:  ctl -
 *              name -
 *              pNewName -
 *              nameLen -
 * Returns:
 *
 */
IP_PUBLIC int
netsysctlname2oid(Ipnet_cmd_sysctl *ctl, char* name, int* pNewName, int* nameLen)
{
#ifdef IPCOM_USE_INET
    int i;
  
    if(ctl == IP_NULL)
    {
        ctl = ipnet_sysctl_inet_table;
        pNewName[(*nameLen)++] = IP_IPPROTO_IP;
    }
    
    /* if you want to support trunk node, pls change this function */
    while (ctl->comp != IP_NULL)
    {
        if (IP_BIT_ISSET(ctl->argument, IPNET_CMD_SYSCTL_NETIF_NO))
        {
            if (ipcom_strcasecmp(name, ctl->comp) == 0)
            {
                for (i = 0; ctl->ctl[i] != -1; i++);
                pNewName[(*nameLen)++] = ctl->ctl[i ? i-1 : i];
            }
        }
        
        /* Do the netifs */
        if (IP_BIT_ISSET(ctl->argument, IPNET_CMD_SYSCTL_NETIF_YES))
        {
            /* Dump the arguments */
            char **argv = ipnet_cmd_sysctl_dump_netif();

            if (argv)
            {
                for (i = 0; argv[i] != IP_NULL; i++)
                {
                    if(ipcom_strcasecmp(name, argv[i])==0)
                    {
                        name += ipcom_strlen(argv[i]) + 1;
                        pNewName[(*nameLen)++] = i;
                        netsysctlname2oid(ipnet_sysctl_inet_table, name, pNewName, nameLen);
                    }
                    
                    ipcom_free(argv[i]);
                }
                
                ipcom_free(argv);
            }
        }
        
        ctl++;
    }
#endif
    
    return 0;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
