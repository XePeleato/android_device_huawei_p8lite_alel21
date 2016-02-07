/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: iptftpc_config.c,v $ $Revision: 1.2 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/iptftpc_config.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 ******************************************************************************
 */

/*
modification history
--------------------
01e,03jan07,kch  Updated CVS revision log entry for Jan 03 code drop.
01d,01dec06,kch  Updated CVS revision log entry.
01c,13sep06,kch  Refresh with Sept 08, 2006 code drop
01b,28aug06,kch  Update based on Aug 22, 2006 code drop.
01a,03aug06,kch  Renamed INCLUDE_TFTP_CLIENT_CMD to INCLUDE_IPTFTP_CLIENT_CMD.
*/

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * TFTP client configuration.
 */

/*
 ****************************************************************************
 * 2                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
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

#ifdef INCLUDE_IPTFTP_CLIENT_CMD
IP_EXTERN int iptftp_cmd_tftp(int argc, char **argv);
#endif

/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err iptftpc_configure(void);


/*
 ****************************************************************************
 * 7                    DATA
 ****************************************************************************
 */

IP_EXTERN IP_CONST Ipcom_sysvar iptftp_conf_sysvar[];


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
 *                    iptftpc_configure
 *===========================================================================
 * Description: Configure TFTP client.
 * Parameters:  none.
 * Returns:     nothing.
 */
IP_PUBLIC Ip_err
iptftpc_configure(void)
{
    Ip_err ret = IPCOM_SUCCESS;

    (void)ipcom_sysvar_setv(iptftp_conf_sysvar, IPCOM_SYSVAR_FLAG_OVERWRITE);

#ifdef INCLUDE_IPTFTP_CLIENT_CMD
    ret = ipcom_shell_add_cmd("tftp", "tftp [-a] <get|put> <host> ...",
                              "Trivial FTP client",
                              iptftp_cmd_tftp,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_DEFAULT);
#endif /* INCLUDE_IPTFTP_CLIENT_CMD */

    return ret;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
