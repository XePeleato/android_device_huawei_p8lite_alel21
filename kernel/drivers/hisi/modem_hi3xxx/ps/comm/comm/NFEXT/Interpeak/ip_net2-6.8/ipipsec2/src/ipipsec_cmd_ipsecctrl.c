/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipipsec_cmd_ipsecctrl.c,v $ $Revision: 1.36.14.1 $
 *     $Source: /home/interpeak/CVSRoot/ipipsec2/src/ipipsec_cmd_ipsecctrl.c,v $
 *     $Author: ulf $ $Date: 2010-07-02 13:21:09 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1  DESCRIPTION.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#include "ipipsec_config.h"


/*
 ****************************************************************************
 * 3  INCLUDE FILES.
 ****************************************************************************
 */

/* IPCOM */
#define IPCOM_USE_CLIB_PROTO
#include <ipcom_list.h>
#include <ipcom_sock.h>
#include <ipcom_sock6.h>
#include <ipcom_inet.h>
#include <ipcom_err.h>
#include <ipcom_clib.h>
#include <ipcom_clib.h>
#include <ipcom_tags.h>

/* Interpeak IPsec */
#include "ipipsec.h"
#include "ipipsec_pfkeyv2.h"


/*
 ****************************************************************************
 * 4  MACROS/TYPES
 ****************************************************************************
 */

#define IPSECSATYPENAME(xtype)   ((xtype) == SADB_SATYPE_UNSPEC   ? "NONE" : \
                                  (xtype) == SADB_X_SATYPE_BYPASS ? "BYPS" : \
                                  (xtype) == SADB_SATYPE_AH       ? "AH  " : \
                                  (xtype) == SADB_SATYPE_ESP      ? "ESP " : \
			                      (xtype) == SADB_X_SATYPE_IPIP   ? "IPIP" : "????")

/*
 ****************************************************************************
 * 5  TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipsecctrl_arg_struct
 *===========================================================================
 * Used to pass more than one argument to the route print callback.
 */
typedef struct Ipsecctrl_arg_struct
{
    int  num;
}
Ipsecctrl_arg;


/*
 *===========================================================================
 *                         Ctrl
 *===========================================================================
 */
#define CTRL_GET   1
#define CTRL_SET   2
typedef struct Ipsecctrl_ctrl_struct
{
    char      *name;
    Ip_tag     id;
    Ip_u32     mode;        /* 1=get, 2=set, 3=get&set */
}
Ipsecctrl_ctrl;


/*
 *===========================================================================
 *                         Variables - Vars
 *===========================================================================
 */
typedef struct Argvars_struct
{
    int        argc;
    char     **argv;

    Ip_fd      fd;
}
Argvars;


/*
 *===========================================================================
 *                         Command
 *===========================================================================
 */
typedef struct Ipipsec_cmd_ipsecctrl_command_struct
{
    char     *name;
    int     (*func)(Argvars *vars);
}
Ipipsec_cmd_ipsecctrl_command;


/*
 *===========================================================================
 *                         Param
 *===========================================================================
 */
typedef struct Param_struct
{
    char        *name;
    unsigned int get;
    unsigned int set;
}
Param;


/*
 ****************************************************************************
 * 6  PROTOTYPES.
 ****************************************************************************
 */

IP_PUBLIC int ipipsec_cmd_ipsecctrl(int argc, char **argv);

static int ipsecctrl_flows(Argvars *vars);
static int ipsecctrl_sa(Argvars *vars);
static int ipsecctrl_mib(Argvars *vars);
static int ipsecctrl_get(Argvars *vars);
static int ipsecctrl_set(Argvars *vars);


/*
 ****************************************************************************
 * 7  DATA.
 ****************************************************************************
 */

static const Param params[] =
{
    { "enable_outfilter", IPIPSEC_CONF_GET_ENABLE_OUTFILTER, IPIPSEC_CONF_SET_ENABLE_OUTFILTER },
    { "enable_infilter", IPIPSEC_CONF_GET_ENABLE_INFILTER, IPIPSEC_CONF_SET_ENABLE_INFILTER },
    { "esp_enable", IPIPSEC_CONF_GET_ENABLE_ESP, IPIPSEC_CONF_SET_ENABLE_ESP },
    { "ah_enable", IPIPSEC_CONF_GET_ENABLE_AH, IPIPSEC_CONF_SET_ENABLE_AH },
    { "ipip_allow", IPIPSEC_CONF_GET_ALLOW_IPIP, IPIPSEC_CONF_SET_ALLOW_IPIP },
    { IP_NULL,      IP_NULL }
};

static const Ipipsec_cmd_ipsecctrl_command commands[] =
{
    /* name,       function */
    { "flows",     ipsecctrl_flows },
    { "sa",        ipsecctrl_sa },
    { "mib",       ipsecctrl_mib },
    { "get",       ipsecctrl_get },
    { "set",       ipsecctrl_set },
    { IP_NULL,      IP_NULL }
};

#define FLOW_TYPE(x)   ((x) > 6 ? "ILLEGAL" : flow_types[x])
static const char *flow_types[] =
{
    "invalid0",
    "use",
    "acquire",
    "require",
    "bypass",
    "deny",
    "dontacq"
};

/* Monitor mib vars */
static const char *mib_AuthAlg[] =
{
    "none",
    "md5 hmac",
    "sha1 hmac",
    "des mac",
    "keyed md5/sha",
    "ripemd160 hmac",
    "unknown",
    "unknown",
    "unknown",
    "aes xcbc mac"
};

static const char *mib_EncAlg[] =
{
    "none",
    "espDesIv64",
    "espDes",
    "esp3Des",
    "espRc5",
    "espIdea",
    "espCast",
    "espBlowfish",
    "esp3Idea",
    "espDesIv32",
    "espRc4(10)",
    "espNull",
    "espAesCbc",
    "espAesCtr",
    "unknown",
};

static const char *mib_Creator[] =
{
    "unknown",
    "static",
    "ike",
    "other"
};

static const char *mib_Encapsulation[] =
{
    "reserved",
    "tunnel",
    "transport"
};


/*
 ****************************************************************************
 * 8  STATIC FUNCTIONS.
 ****************************************************************************
 */


/*
 *===========================================================================
 *                         ipsecctrl_run_command
 *===========================================================================
 */
static int
ipsecctrl_run_command(char *name, Argvars *vars)
{
    int   i;

    if (ipcom_strcmp(name, "exit") == 0)
        ipcom_exit(1);

    for (i = 0; commands[i].name; i++)
        if (ipcom_strcmp(name, commands[i].name) == 0)
        {
            return commands[i].func(vars);
        }

    ipcom_printf( "<ipsecctrl> : unknown command '%s'."IP_LF, name);
    return -1;
}


/*
 *===========================================================================
 *                         ipsecctrl_flows
 *===========================================================================
 */
static int
ipsecctrl_flows(Argvars *vars)
{
    Ipipsec_ctrl_selector   sel;
    char   srcport[32], dstport[32];
    int    num = 0;
    char   str[IP_INET6_ADDRSTRLEN];
    char   str2[IP_INET6_ADDRSTRLEN+1];

    ipcom_printf("Flows:"IP_LF);

    sel.Index = 0;  /* get first selector. */

    /* Get and print the Selectors in MIB format. */
    for (;;)
    {
        /* Get first/next Selector */
        if (ipcom_socketioctl(vars->fd, IP_SIOCXIPSEC_SELECTOR, &sel) < 0)
        {
            ipcom_printf("ipsecctrl: ipcom_socketioctl(SELECTOR) failed (%s)"IP_LF,
                         ipcom_strerror(ipcom_errno));
            return -1;
        }

        if (sel.Index == 0)
            break;

        /* Print selector aka flow: */
        num++;
        ipcom_printf("[%ld] :: %s ", sel.Index, FLOW_TYPE(sel.flowtype));

        if (sel.flowpri != 0)
            ipcom_printf("pri=%d ", (signed char)sel.flowpri);

        /* source */
        if (sel.LocalPort != sel.LocalMaxPort)
            ipcom_sprintf(srcport, "%d-%d",
                          (int)ip_ntohs(sel.LocalPort), (int)ip_ntohs(sel.LocalMaxPort));
        else if (sel.LocalPort == 0)
            ipcom_strcpy(srcport, "any");
        else
            ipcom_sprintf(srcport, "%d", (int)ip_ntohs(sel.LocalPort));
        if (ipcom_memcmp(&sel.LocalId, &sel.LocalMaxId, sizeof(union Ip_in_addr_union)))
            ipcom_sprintf(str2, "-%s",
                          ipcom_inet_ntop(sel.domain, &sel.LocalMaxId, str, sizeof(str)));
        else
            *str2 = '\0';
        ipcom_printf("src=%s%s/%d:%s ",
                     ipcom_inet_ntop(sel.domain, &sel.LocalId, str, sizeof(str)),
                     str2,
                     ipipsec_addrmasklen(sel.domain, (Ip_u8 *)&sel.local_mask),
                     srcport);

        /* destination */
        if (sel.RemotePort != sel.RemoteMaxPort)
            ipcom_sprintf(dstport, "%d-%d",
                          (int)ip_ntohs(sel.RemotePort), (int)ip_ntohs(sel.RemoteMaxPort));
        else if (sel.RemotePort == 0)
            ipcom_strcpy(dstport, "any");
        else
            ipcom_sprintf(dstport, "%d", (int)ip_ntohs(sel.RemotePort));
        if (ipcom_memcmp(&sel.RemoteId, &sel.RemoteMaxId, sizeof(union Ip_in_addr_union)))
            ipcom_sprintf(str2, "-%s",
                          ipcom_inet_ntop(sel.domain, &sel.RemoteMaxId, str, sizeof(str)));
        else
            *str2 = '\0';
        ipcom_printf("dst=%s%s/%d:%s ",
                     ipcom_inet_ntop(sel.domain, &sel.RemoteId, str, sizeof(str)),
                     str2,
                     ipipsec_addrmasklen(sel.domain, (Ip_u8 *)&sel.remote_mask),
                     dstport);

        ipcom_printf("%s", ipcom_ipproto_name(sel.Protocol));
        switch (sel.Protocol)
        {
        case IP_IPPROTO_ICMP:
        case IP_IPPROTO_ICMPV6:
            if (sel.ports[0] || sel.ports[1])
                ipcom_printf(" type=%d code=%d", sel.ports[0], sel.ports[1]);
            break;
        case IP_IPPROTO_MH:
            if (sel.ports[0])
                ipcom_printf(" type=%d", sel.ports[0]);
            break;
        default:
            break;
        }

        /* Print SA */
        if (sel.sa.domain)
        {
            if (sel.sa.domain == IPIPSEC_AF_BYPASS)
                ipcom_printf(" %s SA: %s",
                             sel.direction == IPIPSEC_SADIR_INPUT ? "<-" :
                             sel.direction == IPIPSEC_SADIR_OUTPUT ? "->" : "--",
                             IPSECSATYPENAME(sel.sa.satype));
            else
                ipcom_printf(" %s SA: %s spi=0x%lx src=%s dst=%s",
                             sel.direction == IPIPSEC_SADIR_INPUT ? "<-" :
                             sel.direction == IPIPSEC_SADIR_OUTPUT ? "->" : "--",
                             IPSECSATYPENAME(sel.sa.satype),
                             ip_ntohl(sel.sa.spi_n),
                             ipcom_inet_ntop(sel.sa.domain, &sel.sa.src, str, sizeof(str)),
                             ipcom_inet_ntop(sel.sa.domain, &sel.sa.dst, str2, sizeof(str2)));
        }
        else
            ipcom_printf(" %s SA: none",
                         sel.direction == IPIPSEC_SADIR_INPUT ? "<-" :
                         sel.direction == IPIPSEC_SADIR_OUTPUT ? "->" : "--");
        ipcom_printf(IP_LF);
    }
    ipcom_printf("Total of %d flows."IP_LF, num);

    return 0;
}


/*
 *===========================================================================
 *                         ipsecctrl_sa
 *===========================================================================
 */
static int
ipsecctrl_sa(Argvars *vars)
{
    Ipipsec_ioctl_sactrl   sa_next;
    Ipipsec_ioctl_sactrl   sa_group;
    int       num;
    Ip_err    retval;
    Ip_tag    tags[7];
#ifdef IPCOM_USE_INET6
    char str[IP_INET6_ADDRSTRLEN];
    char str2[IP_INET6_ADDRSTRLEN];
#else
    char str[16], str2[16];
#endif

    /* Print all SAs. */
    ipcom_printf("SA - Security Associations:"IP_LF);

    sa_next.arg.next.spi_n = 0;  /* get first SA. */

    for (num = 0;;)
    {
        ipcom_memcpy(&sa_next.sa, &sa_next.arg.next, sizeof(Ipipsec_sa_handle));
        sa_next.arg.next.direction = IPIPSEC_SADIR_ANY;
        sa_next.arg.next.protocol  = 0;
        sa_next.arg.next.domain    = 0;

        /* Get first/next Selector */
        if (ipcom_socketioctl(vars->fd, IP_SIOCXIPSEC_NEXT_SA, &sa_next) < 0)
        {
            ipcom_printf("ipsecctrl: ipcom_socketioctl(NEXT_SA) failed (%s)"IP_LF,
                         ipcom_strerror(ipcom_errno));
            return -1;
        }

        if (sa_next.arg.next.spi_n == 0)
            break;

        /* Get additional information. */
        tags[0] = IPIPSEC_SACTRLT_GET_AuthAlg;
        tags[2] = IPIPSEC_SACTRLT_GET_EncAlg;
        tags[4] = IP_TAG_END;
        retval = ipipsec_sactrl(&sa_next.arg.next, tags);
        if (retval != IPCOM_SUCCESS)
        {
            ipcom_printf("ipsecctrl: ipipsec_sactrl(info) failed, error = %d"IP_LF, retval);
            return -1;
        }
        ipcom_printf("[%2d] ::  %-6s %s spi=0x%-5lx src=%-17s dst=%-17s auth=%s",
                     ++num,
                     sa_next.arg.next.direction == IPIPSEC_SADIR_INPUT ? "input" :
                     sa_next.arg.next.direction == IPIPSEC_SADIR_OUTPUT ? "output" : "any",
                     ipcom_ipproto_name(sa_next.arg.next.protocol),
                     ip_ntohl(sa_next.arg.next.spi_n),
                     ipcom_inet_ntop(sa_next.arg.next.domain, &sa_next.arg.next.src, str, sizeof(str)),
                     ipcom_inet_ntop(sa_next.arg.next.domain, &sa_next.arg.next.dst, str2, sizeof(str2)),
                     mib_AuthAlg[tags[1]]);


        if (sa_next.arg.next.protocol == IP_IPPROTO_ESP)
            ipcom_printf(" enc=%s", mib_EncAlg[tags[3]]);
        if (sa_next.arg.next.priority != 0)
            ipcom_printf(" pri=%d", (int)sa_next.arg.next.priority);
        if (sa_next.arg.next.dscp != 0)
            ipcom_printf(" DSCP=%d", (int)sa_next.arg.next.dscp);
        ipcom_printf(IP_LF);

        /* Get grouped info */
        ipcom_memcpy(&sa_group.sa, &sa_next.arg.next, sizeof(Ipipsec_sa_handle));
        if (ipcom_socketioctl(vars->fd, IP_SIOCXIPSEC_Grouped, &sa_group) < 0)
        {
            ipcom_printf("ipsecctrl: ipcom_socketioctl(Grouped) failed (%s)"IP_LF,
                         ipcom_strerror(ipcom_errno));
            return -1;
        }
        if (sa_group.arg.group.spi_n) /*lint !e530 */
        {
            ipcom_printf("           ->   %s spi=0x%-5lx src=%-17s dst=%-17s",
                         ipcom_ipproto_name(sa_group.arg.group.protocol),
                         ip_ntohl(sa_group.arg.group.spi_n),
                         ipcom_inet_ntop(sa_next.arg.next.domain, &sa_next.arg.next.src, str, sizeof(str)),
                         ipcom_inet_ntop(sa_group.arg.group.domain, &sa_next.arg.next.dst, str2, sizeof(str2)));
            if (sa_group.arg.group.priority != 0)
                ipcom_printf(" pri=%d", (int)sa_group.arg.group.priority);
            if (sa_group.arg.group.dscp != 0)
                ipcom_printf(" DSCP=%d", (int)sa_group.arg.group.dscp);
            ipcom_printf(IP_LF);
        }
    }

    ipcom_printf("Total of %d SAs."IP_LF, num);
    return 0;
}


/*
 *===========================================================================
 *                    ipsecctrl_trap_print
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPIPSEC_USE_TRAPS
static void
ipsecctrl_trap_print(void)
{
    Ip_tag  tags[17];
    Ip_err retval;

    ipcom_printf(IP_LF"***** IPsec trap control ::"IP_LF);

    tags[ 0] = IPIPSEC_CTRLT_GET_espAuthFailureTrapEnable;
    tags[ 2] = IPIPSEC_CTRLT_GET_ahAuthFailureTrapEnable;
    tags[ 4] = IPIPSEC_CTRLT_GET_espReplayFailureTrapEnable;
    tags[ 6] = IPIPSEC_CTRLT_GET_ahReplayFailureTrapEnable;
    tags[ 8] = IPIPSEC_CTRLT_GET_espPolicyFailureTrapEnable;
    tags[10] = IPIPSEC_CTRLT_GET_ahPolicyFailureTrapEnable;
    tags[12] = IPIPSEC_CTRLT_GET_invalidSpiTrapEnable;
    tags[14] = IPIPSEC_CTRLT_GET_otherPolicyFailureTrapEnable;
    tags[16] = IP_TAG_END;

    retval = ipipsec_ctrl(tags);
    if (retval != IPCOM_SUCCESS)
        ipcom_printf("error: trap print aborted: error = %d"IP_LF, retval);
    else
    {
        ipcom_printf("espAuthFailureTrapEnable = %ld"IP_LF, tags[ 1]);
        ipcom_printf("ahAuthFailureTrapEnable = %ld"IP_LF, tags[ 3]);
        ipcom_printf("espReplayFailureTrapEnable = %ld"IP_LF, tags[ 5]);
        ipcom_printf("ahReplayFailureTrapEnable = %ld"IP_LF, tags[ 7]);
        ipcom_printf("espPolicyFailureTrapEnable = %ld"IP_LF, tags[ 9]);
        ipcom_printf("ahPolicyFailureTrapEnable = %ld"IP_LF, tags[11]);
        ipcom_printf("invalidSpiTrapEnable = %ld"IP_LF, tags[13]);
        ipcom_printf("otherPolicyFailureTrapEnable = %ld"IP_LF, tags[15]);
    }
}
#endif /* IPIPSEC_USE_TRAPS */


/*
 *===========================================================================
 *                    ipsecctrl_stats_print
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static void
ipsecctrl_stats_print(void)
{
    Ip_tag  tags[31];
    Ip_err retval;

    ipcom_printf(IP_LF"***** entity IPsec statistics ::"IP_LF);

    tags[ 0] = IPIPSEC_CTRLT_GET_ipsecEspCurrentInboundSAs;
    tags[ 2] = IPIPSEC_CTRLT_GET_ipsecEspTotalInboundSAs;
    tags[ 4] = IPIPSEC_CTRLT_GET_ipsecEspCurrentOutboundSAs;
    tags[ 6] = IPIPSEC_CTRLT_GET_ipsecEspTotalOutboundSAs;
    tags[ 8] = IPIPSEC_CTRLT_GET_ipsecAhCurrentInboundSAs;
    tags[10] = IPIPSEC_CTRLT_GET_ipsecAhTotalInboundSAs;
    tags[12] = IPIPSEC_CTRLT_GET_ipsecAhCurrentOutboundSAs;
    tags[14] = IPIPSEC_CTRLT_GET_ipsecAhTotalOutboundSAs;
    tags[16] = IPIPSEC_CTRLT_GET_ipsecDecryptionErrors;
    tags[18] = IPIPSEC_CTRLT_GET_ipsecAuthenticationErrors;
    tags[20] = IPIPSEC_CTRLT_GET_ipsecReplayErrors;
    tags[22] = IPIPSEC_CTRLT_GET_ipsecPolicyErrors;
    tags[24] = IPIPSEC_CTRLT_GET_ipsecOtherReceiveErrors;
    tags[26] = IPIPSEC_CTRLT_GET_ipsecSendErrors;
    tags[28] = IPIPSEC_CTRLT_GET_ipsecUnknownSpiErrors;
    tags[30] = IP_TAG_END;

    retval = ipipsec_ctrl(tags);
    if (retval != IPCOM_SUCCESS)
        ipcom_printf("error: Statistics print aborted: error = %d"IP_LF, retval);
    else
    {
        ipcom_printf("ipsecEspCurrentInboundSAs = %ld"IP_LF, tags[ 1]);
        ipcom_printf("ipsecEspTotalInboundSAs = %ld"IP_LF, tags[ 3]);
        ipcom_printf("ipsecEspCurrentOutboundSAs = %ld"IP_LF, tags[ 5]);
        ipcom_printf("ipsecEspTotalOutboundSAs = %ld"IP_LF, tags[ 7]);
        ipcom_printf("ipsecAhCurrentInboundSAs = %ld"IP_LF, tags[ 9]);
        ipcom_printf("ipsecAhTotalInboundSAs = %ld"IP_LF, tags[11]);
        ipcom_printf("ipsecAhCurrentOutboundSAs = %ld"IP_LF, tags[13]);
        ipcom_printf("ipsecAhTotalOutboundSAs = %ld"IP_LF, tags[15]);
        ipcom_printf(IP_LF"***** IPsec error counts ::"IP_LF);
        ipcom_printf("ipsecDecryptionErrors = %ld"IP_LF, tags[17]);
        ipcom_printf("ipsecAuthenticationErrors = %ld"IP_LF, tags[19]);
        ipcom_printf("ipsecReplayErrors = %ld"IP_LF, tags[21]);
        ipcom_printf("ipsecPolicyErrors = %ld"IP_LF, tags[23]);
        ipcom_printf("ipsecOtherReceiveErrors = %ld"IP_LF, tags[25]);
        ipcom_printf("ipsecSendErrors = %ld"IP_LF, tags[27]);
        ipcom_printf("ipsecUnknownSpiErrors = %ld"IP_LF, tags[29]);
    }
}


/*
 *===========================================================================
 *                    ipsecctrl_selectorTable_print
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static void
ipsecctrl_selectorTable_print(Argvars *vars)
{
    Ipipsec_ctrl_selector   sel;
#ifdef IPCOM_USE_INET6
    char str[IP_INET6_ADDRSTRLEN];
#else
    char str[16];
#endif

    ipcom_printf(IP_LF"***** selectorTable ::"IP_LF);

    sel.Index = 0;  /* get first selector. */

    /* Get and print the Selectors in MIB format. */
    for (;;)
    {
        /* Get first/next Selector */
        if (ipcom_socketioctl(vars->fd, IP_SIOCXIPSEC_SELECTOR, &sel) < 0)
        {
            ipcom_printf("ipsecctrl: ipcom_socketioctl(SELECTOR) failed (%s)"IP_LF,
                         ipcom_strerror(ipcom_errno));
            return;
        }

        if (sel.Index == 0)
            break;

        /* Print selector index */
        ipcom_printf(IP_LF"Selector Entry #%ld:"IP_LF
                     "selectorIndex = %ld"IP_LF,
                     sel.Index,
                     sel.Index);

        ipcom_printf("selectorLocalId = %s"IP_LF
                     "selectorLocalIdType = %s"IP_LF,
                     ipcom_inet_ntop(sel.domain, &sel.LocalId, str, sizeof(str)),
                     sel.domain == IP_AF_INET ? "idIpv4Addr" : "idIpv6Addr");


        ipcom_printf("selectorRemoteId = %s"IP_LF
                     "selectorRemoteIdType = %s"IP_LF,
                     ipcom_inet_ntop(sel.domain, &sel.RemoteId, str, sizeof(str)),
                     sel.domain == IP_AF_INET ? "idIpv4Addr" : "idIpv6Addr");

        ipcom_printf("selectorProtocol = %s"IP_LF,
                     ipcom_ipproto_name(sel.Protocol));

        ipcom_printf("selectorLocalPort = %d"IP_LF,
                     ip_ntohs(sel.LocalPort));
        ipcom_printf("selectorRemotePort = %d"IP_LF,
                     ip_ntohs(sel.RemotePort));
    }
}


/*
 *===========================================================================
 *                    ipsecctrl_saTables_print
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#define IPIPSEC_MAX_U32_TAGS   24
#define IPIPSEC_MAX_U64_TAGS   4

static void
ipsecctrl_saTables_print(Argvars *vars, Ip_u8 direction, Ip_u8 protocol, int domain)
{
    Ipipsec_ioctl_sactrl   sa_next;
    Ipipsec_ioctl_sactrl   sa_u64;
    Ip_err   retval;
    int  i, num;
    Ip_tag tags[41];
    char table[32];
#ifdef IPCOM_USE_INET6
    char str[IP_INET6_ADDRSTRLEN];
#else
    char str[16];
#endif

    /* pre-init print stuff. */
    ipcom_sprintf(table, "ipsecSa%s%s",
                  protocol == IP_IPPROTO_AH ? "Ah" : "Esp",
                  direction == IPIPSEC_SADIR_INPUT ? "In" :
                  direction == IPIPSEC_SADIR_OUTPUT ? "Out" :
                  "Any");

    /* Fill in the info tags. */
    tags[0] = IPIPSEC_SACTRLT_GET_SaSelector;
    tags[2] = IPIPSEC_SACTRLT_GET_SaCreator;
    tags[4] = IPIPSEC_SACTRLT_GET_Encapsulation;
    tags[6] = IPIPSEC_SACTRLT_GET_EncAlg;
    tags[8] = IPIPSEC_SACTRLT_GET_EncKeyLength;
    tags[10] = IPIPSEC_SACTRLT_GET_AuthAlg;
    tags[12] = IPIPSEC_SACTRLT_GET_AuthKeyLength;
    tags[14] = IPIPSEC_SACTRLT_GET_RepWinSize;
    tags[16] = IPIPSEC_SACTRLT_GET_LimitSeconds;
    tags[18] = IPIPSEC_SACTRLT_GET_LimitKbytes;
    tags[20] = IPIPSEC_SACTRLT_GET_AccSeconds;
    tags[22] = IPIPSEC_SACTRLT_GET_InAccKbytes;
    tags[24] = IPIPSEC_SACTRLT_GET_DecryptErrors;
    tags[26] = IPIPSEC_SACTRLT_GET_AuthErrors;
    tags[28] = IPIPSEC_SACTRLT_GET_ReplayErrors;
    tags[30] = IPIPSEC_SACTRLT_GET_PolicyErrors;
    tags[32] = IPIPSEC_SACTRLT_GET_PadErrors;
    tags[34] = IPIPSEC_SACTRLT_GET_OtherReceiveErrors;
    tags[36] = IPIPSEC_SACTRLT_GET_OutAccKbytes;
    tags[38] = IPIPSEC_SACTRLT_GET_OutSendErrors;
    tags[40] = IP_TAG_END;

    ipcom_printf(IP_LF"***** %stable ::"IP_LF, table);

    /* Get and print the SAs in MIB format. */
    sa_next.arg.next.spi_n = 0;

    for (num = 0;;)
    {
        ipcom_memcpy(&sa_next.sa, &sa_next.arg.next, sizeof(Ipipsec_sa_handle));
        sa_next.arg.next.direction = direction;
        sa_next.arg.next.protocol  = protocol;
        sa_next.arg.next.domain    = (Ip_u8)domain;
        if (ipcom_socketioctl(vars->fd, IP_SIOCXIPSEC_NEXT_SA, &sa_next) < 0)
        {
            ipcom_printf("ipsecctrl: ipcom_socketioctl(NEXT_SA) failed (%s)"IP_LF,
                         ipcom_strerror(ipcom_errno));
            return;
        }

        if (sa_next.arg.next.spi_n == 0)
            break;

        retval = ipipsec_sactrl(&sa_next.arg.next, tags);
        if (retval != IPCOM_SUCCESS)
        {
            ipcom_printf("ipsecctrl: ipipsec_sactrl(info) failed, error = %d"IP_LF, retval);
            return;
        }

        /* SA id */
        ipcom_printf(IP_LF"%sEntry #%d:"IP_LF
                     "%sAddresstype = %d"IP_LF
                     "%sAddress = %s"IP_LF
                     "%sSpi = 0x%lx"IP_LF
                     , table, num
                     , table, domain == IP_AF_INET ? 1 : domain == IP_AF_INET6 ? 2 : 3
                     , table, ipcom_inet_ntop(sa_next.arg.next.domain, &sa_next.arg.next.dst, str, sizeof(str))
                     , table, ip_ntohl(sa_next.arg.next.spi_n));

        /* SA MIB entries */
        for (i = 0; tags[i] != IP_TAG_END; i += 2)
        {
            switch(tags[i])
            {
            case IPIPSEC_SACTRLT_GET_SaSelector :
                ipcom_printf("%sSelector = %ld"IP_LF, table, tags[i+1]);
                break;

            case IPIPSEC_SACTRLT_GET_SaCreator :
                ipcom_printf("%sCreator = %s"IP_LF, table, mib_Creator[tags[i+1]]);
                break;

            case IPIPSEC_SACTRLT_GET_Encapsulation :
                ipcom_printf("%sEncapsulation = %s"IP_LF, table, mib_Encapsulation[tags[i+1]]);
                break;

            case IPIPSEC_SACTRLT_GET_EncAlg :
                ipcom_printf("%sEncAlg = %s"IP_LF, table, mib_EncAlg[tags[i+1]]);
                break;

            case IPIPSEC_SACTRLT_GET_EncKeyLength :
                ipcom_printf("%sEncKeyLength = %ld"IP_LF, table, tags[i+1]);
                break;

            case IPIPSEC_SACTRLT_GET_AuthAlg :
                ipcom_printf("%sAuthAlg = %s"IP_LF, table, mib_AuthAlg[tags[i+1]]);
                break;

            case IPIPSEC_SACTRLT_GET_AuthKeyLength :
                ipcom_printf("%sAuthKeyLength = %ld"IP_LF, table, tags[i+1]);
                break;

            case IPIPSEC_SACTRLT_GET_RepWinSize :
                ipcom_printf("%sRepWinSize = %ld"IP_LF, table, tags[i+1]);
                break;

            case IPIPSEC_SACTRLT_GET_LimitSeconds :
                ipcom_printf("%sLimitSeconds = %ld"IP_LF, table, tags[i+1]);
                break;

            case IPIPSEC_SACTRLT_GET_LimitKbytes :
                ipcom_printf("%sLimitKbytes = %ld"IP_LF, table, tags[i+1]);
                break;

            case IPIPSEC_SACTRLT_GET_AccSeconds :
                ipcom_printf("%sAccSeconds = %ld"IP_LF, table, tags[i+1]);
                break;

            case IPIPSEC_SACTRLT_GET_InAccKbytes :
                ipcom_printf("%sInAccKbytes = %ld"IP_LF, table, tags[i+1]);
                break;

            case IPIPSEC_SACTRLT_GET_OutAccKbytes :
                ipcom_printf("%sOutAccKbytes = %ld"IP_LF, table, tags[i+1]);
                break;

            case IPIPSEC_SACTRLT_GET_DecryptErrors :
                ipcom_printf("%sDecryptErrors = %ld"IP_LF, table, tags[i+1]);
                break;

            case IPIPSEC_SACTRLT_GET_AuthErrors :
                ipcom_printf("%sAuthErrors = %ld"IP_LF, table, tags[i+1]);
                break;

            case IPIPSEC_SACTRLT_GET_ReplayErrors :
                ipcom_printf("%sReplayErrors = %ld"IP_LF, table, tags[i+1]);
                break;

            case IPIPSEC_SACTRLT_GET_PolicyErrors :
                ipcom_printf("%sPolicyErrors = %ld"IP_LF, table, tags[i+1]);
                break;

            case IPIPSEC_SACTRLT_GET_PadErrors :
                ipcom_printf("%sPadErrors = %ld"IP_LF, table, tags[i+1]);
                break;

            case IPIPSEC_SACTRLT_GET_OtherReceiveErrors :
                ipcom_printf("%sOtherReceiveErrors = %ld"IP_LF, table, tags[i+1]);
                break;

            case IPIPSEC_SACTRLT_GET_OutSendErrors :
                ipcom_printf("%sOutSendErrors = %ld"IP_LF, table, tags[i+1]);
                break;

            default:
                ipcom_printf("error: unknown tag type '%ld'"IP_LF, tags[i]);
                break;
            }
        } /* for */

        ipcom_memcpy(&sa_u64.sa, &sa_next.arg.next, sizeof(Ipipsec_sa_handle));
        if (ipcom_socketioctl(vars->fd, IP_SIOCXIPSEC_InUserOctets, &sa_u64) == 0)
            ipcom_printf("%sInUserOctets = %ld"IP_LF, table, IP_U64_GETLO(sa_u64.arg.u64)); /*lint !e530 */
        if (ipcom_socketioctl(vars->fd, IP_SIOCXIPSEC_InUserPackets, &sa_u64) == 0)
            ipcom_printf("%sInUserPackets = %ld"IP_LF, table, IP_U64_GETLO(sa_u64.arg.u64)); /*lint !e530 */
        if (ipcom_socketioctl(vars->fd, IP_SIOCXIPSEC_OutUserOctets, &sa_u64) == 0)
            ipcom_printf("%sOutUserOctets = %ld"IP_LF, table, IP_U64_GETLO(sa_u64.arg.u64)); /*lint !e530 */
        if (ipcom_socketioctl(vars->fd, IP_SIOCXIPSEC_OutUserPackets, &sa_u64) == 0)
            ipcom_printf("%sOutUserPackets = %ld"IP_LF, table, IP_U64_GETLO(sa_u64.arg.u64)); /*lint !e530 */
    }
}


/*
 *===========================================================================
 *                         ipsecctrl_mib
 *===========================================================================
 * List the IPSec Monitoring MIB (draft-ietf-ipsec-monitor-mib-03.txt).
 *
 * ESP Input table.
 * AH Input table.
 * ESP output table.
 * AH output table.
 */
static int
ipsecctrl_mib(Argvars *vars)
{
    (void)vars;

    ipcom_printf("draft-ietf-ipsec-monitor-mib-03.txt - IPSec Monitoring MIB:"IP_LF);

    ipsecctrl_selectorTable_print(vars);

#ifdef IPCOM_USE_INET
    ipsecctrl_saTables_print(vars, IPIPSEC_SADIR_INPUT, IP_IPPROTO_ESP, IP_AF_INET);
    ipsecctrl_saTables_print(vars, IPIPSEC_SADIR_INPUT, IP_IPPROTO_AH, IP_AF_INET);
    ipsecctrl_saTables_print(vars, IPIPSEC_SADIR_OUTPUT, IP_IPPROTO_ESP, IP_AF_INET);
    ipsecctrl_saTables_print(vars, IPIPSEC_SADIR_OUTPUT, IP_IPPROTO_AH, IP_AF_INET);
#endif

#ifdef IPCOM_USE_INET6
    ipsecctrl_saTables_print(vars, IPIPSEC_SADIR_INPUT, IP_IPPROTO_ESP, IP_AF_INET6);
    ipsecctrl_saTables_print(vars, IPIPSEC_SADIR_INPUT, IP_IPPROTO_AH, IP_AF_INET6);
    ipsecctrl_saTables_print(vars, IPIPSEC_SADIR_OUTPUT, IP_IPPROTO_ESP, IP_AF_INET6);
    ipsecctrl_saTables_print(vars, IPIPSEC_SADIR_OUTPUT, IP_IPPROTO_AH, IP_AF_INET6);
#endif

    ipsecctrl_stats_print();

#ifdef IPIPSEC_USE_TRAPS
    ipsecctrl_trap_print();
#endif

    return 0;
}


/*
 *===========================================================================
 *                         ipsecctrl_get
 *===========================================================================
 */
static int
ipsecctrl_get(Argvars *vars)
{
    int i;
    Ip_tag  tags[3];

    (void)vars;

    ipcom_printf("IPIPSEC configuration variables:"IP_LF);

    for (i = 0; params[i].name != IP_NULL; i++)
    {
        tags[0] = params[i].get;
        tags[1] = 0;
        tags[2] = IP_TAG_END;
        ipipsec_conf(tags);

        ipcom_printf("%s = %d"IP_LF, params[i].name, tags[1]);
    }

    return 0;
}


/*
 *===========================================================================
 *                         ipsecctrl_set
 *===========================================================================
 */
static int
ipsecctrl_set(Argvars *vars)
{
    int     i;
    Ip_tag  tags[3];

    if (vars->argc != 4)
    {
        ipcom_printf("usage: > ipsecctrl set <name> <integer value>"IP_LF);
        return -1;
    }

    for (i = 0; params[i].name != IP_NULL; i++)
        if (ipcom_strcmp(vars->argv[2], params[i].name) == 0)
        {
            tags[0] = params[i].set;
            tags[1] = ipcom_atoi(vars->argv[3]);
            tags[2] = IP_TAG_END;
            ipipsec_conf(tags);
            ipcom_printf("ipsecctrl: %s set to %d"IP_LF, params[i].name, tags[1]);
            return 0;
        }

    ipcom_printf("ipsecctrl set: '%s' not found"IP_LF, vars->argv[2]);
    return -1;
}


/*
 ****************************************************************************
 * 9  PUBLIC FUNCTIONS.
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipipsec_cmd_ipsecctrl
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipipsec_cmd_ipsecctrl(int argc, char **argv)
{
    Argvars   vars;
    int       ret;

    if (argc < 2)
        goto usage;

    vars.fd = ipcom_socket(IP_PF_KEY, IP_SOCK_RAW, PF_KEY_V2);
    if (vars.fd < 0)
    {
        ipcom_printf("ipsecctrl: ipcom_socket() failed (%s)"IP_LF,
                     ipcom_strerror(ipcom_errno));
        return -1;
    }

    vars.argc = argc;
    vars.argv = argv;

    ret = ipsecctrl_run_command(argv[1], &vars);
    ipcom_socketclose(vars.fd);

    if (ret == 0)
        return 0;

 usage:
    ipcom_printf(
                 "IPsec control command, version 2.0"IP_LF
                 "Copyright (c) 2000-2009, Interpeak AB <www.interpeak.com>"IP_LF
                 "usage: ipsecctrl <command>"IP_LF
                 "   Commands:"IP_LF
                 "\tflows\t\t  list all IPsec output flows"IP_LF
                 "\tsa\t\t  list all kernel Security Associations"IP_LF
                 "\tmib\t\t  print IPSec monitor MIB"IP_LF
                 "\tget\t\t  get IPSec configuration values"IP_LF
                 "\tset <name> <val>  set IPSec configuration values"IP_LF
                 );

    return 0;
}


/*
 ****************************************************************************
 *                         END OF FILE
 ****************************************************************************
 */

