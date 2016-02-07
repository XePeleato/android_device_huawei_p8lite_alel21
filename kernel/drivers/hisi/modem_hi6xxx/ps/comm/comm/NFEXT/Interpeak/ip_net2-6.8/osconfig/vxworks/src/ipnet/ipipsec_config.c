/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipipsec_config.c,v $ $Revision: 1.9 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/ipipsec_config.c,v $
 *   $Author: lob $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2006 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
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

#include "ipipsec_config.h"

#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_sysvar.h>
#include <ipcom_os.h>
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

IPCOM_PROCESS(ipipsec_config);

#ifdef INCLUDE_IPSECCTRL_CMD
IP_PUBLIC int ipipsec_cmd_ipsecctrl(int argc, char **argv);
#endif /* INCLUDE_IPSECCTRL_CMD */

#ifdef INCLUDE_KEYADM_CMD
IP_PUBLIC int ipipsec_cmd_keyadm(int argc, char **argv);
#endif /* INCLUDE_KEYADM_CMD */

/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipipsec_configure(void);


/*
 ****************************************************************************
 * 7                    DATA
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         SYSTEM VARIABLES
 *===========================================================================
 */

/*
 *===========================================================================
 *                         ipipsec_conf_sysvar
 *===========================================================================
 */
IP_STATIC IP_CONST Ipcom_sysvar ipipsec_conf_sysvar[] =
{
    /* Default larval SA (created by getspi) timeout in seconds. */
    { "ipipsec.larval_sa_tmo_seconds", IPIPSEC_LARVAL_SA_TMO_SECONDS },

    /* Set to 1 to enable extended sequence numbers (64-bit).
     * Set to 0 to use 32-bit, which is default for most systems today.
     */
    { "ipipsec.pfkey.esn_is_default", "0" },

    { IP_NULL, IP_NULL }
};


/*
 *===========================================================================
 *                    dynamic configuration parameters
 *===========================================================================
 * Note: not sysvars due to performance reasons!
 */
int ipipsec_conf_enable_outfilter = IPIPSEC_CONF_ENABLE_OUTFILTER;
int ipipsec_conf_enable_infilter = IPIPSEC_CONF_ENABLE_INFILTER;

/* Enable ESP, AH and IPIP. */
int ipipsec_conf_esp_enable = IPIPSEC_CONF_ESP_ENABLE;
int ipipsec_conf_ah_enable = IPIPSEC_CONF_AH_ENABLE;
int ipipsec_conf_ipip_allow = IPIPSEC_CONF_IPIP_ALLOW;   /* WARNING, IPIP IS A SECURITY RISK! */

/* Maximum size of input SA hash-cache in unique packets that use SAs. */
int ipipsec_conf_sa_cache_maxsize = IPIPSEC_CONF_SA_CACHE_MAXSIZE;

/* Maximum size of input/output flow hash-cache in unique packets. */
int ipipsec_conf_flow_cache_maxsize = IPIPSEC_CONF_FLOW_CACHE_MAXSIZE;

#ifdef IPIPSEC_USE_ESN
int ipipsec_conf_esn_resync_trigger = 1000;
#endif

#ifdef IPIPSEC_USE_TFC
/* performance reasons! */
int ipipsec_max_tfc_bytes = IPIPSEC_MAX_TFC_PADDING;
int ipipsec_max_tfc_delay = IPIPSEC_MAX_TFC_DELAY;
int ipipsec_min_tfc_delay = IPIPSEC_MIN_TFC_DELAY;
int ipipsec_max_dummy_payload = IPIPSEC_MAX_DUMMY_SIZE;
#endif

/* Number of CPUs to be reserved for IPsec in SMP. (not dynamic) */
#ifdef INCLUDE_IP_SECURITY_SMP
int ipipsec_conf_num_cpus = IPIPSEC_SMP_NUM_CPUS;
#endif


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
 *                    ipipsec_configure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:     IPCOM_SUCCESS = success.
 *
 */
IP_PUBLIC Ip_err
ipipsec_configure(void)
{
    /* Set IPIPSEC system variables. */
    (void)ipcom_sysvar_setv(ipipsec_conf_sysvar, 0);

#if !defined(IP_PORT_LKM) && !defined(IP_PORT_OSE5)
    (void)ipcom_proc_create("ipipsec_config", (Ipcom_proc_func)ipipsec_config, IPCOM_PROC_STACK_DEFAULT, IP_NULL);

#ifdef INCLUDE_IPSECCTRL_CMD
    (void)ipcom_shell_add_cmd("ipsecctrl", "ipsecctrl [command] <modifier...>", "config ipsec",
                              ipipsec_cmd_ipsecctrl,
                              IPCOM_SHELL_PRIO_SAME, 2048);
#endif /* INCLUDE_IPSECCTRL_CMD */

#ifdef INCLUDE_KEYADM_CMD
    (void)ipcom_shell_add_cmd("keyadm", "keyadm [command] <modifier...>", "admin IPsec keys",
                              ipipsec_cmd_keyadm,
                              IPCOM_SHELL_PRIO_SAME, 4096);
#endif /* INCLUDE_KEYADM_CMD */

#endif /* !defined(IP_PORT_LKM) && !defined(IP_PORT_OSE5) */

    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
