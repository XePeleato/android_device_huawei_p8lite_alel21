/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_mib2.h,v $ $Revision: 1.15 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_mib2.h,v $
 *   $Author: dlkrejsa $ $Date: 2008-09-16 19:14:26 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_MIB2_H
#define IPCOM_MIB2_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * MIB-2 macros and structures.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_mib2_api.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    IPCOM_MIB2
 *===========================================================================
 * Macros for system wide MIB-2 counters
 */
#ifdef IPCOM_USE_MIB2
# if defined(IPNET)
#  define IPCOM_MIB2(xentry)                       ipnet->mib2.xentry
#  ifdef IPCOM_USE_MIB2_NEW
#   define IPCOM_MIB2_U64_INC(xentry)              IP_U64_ADD32(ipnet->mib2.xentry, 1)
#   define IPCOM_MIB2_SYSWI_U32_ADD(ver, var, val) ipnet->mib2.syswide.ver.var += val
#   define IPCOM_MIB2_SYSWI_U64_ADD(ver, var, val) IP_U64_ADD32(ipnet->mib2.syswide.ver.var, val)
#  else
#   define IPCOM_MIB2_U64_INC(xentry)              IP_NOOP
#   define IPCOM_MIB2_SYSWI_U32_ADD(ver, var, val) IP_NOOP
#   define IPCOM_MIB2_SYSWI_U64_ADD(ver, var, val) IP_NOOP
#  endif
# elif defined(IPLITE)
#  define IPCOM_MIB2(xentry)                       (&iplite)->mib2.xentry
#  ifdef IPCOM_USE_MIB2_NEW
#   define IPCOM_MIB2_U64_INC(xentry)              IP_U64_ADD32((&iplite)->mib2.xentry, 1)
#   define IPCOM_MIB2_SYSWI_U32_ADD(ver, var, val) (&iplite)->mib2.syswide.ver.var += val
#   define IPCOM_MIB2_SYSWI_U64_ADD(ver, var, val) IP_U64_ADD32((&iplite)->mib2.syswide.ver.var, val)
#  else
#   define IPCOM_MIB2_U64_INC(xentry)              IP_NOOP
#   define IPCOM_MIB2_SYSWI_U32_ADD(ver, var, val) IP_NOOP
#   define IPCOM_MIB2_SYSWI_U64_ADD(ver, var, val) IP_NOOP
#  endif
# else
#  error The IPCOM_USE_MIB2 macro requires either IPNET or IPLITE
# endif
#else /* #ifdef IPCOM_USE_MIB2 */
# define IPCOM_MIB2(xentry)                        IP_NOOP
# define IPCOM_MIB2_U64_INC(xentry)                IP_NOOP
# define IPCOM_MIB2_SYSWI_U32_ADD(ver, var, val)   IP_NOOP
# define IPCOM_MIB2_SYSWI_U64_ADD(ver, var, val)   IP_NOOP
#endif


/*
 *===========================================================================
 *                    IPCOM_MIB2_NETIF
 *===========================================================================
 * Macros for per interface MIB-2 counters
 */
#ifdef IPCOM_USE_MIB2
# define IPCOM_MIB2_NETIF2(xentry, netif)              do \
                                                       { \
                                                           ip_assert(netif != IP_NULL); \
                                                           if(netif != IP_NULL) \
                                                               netif->ipcom.mib2.xentry; \
                                                       } \
                                                       while((0))
# define IPCOM_MIB2_NETIF(xentry) IPCOM_MIB2_NETIF2(xentry, netif)
# ifdef IPCOM_USE_MIB2_NEW
#  define IPCOM_MIB2_PERIF_U32_ADD(ver, var, val, ifc) do \
                                                       { \
                                                           ip_assert(ifc != IP_NULL); \
                                                           if(ifc != IP_NULL) \
                                                               ifc->ipcom.mib2.perif.ver.var += val; \
                                                       } \
                                                       while((0))
#  define IPCOM_MIB2_PERIF_U64_ADD(ver, var, val, ifc) do \
                                                       { \
                                                           ip_assert(ifc != IP_NULL); \
                                                           if(ifc != IP_NULL) \
                                                               IP_U64_ADD32(ifc->ipcom.mib2.perif.ver.var, val); \
                                                       } \
                                                       while((0))
# else  /* #ifdef IPCOM_USE_MIB2_NEW */
#  define IPCOM_MIB2_PERIF_U32_ADD(ver, var, val, ifc) IP_NOOP
#  define IPCOM_MIB2_PERIF_U64_ADD(ver, var, val, ifc) IP_NOOP
# endif
#else /* #ifdef IPCOM_USE_MIB2 */
# define IPCOM_MIB2_NETIF2(xentry, netif)              IP_NOOP
# define IPCOM_MIB2_NETIF(xentry)                      IP_NOOP
# define IPCOM_MIB2_PERIF_U32_ADD(ver, var, val, ifc)  IP_NOOP
# define IPCOM_MIB2_PERIF_U64_ADD(ver, var, val, ifc)  IP_NOOP
#endif


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    Ipcom_mib2_stats_syswide
 *===========================================================================
 */
typedef struct Ipcom_mib2_stats_syswide_struct
{
    /* ipSystemStatsTable */
    Ip_u64 ipSystemStatsHCInReceives;        /* ipSystemStatsInReceives in low 32 bits */
    Ip_u64 ipSystemStatsHCInOctets;          /* ipSystemStatsInOctets in low 32 bits */
    Ip_u32 ipSystemStatsInHdrErrors;
    Ip_u32 ipSystemStatsInNoRoutes;
    Ip_u32 ipSystemStatsInAddrErrors;
    Ip_u32 ipSystemStatsInUnknownProtos;
    Ip_u32 ipSystemStatsInTruncatedPkts;
    Ip_u64 ipSystemStatsHCInForwDatagrams;   /* ipSystemStatsInForwDatagrams in low 32 bits */
    Ip_u32 ipSystemStatsReasmReqds;
    Ip_u32 ipSystemStatsReasmOKs;
    Ip_u32 ipSystemStatsReasmFails;
    Ip_u32 ipSystemStatsInDiscards;
    Ip_u64 ipSystemStatsHCInDelivers;        /* ipSystemStatsInDelivers in low 32 bits */
    Ip_u64 ipSystemStatsHCOutRequests;       /* ipSystemStatsOutRequests in low 32 bits */
    Ip_u32 ipSystemStatsOutNoRoutes;
    Ip_u64 ipSystemStatsHCOutForwDatagrams;  /* ipSystemStatsOutForwDatagrams in low 32 bits */
    Ip_u32 ipSystemStatsOutDiscards;
    Ip_u32 ipSystemStatsOutFragReqds;
    Ip_u32 ipSystemStatsOutFragOKs;
    Ip_u32 ipSystemStatsOutFragFails;
    Ip_u32 ipSystemStatsOutFragCreates;
    Ip_u64 ipSystemStatsHCOutTransmits;      /* ipSystemStatsOutTransmits in low 32 bits */
    Ip_u64 ipSystemStatsHCOutOctets;         /* ipSystemStatsOutOctets in low 32 bits */
    Ip_u64 ipSystemStatsHCInMcastPkts;       /* ipSystemStatsInMcastPkts in low 32 bits */
    Ip_u64 ipSystemStatsHCInMcastOctets;     /* ipSystemStatsInMcastOctets in low 32 bits */
    Ip_u64 ipSystemStatsHCOutMcastPkts;      /* ipSystemStatsOutMcastPkts in low 32 bits */
    Ip_u64 ipSystemStatsHCOutMcastOctets;    /* ipSystemStatsOutMcastOctets in low 32 bits */
    Ip_u64 ipSystemStatsHCInBcastPkts;       /* ipSystemStatsInBcastPkts in low 32 bits */
    Ip_u64 ipSystemStatsHCOutBcastPkts;      /* ipSystemStatsOutBcastPkts in low 32 bits */

    /* icmpStatsTable */
    Ip_u32 icmpStatsInMsgs;
    Ip_u32 icmpStatsInErrors;
    Ip_u32 icmpStatsOutMsgs;
    Ip_u32 icmpStatsOutErrors;

    /* icmpMsgStatsTable */
    Ip_u32 icmpMsgStatsInPkts[256];          /* array is indexed by icmp type */
    Ip_u32 icmpMsgStatsOutPkts[256];         /* array is indexed by icmp type */
}
Ipcom_mib2_stats_syswide;


/*
 *===========================================================================
 *                    Ipcom_mib2_stats_perif
 *===========================================================================
 */
typedef struct Ipcom_mib2_stats_perif_struct
{
    /* ipIfStatsTable */
    Ip_u64 ipIfStatsHCInReceives;        /* ipIfStatsInReceives in low 32 bits */
    Ip_u64 ipIfStatsHCInOctets;          /* ipIfStatsInOctets in low 32 bits */
    Ip_u32 ipIfStatsInHdrErrors;
    Ip_u32 ipIfStatsInNoRoutes;
    Ip_u32 ipIfStatsInAddrErrors;
    Ip_u32 ipIfStatsInUnknownProtos;
    Ip_u32 ipIfStatsInTruncatedPkts;
    Ip_u64 ipIfStatsHCInForwDatagrams;   /* ipIfStatsInForwDatagrams in low 32 bits */
    Ip_u32 ipIfStatsReasmReqds;
    Ip_u32 ipIfStatsReasmOKs;
    Ip_u32 ipIfStatsReasmFails;
    Ip_u32 ipIfStatsInDiscards;
    Ip_u64 ipIfStatsHCInDelivers;        /* ipIfStatsInDelivers in low 32 bits */
    Ip_u64 ipIfStatsHCOutRequests;       /* ipIfStatsOutRequests in low 32 bits */
    Ip_u64 ipIfStatsHCOutForwDatagrams;  /* ipIfStatsOutForwDatagrams in low 32 bits */
    Ip_u32 ipIfStatsOutDiscards;
    Ip_u32 ipIfStatsOutFragReqds;
    Ip_u32 ipIfStatsOutFragOKs;
    Ip_u32 ipIfStatsOutFragFails;
    Ip_u32 ipIfStatsOutFragCreates;
    Ip_u64 ipIfStatsHCOutTransmits;      /* ipIfStatsOutTransmits in low 32 bits */
    Ip_u64 ipIfStatsHCOutOctets;         /* ipIfStatsOutOctets in low 32 bits */
    Ip_u64 ipIfStatsHCInMcastPkts;       /* ipIfStatsInMcastPkts in low 32 bits */
    Ip_u64 ipIfStatsHCInMcastOctets;     /* ipIfStatsInMcastOctets in low 32 bits */
    Ip_u64 ipIfStatsHCOutMcastPkts;      /* ipIfStatsOutMcastPkts in low 32 bits */
    Ip_u64 ipIfStatsHCOutMcastOctets;    /* ipIfStatsOutMcastOctets in low 32 bits */
    Ip_u64 ipIfStatsHCInBcastPkts;       /* ipIfStatsInBcastPkts in low 32 bits */
    Ip_u64 ipIfStatsHCOutBcastPkts;      /* ipIfStatsOutBcastPkts in low 32 bits */
}
Ipcom_mib2_stats_perif;


/*
 *===========================================================================
 *                    Ipcom_mib2_stats
 *===========================================================================
 *
 */
typedef struct Ipcom_mib2_stats_struct
{
#ifdef IPCOM_USE_INET
    /* the IP group */
    Ip_u32 ipInReceives;
    Ip_u32 ipInHdrErrors;
    Ip_u32 ipInAddrErrors;
    Ip_u32 ipForwDatagrams;
    Ip_u32 ipInUnknownProtos;
    Ip_u32 ipInDiscards;
    Ip_u32 ipInDelivers;
    Ip_u32 ipOutRequests;
    Ip_u32 ipOutDiscards;
    Ip_u32 ipOutNoRoutes;
    Ip_s32 ipReasmTimeout;
    Ip_u32 ipReasmReqds;
    Ip_u32 ipReasmOKs;
    Ip_u32 ipReasmFails;
    Ip_u32 ipFragOKs;
    Ip_u32 ipFragFails;
    Ip_u32 ipFragCreates;
    Ip_u32 ipRoutingDiscards;

    /* the ICMP group */
    Ip_u32 icmpInMsgs;
    Ip_u32 icmpInErrors;
    Ip_u32 icmpInDestUnreachs;
    Ip_u32 icmpInTimeExcds;
    Ip_u32 icmpInParmProbs;
    Ip_u32 icmpInSrcQuenchs;
    Ip_u32 icmpInRedirects;
    Ip_u32 icmpInEchos;
    Ip_u32 icmpInEchoReps;
    Ip_u32 icmpInTimestamps;
    Ip_u32 icmpInTimestampReps;
    Ip_u32 icmpInAddrMasks;
    Ip_u32 icmpInAddrMaskReps;
    Ip_u32 icmpOutMsgs;
    Ip_u32 icmpOutErrors;
    Ip_u32 icmpOutDestUnreachs;
    Ip_u32 icmpOutTimeExcds;
    Ip_u32 icmpOutParmProbs;
    Ip_u32 icmpOutSrcQuenchs;
    Ip_u32 icmpOutRedirects;
    Ip_u32 icmpOutEchos;
    Ip_u32 icmpOutEchoReps;
    Ip_u32 icmpOutTimestamps;
    Ip_u32 icmpOutTimestampReps;
    Ip_u32 icmpOutAddrMasks;
    Ip_u32 icmpOutAddrMaskReps;
#endif

    /* the TCP group */
    Ip_s32 tcpRtoAlgorithm;
    Ip_s32 tcpRtoMin;
    Ip_s32 tcpRtoMax;
    Ip_s32 tcpMaxConn;
    Ip_u32 tcpActiveOpens;
    Ip_u32 tcpPassiveOpens;
    Ip_u32 tcpAttemptFails;
    Ip_u32 tcpEstabResets;
    Ip_u32 tcpInSegs;
    Ip_u32 tcpOutSegs;
    Ip_u32 tcpRetransSegs;
    Ip_u32 tcpInErrs;
    Ip_u32 tcpOutRsts;

    /* the UDP group */
    Ip_u32 udpInDatagrams;
    Ip_u32 udpNoPorts;
    Ip_u32 udpInErrors;
    Ip_u32 udpOutDatagrams;

#ifdef IPCOM_USE_INET
    /* the interfaces table */
    Ip_u32 ifTableLastChange;
#endif

#ifdef IPCOM_USE_INET6
    /* the IPv6 general group */
    Ip_u32 ipv6IfTableLastChange;
    Ip_u32 ipv6DiscardedRoutes;
#endif

    /* RFC4293 system wide statistics */
#ifdef IPCOM_USE_MIB2_NEW
    Ip_u32 ipIfStatsTableLastChange;
    Ip_s32 ipAddressSpinLock;
# ifdef IPCOM_USE_INET
    Ip_u32 ipv4InterfaceTableLastChange;
# endif
# ifdef IPCOM_USE_INET6
    Ip_u32 ipv6InterfaceTableLastChange;
    Ip_s32 ipv6RouterAdvertSpinLock;
# endif

    struct
    {
# ifdef IPCOM_USE_INET
        Ipcom_mib2_stats_syswide v4;
# endif
# ifdef IPCOM_USE_INET6
        Ipcom_mib2_stats_syswide v6;
# endif
    }
    syswide;

    /* RFC 4113 */
    Ip_u64 udpHCInDatagrams;
    Ip_u64 udpHCOutDatagrams;

    /* RFC 4022 */
    Ip_u64 tcpHCInSegs;
    Ip_u64 tcpHCOutSegs;

    /* Function to use for sysUpTime */
    Ipcom_mib2_sysUpTimeFunc sysUpTimeFp;
#endif /* #ifdef IPCOM_USE_MIB2_NEW */
}
Ipcom_mib2_stats;


/*
 *===========================================================================
 *                    Ipcom_mib2_stats_netif
 *===========================================================================
 *
 */
typedef struct Ipcom_mib2_stats_netif_struct
{
    /* IfTable */
    Ip_u32 ifLastChange;
    Ip_u32 ifInOctets;
    Ip_u32 ifInUcastPkts;
    Ip_u32 ifInDiscards;
    Ip_u32 ifInErrors;
    Ip_u32 ifInUnknownProtos;
    Ip_u32 ifOutOctets;
    Ip_u32 ifOutUcastPkts;
    Ip_u32 ifOutDiscards;
    Ip_u32 ifOutErrors;

    /* IfXTable */
    Ip_u32 ifInMulticastPkts;
    Ip_u32 ifInBroadcastPkts;
    Ip_u32 ifOutMulticastPkts;
    Ip_u32 ifOutBroadcastPkts;
    Ip_s32 ifLinkUpDownTrapEnable;

#ifdef IPCOM_USE_INET6
    /* ipv6IfTable */
    Ip_u32 ipv6IfLastChange;

    /* ipv6IfStatsTable */
    Ip_u32 ipv6IfStatsInReceives;
    Ip_u32 ipv6IfStatsInHdrErrors;
    Ip_u32 ipv6IfStatsInTooBigErrors;
    Ip_u32 ipv6IfStatsInNoRoutes;
    Ip_u32 ipv6IfStatsInAddrErrors;
    Ip_u32 ipv6IfStatsInUnknownProtos;
    Ip_u32 ipv6IfStatsInTruncatedPkts;
    Ip_u32 ipv6IfStatsInDiscards;
    Ip_u32 ipv6IfStatsInDelivers;
    Ip_u32 ipv6IfStatsOutForwDatagrams;
    Ip_u32 ipv6IfStatsOutRequests;
    Ip_u32 ipv6IfStatsOutDiscards;
    Ip_u32 ipv6IfStatsOutFragOKs;
    Ip_u32 ipv6IfStatsOutFragFails;
    Ip_u32 ipv6IfStatsOutFragCreates;
    Ip_u32 ipv6IfStatsReasmReqds;
    Ip_u32 ipv6IfStatsReasmOKs;
    Ip_u32 ipv6IfStatsReasmFails;
    Ip_u32 ipv6IfStatsInMcastPkts;
    Ip_u32 ipv6IfStatsOutMcastPkts;

    Ip_u32 ipv6IfIcmpInMsgs;
    Ip_u32 ipv6IfIcmpInErrors;
    Ip_u32 ipv6IfIcmpInDestUnreachs;
    Ip_u32 ipv6IfIcmpInAdminProhibs;
    Ip_u32 ipv6IfIcmpInTimeExcds;
    Ip_u32 ipv6IfIcmpInParmProblems;
    Ip_u32 ipv6IfIcmpInPktTooBigs;
    Ip_u32 ipv6IfIcmpInEchos;
    Ip_u32 ipv6IfIcmpInEchoReplies;
    Ip_u32 ipv6IfIcmpInRouterSolicits;
    Ip_u32 ipv6IfIcmpInRouterAdvertisements;
    Ip_u32 ipv6IfIcmpInNeighborSolicits;
    Ip_u32 ipv6IfIcmpInNeighborAdvertisements;
    Ip_u32 ipv6IfIcmpInRedirects;
    Ip_u32 ipv6IfIcmpInGroupMembQueries;
    Ip_u32 ipv6IfIcmpInGroupMembResponses;
    Ip_u32 ipv6IfIcmpInGroupMembReductions;
    Ip_u32 ipv6IfIcmpOutMsgs;
    Ip_u32 ipv6IfIcmpOutErrors;
    Ip_u32 ipv6IfIcmpOutDestUnreachs;
    Ip_u32 ipv6IfIcmpOutAdminProhibs;
    Ip_u32 ipv6IfIcmpOutTimeExcds;
    Ip_u32 ipv6IfIcmpOutParmProblems;
    Ip_u32 ipv6IfIcmpOutPktTooBigs;
    Ip_u32 ipv6IfIcmpOutEchos;
    Ip_u32 ipv6IfIcmpOutEchoReplies;
    Ip_u32 ipv6IfIcmpOutRouterSolicits;
    Ip_u32 ipv6IfIcmpOutRouterAdvertisements;
    Ip_u32 ipv6IfIcmpOutNeighborSolicits;
    Ip_u32 ipv6IfIcmpOutNeighborAdvertisements;
    Ip_u32 ipv6IfIcmpOutRedirects;
    Ip_u32 ipv6IfIcmpOutGroupMembQueries;
    Ip_u32 ipv6IfIcmpOutGroupMembResponses;
    Ip_u32 ipv6IfIcmpOutGroupMembReductions;
#endif

    /* RFC4293 per interface statistics */
#ifdef IPCOM_USE_MIB2_NEW
    struct
    {
# ifdef IPCOM_USE_INET
        Ipcom_mib2_stats_perif v4;
# endif
# ifdef IPCOM_USE_INET6
        Ipcom_mib2_stats_perif v6;
# endif
    }
    perif;
#endif /* #ifdef IPCOM_USE_MIB2_NEW */
}
Ipcom_mib2_stats_netif;


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef IPCOM_MIB2_H */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
