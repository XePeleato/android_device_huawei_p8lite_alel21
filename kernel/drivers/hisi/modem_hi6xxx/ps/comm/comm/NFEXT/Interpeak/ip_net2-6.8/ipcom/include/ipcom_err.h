/*
******************************************************************************
*                     HEADER FILE
*
*     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_err.h,v $ $Revision: 1.14 $
*     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_err.h,v $
*     $Author: lob $ $Date: 2004-11-05 15:59:01 $
*     $State: Exp $ $Locker:  $
*
*     INTERPEAK_COPYRIGHT_STRING
*     Design and implementation by Lennart Bang <lob@interpeak.se>
******************************************************************************
*/
#ifndef IPCOM_ERR_H
#define IPCOM_ERR_H

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

#include "ipcom_cstyle.h"

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
 *                         success/failure
 *===========================================================================
 */

/* System call succeeded. */
#define IPCOM_SUCCESS                  0

/* System call failed (general failure). */
#define IPCOM_ERR_FAILED              -1000


/*
 *===========================================================================
 *                         general failures
 *===========================================================================
 */

/* Operation failed because insufficient memory is available. */
#define IPCOM_ERR_NO_MEMORY           -1010

/* failed because (entry) not found. */
#define IPCOM_ERR_NOT_FOUND           -1011

/* failed because not supported. */
#define IPCOM_ERR_NOT_SUPPORTED       -1012

/* failed because permission denied. */
#define IPCOM_ERR_PERMISSION_DENIED   -1013


/*
 *===========================================================================
 *                         'already' failures
 *===========================================================================
 */

/* failed to open because already opened. */
#define IPCOM_ERR_ALREADY_OPEN        -1040

/* failed to close because already closed. */
#define IPCOM_ERR_ALREADY_CLOSED      -1041

/* failed to install because already installed. */
#define IPCOM_ERR_ALREADY_INSTALLED   -1042

/* failed to start because already started. */
#define IPCOM_ERR_ALREADY_STARTED     -1043


/*
 *===========================================================================
 *                         'not' failures
 *===========================================================================
 */

/* failed because not opened. */
#define IPCOM_ERR_NOT_OPENED          -1050

/* failed because not closed. */
#define IPCOM_ERR_NOT_CLOSED          -1051

/* failed because not installed. */
#define IPCOM_ERR_NOT_INSTALLED       -1052

/* Operation failed because interface is not started. */
#define IPCOM_ERR_NOT_STARTED         -1053


/*
 *===========================================================================
 *                         'invalid' failures
 *===========================================================================
 */

/* Invalid argument in function call. */
#define IPCOM_ERR_INVALID_ARG         -1060

/* Invalid configuration. */
#define IPCOM_ERR_INVALID_CONFIG      -1061

/* Invalid version number. */
#define IPCOM_ERR_INVALID_VERSION     -1062

/* Invalid alignment. */
#define IPCOM_ERR_INVALID_ALIGNMENT   -1063

/* Invalid interface. */
#define IPCOM_ERR_INVALID_INTERFACE   -1064


/*
 *===========================================================================
 *                         route failures
 *===========================================================================
 */

/* No such route. */
#define IPCOM_ERR_NO_ROUTE            -1070

/* Dead route, i.e. scheduled for deletion. */
#define IPCOM_ERR_DEAD_ROUTE          -1071

/* Invalid route syntax. */
#define IPCOM_ERR_INVALID_ROUTE       -1072

/* Invalid, i.e. non-10contiguous mask. */
#define IPCOM_ERR_INVALID_MASK        -1073

/* Invalid route client, e.g. not found. */
#define IPCOM_ERR_INVALID_RTCLIENT    -1074


/*
 *===========================================================================
 *                         entries failures
 *===========================================================================
 */

/* Operation failed because there is a duplicate entry. */
#define IPCOM_ERR_DUPLICATE           -1080

/* Command failed because entry is locked. */
#define IPCOM_ERR_LOCKED              -1081

/* No more entries, end of operation, list, array etc. */
#define IPCOM_ERR_END                 -1082

/* Operation failed because entry is read only. */
#define IPCOM_ERR_READONLY            -1083


/*
 *===========================================================================
 *                         tag failures
 *===========================================================================
 */

/* Invalid tag value. */
#define IPCOM_ERR_INVALID_TAG_VALUE   -1090

/* Unsupported tag. */
#define IPCOM_ERR_UNSUPPORTED_TAG     -1091


/*
 *===========================================================================
 *                         packet failures
 *===========================================================================
 */

/* Invalid packet. */
#define IPCOM_ERR_INVALID_PACKET      -1100

/* Badly formatted, i.e. corrupt/invalid packet. */
#define IPCOM_ERR_BAD_PACKET          -1101

/* Packet ignored (not necessarily an error) */
#define IPCOM_ERR_IGNORED_PACKET      -1102


/*
 *===========================================================================
 *                         BSD failures
 *===========================================================================
 */

/* System call failed due to BSD socket system-call failed. */
#define IPCOM_ERR_SOCKCALLFAILED      -1110


/*
 *===========================================================================
 *                         'min/max' failures
 *===========================================================================
 */

/* Operation failed because minimum is reached. */
#define IPCOM_ERR_REACHED_MIN         -1111

/* Operation failed because maximum is reached. */
#define IPCOM_ERR_REACHED_MAX         -1112


/*
 *===========================================================================
 *                         system call failures
 *===========================================================================
 */

/* Operation timed out and failed. */
#define IPCOM_ERR_TIMEOUT             -1120

/* Operation failed because blocking is illegal. */
#define IPCOM_ERR_WOULDBLOCK          -1121

/* Operation was interrupted */
#define IPCOM_ERR_INTERRUPT           -1122

/* Operation is pending. */
#define IPCOM_ERR_PENDING             -1123


/*
 *===========================================================================
 *                         authentication failures
 *===========================================================================
 */

/* Operation failed because authentication failed. */
#define IPCOM_ERR_AUTH_FAILED         -1130

/* Unknown user in authentication. */
#define IPCOM_ERR_AUTH_UNKNOWN        -1131


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
 *                    ipcom_err_string
 *===========================================================================
 * Description:   Return error string.
 * Parameters:    Ip_err error code.
 * Returns:       error string.
 *
 */
IP_PUBLIC char *ipcom_err_string(Ip_err err);


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

