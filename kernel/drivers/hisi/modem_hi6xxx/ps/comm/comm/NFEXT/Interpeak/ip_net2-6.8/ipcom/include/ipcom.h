/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom.h,v $ $Revision: 1.16 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom.h,v $
 *     $Author: lob $ $Date: 2004-05-04 12:00:50 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_H
#define IPCOM_H

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

#include "ipcom_config.h"

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

/* target specific types and definitions */
#include "ipcom_type.h"

/* types and macros */
#include "ipcom_tags.h"
#include "ipcom_cstyle.h"
#include "ipcom_err.h"
#include "ipcom_errno.h"

/* functions */
#include "ipcom_auth.h"
#include "ipcom_buffer.h"
#include "ipcom_clib.h"
#include "ipcom_clib2.h"
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
#include "ipcom_file.h"
#endif
#include "ipcom_getopt.h"
#include "ipcom_hash.h"
#include "ipcom_inet.h"
#include "ipcom_ipd.h"
#include "ipcom_list.h"
#include "ipcom_prof.h"
#ifdef IPCOM_USE_ROUTE
#include "ipcom_route.h"
#endif
#if IPCOM_USE_SHELL != IPCOM_SHELL_NONE
#include "ipcom_shell.h"
#endif
#include "ipcom_sock.h"
#include "ipcom_sock2.h"
#ifdef IPCOM_USE_INET6
#include "ipcom_sock6.h"
#endif
#include "ipcom_strlib.h"
#include "ipcom_syslog.h"
#include "ipcom_sysvar.h"
#include "ipcom_time.h"
#include "ipcom_tmo.h"

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

#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

