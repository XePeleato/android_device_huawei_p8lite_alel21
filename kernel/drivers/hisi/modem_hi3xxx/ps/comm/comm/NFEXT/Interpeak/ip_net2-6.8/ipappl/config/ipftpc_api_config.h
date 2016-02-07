/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipftpc_api_config.h,v $ $Revision: 1.7 $
 *   $Source: /home/interpeak/CVSRoot/ipappl/config/ipftpc_api_config.h,v $
 *   $Author: jyang $ $Date: 2009-01-24 03:20:00 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Ulf Olofsson <ulf@interpeak.se>
 ******************************************************************************
 */
#ifndef IPFTPC_API_CONFIG_H
#define IPFTPC_API_CONFIG_H

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

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Define this to enable verbatim console output */
/*#define IPFTPC_USE_VERBATIM_CONOUT*/

/* Define this to remove partially transferred files */
/*#define IPFTPC_USE_REMOVE_FILE_ON_ERROR*/
  
/* Data transfer buffer size */    
#define IPFTPC_DATABUFSIZE 4096

/* FTP command buffer size */    
#define IPFTPC_SINBUFSIZE 1024

/* Max no of simultaneous sessions */
#define IPFTPC_MAX_SESSIONS 8

/* Max no of arguments for FTP command */
#define IPFTPC_MAX_ARGS 10

/* Argument delimiters */
#define IPFTPC_DELIMITERS " ,|\r\n"

/* Directory path separators */
#define IPFTPC_DIRSEP "/\\"

/* Timeout when sending data to socket */
#define IPFTPC_SEND_TIMEOUT 30

/* Timeout when receiving data to socket */
#define IPFTPC_RECEIVE_TIMEOUT 30

/* Attempts to bind data socket when using random ports */
#define IPFTPC_BIND_ATTEMPTS 5

/* Select level of verbosity for syslog messages */
#ifdef IP_DEBUG
 #define IPCOM_SYSLOG_PRIORITY IPCOM_LOG_DEBUG
#else
 #define IPCOM_SYSLOG_PRIORITY IPCOM_LOG_WARNING
#endif

/* Version */
#define IPFTPC_VERSION_API 1

/* Transport protocol for FTPC */
#undef IPFTPC_USE_SCTP

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

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
