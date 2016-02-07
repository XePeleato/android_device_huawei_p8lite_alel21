/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_clib2.h,v $ $Revision: 1.12 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_clib2.h,v $
 *     $Author: lob $ $Date: 2005-11-01 15:16:09 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_CLIB2_H
#define IPCOM_CLIB2_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Macro wrapper for clib functions with 'va_list' function argument (evil!)
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_clibmap.h"

#include <stdarg.h>

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
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         ipcom_vsprintf
 *===========================================================================
 */
#if defined(IPCOM_VSPRINTF) && IPCOM_VSPRINTF != 1
#define ipcom_vsprintf IPCOM_VSPRINTF
#elif !defined(IPCOM_VSPRINTF)
#define ipcom_vsprintf vsprintf
#endif

#if (defined(IPCOM_VSPRINTF) && IPCOM_VSPRINTF == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_vsprintf(char *s, const char *format, va_list ap);
#endif


/*
 *===========================================================================
 *                         ipcom_vsnprintf
 *===========================================================================
 */

#if defined(IPCOM_VSNPRINTF) && IPCOM_VSNPRINTF != 1
#define ipcom_vsnprintf IPCOM_VSNPRINTF
#elif !defined(IPCOM_VSNPRINTF)
#define ipcom_vsnprintf vsnprintf
#endif

#if (defined(IPCOM_VSNPRINTF) && IPCOM_VSNPRINTF == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_vsnprintf(char *s, Ip_size_t n, const char *format, va_list ap);
#endif


/*
 *===========================================================================
 *                         ipcom_vfprintf
 *===========================================================================
 */

#if defined(IPCOM_VFPRINTF) && IPCOM_VFPRINTF != 1
#define ipcom_vfprintf IPCOM_VFPRINTF
#elif !defined(IPCOM_VFPRINTF)
#define ipcom_vfprintf vfprintf
#endif

#if (defined(IPCOM_VFPRINTF) && IPCOM_VFPRINTF == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_vfprintf(IP_FILE *stream, const char *format, va_list ap);
#endif


#ifdef __cplusplus
}
#endif

#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

