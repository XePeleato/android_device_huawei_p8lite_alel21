/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_cmd.c,v $ $Revision: 1.62.12.1.4.2 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_cmd.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
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
#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_clib.h>
#include <ipcom_sock.h>

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_cmd.h"
#include "ipnet_routesock.h"


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

#ifdef IPNET_USE_NAT
/* Protocol number to name mappings */
IP_CONST struct protocol
{
    Ip_u8 number;
    char *name;
} ipnet_nat_protocols[] = {{IP_IPPROTO_IP,     "ip"},
                           {IP_IPPROTO_ICMP,   "icmp"},
                           {IP_IPPROTO_IGMP,   "igmp"},
                           {IP_IPPROTO_TCP,    "tcp"},
                           {IP_IPPROTO_UDP,    "udp"},
                           {IP_IPPROTO_GRE,    "gre"},
                           {IP_IPPROTO_ESP,    "esp"},
                           {IP_IPPROTO_AH,     "ah"},
                           {IP_IPPROTO_IPV4,   "ipv4"},
                           {IP_IPPROTO_IPV6,   "ipv6"},
                           {IP_IPPROTO_RAW,    "raw"},
                           {IP_IPPROTO_ICMPV6, "ipv6-icmp"}};
#endif  /* #ifdef IPNET_USE_NAT */


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_cmd_rt_flags_to_str
 *===========================================================================
 * Description: Returns a string representation of the IPNET_RTF_xxx flags.
 * Parameters:  flags - The IPNET_RTF_xxx flags.
 *              str - The buffer to put the flag string form.
 *              len - Length of the str buffer.
 * Returns:     The string representation.
 *
 */
IP_GLOBAL IP_CONST char *
ipnet_cmd_rt_flags_to_str(Ip_u32 flags, char *str, Ip_size_t len)
{
    str[0] = '\0';
    ipcom_snprintf(str, len, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
                   flags & IPNET_RTF_UP ? "UP " : "",
                   flags & IPNET_RTF_GATEWAY ? "GATEWAY " : "",
                   flags & IPNET_RTF_HOST ? "HOST " : "",
                   flags & IPNET_RTF_REJECT ? "REJECT " : "",
                   flags & IPNET_RTF_DYNAMIC ? "DYNAMIC " : "",
                   flags & IPNET_RTF_MODIFIED ? "MODIFIED " : "",
                   flags & IPNET_RTF_DONE ? "DONE " : "",
                   flags & IPNET_RTF_MASK ? "MASK " : "",
                   flags & IPNET_RTF_CLONING ? "CLONING " : "",
                   flags & IPNET_RTF_XRESOLVE ? "XRESOLVE " : "",
                   flags & IPNET_RTF_LLINFO ? "LLINFO " : "",
                   flags & IPNET_RTF_STATIC ? "STATIC " : "",
                   flags & IPNET_RTF_BLACKHOLE ? "BLACKHOLE " : "",
                   flags & IPNET_RTF_PROTO1 ? "PROTO1 " : "",
                   flags & IPNET_RTF_PROTO2 ? "PROTO2 " : "",
                   flags & IPNET_RTF_PREF ? "PREF " : "",
                   flags & IPNET_RTF_TUNNELEND ? "TUNNELEND " : "",
                   flags & IPNET_RTF_SKIP ? "SKIP " : "");
    return str;
}


/*
 *===========================================================================
 *                    ipnet_cmd_rt_flags_to_str_short
 *===========================================================================
 * Description: Returns a string representation of the IPNET_RTF_xxx flags.
 * Parameters:  flags - The IPNET_RTF_xxx flags.
 *              str - The buffer to put the flag string form.
 *              len - Length of the str buffer.
 * Returns:     The string representation.
 *
 */
IP_GLOBAL IP_CONST char *
ipnet_cmd_rt_flags_to_str_short(Ip_u32 flags, Ip_bool full)
{
    static char str[20];
    ipcom_snprintf(str, sizeof(str) - 1, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
                   flags & IPNET_RTF_UP ? "U" : "",
                   flags & IPNET_RTF_GATEWAY ? "G" : "",
                   flags & IPNET_RTF_HOST ? "H" : "",
                   flags & IPNET_RTF_REJECT ? "R" : "",
                   flags & IPNET_RTF_DYNAMIC ? "D" : "",
                   flags & IPNET_RTF_MODIFIED ? "m" : "",
                   (full && (flags & IPNET_RTF_DONE)) ? "d" : "",
                   (full && (flags & IPNET_RTF_MASK)) ? "M" : "",
                   flags & IPNET_RTF_CLONING ? "C" : "",
                   flags & IPNET_RTF_XRESOLVE ? "X" : "",
                   flags & IPNET_RTF_LLINFO ? "L" : "",
                   flags & IPNET_RTF_STATIC ? "S" : "",
                   flags & IPNET_RTF_BLACKHOLE ? "B" : "",
                   flags & IPNET_RTF_PROTO1 ? "1" : "",
                   flags & IPNET_RTF_PROTO2 ? "2" : "",
                   flags & IPNET_RTF_PREF ? "P" : "",
                   flags & IPNET_RTF_TUNNELEND ? "T" : "",
                   flags & IPNET_RTF_MBINDING ? "+" : "",
                   flags & IPNET_RTF_SKIP ? "-" : "");
    return str;
}


/*
 *===========================================================================
 *                    ipnet_cmd_if_type_to_str
 *===========================================================================
 * Description: Returns a string representation of the IP_IFT_xxx constants.
 * Parameters:  type - The IP_IFT_xxx type.
 *              str - The buffer to put the flag string form.
 *              len - Length of the str buffer.
 * Returns:     The string representation.
 *
 */
IP_GLOBAL IP_CONST char *
ipnet_cmd_if_type_to_str(int type, char *str, Ip_size_t len)
{
    switch (type)
    {
    case IP_IFT_ETHER:
        ipcom_strncpy(str, "Ethernet", len);
        break;
    case IP_IFT_L2VLAN:
        ipcom_strncpy(str, "Layer 2 virtual LAN", len);
        break;
    case IP_IFT_PPP:
        ipcom_strncpy(str, "Point to point", len);
        break;
    case IP_IFT_PPPOE:
        ipcom_strncpy(str, "PPP over Ethernet", len);
        break;
    case IP_IFT_LOOP:
        ipcom_strncpy(str, "Local loopback", len);
        break;
    case IP_IFT_TUNNEL:
        ipcom_strncpy(str, "Tunnel", len);
        break;
    case IP_IFT_SOCKDEV:
        ipcom_strncpy(str, "Socket Device", len);
        break;
    case IP_IFT_PPPL2TP:
        ipcom_strncpy(str, "PPP over L2TP", len);
        break;
    default:
        ipcom_snprintf(str, len, "%d", type);
        break;
    }
    return str;
}


/*
 *===========================================================================
 *                    ipnet_cmd_init_addrs
 *===========================================================================
 * Description: Sets the address pointer to point to the correct entry
 *              in the 'rt' buffer.
 * Parameters:  msg - The first socket address in the route socket message.
 *              addrs - The IPNET_RTA_xxx from the message.
 *              sas - Will contain the route pointer.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_cmd_init_addrs(void *msg, Ip_u32 addrs, struct Ip_sockaddr **sas)
{
    int       i;
    Ip_size_t offset = 0;

    for (i = 0; i < IPNET_RTAX_MAX; i++)
    {
        if (IP_BIT_ISSET(addrs, 1 << i))
        {
            sas[i] = (struct Ip_sockaddr *) &((Ip_u8 *) msg)[offset];
            offset += IPCOM_SA_LEN_GET(sas[i]) ?
                IP_ROUNDUP(IPCOM_SA_LEN_GET(sas[i]), sizeof(Ip_u32)) : sizeof(Ip_u32);
        }
        else
            sas[i] = IP_NULL;
    }
}


/*
 *===========================================================================
 *                          ipnet_cmd_msec_since
 *===========================================================================
 * Description: Returns how many milliseconds has passed since "base".
 * Parameters:  base - time to be used as "beginning of time".
 * Returns:
 *
 */
IP_GLOBAL unsigned
ipnet_cmd_msec_since(struct Ip_timeval *base)
{
    struct Ip_timeval tv;

    ipcom_microtime(&tv);
    ip_timeval_sub(&tv, base, &tv);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}


/*
 *===========================================================================
 *                        ipnet_bit_array_clr
 *===========================================================================
 * Description: Clears a bit
 * Parameters:  bit_array - a bit array
 *              bit - to clear
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_bit_array_clr(void *bit_array, unsigned bit)
{
    IP_BIT_CLR(((char *)bit_array)[bit / 8], 1 << (bit % 8));
}


/*
 *===========================================================================
 *                    ipnet_bit_array_test_and_set
 *===========================================================================
 * Description: Sets the bit return the state of the bit before this operation.
 * Parameters:  bit_array - a bit array
 *              bit - bit to test and then set.
 * Returns:     IP_TRUE if the bit was already set
 *              IP_FALSE if the bit was not set
 *
 */
IP_GLOBAL Ip_bool
ipnet_bit_array_test_and_set(void *bit_array, unsigned bit)
{
    unsigned bit8 = 1 << (bit % 8);
    char *arr_ptr = bit_array;

    if (IP_BIT_ISSET(arr_ptr[bit / 8], bit8))
        return IP_TRUE;

    IP_BIT_SET(arr_ptr[bit / 8], bit8);
    return IP_FALSE;
}


/*
 *===========================================================================
 *                    ipnet_nat_protocol_number
 *===========================================================================
 * Description: Convert a protocol name to protocol number.
 * Parameters:  name - pointer to string with name.
 * Returns:     protocol number or -1 if not found.
 */
#ifdef IPNET_USE_NAT
IP_GLOBAL Ip_s32
ipnet_nat_protocol_number(const char *name)
{
    Ip_u32 i, num_protocols;
    num_protocols = sizeof(ipnet_nat_protocols) / sizeof(struct protocol);

    for(i=0; i<num_protocols; i++)
        if(!ipcom_strcasecmp(ipnet_nat_protocols[i].name, name))
            return (Ip_s32)ipnet_nat_protocols[i].number;

    return -1;      /* Not found */
}
#endif  /* #ifdef IPNET_USE_NAT */


/*
 *===========================================================================
 *                    ipnet_nat_protocol_name
 *===========================================================================
 * Description: Convert a protocol number to protocol name.
 * Parameters:  protocol - protocol number.
 * Returns:     pointer to string with protocol name or the protocol number.
 */
#ifdef IPNET_USE_NAT
IP_GLOBAL char *
ipnet_nat_protocol_name(Ip_u8 protocol, char *protostr, Ip_u32 protostrlen)
{
    Ip_u32 i, num_protocols;
    num_protocols = sizeof(ipnet_nat_protocols) / sizeof(struct protocol);

    if (protostrlen == 0)
        return protostr;
    ipcom_memset(protostr, 0, protostrlen);

    ipcom_snprintf(protostr, protostrlen-1, "%d", protocol);
    for(i=0; i<num_protocols; i++)
        if(ipnet_nat_protocols[i].number == protocol)
            ipcom_strncpy(protostr, ipnet_nat_protocols[i].name, protostrlen-1);

    return protostr;
}
#endif  /* #ifdef IPNET_USE_NAT */


/*
 *===========================================================================
 *                    ipnet_ping_stat_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_ping_stat_init(Ipnet_cmd_ping_stat_t *stat)
{
    struct Ip_timeval now;

    ipcom_memset(stat, 0, sizeof(*stat));
    stat->rtt_min = ~0u;
    ipcom_microtime(&now);
    stat->start_time = IPNET_TIMEVAL_TO_MSEC(now);
}


/*
 *===========================================================================
 *                    ipnet_ping_stat_transmit
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_ping_stat_transmit(Ipnet_cmd_ping_stat_t *stat)
{
    stat->transmitted++;
}


/*
 *===========================================================================
 *                    ipnet_ping_stat_receive
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_ping_stat_receive(Ipnet_cmd_ping_stat_t *stat,
                        unsigned elapsed_time,
                        Ip_bool duplicate)
{
    if (duplicate)
        stat->duplicates++;
    else
        stat->received++;
    stat->rtt_min  = IP_MIN(stat->rtt_min, elapsed_time);
    stat->rtt_max  = IP_MAX(stat->rtt_max, elapsed_time);
    stat->rtt_sum += elapsed_time;
}


/*
 *===========================================================================
 *                    ipnet_ping_print_stat
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_ping_stat_report(Ipnet_cmd_ping_stat_t *stat, const char *peer)
{
    struct Ip_timeval now;

    ipcom_microtime(&now);

    ipcom_printf("--- %s ping statistics ---" IP_LF, peer);
    ipcom_printf("%u packets transmitted, %u received, ",
                 stat->transmitted,
                 stat->received);
    if (stat->duplicates > 0)
        ipcom_printf("+%u duplicates, ", stat->duplicates);
    ipcom_printf("%u%% packet loss, time %u ms" IP_LF,
                 stat->received == 0 ? 100 : 100 * (stat->transmitted - stat->received) / stat->transmitted,
                 IPNET_TIMEVAL_TO_MSEC(now) - stat->start_time);

    if (stat->received > 0)
    {
        ipcom_printf("rtt min/avg/max = %u/%u/%u ms" IP_LF,
                     stat->rtt_min,
                     stat->rtt_sum / (stat->received + stat->duplicates),
                     stat->rtt_max);
    }
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

