/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipmpls_config.c,v $ $Revision: 1.2 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/ipmpls_config.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *
 *   Design and implementation by Jonas Rendel <Jonas.Rendel@interpeak.se>
 ******************************************************************************
 */

/*
modification history
--------------------
01c,03jan07,kch  Updated CVS revision log entry for Jan 03 code drop.
01b,14dec06,kch  Reverted to the original implementation - use Ipcom_sysvar
                 instead of Ipcom_sysvar_ext and replaced MPLS_FWCONF_CMD_LIST
                 with IPMPLS_FWCONF_SYSVAR.
01a,22sep06,kch  Integrated to VxWorks build mechanism.
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

#include "ipmpls_config.h"
#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_sysvar.h>
#include <ipcom_err.h>

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

/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */
IP_PUBLIC Ip_err ipmpls_configure(void);

/*
 ****************************************************************************
 * 7                    DATA
 ****************************************************************************
 */

/*
 * ipmpls_fwdconf_sysvar - example implementation of MPLS network
 *                         pre-configuration
 * 
 * MPLS network pre-configuration array is used to statically configure
 * MPLS forwarding with an initial setup when IPMPLS is initialized. Each
 * system variable in this array must be named according to the rule 
 * ipmpls.fwconf.<index>, where <index> is a unique line indicator. The value
 * of each system variable must be composed of the IPMPLS shell command
 * mplsctl together with the flags/paramters that normally are invoked from
 * the IPCOM shell. The commands are executed in the order they are defined.
 * It is important that the "silent" flag "-s" is included  on each statement.
 *
 * To use the ipmpls_fwdconf_sysvar, defines the INCLUDE_IPMPLS_USE_EXAMPLE
 * and set the configuration macro IPMPLS_FWCONF_SYSVAR in your bsp config.h
 * (or in your VxWorks Image Project) to ipmpls_fwdconf_sysvar.
 *
 * IPMPLS_INCLUDE_CMD is defined in ipmpls_config.h. If changing that define,
 * the <ipmpls> directory must be recompiled
 *
 */
#if defined(IPMPLS_INCLUDE_CMD) && defined(INCLUDE_IPMPLS_USE_EXAMPLE)
IP_STATIC IP_CONST Ipcom_sysvar ipmpls_fwdconf_sysvar[] =
{
    {"ipmpls.fwdconf.00", "mplsctl -s -a -n 1"},
    {"ipmpls.fwdconf.01", "mplsctl -s -a -n 1 -o push,123,push,234,set,eth0,10.1.1.239"},
    {"ipmpls.fwdconf.02", "mplsctl -s -a -I mpls0"},
    {"ipmpls.fwdconf.03", "mplsctl -s -b -n 1 -I mpls0"},
    {"ipmpls.fwdconf.04", "mplsctl -s -a -i 4711 -p ipv4"},
    {"ipmpls.fwdconf.05", "mplsctl -s -a -i 4711 -o pop,setrx,mpls0,dlv"},

    /* End of configuration - DO NOT REMOVE! */
    {IP_NULL, IP_NULL}
};
#endif /* IPMPLS_INCLUDE_CMD && INCLUDE_IPMPLS_USE_EXAMPLE */

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
 *===========================================================================
 *                    ipmpls_configure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipmpls_configure(void)
{
    /* 
     * Note: IPMPLS_INCLUDE_CMD is defined in ipmpls_config.h. If
     *       changing that define, the <ipmpls> directory must be
     *       recompiled
     */
#ifdef IPMPLS_INCLUDE_CMD    
    {
        Ipcom_sysvar *entries;

        if ((entries = (Ipcom_sysvar *)IPMPLS_FWCONF_SYSVAR) != NULL)
        {
            /* initialize IPMPLS network preconfiguration */
            (void)ipcom_sysvar_setv(entries, IPCOM_SYSVAR_FLAG_OVERWRITE);
        }
    }
#endif /* IPMPLS_INCLUDE_CMD */

    return IPCOM_SUCCESS;
}

/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
