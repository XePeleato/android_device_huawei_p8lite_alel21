/* ipcom_ipd.h - library for ipd and the IPCOM daemon framework */

/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_ipd.h,v $ $Revision: 1.58.24.1 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_ipd.h,v $
 *   $Author: dbush $ $Date: 2010-07-22 20:42:11 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */

#ifndef IPCOM_IPD_H
#define IPCOM_IPD_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
DESCRIPTION
This library contains the ipd and the IPCOM daemon framework.
*/

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_type.h>
#include <ipcom_cstyle.h>


#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Product IDs
 *===========================================================================
 */
#define IPCOM_IPD_ID_IPLITE            0
#define IPCOM_IPD_ID_IPNET             1
#define IPCOM_IPD_ID_IPTELNETS         2
#define IPCOM_IPD_ID_IPCRYPTO          3
#define IPCOM_IPD_ID_IPRADIUS          4
#define IPCOM_IPD_ID_IPPPP             5
#define IPCOM_IPD_ID_IPMIPMN           6
#define IPCOM_IPD_ID_IPL2TP            7
#define IPCOM_IPD_ID_IPNAT             8
#define IPCOM_IPD_ID_IPIPSEC           9
#define IPCOM_IPD_ID_IPIKE            10
#define IPCOM_IPD_ID_IPSSL            11
#define IPCOM_IPD_ID_IPSSLPROXY       12
#define IPCOM_IPD_ID_IPLDAPC          13
#define IPCOM_IPD_ID_IPSSH            14
#define IPCOM_IPD_ID_IPFIREWALL       15
#define IPCOM_IPD_ID_IPDHCPS          16
#define IPCOM_IPD_ID_IPDHCPS6         17
#define IPCOM_IPD_ID_IPDHCPR          18
#define IPCOM_IPD_ID_IPHIFN           19
#define IPCOM_IPD_ID_IPWEBS           20
#define IPCOM_IPD_ID_IPRIP            21
#define IPCOM_IPD_ID_IPSNTP           22
#define IPCOM_IPD_ID_IPSNMP           23
#define IPCOM_IPD_ID_ZEBOS            24
#define IPCOM_IPD_ID_IPTCP            25
#define IPCOM_IPD_ID_IPDC             26
#define IPCOM_IPD_ID_IPMLDS           27
#define IPCOM_IPD_ID_IPTFTPS          28
#define IPCOM_IPD_ID_IPFTPS           29
#define IPCOM_IPD_ID_IPPPPOE          30
#define IPCOM_IPD_ID_IPEMANATE        31
#define IPCOM_IPD_ID_IPNET_RADVD      32
#define IPCOM_IPD_ID_IPNGC            33
#define IPCOM_IPD_ID_IPMIPHA          34
#define IPCOM_IPD_ID_IPMIPFA          35
#define IPCOM_IPD_ID_IPDHCPC          36
#define IPCOM_IPD_ID_IPDNSC           37
#define IPCOM_IPD_ID_IPTFTPC          38
#define IPCOM_IPD_ID_IPFTPC           39
#define IPCOM_IPD_ID_IPFREESCALE      40
#define IPCOM_IPD_ID_IPMPLS           41
#define IPCOM_IPD_ID_IPMCP            42
#define IPCOM_IPD_ID_IPPRISM          43
#define IPCOM_IPD_ID_IPNET_VRRPD      44
#define IPCOM_IPD_ID_IPEAP            45
#define IPCOM_IPD_ID_IP8021X          46
#define IPCOM_IPD_ID_IPSAFENET        47
#define IPCOM_IPD_ID_IPMIP6MN         48
#define IPCOM_IPD_ID_IPHWCRYPTO       49
#define IPCOM_IPD_ID_IPNETPERF        50
#define IPCOM_IPD_ID_IPNETSNMP        51
#define IPCOM_IPD_ID_IPMARVELL        52
#define IPCOM_IPD_ID_IPQUAGGA         53
#define IPCOM_IPD_ID_IPRIPNG          54
#define IPCOM_IPD_ID_IPDHCPC6         55
#define IPCOM_IPD_ID_IPCCI            56
#define IPCOM_IPD_ID_IPANVL           57
#define IPCOM_IPD_ID_IPDIAMETERC      58
#define IPCOM_IPD_ID_IPWLAN           59
#define IPCOM_IPD_ID_IPMIPPM          60
#define IPCOM_IPD_ID_IPDIAMETERP      61
#define IPCOM_IPD_ID_IPNET_SEND       62
#define IPCOM_IPD_ID_IPMIP6HA         63
#define IPCOM_IPD_ID_IPMIP6CN         64
#define IPCOM_IPD_ID_IPMIP6PM         65
#define IPCOM_IPD_ID_IPROHC           66
#define IPCOM_IPD_ID_IPCAVIUM         67
#define IPCOM_IPD_ID_IPSCTP           68
#define IPCOM_IPD_ID_IPIFPROXY        69
#define IPCOM_IPD_ID_IPWPS            70
#define IPCOM_IPD_ID_WRNAD            71
#define IPCOM_IPD_ID_MAX              72  /* Keep updated! */


/*
 *===========================================================================
 *                         UDP port base
 *===========================================================================
 */
#ifndef IPCOM_IPD_PORT_BASE
 #define IPCOM_IPD_PORT_BASE          20000
#endif

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipcom_ipd_product
 *===========================================================================
 */

#ifndef IP_PORT_LAS
typedef Ip_err (*Ipcom_ipd_function)(void);
typedef const char * (*Ipcom_ipd_vfunction)(void);

typedef struct Ipcom_ipd_product_struct
{
    const char *         name;
    Ipcom_ipd_function   create;
    Ipcom_ipd_function   configure;
    Ipcom_ipd_function   start;
    Ipcom_ipd_vfunction  version;
    int                  id;
    int                  syslog_id;
    int                  flags;
}
Ipcom_ipd_product;

#else

typedef Ip_err (*Ipcom_ipd_function)(const char *name, const char *binary);

typedef struct Ipcom_ipd_product_struct
{
    const char          *name;
    const char          *binary;
    Ipcom_ipd_function  start;
    int                 id;
    int                 flags;
}
Ipcom_ipd_product;

#endif


#define IPCOM_IPD_FLAG_IPD_START   0x1 /* Application supports IPD message API */
#define IPCOM_IPD_FLAG_HALTED      0x2 /* Do not call start() hook at boot. */

IP_EXTERN const Ipcom_ipd_product ipcom_ipd_products[];


/*
 *===========================================================================
 *                         Ipcom_ipd_msg
 *===========================================================================
 */
typedef struct Ipcom_ipd_msg_struct
{
    int      msgtype;
    Ip_err   msgerr;
}
Ipcom_ipd_msg;

/* msgtype values: */
#define IPCOM_IPD_MSGTYPE_START         0
#define IPCOM_IPD_MSGTYPE_RECONFIGURE   1
#define IPCOM_IPD_MSGTYPE_KILL          2
#define IPCOM_IPD_MSGTYPE_PING          3


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_ipd_init
 *===========================================================================
 * Description: Called by application to init IPD.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err ipcom_ipd_init(const char *name, Ip_err err, Ip_fd *fd);


/*
 *===========================================================================
 *                    ipcom_ipd_exit
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err ipcom_ipd_exit(const char *name, Ip_fd fd);


/*
 *===========================================================================
 *                    ipcom_ipd_input_process
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int ipcom_ipd_input_process(Ip_fd fd, Ipcom_ipd_msg *msg);


/*
 *===========================================================================
 *                    ipcom_ipd_input
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:     IPCOM_IPD_EVENT_xxx event.
 *
 */
IP_PUBLIC int ipcom_ipd_input(Ip_fd fd);

/*
 *===========================================================================
 *                    ipcom_ipd_read
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:      Event
 *
 */
IP_PUBLIC int ipcom_ipd_read(Ip_fd fd, char *buf, Ip_size_t buflen, int *event);


#define IPCOM_IPD_EVENT_NONE           0
#define IPCOM_IPD_EVENT_RECONFIGURE    1
#define IPCOM_IPD_EVENT_KILL           2


/*******************************************************************************
*
* ipcom_ipd_start - starts a daemon
*
* DESCRIPTION
* Start a daemon
*
* Parameter:
* \is
* \i <name>
* Name of the daemon to start.
* \ie
*
* RETURNS:
* IPCOM_SUCCESS if the daemon was successfully started, or one of the following errors:
* \is
* \i 'IPCOM_ERR_ALREADY_STARTED' if the daemon is already running.
* \i 'IPCOM_ERR_NOT_FOUND'
* Daemon name is unknown.
* \i 'IPCOM_ERR_FAILED'
* Daemon failed to start.
* \i 'IPCOM_ERR_NOT_SUPPORTED'
* 'name' does not refer to a daemon.
* \ie
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipcom_ipd_start(
				 const char *name
				 );


/*******************************************************************************
*
* ipcom_ipd_kill - stops a daemon
*
* DESCRIPTION
* Stop a daemon
*
* Parameter:
* \is
* \i <name>
* Name of the daemon to stop.
* \ie
*
* RETURNS:
* IPCOM_SUCCESS if the daemon was successfully stopped', or one of the following errors:
* \is
* \i 'IPCOM_ERR_NOT_FOUND'
* Daemon name is unknown.
* \i 'IPCOM_ERR_FAILED'
* Daemon failed to stop.
* \i 'IPCOM_ERR_NOT_SUPPORTED'
* 'name' does not refer to a daemon.
* \i 'IPCOM_ERR_NOT_STARTED'
* Daemon is not running.
* \ie
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipcom_ipd_kill(
				const char *name
				);


/*******************************************************************************
*
* ipcom_ipd_reconfigure - sends a reconfigure signal to a daemon
*
* DESCRIPTION
* Signals to a daemon that it shall reread its configuration
*
* Parameter:
* \is
* \i <name>
* Name of the daemon to send the reconfigure signal to.
* \ie
*
* RETURNS:
* 'IPCOM_SUCCESS' if the daemon was successfully sent the signal, or one of the following errors:
* \is
* \i 'IPCOM_ERR_NOT FOUND'
* Daemon name is unknown.
* \i 'IPCOM_ERR_FAILED'
* Signal could not be sent to the daemon.
* \i 'IPCOM_ERR_NOT_SUPPORTED'
* 'name' does not refer to a daemon.
* \i 'IPCOM_ERR_NOT_STARTED'
* Daemon is not running.
* \ie
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipcom_ipd_reconfigure(
				       const char *name
				       );


/*
 *===========================================================================
 *                    ipcom_ipd_send
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err ipcom_ipd_send(const char *name, int msgtype);


/*
 *===========================================================================
 *                    ipcom_ipd_sendmsg
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err ipcom_ipd_sendmsg(const char *name, void *msgbuf, Ip_size_t msglen);


/*
 *===========================================================================
 *                    ipcom_ipd_datamsg
 *===========================================================================
 * Description: Similar to ipcom_ipd_sendmsg, but returns a generic message
 *              as opposed to a message ack.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err ipcom_ipd_datamsg(const char *name, void *outbuf, Ip_size_t outlen,
                                   void *inbuf, Ip_size_t inlen);
#ifdef __cplusplus
}
#endif

#endif /* IPCOM_IPD_H */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
