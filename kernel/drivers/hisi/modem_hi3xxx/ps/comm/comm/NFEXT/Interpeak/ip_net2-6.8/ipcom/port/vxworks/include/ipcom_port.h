/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_port.h,v $ $Revision: 1.5.126.1 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/include/ipcom_port.h,v $
 *     $Author: ulf $ $Date: 2010-07-02 13:20:57 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *        Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_PORT_H
#define IPCOM_PORT_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Interpeak VxWorks port.
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
 *                         Ipcom_ipc
 *===========================================================================
 * typedef struct Ipcom_ipc
 * {
 *     Ip_u32     magic;
 *     Ip_pid_t   peer_pid;
 * }
 * Ipcom_ipc;
 */
#define IPCOM_IPC_TYPE_SIZE   2


/*
 *===========================================================================
 *                         process declaration
 *===========================================================================
 */

/* Process prototype: */
#ifdef __cplusplus
#define IPCOM_PROCESS(xname) extern "C" void xname(void)
#else
#define IPCOM_PROCESS(xname) void xname(void)
#endif


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

#define IP_S32     long
#define IP_U32     unsigned long
#define IP_SIZE_T  unsigned int
#define IP_SSIZE_T int

#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
