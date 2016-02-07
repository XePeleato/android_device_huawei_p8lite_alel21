/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcavium_config.c,v $ $Revision: 1.1 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/ipcavium_config.c,v $
 *   $Author: markus $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2006 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *   Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
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
#include <ipcom_shell.h>


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

#ifdef INCLUDE_IPCAVIUM_CMD
IP_EXTERN int ipcavium_cmd_cavium(int argc, char **argv);
#endif


/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipcavium_configure(void);


/*
 ****************************************************************************
 * 7                    DATA
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         ipcavium_conf_sysvar
 *===========================================================================
 * Cavium configuration Variables
 */
IP_CONST Ipcom_sysvar ipcavium_conf_sysvar[] =
{
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
 *                      ipcavium_configure
 *===========================================================================
 * Description: Product configure hook. Called by IPCOM at startup.
 * Parameters:  none.
 * Returns:     IPCOM_SUCCESS = ok
 *              IPCOM_ERR_FAILED = error
 *
 */
IP_PUBLIC Ip_err
ipcavium_configure(void)
{
    Ip_err ret;

    ret = ipcom_sysvar_setv(ipcavium_conf_sysvar, 0);
    if(ret != IPCOM_SUCCESS)
        return ret;

#ifdef INCLUDE_IPCAVIUM_CMD
    ret = ipcom_shell_add_cmd("cavium", "cavium [cmd] <modifier...>",
                              "Test Cavium hardware crypto",
                              (Ipcom_shell_cmd_type)ipcavium_cmd_cavium,
                              IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_DEFAULT);
#endif

    return ret;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
