/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_cmd_mib2.c,v $ $Revision: 1.15 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_cmd_mib2.c,v $
 *   $Author: markus $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * MIB-2 test command.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"

#ifdef IPCOM_USE_MIB2_CMD


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_clib.h"
#include "ipcom_netif.h"
#include "ipcom_mib2_api.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Size of buffer for MIB2 info */
#define IPCOM_CMD_MIB2_BUFLEN 8092


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    Ipcom_mib2_cb_user
 *===========================================================================
 */
typedef struct Ipcom_mib2_cb_user_struct
{
    char *buf;
    int  len;
}
Ipcom_mib2_cb_user;


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

IP_PUBLIC int ipcom_cmd_mib2(int argc, char **argv);


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
 *                    ipcom_cmd_mib2_print_usage
 *===========================================================================
 * Description: Print usage information for the MIB-2 shell command.
 * Parameters:  none.
 * Returns:     nothing.
 */
IP_STATIC void
ipcom_cmd_mib2_print_usage(void)
{
    ipcom_printf(IP_LF);
    ipcom_printf("NAME"IP_LF);
    ipcom_printf("     mib2 - Print MIB-2 statistics."IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("SYNOPSIS"IP_LF);
    ipcom_printf("     mib2 <cmd [group/table/variable] [value]>"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("DESCRIPTION"IP_LF);
    ipcom_printf("     The 'mib2' shell command displays MIB-2 statistics."IP_LF);
    ipcom_printf("     Available commands are:"IP_LF);
    ipcom_printf("         clear - Clear all MIB-2 statistics."IP_LF);
    ipcom_printf("         help  - Show this info."IP_LF);
    ipcom_printf("         show <group or table>"IP_LF);
    ipcom_printf("               - Display a MIB-2 group or table:"IP_LF);
#ifdef IPCOM_USE_INET
    ipcom_printf("                 ip"IP_LF);
#endif
#ifdef IPCOM_USE_INET6
    ipcom_printf("                 ipv6"IP_LF);
#endif
#ifdef IPCOM_USE_INET
    ipcom_printf("                 icmp"IP_LF);
#endif
    ipcom_printf("                 udp"IP_LF);
    ipcom_printf("                 tcp"IP_LF);
#ifdef IPCOM_USE_INET
    ipcom_printf("                 ipv4InterfaceTable"IP_LF);
#endif
#ifdef IPCOM_USE_INET6
    ipcom_printf("                 ipv6InterfaceTable"IP_LF);
#endif
#ifdef IPCOM_USE_INET
    ipcom_printf("                 ipAddrTable"IP_LF);
#endif
    ipcom_printf("                 ipSystemStatsTable"IP_LF);
    ipcom_printf("                 ipIfStatsTable"IP_LF);
    ipcom_printf("                 ipAddressPrefixTable"IP_LF);
    ipcom_printf("                 ipAddressTable"IP_LF);
    ipcom_printf("                 ipNetToPhysicalTable"IP_LF);
#ifdef IPCOM_USE_INET6
    ipcom_printf("                 ipv6ScopeZoneIndexTable"IP_LF);
#endif
    ipcom_printf("                 ipDefaultRouterTable"IP_LF);
#ifdef IPCOM_USE_INET6
    ipcom_printf("                 ipv6RouterAdvertTable"IP_LF);
#endif
    ipcom_printf("                 icmpStatsTable"IP_LF);
    ipcom_printf("                 icmpMsgStatsTable"IP_LF);
    ipcom_printf("                 tcpConnectionTable"IP_LF);
    ipcom_printf("                 tcpListenerTable"IP_LF);
    ipcom_printf("                 udpEndpointTable"IP_LF);
    ipcom_printf("                 inetCidrRouteTable"IP_LF);
    ipcom_printf("         set <variable> <value1> [value2] [value3] [value4] [value5]"IP_LF);
    ipcom_printf("               - Write a MIB-2 variable:"IP_LF);
#ifdef IPCOM_USE_INET
    ipcom_printf("                 ipForwarding"IP_LF);
    ipcom_printf("                 ipDefaultTTL"IP_LF);
    ipcom_printf("                 ipv4InterfaceEnableStatus"IP_LF);
#endif
#ifdef IPCOM_USE_INET6
    ipcom_printf("                 ipv6IpForwarding"IP_LF);
    ipcom_printf("                 ipv6IpDefaultHopLimit"IP_LF);
    ipcom_printf("                 ipv6InterfaceEnableStatus"IP_LF);
    ipcom_printf("                 ipv6InterfaceForwarding"IP_LF);
#endif
    ipcom_printf("                 ipAddressSpinLock"IP_LF);
    ipcom_printf("                 ipv6RouterAdvertSpinLock"IP_LF);
    ipcom_printf("                 tcpConnectionState"IP_LF);
}


/*
 *===========================================================================
 *                    ipcom_cmd_mib2_tcp_group
 *===========================================================================
 * Description: Print tcp group.
 * Parameters:  none.
 * Returns:     nothing.
 */
IP_STATIC void
ipcom_cmd_mib2_tcp_group(void)
{
    Ipcom_mib2_tcp_group group;

    (void)ipcom_mib2_get_tcp_group(&group);

    ipcom_printf("tcpRtoAlgorithm: %ld"IP_LF
                 "tcpRtoMin:       %ld"IP_LF
                 "tcpRtoMax:       %ld"IP_LF
                 "tcpMaxConn:      %ld"IP_LF
                 "tcpActiveOpens:  %lu"IP_LF
                 "tcpPassiveOpens: %lu"IP_LF
                 "tcpAttemptFails: %lu"IP_LF
                 "tcpEstabResets:  %lu"IP_LF
                 "tcpCurrEstab:    %lu"IP_LF
                 "tcpInSegs:       %lu"IP_LF
                 "tcpOutSegs:      %lu"IP_LF
                 "tcpRetransSegs:  %lu"IP_LF
                 "tcpInErrs:       %lu"IP_LF
                 "tcpOutRsts:      %lu"IP_LF
                 "tcpHCInSegs:     %llu"IP_LF
                 "tcpHCOutSegs:    %llu"IP_LF,
                 group.tcpRtoAlgorithm,
                 group.tcpRtoMin,
                 group.tcpRtoMax,
                 group.tcpMaxConn,
                 group.tcpActiveOpens,
                 group.tcpPassiveOpens,
                 group.tcpAttemptFails,
                 group.tcpEstabResets,
                 group.tcpCurrEstab,
                 group.tcpInSegs,
                 group.tcpOutSegs,
                 group.tcpRetransSegs,
                 group.tcpInErrs,
                 group.tcpOutRsts,
                 group.tcpHCInSegs,
                 group.tcpHCOutSegs);
}


/*
 *===========================================================================
 *                    ipcom_cmd_mib2_udp_group
 *===========================================================================
 * Description: Print udp group.
 * Parameters:  none.
 * Returns:     nothing.
 */
IP_STATIC void
ipcom_cmd_mib2_udp_group(void)
{
    Ipcom_mib2_udp_group group;

    (void)ipcom_mib2_get_udp_group(&group);

    ipcom_printf("udpInDatagrams:    %lu"IP_LF
                 "udpNoPorts:        %lu"IP_LF
                 "udpInErrors:       %lu"IP_LF
                 "udpOutDatagrams:   %lu"IP_LF
                 "udpHCInDatagrams:  %llu"IP_LF
                 "udpHCOutDatagrams: %llu"IP_LF,
                 group.udpInDatagrams,
                 group.udpNoPorts,
                 group.udpInErrors,
                 group.udpOutDatagrams,
                 group.udpHCInDatagrams,
                 group.udpHCOutDatagrams);
}


/*
 *===========================================================================
 *                    ipcom_cmd_mib2_ip_general_group
 *===========================================================================
 * Description: Print ip general group.
 * Parameters:  none.
 * Returns:     nothing.
 */
#ifdef IPCOM_USE_INET
IP_STATIC void
ipcom_cmd_mib2_ip_general_group(void)
{
    Ipcom_mib2_ip_general_group group;

    (void)ipcom_mib2_get_ip_general_group(&group);

    ipcom_printf("ipForwarding:      %ld"IP_LF, group.ipForwarding);
    ipcom_printf("ipDefaultTTL:      %ld"IP_LF, group.ipDefaultTTL);
    ipcom_printf("ipReasmTimeout:    %ld"IP_LF, group.ipReasmTimeout);
}
#endif /* #ifdef IPCOM_USE_INET */


/*
 *===========================================================================
 *                    ipcom_cmd_mib2_ip_group
 *===========================================================================
 * Description: Print ip group.
 * Parameters:  none.
 * Returns:     nothing.
 */
#ifdef IPCOM_USE_INET
IP_STATIC void
ipcom_cmd_mib2_ip_group(void)
{
    Ipcom_mib2_ip_group group;

    (void)ipcom_mib2_get_ip_group(&group);

    ipcom_printf("ipInReceives:      %lu"IP_LF, group.ipInReceives);
    ipcom_printf("ipInHdrErrors:     %lu"IP_LF, group.ipInHdrErrors);
    ipcom_printf("ipInAddrErrors:    %lu"IP_LF, group.ipInAddrErrors);
    ipcom_printf("ipForwDatagrams:   %lu"IP_LF, group.ipForwDatagrams);
    ipcom_printf("ipInUnknownProtos: %lu"IP_LF, group.ipInUnknownProtos);
    ipcom_printf("ipInDiscards:      %lu"IP_LF, group.ipInDiscards);
    ipcom_printf("ipInDelivers:      %lu"IP_LF, group.ipInDelivers);
    ipcom_printf("ipOutRequests:     %lu"IP_LF, group.ipOutRequests);
    ipcom_printf("ipOutDiscards:     %lu"IP_LF, group.ipOutDiscards);
    ipcom_printf("ipOutNoRoutes:     %lu"IP_LF, group.ipOutNoRoutes);
    ipcom_printf("ipReasmTimeout:    %ld"IP_LF, group.ipReasmTimeout);
    ipcom_printf("ipReasmReqds:      %lu"IP_LF, group.ipReasmReqds);
    ipcom_printf("ipReasmOKs:        %lu"IP_LF, group.ipReasmOKs);
    ipcom_printf("ipReasmFails:      %lu"IP_LF, group.ipReasmFails);
    ipcom_printf("ipFragOKs:         %lu"IP_LF, group.ipFragOKs);
    ipcom_printf("ipFragFails:       %lu"IP_LF, group.ipFragFails);
    ipcom_printf("ipFragCreates:     %lu"IP_LF, group.ipFragCreates);
    ipcom_printf("ipRoutingDiscards: %lu"IP_LF, group.ipRoutingDiscards);
}
#endif /* #ifdef IPCOM_USE_INET */


/*
 *===========================================================================
 *                    ipcom_cmd_mib2_icmp_group
 *===========================================================================
 * Description: Print icmp group.
 * Parameters:  none.
 * Returns:     nothing.
 */
#ifdef IPCOM_USE_INET
IP_STATIC void
ipcom_cmd_mib2_icmp_group(void)
{
    Ipcom_mib2_icmp_group group;

    (void)ipcom_mib2_get_icmp_group(&group);

    ipcom_printf("icmpInMsgs:           %lu"IP_LF, group.icmpInMsgs);
    ipcom_printf("icmpInErrors:         %lu"IP_LF, group.icmpInErrors);
    ipcom_printf("icmpInDestUnreachs:   %lu"IP_LF, group.icmpInDestUnreachs);
    ipcom_printf("icmpInTimeExcds:      %lu"IP_LF, group.icmpInTimeExcds);
    ipcom_printf("icmpInParmProbs:      %lu"IP_LF, group.icmpInParmProbs);
    ipcom_printf("icmpInSrcQuenchs:     %lu"IP_LF, group.icmpInSrcQuenchs);
    ipcom_printf("icmpInRedirects:      %lu"IP_LF, group.icmpInRedirects);
    ipcom_printf("icmpInEchos:          %lu"IP_LF, group.icmpInEchos);
    ipcom_printf("icmpInEchoReps:       %lu"IP_LF, group.icmpInEchoReps);
    ipcom_printf("icmpInTimestamps:     %lu"IP_LF, group.icmpInTimestamps);
    ipcom_printf("icmpInTimestampReps:  %lu"IP_LF, group.icmpInTimestampReps);
    ipcom_printf("icmpInAddrMasks:      %lu"IP_LF, group.icmpInAddrMasks);
    ipcom_printf("icmpInAddrMaskReps:   %lu"IP_LF, group.icmpInAddrMaskReps);
    ipcom_printf("icmpOutMsgs:          %lu"IP_LF, group.icmpOutMsgs);
    ipcom_printf("icmpOutErrors:        %lu"IP_LF, group.icmpOutErrors);
    ipcom_printf("icmpOutDestUnreachs:  %lu"IP_LF, group.icmpOutDestUnreachs);
    ipcom_printf("icmpOutTimeExcds:     %lu"IP_LF, group.icmpOutTimeExcds);
    ipcom_printf("icmpOutParmProbs:     %lu"IP_LF, group.icmpOutParmProbs);
    ipcom_printf("icmpOutSrcQuenchs:    %lu"IP_LF, group.icmpOutSrcQuenchs);
    ipcom_printf("icmpOutRedirects:     %lu"IP_LF, group.icmpOutRedirects);
    ipcom_printf("icmpOutEchos:         %lu"IP_LF, group.icmpOutEchos);
    ipcom_printf("icmpOutEchoReps:      %lu"IP_LF, group.icmpOutEchoReps);
    ipcom_printf("icmpOutTimestamps:    %lu"IP_LF, group.icmpOutTimestamps);
    ipcom_printf("icmpOutTimestampReps: %lu"IP_LF, group.icmpOutTimestampReps);
    ipcom_printf("icmpOutAddrMasks:     %lu"IP_LF, group.icmpOutAddrMasks);
    ipcom_printf("icmpOutAddrMaskReps:  %lu"IP_LF, group.icmpOutAddrMaskReps);
}
#endif /* #ifdef IPCOM_USE_INET */


/*
 *===========================================================================
 *                    ipcom_cmd_mib2_ipv6_general_group
 *===========================================================================
 * Description: Print ipv6 general group.
 * Parameters:  none.
 * Returns:     nothing.
 */
#ifdef IPCOM_USE_INET6
IP_STATIC void
ipcom_cmd_mib2_ipv6_general_group(void)
{
    Ipcom_mib2_ipv6_general_group group;

    (void)ipcom_mib2_get_ipv6_general_group(&group);
    ipcom_printf("ipv6IpForwarding ipv6IpDefaultHopLimit"IP_LF);
    ipcom_printf("%ld %ld"IP_LF,
                 group.ipv6IpForwarding,
                 group.ipv6IpDefaultHopLimit);
}
#endif /* #ifdef IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                    ipcom_cmd_mib2_Ipv4ipv4InterfaceTableCb
 *===========================================================================
 * Description: Callback for ipcom_mib2_iterate_ipv4InterfaceTable
 * Parameters:  entry - pointer to the Ipcom_mib2_Ipv4InterfaceEntry
 *              user  - pointer to the user defined structure.
 */
#ifdef IPCOM_USE_INET
IP_STATIC int
ipcom_cmd_mib2_ipv4InterfaceTableCb(Ipcom_mib2_Ipv4InterfaceEntry *entry, void *user)
{
    Ipcom_mib2_cb_user *user_cb = (Ipcom_mib2_cb_user *)user;

    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld %ld %ld %lu"IP_LF,
                                   entry->ipv4InterfaceIfIndex,
                                   entry->ipv4InterfaceReasmMaxSize,
                                   entry->ipv4InterfaceEnableStatus,
                                   entry->ipv4InterfaceRetransmitTime);
    return 0;
}
#endif /* #ifdef IPCOM_USE_INET */


/*
 *===========================================================================
 *                    ipcom_cmd_mib2_ipv6InterfaceTableCb
 *===========================================================================
 * Description: Callback for ipcom_mib2_iterate_ipv6InterfaceTable
 * Parameters:  entry - pointer to the Ipcom_mib2_Ipv6InterfaceEntry
 *              user  - pointer to the user defined structure.
 * Returns:     0
 */
#ifdef IPCOM_USE_INET6
IP_STATIC int
ipcom_cmd_mib2_ipv6InterfaceTableCb(Ipcom_mib2_Ipv6InterfaceEntry *entry, void *user)
{
    Ipcom_mib2_cb_user *user_cb = (Ipcom_mib2_cb_user *)user;

    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld %lu %02x%02x:%02x%02x:%02x%02x:%02x%02x %ld %lu %lu %ld"IP_LF,
                                   entry->ipv6InterfaceIfIndex,
                                   entry->ipv6InterfaceReasmMaxSize,
                                   entry->ipv6InterfaceIdentifier[4],
                                   entry->ipv6InterfaceIdentifier[5],
                                   entry->ipv6InterfaceIdentifier[6],
                                   entry->ipv6InterfaceIdentifier[7],
                                   entry->ipv6InterfaceIdentifier[8],
                                   entry->ipv6InterfaceIdentifier[9],
                                   entry->ipv6InterfaceIdentifier[10],
                                   entry->ipv6InterfaceIdentifier[11],
                                   entry->ipv6InterfaceEnableStatus,
                                   entry->ipv6InterfaceReachableTime,
                                   entry->ipv6InterfaceRetransmitTime,
                                   entry->ipv6InterfaceForwarding);
    return 0;
}
#endif /* #ifdef IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                    ipcom_cmd_mib2_ipAddrTableCb
 *===========================================================================
 * Description: Callback for ipcom_mib2_iterate_ipAddrTable
 * Parameters:  entry - pointer to the Ipcom_mib2_IpAddrEntry
 *              user  - pointer to the user defined structure.
 * Returns:     0
 */
#ifdef IPCOM_USE_INET
IP_STATIC int
ipcom_cmd_mib2_ipAddrTableCb(Ipcom_mib2_IpAddrEntry *entry, void *user)
{
    Ipcom_mib2_cb_user *user_cb = (Ipcom_mib2_cb_user *)user;
    char buf[64];
    int len;

    if (ipcom_inet_ntop(IP_AF_INET, entry->ipAdEntAddr, buf, sizeof(buf)) == IP_NULL)
        ipcom_strcpy(buf, "presentation format failed!");
    len = ipcom_strlen(buf);
    buf[len++] = ' ';
    buf[len++] = '\0';
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   buf);

    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->ipAdEntIfIndex);

    if (ipcom_inet_ntop(IP_AF_INET, entry->ipAdEntNetMask, buf, sizeof(buf)) == IP_NULL)
        ipcom_strcpy(buf, "presentation format failed!");
    len = ipcom_strlen(buf);
    buf[len++] = ' ';
    buf[len++] = '\0';
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   buf);

    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->ipAdEntBcastAddr);

    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld"IP_LF,
                                   entry->ipAdEntReasmMaxSize);

    return 0;
}
#endif /* #ifdef IPCOM_USE_INET */


/*
 *===========================================================================
 *                    ipcom_cmd_mib2_ipSystemStatsTableCb
 *===========================================================================
 * Description: Callback for ipcom_mib2_iterate_ipSystemStatsTable
 * Parameters:  entry - pointer to the Ipcom_mib2_IpSystemStatsEntry
 *              user  - pointer to the user defined structure.
 * Returns:     0
 */
IP_STATIC int
ipcom_cmd_mib2_ipSystemStatsTableCb(Ipcom_mib2_IpSystemStatsEntry *entry, void *user)
{
    Ipcom_mib2_cb_user *user_cb = (Ipcom_mib2_cb_user *)user;

    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "IPVersion: %ld"IP_LF, entry->ipSystemStatsIPVersion);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCInReceives: %llu"IP_LF, entry->ipSystemStatsHCInReceives);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCInOctets: %llu"IP_LF, entry->ipSystemStatsHCInOctets);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "InHdrErrors: %lu"IP_LF, entry->ipSystemStatsInHdrErrors);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "InNoRoutes: %lu"IP_LF, entry->ipSystemStatsInNoRoutes);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "InAddrErrors: %lu"IP_LF, entry->ipSystemStatsInAddrErrors);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "InUnknownProtos: %lu"IP_LF, entry->ipSystemStatsInUnknownProtos);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "InTruncatedPkts: %lu"IP_LF, entry->ipSystemStatsInTruncatedPkts);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCInForwDatagrams: %llu"IP_LF, entry->ipSystemStatsHCInForwDatagrams);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "ReasmReqds: %lu"IP_LF, entry->ipSystemStatsReasmReqds);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "ReasmOKs: %lu"IP_LF, entry->ipSystemStatsReasmOKs);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "ReasmFails: %lu"IP_LF, entry->ipSystemStatsReasmFails);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "InDiscards: %lu"IP_LF, entry->ipSystemStatsInDiscards);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCInDelivers: %llu"IP_LF, entry->ipSystemStatsHCInDelivers);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCOutRequests: %llu"IP_LF, entry->ipSystemStatsHCOutRequests);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "OutNoRoutes: %lu"IP_LF, entry->ipSystemStatsOutNoRoutes);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCOutForwDatagrams: %llu"IP_LF, entry->ipSystemStatsHCOutForwDatagrams);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "OutDiscards: %lu"IP_LF, entry->ipSystemStatsOutDiscards);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "OutFragReqds: %lu"IP_LF, entry->ipSystemStatsOutFragReqds);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "OutFragOKs: %lu"IP_LF, entry->ipSystemStatsOutFragOKs);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "OutFragFails: %lu"IP_LF, entry->ipSystemStatsOutFragFails);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "OutFragCreates: %lu"IP_LF, entry->ipSystemStatsOutFragCreates);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCOutTransmits: %llu"IP_LF, entry->ipSystemStatsHCOutTransmits);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCOutOctets: %llu"IP_LF, entry->ipSystemStatsHCOutOctets);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCInMcastPkts: %llu"IP_LF, entry->ipSystemStatsHCInMcastPkts);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCInMcastOctets: %llu"IP_LF, entry->ipSystemStatsHCInMcastOctets);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCOutMcastPkts: %llu"IP_LF, entry->ipSystemStatsHCOutMcastPkts);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCOutMcastOctets: %llu"IP_LF, entry->ipSystemStatsHCOutMcastOctets);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCInBcastPkts: %llu"IP_LF, entry->ipSystemStatsHCInBcastPkts);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCOutBcastPkts: %llu"IP_LF, entry->ipSystemStatsHCOutBcastPkts);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "DiscontinuityTime: %lu"IP_LF, entry->ipSystemStatsDiscontinuityTime);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "RefreshRate: %lu"IP_LF, entry->ipSystemStatsRefreshRate);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_cmd_mib2_ipIfStatsTableCb
 *===========================================================================
 * Description: Callback for ipcom_mib2_iterate_ipIfStatsTable
 * Parameters:  entry - pointer to the Ipcom_mib2_IpIfStatsEntry
 *              user  - pointer to the user defined structure.
 * Returns:     0
 */
IP_STATIC int
ipcom_cmd_mib2_ipIfStatsTableCb(Ipcom_mib2_IpIfStatsEntry *entry, void *user)
{
    Ipcom_mib2_cb_user *user_cb = (Ipcom_mib2_cb_user *)user;

    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "IPVersion: %ld"IP_LF, entry->ipIfStatsIPVersion);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "IfIndex: %ld"IP_LF, entry->ipIfStatsIfIndex);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCInReceives: %llu"IP_LF, entry->ipIfStatsHCInReceives);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCInOctets: %llu"IP_LF, entry->ipIfStatsHCInOctets);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "InHdrErrors: %lu"IP_LF, entry->ipIfStatsInHdrErrors);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "InNoRoutes: %lu"IP_LF, entry->ipIfStatsInNoRoutes);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "InAddrErrors: %lu"IP_LF, entry->ipIfStatsInAddrErrors);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "InUnknownProtos: %lu"IP_LF, entry->ipIfStatsInUnknownProtos);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "InTruncatedPkts: %lu"IP_LF, entry->ipIfStatsInTruncatedPkts);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCInForwDatagrams: %llu"IP_LF, entry->ipIfStatsHCInForwDatagrams);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "ReasmReqds: %lu"IP_LF, entry->ipIfStatsReasmReqds);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "ReasmOKs: %lu"IP_LF, entry->ipIfStatsReasmOKs);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "ReasmFails: %lu"IP_LF, entry->ipIfStatsReasmFails);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "InDiscards: %lu"IP_LF, entry->ipIfStatsInDiscards);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCInDelivers: %llu"IP_LF, entry->ipIfStatsHCInDelivers);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCOutRequests: %llu"IP_LF, entry->ipIfStatsHCOutRequests);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCOutForwDatagrams: %llu"IP_LF, entry->ipIfStatsHCOutForwDatagrams);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "OutDiscards: %lu"IP_LF, entry->ipIfStatsOutDiscards);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "OutFragReqds: %lu"IP_LF, entry->ipIfStatsOutFragReqds);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "OutFragOKs: %lu"IP_LF, entry->ipIfStatsOutFragOKs);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "OutFragFails: %lu"IP_LF, entry->ipIfStatsOutFragFails);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "OutFragCreates: %lu"IP_LF, entry->ipIfStatsOutFragCreates);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCOutTransmits: %llu"IP_LF, entry->ipIfStatsHCOutTransmits);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCOutOctets: %llu"IP_LF, entry->ipIfStatsHCOutOctets);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCInMcastPkts: %llu"IP_LF, entry->ipIfStatsHCInMcastPkts);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCInMcastOctets: %llu"IP_LF, entry->ipIfStatsHCInMcastOctets);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCOutMcastPkts: %llu"IP_LF, entry->ipIfStatsHCOutMcastPkts);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCOutMcastOctets: %llu"IP_LF, entry->ipIfStatsHCOutMcastOctets);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCInBcastPkts: %llu"IP_LF, entry->ipIfStatsHCInBcastPkts);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "HCOutBcastPkts: %llu"IP_LF, entry->ipIfStatsHCOutBcastPkts);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "DiscontinuityTime: %lu"IP_LF, entry->ipIfStatsDiscontinuityTime);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "RefreshRate: %lu"IP_LF, entry->ipIfStatsRefreshRate);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_cmd_mib2_ipAddressPrefixTableCb
 *===========================================================================
 * Description: Callback for ipcom_mib2_iterate_ipAddressPrefixTable
 * Parameters:  entry - pointer to the Ipcom_mib2_IpAddressPrefixEntry
 *              user  - pointer to the user defined structure.
 * Returns:     0
 */
IP_STATIC int
ipcom_cmd_mib2_ipAddressPrefixTableCb(Ipcom_mib2_IpAddressPrefixEntry *entry, void *user)
{
    Ipcom_mib2_cb_user *user_cb = (Ipcom_mib2_cb_user *)user;
    char buf[40];

    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->ipAddressPrefixIfIndex);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->ipAddressPrefixType);
    if (entry->ipAddressPrefixType == 1)
    {
        if (ipcom_inet_ntop(IP_AF_INET, entry->ipAddressPrefixPrefix, buf, sizeof(buf)) == IP_NULL)
            ipcom_strcpy(buf, "presentation format failed! ");
    }
    else
    {
        if (ipcom_inet_ntop(IP_AF_INET6, entry->ipAddressPrefixPrefix, buf, sizeof(buf)) == IP_NULL)
            ipcom_strcpy(buf, "presentation format failed! ");
    }
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%s ",
                                   buf);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->ipAddressPrefixLength);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->ipAddressPrefixOrigin);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->ipAddressPrefixOnLinkFlag);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->ipAddressPrefixAutonomousFlag);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->ipAddressPrefixAdvPreferredLifetime);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu"IP_LF,
                                   entry->ipAddressPrefixAdvValidLifetime);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_cmd_mib2_ipAddressTableCb
 *===========================================================================
 * Description: Callback for ipcom_mib2_iterate_ipAddressTable
 * Parameters:  entry - pointer to the Ipcom_mib2_IpAddressEntry
 *              user  - pointer to the user defined structure.
 * Returns:     0
 */
IP_STATIC int
ipcom_cmd_mib2_ipAddressTableCb(Ipcom_mib2_IpAddressEntry *entry, void *user)
{
    Ipcom_mib2_cb_user *user_cb = (Ipcom_mib2_cb_user *)user;
    char buf[64];
    int len;
    Ip_u32 i;

    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->ipAddressAddrType);
    if (entry->ipAddressAddrType == 1)
    {
        if (ipcom_inet_ntop(IP_AF_INET, entry->ipAddressAddr, buf, sizeof(buf)) == IP_NULL)
            ipcom_strcpy(buf, "presentation format failed!");
        len = ipcom_strlen(buf);
        buf[len++] = ' ';
        buf[len++] = '\0';
    }
    else
    {
        if (ipcom_inet_ntop(IP_AF_INET6, entry->ipAddressAddr, buf, sizeof(buf)) == IP_NULL)
            ipcom_strcpy(buf, "presentation format failed!");
        len = ipcom_strlen(buf);
        if (entry->ipAddressAddrType == 4)
        {
            buf[len++] = '%';
            buf[len++] = '%';
            ipcom_sprintf(&buf[len], "%lu ", ((Ip_u32 *)entry->ipAddressAddr)[4]);
        }
        else
        {
            buf[len++] = ' ';
            buf[len++] = '\0';
        }
    }
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   buf);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->ipAddressIfIndex);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->ipAddressType);
    for (i=0; i<entry->ipAddressPrefix[0]; i++)
    {
        user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                       IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                       "%lu.",
                                       entry->ipAddressPrefix[i+1]);
    }
    user_cb->buf[user_cb->len-1] = ' '; /* Remove  the last . */
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->ipAddressOrigin);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->ipAddressStatus);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->ipAddressCreated);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->ipAddressLastChanged);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->ipAddressRowStatus);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld"IP_LF,
                                   entry->ipAddressStorageType);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_cmd_mib2_ipNetToPhysicalTableCb
 *===========================================================================
 * Description: Callback for ipcom_mib2_iterate_ipNetToPhysicalTable
 * Parameters:  entry - pointer to the Ipcom_mib2_IpNetToPhysicalEntry
 *              user  - pointer to the user defined structure.
 * Returns:     0
 */
IP_STATIC int
ipcom_cmd_mib2_ipNetToPhysicalTableCb(Ipcom_mib2_IpNetToPhysicalEntry *entry, void *user)
{
    Ipcom_mib2_cb_user *user_cb = (Ipcom_mib2_cb_user *)user;
    char buf[64];

    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->ipNetToPhysicalIfIndex);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->ipNetToPhysicalNetAddressType);
    if (entry->ipNetToPhysicalNetAddressType == 1)
    {
        if (ipcom_inet_ntop(IP_AF_INET, entry->ipNetToPhysicalNetAddress, buf, sizeof(buf)) == IP_NULL)
            ipcom_strcpy(buf, "presentation format failed! ");
    }
    else
    {
        if (ipcom_inet_ntop(IP_AF_INET6, entry->ipNetToPhysicalNetAddress, buf, sizeof(buf)) == IP_NULL)
            ipcom_strcpy(buf, "presentation format failed! ");
    }
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%s ",
                                   buf);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%02x:%02x:%02x:%02x:%02x:%02x ",
                                   entry->ipNetToPhysicalPhysAddress[4],
                                   entry->ipNetToPhysicalPhysAddress[5],
                                   entry->ipNetToPhysicalPhysAddress[6],
                                   entry->ipNetToPhysicalPhysAddress[7],
                                   entry->ipNetToPhysicalPhysAddress[8],
                                   entry->ipNetToPhysicalPhysAddress[9]);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->ipNetToPhysicalLastUpdated);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->ipNetToPhysicalType);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->ipNetToPhysicalState);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld"IP_LF,
                                   entry->ipNetToPhysicalRowStatus);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_cmd_mib2_ipv6ScopeZoneIndexTableCb
 *===========================================================================
 * Description: Callback for ipcom_mib2_iterate_ipv6ScopeZoneIndexTable
 * Parameters:  entry - pointer to the Ipcom_mib2_Ipv6ScopeZoneIndexEntry
 *              user  - pointer to the user defined structure.
 * Returns:     0
 */
#ifdef IPCOM_USE_INET6
IP_STATIC int
ipcom_cmd_mib2_ipv6ScopeZoneIndexTableCb(Ipcom_mib2_Ipv6ScopeZoneIndexEntry *entry, void *user)
{
    Ipcom_mib2_cb_user *user_cb = (Ipcom_mib2_cb_user *)user;

    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->ipv6ScopeZoneIndexIfIndex);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->ipv6ScopeZoneIndexLinkLocal);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->ipv6ScopeZoneIndex3);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->ipv6ScopeZoneIndexAdminLocal);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->ipv6ScopeZoneIndexSiteLocal);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->ipv6ScopeZoneIndex6);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->ipv6ScopeZoneIndex7);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->ipv6ScopeZoneIndexOrganizationLocal);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->ipv6ScopeZoneIndex9);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->ipv6ScopeZoneIndexA);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->ipv6ScopeZoneIndexB);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->ipv6ScopeZoneIndexC);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu"IP_LF,
                                   entry->ipv6ScopeZoneIndexD);
    return 0;
}
#endif /* #ifdef IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                    ipcom_cmd_mib2_ipDefaultRouterTableCb
 *===========================================================================
 * Description: Callback for ipcom_mib2_iterate_ipDefaultRouterTable
 * Parameters:  entry - pointer to the Ipcom_mib2_IpDefaultRouterEntry
 *              user  - pointer to the user defined structure.
 * Returns:     0
 */
IP_STATIC int
ipcom_cmd_mib2_ipDefaultRouterTableCb(Ipcom_mib2_IpDefaultRouterEntry *entry, void *user)
{
    Ipcom_mib2_cb_user *user_cb = (Ipcom_mib2_cb_user *)user;
    char buf[64];

    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->ipDefaultRouterAddressType);
    if (entry->ipDefaultRouterAddressType == 1)
    {
        if (ipcom_inet_ntop(IP_AF_INET, entry->ipDefaultRouterAddress, buf, sizeof(buf)) == IP_NULL)
            ipcom_strcpy(buf, "presentation format failed! ");
    }
    else
    {
        if (ipcom_inet_ntop(IP_AF_INET6, entry->ipDefaultRouterAddress, buf, sizeof(buf)) == IP_NULL)
            ipcom_strcpy(buf, "presentation format failed! ");
    }
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%s ",
                                   buf);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->ipDefaultRouterIfIndex);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->ipDefaultRouterLifetime);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld"IP_LF,
                                   entry->ipDefaultRouterPreference);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_cmd_mib2_ipv6RouterAdvertTableCb
 *===========================================================================
 * Description: Callback for ipcom_mib2_iterate_ipv6RouterAdvertTable
 * Parameters:  entry - pointer to the Ipcom_mib2_Ipv6RouterAdvertEntry
 *              user  - pointer to the user defined structure.
 * Returns:     0
 */
#ifdef IPCOM_USE_INET6
IP_STATIC int
ipcom_cmd_mib2_ipv6RouterAdvertTableCb(Ipcom_mib2_Ipv6RouterAdvertEntry *entry, void *user)
{
    Ipcom_mib2_cb_user *user_cb = (Ipcom_mib2_cb_user *)user;

    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->ipv6RouterAdvertIfIndex);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->ipv6RouterAdvertSendAdverts);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->ipv6RouterAdvertMaxInterval);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->ipv6RouterAdvertMinInterval);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->ipv6RouterAdvertManagedFlag);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->ipv6RouterAdvertOtherConfigFlag);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->ipv6RouterAdvertLinkMTU);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->ipv6RouterAdvertReachableTime);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->ipv6RouterAdvertRetransmitTime);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->ipv6RouterAdvertCurHopLimit);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->ipv6RouterAdvertDefaultLifetime);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld"IP_LF,
                                   entry->ipv6RouterAdvertRowStatus);
    return 0;
}
#endif /* #ifdef IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                    ipcom_cmd_mib2_icmpStatsTableCb
 *===========================================================================
 * Description: Callback for ipcom_mib2_iterate_icmpMsgStatsTable
 * Parameters:  entry - pointer to the Ipcom_mib2_IcmpMsgStatsEntry
 *              user  - pointer to the user defined structure.
 * Returns:     0
 */
IP_STATIC int
ipcom_cmd_mib2_icmpStatsTableCb(Ipcom_mib2_IcmpStatsEntry *entry, void *user)
{
    Ipcom_mib2_cb_user *user_cb = (Ipcom_mib2_cb_user *)user;

    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld %lu %lu %lu %lu"IP_LF,
                                   entry->icmpStatsIPVersion,
                                   entry->icmpStatsInMsgs,
                                   entry->icmpStatsInErrors,
                                   entry->icmpStatsOutMsgs,
                                   entry->icmpStatsOutErrors);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_cmd_mib2_icmpMsgStatsTableCb
 *===========================================================================
 * Description: Callback for ipcom_mib2_iterate_icmpMsgStatsTable
 * Parameters:  entry - pointer to the Ipcom_mib2_IcmpMsgStatsEntry
 *              user  - pointer to the user defined structure.
 * Returns:     0
 */
IP_STATIC int
ipcom_cmd_mib2_icmpMsgStatsTableCb(Ipcom_mib2_IcmpMsgStatsEntry *entry, void *user)
{
    Ipcom_mib2_cb_user *user_cb = (Ipcom_mib2_cb_user *)user;

    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld %ld %lu %lu"IP_LF,
                                   entry->icmpMsgStatsIPVersion,
                                   entry->icmpMsgStatsType,
                                   entry->icmpMsgStatsInPkts,
                                   entry->icmpMsgStatsOutPkts);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_cmd_mib2_tcpConnectionTableCb
 *===========================================================================
 * Description: Callback for ipcom_mib2_iterate_tcpConnectionTable
 * Parameters:  entry - pointer to the Ipcom_mib2_TcpConnectionEntry
 *              user  - pointer to the user defined structure.
 * Returns:     0
 */
IP_STATIC int
ipcom_cmd_mib2_tcpConnectionTableCb(Ipcom_mib2_TcpConnectionEntry *entry, void *user)
{
    Ipcom_mib2_cb_user *user_cb = (Ipcom_mib2_cb_user *)user;
    char buf[64];
    int len;

    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->tcpConnectionLocalAddressType);
    if (entry->tcpConnectionLocalAddressType == 1)
    {
        if (ipcom_inet_ntop(IP_AF_INET, entry->tcpConnectionLocalAddress, buf, sizeof(buf)) == IP_NULL)
            ipcom_strcpy(buf, "presentation format failed!");
        len = ipcom_strlen(buf);
        buf[len++] = ' ';
        buf[len++] = '\0';
    }
    else
    {
        if (ipcom_inet_ntop(IP_AF_INET6, entry->tcpConnectionLocalAddress, buf, sizeof(buf)) == IP_NULL)
            ipcom_strcpy(buf, "presentation format failed!");
        len = ipcom_strlen(buf);
        if (entry->tcpConnectionLocalAddressType == 4)
        {
            buf[len++] = '%';
            buf[len++] = '%';
            ipcom_sprintf(&buf[len], "%lu ", ((Ip_u32 *)entry->tcpConnectionLocalAddress)[4]);
        }
        else
        {
            buf[len++] = ' ';
            buf[len++] = '\0';
        }
    }
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   buf);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->tcpConnectionLocalPort);


    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->tcpConnectionRemAddressType);
    if (entry->tcpConnectionRemAddressType == 1)
    {
        if (ipcom_inet_ntop(IP_AF_INET, entry->tcpConnectionRemAddress, buf, sizeof(buf)) == IP_NULL)
            ipcom_strcpy(buf, "presentation format failed!");
        len = ipcom_strlen(buf);
        buf[len++] = ' ';
        buf[len++] = '\0';
    }
    else
    {
        if (ipcom_inet_ntop(IP_AF_INET6, entry->tcpConnectionRemAddress, buf, sizeof(buf)) == IP_NULL)
            ipcom_strcpy(buf, "presentation format failed!");
        len = ipcom_strlen(buf);
        if (entry->tcpConnectionRemAddressType == 4)
        {
            buf[len++] = '%';
            buf[len++] = '%';
            ipcom_sprintf(&buf[len], "%lu ", ((Ip_u32 *)entry->tcpConnectionRemAddress)[4]);
        }
        else
        {
            buf[len++] = ' ';
            buf[len++] = '\0';
        }
    }
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   buf);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->tcpConnectionRemPort);

    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->tcpConnectionState);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu"IP_LF,
                                   entry->tcpConnectionProcess);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_cmd_mib2_tcpListenerTableCb
 *===========================================================================
 * Description: Callback for ipcom_mib2_iterate_tcpListenerTable
 * Parameters:  entry - pointer to the Ipcom_mib2_TcpListenerEntry
 *              user  - pointer to the user defined structure.
 * Returns:     0
 */
IP_STATIC int
ipcom_cmd_mib2_tcpListenerTableCb(Ipcom_mib2_TcpListenerEntry *entry, void *user)
{
    Ipcom_mib2_cb_user *user_cb = (Ipcom_mib2_cb_user *)user;
    char buf[64];
    int len;

    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->tcpListenerLocalAddressType);
    if (entry->tcpListenerLocalAddressType == 1)
    {
        if (ipcom_inet_ntop(IP_AF_INET, entry->tcpListenerLocalAddress, buf, sizeof(buf)) == IP_NULL)
            ipcom_strcpy(buf, "presentation format failed!");
        len = ipcom_strlen(buf);
        buf[len++] = ' ';
        buf[len++] = '\0';
    }
    else
    {
        if (ipcom_inet_ntop(IP_AF_INET6, entry->tcpListenerLocalAddress, buf, sizeof(buf)) == IP_NULL)
            ipcom_strcpy(buf, "presentation format failed!");
        len = ipcom_strlen(buf);
        if (entry->tcpListenerLocalAddressType == 4)
        {
            buf[len++] = '%';
            buf[len++] = '%';
            ipcom_sprintf(&buf[len], "%lu ", ((Ip_u32 *)entry->tcpListenerLocalAddress)[4]);
        }
        else
        {
            buf[len++] = ' ';
            buf[len++] = '\0';
        }
    }
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   buf);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->tcpListenerLocalPort);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu"IP_LF,
                                   entry->tcpListenerProcess);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_cmd_mib2_udpEndpointTableCb
 *===========================================================================
 * Description: Callback for ipcom_mib2_iterate_udpEndpointTable
 * Parameters:  entry - pointer to the Ipcom_mib2_UdpEndpointEntry
 *              user  - pointer to the user defined structure.
 * Returns:     0
 */
IP_STATIC int
ipcom_cmd_mib2_udpEndpointTableCb(Ipcom_mib2_UdpEndpointEntry *entry, void *user)
{
    Ipcom_mib2_cb_user *user_cb = (Ipcom_mib2_cb_user *)user;
    char buf[64];
    int len;

    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->udpEndpointLocalAddressType);
    if (entry->udpEndpointLocalAddressType == 1)
    {
        if (ipcom_inet_ntop(IP_AF_INET, entry->udpEndpointLocalAddress, buf, sizeof(buf)) == IP_NULL)
            ipcom_strcpy(buf, "presentation format failed!");
        len = ipcom_strlen(buf);
        buf[len++] = ' ';
        buf[len++] = '\0';
    }
    else
    {
        if (ipcom_inet_ntop(IP_AF_INET6, entry->udpEndpointLocalAddress, buf, sizeof(buf)) == IP_NULL)
            ipcom_strcpy(buf, "presentation format failed!");
        len = ipcom_strlen(buf);
        if (entry->udpEndpointLocalAddressType == 4)
        {
            buf[len++] = '%';
            buf[len++] = '%';
            ipcom_sprintf(&buf[len], "%lu ", ((Ip_u32 *)entry->udpEndpointLocalAddress)[4]);
        }
        else
        {
            buf[len++] = ' ';
            buf[len++] = '\0';
        }
    }
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   buf);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->udpEndpointLocalPort);


    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->udpEndpointRemoteAddressType);
    if (entry->udpEndpointRemoteAddressType == 1)
    {
        if (ipcom_inet_ntop(IP_AF_INET, entry->udpEndpointRemoteAddress, buf, sizeof(buf)) == IP_NULL)
            ipcom_strcpy(buf, "presentation format failed!");
        len = ipcom_strlen(buf);
        buf[len++] = ' ';
        buf[len++] = '\0';
    }
    else
    {
        if (ipcom_inet_ntop(IP_AF_INET6, entry->udpEndpointRemoteAddress, buf, sizeof(buf)) == IP_NULL)
            ipcom_strcpy(buf, "presentation format failed!");
        len = ipcom_strlen(buf);
        if (entry->udpEndpointRemoteAddressType == 4)
        {
            buf[len++] = '%';
            buf[len++] = '%';
            ipcom_sprintf(&buf[len], "%lu ", ((Ip_u32 *)entry->udpEndpointRemoteAddress)[4]);
        }
        else
        {
            buf[len++] = ' ';
            buf[len++] = '\0';
        }
    }
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   buf);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->udpEndpointRemotePort);

    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->udpEndpointInstance);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu"IP_LF,
                                   entry->udpEndpointProcess);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_cmd_mib2_ipDefaultRouterTableCb
 *===========================================================================
 * Description: Callback for ipcom_mib2_iterate_ipDefaultRouterTable
 * Parameters:  entry - pointer to the Ipcom_mib2_InetCidrRouteEntry
 *              user  - pointer to the user defined structure.
 * Returns:     0
 */
IP_STATIC int
ipcom_cmd_mib2_inetCidrRouteTableCb(Ipcom_mib2_InetCidrRouteEntry *entry, void *user)
{
    Ipcom_mib2_cb_user *user_cb = (Ipcom_mib2_cb_user *)user;
    char buf[64];
    Ip_u32 i;

    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->inetCidrRouteDestType);
    if (entry->inetCidrRouteDestType == 1)
    {
        if (ipcom_inet_ntop(IP_AF_INET, entry->inetCidrRouteDest, buf, sizeof(buf)) == IP_NULL)
            ipcom_strcpy(buf, "presentation format failed! ");
    }
    else
    {
        if (ipcom_inet_ntop(IP_AF_INET6, entry->inetCidrRouteDest, buf, sizeof(buf)) == IP_NULL)
            ipcom_strcpy(buf, "presentation format failed! ");
    }
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%s ",
                                   buf);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->inetCidrRoutePfxLen);
    for (i=0; i<entry->inetCidrRoutePolicy[0]; i++)
    {
        user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                       IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                       "%lu.",
                                       entry->inetCidrRoutePolicy[i+1]);
    }
    user_cb->buf[user_cb->len-1] = ' '; /* Remove  the last . */
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->inetCidrRouteNextHopType);
    if (entry->inetCidrRouteNextHopType == 1)
    {
        if (ipcom_inet_ntop(IP_AF_INET, entry->inetCidrRouteNextHop, buf, sizeof(buf)) == IP_NULL)
            ipcom_strcpy(buf, "presentation format failed! ");
    }
    else if (entry->inetCidrRouteNextHopType == 2)
    {
        if (ipcom_inet_ntop(IP_AF_INET6, entry->inetCidrRouteNextHop, buf, sizeof(buf)) == IP_NULL)
            ipcom_strcpy(buf, "presentation format failed! ");
    }
    else
    {
        if (entry->inetCidrRouteDestType == 1)
            ipcom_strcpy(buf, "0.0.0.0");
        else
            ipcom_strcpy(buf, "::");

    }
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%s ",
                                   buf);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->inetCidrRouteIfIndex);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->inetCidrRouteType);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->inetCidrRouteProto);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->inetCidrRouteAge);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%lu ",
                                   entry->inetCidrRouteNextHopAS);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->inetCidrRouteMetric1);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->inetCidrRouteMetric2);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->inetCidrRouteMetric3);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->inetCidrRouteMetric4);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld ",
                                   entry->inetCidrRouteMetric5);
    user_cb->len += ipcom_snprintf(&user_cb->buf[user_cb->len],
                                   IPCOM_CMD_MIB2_BUFLEN - user_cb->len - 1,
                                   "%ld"IP_LF,
                                   entry->inetCidrRouteStatus);
    return 0;
}


/*
 ****************************************************************************
 * 10                    PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_cmd_mib2
 *===========================================================================
 * Description:     Shell command: Print MIB-2 statistics.
 * Parameters:      argc - number of arguments
 *                  argv - pointer to pointer to the arguments
 * Returns:         0 for success or -1 for fail.
 *
 */
IP_PUBLIC int
ipcom_cmd_mib2(int argc, char **argv)
{
    int i = 0, ret = 0, val1, val2, wret;
    Ipcom_mib2_cb_user     user_cb = { IP_NULL, 0 };
    Ipcom_mib2_TimeStamp   timestamp;
    Ipcom_mib2_TestAndIncr spinlock;
    Ipcom_mib2_Gauge32     numroutes;
    Ipcom_mib2_Counter32   numdiscards;

    if(argc < 2)
    {
        ipcom_cmd_mib2_print_usage();
        return 0;
    }

    /* Find command */
    if (ipcom_strcmp(argv[1], "clear") == 0)
    {
        ipcom_mib2_clear();
        return 0;
    }
    else if (ipcom_strcmp(argv[1], "help") == 0)
    {
        ipcom_cmd_mib2_print_usage();
        return 0;
    }
    else if (ipcom_strcmp(argv[1], "show") == 0)
    {
        if(argc < 3)
        {
            ipcom_printf("mib2 :: no group or table specified!"IP_LF);
            return -1;
        }

        user_cb.buf = ipcom_malloc(IPCOM_CMD_MIB2_BUFLEN);
        if (user_cb.buf == IP_NULL)
        {
            ipcom_printf("mib2 :: could not allocate %d bytes of memory!"IP_LF, IPCOM_CMD_MIB2_BUFLEN);
            return -1;
        }
        user_cb.len = 0;

        /* Find group or table */
        if (ipcom_strcmp(argv[2], "udp") == 0)
        {
            ipcom_cmd_mib2_udp_group();
        }
        else if (ipcom_strcmp(argv[2], "tcp") == 0)
        {
            ipcom_cmd_mib2_tcp_group();
        }
#ifdef IPCOM_USE_INET
        else if (ipcom_strcmp(argv[2], "ip") == 0)
        {
            ipcom_cmd_mib2_ip_general_group();
            ipcom_printf(IP_LF);
            ipcom_cmd_mib2_ip_group();
        }
        else if (ipcom_strcmp(argv[2], "icmp") == 0)
        {
            ipcom_cmd_mib2_icmp_group();
        }
#endif
#ifdef IPCOM_USE_INET6
        else if (ipcom_strcmp(argv[2], "ipv6") == 0)
        {
            ipcom_cmd_mib2_ipv6_general_group();
        }
#endif
#ifdef IPCOM_USE_INET
        else if (ipcom_strcmp(argv[2], "ipv4InterfaceTable") == 0)
        {
            (void)ipcom_mib2_get_ipv4InterfaceTableLastChange(&timestamp);
            ipcom_printf("LastChange: %lu"IP_LF, timestamp);
            ipcom_printf("IfIndex ReasmMaxSize EnableStatus RetransmitTime"IP_LF);
            (void)ipcom_mib2_iterate_ipv4InterfaceTable(ipcom_cmd_mib2_ipv4InterfaceTableCb, &user_cb);
        }
#endif
#ifdef IPCOM_USE_INET6
        else if (ipcom_strcmp(argv[2], "ipv6InterfaceTable") == 0)
        {
            (void)ipcom_mib2_get_ipv6InterfaceTableLastChange(&timestamp);
            ipcom_printf("LastChange: %lu"IP_LF, timestamp);
            ipcom_printf("IfIndex ReasmMaxSize Identifier EnableStatus ReachableTime RetransmitTime Forwarding"IP_LF);
            (void)ipcom_mib2_iterate_ipv6InterfaceTable(ipcom_cmd_mib2_ipv6InterfaceTableCb, &user_cb);
        }
#endif
#ifdef IPCOM_USE_INET
        else if (ipcom_strcmp(argv[2], "ipAddrTable") == 0)
        {
            ipcom_printf("Addr IfIndex NetMask BcastAddr ReasmMaxSize"IP_LF);
            (void)ipcom_mib2_iterate_ipAddrTable(ipcom_cmd_mib2_ipAddrTableCb, &user_cb);
        }
#endif
        else if (ipcom_strcmp(argv[2], "ipSystemStatsTable") == 0)
        {
            (void)ipcom_mib2_iterate_ipSystemStatsTable(ipcom_cmd_mib2_ipSystemStatsTableCb, &user_cb);
        }
        else if (ipcom_strcmp(argv[2], "ipIfStatsTable") == 0)
        {
            (void)ipcom_mib2_get_ipIfStatsTableLastChange(&timestamp);
            ipcom_printf("LastChange: %lu"IP_LF, timestamp);
            (void)ipcom_mib2_iterate_ipIfStatsTable(ipcom_cmd_mib2_ipIfStatsTableCb, &user_cb);
        }
        else if (ipcom_strcmp(argv[2], "icmpStatsTable") == 0)
        {
            ipcom_printf("IPVersion InMsgs InErrors OutMsgs OutErrors"IP_LF);
            (void)ipcom_mib2_iterate_icmpStatsTable(ipcom_cmd_mib2_icmpStatsTableCb, &user_cb);
        }
        else if (ipcom_strcmp(argv[2], "icmpMsgStatsTable") == 0)
        {
            ipcom_printf("IPVersion Type InPkts OutPkts"IP_LF);
            (void)ipcom_mib2_iterate_icmpMsgStatsTable(ipcom_cmd_mib2_icmpMsgStatsTableCb, &user_cb);
        }
        else if (ipcom_strcmp(argv[2], "ipAddressPrefixTable") == 0)
        {
            ipcom_printf("IfIndex Type Prefix Length Origin OnLinkFlag AutonomousFlag AdvPreferredLifetime AdvValidLifetime"IP_LF);
            (void)ipcom_mib2_iterate_ipAddressPrefixTable(ipcom_cmd_mib2_ipAddressPrefixTableCb, &user_cb);
        }
        else if (ipcom_strcmp(argv[2], "ipAddressTable") == 0)
        {
            (void)ipcom_mib2_get_ipAddressSpinLock(&spinlock);
            ipcom_printf("SpinLock: %ld"IP_LF, spinlock);
            ipcom_printf("AddrType Addr IfIndex Type Prefix Origin Status Created LastChanged RowStatus StorageType"IP_LF);
            (void)ipcom_mib2_iterate_ipAddressTable(ipcom_cmd_mib2_ipAddressTableCb, &user_cb);
        }
        else if (ipcom_strcmp(argv[2], "ipNetToPhysicalTable") == 0)
        {
            ipcom_printf("IfIndex NetAddressType NetAddress PhysAddress LastUpdated Type State RowStatus"IP_LF);
            (void)ipcom_mib2_iterate_ipNetToPhysicalTable(ipcom_cmd_mib2_ipNetToPhysicalTableCb, &user_cb);
        }
#ifdef IPCOM_USE_INET6
        else if (ipcom_strcmp(argv[2], "ipv6ScopeZoneIndexTable") == 0)
        {
            ipcom_printf("IfIndex LinkLocal 3 AdminLocal SiteLocal 6 7 OriganizationLocal 9 A B C D"IP_LF);
            (void)ipcom_mib2_iterate_ipv6ScopeZoneIndexTable(ipcom_cmd_mib2_ipv6ScopeZoneIndexTableCb, &user_cb);
        }
#endif
        else if (ipcom_strcmp(argv[2], "ipDefaultRouterTable") == 0)
        {
            ipcom_printf("AddressType Address IfIndex Lifetime Preference"IP_LF);
            (void)ipcom_mib2_iterate_ipDefaultRouterTable(ipcom_cmd_mib2_ipDefaultRouterTableCb, &user_cb);
        }
#ifdef IPCOM_USE_INET6
        else if (ipcom_strcmp(argv[2], "ipv6RouterAdvertTable") == 0)
        {
            (void)ipcom_mib2_get_ipv6RouterAdvertSpinLock(&spinlock);
            ipcom_printf("SpinLock: %ld"IP_LF, spinlock);
            ipcom_printf("IfIndex SendAdverts MaxInterval MinInterval ManagedFlag OtherConfigFlag LinkMTU ReachableTime RetransmitTime CurHopLimit DefaultLifetime RowStatus"IP_LF);
            (void)ipcom_mib2_iterate_ipv6RouterAdvertTable(ipcom_cmd_mib2_ipv6RouterAdvertTableCb, &user_cb);
        }
#endif
        else if (ipcom_strcmp(argv[2], "tcpConnectionTable") == 0)
        {
            ipcom_printf("LocalAddressType LocalAddress LocalPort RemAddressType RemAddress RemPort State Process"IP_LF);
            (void)ipcom_mib2_iterate_tcpConnectionTable(ipcom_cmd_mib2_tcpConnectionTableCb, &user_cb);
        }
        else if (ipcom_strcmp(argv[2], "tcpListenerTable") == 0)
        {
            ipcom_printf("LocalAddressType LocalAddress LocalPort Process"IP_LF);
            (void)ipcom_mib2_iterate_tcpListenerTable(ipcom_cmd_mib2_tcpListenerTableCb, &user_cb);
        }
        else if (ipcom_strcmp(argv[2], "udpEndpointTable") == 0)
        {
            ipcom_printf("LocalAddressType LocalAddress LocalPort RemoteAddressType RemoteAddress RemotePort Instance Process"IP_LF);
            (void)ipcom_mib2_iterate_udpEndpointTable(ipcom_cmd_mib2_udpEndpointTableCb, &user_cb);
        }
        else if (ipcom_strcmp(argv[2], "inetCidrRouteTable") == 0)
        {

            (void)ipcom_mib2_get_inetCidrRouteNumber(&numroutes);
            (void)ipcom_mib2_get_inetCidrRouteDiscards(&numdiscards);
            ipcom_printf("Number: %lu"IP_LF, numroutes);
            ipcom_printf("Discards: %lu"IP_LF, numdiscards);
            ipcom_printf("DestType Dest PfxLen Policy NextHopType NextHop IfIndex Type Proto Age NextHopAS "
                         "Metric1 Metric2 Metric3 Metric4 Metric5 Status"IP_LF);
            (void)ipcom_mib2_iterate_inetCidrRouteTable(ipcom_cmd_mib2_inetCidrRouteTableCb, &user_cb);
        }
        else
        {
            ipcom_printf("mib2 :: unknown group or table (%s)!"IP_LF, argv[2]);
            ret = -1;
            goto exit;
        }

        user_cb.buf[user_cb.len] = '\0';
        while (user_cb.buf[i] != '\0')
        {
            if (user_cb.buf[i] == '%')
                ipcom_printf("%%");
            else
                ipcom_printf("%c", user_cb.buf[i]);
            i++;
        }
    }
    else if (ipcom_strcmp(argv[1], "set") == 0)
    {
        if(argc < 3)
        {
            ipcom_printf("mib2 :: no variable specified!"IP_LF);
            return -1;
        }

        if(argc < 4)
        {
            ipcom_printf("mib2 :: no value1 specified!"IP_LF);
            return -1;
        }

        /* Get new variable value1 */
        val1 = ipcom_atoi(argv[3]);

        /* Find variable */
        if (ipcom_strcmp(argv[2], "ipAddressSpinLock") == 0)
        {
            wret = ipcom_mib2_set_ipAddressSpinLock((Ipcom_mib2_Integer)val1);
        }
        else if (ipcom_strcmp(argv[2], "tcpConnectionState") == 0)
        {
            Ipcom_mib2_TcpConnectionEntry entry;

            if(argc < 8)
            {
                ipcom_printf("mib2 :: too few values specified!"IP_LF);
                return -1;
            }

            if (ipcom_inet_pton(IP_AF_INET, argv[4], entry.tcpConnectionLocalAddress) == 1)
                entry.tcpConnectionLocalAddressType = 1;
            else if (ipcom_inet_pton(IP_AF_INET6, argv[4], entry.tcpConnectionLocalAddress) == 1)
                entry.tcpConnectionLocalAddressType = 2;
            else
            {
                ipcom_printf("mib2 :: invalid address (%s)!"IP_LF, argv[4]);
                return -1;
            }
            entry.tcpConnectionLocalPort = ipcom_atoi(argv[5]);

            if (ipcom_inet_pton(IP_AF_INET, argv[6], entry.tcpConnectionRemAddress) == 1)
                entry.tcpConnectionRemAddressType = 1;
            else if (ipcom_inet_pton(IP_AF_INET6, argv[6], entry.tcpConnectionRemAddress) == 1)
                entry.tcpConnectionRemAddressType = 2;
            else
            {
                ipcom_printf("mib2 :: invalid address (%s)!"IP_LF, argv[6]);
                return -1;
            }
            entry.tcpConnectionRemPort = ipcom_atoi(argv[7]);

            wret = ipcom_mib2_set_tcpConnectionState(&entry, (Ipcom_mib2_Integer)val1);
        }
#ifdef IPCOM_USE_INET
        else if (ipcom_strcmp(argv[2], "ipForwarding") == 0)
        {
            wret = ipcom_mib2_set_ipForwarding((Ipcom_mib2_Integer)val1);
        }
        else if (ipcom_strcmp(argv[2], "ipDefaultTTL") == 0)
        {
            wret = ipcom_mib2_set_ipDefaultTTL((Ipcom_mib2_Integer)val1);
        }
        else if (ipcom_strcmp(argv[2], "ipv4InterfaceEnableStatus") == 0)
        {
            if(argc < 5)
            {
                ipcom_printf("mib2 :: no value2 specified!"IP_LF);
                return -1;
            }
            /* Get new variable value2 */
            val2 = ipcom_atoi(argv[4]);
            wret = ipcom_mib2_set_ipv4InterfaceEnableStatus((Ipcom_mib2_InterfaceIndex)val1, (Ipcom_mib2_Integer)val2);
        }
#endif
#ifdef IPCOM_USE_INET6
        else if (ipcom_strcmp(argv[2], "ipv6IpForwarding") == 0)
        {
            wret = ipcom_mib2_set_ipv6IpForwarding((Ipcom_mib2_Integer)val1);
        }
        else if (ipcom_strcmp(argv[2], "ipv6IpDefaultHopLimit") == 0)
        {
            wret = ipcom_mib2_set_ipv6IpDefaultHopLimit((Ipcom_mib2_Integer)val1);
        }
        else if (ipcom_strcmp(argv[2], "ipv6InterfaceEnableStatus") == 0)
        {
            if(argc < 5)
            {
                ipcom_printf("mib2 :: no value2 specified!"IP_LF);
                return -1;
            }
            /* Get new variable value2 */
            val2 = ipcom_atoi(argv[4]);
            wret = ipcom_mib2_set_ipv6InterfaceEnableStatus((Ipcom_mib2_InterfaceIndex)val1, (Ipcom_mib2_Integer)val2);
        }
        else if (ipcom_strcmp(argv[2], "ipv6InterfaceForwarding") == 0)
        {
            if(argc < 5)
            {
                ipcom_printf("mib2 :: no value2 specified!"IP_LF);
                return -1;
            }
            /* Get new variable value2 */
            val2 = ipcom_atoi(argv[4]);
            wret = ipcom_mib2_set_ipv6InterfaceForwarding((Ipcom_mib2_InterfaceIndex)val1, (Ipcom_mib2_Integer)val2);
        }
        else if (ipcom_strcmp(argv[2], "ipv6RouterAdvertSpinLock") == 0)
        {
            wret = ipcom_mib2_set_ipv6RouterAdvertSpinLock((Ipcom_mib2_Integer)val1);
        }
#endif
        else
        {
            ipcom_printf("mib2 :: unknown variable (%s)!"IP_LF, argv[2]);
            ret = -1;
            goto exit;
        }

        if (wret)
        {
            ipcom_printf("mib2 :: failed to set variable!"IP_LF);
            ret = -1;
            goto exit;
        }
    }
    else
    {
        ipcom_printf("mib2 :: unknown command (%s)!"IP_LF, argv[1]);
        return -1;
    }

exit:
    if (user_cb.buf != IP_NULL)
        ipcom_free(user_cb.buf);
    return ret;
}

#else
int ipcom_cmd_mib2_empty_file;
#endif /* #ifdef IPCOM_USE_MIB2_CMD */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
