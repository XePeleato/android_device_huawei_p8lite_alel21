/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipftpc_config.c,v $ $Revision: 1.2 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/ipftpc_config.c,v $
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
01a,03aug06,kch  Renamed INCLUDE_FTP_CMD to INCLUDE_IPFTP_CMD.
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
#ifdef INCLUDE_IPFTP_CMD
IP_EXTERN int ipftpc_cmd_main(int argc, char *argv[]);
#endif


/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipftpc_configure(void);


/*
 ****************************************************************************
 * 7                    DATA
 ****************************************************************************
 */

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
 *                    ipftpc_configure
 *===========================================================================
 * Description: Configure FTP client.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipftpc_configure(void)
{
    Ip_err ret = IPCOM_SUCCESS;

#ifdef INCLUDE_IPFTP_CMD
    ret = ipcom_shell_add_cmd("ftp", "ftp [options] [host]", "FTP client",
                              ipftpc_cmd_main,
                              IPCOM_SHELL_PRIO_SAME,
                              IPCOM_PROC_STACK_DEFAULT);
#endif /* INCLUDE_IPFTP_CMD */

    return ret;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
