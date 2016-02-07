/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipsntp_config.c,v $ $Revision: 1.3 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/ipsntp_config.c,v $
 *   $Author: markus $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *   Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */

/*
modification history
--------------------
01f,03jan07,kch  Added precision and mcast.ttl sysvars for sntp server.
01e,01dec06,kch  Added additional configuration parameters.
01d,13sep06,kch  Refresh with Sept 08, 2006 code drop
01c,28aug06,kch  Update based on Aug 22, 2006 code drop.
01b,11aug06,kch  Updated sntp configuration parameters.
01a,03aug06,kch  Renamed INCLUDE_SNTP_CMD to INCLUDE_IPSNTP_CMD.
*/


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * SNTP configuration
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

#include "ipcom_cstyle.h"
#include "ipcom_err.h"
#include "ipcom_sysvar.h"
#include "ipcom_shell.h"
#include "ipcom_os.h"


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
#ifdef INCLUDE_IPSNTP_CMD
IP_EXTERN int ipsntp_cmd_sntp(int argc, char **argv);
#endif

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipsntp_configure(void);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         SYSVAR CONFIGURATION
 *===========================================================================
 */
IP_STATIC IP_CONST Ipcom_sysvar ipsntp_conf_sysvar[] =
{
    /* SNTP daemon mode: "client" or "server" */
#if defined(INCLUDE_IPSNTPC)
    { "ipsntp.daemon.mode", "client" },
#elif defined(INCLUDE_IPSNTPS)
    { "ipsntp.daemon.mode", "server" },
#endif

    /* SNTP server's listening port and SNTP client's destination port */
    { "ipsntp.udp.port", SNTP_LISTENING_PORT},

#ifdef INCLUDE_IPSNTPS
    /* SNTP server stratum */
    { "ipsntp.server.stratum", SNTPS_STRATUM},

    /* SNTP server precision */
    { "ipsntp.server.precision", SNTPS_PRECISION},

    /* SNTP multicast mode send interval (0 = unicast mode only) */
    { "ipsntp.server.mcast.interval", SNTPS_MULTICAST_INTERVAL},

    /* SNTP multicast mode TTL */
    { "ipsntp.server.mcast.ttl", SNTPS_MULTICAST_TTL},

#ifdef INCLUDE_IPCOM_USE_INET
    /* SNTP multicast mode IPv4 destination address */
    { "ipsntp.server.mcast.addr",  SNTPS_IPV4_MULTICAST_ADDR},
#endif
#ifdef INCLUDE_IPCOM_USE_INET6
    /* SNTP multicast mode IPv6 destination address */
    { "ipsntp.server.mcast.addr6", SNTPS_IPV6_MULTICAST_ADDR },
#endif
#endif /* INCLUDE_IPSNTPS */

#ifdef INCLUDE_IPSNTPC
    /* SNTP unicast client mode poll interval (0 = multicast mode only )*/
    { "ipsntp.client.poll.interval",  SNTPC_POLL_INTERVAL},

    /* Number of retransmissions */
    { "ipsntp.client.poll.count",  SNTPC_POLL_COUNT},

    /* Seconds between retransmissions */
    { "ipsntp.client.poll.timeout", SNTPC_POLL_TIMEOUT},

#ifdef INCLUDE_IPCOM_USE_INET
    /* Primary server IPv4 address */
    { "ipsntp.client.primary.addr",  SNTPC_PRIMARY_IPV4_ADDR},

    /* Backup server IPv4 address */
    { "ipsntp.client.backup.addr",  SNTPC_BACKUP_IPV4_ADDR},

    /* SNTP multicast client mode interface */
    { "ipsntp.client.multi.if", SNTPC_MULTICAST_MODE_IF},

    /* SNTP multicast client mode multicast group */
    { "ipsntp.client.multi.addr", SNTPC_MULTICAST_GROUP_ADDR},
#endif
#ifdef INCLUDE_IPCOM_USE_INET6
    /* Primary server IPv6 address */
    { "ipsntp.client.primary.addr6", SNTPC_PRIMARY_IPV6_ADDR },

    /* Backup server IPv6 address */
    { "ipsntp.client.backup.addr6",  SNTPC_BACKUP_IPV6_ADDR},

    /* SNTP multicast client mode IPv6 interface */
    { "ipsntp.client.multi.if6",   SNTPC_MULTICAST_MODE_IPV6_IF },

    /* SNTP multicast client mode IPv6 multicast group */
    { "ipsntp.client.multi.addr6", SNTPC_MULTICAST_GROUP_IPV6_ADDR},
#endif
#endif /* INCLUDE_IPSNTPC */

    { IP_NULL, IP_NULL }
};


/*
 ****************************************************************************
 * 9                    PUBLIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipsntp_configure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipsntp_configure(void)
{
#ifdef INCLUDE_IPSNTP_CMD
    Ip_err ret;

    ret = ipcom_shell_add_cmd("sntp", "sntp [ options | <modifier..>",
                              "(S)NTP tool",
                              (Ipcom_shell_cmd_type)ipsntp_cmd_sntp,
                              IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_SMALL);
    if(ret != IPCOM_SUCCESS)
        return ret;
#endif /* INCLUDE_IPSNTP_CMD */

	/* Initialize IPSNTP system variables. */
    return ipcom_sysvar_setv(ipsntp_conf_sysvar, 0);
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

