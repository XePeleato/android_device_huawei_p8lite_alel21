/*
modification history
--------------------
01a,16jun10,spw  Written.
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

#include "ipcom_config.h"
#include "ipcom_sysvar.h"

/*
 ****************************************************************************
 * 3                    TYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 4                    GENERAL
 ****************************************************************************
 */
/*
 *===========================================================================
 *                      wrnad_conf_sysvar
 *===========================================================================
 * Description: Initiates mandatory system configuration variables specific
 *              for WRNAD.
 *
 */ 

IP_STATIC IP_CONST Ipcom_sysvar wrnad_conf_sysvar[] =
{
    /*
     * nae_bus
     *
     * MIPC bus to use for wrnad communication with the NAEs.
     * 
     * Default: 0 
     */ 

    { "wrnad.mipc.nae_bus",  WRNAD_MIPC_NAE_BUS},

    /*
     * nae_node
     *
     * MIPC node number of the NAE to connect to. 
     *
     * Default: 1 
     */

    { "wrnad.mipc.nae_node",  WRNAD_MIPC_NAE_NODE},

    /*
     * nae_port
     *
     * MIPC port used by the NAE for WRNAD communication.
     *
     * Default: 3 
     */

    { "wrnad.mipc.nae_port",  WRNAD_MIPC_NAE_PORT},

    { IP_NULL, IP_NULL}
};

/*
 ****************************************************************************
 * 5                    PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*******************************************************************************
*
* wrnad_configure - configure NAD
*
* This routine is used to configure the NAD with IPCOM
*
* RETURNS: 0
*
* ERRNO: N/A
*/

IP_PUBLIC Ip_err
wrnad_configure(void)
{
    (void)ipcom_sysvar_setv(wrnad_conf_sysvar, 0);

    return 0;
}

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
