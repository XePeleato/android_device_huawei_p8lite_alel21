/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_waitif.h,v $ $Revision: 1.1 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_waitif.h,v $
 *     $Author: uolofsso $ $Date: 2007-09-25 13:14:07 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 ******************************************************************************
 */
#ifndef IPCOM_WAITIF_H
#define IPCOM_WAITIF_H

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

#ifdef __cplusplus
extern "C" {
#endif


    /*
 ****************************************************************************
 * 5                    FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_waitif
 *===========================================================================
 * Description: Wait for interface to get an IP address assigned.
 * Parameters:  ifindex - Index of the interface to wait for.
 *                        0 = any interface except loopback.
 *              domain - The address domain.
 * Returns:     0 = success, -1 = error, and sets errno.
 *
 */
IP_PUBLIC int
ipcom_waitif(int ifindex, int domain);

#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

