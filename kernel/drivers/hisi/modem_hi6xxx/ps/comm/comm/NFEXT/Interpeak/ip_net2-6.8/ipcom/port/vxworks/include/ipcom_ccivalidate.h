/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_ccivalidate.h,v $ $Revision: 1.3 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/include/ipcom_ccivalidate.h,v $
 *     $Author: braghava $ $Date: 2007-09-28 09:44:18 $
 *     $State: Exp $ $Locker:  $
 *
 *     Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *     Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_CCIVALIDATE_H
#define IPCOM_CCIVALIDATE_H

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
#include "ipcom_config.h"
#include "ipcom_type.h"
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
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

IP_GLOBAL int  ipcom_cmd_ccivalidate(int argc, char **argv);
IP_GLOBAL int  ipcom_cmd_cciperformance(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef IPCOM_CCIVALIDATE_H*/

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
