/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipwlan_broadcom_config.c,v $ $Revision: 1.5 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/ipwlan_broadcom_config.c,v $
 *   $Author: dbush $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *   Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */

/*
modification history
--------------------
01i,29oct07,dwb  Fix for WIND00109453
01h,15oct07,dwb  Fix for WIND00108079
01g,14sep07,spw  Removed definition of IPCOM_USE_CLIB_PROTO
01f,25apr07,rime Created
*/

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * BroadCom wireless configuration
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
IP_EXTERN int ipwlan_broadcom_cmd_broadcom(int argc, char **argv);

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipwlan_broadcom_configure(void);

#define IPBCM_STR2(a) #a
#define IPBCM_STR(a) IPBCM_STR2(a)
/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */
const char *ipwlan_broadcom_default_if = BCM43XX_DEFAULT_INTERFACE;
const char *ipwlan_broadcom_attach[] = {"wifi_bcm " BCM43XX_DEFAULT_INTERFACE " attach"};

const char *ipwlan_broadcom_config[] =
{
	"wifi " BCM43XX_DEFAULT_INTERFACE " debug 0x0",
	
#ifdef BCM43XX_DEFAULT_MODE
	"wifi " BCM43XX_DEFAULT_INTERFACE " mode " BCM43XX_DEFAULT_MODE,
#endif
#ifdef BCM43XX_DEFAULT_CHANNEL
	"wifi " BCM43XX_DEFAULT_INTERFACE " channel " IPBCM_STR(BCM43XX_DEFAULT_CHANNEL),
#endif

    "wifi " BCM43XX_DEFAULT_INTERFACE " auth open",
    "wifi " BCM43XX_DEFAULT_INTERFACE " encr none",

#ifdef INCLUDE_BCM43XX_CIPH_AES
    "wifi " BCM43XX_DEFAULT_INTERFACE " encr ccmp",
#endif
#ifdef INCLUDE_BCM43XX_CIPH_TKIP
    "wifi " BCM43XX_DEFAULT_INTERFACE " encr tkip",
#endif	

#ifdef BCM43XX_DEFAULT_COUNTRY
	"wifi " BCM43XX_DEFAULT_INTERFACE " country " IPBCM_STR(BCM43XX_DEFAULT_COUNTRY),
#endif	
#ifdef BCM43XX_DEFAULT_WEPKEYID
    "wifi " BCM43XX_DEFAULT_INTERFACE " auth shared-key ",
#ifdef BCM43XX_DEFAULT_WEP40KEY
    "wifi " BCM43XX_DEFAULT_INTERFACE " encr wep " IPBCM_STR(BCM43XX_DEFAULT_WEPKEYID) " " BCM43XX_DEFAULT_WEP40KEY,
#endif
#ifdef BCM43XX_DEFAULT_WEP104KEY
    "wifi " BCM43XX_DEFAULT_INTERFACE " encr wep " IPBCM_STR(BCM43XX_DEFAULT_WEPKEYID) " " BCM43XX_DEFAULT_WEP104KEY,
#endif
    "wifi " BCM43XX_DEFAULT_INTERFACE " encr wep " IPBCM_STR(BCM43XX_DEFAULT_WEPKEYID),
#endif

#ifndef BCM43XX_DEFAULT_WEPKEYID
#ifndef INCLUDE_BCM43XX_AUTH_WPA_PSK
#ifndef INCLUDE_BCM43XX_AUTH_WPA_PSK_PASSPHRASE
#ifndef INCLUDE_BCM43XX_AUTH_WPA2_PSK
#ifndef INCLUDE_BCM43XX_AUTH_WPA2_PSK_PASSPHRASE
#ifndef INCLUDE_BCM43XX_AUTH_WPA_8021X
#ifndef INCLUDE_BCM43XX_AUTH_WPA2_8021X    
    "wifi " BCM43XX_DEFAULT_INTERFACE " auth open",
    "wifi " BCM43XX_DEFAULT_INTERFACE " encr none",
#endif
#endif
#endif
#endif
#endif
#endif
#endif    

#ifdef INCLUDE_BCM43XX_AUTH_WPA_PSK    
    "wifi " BCM43XX_DEFAULT_INTERFACE " encr wpa " BCM43XX_DEFAULT_PSK_KEY,
#endif
#ifndef INCLUDE_BCM43XX_AUTH_WPA_PSK
#ifdef INCLUDE_BCM43XX_AUTH_WPA_PSK_PASSPHRASE   
    "wifi " BCM43XX_DEFAULT_INTERFACE " encr wpa " BCM43XX_DEFAULT_PSK_PASSPHRASE,
#endif
#endif
    
#ifdef INCLUDE_BCM43XX_AUTH_WPA2_PSK    
    "wifi " BCM43XX_DEFAULT_INTERFACE " encr wpa2 " BCM43XX_DEFAULT_PSK_KEY,
#endif
#ifndef INCLUDE_BCM43XX_AUTH_WPA2_PSK    
#ifdef INCLUDE_BCM43XX_AUTH_WPA2_PSK_PASSPHRASE   
    "wifi " BCM43XX_DEFAULT_INTERFACE " encr wpa2 " BCM43XX_DEFAULT_PSK_PASSPHRASE,
#endif
#endif

#ifdef INCLUDE_BCM43XX_AUTH_WPA_PSK
	"wifi " BCM43XX_DEFAULT_INTERFACE " auth wpa-psk ",	
#endif
#ifdef INCLUDE_BCM43XX_AUTH_WPA_PSK_PASSPHRASE
	"wifi " BCM43XX_DEFAULT_INTERFACE " auth wpa-psk ",
#endif
#ifdef INCLUDE_BCM43XX_AUTH_WPA_8021X
	"wifi " BCM43XX_DEFAULT_INTERFACE " auth wpa ",
#endif
#ifdef INCLUDE_BCM43XX_AUTH_WPA2_PSK
	"wifi " BCM43XX_DEFAULT_INTERFACE " auth wpa2-psk ",	
#endif
#ifdef INCLUDE_BCM43XX_AUTH_WPA2_PSK_PASSPHRASE
	"wifi " BCM43XX_DEFAULT_INTERFACE " auth wpa2-psk ",
#endif	
#ifdef INCLUDE_BCM43XX_AUTH_WPA2_8021X
	"wifi " BCM43XX_DEFAULT_INTERFACE " auth wpa2 ",
#endif

#ifdef BCM43XX_DEFAULT_SSID
    "wifi " BCM43XX_DEFAULT_INTERFACE " ssid " BCM43XX_DEFAULT_SSID
#endif  
};

IP_PUBLIC IP_CONST int ipwlan_broadcom_config_len = sizeof(ipwlan_broadcom_config)/sizeof(ipwlan_broadcom_config[0]);

/*
 ****************************************************************************
 * 9                    PUBLIC FUNCTIONS
 ****************************************************************************
 */
IP_EXTERN int ipwlan_cmd_wlan(int argc, char **argv);
/*
 *===========================================================================
 *                    ipwlan_broadcom_configure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipwlan_broadcom_configure(void)
{
	Ip_err ret;
	
    ret = ipcom_shell_add_cmd("wifi_bcm", "wifi_bcm", "BroadCom specific commands",
                               ipwlan_broadcom_cmd_broadcom, IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_DEFAULT);
							
	return ret;
}

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

