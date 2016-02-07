/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipike_config.c,v $ $Revision: 1.5.20.1 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/ipike_config.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright Interpeak AB 2000-2003 <www.interpeak.se>. All rights reserved.
 *     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */

/*
modification history
--------------------
01c,May1208,brg  Removed allow_duplicate_ike_sa as it is an internal
                 configuration parameter.(WIND00108854)
01b,25sep07,aja  Fix WIND00105339: The line which sets the sysvar
                 ipike.port_nat is commented out, which means that regardless
                 of which value you specify in Workbench, NAT-T detection will
                 be disabled.
01a,06mar07,kch  Added support for dynamic address, isakmp_max_exchanges and
                 allow_duplicate_ike_sa sysvars (WIND00089895).
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

#ifdef IPCOM_USE_CLIB_PROTO
#undef IPCOM_USE_CLIB_PROTO
#endif

#include "ipike_config.h"
#include <ipcom_err.h>
#include <ipcom_sock.h>
#include <ipcom_file.h>
#include <ipcom_clib.h>

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
 *===========================================================================
 *                      ipike_configure
 *===========================================================================
 */
IP_PUBLIC Ip_err
ipike_configure(void);

/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */

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
IP_CONST Ipcom_sysvar ipike_conf_sysvar[] = {
    /* ISAKMP socket buffer size (in bytes), this is the largest ISAKMP message
       the IKE daemon can send and receive */
    { "ipike.isakmp_sock_buf_size", IPIKE_SOCK_BUF_SIZE },

    /* The maximum number of active exchanges, ISAKMP packets for new exchanges
       will be dropped when the maximum is reached. Set to 0 to allow infinite
       number of exchanges. */
    { "ipike.isakmp_max_exchanges", IPIKE_MAX_EXCHANGES},

    /* The UDP port on which to listen for ISAKMP packets.
       500 is the default IKE port */
    { "ipike.port",                 IPIKE_PORT },

    /* The UDP port on which to listen for ISAKMP packets when using NAT-Traversal.
       The only supported number is 4500 (limitation IPNet). */
    { "ipike.port_nat",             IPIKE_NAT_PORT },

    /* Number of seconds between NAT-Traversal keep-alive packets. Default
       20 seconds. */
    { "ipike.keepalive_nat",        IPIKE_NAT_KEEPALIVE },

    /* Base time to wait (in seconds) before resending a ISAKMP message,
       the time to wait is calulated on tWait = ipike.resend_delay * resend_count */
    { "ipike.resend_delay",         IPIKE_RESEND_DELAY },

    /* The filename where the configuration is stored */
    { "ipike.filename",             IPIKE_CONFIG_FILE_PATH IPIKE_CONFIG_FILE_NAME },

    /* The size (in bytes) of the created nonce (must be 8-256) */
    { "ipike.nonce_size",           IPIKE_NONCE_SIZE },

    /* Set this to 0 if IPIKE should not create a default 'ipike.filename' if
       no such already exists */
    { "ipike.create_default_conf",  IPIKE_CREATE_DEFAULT_CONFIG_FILE },

    /* Internal configuration variable. Set to 1 to allow duplicate IKE SAs */
    { "ipike.allow_duplicate_ike_sa",  0 },

    /* Set flush to 1 if IPIKE should flush IPSec keys at startup or shutdown,
       only needed when not running Interpeak IPSec */
    { "ipike.pfkeyv2.flush_startup",  IPIKE_FLUSH_STARTUP },
    { "ipike.pfkeyv2.flush_shutdown", IPIKE_FLUSH_SHUTDOWN },
    { "ipike.pfkeyv2.flush_reconfigure", IPIKE_FLUSH_RECONFIGURE },

    /* Number of seconds before the hard timeout the soft timeout (and the renewal
       negotiation) should be */
    { "ipike.pfkeyv2.soft_to_hard_time",  IPIKE_SOFT_TO_HARD_TIME },

    /* Dynamic address pool (responder), used for test to simulate VPN gateway */
    { "ipike.address_pool.network4", IPIKE_DYNAMIC_ADDRESS_POOL_NETWORK4 },
    { "ipike.address_pool.dns4", IPIKE_DYNAMIC_ADDRESS_POOL_DNS4 },
    { "ipike.address_pool.expire4", IPIKE_DYNAMIC_ADDRESS_POOL_EXPIRE4 },
    { "ipike.address_pool.anyflow", IPIKE_DYNAMIC_ADDRESS_ANYFLOW },
    { "ipike.address_pool.homeaddr", IPIKE_DYNAMIC_ADDRESS_HOMEADDR },

    /* Dynamic address mode IKE v1 (initiator):
     *     0 : Will set IPSec src flow as "any" address
     *     1 : Will set IPSec src flow as retrieved address
     *     2 : Will set IPSec src flow as retrieved network */
    { "ipike.dynamic_address.mode", IPIKE_DYNAMIC_ADDRESS_MODE },

    /* Set to '1' to enable MOBIKE (RFC4555), default '0' */
    { "ipike.mobike", "0" },

    /* Set to '1' to enable routability check for MOBIKE, default '1' */
    { "ipike.mobike.routability_check", "1" },

    /* Set to '1' to initiate rekey for IPSec SAs for passive connections. */
    { "ipike.rekey_passive", "1" },

    /* End marker */
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
 *                    ipike_configure
 *===========================================================================
 * Description: Configures IPIKE.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipike_configure(void)
{
    /* Set IPNET system variables. */
    (void)ipcom_sysvar_setv(ipike_conf_sysvar, 0);

    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
