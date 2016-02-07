/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipdhcpc_config.h,v $ $Revision: 1.17 $
 *   $Source: /home/interpeak/CVSRoot/ipappl/config/ipdhcpc_config.h,v $
 *   $Author: swebster $ $Date: 2008-10-10 19:16:03 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *
 ******************************************************************************
 */
#ifndef IPDHCPC_CONFIG_H
#define IPDHCPC_CONFIG_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif

/*
 *===========================================================================
 *                         DHCP_MAX_MESSAGE_SIZE
 *===========================================================================
 * Maximum DHCP message size the client is willing to accept.
 * Minimum valid value is 576 bytes.
 */
#define IPDHCPC_MAX_MESSAGE_SIZE   576


/*
 *===========================================================================
 *                         DHCP_MAX_USER_OPT_SIZE
 *===========================================================================
 * Maximum size for additional user DHCP options
 */
#define IPDHCPC_MAX_USER_OPT_SIZE   100


/*
 *===========================================================================
 *                     IPDHCPC_CLIENT_ID_MAXSIZE
 *===========================================================================
 * Maximum size of the DHCP client-identifier size contents (code 61).
 */
#define IPDHCPC_CLIENT_ID_MAXSIZE    32


/*
 *===========================================================================
 *                     IPDHCPC_RFC2131_INIT_DELAY
 *===========================================================================
 * RFC-compliant Congestion Avoidance Initialization Delay.
 * 1 = enabled, 0 = disabled.
 */
#define IPDHCPC_RFC2131_INIT_DELAY    1


/*
 *===========================================================================
 *                     IPDHCPC_RFC2131_EXP_BACKOFF_DELAY
 *===========================================================================
 * RFC-compliant Exponential Back-off Delay between retries.
 * 1 = enabled, 0 = disabled.
 */
#define IPDHCPC_RFC2131_EXP_BACKOFF_DELAY    1


/*
 *===========================================================================
 *                     IPDHCPC_DISCOVER_RETRIES
 *===========================================================================
 * Number of DHCP Client Attempts.
 */
#define IPDHCPC_DISCOVER_RETRIES    4


/*
 *===========================================================================
 *                     IPDHCPC_OFFER_TIMEOUT
 *===========================================================================
 * Number of milliseconds to wait for initial offer from Server. Minimum 200.
 */
#define IPDHCPC_OFFER_TIMEOUT    2000


/*
 *===========================================================================
 *                     IPDHCPC_BOOTP_MODE
 *===========================================================================
 * Operate as BOOTP client only.
 * 1 = enabled, 0 = disabled.
 */
#define IPDHCPC_BOOTP_MODE    0
	

/*
 *===========================================================================
 *                     IPDHCPC_REMOVE_EXISTING_ADDRESSES
 *===========================================================================
 * Delete all unicast addresses from the interface before negotiation begins. 
 * 1 = enabled, 0 = disabled.
 */
#define IPDHCPC_REMOVE_EXISTING_ADDRESSES    1

/*
 *===========================================================================
 *                     IPDHCPC_REMOVE_LEASED_ADDRESS
 *===========================================================================
 * Delete negotiated addresses from interface when lease expires. 
 * 1 = enabled, 0 = disabled.
 */
#define	IPDHCPC_REMOVE_LEASED_ADDRESS    1
	
/*
 *===========================================================================
 *                     IPDHCPC_AUTO_DISABLE
 *===========================================================================
 * Disable DHCP on the interface if a static address is manually applied. 
 * 1 = enabled, 0 = disabled.
 */
#define	IPDHCPC_AUTO_DISABLE    1

/*
 *===========================================================================
 *                     IPDHCPC_TTL
 *===========================================================================
 * IP Time-To-Live Default Setting.  Default is 1.  May be increased to
 * better interoperate with certain DHCP relay agents.
 */

#define IPDHCPC_TTL 1

#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
