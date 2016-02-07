/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_psock.h,v $ $Revision: 1.9 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/include/ipcom_psock.h,v $
 *     $Author: lob $ $Date: 2004-05-04 12:02:18 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *      Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_PSOCK_H
#define IPCOM_PSOCK_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * VxWorks WindNet TCP/IP stack defines.
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

#if IPCOM_USE_SOCK != IPCOM_SOCK_NATIVE
#error ipcom_psock_h_only_used_with_native_stack
#endif

#include "ipcom_type.h"

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
 *                         setsockopt
 *===========================================================================
 */

/* Standard option flags per-socket.  */
#define IP_SO_DEBUG        0x0001          /* Turn on debugging info recording. */
#undef IP_SO_KEEPALIVE
#define IP_SO_KEEPALIVE    0x0008          /* Keep TCP connections alive. */


/*
 *===========================================================================
 *                         select
 *===========================================================================
 */
#undef IP_FD_SETSIZE
#define IP_FD_SETSIZE     256              /* Default value for vxWorks */


/*
 *===========================================================================
 *                    IOCTL requests/replies
 *===========================================================================
 * IOCTL requests/replies. Values taken from ioLib.h.
 */
#undef IP_FIONBIO
#define IP_FIONBIO        16 /* FIONBIO set/clear non-blocking i/o */


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

