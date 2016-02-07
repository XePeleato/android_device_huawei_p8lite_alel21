/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipftps_config.h,v $ $Revision: 1.6 $
 *   $Source: /home/interpeak/CVSRoot/ipappl/config/ipftps_config.h,v $
 *   $Author: jyang $ $Date: 2009-01-24 03:20:00 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Ulf Olofsson <ulf@interpeak.se>
 ******************************************************************************
 */
#ifndef IPFTPS_CONFIG_H
#define IPFTPS_CONFIG_H

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

#include <ipcom_config.h>

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Test system requires 'IPFTPS_USE_TEST' */
#ifdef IPTESTENGINE
#if !defined(IPFTPS_USE_TEST)
#define IPFTPS_USE_TEST
#endif
#endif

/* Data transfer buffer size */
#define IPFTPS_DATABUFSIZE 4096

/* FTP command buffer size */
#define IPFTPS_CMDBUFSIZE 1024

/* Max no of arguments for FTP command */
#define IPFTPS_MAX_ARGS 10

/* Argument delimiters */
#define IPFTPS_DELIMITERS " ,|"

/* Directory path separators */
#define IPFTPS_DIRSEP "/\\"

/* Attempts to bind data socket when using random ports */
#define IPFTPS_BIND_ATTEMPTS 5

/* Mimic IIS behaviour (send 150 before establishing data connection) */
/*#define IPFTPS_USE_IIS_150*/

/* Select level of verbosity for syslog messages if it is not defined */
#ifndef IPCOM_SYSLOG_PRIORITY
 #ifdef IP_DEBUG
  #define IPCOM_SYSLOG_PRIORITY IPCOM_LOG_DEBUG
 #else
  #define IPCOM_SYSLOG_PRIORITY IPCOM_LOG_WARNING
 #endif
#endif /* IPCOM_SYSLOG_PRIORITY */


/* Transport protocol for FTPS */
#undef IPFTPS_USE_SCTP

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
