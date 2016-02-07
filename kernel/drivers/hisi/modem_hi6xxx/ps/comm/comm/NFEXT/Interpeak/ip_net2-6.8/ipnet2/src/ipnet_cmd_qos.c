/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_cmd_qos.c,v $ $Revision: 1.22 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_cmd_qos.c,v $
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

#include "ipnet.h"
#include "ipnet_policy_routing.h"
#ifdef IPCOM_USE_INET6
#include "ipnet_ip6.h"
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Operations */
#define IPNET_CMD_QOS_OP_ADD  1 /* Add something */
#define IPNET_CMD_QOS_OP_DEL  2 /* Delete somthing */
#define IPNET_CMD_QOS_OP_GET  3 /* Get something */
#define IPNET_CMD_QOS_OP_LIST 4 /* List something */

/* Operation types */
#define IPNET_CMD_QOS_T_POLICY_RT 1 /* Policy routing */

/* Errors */
#define IPNET_CMD_ERR                -1
#define IPNET_CMD_ERR_UNSUPPORTED_OP -2

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipnet_cmd_qos
 *===========================================================================
 */
typedef struct Ipnet_cmd_qos_struct
{
    int sock;
    int op;         /* Operation IPNET_CMD_QOS_OP_xxx */
    int op_type;    /* Operation type IPNET_CMD_QOS_T_xxx */
    int domain;     /* Domain the rule applies to (IP_AF_INET | IP_AF_INET6) */
    union
    {
        struct Ipnet_policy_routing_rule rule;
    } d;
}
Ipnet_cmd_qos;


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

IP_STATIC Ipcom_cmd_int_str_map ipnet_cmd_qos_ops[] =
{
    { IPNET_CMD_QOS_OP_ADD, "add"    },
    { IPNET_CMD_QOS_OP_DEL, "del"    },
    { IPNET_CMD_QOS_OP_DEL, "delete" },
    { IPNET_CMD_QOS_OP_GET, "get"    },
    { IPNET_CMD_QOS_OP_LIST,"list"   },
    { -1, IP_NULL                    }
};

IP_STATIC Ipcom_cmd_int_str_map ipnet_cmd_qos_pktflags[] =
{
    { IPCOM_PKT_FLAG_TUNNELED,  "tunneled"  },
    { IPCOM_PKT_FLAG_FORWARDED, "forwarded" },
    { IPCOM_PKT_FLAG_IPV6,      "ipv6"      },
    { IPCOM_PKT_FLAG_IPV4,      "ipv4"      },
    { IPCOM_PKT_FLAG_FRAGMENT,  "frag"      },
    { IPCOM_PKT_FLAG_MORE_FRAG, "mf"        },
    { IPCOM_PKT_FLAG_MULTICAST, "multicast" },
    { IPCOM_PKT_FLAG_BROADCAST, "broadcast" },
    { -1, IP_NULL                           }
};

#ifdef IPCOM_USE_INET6
IP_STATIC Ipcom_cmd_int_str_map ipnet_cmd_qos_scope[] =
{
    { IPNET_IP6_SCOPE_GLOBAL,     "global" },
    { IPNET_IP6_SCOPE_NODE_LOCAL, "node"   },
    { IPNET_IP6_SCOPE_LINK_LOCAL, "link"   },
    { IPNET_IP6_SCOPE_SITE_LOCAL, "site"   },
    { IPNET_IP6_SCOPE_ORG_LOCAL,  "org"    },
    { -1, IP_NULL                          }
};
#endif /* IPCOM_USE_INET6 */

IP_STATIC Ipcom_cmd_int_str_map ipnet_cmd_qos_types[] =
{
    { IPNET_CMD_QOS_T_POLICY_RT, "policy" },
    { -1, IP_NULL                         }
};

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_cmd_qos_print_policy_rule
 *===========================================================================
 * Description: Prints a policy rule to the standard output.
 * Parameters:  rule - Rule to print.
 * Returns:
 *
 */
IP_STATIC void
ipcom_cmd_qos_print_policy_rule(struct Ipnet_policy_routing_rule *rule)
{
    char str[48];

    ipcom_printf("Policy routing rule id %u prio %d:"IP_LF, (unsigned)rule->id, (int)rule->prio);
    ipcom_printf(" %-10s: %-10s"IP_LF, "domain", rule->af == IP_AF_INET ? "INET" : "INET6");
    ipcom_printf(" %-10s: %-10u"IP_LF, "table", (unsigned) rule->table);
    if (IP_BIT_ISSET(rule->mask, IPNET_PR_RULE_RTKEYSADDR))
        ipcom_printf(" %-10s: %-10s"IP_LF, "lookup", "reverse");
    if (IP_BIT_ISSET(rule->mask, IPNET_PR_RULE_DS))
        ipcom_printf(" %-10s: %-10u"IP_LF, rule->af == IP_AF_INET ? "tos" : "tclass", rule->ds);
    if (IP_BIT_ISSET(rule->mask, IPNET_PR_RULE_PROTO))
        ipcom_printf(" %-10s: %-10u"IP_LF, "proto", rule->proto);
    if (IP_BIT_ISSET(rule->mask, IPNET_PR_RULE_FLOW))
        ipcom_printf(" %-10s: %-10u"IP_LF, "flow", (unsigned)rule->flow);
    if (IP_BIT_ISSET(rule->mask, IPNET_PR_RULE_IFINDEX))
        ipcom_printf(" %-10s: %-10u"IP_LF, "ifindex", (unsigned)rule->ifindex);
#ifdef IPCOM_USE_INET6
    if (IP_BIT_ISSET(rule->mask, IPNET_PR_RULE_SCOPE))
        ipcom_printf(" %-10s: %-10s"IP_LF, "scope",
                     ipcom_cmd_key_to_str(ipnet_cmd_qos_scope, rule->scope, str, sizeof(str)));
#endif /* IPCOM_USE_INET6 */
    if (IP_BIT_ISSET(rule->mask, IPNET_PR_RULE_SADDR))
        ipcom_printf(" %-10s: %-10s/%d"IP_LF,
                     "from",
                     ipcom_inet_ntop(rule->af, &rule->saddr, str, sizeof(str)),
                     rule->saddr_prefixlen);
    if (IP_BIT_ISSET(rule->mask, IPNET_PR_RULE_DADDR))
        ipcom_printf(" %-10s: %-10s/%d"IP_LF,
                     "to",
                     ipcom_inet_ntop(rule->af, &rule->daddr, str, sizeof(str)),
                     rule->daddr_prefixlen);

    if (IP_BIT_ISSET(rule->mask, IPNET_PR_RULE_PKTFLAGS))
    {
        Ip_u32 i32      = 1;
        Ip_u32 found    = 0;

        ipcom_printf(" %-10s: ", "flags");
        /* Loop 'til we're no more */
        for (; i32 != 0; i32 <<= 1)
        {
            /* Is this particular bit set? */
            if (IP_BIT_ISSET(rule->pkt_mask, i32))
            {
                ipcom_printf("%s%s%s",
                             found == 0? "" : ",",
                             IP_BIT_ISSET(rule->pkt_result, i32)? "" : "!",
                             ipcom_cmd_key_to_str(ipnet_cmd_qos_pktflags,
                                                  i32,
                                                  str,
                                                  sizeof(str)));
                found++;
            }
        }

        /* Must terminate */
        ipcom_printf(IP_LF);
    }
    ipcom_printf(" %-10s: %-10s"IP_LF, "last", rule->last ? "TRUE" : "FALSE");

    /* Line break */
    ipcom_printf(IP_LF);
}


/*
 *===========================================================================
 *                    ipnet_cmd_qos_policy_routing
 *===========================================================================
 * Description: Handles the policy subcommand.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qos_policy_routing(Ipnet_cmd_qos *cmd, char **argv)
{
    IP_STATIC Ipcom_cmd_int_str_map ipnet_cmd_qos_args[] =
        {
            /* Indexes <10 has must have one integer argument
               >=10 to <100 has one argument that needs custom argument parsing
               >=100 have no argument */
            { 1,   "proto"   },
            { 2,   "tos"     },
            { 2,   "tclass"  },
            { 3,   "flow"    },
            { 4,   "table"   },
            { 5,   "prio"    },
            { 7,   "ifindex" },
            { 8,   "id"      },
            { 8,   "index"   }, /* Let 'index' be and alias for 'id' since
                                   the documentation said index instead of id */
            { 10,  "from"    },
#ifdef IPCOM_USE_INET6
            { 11,  "scope"   },
#endif
            { 12,  "to"      },
            { 13,  "flags"   },
            { 100, "last"    },
            { 101, "reverse" },
            { -1, IP_NULL    }
        };
    int    c;
    int    i = 0;
    char  *p = IP_NULL;
    char  *str_prefixlen;
    Ip_u8  prefix_len;

    cmd->d.rule.af    = (Ip_u8) cmd->domain;
    cmd->d.rule.id    = IPNET_PR_ID_NONE;
    cmd->d.rule.table = IPCOM_ROUTE_TABLE_DEFAULT;

    while (*argv != IP_NULL)
    {
        c = ipcom_cmd_str_to_key(ipnet_cmd_qos_args, *argv);
        if (c == -1)
        {
            ipcom_printf("'%s' is an unknown argument"IP_LF, *argv);
            return IPNET_CMD_ERR;
        }
        if (c < 100)
        {
            if (argv[1] == IP_NULL)
            {
                ipcom_printf("The argument %s requires an parameter"IP_LF, argv[0]);
                return IPNET_CMD_ERR;
            }
            argv++;
            if (c < 10)
                i = ipcom_atoi(*argv);
            else
                p = *argv;
        }

        switch (c)
        {
        case 1:
            IP_BIT_SET(cmd->d.rule.mask, IPNET_PR_RULE_PROTO);
            cmd->d.rule.proto = (Ip_u8) i;
            break;
        case 2:
            IP_BIT_SET(cmd->d.rule.mask, IPNET_PR_RULE_DS);
            cmd->d.rule.ds = (Ip_u8) i;
            break;
        case 3:
            IP_BIT_SET(cmd->d.rule.mask, IPNET_PR_RULE_FLOW);
            cmd->d.rule.flow = (Ip_u32) i;
            break;
        case 4:
            cmd->d.rule.table = (Ip_u32) i;
            break;
        case 5:
            cmd->d.rule.prio = (Ip_u16) i;
            break;
        case 7:
            IP_BIT_SET(cmd->d.rule.mask, IPNET_PR_RULE_IFINDEX);
            cmd->d.rule.ifindex = (Ip_u32) i;
            break;
        case 8:
            cmd->d.rule.id = (Ip_u32) i;
            break;
        case 10:
        case 12:
            str_prefixlen = ipcom_strstr(p, "/");
            if (str_prefixlen == IP_NULL || ipcom_strlen(p) <= 1)
                prefix_len = 0;
            else
            {
                *str_prefixlen = '\0';
                prefix_len = (Ip_u8) ipcom_atoi(str_prefixlen + 1);
            }
            if (c == 10)
            {
                if (ipcom_inet_pton(cmd->domain, p, &cmd->d.rule.saddr) != 1)
                {
                    ipcom_printf("'%s' is not an valid IP address" IP_LF, p);
                    return IPNET_CMD_ERR;
                }
                IP_BIT_SET(cmd->d.rule.mask, IPNET_PR_RULE_SADDR);
                cmd->d.rule.saddr_prefixlen = prefix_len;
            }
            else
            {
                if (ipcom_inet_pton(cmd->domain, p, &cmd->d.rule.daddr) != 1)
                {
                    ipcom_printf("'%s' is not an valid IP address" IP_LF, p);
                    return IPNET_CMD_ERR;
                }
                IP_BIT_SET(cmd->d.rule.mask, IPNET_PR_RULE_DADDR);
                cmd->d.rule.daddr_prefixlen = prefix_len;
            }
            break;
#ifdef IPCOM_USE_INET6
        case 11:
            IP_BIT_SET(cmd->d.rule.mask, IPNET_PR_RULE_SCOPE);
            if (ipcom_cmd_str_to_key(ipnet_cmd_qos_scope, p) < 0)
            {
                ipcom_printf("'%s' is not one of global,link,site,node or org"IP_LF, p);
                return IPNET_CMD_ERR;
            }
            cmd->d.rule.scope = (Ip_u32) ipcom_cmd_str_to_key(ipnet_cmd_qos_scope, p);
            break;
#endif /* IPCOM_USE_INET6 */
        case 13:
            {
                char *ptr, *optr;

                optr = ptr = ipcom_strdup(p);
                if (optr == IP_NULL)
                {
                    ipcom_printf("Failed to duplicate string '%s': Out of memory"IP_LF, p);
                    return IPNET_CMD_ERR;
                }

                IP_BIT_SET(cmd->d.rule.mask, IPNET_PR_RULE_PKTFLAGS);

                for (;;)
                {
                    int     flag;
                    Ip_bool not_op = IP_FALSE;
                    char   *key;

                    key = ipcom_strtok_r(ptr == optr ? ptr : IP_NULL, ",", &ptr);
                    if (key == IP_NULL)
                        break;

                    /* Resolve potential negations */
                    if (key[0] == '!')
                    {
                        key++;
                        not_op = IP_TRUE;
                    }

                    flag = ipcom_cmd_str_to_key(ipnet_cmd_qos_pktflags, key);
                    if (flag == -1)
                    {
                        ipcom_printf("'%s' is not a valid flags string"IP_LF, key);
                        ipcom_free(optr);
                        return IPNET_CMD_ERR;
                    }

                    IP_BIT_SET(cmd->d.rule.pkt_mask, flag);
                    if (!not_op)
                        /* It's required, set in both */
                        IP_BIT_SET(cmd->d.rule.pkt_result, flag);
                }
                ipcom_free(optr);
            }
            break;
        case 100:
            cmd->d.rule.last = IP_TRUE;
            break;
        case 101:
            IP_BIT_SET(cmd->d.rule.mask, IPNET_PR_RULE_RTKEYSADDR);
            break;
        default:
            IP_PANIC();
            break;
        }

        argv++;
    }

    switch (cmd->op)
    {
    case IPNET_CMD_QOS_OP_ADD:
        return ipcom_socketioctl(cmd->sock, IPNET_SIOCSPRRULE, &cmd->d.rule);
    case IPNET_CMD_QOS_OP_DEL:
        return ipcom_socketioctl(cmd->sock, IPNET_SIOCDPRRULE, &cmd->d.rule);
    case IPNET_CMD_QOS_OP_GET:
        if (ipcom_socketioctl(cmd->sock, IPNET_SIOCGPRRULE, &cmd->d.rule) < 0)
            ipcom_cmd_qos_print_policy_rule(&cmd->d.rule);
        else
            ipcom_perror("Failed to get policy routing rule");
        return 0;
    case IPNET_CMD_QOS_OP_LIST:
        while (ipcom_socketioctl(cmd->sock, IPNET_SIOCEPRRULE, &cmd->d.rule) == 0)
            ipcom_cmd_qos_print_policy_rule(&cmd->d.rule);
        return 0;

    default:
        return IPNET_CMD_ERR_UNSUPPORTED_OP;
    }
}


/*
 *===========================================================================
 *                    ipnet_cmd_qos_print_usage
 *===========================================================================
 * Description: Prints the usage for the command.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_qos_print_usage(void)
{
    ipcom_printf("NAME" IP_LF);
    ipcom_printf("     qos - controls the QoS features in IPNET" IP_LF);
    ipcom_printf("" IP_LF);
    ipcom_printf("SYNOPSIS" IP_LF);
    ipcom_printf("     qos [option]... op optype [args]" IP_LF);
    ipcom_printf("" IP_LF);
    ipcom_printf("DESCRIPTION" IP_LF);
    ipcom_printf("     The qos command is used to configure and view the Quality of Service" IP_LF);
    ipcom_printf("     features within IPNET, except interface queues which uses 'qc'." IP_LF);
    ipcom_printf("" IP_LF);
    ipcom_printf("     The following options:" IP_LF);
    ipcom_printf("     -4          Use IPv4 as address domain (default)." IP_LF);
    ipcom_printf("     -6          Use IPv6 as address domain." IP_LF);
    ipcom_printf("     -V <tab>    Operate on virtual router with index <tab>." IP_LF);
    ipcom_printf("" IP_LF);
    ipcom_printf("     The following operations (op) exists:" IP_LF);
    ipcom_printf("     add         Add something." IP_LF);
    ipcom_printf("     delete      Delete something." IP_LF);
    ipcom_printf("     get         Lookup and display something." IP_LF);
    ipcom_printf("     list        Lookup and display something." IP_LF);
    ipcom_printf("" IP_LF);
    ipcom_printf("     The following operation types (optype) exists:" IP_LF);
    ipcom_printf("     policy      Policy routing." IP_LF);
    ipcom_printf("" IP_LF);
    ipcom_printf("     Policy routing arguments:" IP_LF);
    ipcom_printf("         Add operaion:" IP_LF);
    ipcom_printf("     from <addr> Apply this rule if the source address is <addr>" IP_LF);
    ipcom_printf("     to <addr>   Apply this rule if the destination address is <addr>" IP_LF);
    ipcom_printf("     proto <p>   Apply this rule if the transport proto is <p>" IP_LF);
    ipcom_printf("     tos <n>     Apply this rule if the TOS is <n>" IP_LF);
    ipcom_printf("     tclass <n>  Apply this rule if the traffic class is <n>" IP_LF);
    ipcom_printf("     flow <f>    Apply this rule if the traffic flow is <f>" IP_LF);
    ipcom_printf("     ifindex <if> Apply this rule if the ifindex of the incoming interface is <if>" IP_LF);
    ipcom_printf("     flags <f>   Apply this rule if the packet has the appropriate flags"IP_LF);
    ipcom_printf("                 set or unset. The flags are comma separated, and to require"IP_LF);
    ipcom_printf("                 a flag to be not set, it should be prepended with a '!'. Existing"IP_LF);
    ipcom_printf("                 flags are: forwarded,multicast,tunneled,broadcast,fragment,mf"IP_LF);
    ipcom_printf("                 ipv4,ipv6"IP_LF);
#ifdef IPCOM_USE_INET6
    ipcom_printf("     scope <s>   Apply this rule if the scope of the address is <s>" IP_LF);
#endif
    ipcom_printf("     reverse     If this rule is applied, do the lookup based on src instead of dst" IP_LF);
    ipcom_printf("     table <t>   Use the routing table with policy ID <t>" IP_LF);
    ipcom_printf("                 if the rule matches" IP_LF);
    ipcom_printf("     last        Do not look at any more rules, even if no match" IP_LF);
    ipcom_printf("                 on this table" IP_LF);
    ipcom_printf("     prio <p>    The prio of the rule to add [-32768..32767]" IP_LF);
    ipcom_printf("         Delete/get operation:" IP_LF);
    ipcom_printf("     id <i>      The ID of the rule to delete/show" IP_LF);
    ipcom_printf("" IP_LF);
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
 *                    ipnet_cmd_qos
 *===========================================================================
 * Description: The entry point for the 'qos' shell command.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipnet_cmd_qos(int argc, char **argv)
{
    Ipnet_cmd_qos cmd;
    Ipcom_getopt  opt;
    int           i;
    char        **arg;
    int           ret = IPNET_CMD_ERR;
    char         *op;
    char         *op_type;
    int           vr;

    ipcom_memset(&cmd, 0, sizeof(cmd));
    cmd.domain = IP_AF_INET;

    cmd.sock = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, 0);
    if (cmd.sock == IP_INVALID_SOCKET)
    {
        ipcom_perror("Failed to create socket");
        goto cleanup;
    }

    ipcom_getopt_clear_r(&opt);
    while ((i = ipcom_getopt_r(argc, argv, "46V:", &opt)) != -1)
    {
        switch (i)
        {
        case '4':
            cmd.domain = IP_AF_INET;
            break;
        case '6':
            cmd.domain = IP_AF_INET6;
            break;
        case 'V':
            vr = ipcom_atoi(opt.optarg);
            if (ipcom_setsockopt(cmd.sock, IP_SOL_SOCKET, IP_SO_X_VR, &vr, sizeof(vr)) < 0)
            {
                ipcom_perror("Failed to changed VR");
                goto cleanup;
            }
            break;
        default:
            ipcom_printf("invalid option '%c'"IP_LF, i);
            goto cleanup;
        }
    }

    if (argc < opt.optind + 2)
    {
        ipnet_cmd_qos_print_usage();
        goto cleanup;
    }

    arg = &argv[opt.optind];
    op = *arg;
    cmd.op = ipcom_cmd_str_to_key(ipnet_cmd_qos_ops, op);
    if (cmd.op == -1)
    {
        ipcom_printf("'%s' is an invalid operation"IP_LF, op);
        goto cleanup;
    }
    arg++;
    op_type = *arg;
    cmd.op_type = ipcom_cmd_str_to_key(ipnet_cmd_qos_types, op_type);
    arg++;
    switch (cmd.op_type)
    {
    case IPNET_CMD_QOS_T_POLICY_RT:
        ret = ipnet_cmd_qos_policy_routing(&cmd, arg);
        break;
    default:
        ipcom_printf("'%s' is an invalid operation type"IP_LF, *arg);
        break;
    }

    if (ret == IPNET_CMD_ERR_UNSUPPORTED_OP)
        ipcom_printf("The operation %s is not supported by type %s" IP_LF, op, op_type);

 cleanup:
    ipcom_socketclose(cmd.sock);
    return ret;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

