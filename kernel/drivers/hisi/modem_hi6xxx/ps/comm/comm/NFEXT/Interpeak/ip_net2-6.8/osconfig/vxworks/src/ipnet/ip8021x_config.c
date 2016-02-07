/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ip8021x_config.c,v $ $Revision: 1.7 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/ip8021x_config.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2006 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *   Design and implementation by Roger Boden <roger@interpeak.se>
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

#include <ipcom_cstyle.h>
#include <ipcom_clib.h>
#include <ipcom_err.h>
#include <ipcom_shell.h>
#include <ipcom_sysvar.h>
#include <ipcom_syslog.h>

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

IP_GLOBAL int ip8021x_cmd_8021x(int argc, char** argv);

#ifdef INCLUDE_IP8021X_SUPPLICANT_MIB
IP_EXTERN IP_GLOBAL void ip8021x_vxmib_handler_install(void);
#endif

/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ip8021x_configure(void);


/*
 ****************************************************************************
 * 7                    DATA
 ****************************************************************************
 */

IP_CONST Ipcom_sysvar ip8021x_conf_sysvar[] =
{
	{ "ip8021x.if_name", 		IPSUPP_IFNAM },
    { "ip8021x.ssid",			IPSUPP_SSID },
    { "ip8021x.psk", 			IPSUPP_PSK },
    { "ip8021x.auth_type", 		IPSUPP_AUTHTYPE }, /* wps | wired | wpa-psk | wpa2-psk | wpa | wpa2 */
    { "ip8021x.encr_type", 		IPSUPP_ENCRTYPE }, /* tkip | ccmp */
    { "ip8021x.enforce_rfc4017",IPSUPP_ENFORCE_RFC4017 }, 
    { "ip8021x.auth_period", 	IPSUPP_AUTHPERIOD },
    { "ip8021x.start_period", 	IPSUPP_STARTPERIOD },
    { "ip8021x.held_period", 	IPSUPP_HELDPERIOD },
    { "ip8021x.max_start", 		IPSUPP_MAXSTART },
    { IP_NULL, 					IP_NULL }
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
 *                    ip8021x_configure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ip8021x_configure(void)
{	
    (void)ipcom_sysvar_setv(ip8021x_conf_sysvar, 0);

    ipcom_openlog(IP_NULL, 0, IPCOM_LOG_IP8021X);
    ipcom_setlogmask_on(IP8021X_LOG_LEVEL, IPCOM_LOG_IP8021X);

#if 1
    ipcom_shell_add_cmd("8021x", "8021x <connect|disconnect|logoff|getstatus|add|delete|list|eapcr>", "8021x", ip8021x_cmd_8021x,
                        IPCOM_PROC_PRIO_4, IPCOM_PROC_STACK_DEFAULT);
#endif
#ifdef INCLUDE_IP8021X_SUPPLICANT_MIB
    ip8021x_vxmib_handler_install();
#endif

	return IPCOM_SUCCESS;
}

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

