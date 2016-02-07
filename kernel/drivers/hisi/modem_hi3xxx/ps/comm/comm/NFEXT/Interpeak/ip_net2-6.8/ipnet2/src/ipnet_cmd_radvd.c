/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_cmd_radvd.c,v $ $Revision: 1.17.6.1.4.3 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_cmd_radvd.c,v $
 *   $Author: jhorteli $
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
/* IPNET router advertisement daemon */

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
#include <ipcom_ipd.h>
#include <ipcom_sysvar.h>
#include <ipcom_strlib.h>

#include "ipnet.h"
#include "ipnet_cmd.h"
#include "ipnet_radvd.h"

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_RADVD_CMD_ADD     1
#define IPNET_RADVD_CMD_DELETE  2
#define IPNET_RADVD_CMD_ENABLE  3
#define IPNET_RADVD_CMD_DISABLE 4

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

enum Ipnet_radvd_opt {
    IPNET_RADVD_OPT_ENABLE_AUTO,
    IPNET_RADVD_OPT_DISABLE_AUTO,
    IPNET_RADVD_OPT_ENABLE_ON_LINK,
    IPNET_RADVD_OPT_DISABLE_ON_LINK,
    IPNET_RADVD_OPT_VALID_LIFETIME,
    IPNET_RADVD_OPT_PREFERRED_LIFETIME
};

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

#ifdef IPCOM_USE_INET6

IP_STATIC Ipcom_cmd_int_str_map ipnet_radvd_cmd_map[] =
{
    { IPNET_RADVD_CMD_ADD,     "add" },
    { IPNET_RADVD_CMD_DELETE,  "delete" },
    { IPNET_RADVD_CMD_ENABLE,  "enable" },
    { IPNET_RADVD_CMD_DISABLE, "disable" },
    { -1, IP_NULL }
};


IP_STATIC Ipcom_cmd_int_str_map ipnet_radvd_opt_map[] =
{
    { IPNET_RADVD_OPT_ENABLE_AUTO,        "A" },
    { IPNET_RADVD_OPT_DISABLE_AUTO,       "-A" },
    { IPNET_RADVD_OPT_ENABLE_ON_LINK,     "L" },
    { IPNET_RADVD_OPT_DISABLE_ON_LINK,    "-L" },
    { IPNET_RADVD_OPT_VALID_LIFETIME,     "valid" },
    { IPNET_RADVD_OPT_PREFERRED_LIFETIME, "preferred" },
    { -1, IP_NULL }
};

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipnet_cmd_radvd_print_usage
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_radvd_print_usage(void)
{
    ipcom_printf("NAME"IP_LF
                 "   radvdconfig - Configure IPNET Router Advertisment Daemon"IP_LF
                 IP_LF
                 "SYNOPSIS"IP_LF
                 "   radvdconfig [-silent] <interface> add <id> <prefix>/<prefix_len> [optional parameters]"IP_LF
                 "   radvdconfig [-silent] <interface> enable|disable|delete <id>"IP_LF
                 IP_LF
                 "DESCRIPTION"IP_LF
                 "   radvdconfig is used to add/delete/enable/disable prefixes that"IP_LF
                 "   will be announced by IPNET in a IPv6 Router Advertisment message."IP_LF
                 IP_LF
                 "OPTIONS:"IP_LF
                 "   -silent      Suppress output. MUST be first argument"IP_LF
                 IP_LF
                 "COMMANDS"IP_LF
                 "   add <id> <prefix>/<prefix_len>."IP_LF
                 "                Adds a new prefix in disabled state."IP_LF
                 "   delete  <id> Deletes a prefix entry."IP_LF
                 "   enable  <id> Enables a prefix entry."IP_LF
                 "   disable <id> Disables a prefix entry."IP_LF
                 IP_LF
                 "VALUES"IP_LF
                 "   <interface>  The name of an attached network interface."IP_LF
                 "   <id>         String that identifies the prefix."IP_LF
                 "   <prefix>     Prefix specified as an IPv6 address."IP_LF
                 "   <prefixlen>  Number of bits in <prefix> that is used for network ID."IP_LF
                 "OPTIONAL PARAMETERS"IP_LF
                 "   valid <sec>  Number of seconds the prefix is valid."IP_LF
                 "   preferred <sec> Number of seconds the prefix is preferred."IP_LF
                 "   A            The prefix can be used for autonomous address"IP_LF
                 "   -A           The prefix cannot be used for autonomous address"IP_LF
                 "   L            The prefix can be used for on-link determination."IP_LF
                 "   -L           The prefix cannot be used for on-link determination."IP_LF
                 "                configuration."IP_LF);
}


/*
 *===========================================================================
 *                        ipnet_cmd_radvd_add
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_radvd_add(Ip_bool silent, char *ifname, char *id, int argc, char **argv)
{
    char               sysvar_name[IPNET_RADVD_SYSVAR_NAME_LEN];
    char              *val_name;
    int                n;
    struct Ip_in6_addr a;

    if (argc < 1)
    {
        if (silent == IP_FALSE)
            ipcom_printf("Must specify <prefix>/<prefixlen>"IP_LF);
        return;
    }

    if (ipcom_getinaddrprefixbyaddr(IP_AF_INET6, argv[0], &a, &n) < 0)
    {
        if (silent == IP_FALSE)
            ipcom_printf("%s is not a valid <prefix>/<prefixlen>"IP_LF, argv[0]);
        return;
    }

    ipcom_snprintf(sysvar_name, sizeof(sysvar_name),
                   "ipnet.inet6.radvd.%s.AdvPrefixList",
                   ifname);
    (void)ipcom_sysvar_set(sysvar_name, "", 0);

    ipcom_snprintf(sysvar_name, sizeof(sysvar_name),
                   "ipnet.inet6.radvd.%s.prefix.%s",
                   ifname, id);
    (void)ipcom_sysvar_set(sysvar_name, argv[0], IPCOM_SYSVAR_FLAG_OVERWRITE);

    ipcom_strcat(sysvar_name, ".");
    val_name = sysvar_name + ipcom_strlen(sysvar_name);

    while (++argv, --argc)
    {
        enum Ipnet_radvd_opt opt;

        opt = ipcom_cmd_str_to_key(ipnet_radvd_opt_map, *argv);
        switch (opt)
        {
        case IPNET_RADVD_OPT_ENABLE_AUTO:
        case IPNET_RADVD_OPT_DISABLE_AUTO:
            ipcom_strcpy(val_name, "AdvAutonomousFlag");
            (void)ipcom_sysvar_set(sysvar_name,
                                   opt == IPNET_RADVD_OPT_ENABLE_AUTO ? "1" : "0",
                                   IPCOM_SYSVAR_FLAG_OVERWRITE);
            break;

        case IPNET_RADVD_OPT_ENABLE_ON_LINK:
        case IPNET_RADVD_OPT_DISABLE_ON_LINK:
            ipcom_strcpy(val_name, "AdvOnLinkFlag");
            (void)ipcom_sysvar_set(sysvar_name,
                                   opt == IPNET_RADVD_OPT_ENABLE_ON_LINK ? "1" : "0",
                                   IPCOM_SYSVAR_FLAG_OVERWRITE);
            break;

        case IPNET_RADVD_OPT_PREFERRED_LIFETIME:
        case IPNET_RADVD_OPT_VALID_LIFETIME:
            argv++;
            if (argc-- == 0)
            {
                if (silent == IP_FALSE)
                    ipcom_printf("Missing timeout in seconds"IP_LF);
                return;
            }
            ipcom_strcpy(val_name,
                         (opt == IPNET_RADVD_OPT_VALID_LIFETIME
                          ? "AdvValidLifetime"
                          : "AdvPreferredLifetime"));
            (void)ipcom_sysvar_set(sysvar_name, *argv, IPCOM_SYSVAR_FLAG_OVERWRITE);
            break;

        default:
            if (silent == IP_FALSE)
                ipcom_printf("'%s' is a unknown option"IP_LF, *argv);
            return;
        }
    }
}


/*
 *===========================================================================
 *                        ipnet_cmd_radvd_delete
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_radvd_delete(Ip_bool silent, char *ifname, char *id)
{
    char  sysvar_name[IPNET_RADVD_SYSVAR_NAME_LEN];

    (void)silent;

    ipcom_snprintf(sysvar_name, sizeof(sysvar_name),
                   "ipnet.inet6.radvd.%s.prefix.%s",
                   ifname, id);
    (void)ipcom_sysvar_unset(sysvar_name);
}


/*
 *===========================================================================
 *                        ipnet_cmd_radvd_enable
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_radvd_enable(Ip_bool silent, char *ifname, char *id)
{
    char  sysvar_name[IPNET_RADVD_SYSVAR_NAME_LEN];
    char *prefix_list;
    char *p;

    ipcom_snprintf(sysvar_name, sizeof(sysvar_name),
                   "ipnet.inet6.radvd.%s.AdvPrefixList",
                   ifname);
    prefix_list = ipcom_sysvar_get(sysvar_name, IP_NULL, IP_NULL);
    if (prefix_list == IP_NULL)
        return;

    if (ipcom_str_in_list(id, prefix_list))
        goto cleanup;

    p = ipcom_realloc(prefix_list, ipcom_strlen(prefix_list)
                      + ipcom_strlen(id)
                      + 2);
    if (p == IP_NULL)
        goto cleanup;
    prefix_list = p;

    if (ipcom_strlen(prefix_list))
        ipcom_strcat(prefix_list, " ");
    ipcom_strcat(prefix_list, id);
    (void)ipcom_sysvar_set(sysvar_name, prefix_list, IPCOM_SYSVAR_FLAG_OVERWRITE);
 cleanup:
    ipcom_free(prefix_list);
}


/*
 *===========================================================================
 *                        ipnet_cmd_radvd_disable
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_radvd_disable(Ip_bool silent, char *ifname, char *id)
{
    char  sysvar_name[IPNET_RADVD_SYSVAR_NAME_LEN];
    char *prefix_list;
    char *id_in_list;

    ipcom_snprintf(sysvar_name, sizeof(sysvar_name),
                   "ipnet.inet6.radvd.%s.AdvPrefixList",
                   ifname);
    prefix_list = ipcom_sysvar_get(sysvar_name, IP_NULL, IP_NULL);
    if (prefix_list == IP_NULL)
        return;

    id_in_list = ipcom_str_in_list(id, prefix_list);
    if (id_in_list)
    {
        ipcom_memmove(id_in_list - (id_in_list != prefix_list),
                      id_in_list + strlen(id),
                      ipcom_strlen(id_in_list + strlen(id)) + 1);
    }
    (void)ipcom_sysvar_set(sysvar_name, prefix_list, IPCOM_SYSVAR_FLAG_OVERWRITE);
    ipcom_free(prefix_list);
}


/*
 *===========================================================================
 *                        ipnet_radvd_exists
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_radvd_exists(char *ifname, char *id)
{
    char    sysvar_name[IPNET_RADVD_SYSVAR_NAME_LEN];
    char   *val;
    Ip_bool exists;

    ipcom_snprintf(sysvar_name, sizeof(sysvar_name),
                   "ipnet.inet6.radvd.%s.prefix.%s",
                   ifname, id);
    val = ipcom_sysvar_get(sysvar_name, IP_NULL, IP_NULL);
    exists = (val != IP_NULL);
    ipcom_free(val);
    return exists;
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
 *                        ipnet_radvd_cmd
 *===========================================================================
 * Description: Configures the daemon.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipnet_cmd_radvd(int argc, char **argv)
{
    char *ifname;
    char *cmd_string;
    char *id;
    int   cmd;
    Ip_bool silent = IP_FALSE;
    int   ret      = 1;
#if IPCOM_VR_MAX > 1
    int  vr      = ipcom_proc_vr_get();
    int  vr_new  = vr;
#endif

    if (argc > 1)
    {
        if (ipcom_strcmp(argv[1], "-silent") == 0)
        {
            silent = IP_TRUE;
            argc--;
            argv++;
        }
    }

#if IPCOM_VR_MAX > 1
    if (argc > 2)
    {
        if (ipcom_strcmp(argv[1], "-V") == 0)
        {
            vr_new = ipcom_atoi(argv[2]);
            argc -= 2;
            argv += 2;
        }
    }

    if (vr != vr_new)
        ipcom_proc_vr_set(vr_new);
#endif

    if (argc < 4)
    {
        if (silent == IP_FALSE)
            ipnet_cmd_radvd_print_usage();
        goto error_out;
    }

    ifname = argv[1];
    cmd_string = argv[2];
    id = argv[3];
    argv += 4;
    argc -= 4;

    if (ipcom_if_nametoindex(ifname) == 0)
    {
        if (silent == IP_FALSE)
            ipcom_printf("Interface %s does not exist"IP_LF, ifname);
        goto error_out;
    }

    cmd = ipcom_cmd_str_to_key(ipnet_radvd_cmd_map, cmd_string);
    if (cmd == IPNET_RADVD_CMD_ADD)
    {
        if (ipnet_radvd_exists(ifname, id))
        {
            if (silent == IP_FALSE)
                ipcom_printf("%s already exist on interface %s"IP_LF, id, ifname);
            goto error_out;
        }
    }
    else if (ipnet_radvd_exists(ifname, id) == IP_FALSE)
    {
        if (silent == IP_FALSE)
            ipcom_printf("%s does not exist on interface %s"IP_LF, id, ifname);
        goto error_out;
    }

    switch (cmd)
    {
    case IPNET_RADVD_CMD_ADD:
        ipnet_cmd_radvd_add(silent, ifname, id, argc, argv);
        ret = 0;
        goto error_out;
    case IPNET_RADVD_CMD_DELETE:
        ipnet_cmd_radvd_disable(IP_TRUE, ifname, id);
        ipnet_cmd_radvd_delete(silent, ifname, id);
        break;
    case IPNET_RADVD_CMD_ENABLE:
        ipnet_cmd_radvd_enable(silent, ifname, id);
        break;
    case IPNET_RADVD_CMD_DISABLE:
        ipnet_cmd_radvd_disable(silent, ifname, id);
        break;
    default:
        if (silent == IP_FALSE)
            ipcom_printf("%s is an unknown command"IP_LF, cmd_string);
        goto error_out;
    }

    ret = ipcom_ipd_reconfigure(IPNET_RADVD_IPD_NAME);

error_out:
#if IPCOM_VR_MAX > 1
    if (vr != vr_new)
        ipcom_proc_vr_set(vr);
#endif
    return ret;
}
#endif /* IPCOM_USE_INET6 */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
