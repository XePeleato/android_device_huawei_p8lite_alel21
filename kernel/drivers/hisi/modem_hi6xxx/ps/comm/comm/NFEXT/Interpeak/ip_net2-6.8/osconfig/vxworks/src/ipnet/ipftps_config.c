/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipftps_config.c,v $ $Revision: 1.4 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/ipftps_config.c,v $
 *   $Author: gzadoyan $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 ******************************************************************************
 */

/*
modification history
--------------------
01g,19dec,09ggz  Added ipftps data socket linger time to sysvar.Defect WIND00127796.
01f,08dec08,ggz  Added FTPS_SOCKET_BACKLOG parameter for FTP control socket
01e,27feb07,kch  ipftps.versionstr should not be made configurable 
                 (WIND00089286).
01d,03jan07,kch  Updated CVS revision log entry for Jan 03 code drop.
01c,01dec06,kch  Updated CVS revision log entry.
01b,13sep06,kch  Refresh with Sept 08, 2006 code drop
01a,28aug06,kch  Update based on Aug 22, 2006 code drop.
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


/*
 ****************************************************************************
 * 3                    DEFINES
 ****************************************************************************
 */
#ifndef FTPS_DATA_SOCKET_LINGER
#define FTPS_DATA_SOCKET_LINGER   "1"  /* Data socket linger time */
#endif

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

IP_PUBLIC Ip_err ipftps_configure(void);


/*
 ****************************************************************************
 * 7                    DATA
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         ipftps_conf_sysvar
 *===========================================================================
 * FTP Server Configuration Variables
 */

IP_CONST Ipcom_sysvar ipftps_conf_sysvar[] =
{
    /* Defaults for new sessions */

    /* Auth attempts before disconnect */
    {"ipftps.authentications", FTPS_AUTH_ATTEMPTS},
    
    /* Time to sleep after auth fail */
    {"ipftps.authsleep",       FTPS_SLEEP_TIME},

    /* Use local ports from lportbase+1 */
    {"ipftps.lportbase",       FTPS_LOCAL_PORT_BASE},  

    /* Reject peer ports <= pportbase */
    {"ipftps.pportbase",       FTPS_PEER_PORT_BASE},   
    {"ipftps.proxy",           FTPS_ENABLE_PROXY},    /* Disable proxy FTP */
    {"ipftps.readonly",        FTPS_MODE},      /* Read/write mode */
    {"ipftps.root",            FTPS_ROOT_DIR},      /* Directory root */
    {"ipftps.dir",             FTPS_INITIAL_DIR}, /* Initial directory */

    /* General defaults */

    /* Max no of simultaneous sessions */
    {"ipftps.max_sessions",    FTPS_MAX_SESSIONS},
    {"ipftps.port_number",     FTPS_PORT_NUM},     /* Server port number */
    {"ipftps.receive_timeout", FTPS_RECV_TIMEOUT}, /* Data receive timeout */
    {"ipftps.send_timeout",    FTPS_SEND_TIMEOUT},     /* Data send timeout */
    
    /* Socket backlog queue number set by listen */
    {"ipftps.socket_backlog",    FTPS_SOCKET_BACKLOG},

    /* Data socket linger time */
    {"ipftps.dsock_linger", FTPS_DATA_SOCKET_LINGER},

    /* User inactivity timeout */
    {"ipftps.session_timeout", FTPS_INACTIVITY_TIMEOUT},
    {"ipftps.system",          FTPS_SYS_TYPE},   /* Reported system type */
    {"ipftps.versionstr",      "6.5"},  /* Reported version */

    {IP_NULL, IP_NULL}
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
 *                    ipftps_configure
 *===========================================================================
 * Description: Configure FTP server.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipftps_configure(void)
{
    (void)ipcom_sysvar_setv(ipftps_conf_sysvar, 0);
    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
