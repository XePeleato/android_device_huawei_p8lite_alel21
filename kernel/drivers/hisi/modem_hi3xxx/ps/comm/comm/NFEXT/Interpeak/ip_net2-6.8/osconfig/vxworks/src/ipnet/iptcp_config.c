/*
 ******************************************************************************
 *                     IPNET USER CONFIGURATION FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: iptcp_config.c,v $ $Revision: 1.3.34.2.2.2 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/iptcp_config.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */

/*
modification history
--------------------
01d,03jan07,kch  Updated CVS revision log entry for Jan 03 code drop.
01c,01dec06,kch  Added additional configuration parameters.
01b,13sep06,kch  Refresh with Sept 08, 2006 code drop
01a,28aug06,kch  Update based on Aug 22, 2006 code drop.
*/


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * User IPTCP configuration file.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include <ipcom_cstyle.h>
#include <ipcom_err.h>

/*
 * Definition of default values
 */
#ifndef TCP_ALLOW_ZERO_WINDOW
#define TCP_ALLOW_ZERO_WINDOW "0"
#endif

#ifndef TCP_ALLOW_TINY_WINDOW
#define TCP_ALLOW_TINY_WINDOW "1"
#endif

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#ifndef TCP_PERSIST_TMO
/* Default timeout must be 0 (infinite) according to
   "draft-ananth-tcpm-persist-02" */
#define TCP_PERSIST_TMO "0"
#endif

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

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err iptcp_configure(void);


/*
 *===========================================================================
 *                         SYSTEM VARIABLES
 *===========================================================================
 */
IP_STATIC IP_CONST Ipcom_sysvar iptcp_conf_sysvar[] =
{
    /* Define the number of seconds IPNET will try to create a TCP connection
       before giving up. Must be 300 or larger when running the Ixia ANVL
       suite, several tests will fail otherwise */
    { "iptcp.ConnectionTimeout", TCP_CONN_TIMEOUT},

    /* Maximum number of times a TCP segment will be resent before giving up
       with an IPTCP_ERRNO_ETIMEDOUT error. Exponential back off is applied before
       each resend (max time between two resends is 1 minute). */
    { "iptcp.MaxRetransmits", TCP_MAX_RETRANSMITS},

    /* Maximum segment lifetime. TCP sockets that enter the TIME_WAIT state will
       be in that state for 2 times this value.
       Should be 120 sec (2 min) according to RFC 793, but most TCP/IP stacks
       sets this value to 30 sec */
    { "iptcp.Msl", TCP_MSL},

    /* Set to 1 to disable LAND attacks */
    { "iptcp.DisableLandAttack", TCP_DISABLE_LAND_ATTACK},

    /*
     * Set to !=0 to allow zero window in ACK of a SYN/ACK.
     */
    { "iptcp.AllowZeroWindow", TCP_ALLOW_ZERO_WINDOW},

    /*
     * Set to !=0 to allow a window less than one MSS in ACK of a
     * SYN/ACK.
     */
    { "iptcp.AllowTinyWindow", TCP_ALLOW_TINY_WINDOW},

    /* Defines the largest Maximum Segment Size that TCP will ever suggest.
       The actual MSS suggested might be smaller than this number if depending on
       MTU for the interface. Set to 0 if the largest MSS allowed by the
       MTU should be used */
    { "iptcp.MaxMss", TCP_MAX_MSS},

    /* Set to 0 or not defined to make IPTCP not include the timestamp option
       in TCP segments. */
    { "iptcp.opt.Timestamp", TCP_USE_TIMESTAMP},

    /* Set to 1 to use urgent data as described in RFC 1122 chapter 4.2.2.4
       set to 0 to use urgent data as described in RFC 793
       Should be set to 0 since that is how most stacks works, must be set
       to 1 in order successfully pass Ixia ANVL TCP-CORE group 19 tests */
    { "iptcp.urg.Rfc1122", TCP_USE_RFC1122_URGENT_DATA},

    /* The size multiplier that will be used on outgoing segments when the
       outgoing interface support TCP Segmentation Offload. Higher value
       will generally give better performance, but any factor higher than
       2 is not guaranteed to work on all network cards */
    { "iptcp.tso.Factor", TCP_SEGMENT_MULTIPLIER},

    { "iptcp.PersistTimeout", TCP_PERSIST_TMO },

    { IP_NULL, IP_NULL }
};


/* The default size of the backlog (backlog == 0 when calling ipcom_listen()) */
IP_CONST int iptcp_conf_default_backlog = 5;

/*
 ****************************************************************************
 * 7                    PUBLIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    iptcp_configure
 *===========================================================================
 * Description: Configure IPNET
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
iptcp_configure(void)
{
    /* Set IPNET system variables. */
    (void)ipcom_sysvar_setv(iptcp_conf_sysvar, 0);

    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */


