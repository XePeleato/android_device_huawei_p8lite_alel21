/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_sysctl.h,v $ $Revision: 1.3 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_sysctl.h,v $
 *   $Author: lob $ $Date: 2007-01-18 16:27:49 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPNET_SYSCTL_H
#define IPNET_SYSCTL_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */
/* IPNET sysctl */

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
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    Ipnet_cmd_sysctl
 *===========================================================================
 * Parameter struct used for registering IPNET base table.
 */
typedef struct Ipnet_cmd_sysctl_struct
{
    const char                              *comp;

    int                                     type;
    int                                     argument;

    const struct Ipnet_cmd_sysctl_struct    *ntable;
    int                                     ctl[3];
} Ipnet_cmd_sysctl;

/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_sysctl_get_base
 *===========================================================================
 * Description: This routine exposes the IPNET sysctl tree root node.
 * Parameters: None.
 * Returns: A pointer to Ipnet_cmd_sysctl
 *
 */
IP_PUBLIC Ipnet_cmd_sysctl * ipnet_sysctl_get_base(void);

#ifdef __cplusplus
}
#endif

#endif /* IPNET_SYSCTL_H */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
