/* ipwps_config.c - IPWPS vxWorks specific configuration applet */
/*
* Copyright (c) 2008 Wind River Systems, Inc.
*
* The right to copy, distribute, modify, or otherwise make use
* of this software may be licensed only pursuant to the terms
* of an applicable Wind River license agreement.
* 
*/

/*
modification history
--------------------
01a,24Dec08,mry  Created
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
#include <ipcom_sysvar.h>
#include <ipcom_syslog.h>


#include "ipwps_sysvar.h"
#include "ipwps_common.h"

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

IP_GLOBAL void ipwps_cmd_init(void);

/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipwps_configure(void);


/*
 ****************************************************************************
 * 7                    DATA
 ****************************************************************************
 */

IP_CONST Ipcom_sysvar ipwps_conf_sysvar[] =
{
#ifdef INCLUDE_IPWPS_MODEL_CUSTOM
	{ IPWPS_SYSVAR_TEMPLATE, IPWPS_MODEL_DESC_CUST},
#elif defined(INCLUDE_IPWPS_MODEL_STANDALONE_AP)
	{ IPWPS_SYSVAR_TEMPLATE, IPWPS_MODEL_DESC_ST_AP},
#elif defined(INCLUDE_IPWPS_MODEL_STANDALONE_AP_WITH_REGISTRAR)
	{ IPWPS_SYSVAR_TEMPLATE, IPWPS_MODEL_DESC_ST_AP_REG},
#elif defined(INCLUDE_IPWPS_MODEL_ENROLLEE)
	{ IPWPS_SYSVAR_TEMPLATE, IPWPS_MODEL_DESC_ENROLLEE},

#endif
	
#ifdef INCLUDE_IPWPS
	{ IPWPS_SYSVAR_ENABLE, 				IPWPS_ENABLE },	
	{ IPWPS_SYSVAR_WLS_IF, 				IPWPS_WLS_IF },
#endif
	
#ifdef INCLUDE_IPWPS_COMMONDEVINFO
	{ IPWPS_SYSVAR_DEVINFO_MANUFACTURER,IPWPS_COMMONDEVINFO_MANUFACTURER },
	{ IPWPS_SYSVAR_DEVINFO_MODELNAME, 	IPWPS_COMMONDEVINFO_MODELNAM },
	{ IPWPS_SYSVAR_DEVINFO_MODELNO, 	IPWPS_COMMONDEVINFO_MODELNO },
	{ IPWPS_SYSVAR_DEVINFO_SERIALNO, 	IPWPS_COMMONDEVINFO_SERIALNO },
	{ IPWPS_SYSVAR_DEVINFO_DEVNAME, 	IPWPS_COMMONDEVINFO_DEVNAM },
	{ IPWPS_SYSVAR_DEVINFO_CATID, 		IPWPS_COMMONDEVINFO_CATID },
	{ IPWPS_SYSVAR_DEVINFO_OUI, 		IPWPS_COMMONDEVINFO_OUI },
	{ IPWPS_SYSVAR_DEVINFO_SUBCATID, 	IPWPS_COMMONDEVINFO_SUBCATID },
#endif

#ifdef INCLUDE_IPWPS_CONFIG_PIN
	{ IPWPS_SYSVAR_INCLUDE_CONFIGMEHTOD_PIN,	"1" },
	{ IPWPS_SYSVAR_PIN_ENABLE,					IPWPS_CONFIG_PIN_ENABLE },
#if defined(INCLUDE_IPWPS_CONFIG_PIN_AUTO)
	{ IPWPS_SYSVAR_PIN_MODE, 					IPWPS_PINMODE_AUTO},
#elif defined(INCLUDE_IPWPS_CONFIG_PIN_HARDCODE)
	{ IPWPS_SYSVAR_PIN_MODE, 					IPWPS_PINMODE_HARDCODE},
	{ IPWPS_SYSVAR_PIN_HARDCODED_VAL,			IPWPS_PIN_HARDCODED_VALUE},
#elif defined (INCLUDE_IPWPS_CONFIG_PIN_INTERACTIVE_USER)
	{ IPWPS_SYSVAR_PIN_MODE, 					IPWPS_PINMODE_INTERACT_USER},
#elif defined (INCLUDE_IPWPS_CONFIG_PIN_INTERACTIVE_REGS)
	{ IPWPS_SYSVAR_PIN_MODE, 					IPWPS_PINMODE_INTERACT_REGS},
#endif	
#else 
	{ IPWPS_SYSVAR_INCLUDE_CONFIGMEHTOD_PIN,	"0" },
#endif
	
#ifdef INCLUDE_IPWPS_CONFIG_PBC		
	{ IPWPS_SYSVAR_INCLUDE_CONFIGMEHTOD_PBC,	"1" },
	{ IPWPS_SYSVAR_PBC_ENABLE,					IPWPS_CONFIG_PBC_ENABLE },
#else 
	{ IPWPS_SYSVAR_INCLUDE_CONFIGMEHTOD_PBC,	"0" },
#endif	
	
#ifdef INCLUDE_IPWPS_DRV_IF
	{ IPWPS_SYSVAR_DRVIF_ENABLE, 				"1" },
#endif
	
#ifdef INCLUDE_IPWPS_ENROLLEE
	{ IPWPS_SYSVAR_ENROLLEE_ENABLE, 	IPWPS_ENROLLEE_ENABLE },
	{ IPWPS_SYSVAR_ENROLLEE_UUID,	 	IPWPS_ENROLLEE_UUID },
	{ IPWPS_SYSVAR_ENROLLEE_AUTO_REG_INIT, 		IPWPS_ENROLLEE_AUTO_REG_INIT },
	{ IPWPS_SYSVAR_ENROLLEE_NON_WPS_DRV, 		IPWPS_ENROLLEE_NON_WPS_DRIVER },
#ifdef INCLUDE_IPWPS_ENROLLEE_ADHOC
	{ IPWPS_SYSVAR_ENROLLEE_ADHOC,		"1" },
#else
	{ IPWPS_SYSVAR_ENROLLEE_ADHOC,		"0" },
#endif	
#endif
	
#ifdef INCLUDE_IPWPS_AP
	{ IPWPS_SYSVAR_AP_ENABLE, 			IPWPS_AP_ENABLE },
	{ IPWPS_SYSVAR_AP_UUID,	 			IPWPS_AP_UUID },
#endif	
	
#ifdef 	INCLUDE_IPWPS_REGISTRAR
	{ IPWPS_SYSVAR_REGISTRAR_ENABLE, 	IPWPS_REGISTRAR_ENABLE },
	{ IPWPS_SYSVAR_REGISTRAR_IPWPS_OFFER_ALL_CREDS, 
										IPWPS_REGISTRAR_OFFER_ALL_CREDS},
	{ IPWPS_SYSVAR_REGISTRAR_REG_MODE, 	IPWPS_REGISTRAR_REG_MODE },
	{ IPWPS_SYSVAR_REGISTRAR_UUID, 		IPWPS_REGISTRAR_UUID },
#endif
	
#ifdef 	INCLUDE_IPWPS_CRED_CONFIG_AUTO
	{ IPWPS_SYSVAR_CREDENTIAL_METHOD, 		IPWPS_CRED_CONFIG_METHOD_AUTO},
	{ IPWPS_SYSVAR_CREDENTIAL_AUTO_KEYLEN, 	IPWPS_CRED_CONFIG_AUTO_KEYLEN},
	{ IPWPS_SYSVAR_CREDENTIAL_AUTO_NETINDEX,IPWPS_CRED_CONFIG_AUTO_NETINDEX},
	{ IPWPS_SYSVAR_CREDENTIAL_AUTO_SSID,	IPWPS_CRED_CONFIG_AUTO_SSID},
#elif defined(INCLUDE_IPWPS_CRED_CONFIG_DEF) 	
	{ IPWPS_SYSVAR_CREDENTIAL_METHOD, 		IPWPS_CRED_CONFIG_METHOD_DEFAULT},
	{ IPWPS_SYSVAR_CREDENTIAL_DEF_NETINDEX,	IPWPS_CRED_CONFIG_NETINDEX},
	{ IPWPS_SYSVAR_CREDENTIAL_DEF_SSID, 	IPWPS_CRED_CONFIG_SSID},
	{ IPWPS_SYSVAR_CREDENTIAL_DEF_AUTH, 	IPWPS_CRED_CONFIG_AUTH},
	{ IPWPS_SYSVAR_CREDENTIAL_DEF_ENCR, 	IPWPS_CRED_CONFIG_ENCR},
	{ IPWPS_SYSVAR_CREDENTIAL_DEF_KEYINDEX, IPWPS_CRED_CONFIG_KEYINDEX},
	{ IPWPS_SYSVAR_CREDENTIAL_DEF_KEY, 		IPWPS_CRED_CONFIG_KEY},
	{ IPWPS_SYSVAR_CREDENTIAL_DEF_8021X, 	IPWPS_CRED_CONFIG_8021X},
	{ IPWPS_SYSVAR_CREDENTIAL_DEF_EAP_ID, 	IPWPS_CRED_CONFIG_EAP_ID},
	{ IPWPS_SYSVAR_CREDENTIAL_DEF_EAP_TYPE, IPWPS_CRED_CONFIG_EAP_TYPES},
#elif defined(INCLUDE_IPWPS_CRED_CONFIG_INTERACTIVE) 	
	{ IPWPS_SYSVAR_CREDENTIAL_METHOD, 		IPWPS_CRED_CONFIG_METHOD_INTERACT},
#endif
	
#ifdef INCLUDE_IPWPS_PROTO
	{ IPWPS_SYSVAR_WPS_PROTO_ENABLE,	IPWPS_PROTO_ENABLE },
#endif
	
    { IP_NULL, 							IP_NULL}
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
 *                    ipwps_configure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipwps_configure(void)
{	
	Ip_err rc = IPCOM_SUCCESS;
	
    (void)ipcom_sysvar_setv(ipwps_conf_sysvar, 0);

    ipcom_openlog(IP_NULL, 0, IPCOM_LOG_IPWPS); 
    ipcom_setlogmask_on(IPWPS_LOG_LEVEL, IPCOM_LOG_IPWPS);
   

#ifdef INCLUDE_IPWPS_CMD
    ipwps_cmd_init();
#endif

	return rc;
}

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

