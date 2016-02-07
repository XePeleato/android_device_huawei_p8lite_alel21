/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipfreescale_config.c,v $ $Revision: 1.3 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/ipfreescale_config.c,v $
 *   $Author: markus $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2006 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *   Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */

/*
modification history
--------------------
01a,02jul07,mcarlste  Added file contents
*/

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

#include <ipcom_type.h>       /* Import Ip_err */
#include <ipcom_cstyle.h>     /* Import IP_PUBLIC */
#include <ipcom_err.h>        /* Import IPCOM_SUCCESS */
#include <ipcom_sysvar.h>


/*
 ****************************************************************************
 * 3                    DEFINES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 4                    TYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 5                    EXTERN PROTOTYPES
 ****************************************************************************
 */

#ifdef INCLUDE_IPFREESCALE_CMD
IP_EXTERN int ipfreescale_cmd_freescale(int argc, char **argv);
#endif


/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipfreescale_configure(void);


/*
 ****************************************************************************
 * 7                    DATA
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         ipfreescale_conf_sysvar
 *===========================================================================
 * Freescale configuration Variables
 */
IP_CONST Ipcom_sysvar ipfreescale_conf_sysvar[] =
{
    /* SEC core version: 2.0, 2.1, 2.2 or 2.4 */
    { "ipfreescale.sec2.core_version",  SEC2_CORE_VERSION },

    /* Device family: PowerQUICC II Pro (2) or PowerQUICC III (3) */
    { "ipfreescale.sec2.device_family", SEC2_DEVICE_FAMILY },

    { IP_NULL, IP_NULL }
};


/*
 ****************************************************************************
 * 8                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                      ipfreescale_configure
 *===========================================================================
 * Description: Product configure hook. Called by IPCOM at startup.
 * Parameters:  none.
 * Returns:     IPCOM_SUCCESS = ok
 *              IPCOM_ERR_FAILED = error
 *
 */
IP_PUBLIC Ip_err
ipfreescale_configure(void)
{
    Ip_err ret;

    ret = ipcom_sysvar_setv(ipfreescale_conf_sysvar, 0);
    if(ret != IPCOM_SUCCESS)
        return ret;

#ifdef INCLUDE_IPFREESCALE_CMD
    ret = ipcom_shell_add_cmd("freescale", "freescale [cmd] <modifier...>",
                              "Test Freescale hardware crypto",
                              (Ipcom_shell_cmd_type)ipfreescale_cmd_freescale,
                              IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_DEFAULT);
#endif

    return ret;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

