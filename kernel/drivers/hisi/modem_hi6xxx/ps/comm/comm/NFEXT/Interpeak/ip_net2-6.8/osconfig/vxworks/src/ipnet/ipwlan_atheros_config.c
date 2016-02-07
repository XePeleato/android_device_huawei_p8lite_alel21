/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipwlan_atheros_config.c,v $ $Revision: 1.3 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/ipwlan_atheros_config.c,v $
 *   $Author: myakan $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *   Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */

/*
modification history
--------------------
01h,14sep07,spw  Removed definition of IPCOM_USE_CLIB_PROTO
01g,23aug07,spw  Changed INCLUDE_DOT11_END to INCLUDE_IPWLAN_ATHEROS
01f,25apr07,spw  Created
*/

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Atheros wireless configuration
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

#ifdef IPCOM_USE_CLIB_PROTO
#undef IPCOM_USE_CLIB_PROTO
#endif

#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_sock.h>

#include <ipcom_syslog.h>
#include <ipcom_errno.h>
#include <ipcom_sysvar.h>
#include <ipcom_os.h>
#include <ipcom_shell.h>
#include <ipcom_netif.h>

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
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */
IP_EXTERN int ipwlan_atheros_cmd_atheros(int argc, char **argv);

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipwlan_atheros_configure(void);

#define IPATH_STR2(a) #a
#define IPATH_STR(a) IPATH_STR2(a)
/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */
const char *ipwlan_atheros_attach[] = {"wifi_ath " AR521X_DEFAULT_INTERFACE " attach"};
const char *ipwlan_atheros_default_if = AR521X_DEFAULT_INTERFACE;

const char *ipwlan_atheros_config[] =
{
	"wifi_ath " AR521X_DEFAULT_INTERFACE " debug 0x0"
};

IP_PUBLIC IP_CONST int ipwlan_atheros_config_len = sizeof(ipwlan_atheros_config)/sizeof(ipwlan_atheros_config[0]);

/*
 ****************************************************************************
 * 9                    PUBLIC FUNCTIONS
 ****************************************************************************
 */
IP_EXTERN int ipwlan_cmd_wlan(int argc, char **argv);
/*
 *===========================================================================
 *                    ipwlan_atheros_configure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipwlan_atheros_configure(void)
{
	Ip_err ret;
	
    ret = ipcom_shell_add_cmd("wifi_ath", "wifi_ath", "Atheros specific commands",
                               ipwlan_atheros_cmd_atheros, IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_DEFAULT);
							
	return ret;
}

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

