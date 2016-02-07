/* ipcom_mib2_api.h - Public API of Wind River MIB-2 access routines */

/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_mib2_api.h,v $ $Revision: 1.12 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_mib2_api.h,v $
 *   $Author: markus $ $Date: 2008-04-29 12:37:55 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_MIB2_API_H
#define IPCOM_MIB2_API_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
DESCRIPTION
This library contains the APIs used to retrieve MIB-2 information from the
Wind River TCP/IP stack.
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

#ifdef __cplusplus
extern "C" {
#endif


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
 *===========================================================================
 *                    SNMPv2-SMI types
 *===========================================================================
 */
typedef Ip_u32 Ipcom_mib2_ObjectIdentifier;
typedef Ip_u8  Ipcom_mib2_OctetString;
typedef Ip_s32 Ipcom_mib2_Integer;
typedef Ip_s32 Ipcom_mib2_Integer32;
typedef Ip_u32 Ipcom_mib2_Unsigned32;
typedef Ip_u32 Ipcom_mib2_Counter32;
typedef Ip_u64 Ipcom_mib2_Counter64;
typedef Ip_u32 Ipcom_mib2_Gauge32;


/*
 *===========================================================================
 *                    SNMPv2-TC types
 *===========================================================================
 */
typedef Ipcom_mib2_Integer          Ipcom_mib2_TruthValue; /* true(1), false(2) */
typedef Ipcom_mib2_Integer          Ipcom_mib2_TestAndIncr;
typedef Ipcom_mib2_Unsigned32       Ipcom_mib2_TimeStamp;
typedef Ipcom_mib2_Integer32        Ipcom_mib2_InterfaceIndex;
typedef Ipcom_mib2_Integer32        Ipcom_mib2_InterfaceIndexOrZero;
typedef Ipcom_mib2_OctetString      Ipcom_mib2_Ipv6AddressIfIdentifierTC;
typedef Ipcom_mib2_Integer          Ipcom_mib2_InetVersion; /* ipv4(1), ipv6(2) */
typedef Ipcom_mib2_Integer          Ipcom_mib2_IpAddressOriginTC;
typedef Ipcom_mib2_Integer          Ipcom_mib2_IpAddressStatusTC;
typedef Ipcom_mib2_Integer          Ipcom_mib2_IpAddressPrefixOriginTC;
typedef Ipcom_mib2_Integer          Ipcom_mib2_InetAddressType;
typedef Ipcom_mib2_OctetString      Ipcom_mib2_InetAddress;
typedef Ipcom_mib2_Unsigned32       Ipcom_mib2_InetAddressPrefixLength;
typedef Ipcom_mib2_ObjectIdentifier Ipcom_mib2_RowPointer;
typedef Ipcom_mib2_Integer          Ipcom_mib2_RowStatus;
typedef Ipcom_mib2_Integer          Ipcom_mib2_StorageType;
typedef Ipcom_mib2_OctetString      Ipcom_mib2_PhysAddress;
typedef Ipcom_mib2_Unsigned32       Ipcom_mib2_InetZoneIndex;
typedef Ipcom_mib2_Unsigned32       Ipcom_mib2_InetPortNumber;
typedef Ipcom_mib2_Integer          Ipcom_mib2_IANAipRouteProtocol;
typedef Ipcom_mib2_Unsigned32       Ipcom_mib2_InetAutonomousSystemNumber;


/*
 *===========================================================================
 *                    Ipcom_mib2_sysUpTimeFunc
 *===========================================================================
 * Function pointer type for the sysUpTime function
 */
typedef Ipcom_mib2_TimeStamp (* Ipcom_mib2_sysUpTimeFunc) (void);


/*
 *===========================================================================
 *                    RFC1213
 *===========================================================================
 */

/*
 *===========================================================================
 *                    Ipcom_mib2_ip_group
 *===========================================================================
 */
typedef struct Ipcom_mib2_ip_group_struct
{
    Ipcom_mib2_Counter32 ipInReceives;
    Ipcom_mib2_Counter32 ipInHdrErrors;
    Ipcom_mib2_Counter32 ipInAddrErrors;
    Ipcom_mib2_Counter32 ipForwDatagrams;
    Ipcom_mib2_Counter32 ipInUnknownProtos;
    Ipcom_mib2_Counter32 ipInDiscards;
    Ipcom_mib2_Counter32 ipInDelivers;
    Ipcom_mib2_Counter32 ipOutRequests;
    Ipcom_mib2_Counter32 ipOutDiscards;
    Ipcom_mib2_Counter32 ipOutNoRoutes;
    Ipcom_mib2_Integer   ipReasmTimeout;
    Ipcom_mib2_Counter32 ipReasmReqds;
    Ipcom_mib2_Counter32 ipReasmOKs;
    Ipcom_mib2_Counter32 ipReasmFails;
    Ipcom_mib2_Counter32 ipFragOKs;
    Ipcom_mib2_Counter32 ipFragFails;
    Ipcom_mib2_Counter32 ipFragCreates;
    Ipcom_mib2_Counter32 ipRoutingDiscards;
}
Ipcom_mib2_ip_group;


/*
 *===========================================================================
 *                    Ipcom_mib2_icmp_group
 *===========================================================================
 */
typedef struct Ipcom_mib2_icmp_group_struct
{
    Ipcom_mib2_Counter32 icmpInMsgs;
    Ipcom_mib2_Counter32 icmpInErrors;
    Ipcom_mib2_Counter32 icmpInDestUnreachs;
    Ipcom_mib2_Counter32 icmpInTimeExcds;
    Ipcom_mib2_Counter32 icmpInParmProbs;
    Ipcom_mib2_Counter32 icmpInSrcQuenchs;
    Ipcom_mib2_Counter32 icmpInRedirects;
    Ipcom_mib2_Counter32 icmpInEchos;
    Ipcom_mib2_Counter32 icmpInEchoReps;
    Ipcom_mib2_Counter32 icmpInTimestamps;
    Ipcom_mib2_Counter32 icmpInTimestampReps;
    Ipcom_mib2_Counter32 icmpInAddrMasks;
    Ipcom_mib2_Counter32 icmpInAddrMaskReps;
    Ipcom_mib2_Counter32 icmpOutMsgs;
    Ipcom_mib2_Counter32 icmpOutErrors;
    Ipcom_mib2_Counter32 icmpOutDestUnreachs;
    Ipcom_mib2_Counter32 icmpOutTimeExcds;
    Ipcom_mib2_Counter32 icmpOutParmProbs;
    Ipcom_mib2_Counter32 icmpOutSrcQuenchs;
    Ipcom_mib2_Counter32 icmpOutRedirects;
    Ipcom_mib2_Counter32 icmpOutEchos;
    Ipcom_mib2_Counter32 icmpOutEchoReps;
    Ipcom_mib2_Counter32 icmpOutTimestamps;
    Ipcom_mib2_Counter32 icmpOutTimestampReps;
    Ipcom_mib2_Counter32 icmpOutAddrMasks;
    Ipcom_mib2_Counter32 icmpOutAddrMaskReps;
}
Ipcom_mib2_icmp_group;


/*
 *===========================================================================
 *                    Ipcom_mib2_IpAddrEntry
 *===========================================================================
 */
typedef struct Ipcom_mib2_IpAddrEntry_struct
{
    Ipcom_mib2_InetAddress      ipAdEntAddr[4];
    Ipcom_mib2_InterfaceIndex   ipAdEntIfIndex;
    Ipcom_mib2_InetAddress      ipAdEntNetMask[4];
    Ipcom_mib2_Integer          ipAdEntBcastAddr;
    Ipcom_mib2_Integer          ipAdEntReasmMaxSize;
}
Ipcom_mib2_IpAddrEntry;


/*
 *===========================================================================
 *                    Ipcom_mib2_ipAddrTableCb
 *===========================================================================
 * Function pointer type for the RFC1213 ipAddrTable callback.
 * The callback shall return 1 to break iterations and 0 to continue with
 * next iteration.
 */
typedef int (* Ipcom_mib2_ipAddrTableCb) (Ipcom_mib2_IpAddrEntry *entry, void *user);


/*
 *===========================================================================
 *                    RFC4293 IP-MIB
 *===========================================================================
 */

/*
 *===========================================================================
 *                    Ipcom_mib2_ip_general_group
 *===========================================================================
 */
typedef struct Ipcom_mib2_ip_general_group_struct
{
    Ipcom_mib2_Integer   ipForwarding;
    Ipcom_mib2_Integer32 ipDefaultTTL;
    Ipcom_mib2_Integer32 ipReasmTimeout;
}
Ipcom_mib2_ip_general_group;


/*
 *===========================================================================
 *                    Ipcom_mib2_ipv6_general_group
 *===========================================================================
 */
typedef struct Ipcom_mib2_ipv6_general_group_struct
{
    Ipcom_mib2_Integer   ipv6IpForwarding;
    Ipcom_mib2_Integer32 ipv6IpDefaultHopLimit;
}
Ipcom_mib2_ipv6_general_group;


/*
 *===========================================================================
 *                    Ipcom_mib2_Ipv4InterfaceEntry
 *===========================================================================
 */
typedef struct Ipcom_mib2_Ipv4InterfaceEntry_struct
{
    Ipcom_mib2_InterfaceIndex ipv4InterfaceIfIndex;
    Ipcom_mib2_Integer32      ipv4InterfaceReasmMaxSize;
    Ipcom_mib2_Integer        ipv4InterfaceEnableStatus;
    Ipcom_mib2_Unsigned32     ipv4InterfaceRetransmitTime;
}
Ipcom_mib2_Ipv4InterfaceEntry;


/*
 *===========================================================================
 *                    Ipcom_mib2_ipv4InterfaceTableCb
 *===========================================================================
 * Function pointer type for the RFC4293 IP-MIB ipv4InterfaceTable callback.
 * The callback shall return 1 to break iterations and 0 to continue with
 * next iteration.
 */
typedef int (* Ipcom_mib2_ipv4InterfaceTableCb) (Ipcom_mib2_Ipv4InterfaceEntry *entry, void *user);


/*
 *===========================================================================
 *                    Ipcom_mib2_Ipv6InterfaceEntry
 *===========================================================================
 */
typedef struct Ipcom_mib2_Ipv6InterfaceEntry_struct
{
    Ipcom_mib2_InterfaceIndex            ipv6InterfaceIfIndex;
    Ipcom_mib2_Unsigned32                ipv6InterfaceReasmMaxSize;
    Ipcom_mib2_Ipv6AddressIfIdentifierTC ipv6InterfaceIdentifier[12]; /* length in first 4 bytes (Ip_u32) */
    Ipcom_mib2_Integer                   ipv6InterfaceEnableStatus;
    Ipcom_mib2_Unsigned32                ipv6InterfaceReachableTime;
    Ipcom_mib2_Unsigned32                ipv6InterfaceRetransmitTime;
    Ipcom_mib2_Integer                   ipv6InterfaceForwarding;
}
Ipcom_mib2_Ipv6InterfaceEntry;


/*
 *===========================================================================
 *                    Ipcom_mib2_ipv6InterfaceTableCb
 *===========================================================================
 * Function pointer type for the RFC4293 IP-MIB ipv6InterfaceTable callback.
 * The callback shall return 1 to break iterations and 0 to continue with
 * next iteration.
 */
typedef int (* Ipcom_mib2_ipv6InterfaceTableCb) (Ipcom_mib2_Ipv6InterfaceEntry *entry, void *user);


/*
 *===========================================================================
 *                    Ipcom_mib2_IpSystemStatsEntry
 *===========================================================================
 */
typedef struct Ipcom_mib2_IpSystemStatsEntry_struct
{
    Ipcom_mib2_InetVersion ipSystemStatsIPVersion;
    Ipcom_mib2_Counter32   ipSystemStatsInReceives;
    Ipcom_mib2_Counter64   ipSystemStatsHCInReceives;
    Ipcom_mib2_Counter32   ipSystemStatsInOctets;
    Ipcom_mib2_Counter64   ipSystemStatsHCInOctets;
    Ipcom_mib2_Counter32   ipSystemStatsInHdrErrors;
    Ipcom_mib2_Counter32   ipSystemStatsInNoRoutes;
    Ipcom_mib2_Counter32   ipSystemStatsInAddrErrors;
    Ipcom_mib2_Counter32   ipSystemStatsInUnknownProtos;
    Ipcom_mib2_Counter32   ipSystemStatsInTruncatedPkts;
    Ipcom_mib2_Counter32   ipSystemStatsInForwDatagrams;
    Ipcom_mib2_Counter64   ipSystemStatsHCInForwDatagrams;
    Ipcom_mib2_Counter32   ipSystemStatsReasmReqds;
    Ipcom_mib2_Counter32   ipSystemStatsReasmOKs;
    Ipcom_mib2_Counter32   ipSystemStatsReasmFails;
    Ipcom_mib2_Counter32   ipSystemStatsInDiscards;
    Ipcom_mib2_Counter32   ipSystemStatsInDelivers;
    Ipcom_mib2_Counter64   ipSystemStatsHCInDelivers;
    Ipcom_mib2_Counter32   ipSystemStatsOutRequests;
    Ipcom_mib2_Counter64   ipSystemStatsHCOutRequests;
    Ipcom_mib2_Counter32   ipSystemStatsOutNoRoutes;
    Ipcom_mib2_Counter32   ipSystemStatsOutForwDatagrams;
    Ipcom_mib2_Counter64   ipSystemStatsHCOutForwDatagrams;
    Ipcom_mib2_Counter32   ipSystemStatsOutDiscards;
    Ipcom_mib2_Counter32   ipSystemStatsOutFragReqds;
    Ipcom_mib2_Counter32   ipSystemStatsOutFragOKs;
    Ipcom_mib2_Counter32   ipSystemStatsOutFragFails;
    Ipcom_mib2_Counter32   ipSystemStatsOutFragCreates;
    Ipcom_mib2_Counter32   ipSystemStatsOutTransmits;
    Ipcom_mib2_Counter64   ipSystemStatsHCOutTransmits;
    Ipcom_mib2_Counter32   ipSystemStatsOutOctets;
    Ipcom_mib2_Counter64   ipSystemStatsHCOutOctets;
    Ipcom_mib2_Counter32   ipSystemStatsInMcastPkts;
    Ipcom_mib2_Counter64   ipSystemStatsHCInMcastPkts;
    Ipcom_mib2_Counter32   ipSystemStatsInMcastOctets;
    Ipcom_mib2_Counter64   ipSystemStatsHCInMcastOctets;
    Ipcom_mib2_Counter32   ipSystemStatsOutMcastPkts;
    Ipcom_mib2_Counter64   ipSystemStatsHCOutMcastPkts;
    Ipcom_mib2_Counter32   ipSystemStatsOutMcastOctets;
    Ipcom_mib2_Counter64   ipSystemStatsHCOutMcastOctets;
    Ipcom_mib2_Counter32   ipSystemStatsInBcastPkts;
    Ipcom_mib2_Counter64   ipSystemStatsHCInBcastPkts;
    Ipcom_mib2_Counter32   ipSystemStatsOutBcastPkts;
    Ipcom_mib2_Counter64   ipSystemStatsHCOutBcastPkts;
    Ipcom_mib2_TimeStamp   ipSystemStatsDiscontinuityTime;
    Ipcom_mib2_Unsigned32  ipSystemStatsRefreshRate;
}
Ipcom_mib2_IpSystemStatsEntry;


/*
 *===========================================================================
 *                    Ipcom_mib2_ipSystemStatsTableCb
 *===========================================================================
 * Function pointer type for the RFC4293 IP-MIB ipSystemStatsTable callback.
 * The callback shall return 1 to break iterations and 0 to continue with
 * next iteration.
 */
typedef int (* Ipcom_mib2_ipSystemStatsTableCb) (Ipcom_mib2_IpSystemStatsEntry *entry, void *user);


/*
 *===========================================================================
 *                    Ipcom_mib2_IpIfStatsEntry
 *===========================================================================
 */
typedef struct Ipcom_mib2_IpIfStatsEntry_struct
{
    Ipcom_mib2_InetVersion    ipIfStatsIPVersion;
    Ipcom_mib2_InterfaceIndex ipIfStatsIfIndex;
    Ipcom_mib2_Counter32      ipIfStatsInReceives;
    Ipcom_mib2_Counter64      ipIfStatsHCInReceives;
    Ipcom_mib2_Counter32      ipIfStatsInOctets;
    Ipcom_mib2_Counter64      ipIfStatsHCInOctets;
    Ipcom_mib2_Counter32      ipIfStatsInHdrErrors;
    Ipcom_mib2_Counter32      ipIfStatsInNoRoutes;
    Ipcom_mib2_Counter32      ipIfStatsInAddrErrors;
    Ipcom_mib2_Counter32      ipIfStatsInUnknownProtos;
    Ipcom_mib2_Counter32      ipIfStatsInTruncatedPkts;
    Ipcom_mib2_Counter32      ipIfStatsInForwDatagrams;
    Ipcom_mib2_Counter64      ipIfStatsHCInForwDatagrams;
    Ipcom_mib2_Counter32      ipIfStatsReasmReqds;
    Ipcom_mib2_Counter32      ipIfStatsReasmOKs;
    Ipcom_mib2_Counter32      ipIfStatsReasmFails;
    Ipcom_mib2_Counter32      ipIfStatsInDiscards;
    Ipcom_mib2_Counter32      ipIfStatsInDelivers;
    Ipcom_mib2_Counter64      ipIfStatsHCInDelivers;
    Ipcom_mib2_Counter32      ipIfStatsOutRequests;
    Ipcom_mib2_Counter64      ipIfStatsHCOutRequests;
    Ipcom_mib2_Counter32      ipIfStatsOutForwDatagrams;
    Ipcom_mib2_Counter64      ipIfStatsHCOutForwDatagrams;
    Ipcom_mib2_Counter32      ipIfStatsOutDiscards;
    Ipcom_mib2_Counter32      ipIfStatsOutFragReqds;
    Ipcom_mib2_Counter32      ipIfStatsOutFragOKs;
    Ipcom_mib2_Counter32      ipIfStatsOutFragFails;
    Ipcom_mib2_Counter32      ipIfStatsOutFragCreates;
    Ipcom_mib2_Counter32      ipIfStatsOutTransmits;
    Ipcom_mib2_Counter64      ipIfStatsHCOutTransmits;
    Ipcom_mib2_Counter32      ipIfStatsOutOctets;
    Ipcom_mib2_Counter64      ipIfStatsHCOutOctets;
    Ipcom_mib2_Counter32      ipIfStatsInMcastPkts;
    Ipcom_mib2_Counter64      ipIfStatsHCInMcastPkts;
    Ipcom_mib2_Counter32      ipIfStatsInMcastOctets;
    Ipcom_mib2_Counter64      ipIfStatsHCInMcastOctets;
    Ipcom_mib2_Counter32      ipIfStatsOutMcastPkts;
    Ipcom_mib2_Counter64      ipIfStatsHCOutMcastPkts;
    Ipcom_mib2_Counter32      ipIfStatsOutMcastOctets;
    Ipcom_mib2_Counter64      ipIfStatsHCOutMcastOctets;
    Ipcom_mib2_Counter32      ipIfStatsInBcastPkts;
    Ipcom_mib2_Counter64      ipIfStatsHCInBcastPkts;
    Ipcom_mib2_Counter32      ipIfStatsOutBcastPkts;
    Ipcom_mib2_Counter64      ipIfStatsHCOutBcastPkts;
    Ipcom_mib2_TimeStamp      ipIfStatsDiscontinuityTime;
    Ipcom_mib2_Unsigned32     ipIfStatsRefreshRate;
}
Ipcom_mib2_IpIfStatsEntry;


/*
 *===========================================================================
 *                    Ipcom_mib2_ipIfStatsTableCb
 *===========================================================================
 * Function pointer type for the RFC4293 IP-MIB ipIfStatsTable callback.
 * The callback shall return 1 to break iterations and 0 to continue with
 * next iteration.
 */
typedef int (* Ipcom_mib2_ipIfStatsTableCb) (Ipcom_mib2_IpIfStatsEntry *entry, void *user);


/*
 *===========================================================================
 *                    Ipcom_mib2_IpAddressPrefixEntry
 *===========================================================================
 */
typedef struct Ipcom_mib2_IpAddressPrefixEntry_struct
{
    Ipcom_mib2_InterfaceIndex          ipAddressPrefixIfIndex;
    Ipcom_mib2_InetAddressType         ipAddressPrefixType;
    Ipcom_mib2_InetAddress             ipAddressPrefixPrefix[16];
    Ipcom_mib2_InetAddressPrefixLength ipAddressPrefixLength;
    Ipcom_mib2_IpAddressPrefixOriginTC ipAddressPrefixOrigin;
    Ipcom_mib2_TruthValue              ipAddressPrefixOnLinkFlag;
    Ipcom_mib2_TruthValue              ipAddressPrefixAutonomousFlag;
    Ipcom_mib2_Unsigned32              ipAddressPrefixAdvPreferredLifetime;
    Ipcom_mib2_Unsigned32              ipAddressPrefixAdvValidLifetime;
}
Ipcom_mib2_IpAddressPrefixEntry;


/*
 *===========================================================================
 *                    Ipcom_mib2_ipAddressPrefixTableCb
 *===========================================================================
 * Function pointer type for the RFC4293 IP-MIB ipAddressPrefixTable callback.
 * The callback shall return 1 to break iterations and 0 to continue with
 * next iteration.
 */
typedef int (* Ipcom_mib2_ipAddressPrefixTableCb) (Ipcom_mib2_IpAddressPrefixEntry *entry, void *user);


/*
 *===========================================================================
 *                    Ipcom_mib2_IpAddressEntry
 *===========================================================================
 */
typedef struct Ipcom_mib2_IpAddressEntry_struct
{
    Ipcom_mib2_InetAddressType   ipAddressAddrType;
    Ipcom_mib2_InetAddress       ipAddressAddr[20];  /* InetAddressIPv6z needs 20 bytes */
    Ipcom_mib2_InterfaceIndex    ipAddressIfIndex;
    Ipcom_mib2_Integer           ipAddressType;
    Ipcom_mib2_RowPointer        ipAddressPrefix[3]; /* {2,0,0} */
    Ipcom_mib2_IpAddressOriginTC ipAddressOrigin;
    Ipcom_mib2_IpAddressStatusTC ipAddressStatus;
    Ipcom_mib2_TimeStamp         ipAddressCreated;
    Ipcom_mib2_TimeStamp         ipAddressLastChanged;
    Ipcom_mib2_RowStatus         ipAddressRowStatus;
    Ipcom_mib2_StorageType       ipAddressStorageType;
}
Ipcom_mib2_IpAddressEntry;


/*
 *===========================================================================
 *                    Ipcom_mib2_ipAddressTableCb
 *===========================================================================
 * Function pointer type for the RFC4293 IP-MIB ipAddressTable callback.
 * The callback shall return 1 to break iterations and 0 to continue with
 * next iteration.
 */
typedef int (* Ipcom_mib2_ipAddressTableCb) (Ipcom_mib2_IpAddressEntry *entry, void *user);


/*
 *===========================================================================
 *                    Ipcom_mib2_IpNetToPhysicalEntry
 *===========================================================================
 */
typedef struct Ipcom_mib2_IpNetToPhysicalEntry_struct
{
    Ipcom_mib2_InterfaceIndex  ipNetToPhysicalIfIndex;
    Ipcom_mib2_InetAddressType ipNetToPhysicalNetAddressType;
    Ipcom_mib2_InetAddress     ipNetToPhysicalNetAddress[16];
    Ipcom_mib2_PhysAddress     ipNetToPhysicalPhysAddress[10];  /* length in first 4 bytes (Ip_u32) */
    Ipcom_mib2_TimeStamp       ipNetToPhysicalLastUpdated;
    Ipcom_mib2_Integer         ipNetToPhysicalType;
    Ipcom_mib2_Integer         ipNetToPhysicalState;
    Ipcom_mib2_RowStatus       ipNetToPhysicalRowStatus;
}
Ipcom_mib2_IpNetToPhysicalEntry;


/*
 *===========================================================================
 *                    Ipcom_mib2_ipNetToPhysicalTableCb
 *===========================================================================
 * Function pointer type for the RFC4293 IP-MIB ipNetToPhysicalTableTable callback.
 * The callback shall return 1 to break iterations and 0 to continue with
 * next iteration.
 */
typedef int (* Ipcom_mib2_ipNetToPhysicalTableCb) (Ipcom_mib2_IpNetToPhysicalEntry *entry, void *user);


/*
 *===========================================================================
 *                    Ipcom_mib2_Ipv6ScopeZoneIndexEntry
 *===========================================================================
 */
typedef struct Ipcom_mib2_Ipv6ScopeZoneIndexEntry_struct
{
    Ipcom_mib2_InterfaceIndex ipv6ScopeZoneIndexIfIndex;
    Ipcom_mib2_InetZoneIndex  ipv6ScopeZoneIndexLinkLocal;
    Ipcom_mib2_InetZoneIndex  ipv6ScopeZoneIndex3;
    Ipcom_mib2_InetZoneIndex  ipv6ScopeZoneIndexAdminLocal;
    Ipcom_mib2_InetZoneIndex  ipv6ScopeZoneIndexSiteLocal;
    Ipcom_mib2_InetZoneIndex  ipv6ScopeZoneIndex6;
    Ipcom_mib2_InetZoneIndex  ipv6ScopeZoneIndex7;
    Ipcom_mib2_InetZoneIndex  ipv6ScopeZoneIndexOrganizationLocal;
    Ipcom_mib2_InetZoneIndex  ipv6ScopeZoneIndex9;
    Ipcom_mib2_InetZoneIndex  ipv6ScopeZoneIndexA;
    Ipcom_mib2_InetZoneIndex  ipv6ScopeZoneIndexB;
    Ipcom_mib2_InetZoneIndex  ipv6ScopeZoneIndexC;
    Ipcom_mib2_InetZoneIndex  ipv6ScopeZoneIndexD;
}
Ipcom_mib2_Ipv6ScopeZoneIndexEntry;


/*
 *===========================================================================
 *                    Ipcom_mib2_ipv6ScopeZoneIndexTableCb
 *===========================================================================
 * Function pointer type for the RFC4293 IP-MIB ipv6ScopeZoneIndexTable callback.
 * The callback shall return 1 to break iterations and 0 to continue with
 * next iteration.
 */
typedef int (* Ipcom_mib2_ipv6ScopeZoneIndexTableCb) (Ipcom_mib2_Ipv6ScopeZoneIndexEntry *entry, void *user);


/*
 *===========================================================================
 *                    Ipcom_mib2_IpDefaultRouterEntry
 *===========================================================================
 */
typedef struct Ipcom_mib2_IpDefaultRouterEntry_struct
{
    Ipcom_mib2_InetAddressType ipDefaultRouterAddressType;
    Ipcom_mib2_InetAddress     ipDefaultRouterAddress[16];
    Ipcom_mib2_InterfaceIndex  ipDefaultRouterIfIndex;
    Ipcom_mib2_Unsigned32      ipDefaultRouterLifetime;
    Ipcom_mib2_Integer         ipDefaultRouterPreference;
}
Ipcom_mib2_IpDefaultRouterEntry;


/*
 *===========================================================================
 *                    Ipcom_mib2_ipDefaultRouterTableCb
 *===========================================================================
 * Function pointer type for the RFC4293 IP-MIB ipDefaultRouterTable callback.
 * The callback shall return 1 to break iterations and 0 to continue with
 * next iteration.
 */
typedef int (* Ipcom_mib2_ipDefaultRouterTableCb) (Ipcom_mib2_IpDefaultRouterEntry *entry, void *user);


/*
 *===========================================================================
 *                    Ipcom_mib2_Ipv6RouterAdvertEntry
 *===========================================================================
 */
typedef struct Ipcom_mib2_Ipv6RouterAdvertEntry_struct
{
    Ipcom_mib2_InterfaceIndex ipv6RouterAdvertIfIndex;
    Ipcom_mib2_TruthValue     ipv6RouterAdvertSendAdverts;
    Ipcom_mib2_Unsigned32     ipv6RouterAdvertMaxInterval;
    Ipcom_mib2_Unsigned32     ipv6RouterAdvertMinInterval;
    Ipcom_mib2_TruthValue     ipv6RouterAdvertManagedFlag;
    Ipcom_mib2_TruthValue     ipv6RouterAdvertOtherConfigFlag;
    Ipcom_mib2_Unsigned32     ipv6RouterAdvertLinkMTU;
    Ipcom_mib2_Unsigned32     ipv6RouterAdvertReachableTime;
    Ipcom_mib2_Unsigned32     ipv6RouterAdvertRetransmitTime;
    Ipcom_mib2_Unsigned32     ipv6RouterAdvertCurHopLimit;
    Ipcom_mib2_Unsigned32     ipv6RouterAdvertDefaultLifetime;
    Ipcom_mib2_RowStatus      ipv6RouterAdvertRowStatus;
}
Ipcom_mib2_Ipv6RouterAdvertEntry;


/*
 *===========================================================================
 *                    Ipcom_mib2_ipv6RouterAdvertTableCb
 *===========================================================================
 * Function pointer type for the RFC4293 IP-MIB ipv6RouterAdvertTable callback.
 * The callback shall return 1 to break iterations and 0 to continue with
 * next iteration.
 */
typedef int (* Ipcom_mib2_ipv6RouterAdvertTableCb) (Ipcom_mib2_Ipv6RouterAdvertEntry *entry, void *user);


/*
 *===========================================================================
 *                    Ipcom_mib2_IcmpStatsEntry
 *===========================================================================
 */
typedef struct Ipcom_mib2_IcmpStatsEntry_struct
{
    Ipcom_mib2_InetVersion icmpStatsIPVersion;
    Ipcom_mib2_Counter32   icmpStatsInMsgs;
    Ipcom_mib2_Counter32   icmpStatsInErrors;
    Ipcom_mib2_Counter32   icmpStatsOutMsgs;
    Ipcom_mib2_Counter32   icmpStatsOutErrors;

}
Ipcom_mib2_IcmpStatsEntry;


/*
 *===========================================================================
 *                    Ipcom_mib2_icmpStatsTableCb
 *===========================================================================
 * Function pointer type for the RFC4293 IP-MIB icmpStatsTable callback.
 * The callback shall return 1 to break iterations and 0 to continue with
 * next iteration.
 */
typedef int (* Ipcom_mib2_icmpStatsTableCb) (Ipcom_mib2_IcmpStatsEntry *entry, void *user);


/*
 *===========================================================================
 *                    Ipcom_mib2_IcmpMsgStatsEntry
 *===========================================================================
 */
typedef struct Ipcom_mib2_IcmpMsgStatsEntry_struct
{
    Ipcom_mib2_InetVersion icmpMsgStatsIPVersion;
    Ipcom_mib2_Integer32   icmpMsgStatsType;
    Ipcom_mib2_Counter32   icmpMsgStatsInPkts;
    Ipcom_mib2_Counter32   icmpMsgStatsOutPkts;
}
Ipcom_mib2_IcmpMsgStatsEntry;


/*
 *===========================================================================
 *                    Ipcom_mib2_icmpMsgStatsTableCb
 *===========================================================================
 * Function pointer type for the RFC4293 IP-MIB icmpMsgStatsTable callback.
 * The callback shall return 1 to break iterations and 0 to continue with
 * next iteration.
 */
typedef int (* Ipcom_mib2_icmpMsgStatsTableCb) (Ipcom_mib2_IcmpMsgStatsEntry *entry, void *user);


/*
 *===========================================================================
 *                    RFC4022 TCP-MIB
 *===========================================================================
 */

/*
 *===========================================================================
 *                    Ipcom_mib2_tcp_group
 *===========================================================================
 */
typedef struct Ipcom_mib2_tcp_group_struct
{
    Ipcom_mib2_Integer   tcpRtoAlgorithm;
    Ipcom_mib2_Integer32 tcpRtoMin;
    Ipcom_mib2_Integer32 tcpRtoMax;
    Ipcom_mib2_Integer32 tcpMaxConn;
    Ipcom_mib2_Counter32 tcpActiveOpens;
    Ipcom_mib2_Counter32 tcpPassiveOpens;
    Ipcom_mib2_Counter32 tcpAttemptFails;
    Ipcom_mib2_Counter32 tcpEstabResets;
    Ipcom_mib2_Gauge32   tcpCurrEstab;
    Ipcom_mib2_Counter32 tcpInSegs;
    Ipcom_mib2_Counter32 tcpOutSegs;
    Ipcom_mib2_Counter32 tcpRetransSegs;
    Ipcom_mib2_Counter32 tcpInErrs;
    Ipcom_mib2_Counter32 tcpOutRsts;
    Ipcom_mib2_Counter64 tcpHCInSegs;
    Ipcom_mib2_Counter64 tcpHCOutSegs;
}
Ipcom_mib2_tcp_group;


/*
 *===========================================================================
 *                    Ipcom_mib2_TcpConnectionEntry
 *===========================================================================
 */
typedef struct Ipcom_mib2_TcpConnectionEntry_struct
{
    Ipcom_mib2_InetAddressType tcpConnectionLocalAddressType;
    Ipcom_mib2_InetAddress     tcpConnectionLocalAddress[20];
    Ipcom_mib2_InetPortNumber  tcpConnectionLocalPort;
    Ipcom_mib2_InetAddressType tcpConnectionRemAddressType;
    Ipcom_mib2_InetAddress     tcpConnectionRemAddress[20];
    Ipcom_mib2_InetPortNumber  tcpConnectionRemPort;
    Ipcom_mib2_Integer         tcpConnectionState;
    Ipcom_mib2_Unsigned32      tcpConnectionProcess;
}
Ipcom_mib2_TcpConnectionEntry;


/*
 *===========================================================================
 *                    Ipcom_mib2_tcpConnectionTable
 *===========================================================================
 * Function pointer type for the RFC4022 TCP-MIB tcpConnectionTable callback.
 * The callback shall return 1 to break iterations and 0 to continue with
 * next iteration.
 */
typedef int (* Ipcom_mib2_tcpConnectionTableCb) (Ipcom_mib2_TcpConnectionEntry *entry, void *user);


/*
 *===========================================================================
 *                    Ipcom_mib2_TcpListenerEntry
 *===========================================================================
 */
typedef struct Ipcom_mib2_TcpListenerEntry_struct
{
    Ipcom_mib2_InetAddressType tcpListenerLocalAddressType;
    Ipcom_mib2_InetAddress     tcpListenerLocalAddress[20];
    Ipcom_mib2_InetPortNumber  tcpListenerLocalPort;
    Ipcom_mib2_Unsigned32      tcpListenerProcess;
}
Ipcom_mib2_TcpListenerEntry;


/*
 *===========================================================================
 *                    Ipcom_mib2_tcpListenerTable
 *===========================================================================
 * Function pointer type for the RFC4022 TCP-MIB tcpListenerTable callback.
 * The callback shall return 1 to break iterations and 0 to continue with
 * next iteration.
 */
typedef int (* Ipcom_mib2_tcpListenerTableCb) (Ipcom_mib2_TcpListenerEntry *entry, void *user);


/*
 *===========================================================================
 *                    RFC4113 UDP-MIB
 *===========================================================================
 */

/*
 *===========================================================================
 *                    Ipcom_mib2_udp_group
 *===========================================================================
 */
typedef struct Ipcom_mib2_udp_group_struct
{
    Ipcom_mib2_Counter32 udpInDatagrams;
    Ipcom_mib2_Counter32 udpNoPorts;
    Ipcom_mib2_Counter32 udpInErrors;
    Ipcom_mib2_Counter32 udpOutDatagrams;
    Ipcom_mib2_Counter64 udpHCInDatagrams;
    Ipcom_mib2_Counter64 udpHCOutDatagrams;
}
Ipcom_mib2_udp_group;


/*
 *===========================================================================
 *                    Ipcom_mib2_UdpEndpointEntry
 *===========================================================================
 */
typedef struct Ipcom_mib2_UdpEndpointEntry_struct
{
    Ipcom_mib2_InetAddressType udpEndpointLocalAddressType;
    Ipcom_mib2_InetAddress     udpEndpointLocalAddress[20];
    Ipcom_mib2_InetPortNumber  udpEndpointLocalPort;
    Ipcom_mib2_InetAddressType udpEndpointRemoteAddressType;
    Ipcom_mib2_InetAddress     udpEndpointRemoteAddress[20];
    Ipcom_mib2_InetPortNumber  udpEndpointRemotePort;
    Ipcom_mib2_Unsigned32      udpEndpointInstance;
    Ipcom_mib2_Unsigned32      udpEndpointProcess;
}
Ipcom_mib2_UdpEndpointEntry;


/*
 *===========================================================================
 *                    Ipcom_mib2_udpEndpointTable
 *===========================================================================
 * Function pointer type for the RFC4113 UDP-MIB udpEndpointTable callback.
 * The callback shall return 1 to break iterations and 0 to continue with
 * next iteration.
 */
typedef int (* Ipcom_mib2_udpEndpointTableCb) (Ipcom_mib2_UdpEndpointEntry *entry, void *user);


/*
 *===========================================================================
 *                    RFC4292 IP-FORWARD-MIB
 *===========================================================================
 */

/*
 *===========================================================================
 *                    Ipcom_mib2_InetCidrRouteEntry
 *===========================================================================
 */
typedef struct Ipcom_mib2_InetCidrRouteEntry_struct
{
    Ipcom_mib2_InetAddressType            inetCidrRouteDestType;
    Ipcom_mib2_InetAddress                inetCidrRouteDest[16];
    Ipcom_mib2_InetAddressPrefixLength    inetCidrRoutePfxLen;
    Ipcom_mib2_ObjectIdentifier           inetCidrRoutePolicy[3]; /* {2,0,0} */
    Ipcom_mib2_InetAddressType            inetCidrRouteNextHopType;
    Ipcom_mib2_InetAddress                inetCidrRouteNextHop[16];
    Ipcom_mib2_InterfaceIndexOrZero       inetCidrRouteIfIndex;
    Ipcom_mib2_Integer                    inetCidrRouteType;
    Ipcom_mib2_IANAipRouteProtocol        inetCidrRouteProto;
    Ipcom_mib2_Gauge32                    inetCidrRouteAge;
    Ipcom_mib2_InetAutonomousSystemNumber inetCidrRouteNextHopAS;
    Ipcom_mib2_Integer32                  inetCidrRouteMetric1;
    Ipcom_mib2_Integer32                  inetCidrRouteMetric2;
    Ipcom_mib2_Integer32                  inetCidrRouteMetric3;
    Ipcom_mib2_Integer32                  inetCidrRouteMetric4;
    Ipcom_mib2_Integer32                  inetCidrRouteMetric5;
    Ipcom_mib2_RowStatus                  inetCidrRouteStatus;
}
Ipcom_mib2_InetCidrRouteEntry;


/*
 *===========================================================================
 *                    Ipcom_mib2_inetCidrRouteTable
 *===========================================================================
 * Function pointer type for the RFC4292 IP-FORWARD-MIB inetCidrRouteTable callback.
 * The callback shall return 1 to break iterations and 0 to continue with
 * next iteration.
 */
typedef int (* Ipcom_mib2_inetCidrRouteTableCb) (Ipcom_mib2_InetCidrRouteEntry *entry, void *user);


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

/*******************************************************************************
*
* ipcom_mib2_register_sysUpTimeFunc - Register a function for sysUpTime
*
* The ipcom_mib2_register_sysUpTimeFunc routine registers a function
* that the MIB-2 module will call in order to retrieve the current value
* of sysUpTime.
*
* Parameters:
*
* \is
* \i <func>
* The sysUpTime function to register.
* \ie
*
* RETURNS: 0 for success and -1 for failure.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_register_sysUpTimeFunc
(
    Ipcom_mib2_sysUpTimeFunc func
);


/*******************************************************************************
*
* ipcom_mib2_clear - Clear all MIB-2 statistics
*
* The ipcom_mib2_clear routine clears all MIB-2 statistics. Both system wide
* and per interface statistics are cleared.
*
* Parameters:
*
* None.
*
* RETURNS: Nothing.
*
* ERRNO:
*/
IP_PUBLIC void ipcom_mib2_clear(void);


/*
 *===========================================================================
 *                    RFC1213 MIB
 *===========================================================================
 */

/*******************************************************************************
*
* ipcom_mib2_get_ip_group - Retrieve RFC1213 ip group
*
* The ipcom_mib2_get_ip_group routine retrieves the RFC1213 ip
* group information.
*
* Parameters:
* \is
* \i <group>
* Pointer to a structure where the function outputs the result.
* \ie
*
* RETURNS: 0 for success and -1 for failure.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_get_ip_group
(
    Ipcom_mib2_ip_group *group
);


/*******************************************************************************
*
* ipcom_mib2_get_icmp_group - Retrieve RFC1213 icmp group
*
* The ipcom_mib2_get_icmp_group routine retrieves the RFC1213 icmp
* group information.
*
* Parameters:
* \is
* \i <group>
* Pointer to a structure where the function outputs the result.
* \ie
*
* RETURNS: 0 for success and -1 for failure.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_get_icmp_group
(
    Ipcom_mib2_icmp_group *group
);


/*******************************************************************************
*
* ipcom_mib2_iterate_ipAddrTable - Iterate ipAddrTable
*
* The ipcom_mib2_iterate_ipAddrTable routine iterates through the RFC1213
* ipAddrTable and executes a user defined callback function for each
* iteration. Note that if the table is empty the callback will not be called
* and the routine will return zero.
*
* Parameters:
* \is
* \i <cb>
* Function pointer to the callback.
* \i <user>
* User defined cookie that will be provided in each call to the callback.
* \ie
*
* RETURNS: The number of times the callback function was executed or -1
*          for failures.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_iterate_ipAddrTable
(
    Ipcom_mib2_ipAddrTableCb cb,
    void *user
);


/*
 *===========================================================================
 *                    RFC4293 IP-MIB
 *===========================================================================
 */

/*******************************************************************************
*
* ipcom_mib2_get_ip_general_group - Retrieve IP-MIB ip general group
*
* The ipcom_mib2_get_ip_general_group routine retrieves the RFC4293 IP-MIB ip
* general group information.
*
* Parameters:
* \is
* \i <group>
* Pointer to a structure where the function outputs the result.
* \ie
*
* RETURNS: 0 for success and -1 for failure.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_get_ip_general_group
(
    Ipcom_mib2_ip_general_group *group
);


/*******************************************************************************
*
* ipcom_mib2_set_ipForwarding - Set IP-MIB ipForwarding object
*
* The ipcom_mib2_set_ipForwarding routine sets the RFC4293 IP-MIB
* ipForwarding object. The value of ipForwarding must be either 1 or 2.
*
* Parameters:
* \is
* \i <ipForwarding>
* The value to set to the ipForwarding object.
* \ie
*
* RETURNS: 0 for success and -1 for failure.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_set_ipForwarding
(
    Ipcom_mib2_Integer ipForwarding
);


/*******************************************************************************
*
* ipcom_mib2_set_ipDefaultTTL - Set IP-MIB ipDefaultTTL object
*
* The ipcom_mib2_set_ipDefaultTTL routine sets the RFC4293 IP-MIB
* ipDefaultTTL object. The value of ipDefaultTTL must be between 1 and 255.
*
* Parameters:
* \is
* \i <ipDefaultTTL>
* The value to set to the ipDefaultTTL object.
* \ie
*
* RETURNS: 0 for success and -1 for failure.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_set_ipDefaultTTL
(
    Ipcom_mib2_Integer ipDefaultTTL
);


/*******************************************************************************
*
* ipcom_mib2_get_ipv6_general_group - Retrieve IP-MIB ipv6 general group
*
* The ipcom_mib2_get_ipv6_general_group routine retrieves the RFC4293 IP-MIB ipv6
* general group information.
*
* Parameters:
* \is
* \i <group>
* Pointer to a structure where the function outputs the result.
* \ie
*
* RETURNS: 0 for success and -1 for failure.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_get_ipv6_general_group
(
    Ipcom_mib2_ipv6_general_group *group
);


/*******************************************************************************
*
* ipcom_mib2_set_ipv6IpForwarding - Set IP-MIB ipv6IpForwarding object
*
* The ipcom_mib2_set_ipv6IpForwarding routine sets the RFC4293 IP-MIB
* ipv6IpForwarding object. The value of ipv6IpForwarding must be either 1 or 2.
*
* Parameters:
* \is
* \i <ipv6IpForwarding>
* The value to set to the ipv6IpForwarding object.
* \ie
*
* RETURNS: 0 for success and -1 for failure.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_set_ipv6IpForwarding
(
    Ipcom_mib2_Integer ipv6IpForwarding
);


/*******************************************************************************
*
* ipcom_mib2_set_ipv6IpDefaultHopLimit - Set IP-MIB ipv6IpDefaultHopLimit object
*
* The ipcom_mib2_set_ipv6IpDefaultHopLimit routine sets the RFC4293 IP-MIB
* ipv6IpDefaultHopLimit object. The value of ipv6IpDefaultHopLimit must be between
* 0 and 255.
*
* Parameters:
* \is
* \i <ipv6IpDefaultHopLimit>
* The value to set to the ipv6IpDefaultHopLimit object.
* \ie
*
* RETURNS: 0 for success and -1 for failure.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_set_ipv6IpDefaultHopLimit
(
    Ipcom_mib2_Integer ipv6IpDefaultHopLimit
);


/*******************************************************************************
*
* ipcom_mib2_get_ipv4InterfaceTableLastChange - Retrieve IP-MIB ipv4InterfaceTableLastChange object
*
* The ipcom_mib2_get_ipv4InterfaceTableLastChange routine retrieves the RFC4293 IP-MIB
* ipv4InterfaceTableLastChange object.
*
* Parameters:
* \is
* \i <timestamp>
* Pointer to a memory location where the function outputs the result.
* \ie
*
* RETURNS: 0 for success and -1 for failure.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_get_ipv4InterfaceTableLastChange
(
    Ipcom_mib2_TimeStamp *timestamp
);


/*******************************************************************************
*
* ipcom_mib2_set_ipv4InterfaceEnableStatus - Set IP-MIB ipv4InterfaceEnableStatus object
*
* The ipcom_mib2_set_ipv4InterfaceEnableStatus routine sets the RFC4293 IP-MIB
* ipv4InterfaceEnableStatus object. The value of ipv4InterfaceEnableStatus must be either 1 or 2.
*
* Parameters:
* \is
* \i <ifindex>
* Index for the interface that shall enable or disable IPv4.
* \i <ipv4InterfaceEnableStatus>
* up(1) or down(2).
* \ie
*
* RETURNS: 0 for success and -1 for failure.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_set_ipv4InterfaceEnableStatus
(
    Ipcom_mib2_InterfaceIndex ifindex,
    Ipcom_mib2_Integer ipv4InterfaceEnableStatus
);


/*******************************************************************************
*
* ipcom_mib2_get_ipv6InterfaceTableLastChange - Retrieve IP-MIB ipv6InterfaceTableLastChange object
*
* The ipcom_mib2_get_ipv6InterfaceTableLastChange routine retrieves the RFC4293 IP-MIB
* ipv6InterfaceTableLastChange object.
*
* Parameters:
* \is
* \i <timestamp>
* Pointer to a memory location where the function outputs the result.
* \ie
*
* RETURNS: 0 for success and -1 for failure.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_get_ipv6InterfaceTableLastChange
(
    Ipcom_mib2_TimeStamp *timestamp
);


/*******************************************************************************
*
* ipcom_mib2_set_ipv6InterfaceEnableStatus - Set IP-MIB ipv6InterfaceEnableStatus object
*
* The ipcom_mib2_set_ipv6InterfaceEnableStatus routine sets the RFC4293 IP-MIB
* ipv6InterfaceEnableStatus object. The value of ipv6InterfaceEnableStatus must be either 1 or 2.
*
* Parameters:
* \is
* \i <ifindex>
* Index for the interface that shall enable or disable IPv6.
* \i <ipv6InterfaceEnableStatus>
* up(1) or down(2).
* \ie
*
* RETURNS: 0 for success and -1 for failure.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_set_ipv6InterfaceEnableStatus
(
    Ipcom_mib2_InterfaceIndex ifindex,
    Ipcom_mib2_Integer ipv6InterfaceEnableStatus
);


/*******************************************************************************
*
* ipcom_mib2_set_ipv6InterfaceForwarding - Set IP-MIB ipv6InterfaceForwarding object
*
* The ipcom_mib2_set_ipv6InterfaceForwarding routine sets the RFC4293 IP-MIB
* ipv6InterfaceForwarding object. The value of ipv6InterfaceForwarding must be either 1 or 2.
*
* Parameters:
* \is
* \i <ifindex>
* Index for the interface that shall enable or disable IPv6 forwarding.
* \i <ipv6InterfaceForwarding>
* forwarding(1) or notForwarding(2).
* \ie
*
* RETURNS: 0 for success and -1 for failure.
*
* ERRNO:
*/

IP_PUBLIC int ipcom_mib2_set_ipv6InterfaceForwarding
(
    Ipcom_mib2_InterfaceIndex ifindex,
    Ipcom_mib2_Integer ipv6InterfaceForwarding
);


/*******************************************************************************
*
* ipcom_mib2_get_ipIfStatsTableLastChange - Retrieve IP-MIB ipIfStatsTableLastChange  object
*
* The ipcom_mib2_get_ipIfStatsTableLastChange routine retrieves the RFC4293 IP-MIB
* ipIfStatsTableLastChange object.
*
* Parameters:
* \is
* \i <timestamp>
* Pointer to a memory location where the function outputs the result.
* \ie
*
* RETURNS: 0 for success and -1 for failure.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_get_ipIfStatsTableLastChange
(
    Ipcom_mib2_TimeStamp *timestamp
);


/*******************************************************************************
*
* ipcom_mib2_get_ipAddressSpinLock - Retrieve IP-MIB ipAddressSpinLock object
*
* The ipcom_mib2_get_ipAddressSpinLock routine retrieves the RFC4293 IP-MIB
* ipAddressSpinLock object.
*
* Parameters:
* \is
* \i <timestamp>
* Pointer to a memory location where the function outputs the result.
* \ie
*
* RETURNS: 0 for success and -1 for failure.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_get_ipAddressSpinLock
(
    Ipcom_mib2_TestAndIncr *spinlock
);


/*******************************************************************************
*
* ipcom_mib2_set_ipAddressSpinLock - Set IP-MIB ipAddressSpinLock object
*
* The ipcom_mib2_set_ipAddressSpinLock routine sets the RFC4293 IP-MIB
* ipAddressSpinLock object.
*
* Parameters:
* \is
* \i <ipAddressSpinLock>
* The value to set to the ipAddressSpinLock object.
* \ie
*
* RETURNS: 0 for success and -1 for failure.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_set_ipAddressSpinLock
(
    Ipcom_mib2_TestAndIncr spinlock
);


/*******************************************************************************
*
* ipcom_mib2_get_ipv6RouterAdvertSpinLock - Retrieve IP-MIB ipv6RouterAdvertSpinLock object
*
* The ipcom_mib2_get_ipv6RouterAdvertSpinLock routine retrieves the RFC4293 IP-MIB
* ipv6RouterAdvertSpinLock object.
*
* Parameters:
* \is
* \i <timestamp>
* Pointer to a memory location where the function outputs the result.
* \ie
*
* RETURNS: 0 for success and -1 for failure.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_get_ipv6RouterAdvertSpinLock
(
    Ipcom_mib2_TestAndIncr *spinlock
);


/*******************************************************************************
*
* ipcom_mib2_set_ipv6RouterAdvertSpinLock - Set IP-MIB ipv6RouterAdvertSpinLock object
*
* The ipcom_mib2_set_ipv6RouterAdvertSpinLock routine sets the RFC4293 IP-MIB
* ipv6RouterAdvertSpinLock object.
*
* Parameters:
* \is
* \i <ipv6RouterAdvertSpinLock>
* The value to set to the ipv6RouterAdvertSpinLock object.
* \ie
*
* RETURNS: 0 for success and -1 for failure.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_set_ipv6RouterAdvertSpinLock
(
    Ipcom_mib2_TestAndIncr spinlock
);


/*******************************************************************************
*
* ipcom_mib2_iterate_ipv4InterfaceTable - Iterate IP-MIB ipv4InterfaceTable
*
* The ipcom_mib2_iterate_ipv4InterfaceTable routine iterates through the RFC4293 IP-MIB
* ipv4InterfaceTable and executes a user defined callback function for each
* iteration. Note that if the table is empty the callback will not be called
* and the routine will return zero.
*
* Parameters:
* \is
* \i <cb>
* Function pointer to the callback.
* \i <user>
* User defined cookie that will be provided in each call to the callback.
* \ie
*
* RETURNS: The number of times the callback function was executed or -1
*          for failures.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_iterate_ipv4InterfaceTable
(
    Ipcom_mib2_ipv4InterfaceTableCb cb,
    void *user
);



/*******************************************************************************
*
* ipcom_mib2_iterate_ipv6InterfaceTable - Iterate IP-MIB ipv6InterfaceTable
*
* The ipcom_mib2_iterate_ipv4InterfaceTable routine iterates through the RFC4293 IP-MIB
* ipv6InterfaceTable and executes a user defined callback function for each
* iteration. Note that if the table is empty the callback will not be called
* and the routine will return zero.
*
* Parameters:
* \is
* \i <cb>
* Function pointer to the callback.
* \i <user>
* User defined cookie that will be provided in each call to the callback.
* \ie
*
* RETURNS: The number of times the callback function was executed or -1
*          for failures.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_iterate_ipv6InterfaceTable
(
    Ipcom_mib2_ipv6InterfaceTableCb cb,
    void *user
);


/*******************************************************************************
*
* ipcom_mib2_iterate_ipSystemStatsTable - Iterate IP-MIB ipSystemStatsTable
*
* The ipcom_mib2_iterate_ipSystemStatsTable routine iterates through the RFC4293 IP-MIB
* ipSystemStatsTable and executes a user defined callback function for each
* iteration. Note that if the table is empty the callback will not be called
* and the routine will return zero.
*
* Parameters:
* \is
* \i <cb>
* Function pointer to the callback.
* \i <user>
* User defined cookie that will be provided in each call to the callback.
* \ie
*
* RETURNS: The number of times the callback function was executed or -1
*          for failures.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_iterate_ipSystemStatsTable
(
    Ipcom_mib2_ipSystemStatsTableCb cb,
    void *user
);


/*******************************************************************************
*
* ipcom_mib2_iterate_ipIfStatsTable - Iterate IP-MIB ipIfStatsTable
*
* The ipcom_mib2_iterate_ipIfStatsTable routine iterates through the RFC4293 IP-MIB
* ipIfStatsTable and executes a user defined callback function for each
* iteration. Note that if the table is empty the callback will not be called
* and the routine will return zero.
*
* Parameters:
* \is
* \i <cb>
* Function pointer to the callback.
* \i <user>
* User defined cookie that will be provided in each call to the callback.
* \ie
*
* RETURNS: The number of times the callback function was executed or -1
*          for failures.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_iterate_ipIfStatsTable
(
    Ipcom_mib2_ipIfStatsTableCb cb,
    void *user
);


/*******************************************************************************
*
* ipcom_mib2_iterate_ipAddressPrefixTable - Iterate IP-MIB ipAddressPrefixTable
*
* The ipcom_mib2_iterate_ipAddressPrefixTable routine iterates through the RFC4293 IP-MIB
* ipAddressPrefixTable and executes a user defined callback function for each
* iteration. Note that if the table is empty the callback will not be called
* and the routine will return zero.
*
* Parameters:
* \is
* \i <cb>
* Function pointer to the callback.
* \i <user>
* User defined cookie that will be provided in each call to the callback.
* \ie
*
* RETURNS: The number of times the callback function was executed or -1
*          for failures.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_iterate_ipAddressPrefixTable
(
    Ipcom_mib2_ipAddressPrefixTableCb cb,
    void *user
);


/*******************************************************************************
*
* ipcom_mib2_iterate_ipAddressTable - Iterate IP-MIB ipAddressTable
*
* The ipcom_mib2_iterate_ipAddressTable routine iterates through the RFC4293 IP-MIB
* ipAddressTable and executes a user defined callback function for each
* iteration. Note that if the table is empty the callback will not be called
* and the routine will return zero.
*
* Parameters:
* \is
* \i <cb>
* Function pointer to the callback.
* \i <user>
* User defined cookie that will be provided in each call to the callback.
* \ie
*
* RETURNS: The number of times the callback function was executed or -1
*          for failures.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_iterate_ipAddressTable
(
    Ipcom_mib2_ipAddressTableCb cb,
    void *user
);


/*******************************************************************************
*
* ipcom_mib2_iterate_ipNetToPhysicalTable - Iterate IP-MIB ipNetToPhysicalTable
*
* The ipcom_mib2_iterate_ipNetToPhysicalTable routine iterates through the RFC4293 IP-MIB
* ipNetToPhysicalTable and executes a user defined callback function for each
* iteration. Note that if the table is empty the callback will not be called
* and the routine will return zero.
*
* Parameters:
* \is
* \i <cb>
* Function pointer to the callback.
* \i <user>
* User defined cookie that will be provided in each call to the callback.
* \ie
*
* RETURNS: The number of times the callback function was executed or -1
*          for failures.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_iterate_ipNetToPhysicalTable
(
    Ipcom_mib2_ipNetToPhysicalTableCb cb,
    void *user
);


/*******************************************************************************
*
* ipcom_mib2_iterate_ipv6ScopeZoneIndexTable - Iterate IP-MIB ipv6ScopeZoneIndexTable
*
* The ipcom_mib2_iterate_ipv6ScopeZoneIndexTable routine iterates through the RFC4293 IP-MIB
* ipv6ScopeZoneIndexTable and executes a user defined callback function for each
* iteration. Note that if the table is empty the callback will not be called
* and the routine will return zero.
*
* Parameters:
* \is
* \i <cb>
* Function pointer to the callback.
* \i <user>
* User defined cookie that will be provided in each call to the callback.
* \ie
*
* RETURNS: The number of times the callback function was executed or -1
*          for failures.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_iterate_ipv6ScopeZoneIndexTable
(
    Ipcom_mib2_ipv6ScopeZoneIndexTableCb cb,
    void *user
);


/*******************************************************************************
*
* ipcom_mib2_iterate_ipDefaultRouterTable - Iterate IP-MIB ipDefaultRouterTable
*
* The ipcom_mib2_iterate_ipDefaultRouterTable routine iterates through the RFC4293 IP-MIB
* ipDefaultRouterTable and executes a user defined callback function for each
* iteration. Note that if the table is empty the callback will not be called
* and the routine will return zero.
*
* Parameters:
* \is
* \i <cb>
* Function pointer to the callback.
* \i <user>
* User defined cookie that will be provided in each call to the callback.
* \ie
*
* RETURNS: The number of times the callback function was executed or -1
*          for failures.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_iterate_ipDefaultRouterTable
(
    Ipcom_mib2_ipDefaultRouterTableCb cb,
    void *user
);


/*******************************************************************************
*
* ipcom_mib2_iterate_ipv6RouterAdvertTable - Iterate IP-MIB ipv6RouterAdvertTable
*
* The ipcom_mib2_iterate_ipv6RouterAdvertTable routine iterates through the RFC4293 IP-MIB
* ipv6RouterAdvertTable and executes a user defined callback function for each
* iteration. Note that if the table is empty the callback will not be called
* and the routine will return zero.
*
* Parameters:
* \is
* \i <cb>
* Function pointer to the callback.
* \i <user>
* User defined cookie that will be provided in each call to the callback.
* \ie
*
* RETURNS: The number of times the callback function was executed or -1
*          for failures.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_iterate_ipv6RouterAdvertTable
(
    Ipcom_mib2_ipv6RouterAdvertTableCb cb,
    void *user
);


/*******************************************************************************
*
* ipcom_mib2_iterate_icmpStatsTable - Iterate IP-MIB icmpStatsTable
*
* The ipcom_mib2_iterate_icmpStatsTable routine iterates through the RFC4293 IP-MIB
* icmpStatsTable and executes a user defined callback function for each
* iteration. Note that if the table is empty the callback will not be called
* and the routine will return zero.
*
* Parameters:
* \is
* \i <cb>
* Function pointer to the callback.
* \i <user>
* User defined cookie that will be provided in each call to the callback.
* \ie
*
* RETURNS: The number of times the callback function was executed or -1
*          for failures.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_iterate_icmpStatsTable
(
    Ipcom_mib2_icmpStatsTableCb cb,
    void *user
);


/*******************************************************************************
*
* ipcom_mib2_iterate_icmpMsgStatsTable - Iterate IP-MIB icmpMsgStatsTable
*
* The ipcom_mib2_iterate_icmpMsgStatsTable routine iterates through the RFC4293 IP-MIB
* icmpMsgStatsTable and executes a user defined callback function for each
* iteration. Note that if the table is empty the callback will not be called
* and the routine will return zero.
*
* Parameters:
* \is
* \i <cb>
* Function pointer to the callback.
* \i <user>
* User defined cookie that will be provided in each call to the callback.
* \ie
*
* RETURNS: The number of times the callback function was executed or -1
*          for failures.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_iterate_icmpMsgStatsTable
(
    Ipcom_mib2_icmpMsgStatsTableCb cb,
    void *user
);


/*
 *===========================================================================
 *                    RFC4292 IP-FORWARD-MIB
 *===========================================================================
 */

/*******************************************************************************
*
* ipcom_mib2_get_inetCidrRouteNumber - Retrieve IP-FORWARD-MIB inetCidrRouteNumber object
*
* The ipcom_mib2_get_inetCidrRouteNumber routine retrieves the RFC4292 IP-FORWARD-MIB
* inetCidrRouteNumber object.
*
* Parameters:
* \is
* \i <inetCidrRouteNumber>
* Pointer to a memory location where the function outputs the result.
* \ie
*
* RETURNS: 0 for success and -1 for failure.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_get_inetCidrRouteNumber
(
    Ipcom_mib2_Gauge32 *inetCidrRouteNumber
);


/*******************************************************************************
*
* ipcom_mib2_get_inetCidrRouteDiscards - Retrieve IP-FORWARD-MIB inetCidrRouteDiscards object
*
* The ipcom_mib2_get_inetCidrRouteDiscards routine retrieves the RFC4292 IP-FORWARD-MIB
* inetCidrRouteDiscards object.
*
* Parameters:
* \is
* \i <inetCidrRouteDiscards>
* Pointer to a memory location where the function outputs the result.
* \ie
*
* RETURNS: 0 for success and -1 for failure.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_get_inetCidrRouteDiscards
(
    Ipcom_mib2_Counter32 *inetCidrRouteDiscards
);


/*******************************************************************************
*
* ipcom_mib2_iterate_inetCidrRouteTable - Iterate IP-FORWARD-MIB inetCidrRouteTable
*
* The ipcom_mib2_iterate_inetCidrRouteTable routine iterates through the RFC4292 IP-FORWARD-MIB
* inetCidrRouteTable and executes a user defined callback function for each
* iteration. Note that if the table is empty the callback will not be called
* and the routine will return zero.
*
* Parameters:
* \is
* \i <cb>
* Function pointer to the callback.
* \i <user>
* User defined cookie that will be provided in each call to the callback.
* \ie
*
* RETURNS: The number of times the callback function was executed or -1
*          for failures.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_iterate_inetCidrRouteTable
(
    Ipcom_mib2_inetCidrRouteTableCb cb,
    void *user
);


/*
 *===========================================================================
 *                    RFC4022 TCP-MIB
 *===========================================================================
 */

/*******************************************************************************
*
* ipcom_mib2_get_tcp_group - Retrieve TCP-MIB tcp group
*
* The ipcom_mib2_get_tcp_group routine retrieves the RFC4022 TCP-MIB tcp
* group information.
*
* Parameters:
* \is
* \i <group>
* Pointer to a structure where the function outputs the result.
* \ie
*
* RETURNS: 0 for success and -1 for failure.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_get_tcp_group
(
    Ipcom_mib2_tcp_group *group
);


/*******************************************************************************
*
* ipcom_mib2_set_tcpConnectionState - Set TCP-MIB tcpConnectionState object
*
* The ipcom_mib2_set_tcpConnectionState routine sets the RFC4022 IP-MIB
* tcpConnectionState object. The value of tcpConnectionState must be
* 12 (deleteTCB).
*
* Parameters:
* \is
* \i <entry>
* Pointer to a TcpConnectionEntry structure with INDEX variables.
* \i <tcpConnectionState>
* The value to set to the tcpConnectionState object.
* \ie
*
* RETURNS: 0 for success and -1 for failure.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_set_tcpConnectionState
(
    Ipcom_mib2_TcpConnectionEntry *entry,
    Ipcom_mib2_Integer tcpConnectionState
);


/*******************************************************************************
*
* ipcom_mib2_iterate_tcpConnectionTable - Iterate TCP-MIB tcpConnectionTable
*
* The ipcom_mib2_iterate_tcpConnectionTable routine iterates through the RFC4022 TCP-MIB
* tcpConnectionTable and executes a user defined callback function for each
* iteration. Note that if the table is empty the callback will not be called
* and the routine will return zero.
*
* Parameters:
* \is
* \i <cb>
* Function pointer to the callback.
* \i <user>
* User defined cookie that will be provided in each call to the callback.
* \ie
*
* RETURNS: The number of times the callback function was executed or -1
*          for failures.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_iterate_tcpConnectionTable
(
    Ipcom_mib2_tcpConnectionTableCb cb,
    void *user
);


/*******************************************************************************
*
* ipcom_mib2_iterate_tcpListenerTable - Iterate TCP-MIB tcpListenerTable
*
* The ipcom_mib2_iterate_tcpListenerTable routine iterates through the RFC4022 TCP-MIB
* tcpListenerTable and executes a user defined callback function for each
* iteration. Note that if the table is empty the callback will not be called
* and the routine will return zero.
*
* Parameters:
* \is
* \i <cb>
* Function pointer to the callback.
* \i <user>
* User defined cookie that will be provided in each call to the callback.
* \ie
*
* RETURNS: The number of times the callback function was executed or -1
*          for failures.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_iterate_tcpListenerTable
(
    Ipcom_mib2_tcpListenerTableCb cb,
    void *user
);


/*
 *===========================================================================
 *                    RFC4113 UDP-MIB
 *===========================================================================
 */

/*******************************************************************************
*
* ipcom_mib2_get_udp_group - Retrieve UDP-MIB udp group
*
* The ipcom_mib2_get_udp_group routine retrieves the RFC4113 UDP-MIB udp
* group information.
*
* Parameters:
* \is
* \i <group>
* Pointer to a structure where the function outputs the result.
* \ie
*
* RETURNS: 0 for success and -1 for failure.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_get_udp_group
(
    Ipcom_mib2_udp_group *group
);


/*******************************************************************************
*
* ipcom_mib2_iterate_udpEndpointTable - Iterate UDP-MIB udpEndpointTable
*
* The ipcom_mib2_iterate_udpEndpointTable routine iterates through the RFC4113 UDP-MIB
* udpEndpointTable and executes a user defined callback function for each
* iteration. Note that if the table is empty the callback will not be called
* and the routine will return zero.
*
* Parameters:
* \is
* \i <cb>
* Function pointer to the callback.
* \i <user>
* User defined cookie that will be provided in each call to the callback.
* \ie
*
* RETURNS: The number of times the callback function was executed or -1
*          for failures.
*
* ERRNO:
*/
IP_PUBLIC int ipcom_mib2_iterate_udpEndpointTable
(
    Ipcom_mib2_udpEndpointTableCb cb,
    void *user
);



#ifdef __cplusplus
}
#endif

#endif  /* #ifndef IPCOM_MIB2_API_H */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
