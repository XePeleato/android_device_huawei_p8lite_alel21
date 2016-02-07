/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipdnsc_config.h,v $ $Revision: 1.16 $
 *   $Source: /home/interpeak/CVSRoot/ipappl/config/ipdnsc_config.h,v $
 *   $Author: markus $ $Date: 2006-11-29 15:13:10 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 ******************************************************************************
 */
#ifndef IPDNSC_CONFIG_H
#define IPDNSC_CONFIG_H


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * DNS Client compile time configuration.
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
#include <ipcom_sysvar.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Syslog facility */
#ifdef IPCOM_SYSLOG_FACILITY
#undef IPCOM_SYSLOG_FACILITY
#endif
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPDNSC

/* Syslog priority */
#ifdef IPCOM_SYSLOG_PRIORITY
#undef IPCOM_SYSLOG_PRIORITY
#endif
#ifdef IP_DEBUG
#define IPCOM_SYSLOG_PRIORITY    IPCOM_LOG_DEBUG
#else
#define IPCOM_SYSLOG_PRIORITY    IPCOM_LOG_WARNING
#endif


/*
 *===========================================================================
 *                         IPDNSC_CONF_DNS_HOSTS
 *===========================================================================
 * A list of known hosts.
 *
 */
#define IPDNSC_CONF_DNS_HOSTS   { "127.0.0.1", "localhost" }, \
                                { "::1", "localhost6" }, \
                                { IP_NULL, IP_NULL }


/*
 *===========================================================================
 *                         IPDNSC_CONF_MAX_CACHE_ENTRIES
 *===========================================================================
 * The maximum number of entries in the DNS cache.
 *
 */
#define IPDNSC_CONF_MAX_CACHE_ENTRIES       32

/*宏定义基于IPv4/IPv6网络的DNS Client TC flag支持*/
#define IPCOM_DNSC_TC_SUPPORT

#ifdef IPCOM_DNSC_TC_SUPPORT
/* default setting to use TCP to resolver DNS or not*/
#define DNSC_USE_TCP   "0"

/*能同时使用DNS client的用户最大数*/
#define DNSC_MAX_USERS    "32"

#endif

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
