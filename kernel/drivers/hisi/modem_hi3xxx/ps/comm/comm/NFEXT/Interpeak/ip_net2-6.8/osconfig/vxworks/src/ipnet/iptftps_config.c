/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: iptftps_config.c,v $ $Revision: 1.4 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/iptftps_config.c,v $
 *   $Author: gzadoyan $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 ******************************************************************************
 */

/*
modification history
--------------------
01d,03jan07,kch  Updated CVS revision log entry for Jan 03 code drop.
01c,01dec06,kch  Updated CVS revision log entry.
01b,13sep06,kch  Refresh with Sept 08, 2006 code drop
01a,28aug06,kch  Update based on Aug 22, 2006 code drop.
*/


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * TFTP server configuration
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


/*
 ****************************************************************************
 * 3                    DEFINES
 ****************************************************************************
 */

#define  TFTPS_PORT "69   " /* TFTP server listening port */
#define  TFTPC_PEER_PORT "69   " /* TFTP port used by client to send packets */

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

IP_PUBLIC Ip_err iptftps_configure(void);


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
 *                         iptftp_conf_sysvar
 *===========================================================================
 * TFTP Configuration Variables. Both client and server.
 */
IP_CONST Ipcom_sysvar iptftp_conf_sysvar[] =
{
	/* TFTP server working directory */
	{ "iptftp.dir",   TFTPS_DIRS },

    /* TFTP retransmit timeout in seconds */
    { "iptftp.timeout", TFTPS_RETRANSMIT_TIMEOUT},

    /* TFTP number of retries */
    { "iptftp.retries", TFTPS_RETRIES},

    /* TFTP server listening port */
    { "iptftp.srvPort", TFTPS_PORT},
	
    /* TFTP port used by client to send packets */
    { "iptftp.clntSrvPort", TFTPC_PEER_PORT},

    { IP_NULL, IP_NULL }
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
 *                    iptftps_configure
 *===========================================================================
 * Description: Configure TFTP server
 * Parameters:  none.
 * Returns:     nothing.
 */
IP_PUBLIC Ip_err
iptftps_configure(void)
{
    (void)ipcom_sysvar_setv(iptftp_conf_sysvar, IPCOM_SYSVAR_FLAG_OVERWRITE);
    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
