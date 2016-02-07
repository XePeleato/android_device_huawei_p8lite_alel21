/*
 ******************************************************************************
 *                     INTERPEAK AUTOCONF HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_autoconf.h,v $ $Revision: 1.62 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_autoconf.h,v $
 *   $Author: kenneth $ $Date: 2009-07-01 12:35:00 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_AUTOCONF_H
#define IPCOM_AUTOCONF_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * IPCOM AUTO Configuration. Do NOT modify this file!
 *
 * Also see config/ipcom_config.[ch].
 * Also see port/<rtos>/config/ipcom_pconfig.h for RTOS port specific config.
 */

/*
 ****************************************************************************
 * 2                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif


/*
 ****************************************************************************
 * 3                    FEATURES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         release
 *===========================================================================
 */

#define IPCOM_RELEASE 60704


/*
 *===========================================================================
 *                         misc
 *===========================================================================
 */

#if !defined(__FUNCTION__) && !defined(__GNUC__)
/* Define if your compiler does not support the __FUNCTION__ macro. */
#define __FUNCTION__  (const char *)0
#endif


/*
 *===========================================================================
 *                         IPCOM_FILE_ROOT
 *===========================================================================
 */
#if IPCOM_USE_FILE == IPCOM_FILE_IPCOM
#define IPCOM_FILE_ROOT   "/ram/"
#elif IPCOM_USE_FILE == IPCOM_FILE_NATIVE && defined(IPCOM_FILE_NATIVE_ROOT)
#define IPCOM_FILE_ROOT   IPCOM_FILE_NATIVE_ROOT
#else
#define IPCOM_FILE_ROOT   "."
#endif


/*
 *===========================================================================
 *                         IPCOM_USE_DMALLOC
 *===========================================================================
 * Use IPCOM debug malloc/free functions on top of the normal malloc/free.
 */
#if defined(IP_DEBUG) && \
   !defined(IPCOM_USE_DMALLOC) &&              \
   !defined(IPBOUNDS) && \
   !defined(IP_PORT_OSE5) && \
   !defined(IP_PORT_LKM) && \
   !defined(IP_PORT_VXWORKS) && \
   !defined(OSE_REFSYS) && \
   !defined(NETSNMP_INTERPEAK) && \
   !defined(IPEMANATE) && /*&& !defined(GATED_INTERPEAK)*/ \
   !defined(IPVALGRIND)
#define IPCOM_USE_DMALLOC
#endif


/*
 *===========================================================================
 *                         IPCOM_USE_TCP
 *===========================================================================
 * Define if you want to include TCP support.
 */
#if IPCOM_USE_SOCK == IPCOM_SOCK_NATIVE || defined(IPTCP) || \
    defined(IP_PORT_OSE5) || defined(OSE_REFSYS) || defined(IPLITE)
#define IPCOM_USE_TCP
#endif

#if !defined(IPCOM_USE_TCP) && IPCOM_USE_SHELL != IPCOM_SHELL_NONE && !defined(IPNGC)
#error TCP support is required for shell. Maybe you are missing IPTCP?
#endif


/*
 *===========================================================================
 *                         IPCOM_USE_ETHERNET
 *===========================================================================
 * Define if your stack supports Ethernet.
 */
#ifndef IP_TARGET_SC8102
#define IPCOM_USE_ETHERNET
#endif


/*
 *===========================================================================
 *                         IPCOM_USE_PACKET
 *===========================================================================
 * Define to support the IP_AF_PACKET domain.
 */
#if defined(IPNET) || defined(IPPRISM) || defined(IP8021X)
#define IPCOM_USE_PACKET
#endif


/*
 *===========================================================================
 *                         IPCOM_USRLIB
 *===========================================================================
 */
#if (defined(IP_PORT_OSE5) || defined(IPCOM_USE_USRLIB)) && !defined(IPCOM_USRLIB)
#define IPCOM_USRLIB
#endif


/*
 *===========================================================================
 *                         IPCOM_SOCK
 *===========================================================================
 * IPCOM_SOCK is set with IPLITE, IPNET or either.
 */
#if IPCOM_USE_SOCK == IPCOM_SOCK_IPNET \
 || IPCOM_USE_SOCK == IPCOM_SOCK_IPLITE \
 || IPCOM_USE_SOCK == IPCOM_SOCK_IPCOM
#define IPCOM_SOCK
#endif

#if IPCOM_USE_SOCK != IPCOM_SOCK_NONE && \
    !defined(IPCOM_USE_INET) && !defined(IPCOM_USE_INET6)
#error must_define_at_least_one_of_IPCOM_USE_INET_and_IPCOM_USE_INET6
#endif


#ifdef IPCOM_USE_HW_CHECKSUM
#define IPCOM_USE_HW_CHECKSUM_RX
#if !defined(IPIPSEC2) && !defined(IPCOM_USE_HW_CHECKSUM_TX_DISABLED)
    /* Use of IPSec and transmitt checksum CAN be solved, but it causes a lot of
       special cases. TX checksumming is almost free anyway if a good
       ipcom_in_checksum_memcpy() is present */
#define IPCOM_USE_HW_CHECKSUM_TX
#endif
#endif /* IPCOM_USE_HW_CHECKSUM */



#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
