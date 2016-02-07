/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_mcast.h,v $ $Revision: 1.5 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_mcast.h,v $
 *   $Author: kenneth $ $Date: 2009-01-15 14:11:19 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPNET_MCAST_H
#define IPNET_MCAST_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Types that are common for IGMPv3 and MLDv2.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipnet_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_set.h>

#include "ipnet_timeout.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Th Unsolicited Report Interval is the time between repetitions of a
   host's initial report of membership in a group.
   Default: 1000 millisecond. */
#define IPNET_MCAST_UNSOLICITED_REPORT_INTERVAL 1000

#define IPNET_MCAST_DEFAULT_ROBUSTNESS_VARIABLE 2
#define IPNET_MCAST_DEFAULT_QUERY_INTERVAL      125


/* IGMPv3/MLDv2 report types */
#define IPNET_MCAST_REPORT_GENERAL_QUERY  1
#define IPNET_MCAST_REPORT_SPECIFIC_QUERY 2
#define IPNET_MCAST_REPORT_FILTER_CHANGE  3

/* There are a number of different types of Group Records that may be
   included in a Report message (IGMPv3 and MLDv2) */
#define IPNET_MCAST_RECORD_TYPE_NOT_SET           (-1)
#define IPNET_MCAST_RECORD_TYPE_IS_INCLUDE        IPCOM_SET_IS_INCLUDE
#define IPNET_MCAST_RECORD_TYPE_IS_EXCLUDE        IPCOM_SET_IS_EXCLUDE
#define IPNET_MCAST_RECORD_TYPE_CHANGE_TO_INCLUDE 3
#define IPNET_MCAST_RECORD_TYPE_CHANGE_TO_EXCLUDE 4
#define IPNET_MCAST_RECORD_TYPE_ALLOW_NEW_SOURCES 5
#define IPNET_MCAST_RECORD_TYPE_BLOCK_OLD_SOURCES 6


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                   IGMPv3 and MLDv2 report types.
 *===========================================================================
 */

/* This data is only present on addresses of type IPNET_ADDR_TYPE_MULTICAST */
typedef struct Ipnet_mcast_addr_struct
{
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    /* The filter record for this multicast address, the 'user' member
       states the filter mode (IS_INCLUDE or IS_EXCLUDE). Elements in
       this set are of type pointer-to-source-address */
    Ipcom_set                   *filter;
    /* Set of sockets that has been used to join this multicast
       address on this interface. Elements in this set are of type
       pointer-to-socket-descriptor. */
    Ipcom_set                   *sockets;

    /* The usage of the next 3 fields are described in chapter 5.2 of
       RFC 3376 */

    /* Set of sources to report in the next specific query if 'user'
       == IS_INCLUDE. Elements in this set are of type
       pointer-to-source-address */
    Ipcom_set                   *specific_query_sources;
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */
    /* Timer when a response to a general query should be sent */
    struct Ipnet_timeout_struct *general_query_tmo;
    /* Timer when a response to a general query should be sent */
    struct Ipnet_timeout_struct *specific_query_tmo;

    /* Timer when a filter change report should be sent */
    struct Ipnet_timeout_struct *filter_change_tmo;
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    /* The sourc(es) that was added/removed in the last operation set
       of sources that caused the report to be sent if 'user' ==
       [CHANGE_TO_INCLUDE, CHANGE_TO_EXCLUDE, ALLOW_NEW_SOURCES,
       BLOCK_OLD_SOURCES]. Elements in this set are of type
       pointer-to-source-address */
    Ipcom_set                   *filter_change_sources;
#endif
    /* Number of times the last filter change report will be resent */
    Ip_u8                        filter_change_resend_count;

    /* One of the IPNET_MCAST_REPORT_xxx constants, this is the type
       of report that will be sent the next time */
    Ip_u8                        report_type;
}
Ipnet_mcast_addr;


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

IP_GLOBAL void ipnet_mcast_clear(Ipnet_mcast_addr *macb);
IP_GLOBAL void ipnet_mcast_free(Ipnet_mcast_addr *macb);

IP_GLOBAL void ipnet_mcast_schedule_membership_report(Ipnet_mcast_addr *macb,
                                                      Ip_bool is_specific_query,
                                                      Ip_u32 max_delay_msec,
                                                      Ipnet_timeout_handler tmo_handler,
                                                      void *addr_entry);
IP_GLOBAL void ipnet_mcast_report_finish(Ipnet_mcast_addr *macb,
                                         Ipnet_timeout_handler tmo_handler,
                                         void *addr_entry);

#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
IP_GLOBAL int  ipnet_mcast_init(Ipnet_mcast_addr *macb);
IP_GLOBAL void ipnet_mcast_build_if_filter(Ipnet_mcast_addr *macb, Ipcom_set *sources);
IP_GLOBAL Ip_bool ipnet_mcast_build_source_change_report(Ipnet_mcast_addr *macb,
                                                         Ipcom_set_foreach_cb_func build_if_filter_cb,
                                                         void *addr_entry);
IP_GLOBAL Ip_u32 ipnet_mcast_time_to_msec(Ip_u8 t);
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */

#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
