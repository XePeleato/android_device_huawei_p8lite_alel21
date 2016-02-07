/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_h.h,v $ $Revision: 1.66 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_h.h,v $
 *     $Author: lob $ $Date: 2005-06-16 07:05:44 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_H_H
#define IPCOM_H_H

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

#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_syslog.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM


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
 *                         ipcom_create
 *===========================================================================
 * Description:  Port specific start of IPCOM.
 *               Should call ipcom_start() after initializing port.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_create(void);


/*
 *===========================================================================
 *                         ipcom_start
 *===========================================================================
 * Description:  Create and start all IPCOM modules and Interpeak products.
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipcom_start(void);


/*
 *===========================================================================
 *                         ipcom_usr_create
 *===========================================================================
 * Description:  User specific start of IPCOM.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_usr_create(void);


#ifdef __cplusplus
}
#endif

#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
