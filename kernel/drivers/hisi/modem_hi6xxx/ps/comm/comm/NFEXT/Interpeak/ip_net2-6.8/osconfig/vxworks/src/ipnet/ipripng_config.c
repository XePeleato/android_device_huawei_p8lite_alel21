/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipripng_config.c,v $ $Revision: 1.2 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/ipripng_config.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2006 Wind River (http://www.windriver.com). All rights reserved.
 *   Design and implementation by Tong Luo <tong.luo@windriver.com>
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

#if 0
#include "ipripng_config.h"
#endif

#include "ipcom_cstyle.h"
#include "ipcom_err.h"
#include "ipcom_sysvar.h"
#include <ipcom_shell.h>

#if 0
#include <ipcom_clib.h>
#endif


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

IP_PUBLIC int ipripng_cmd_ripngctrl(int argc, char **argv);


/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipripng_configure(void);


/*
 ****************************************************************************
 * 7                    DATA
 ****************************************************************************
 */


/*
 *===========================================================================
 *                         SYSVAR CONFIGURATION
 *===========================================================================
 */

IP_STATIC IP_CONST Ipcom_sysvar ipripng_conf_sysvar[] =
{
    { IP_NULL, IP_NULL }
};




/*
 *===========================================================================
 *                         AUTHENTICATION
 *===========================================================================
 */


/*
 *===========================================================================
 *                    iprip_configure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipripng_configure(void)
{
    /* Initialize IPRIPng system variables. */
    (void)ipcom_sysvar_setv(ipripng_conf_sysvar, 0);

    (void)ipcom_shell_add_cmd("ripngctrl", "ripngctrl <command>",
                              "RIPng daemon control", ipripng_cmd_ripngctrl,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_DEFAULT);

    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

