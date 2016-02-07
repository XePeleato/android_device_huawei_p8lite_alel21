/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_clibmap.h,v $ $Revision: 1.24 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/include/ipcom_clibmap.h,v $
 *     $Author: rboden $ $Date: 2008-04-17 09:33:22 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_CLIBMAP_H
#define IPCOM_CLIBMAP_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 2                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipcom_config.h"

#ifdef __cplusplus
extern "C" {
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#ifdef IP_DEBUG
#define IPCOM_MALLOC 1
#define IPCOM_CALLOC 1
#define IPCOM_REALLOC 1
#define IPCOM_FREE 1
#endif /* IP_DEBUG */

#define IPCOM_RANDOM 1
#define IPCOM_SRANDOM 1
#define IPCOM_RAND 1
#define IPCOM_SRAND 1

#define IPCOM_EXIT 1
#define IPCOM_ABORT 1
#define IPCOM_SIGNAL 1

#define IPCOM_GETUID 1

#define IPCOM_PERROR 1
#define IPCOM_STRERROR 1
#define IPCOM_STRERROR_R 1

#define IPCOM_STRCASECMP 1
#define IPCOM_STRNCASECMP 1

#define IPCOM_QSORT 1
#define IPCOM_BSEARCH 1
#define IPCOM_ISASCII 1

#define IPCOM_MICROTIME 1
#define IPCOM_TIMES 1
#define IPCOM_GETTIMEOFDAY 1
#define IPCOM_SETTIMEOFDAY 1
#define IPCOM_ASCTIME_R ipcom_asctime_r_vxworks
#define IPCOM_CTIME_R ipcom_ctime_r_vxworks

#define IPCOM_SETHOSTNAME 1

#define IPCOM_TZSET 1

#if IP_PORT_VXWORKS <= 60
#define IPCOM_SNPRINTF 1
#define IPCOM_VSNPRINTF 1
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

