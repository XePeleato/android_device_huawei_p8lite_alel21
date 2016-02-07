/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipipsec_debug.c,v $ $Revision: 1.10 $
 *   $Source: /home/interpeak/CVSRoot/ipipsec2/src/ipipsec_debug.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
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
#include "ipipsec_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_syslog.h>
#include <ipcom_sock.h>

#include "ipipsec_h.h"
#include "ipipsec_pfkeyv2.h"


/*
 ****************************************************************************
 * 4                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 5                    DEFINES
 ****************************************************************************
 */

#define IPCOM_SYSLOG_PRIORITY    IPIPSEC_SYSTEM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPIPSEC


/*
 ****************************************************************************
 * 6                    TYPES
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

IP_GLOBAL IP_CONST char *ipipsec_pfkeyv2_command[] =
{
    "RESERVED",
    "GETSPI",
    "UPDATE",
    "ADD",
    "DELETE",
    "GET",
    "ACQUIRE",
    "REGISTER",
    "EXPIRE",
    "FLUSH",
    "DUMP",
    "X_PROMISC",
    "X_ADDFLOW",
    "X_DELFLOW",
    "X_GRPSPIS",
    "X_BINDSA",
    "X_FINDFLOW",
    "X_DUMPFLOW"
};


/*
 ****************************************************************************
 *  9                   STATIC FUNCTIONS
 ****************************************************************************
 */


/*
 ****************************************************************************
 * 10                    GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipipsec_key_name
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL char *
ipipsec_key_name(Ipcom_pkt *pkt, Ipipsec_param *param)
{
    static char flowsel[150];
    char str[46];
    char srcport[10], dstport[32];
    int len;
#ifdef IPCOM_USE_INET
    Ip_u16  frag;
#endif

    ip_assert(pkt != IP_NULL);
    ip_assert(param != IP_NULL);
    ip_assert(param->key.direction == IPIPSEC_DIR_IN
              || param->key.direction == IPIPSEC_DIR_OUT);

    srcport[0] = '\0';
    dstport[0] = '\0';

#ifdef IPCOM_USE_INET
    if (param->key.domain == IP_AF_INET
        && (pkt->data[pkt->ipstart] & 0xf0) == 0x40
        && 0 != (frag = (IP_GET_NTOHS(&pkt->data[pkt->ipstart + 6]) & 0x1fff)))
    {
        ipcom_sprintf(dstport, " frag=%d", (int)frag << 3);
    }
    else
#endif /* IPCOM_USE_INET */
    {
        switch (param->key.proto)
        {
        case IP_IPPROTO_ESP:
        case IP_IPPROTO_AH:
            ipcom_sprintf(dstport, " spi=0x%lx",
                          ip_ntohl(param->key.ports.espah_spi_n));
            break;
        case IP_IPPROTO_TCP:
        case IP_IPPROTO_UDP:
            ipcom_sprintf(srcport, ":%d", ip_ntohs(param->key.ports.udptcp_srcdst_n[0]));
            ipcom_sprintf(dstport, ":%d", ip_ntohs(param->key.ports.udptcp_srcdst_n[1]));
            break;
        case IP_IPPROTO_ICMP:
        case IP_IPPROTO_ICMPV6:
            ipcom_sprintf(dstport, " type=%d code=%d",
                          param->key.ports.icmp_typecode[0],
                          param->key.ports.icmp_typecode[1]);
            break;
        case IP_IPPROTO_MH:
            ipcom_sprintf(dstport, " type=%d",
                          param->key.ports.mh_type[0]);
            break;
        default:
            break;
        }
    }

    len = ipcom_sprintf(flowsel, "%s %s%s -> ",
                        ipcom_ipproto_name(param->key.proto),
                        ipcom_inet_ntop(param->key.domain, &param->key.src, str, sizeof(str)),
                        srcport);
    ipcom_sprintf(flowsel + len, "%s%s payload=%d",
                  ipcom_inet_ntop(param->key.domain, &param->key.dst, str, sizeof(str)),
                  dstport,
                  pkt->end - pkt->start);

    return (char *)flowsel;
}


/*
 *===========================================================================
 *                    ipipsec_flow_name
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL char *
ipipsec_flow_name(Ipipsec_flow *flow, Ipipsec_sa *sa)
{
    static char buf[512];
    char str[46];
    int len;

    ip_assert(flow != IP_NULL);
    ip_assert(flow->magic == IPIPSEC_FLOW_MAGIC);

    len = ipcom_sprintf(buf, "#%d %s %s 'prot=%s src=%s",
                        (int)flow->id,
                        flow->flowsel.key.direction == IPIPSEC_DIR_OUT ? "output" : "input",
                        IPSECFLOWTYPENAME(flow->flowtype),
                        ipcom_ipproto_name(flow->flowsel.key.proto),
                        ipcom_inet_ntop(flow->flowsel.key.domain,
                                        &flow->flowsel.key.src,
                                        str, sizeof(str)));

    len += ipcom_sprintf(buf + len, "/%d:%d",
                         ipipsec_addrmasklen(flow->flowsel.key.domain,
                                             &flow->flowsel.mask.src),
                         (int)ip_ntohs(flow->flowsel.key.ports.udptcp_srcdst_n[0]));
    if (ip_ntohs(flow->flowsel.key.ports.udptcp_srcdst_n[0]) != flow->flowsel.src_max_port)
        len += ipcom_sprintf(buf + len, "-%d",
                             (int)flow->flowsel.src_max_port);

    len += ipcom_sprintf(buf + len, " dst=%s/",
                         ipcom_inet_ntop(flow->flowsel.key.domain,
                                         &flow->flowsel.key.dst,
                                         str, sizeof(str)));

    len += ipcom_sprintf(buf + len, "%d:%d",
                         ipipsec_addrmasklen(flow->flowsel.key.domain,
                                             &flow->flowsel.mask.dst),
                         (int)flow->flowsel.dst_max_port);
    if (flow->flowsel.dst_max_port != flow->flowsel.dst_max_port)
        len += ipcom_sprintf(buf + len, "-%d",
                             (int)flow->flowsel.dst_max_port);

    len += ipcom_sprintf(buf + len, "' -> SA: %s", ipipsec_sa_name(sa));

    return (char *)buf;
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
