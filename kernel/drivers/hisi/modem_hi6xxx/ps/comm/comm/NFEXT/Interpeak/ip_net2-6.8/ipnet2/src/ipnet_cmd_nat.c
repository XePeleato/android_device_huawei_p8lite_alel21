/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_cmd_nat.c,v $ $Revision: 1.83 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_cmd_nat.c,v $
 *     $Author: ulf $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * The NAT shell command.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifdef IPCOM_KERNEL
#undef IPCOM_KERNEL
#endif
#if defined(IPNET) || defined(IP_PORT_WIN32)
#include "ipnet_config.h"
#else
#define IPNET_USE_NAT
#endif /* #ifdef IPNET */

#ifdef IPNET_USE_NAT

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

/* Include IPCOM header files when building for RTOS without memory protection */
#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_getopt.h>
#include <ipcom_sock.h>
#include <ipcom_sock2.h>
#include <ipcom_inet.h>
#include <ipcom_strlib.h>
#include <ipcom_err.h>
#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_cmd.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Available options to the NAT shell command */
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
#define IPNET_NAT_CMD_OPTIONS  "frnpslmCDEFVZ"
#else
#define IPNET_NAT_CMD_OPTIONS  "rnpslmCDEFVZ"
#endif

#include "ipnet_nat.h"
#include "ipnet_nat_h.h"


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
 * 9                   LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_nat_print_version
 *===========================================================================
 * Description: Print the version of the NAT.
 * Parameters:  none.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_print_version(void)
{
    /* Print the NAT version */
    ipcom_printf("@(#) IPNET_NAT $Name: VXWORKS_ITER18A_FRZ10 $ - INTERPEAK_COPYRIGHT_STRING");
    ipcom_printf(IP_LF);
}


/*
 *===========================================================================
 *                    ipnet_nat_print_usage
 *===========================================================================
 * Description: Print usage information for the NAT shell command.
 * Parameters:  none.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_print_usage(void)
{
    ipcom_printf(IP_LF);
    ipcom_printf("NAME"IP_LF);
    ipcom_printf("     nat - NAT (Network Address Translation) configuration."IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("SYNOPSIS"IP_LF);
    ipcom_printf("     nat [-silent] [-"IPNET_NAT_CMD_OPTIONS"] [rule or filename]"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("DESCRIPTION"IP_LF);
    ipcom_printf("     nat configures NAT rules. Default operation is to add"IP_LF);
    ipcom_printf("     a rule. Use the -r flag to remove a rule. nat can also be used"IP_LF);
    ipcom_printf("     to display or flush the rule and mappings tables."IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     -silent      No stdout ouput"IP_LF);
    ipcom_printf("     -r           Remove rule"IP_LF);
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
    ipcom_printf("     -f           Get rules from file"IP_LF);
#endif
    ipcom_printf("     -n           Do not change the rule table, just check rule syntax"IP_LF);
    ipcom_printf("     -l           Display NAT rules"IP_LF);
    ipcom_printf("     -m           Display NAT mappings"IP_LF);
    ipcom_printf("     -p           Display application proxies"IP_LF);
    ipcom_printf("     -s           Display statistics"IP_LF);
    ipcom_printf("     -D           Disable NAT (active mappings will be removed)"IP_LF);
    ipcom_printf("     -E           Enable NAT"IP_LF);
    ipcom_printf("     -F           Flush rule table (active mappings will be removed)"IP_LF);
    ipcom_printf("     -C           Clear active mappings"IP_LF);
    ipcom_printf("     -V           Show NAT version"IP_LF);
    ipcom_printf("     -Z           Clear statistics"IP_LF);
    ipcom_printf("NAT RULE SYNTAX"IP_LF);
    ipcom_printf("     [@<index>] <map|map-block|pt|pt-block> <interface> <addr[/mask]> -> <addr[/mask]>"IP_LF);
    ipcom_printf("     [[portmap <tcp|udp|tcp/udp> <val>:<val>] |"IP_LF);
    ipcom_printf("      [icmpidmap icmp <val>:<val>] |"IP_LF);
    ipcom_printf("      [proxy port <val> <proxyname/protocol> [nonapt]]]"IP_LF);
    ipcom_printf("NAT REDIRECT RULE SYNTAX"IP_LF);
    ipcom_printf("     [@<index>] rdr <interface> <addr[/mask]> port <val> -> <addr> port <val>"IP_LF);
    ipcom_printf("     [tcp/udp/ip/protocol] [proxy <proxyname>]"IP_LF);
}


/*
 *===========================================================================
 *                    ipnet_nat_print_rules
 *===========================================================================
 * Description: Print current NAT ruleset.
 * Parameters:  none.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_print_rules(void)
{
    struct Ipnet_nat_rule_inner *nat_rule_info;
    void *mask, *addr;
    Ip_s32 family, i = 0;
	char buf[64];
    char protostr[16];
    struct Ip_servent *se;
    Ip_socklen_t len;
    Ipnet_nat_ctrl *ctrl = IP_NULL;
    Ip_fd sock;

    /* Create a socket */
    sock = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, 0);
    if(sock == IP_INVALID_SOCKET)
        goto exit;

    /* Allocate memory for the getsockopt call */
    len = sizeof(*ctrl);
    ctrl = ipcom_malloc(len);
    if(ctrl == IP_NULL)
        goto exit;

    /* Get list of rules */
    ipcom_printf("NAT RULE TABLE:"IP_LF);
    ctrl->cmd = IPNET_NAT_CTRL_GET_RULE;
    ctrl->seqno = i;
    while(ipcom_getsockopt(sock, IP_IPPROTO_IP, IP_IP_NAT, ctrl, &len) == 0
        && len == sizeof(*ctrl))
    {
        /* Get the rule and prepare for next */
        nat_rule_info = &ctrl->type.rule_info;
        ctrl->seqno = ++i;
        ip_assert(i <= IPNET_NAT_MAX_RULES);

        /* Print the index */
	    ipcom_printf("@%d ", (int)i);

        /* Print the map keyword */
        if(nat_rule_info->map == 1)
	        ipcom_printf("map ");
        else if(nat_rule_info->map_block == 1)
	        ipcom_printf("map-block ");
        else if(nat_rule_info->rdr == 1)
	        ipcom_printf("rdr ");
        else if(nat_rule_info->pt == 1)
	        ipcom_printf("pt ");
        else if(nat_rule_info->pt_block == 1)
	        ipcom_printf("pt-block ");
        else
        {
            IP_PANIC();
            break;
        }

        /* Print the interface */
        ipcom_printf("%s ", nat_rule_info->ifname);

        /* Print the address and mask */
        if(nat_rule_info->map == 1 || nat_rule_info->map_block == 1)
        {
            addr = &nat_rule_info->src_n;
            mask = &nat_rule_info->src_mask_n;
            family = IP_AF_INET;
        }
        else if(nat_rule_info->rdr == 1)
        {
            addr = &nat_rule_info->dst_n;
            mask = &nat_rule_info->dst_mask_n;
            family = IP_AF_INET;
        }
        else if(nat_rule_info->pt == 1 || nat_rule_info->pt_block == 1)
        {
#ifdef IPNET_USE_NAT_PT
            addr = &nat_rule_info->src_n_pt;
            mask = &nat_rule_info->src_mask_n_pt;
            family = IP_AF_INET6;
#else
            IP_PANIC();
            break;
#endif
        }
        else
        {
            IP_PANIC();
            break;
        }

        if(family == IP_AF_INET && *(Ip_u32 *)addr == 0)
        {
            buf[0] = '0';
            buf[1] = '\0';
        }
        else if(ipcom_inet_ntop(family, addr, buf, sizeof(buf)) == IP_NULL)
        {
            IP_PANIC();
            break;
        }
        ipcom_printf("%s", buf);
        ipcom_printf("/%d ", ipcom_mask_to_prefixlen(mask, family == IP_AF_INET ? 32 : 128));

        if(nat_rule_info->rdr == 1)
        {
            /* Print the port */
            ipcom_printf("port %d ", ip_ntohs(nat_rule_info->rdr_old_port_n));
        }

        ipcom_printf("-> ");

        if(nat_rule_info->map == 1 || nat_rule_info->map_block == 1 || nat_rule_info->rdr == 1)
        {
            addr = &nat_rule_info->nat_addr_n;
            mask = &nat_rule_info->nat_mask_n;
            family = IP_AF_INET;
        }
        else if(nat_rule_info->pt == 1 || nat_rule_info->pt_block == 1)
        {
            addr = &nat_rule_info->nat_addr_n;
            mask = &nat_rule_info->nat_mask_n;
            family = IP_AF_INET;
        }

        /* Print the NAT address and mask */
        if(family == IP_AF_INET && *(Ip_u32 *)addr == 0)
        {
            buf[0] = '0';
            buf[1] = '\0';
        }
        else if(ipcom_inet_ntop(family, addr, buf, sizeof(buf)) == IP_NULL)
        {
            IP_PANIC();
            break;
        }
        ipcom_printf("%s", buf);
        ipcom_printf("/%d ", ipcom_mask_to_prefixlen(mask, family == IP_AF_INET ? 32 : 128));

        if(nat_rule_info->rdr == 1)
        {
            /* Print the port and protocol */
            if(nat_rule_info->rdr_tcpudp == 1)
                ipcom_strcpy(protostr, "tcp/udp");
            else
                (void)ipnet_nat_protocol_name(nat_rule_info->rdr_protocol, protostr, sizeof(protostr));
            ipcom_printf("port %d %s ", ip_ntohs(nat_rule_info->rdr_new_port_n), protostr);
        }

        /* Print the optional portmap */
        if(nat_rule_info->portmap == 1)
        {
	        ipcom_printf("portmap ");
            if(nat_rule_info->portmap_tcpudp == 1)
    	        ipcom_printf("tcp/udp ");
            else
                ipcom_printf("%s ", ipnet_nat_protocol_name(nat_rule_info->portmap_protocol, protostr, sizeof(protostr)));
            ipcom_printf("%d:%d", nat_rule_info->port_lo, nat_rule_info->port_hi);
        }
        else if(nat_rule_info->icmpidmap == 1)
        {
	        ipcom_printf("icmpidmap icmp ");
            ipcom_printf("%d:%d", nat_rule_info->port_lo, nat_rule_info->port_hi);
        }
        else if(nat_rule_info->proxy == 1)
        {
            if (nat_rule_info->rdr == 1)
            {
                ipcom_printf("proxy %s", nat_rule_info->proxy_label);
            }
            else
            {
                ipcom_printf("proxy port ");
                se = ipcom_getservbyport(ip_htons(nat_rule_info->proxy_trigger), IP_NULL);
                if(se != IP_NULL)
                    ipcom_printf("%s ", se->s_name);
                else
                    ipcom_printf("%d ", nat_rule_info->proxy_trigger);
                ipcom_printf("%s/%s",
                            nat_rule_info->proxy_label,
                            ipnet_nat_protocol_name(nat_rule_info->proxy_protocol, protostr, sizeof(protostr)));
                if (nat_rule_info->proxy_nonapt == 1)
                    ipcom_printf(" nonapt");
            }
        }
        ipcom_printf(IP_LF);
    }

    if(i > 0)
        ipcom_printf(IP_LF);

exit:
    if(ctrl != IP_NULL)
        ipcom_free(ctrl);
    if(sock != IP_INVALID_SOCKET)
        ipcom_socketclose(sock);
}


/*
 *===========================================================================
 *                    ipnet_nat_print_mappings
 *===========================================================================
 * Description: Print active NAT mappings.
 * Parameters:  none.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_print_mappings(void)
{
    struct Ipnet_nat_mapping_inner *nat_mapping_info;
    char src[64];
    char nat_addr[IP_INET_ADDRSTRLEN];
    char dst[IP_INET_ADDRSTRLEN];
    char protostr[16];
    char state[8];
    Ip_u16 sport, dport, nat_sport;
	Ip_s32 i = 0;
    Ip_u32 currsec;
    Ip_socklen_t len;
    Ipnet_nat_ctrl *ctrl = IP_NULL;
    Ip_fd sock;
#ifdef IP_ASSERT
	Ip_s32 maxmap;
#endif
    /* Create a socket */
    sock = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, 0);
    if(sock == IP_INVALID_SOCKET)
        goto exit;

    /* Allocate memory for the getsockopt call */
    len = sizeof(*ctrl);
    ctrl = ipcom_malloc(len);
    if(ctrl == IP_NULL)
        goto exit;

    /* Get NAT information */
    ctrl->cmd = IPNET_NAT_CTRL_GET_INFO;
    if(ipcom_getsockopt(sock, IP_IPPROTO_IP, IP_IP_NAT, ctrl, &len) != 0
        || len != sizeof(*ctrl))
    {
        goto exit;
    }

    /* Store the ipnet time and the maximum number of mappings */
    currsec = ctrl->type.info.second;
#ifdef IP_ASSERT
    maxmap = ctrl->type.info.max_mappings;
#endif

    /* Get list of mappings */
    ipcom_printf("NAT MAPPINGS TABLE:"IP_LF);
    ctrl->cmd = IPNET_NAT_CTRL_GET_MAPPING;
    ctrl->seqno = i;
    while(ipcom_getsockopt(sock, IP_IPPROTO_IP, IP_IP_NAT, ctrl, &len) == 0
        && len == sizeof(*ctrl))
    {
        /* Get the mapping and prepare for next */
        nat_mapping_info = &ctrl->type.mapping_info;
        ctrl->seqno = ++i;
        ip_assert(i <= maxmap);

        /* Presentation format of the addresses */
        ipcom_inet_ntop(IP_AF_INET, &nat_mapping_info->nat_addr_n, nat_addr, sizeof(nat_addr));
        ipcom_inet_ntop(IP_AF_INET, &nat_mapping_info->tuple.dst_n, dst, sizeof(dst));
        if(nat_mapping_info->pt == 0)
        {
            ipcom_inet_ntop(IP_AF_INET, &nat_mapping_info->tuple.src_n, src, sizeof(src));
        }
        else
        {
#ifdef IPNET_USE_NAT_PT
            ipcom_inet_ntop(IP_AF_INET6, &nat_mapping_info->tuple.pt_src, src, sizeof(src));
#else
            IP_PANIC();
            break;
#endif
        }

        /* Store the inner state */
        if(nat_mapping_info->inner_state == IPNET_NAT_MAPPING_CLO)
            ipcom_strcpy(state, "CLO/");
        else if(nat_mapping_info->inner_state == IPNET_NAT_MAPPING_SYN)
            ipcom_strcpy(state, "SYN/");
        else if(nat_mapping_info->inner_state == IPNET_NAT_MAPPING_EST)
            ipcom_strcpy(state, "EST/");
        else if(nat_mapping_info->inner_state == IPNET_NAT_MAPPING_FIN)
            ipcom_strcpy(state, "FIN/");
        else
            ipcom_strcpy(state, "NUL/");

        /* Store the outer state */
        if(nat_mapping_info->outer_state == IPNET_NAT_MAPPING_CLO)
            ipcom_strcpy(&state[4], "CLO");
        else if(nat_mapping_info->outer_state == IPNET_NAT_MAPPING_SYN)
            ipcom_strcpy(&state[4], "SYN");
        else if(nat_mapping_info->outer_state == IPNET_NAT_MAPPING_EST)
            ipcom_strcpy(&state[4], "EST");
        else if(nat_mapping_info->outer_state == IPNET_NAT_MAPPING_FIN)
            ipcom_strcpy(&state[4], "FIN");
        else
            ipcom_strcpy(&state[4], "NUL");

        /* Print the address and ports */
        if(nat_mapping_info->tuple.protocol == IP_IPPROTO_UDP
            || nat_mapping_info->tuple.protocol == IP_IPPROTO_TCP
            || (nat_mapping_info->tuple.protocol == IP_IPPROTO_ICMP
                && IPNET_NAT_IS_ICMP_REQUEST(nat_mapping_info->tuple.icmp_type)))
        {
            sport = ip_ntohs(nat_mapping_info->tuple.src_port_n);
            dport = ip_ntohs(nat_mapping_info->tuple.dst_port_n);
            nat_sport = ip_ntohs(nat_mapping_info->nat_port_n);
            ipcom_printf("%s:%d -> %s:%d proto %s, (%s:%d), ",
                    src, sport, dst, dport, ipnet_nat_protocol_name(nat_mapping_info->tuple.protocol, protostr, sizeof(protostr)),
                    nat_addr, nat_sport);
        }
        else
        {
            ipcom_printf("%s -> %s proto %s, (%s), ",
                    src, dst, ipnet_nat_protocol_name(nat_mapping_info->tuple.protocol, protostr, sizeof(protostr)), nat_addr);
        }

        /* Print the states and timeout */
        ipcom_printf("state %s, expire in %lu s"IP_LF,
                     state, nat_mapping_info->tmo_abs_sec - currsec);

        /* Print the TCP sequence numbers */
        if(nat_mapping_info->tuple.protocol == IP_IPPROTO_TCP)
            ipcom_printf("seq_start=%u, curr_delta=%d, prev_delta=%d"IP_LF,
                                 (unsigned) nat_mapping_info->seq_start,
                                 (int) nat_mapping_info->curr_delta,
                                 (int) nat_mapping_info->prev_delta);
	}

    if(i > 0)
        ipcom_printf(IP_LF);

exit:
    if(ctrl != IP_NULL)
        ipcom_free(ctrl);
    if(sock != IP_INVALID_SOCKET)
        ipcom_socketclose(sock);

}


/*
 *===========================================================================
 *                    ipnet_nat_print_proxies
 *===========================================================================
 * Description: Print registered proxies.
 * Parameters:  none.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_print_proxies(void)
{
    struct Ipnet_nat_proxy_inner *nat_proxy_info;
	Ip_s32 i = 0;
    Ip_socklen_t len;
    Ipnet_nat_ctrl *ctrl = IP_NULL;
    Ip_fd sock;
    char protostr[16];

    /* Create a socket */
    sock = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, 0);
    if(sock == IP_INVALID_SOCKET)
        goto exit;

    /* Allocate memory for the getsockopt call */
    len = sizeof(*ctrl);
    ctrl = ipcom_malloc(len);
    if(ctrl == IP_NULL)
        goto exit;

    /* Get list of proxies */
    ipcom_printf("NAT APPLICATION PROXIES:"IP_LF);
    ctrl->cmd = IPNET_NAT_CTRL_GET_PROXY;
    ctrl->seqno = i;
    while(ipcom_getsockopt(sock, IP_IPPROTO_IP, IP_IP_NAT, ctrl, &len) == 0
        && len == sizeof(*ctrl))
    {
        /* Get the proxy and prepare for next */
        nat_proxy_info = &ctrl->type.proxy_info;
        ctrl->seqno = ++i;
        ip_assert(i <= IPNET_NAT_MAX_PROXIES);
        ipcom_printf("label=%-16s protocol=%-4s"IP_LF,
                      nat_proxy_info->label,
                     ipnet_nat_protocol_name(nat_proxy_info->protocol, protostr, sizeof(protostr)));
    }

    if(i > 0)
        ipcom_printf(IP_LF);

exit:
    if(ctrl != IP_NULL)
        ipcom_free(ctrl);
    if(sock != IP_INVALID_SOCKET)
        ipcom_socketclose(sock);
}


/*
 *===========================================================================
 *                    ipnet_nat_print_stats
 *===========================================================================
 * Description: Print NAT statistics.
 * Parameters:  none.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_print_stats(void)
{
    Ipnet_nat_ctrl *ctrl = IP_NULL;
    Ip_socklen_t len;
    Ip_fd sock;

    /* Create a socket */
    sock = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, 0);
    if(sock == IP_INVALID_SOCKET)
        goto exit;

    /* Allocate memory for the getsockopt call */
    len = sizeof(*ctrl);
    ctrl = ipcom_malloc(len);
    if(ctrl == IP_NULL)
        goto exit;

    /* Get NAT information */
    ctrl->cmd = IPNET_NAT_CTRL_GET_INFO;
    if(ipcom_getsockopt(sock, IP_IPPROTO_IP, IP_IP_NAT, ctrl, &len) != 0
        || len != sizeof(*ctrl))
    {
        goto exit;
    }

    /* Print NAT statistics */
    ipcom_printf("NAT STATISTICS:"IP_LF);
    ipcom_printf("translated:                in %u out %u"IP_LF
                 "nomatch:                   in %u out %u"IP_LF
                 "invalid:                   in %u out %u"IP_LF
                 "dropped:                   in %u out %u"IP_LF
                 "added mappings:            %u"IP_LF
                 "expired mappings:          %u"IP_LF
                 "failed mappings:           %u"IP_LF,
                 (unsigned) ctrl->type.info.stats.translated_in,
                 (unsigned) ctrl->type.info.stats.translated_out,
                 (unsigned) ctrl->type.info.stats.no_rule_match_in,
                 (unsigned) ctrl->type.info.stats.no_rule_match_out,
                 (unsigned) ctrl->type.info.stats.invalid_in,
                 (unsigned) ctrl->type.info.stats.invalid_out,
                 (unsigned) ctrl->type.info.stats.dropped_in,
                 (unsigned) ctrl->type.info.stats.dropped_out,
                 (unsigned) ctrl->type.info.stats.mappings_added,
                 (unsigned) ctrl->type.info.stats.mappings_expired,
                 (unsigned) ctrl->type.info.stats.mapping_failures);


exit:
    if(ctrl != IP_NULL)
        ipcom_free(ctrl);
    if(sock != IP_INVALID_SOCKET)
        ipcom_socketclose(sock);
}


/*
 *===========================================================================
 *                    ipnet_nat_parse_pt_address
 *===========================================================================
 * Description: Parse an address and mask in a NAT-PT rule.
 * Parameters:  argv   - pointer to pointer to argument.
 *              addr_n - pointer to variable for the returned IPv6 address.
 *              mask_n - pointer to variable for the returned IPv6 mask.
 *              silent - boolean that controls printing of parsing errors.
 * Returns:     The number of arguments parsed or -1 for failure.
 */
#ifdef IPNET_USE_NAT_PT
IP_STATIC Ip_s32
ipnet_nat_parse_pt_address(char **argv, struct Ip_in6_addr *addr_n, struct Ip_in6_addr *mask_n, Ip_bool silent)
{
    char *cp, *suff;
    Ip_s32 tmp, j, i = 0;

    if(argv[i] == IP_NULL)
    {
        if(silent == IP_FALSE)
            ipcom_printf("ipnet_nat_parse_pt_address() :: No address specified."IP_LF);
        return -1;
    }

    /* Get the address */
    if(ipcom_inet_pton(IP_AF_INET6, argv[i], addr_n) != 1)
    {
        if(silent == IP_FALSE)
            ipcom_printf("ipnet_nat_parse_pt_address() :: Invalid address/mask %s."IP_LF, argv[i]);
        return -1;
    }

    /* Get the optional mask */
    cp = ipcom_strchr(argv[i], '/');
    if(cp == IP_NULL)
    {
        ipcom_memset(mask_n, 0xff, sizeof(*mask_n));
    }
    else
    {
        ++cp;
        tmp = ipcom_strtol(cp, &suff, 10);
        if(tmp < 0 || tmp > 128 || cp == suff || *suff != '\0')
        {
            if(silent == IP_FALSE)
                ipcom_printf("ipnet_nat_parse_pt_address() :: Invalid address/mask %s."IP_LF, argv[i]);
            return -1;
        }
        ipcom_memset(mask_n, 0x00, sizeof(*mask_n));
        for(j=0; j<tmp; j++)
        {
            if(j<32)
                IP_BIT_SET(mask_n->in6.addr32[0], 1<<(31-j));
            else if(j<64)
                IP_BIT_SET(mask_n->in6.addr32[1], 1<<(31-(j-32)));
            else if(j<96)
                IP_BIT_SET(mask_n->in6.addr32[2], 1<<(31-(j-64)));
            else
                IP_BIT_SET(mask_n->in6.addr32[3], 1<<(31-(j-96)));
        }
        mask_n->in6.addr32[0] = ip_htonl(mask_n->in6.addr32[0]);    /* Store in network endian */
        mask_n->in6.addr32[1] = ip_htonl(mask_n->in6.addr32[1]);    /* Store in network endian */
        mask_n->in6.addr32[2] = ip_htonl(mask_n->in6.addr32[2]);    /* Store in network endian */
        mask_n->in6.addr32[3] = ip_htonl(mask_n->in6.addr32[3]);    /* Store in network endian */
    }

    /* Check address/mask */
    for(j=0; j<4; j++)
    {
        if(((addr_n->in6.addr32[j] & ~mask_n->in6.addr32[j]) != 0) ||
            (addr_n->in6.addr32[j] == 0 && mask_n->in6.addr32[j] != 0 && mask_n->in6.addr32[j] != 0xffffffff))
        {
            if(silent == IP_FALSE)
                ipcom_printf("ipnet_nat_parse_pt_address() :: Invalid address/mask %s."IP_LF, argv[i]);
            return -1;
        }
    }

    i++;
    return i;
}
#endif


/*
 *===========================================================================
 *                    ipnet_nat_parse_address
 *===========================================================================
 * Description: Parse an address and mask in a NAT rule.
 * Parameters:  argv   - pointer to pointer to argument.
 *              addr_n - pointer to variable for the returned address.
 *              mask_n - pointer to variable for the returned mask.
 *              silent - boolean that controls printing of parsing errors.
 * Returns:     The number of arguments parsed or -1 for failure.
 */
IP_STATIC Ip_s32
ipnet_nat_parse_address(char **argv, Ip_u32 *addr_n, Ip_u32 *mask_n, Ip_bool silent)
{
    char *cp, *suff;
    Ip_s32 tmp, j, i = 0;

    if(argv[i] == IP_NULL)
    {
        if(silent == IP_FALSE)
            ipcom_printf("ipnet_nat_parse_address() :: No address specified."IP_LF);
        return -1;
    }

    /* Get the address */
    *addr_n = ipcom_inet_addr(argv[i]);
    if(*addr_n == IP_INADDR_NONE)
    {
        if(*argv[i] == '0' && *(argv[i]+1) == '/')
        {
            *addr_n = 0;
        }
        else
        {
            if(silent == IP_FALSE)
                ipcom_printf("ipnet_nat_parse_address() :: Invalid address/mask %s."IP_LF, argv[i]);
            return -1;
        }
    }

    /* Get the optional mask */
    cp = ipcom_strchr(argv[i], '/');
    if(cp == IP_NULL)
    {
        *mask_n = 0xffffffff;
    }
    else
    {
        ++cp;
        tmp = ipcom_strtol(cp, &suff, 10);
        if(tmp < 0 || tmp > 32 || cp == suff || *suff != '\0')
        {
            if(silent == IP_FALSE)
                ipcom_printf("ipnet_nat_parse_address() :: Invalid address/mask %s."IP_LF, argv[i]);
            return -1;
        }
        *mask_n = 0;
        for(j=0; j<tmp; j++)
            IP_BIT_SET(*mask_n, 1<<(31-j));
        *mask_n = ip_htonl(*mask_n);
    }

    /* Check address/mask */
    if(((*addr_n & ~(*mask_n)) != 0) || (*addr_n == 0 && *mask_n != 0 && *mask_n != 0xffffffff))
    {
        if(silent == IP_FALSE)
            ipcom_printf("ipnet_nat_parse_address() :: Invalid address/mask %s."IP_LF, argv[i]);
        return -1;
    }

    i++;
    return i;
}


/*
 *===========================================================================
 *                    ipnet_nat_parse_port
 *===========================================================================
 * Description: Parse a port in a NAT rule.
 * Parameters:  argv   - pointer to pointer to argument.
 *              port   - pointer to variable for the returned port.
 *              silent - boolean that controls printing of parsing errors.
 * Returns:     The number of arguments parsed or -1 for failure.
 */
IP_STATIC Ip_s32
ipnet_nat_parse_port(char **argv, Ip_u16 *port, Ip_bool silent)
{
    Ip_s32 tmp, i=0;
    char *suff;

    if(argv[i] == IP_NULL || ipcom_strcmp(argv[i], "port"))
    {
        if(silent == IP_FALSE)
            ipcom_printf("ipnet_nat_parse_port() :: No 'port' keyword specified."IP_LF);
        return -1;
    }
    i++;

    if(argv[i] == IP_NULL)
    {
        if(silent == IP_FALSE)
            ipcom_printf("ipnet_nat_parse_port() :: No port specified."IP_LF);
        return -1;
    }
    tmp = ipcom_strtol(argv[i], &suff, 10);
    if(tmp < 0 || tmp > 0xffff || argv[i] == suff || *suff != '\0')
    {
        if(silent == IP_FALSE)
            ipcom_printf("ipnet_nat_parse_port() :: Invalid port: %s."IP_LF, argv[i]);
        return -1;
    }
    *port = ip_ntohs((Ip_u16)tmp);
    i++;
    return i;
}


/*
 *===========================================================================
 *                    ipnet_nat_parse_portmap
 *===========================================================================
 * Description: Parse a portmap in a NAT rule.
 * Parameters:  nat_rule - pointer to the NAT rule.
 *              argv     - pointer to pointer to argument.
 *              silent   - boolean that controls printing of parsing errors.
 * Returns:     The number of arguments parsed or -1 for failure.
 */
IP_STATIC Ip_s32
ipnet_nat_parse_portmap(Ipnet_nat_rule *nat_rule, char **argv, Ip_bool silent)
{
    char *cp, *suff;
    Ip_s32 tmp, i = 0;

    if(argv[i] == IP_NULL)
        return i;

    if(!ipcom_strcmp(argv[i], "portmap"))
    {
        nat_rule->inner.portmap = 1;
        i++;
        if(argv[i] == IP_NULL)
        {
            if(silent == IP_FALSE)
                ipcom_printf("ipnet_nat_parse_portmap() :: No protocol specified."IP_LF);
            return -1;
        }

        if(!ipcom_strcmp(argv[i], "tcp"))
        {
            nat_rule->inner.portmap_protocol = IP_IPPROTO_TCP;
        }
        else if(!ipcom_strcmp(argv[i], "udp"))
        {
            nat_rule->inner.portmap_protocol = IP_IPPROTO_UDP;
        }
        else if(!ipcom_strcmp(argv[i], "tcp/udp") || !ipcom_strcmp(argv[i], "tcpudp"))
        {
            nat_rule->inner.portmap_tcpudp = 1;
        }
        else
        {
            if(silent == IP_FALSE)
                ipcom_printf("ipnet_nat_parse_portmap() :: Invalid protocol: %s."IP_LF, argv[i]);
            return -1;
        }
        i++;
        if(argv[i] == IP_NULL)
        {
            if(silent == IP_FALSE)
                ipcom_printf("ipnet_nat_parse_portmap() :: No port interval specified."IP_LF);
            return -1;
        }

        cp = ipcom_strchr(argv[i], ':');
        if(cp == IP_NULL)
        {
            if(silent == IP_FALSE)
                ipcom_printf("ipnet_nat_parse_portmap() :: Invalid port interval %s."IP_LF, argv[i]);
            return -1;
        }
        ++cp;
        tmp = ipcom_strtol(argv[i], &suff, 10);
        if(tmp < 1 || tmp > 0xffff || argv[i] == suff || *suff != ':')
        {
            if(silent == IP_FALSE)
                ipcom_printf("ipnet_nat_parse_portmap() :: Invalid port interval: %s."IP_LF, argv[i]);
            return -1;
        }
        nat_rule->inner.port_lo = (Ip_u16)tmp;
        tmp = ipcom_strtol(cp, &suff, 10);
        if(tmp < 1 || tmp > 0xffff || argv[i] == suff || *suff != '\0')
        {
            if(silent == IP_FALSE)
                ipcom_printf("ipnet_nat_parse_portmap() :: Invalid port interval: %s."IP_LF, argv[i]);
            return -1;
        }
        nat_rule->inner.port_hi = (Ip_u16)tmp;
        if(nat_rule->inner.port_lo > nat_rule->inner.port_hi)
        {
            if(silent == IP_FALSE)
                ipcom_printf("ipnet_nat_parse_portmap() :: Invalid port interval: %s."IP_LF, argv[i]);
            return -1;
        }

        i++;
    }

    return i;
}


/*
 *===========================================================================
 *                    ipnet_nat_parse_icmpidmap
 *===========================================================================
 * Description: Parse an icmpidmap in a NAT rule.
 * Parameters:  nat_rule - pointer to the NAT rule.
 *              argv     - pointer to pointer to argument.
 *              silent   - boolean that controls printing of parsing errors.
 * Returns:     The number of arguments parsed or -1 for failure.
 */
IP_STATIC Ip_s32
ipnet_nat_parse_icmpidmap(Ipnet_nat_rule *nat_rule, char **argv, Ip_bool silent)
{
    char *cp, *suff;
    Ip_s32 tmp, i = 0;

    if(argv[i] == IP_NULL)
        return i;

    if(!ipcom_strcmp(argv[i], "icmpidmap"))
    {
        nat_rule->inner.icmpidmap = 1;
        i++;
        if(argv[i] == IP_NULL)
        {
            if(silent == IP_FALSE)
                ipcom_printf("ipnet_nat_parse_icmpidmap() :: No protocol specified."IP_LF);
            return -1;
        }

        if(ipcom_strcmp(argv[i], "icmp") != 0)
        {
            if(silent == IP_FALSE)
                ipcom_printf("ipnet_nat_parse_icmpidmap() :: Invalid protocol: %s."IP_LF, argv[i]);
            return -1;
        }
        i++;
        if(argv[i] == IP_NULL)
        {
            if(silent == IP_FALSE)
                ipcom_printf("ipnet_nat_parse_icmpidmap() :: No icmp id interval specified."IP_LF);
            return -1;
        }

        cp = ipcom_strchr(argv[i], ':');
        if(cp == IP_NULL)
        {
            if(silent == IP_FALSE)
                ipcom_printf("ipnet_nat_parse_icmpidmap() :: Invalid icmp id interval %s."IP_LF, argv[i]);
            return -1;
        }
        ++cp;
        tmp = ipcom_strtol(argv[i], &suff, 10);
        if(tmp < 0 || tmp > 0xffff || argv[i] == suff || *suff != ':')
        {
            if(silent == IP_FALSE)
                ipcom_printf("ipnet_nat_parse_icmpidmap() :: Invalid icmp id interval: %s."IP_LF, argv[i]);
            return -1;
        }
        nat_rule->inner.port_lo = (Ip_u16)tmp;
        tmp = ipcom_strtol(cp, &suff, 10);
        if(tmp < 0 || tmp > 0xffff || argv[i] == suff || *suff != '\0')
        {
            if(silent == IP_FALSE)
                ipcom_printf("ipnet_nat_parse_icmpidmap() :: Invalid icmp id interval: %s."IP_LF, argv[i]);
            return -1;
        }
        nat_rule->inner.port_hi = (Ip_u16)tmp;
        if(nat_rule->inner.port_lo > nat_rule->inner.port_hi)
        {
            if(silent == IP_FALSE)
                ipcom_printf("ipnet_nat_parse_icmpidmap() :: Invalid icmp id interval: %s."IP_LF, argv[i]);
            return -1;
        }

        i++;
    }

    return i;
}


/*
 *===========================================================================
 *                    ipnet_nat_parse_proxy
 *===========================================================================
 * Description: Parse a proxy in a NAT rule.
 * Parameters:  nat_rule - pointer to the NAT rule.
 *              argv     - pointer to pointer to argument.
 *              silent   - boolean that controls printing of parsing errors.
 * Returns:     The number of arguments parsed or -1 for failure.
 */
IP_STATIC Ip_s32
ipnet_nat_parse_proxy(Ipnet_nat_rule *nat_rule, char **argv, Ip_bool silent)
{
    char *suff, *cp;
    struct Ip_servent *se;
    Ip_s32 tmp, i = 0;

    if(argv[i] == IP_NULL)
        return i;

    if(!ipcom_strcmp(argv[i], "proxy"))
    {
        nat_rule->inner.proxy = 1;
        i++;

        if (nat_rule->inner.rdr == 1)
        {
            if(argv[i] == IP_NULL)
            {
                if(silent == IP_FALSE)
                    ipcom_printf("ipnet_nat_parse_proxy() :: No proxy label specified."IP_LF);
                return -1;
            }
            nat_rule->inner.proxy_trigger  = ip_ntohs(nat_rule->inner.rdr_old_port_n);
            nat_rule->inner.proxy_protocol = nat_rule->inner.rdr_protocol;
            ipcom_strncpy(nat_rule->inner.proxy_label, argv[i], sizeof(nat_rule->inner.proxy_label)-1);
            i++;
        }
        else
        {
            if(argv[i] == IP_NULL || ipcom_strcmp(argv[i], "port"))
            {
                if(silent == IP_FALSE)
                    ipcom_printf("ipnet_nat_parse_proxy() :: No 'port' keyword specified."IP_LF);
                return -1;
            }
            i++;

            if(argv[i] == IP_NULL)
            {
                if(silent == IP_FALSE)
                    ipcom_printf("ipnet_nat_parse_proxy() :: No proxy trigger port specified."IP_LF);
                return -1;
            }
            se = ipcom_getservbyname(argv[i], IP_NULL);
            if(se != IP_NULL)
            {
                nat_rule->inner.proxy_trigger = (Ip_u16)ip_ntohs(se->s_port);
            }
            else
            {
                tmp = ipcom_strtol(argv[i], &suff, 10);
                if(tmp < 0 || tmp > 0xffff || argv[i] == suff || *suff != '\0')
                {
                    if(silent == IP_FALSE)
                        ipcom_printf("ipnet_nat_parse_proxy() :: Invalid proxy trigger port: %s."IP_LF, argv[i]);
                    return -1;
                }
                nat_rule->inner.proxy_trigger = (Ip_u16)tmp;
            }
            i++;

            if(argv[i] == IP_NULL)
            {
                if(silent == IP_FALSE)
                    ipcom_printf("ipnet_nat_parse_proxy() :: No proxy port specified."IP_LF);
                return -1;
            }

            cp = ipcom_strchr(argv[i], '/');
            if(cp == IP_NULL)
            {
                if(silent == IP_FALSE)
                    ipcom_printf("ipnet_nat_parse_proxy() :: Missing '/' in proxy label/protocol: %s."IP_LF, argv[i]);
                return -1;
            }
            else
            {
                *cp++ = '\0';
                tmp = ipnet_nat_protocol_number(cp);
                if (tmp < 0)
                {
                    tmp = ipcom_strtol(cp, &suff, 10);
                    if(tmp < 0 || tmp > 255 || cp == suff || *suff != '\0')
                    {
                        if(silent == IP_FALSE)
                            ipcom_printf("ipnet_nat_parse_proxy() :: Invalid proxy protocol: %s."IP_LF, cp);
                        return -1;
                    }
                }
                nat_rule->inner.proxy_protocol = (Ip_u8)tmp;
            }

            ipcom_strncpy(nat_rule->inner.proxy_label, argv[i], sizeof(nat_rule->inner.proxy_label)-1);
            i++;

            /* Check the nonapt keyword */
            if(argv[i] != IP_NULL && ipcom_strcmp(argv[i], "nonapt") == 0)
            {
                nat_rule->inner.proxy_nonapt = 1;
                i++;
            }
        }
    }

    return i;
}


/*
 *===========================================================================
 *                    ipnet_nat_parse_interface
 *===========================================================================
 * Description: Parse an interface in a NAT rule.
 * Parameters:  nat_rule - pointer to the NAT rule.
 *              argv     - pointer to pointer to argument.
 *              silent   - boolean that controls printing of parsing errors.
 * Returns:     The number of arguments parsed or -1 for failure.
 */
IP_STATIC Ip_s32
ipnet_nat_parse_interface(Ipnet_nat_rule *nat_rule, char **argv, Ip_bool silent)
{
    Ip_s32 i = 0;

    if(argv[i] == IP_NULL)
    {
        if(silent == IP_FALSE)
            ipcom_printf("ipnet_nat_parse_interface() :: No interface specified."IP_LF);
        return -1;
    }

    if(ipcom_strlen(argv[i]) >= sizeof(nat_rule->inner.ifname))
    {
        if(silent == IP_FALSE)
            ipcom_printf("ipnet_nat_parse_interface() :: Interface name too long: %s."IP_LF, argv[i]);
        return -1;
    }
    ipcom_strcpy((char *)nat_rule->inner.ifname, argv[i]);

    i++;
    return i;
}


/*
 *===========================================================================
 *                    ipnet_nat_parse_map
 *===========================================================================
 * Description: Parse the rule type in a NAT rule.
 * Parameters:  nat_rule - pointer to the NAT rule.
 *              argv     - pointer to pointer to argument.
 *              silent   - boolean that controls printing of parsing errors.
 * Returns:     The number of arguments parsed or -1 for failure.
 */
IP_STATIC Ip_s32
ipnet_nat_parse_map(Ipnet_nat_rule *nat_rule, char **argv, Ip_bool silent)
{
    Ip_s32 i = 0;

    if(argv[i] == IP_NULL)
    {
        if(silent == IP_FALSE)
            ipcom_printf("ipnet_nat_parse_map() :: No map specified."IP_LF);
        return -1;
    }

    if(!ipcom_strcmp(argv[i], "map"))
        nat_rule->inner.map = 1;
    else if(!ipcom_strcmp(argv[i], "map-block"))
        nat_rule->inner.map_block = 1;
    else if(!ipcom_strcmp(argv[i], "rdr"))
        nat_rule->inner.rdr = 1;
#ifdef IPNET_USE_NAT_PT
    else if(!ipcom_strcmp(argv[i], "pt"))
        nat_rule->inner.pt = 1;
    else if(!ipcom_strcmp(argv[i], "pt-block"))
        nat_rule->inner.pt_block = 1;
#endif
    else
    {
        if(silent == IP_FALSE)
            ipcom_printf("ipnet_nat_parse_map() :: Unknown rule type: %s."IP_LF, argv[i]);
        return -1;
    }

    i++;
    return i;
}


/*
 *===========================================================================
 *                    ipnet_nat_parse_index
 *===========================================================================
 * Description: Parse the index specification in NAT rule.
 * Parameters:  rule_index - pointer to variable for the returned index.
 *              argv       - pointer to pointer to argument.
 *              silent     - boolean that controls printing of parsing errors.
 * Returns:     The number of arguments parsed or -1 for failure.
 */
IP_STATIC Ip_s32
ipnet_nat_parse_index(Ip_u32 *rule_index, char **argv, Ip_bool silent)
{
    Ip_s32 i = 0;

    if(argv[i] == IP_NULL)
    {
        *rule_index = 0;
        return i;
    }

    if(*argv[i] == '@')
    {
        *rule_index = ipcom_atoi(argv[i]+1);
        if(*rule_index <= 0)
        {
            if(silent == IP_FALSE)
                ipcom_printf("ipnet_nat_parse_index() :: Invalid rule position: %s."IP_LF, argv[0]);
            return -1;
        }
        i++;
    }
    else
    {
        *rule_index = 0;
    }

    return i;
}


/*
 *===========================================================================
 *                    ipnet_nat_parse_rule
 *===========================================================================
 * Description: Parse a NAT rule.
 * Parameters:  rule_index - pointer to variable for the returned index.
 *              argc       - number of arguments in rule.
 *              argv       - pointer to pointer to argument.
 *              silent     - boolean that controls printing of parsing errors.
 * Returns:     The number of arguments parsed or -1 for failure.
 */
IP_STATIC Ipnet_nat_rule *
ipnet_nat_parse_rule(Ip_u32 *rule_index, Ip_s32 argc, char **argv, Ip_bool silent)
{
    Ipnet_nat_rule *nat_rule = IP_NULL;
    Ip_s32 ret, tmp;
    char *suff;

    if(argv[argc] != IP_NULL)
    {
        if(silent == IP_FALSE)
            ipcom_printf("ipnet_nat_parse_rule() :: Invalid argv/argc."IP_LF);
        goto err;
    }

    nat_rule = ipcom_malloc(sizeof(*nat_rule));
    if(nat_rule == IP_NULL)
    {
        if(silent == IP_FALSE)
            ipcom_printf("ipnet_nat_parse_rule() :: Out of memory."IP_LF);
        goto err;
    }
    ipcom_memset(nat_rule, 0, sizeof(*nat_rule));

    /* Check optional order */
    ret = ipnet_nat_parse_index(rule_index, argv, silent);
    if(ret < 0)
        goto err;
    else
        argv += ret;

    /* Check mandatory map keyword */
    ret = ipnet_nat_parse_map(nat_rule, argv, silent);
    if(ret < 0)
        goto err;
    else
        argv += ret;

    /* Check mandatory interface */
    ret = ipnet_nat_parse_interface(nat_rule, argv, silent);
    if(ret < 0)
        goto err;
    else
        argv += ret;

    /* Check mandatory address */
    if(nat_rule->inner.map == 1 || nat_rule->inner.map_block == 1)
    {
        ret = ipnet_nat_parse_address(argv, &nat_rule->inner.src_n, &nat_rule->inner.src_mask_n, silent);
    }
    else if(nat_rule->inner.rdr == 1)
    {
        ret = ipnet_nat_parse_address(argv, &nat_rule->inner.dst_n, &nat_rule->inner.dst_mask_n, silent);
        if(ret < 0)
            goto err;
        else
            argv += ret;

        /* Check mandatory port */
        ret = ipnet_nat_parse_port(argv, &nat_rule->inner.rdr_old_port_n, silent);
    }
    else if(nat_rule->inner.pt == 1 || nat_rule->inner.pt_block == 1)
    {
#ifdef IPNET_USE_NAT_PT
        ret = ipnet_nat_parse_pt_address(argv, &nat_rule->inner.src_n_pt, &nat_rule->inner.src_mask_n_pt, silent);
#endif
    }

    if(ret < 0)
        goto err;
    else
        argv += ret;

    /* Check that mandatory "->" is present */
    if(argv[0] && (!ipcom_strcmp(argv[0], "->") || !ipcom_strcmp(argv[0], "to")))
    {
        argv++;
    }
    else
    {
        if(silent == IP_FALSE)
            ipcom_printf("ipnet_nat_parse_rule() :: Missing '->' or 'to'."IP_LF);
        goto err;
    }

    /* Check mandatory NAT address */
    if(nat_rule->inner.map == 1 || nat_rule->inner.map_block == 1 || nat_rule->inner.rdr == 1)
    {
        ret = ipnet_nat_parse_address(argv, &nat_rule->inner.nat_addr_n, &nat_rule->inner.nat_mask_n, silent);
    }
    else if(nat_rule->inner.pt == 1 || nat_rule->inner.pt_block == 1)
    {
        ret = ipnet_nat_parse_address(argv, &nat_rule->inner.nat_addr_n, &nat_rule->inner.nat_mask_n, silent);
    }

    if(ret < 0)
        goto err;
    else
        argv += ret;

    /* Check the NAT address combos */
    if(nat_rule->inner.map == 1)
    {
        if(nat_rule->inner.nat_mask_n != 0xffffffff && nat_rule->inner.nat_addr_n != 0)
        {
            if(silent == IP_FALSE)
                ipcom_printf("ipnet_nat_parse_rule() :: Invalid subnet mask for \"map\" rule."IP_LF);
            goto err;
        }
    }
    else if(nat_rule->inner.map_block == 1)
    {
        if(nat_rule->inner.src_mask_n != nat_rule->inner.nat_mask_n)
        {
            if(silent == IP_FALSE)
                ipcom_printf("ipnet_nat_parse_rule() :: Invalid subnet masks for \"map-block\" rule."IP_LF);
            goto err;
        }
    }
    else if(nat_rule->inner.pt == 1)
    {
        if(nat_rule->inner.nat_addr_n == 0)
        {
            if(silent == IP_FALSE)
                ipcom_printf("ipnet_nat_parse_rule() :: Invalid address for \"pt\" rule."IP_LF);
            goto err;
        }
    }
    else if(nat_rule->inner.pt_block == 1)
    {
#ifdef IPNET_USE_NAT_PT
        if(nat_rule->inner.src_mask_n_pt.in6.addr32[3] != nat_rule->inner.nat_mask_n)
        {
            if(silent == IP_FALSE)
                ipcom_printf("ipnet_nat_parse_rule() :: Invalid subnet masks for \"pt-block\" rule."IP_LF);
            goto err;
        }
#endif
    }
    else if(nat_rule->inner.rdr == 1)
    {
        if(nat_rule->inner.nat_mask_n != 0xffffffff)
        {
            if(silent == IP_FALSE)
                ipcom_printf("ipnet_nat_parse_rule() :: Invalid subnet mask for \"rdr\" rule."IP_LF);
            goto err;
        }
        if(nat_rule->inner.nat_addr_n == 0)
        {
            if(silent == IP_FALSE)
                ipcom_printf("ipnet_nat_parse_rule() :: Invalid address for \"rdr\" rule. "IP_LF);
            goto err;
        }
    }

    if(nat_rule->inner.map == 1 || nat_rule->inner.map_block == 1 || nat_rule->inner.pt == 1 || nat_rule->inner.pt_block == 1)
    {
        /* Check optional portmap */
        ret = ipnet_nat_parse_portmap(nat_rule, argv, silent);
        if(ret < 0)
            goto err;
        else
            argv += ret;

        /* Check optional icmpidmap (only if portmap not specified) */
        if(nat_rule->inner.portmap == 0)
        {
            ret = ipnet_nat_parse_icmpidmap(nat_rule, argv, silent);
            if(ret < 0)
                goto err;
            else
                argv += ret;
        }

        /* Check optional proxy (only if portmap not specified) */
        if(nat_rule->inner.portmap == 0 && nat_rule->inner.icmpidmap == 0)
        {
            ret = ipnet_nat_parse_proxy(nat_rule, argv, silent);
            if(ret < 0)
                goto err;
            else
                argv += ret;
        }
    }
    else if(nat_rule->inner.rdr == 1)
    {
        /* Check mandatory port */
        ret = ipnet_nat_parse_port(argv, &nat_rule->inner.rdr_new_port_n, silent);
        if(ret < 0)
            goto err;
        else
            argv += ret;

        /* Check optional protocol */
        if(argv[0] != IP_NULL && ipcom_strcmp(argv[0], "proxy") != 0)
        {
            if(!ipcom_strcmp(argv[0], "tcp/udp") || !ipcom_strcmp(argv[0], "tcpudp"))
            {
                nat_rule->inner.rdr_tcpudp = 1;
                argv++;
            }
            else
            {
                tmp = ipnet_nat_protocol_number(argv[0]);
                if (tmp < 0)
                {
                    tmp = ipcom_strtol(argv[0], &suff, 10);
                    if(tmp < 0 || tmp > 255 || argv[0] == suff || *suff != '\0')
                    {
                        if(silent == IP_FALSE)
                            ipcom_printf("ipnet_nat_parse_rule() :: Invalid \"rdr\" protocol: %s."IP_LF, argv[0]);
                        goto err;
                    }
                }
                nat_rule->inner.rdr_protocol = (Ip_u8)tmp;
                argv++;
            }
        }
        else
        {
            /* TCP is default */
            nat_rule->inner.rdr_protocol = IP_IPPROTO_TCP;
        }

        /* Add support for specifying proxies on rdr rules */
        ret = ipnet_nat_parse_proxy(nat_rule, argv, silent);
        if(ret < 0)
            goto err;
        else
            argv += ret;
    }

    if(argv[0] != IP_NULL)
    {
        if(silent == IP_FALSE)
            ipcom_printf("ipnet_nat_parse_rule() :: Superfluous data at end of rule."IP_LF);
        goto err;
    }

    return nat_rule;
err:
    if(nat_rule != IP_NULL)
        ipcom_free(nat_rule);
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_nat_addremove_parsed_rule
 *===========================================================================
 * Description: Add or remove a parsed rule.
 * Parameters:  nat_rule   - pointer to the NAT rule.
 *              rule_index - rule index.
 *              remove     - IP_FALSE means add the rule and IP_TRUE means
 *                           remove.
 * Returns:     IPCOM_SUCCESS
 *              IPCOM_ERR_NO_MEMORY
 *              IPCOM_ERR_FAILED
 */
IP_STATIC Ip_err
ipnet_nat_addremove_parsed_rule(Ipnet_nat_rule *nat_rule, Ip_u32 rule_index, Ip_bool remove)
{
    Ipnet_nat_ctrl *ctrl = IP_NULL;
    Ip_socklen_t len;
    Ip_fd sock;
    Ip_err ret = IPCOM_SUCCESS;

    /* Create a socket */
    sock = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, 0);
    if(sock == IP_INVALID_SOCKET)
    {
        ret = IPCOM_ERR_FAILED;
        goto exit;
    }

    /* Allocate memory for the setsockopt call */
    len = sizeof(*ctrl);
    ctrl = ipcom_malloc(len);
    if(ctrl == IP_NULL)
    {
        ret = IPCOM_ERR_NO_MEMORY;
        goto exit;
    }

    if(remove == IP_TRUE)
    {
        ctrl->cmd = IPNET_NAT_CTRL_DEL_RULE;
        ipcom_memcpy(&ctrl->type.rule_info, &nat_rule->inner, sizeof(nat_rule->inner));
    }
    else
    {
        ctrl->cmd = IPNET_NAT_CTRL_ADD_RULE;
        ctrl->seqno = rule_index;
        ipcom_memcpy(&ctrl->type.rule_info, &nat_rule->inner, sizeof(nat_rule->inner));
    }

    if(ipcom_setsockopt(sock, IP_IPPROTO_IP, IP_IP_NAT, ctrl, len) != 0)
    {
        ret = IPCOM_ERR_FAILED;
        goto exit;
    }

exit:
    if(sock != IP_INVALID_SOCKET)
        ipcom_socketclose(sock);
    if(ctrl != IP_NULL)
        ipcom_free(ctrl);
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_nat_addremove_rule
 *===========================================================================
 * Description: Parse a rule and then add or remove it.
 * Parameters:  rule   - pointer to string with the NAT rule.
 *              remove - IP_FALSE means add the rule and IP_TRUE means remove.
 * Returns:     IPCOM_SUCCESS
 *              IPCOM_ERR_NO_MEMORY
 *              IPCOM_ERR_FAILED
 */
IP_STATIC Ip_err
ipnet_nat_addremove_rule(const char *rule, Ip_bool remove)
{
    Ip_err err;
    int argc = 0;
    char *argv[IPNET_NAT_MAX_RULE_ARGS+1];
    Ipnet_nat_rule *nat_rule = IP_NULL;
    Ip_u32 rule_index;
    char *tmpstr, *token, *last = IP_NULL;

    /* Split rule */
    tmpstr = ipcom_strdup(rule);
    if(tmpstr == IP_NULL)
    {
        err = IPCOM_ERR_FAILED;
        goto exit;
    }

    token = ipcom_strtok_r(tmpstr, " \t", &last);
    while(token != IP_NULL && argc < IPNET_NAT_MAX_RULE_ARGS)
    {
      argv[argc++] = token;
      token = ipcom_strtok_r(IP_NULL, " \t", &last);
    }
    argv[argc] = IP_NULL;

    /* Parse rule */
    nat_rule = ipnet_nat_parse_rule(&rule_index, argc, argv, IP_TRUE);
    if(nat_rule == IP_NULL)
    {
        err = IPCOM_ERR_FAILED;
        goto exit;
    }

    /* Add/Remove rule */
    err = ipnet_nat_addremove_parsed_rule(nat_rule, rule_index, remove);

exit:
    if(tmpstr != IP_NULL)
        ipcom_free(tmpstr);
    if(nat_rule != IP_NULL)
        ipcom_free(nat_rule);
    return err;
}


/*
 *===========================================================================
 *                    ipnet_nat_setsockopt_single
 *===========================================================================
 * Description: Issue a single command to the NAT with the IP_IP_NAT socket
 *              option.
 * Parameters:  cmd - IP_IP_NAT socket option command.
 * Returns:     IPCOM_SUCCESS
 *              IPCOM_ERR_FAILED
 */
IP_STATIC Ip_err
ipnet_nat_setsockopt_single(Ip_s32 cmd)
{
    Ipnet_nat_ctrl ctrl;
    Ip_fd sock;
    Ip_err ret = IPCOM_SUCCESS;

    /* Create a socket */
    sock = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, 0);
    if(sock == IP_INVALID_SOCKET)
        return IPCOM_ERR_FAILED;

    ctrl.cmd = cmd;
    if(ipcom_setsockopt(sock, IP_IPPROTO_IP, IP_IP_NAT, &ctrl, sizeof(ctrl)) != 0)
        ret = IPCOM_ERR_FAILED;
    ipcom_socketclose(sock);
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_nat_clear_stats
 *===========================================================================
 * Description: Clear NAT statistics.
 * Parameters:  none.
 * Returns:     IPCOM_SUCCESS
 *              IPCOM_ERR_FAILED
 */
IP_STATIC Ip_err
ipnet_nat_clear_stats(void)
{
    return ipnet_nat_setsockopt_single(IPNET_NAT_CTRL_CLEAR_STATS);
}


/*
 *===========================================================================
 *                    ipnet_nat_enable_disable
 *===========================================================================
 * Description: Enable or disable the NAT.
 * Parameters:  enable - IP_TRUE to enable. IP_FALSE to disable.
 * Returns:     IPCOM_SUCCESS
 *              IPCOM_ERR_FAILED
 *              IPCOM_ERR_ALREADY_CLOSED
 *              IPCOM_ERR_ALREADY_OPEN
 *              IPCOM_ERR_NO_MEMORY
 */
IP_STATIC Ip_err
ipnet_nat_enable_disable(Ip_bool enable)
{
    Ipnet_nat_ctrl *ctrl = IP_NULL;
    Ip_socklen_t len;
    Ip_fd sock;
    Ip_err ret;

    /* Create a socket */
    sock = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, 0);
    if(sock == IP_INVALID_SOCKET)
    {
        ret = IPCOM_ERR_FAILED;
        goto exit;
    }

    /* Allocate memory for the get-/setsockopt call */
    len = sizeof(*ctrl);
    ctrl = ipcom_malloc(len);
    if(ctrl == IP_NULL)
    {
        ret = IPCOM_ERR_NO_MEMORY;
        goto exit;
    }

    /* Get NAT information */
    ctrl->cmd = IPNET_NAT_CTRL_GET_INFO;
    if(ipcom_getsockopt(sock, IP_IPPROTO_IP, IP_IP_NAT, ctrl, &len) != 0
        || len != sizeof(*ctrl))
    {
        ret = IPCOM_ERR_FAILED;
        goto exit;
    }

    /* Check if already open */
    if(enable == IP_TRUE && ctrl->type.info.open == 1)
    {
        ret = IPCOM_ERR_ALREADY_OPEN;
        goto exit;
    }
    else if(enable == IP_FALSE && ctrl->type.info.open == 0)
    {
        ret = IPCOM_ERR_ALREADY_CLOSED;
        goto exit;
    }

    ret = enable == IP_TRUE
        ? ipnet_nat_setsockopt_single(IPNET_NAT_CTRL_ENABLE)
        : ipnet_nat_setsockopt_single(IPNET_NAT_CTRL_DISABLE);

exit:
    if(sock != IP_INVALID_SOCKET)
        ipcom_socketclose(sock);
    if(ctrl != IP_NULL)
        ipcom_free(ctrl);
    return ret;
}


#if IPCOM_USE_FILE != IPCOM_FILE_NONE
/*
 *===========================================================================
 *                    ipnet_nat_add_rule_file
 *===========================================================================
 * Description: Add NAT rules from a file.
 * Parameters:  filename - name of file with rules to add.
 *              silent - boolean that controls printing of parsing errors.
 * Returns:     0 = success.
 *             -1 = failed.
 */
IP_STATIC Ip_s32
ipnet_nat_add_rule_file(char *filename, Ip_bool silent)
{
    IP_FILE *fp;
    char line[200];
    char *cp;
    Ip_s32 ret = -1;
    int lineno = 0;

    fp = ipcom_fopen(filename, "r");
    if(fp == IP_NULL)
    {
        if(silent == IP_FALSE)
            ipcom_printf("nat :: Could not open file %s."IP_LF, filename);
        goto exit;
    }

    while(ipcom_fgets(line, sizeof(line), fp) != IP_NULL)
    {
        lineno++;

        /* Remove carriage return */
        cp = ipcom_strchr(line, '\r');
        if(cp)
        {
            *cp = '\n';
        }

        /* Remove linefeed */
        cp = ipcom_strchr(line, '\n');
        if(cp)
        {
            *cp = '\0';
        }
        else if(ipcom_fgetc(fp) != IP_EOF)
        {
            if(silent == IP_FALSE)
                ipcom_printf("nat :: Line %d is too long."IP_LF, lineno);
            goto exit;
        }

        /* Remove comment */
        cp = ipcom_strchr(line, '#');
        if(cp)
            *cp = '\0';

        /* Skip empty and comment lines */
        if(line[0] == '\0')
            continue;

        /* Add rule */
        if(ipnet_nat_add_rule(line) != IPCOM_SUCCESS)
        {
            if(silent == IP_FALSE)
                ipcom_printf("nat :: Could not add rule on line %d"IP_LF, lineno);
            goto exit;
        }

    }

    ret = 0;
exit:
    if(fp != IP_NULL)
        (void)ipcom_fclose(fp);
    return ret;
}
#endif


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
 *                    ipnet_cmd_nat
 *===========================================================================
 * Description:     The NAT shell command
 * Parameters:      argc - number of arguments.
 *                  argv - pointer to pointer to arguments.
 * Returns:         0 = OK
 *                 -1 = Failed
 */
IP_PUBLIC int
ipnet_cmd_nat(int argc, char **argv)
{
    Ipnet_nat_rule *nat_rule;
    Ip_bool remove = IP_FALSE;
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
    Ip_bool from_file = IP_FALSE;
#endif
    Ip_bool no_change = IP_FALSE;
    Ip_bool silent = IP_FALSE;
    Ip_s32 i;
    Ip_u32 rule_index;
    Ip_err ret;
    int retval = 0;
    Ipcom_getopt opt;

    if(argc < 2)
    {
        ipnet_nat_print_usage();
        return 0;
    }

    /* Check for silent mode */
    if(ipcom_strcmp(argv[1], "-silent") == 0)
    {
        argc--;
        argv++;
        silent = IP_TRUE;
        if(argc < 2)
            return 0;
    }

    ipcom_getopt_clear_r(&opt);
    while ((i = ipcom_getopt_r(argc, argv, IPNET_NAT_CMD_OPTIONS, &opt)) != -1)
    {
        switch (i)
        {
        case 'V':
            if(silent == IP_FALSE)
                ipnet_nat_print_version();
            return 0;
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
        case 'f':
            from_file = IP_TRUE;
            break;
#endif
        case 'r':
            remove = IP_TRUE;
            break;
        case 'n':
            no_change = IP_TRUE;
            break;
        case 'l':
            if(silent == IP_FALSE)
                ipnet_nat_print_rules();
            break;
        case 'm':
            if(silent == IP_FALSE)
                ipnet_nat_print_mappings();
            break;
        case 's':
            if(silent == IP_FALSE)
                ipnet_nat_print_stats();
            break;
        case 'p':
            if(silent == IP_FALSE)
                ipnet_nat_print_proxies();
            break;
        case 'D':
            ret = ipnet_nat_disable();
            if(silent == IP_FALSE)
            {
                if(ret == IPCOM_ERR_ALREADY_CLOSED)
                    ipcom_printf("nat :: NAT already disabled!"IP_LF);
                else if(ret != IPCOM_SUCCESS)
                    ipcom_printf("nat :: Failed to disable NAT!"IP_LF);
            }
            break;
        case 'E':
            ret = ipnet_nat_enable();
            if(silent == IP_FALSE)
            {
                if(ret == IPCOM_ERR_ALREADY_OPEN)
                    ipcom_printf("nat :: NAT already enabled!"IP_LF);
                else if(ret != IPCOM_SUCCESS)
                    ipcom_printf("nat :: Failed to enable NAT!"IP_LF);
            }
            break;
        case 'F':
            if(ipnet_nat_flush_mappings() != IPCOM_SUCCESS)
            {
                if(silent == IP_FALSE)
                    ipcom_printf("nat :: Failed to flush mappings!"IP_LF);
            }
            if(ipnet_nat_flush_rules() != IPCOM_SUCCESS)
            {
                if(silent == IP_FALSE)
                    ipcom_printf("nat :: Failed flush rules!"IP_LF);
            }
            break;
        case 'C':
            if(ipnet_nat_flush_mappings() != IPCOM_SUCCESS)
            {
                if(silent == IP_FALSE)
                    ipcom_printf("nat :: Failed to flush mappings!"IP_LF);
            }
            break;
        case 'Z':
            if(ipnet_nat_clear_stats() != IPCOM_SUCCESS)
            {
                if(silent == IP_FALSE)
                    ipcom_printf("nat :: Failed to clear statistics!"IP_LF);
            }
            break;
        default:
            if(silent == IP_FALSE)
                ipcom_printf("nat :: Unknown switch!"IP_LF);
            return -1;
        }
    }

#if IPCOM_USE_FILE != IPCOM_FILE_NONE
    if (from_file == IP_TRUE && remove == IP_TRUE)
    {
        if(silent == IP_FALSE)
            ipcom_printf("nat :: Cannot remove rules from file."IP_LF);
        return -1;
    }

    if(from_file == IP_TRUE && no_change == IP_TRUE)
    {
        if(silent == IP_FALSE)
            ipcom_printf("nat :: Rule syntax can not be checked from file."IP_LF);
        return -1;
    }
#endif

    if (argc == opt.optind)
    {
        if (remove == IP_TRUE)
        {
            if(silent == IP_FALSE)
                ipcom_printf("nat :: No rule specified."IP_LF);
            return -1;
        }
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
        if (from_file == IP_TRUE)
        {
            if(silent == IP_FALSE)
                ipcom_printf("nat :: No filename specified."IP_LF);
            return -1;
        }
#endif
        return 0;
    }


#if IPCOM_USE_FILE != IPCOM_FILE_NONE
    if (from_file == IP_TRUE)
    {
        retval = ipnet_nat_add_rule_file(argv[opt.optind], silent);
    }
    else
    {
#endif
        /* Parse the rule */
        nat_rule = ipnet_nat_parse_rule(&rule_index, argc - opt.optind, argv + opt.optind, silent);
        if (nat_rule != IP_NULL && no_change == IP_FALSE)
        {
            if(ipnet_nat_addremove_parsed_rule(nat_rule, rule_index, remove) != IPCOM_SUCCESS)
            {
                if(silent == IP_FALSE)
                    ipcom_printf("nat :: Could not %s rule."IP_LF, remove == IP_TRUE ? "remove" : "add");
                retval = -1;
            }
        }

        if(nat_rule != IP_NULL)
            ipcom_free(nat_rule);
        else
            retval = -1;

#if IPCOM_USE_FILE != IPCOM_FILE_NONE
    }
#endif

    return retval;
}


/*
 *===========================================================================
 *                    ipnet_nat_enable
 *===========================================================================
 * Description:     Enable the NAT and re-read configuration
 * Parameters:      none.
 * Returns:         IPCOM_SUCCESS
 *                  IPCOM_ERR_FAILED
 *                  IPCOM_ERR_NO_MEMORY
 *                  IPCOM_ERR_ALREADY_OPEN
 */
IP_PUBLIC Ip_err
ipnet_nat_enable(void)
{
    return ipnet_nat_enable_disable(IP_TRUE);
}


/*
 *===========================================================================
 *                    ipnet_nat_disable
 *===========================================================================
 * Description:     Disable the NAT and flush all mappings.
 * Parameters:      none.
 * Returns:         IPCOM_SUCCESS
 *                  IPCOM_ERR_FAILED
 *                  IPCOM_ERR_NO_MEMORY
 *                  IPCOM_ERR_ALREADY_CLOSED
 */
IP_PUBLIC Ip_err
ipnet_nat_disable(void)
{
    return ipnet_nat_enable_disable(IP_FALSE);
}


/*
 *===========================================================================
 *                    ipnet_nat_add_rule
 *===========================================================================
 * Description:     Add a NAT rule.
 * Parameters:      rule - A string with an ASCII representation of the rule.
 * Returns:         IPCOM_SUCCESS
 *                  IPCOM_ERR_FAILED
 *                  IPCOM_ERR_NO_MEMORY
 */
IP_PUBLIC Ip_err
ipnet_nat_add_rule(const char *rule)
{
    return ipnet_nat_addremove_rule(rule, IP_FALSE);
}

/*
 *===========================================================================
 *                    ipnet_nat_remove_rule
 *===========================================================================
 * Description:     Remove a NAT rule.
 * Parameters:      rule - A string with an ASCII representation of the rule.
 * Returns:         IPCOM_SUCCESS
 *                  IPCOM_ERR_FAILED
 *                  IPCOM_ERR_NO_MEMORY
 */
IP_PUBLIC Ip_err
ipnet_nat_remove_rule(const char *rule)
{
    return ipnet_nat_addremove_rule(rule, IP_TRUE);
}


/*
 *===========================================================================
 *                    ipnet_nat_flush_rules
 *===========================================================================
 * Description:     Remove all NAT rules. Will also remove all mappings.
 * Parameters:      none.
 * Returns:         IPCOM_SUCCESS
 *                  IPCOM_ERR_FAILED
 */
IP_PUBLIC Ip_err
ipnet_nat_flush_rules(void)
{
    return ipnet_nat_setsockopt_single(IPNET_NAT_CTRL_FLUSH_RULES);
}


/*
 *===========================================================================
 *                    ipnet_nat_flush_mappings
 *===========================================================================
 * Description:     Remove all NAT mappings.
 * Parameters:      none.
 * Returns:         IPCOM_SUCCESS
 *                  IPCOM_ERR_FAILED
 */
IP_PUBLIC Ip_err
ipnet_nat_flush_mappings(void)
{
    return ipnet_nat_setsockopt_single(IPNET_NAT_CTRL_FLUSH_MAPPINGS);
}


#else
int ipnet_cmd_nat_empty_file;
#endif /* IPNET_USE_NAT */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
