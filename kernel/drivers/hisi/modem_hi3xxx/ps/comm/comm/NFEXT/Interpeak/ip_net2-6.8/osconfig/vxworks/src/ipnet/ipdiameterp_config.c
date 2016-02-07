/* ipdiameterp_config.c - Diameter proxy agent configellete file */

/*
 * Copyright (c) 2006-2007 Wind River Systems, Inc.
 *
 * The right to copy, distribute or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement. No license to Wind River intellectual property rights
 * is granted herein. All rights not licensed by Wind River are reserved
 * by Wind River.
 */


/*
modification history
--------------------
01c,11dec07,tlu  Port to Linux
01b,13nov07,tlu  Add timer config parameters
01a,08nov07,tlu  created.
*/

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
#if 0
#include "ipdiameter_config.h"
#endif

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipcom_cstyle.h"
#include "ipcom_err.h"
#include "ipcom_sysvar.h"
#include "ipcom_shell.h"

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#ifndef DIAMETER_IDENTIFIER
#define DIAMETER_IDENTIFIER                 "apple.example.com"
#endif

#ifndef DIAMETER_HOST_REALM
#define DIAMETER_HOST_REALM                 "example.com"
#endif

#ifndef DIAMETER_HOST_DOMAIN
#define DIAMETER_HOST_DOMAIN                "2"
#endif

#ifndef DIAMETER_HOST_IPV4_ADDRESS
#define DIAMETER_HOST_IPV4_ADDRESS          "10.1.2.1"
#endif

#ifndef DIAMETER_HOST_IPV6_ADDRESS
#define DIAMETER_HOST_IPV6_ADDRESS          "3ffe:300::5"
#endif

#ifndef DIAMETER_HOST_TCP_PORT
#define DIAMETER_HOST_TCP_PORT              "3868"
#endif

#ifndef DIAMETER_APPLICATION_LIST
#define DIAMETER_APPLICATION_LIST           "2"
#endif

#ifndef DIAMETER_VENDOR_ID
#define DIAMETER_VENDOR_ID                  "0"
#endif

#ifndef DIAMETER_PEER_RECONN_INTERVAL
#define DIAMETER_PEER_RECONN_INTERVAL       "30"
#endif

#ifndef DIAMETER_PEER_MAX_RECONN_ATTEMPT
#define DIAMETER_PEER_MAX_RECONN_ATTEMPT    "30"
#endif

#ifndef DIAMETER_WATCHDOG_TIMEOUT
#define DIAMETER_WATCHDOG_TIMEOUT           "30"
#endif

#ifndef DIAMETER_REQ_RETX_INTERVAL
#define DIAMETER_REQ_RETX_INTERVAL          "10"
#endif

#ifndef DIAMETER_REQ_MAX_RETX
#define DIAMETER_REQ_MAX_RETX               "3"
#endif

#ifndef DIAMETER_TLS_ENABLED
#define DIAMETER_TLS_ENABLED                "0"
#endif

#ifndef DIAMETER_TLS_CERTIFICATE
#define DIAMETER_TLS_CERTIFICATE            "certfile.pem"
#endif

#ifndef DIAMETER_TLS_KEY_FILE
#define DIAMETER_TLS_KEY_FILE               "keyfile.pem"
#endif

#ifndef DIAMETER_TLS_KEY_PASSWORD
#define DIAMETER_TLS_KEY_PASSWORD           "clientkey"
#endif

#ifndef DIAMETER_TLS_ROOT_CERTIFICATE
#define DIAMETER_TLS_ROOT_CERTIFICATE       "cacertfile.pem"
#endif

#ifndef DIAMETER_TLS_ROOT_DIR
#define DIAMETER_TLS_ROOT_DIR               "/romfs"
#endif


#if !defined(WRS_IPNET) && !defined(PRJ_BUILD)
#define INCLUDE_IPDIAMETERP_CMD
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

#ifdef INCLUDE_IPDIAMETERP_CMD
IP_PUBLIC int ipdiameter_cmd_diameterp(int argc, char **argv);
#endif /* INCLUDE_IPDIAMETERP_CMD */

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipdiameterp_configure(void);


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

IP_STATIC IP_CONST Ipcom_sysvar ipdiameter_conf_sysvar[] =
{
    /* Diameter Identifier: node's FQDN. e.g. apple.example.com */
    { "ipdiameter.identifier", DIAMETER_IDENTIFIER },

    /* Diameter realm, e.g. example.com */
    { "ipdiameter.realm", DIAMETER_HOST_REALM },

    /* Diameter local node's address family, e.g. IP_AF_INET */
    { "ipdiameter.domain", DIAMETER_HOST_DOMAIN },

    /* Diameter local node's IPv4 address, e.g. "10.1.2.1" */
    { "ipdiameter.ipv4_address", DIAMETER_HOST_IPV4_ADDRESS },

    /* Diameter local node's IPv6 address, e.g. "3ffe:300::5" */
    { "ipdiameter.ipv6_address", DIAMETER_HOST_IPV6_ADDRESS },

    /* Diameter local node's TCP port number, default "3868" */
    { "ipdiameter.tcp_port", DIAMETER_HOST_TCP_PORT },

    /* Diameter supported application IDs */
    { "ipdiameter.appl_list", DIAMETER_APPLICATION_LIST },

    /* Diameter vendor ID */
    { "ipdiameter.vendorid", DIAMETER_VENDOR_ID },

    /* Diameter peer reconnect interval in seconds */
    { "ipdiameter.timeout.reconninterval", DIAMETER_PEER_RECONN_INTERVAL },

    /* Diameter max peer reconnection attempts */
    { "ipdiameter.maxreconnattempt", DIAMETER_PEER_MAX_RECONN_ATTEMPT },

    /* Diameter watchdog timeout in seconds */
    { "ipdiameter.timeout.watchdog", DIAMETER_WATCHDOG_TIMEOUT },

    /* Diameter request retransmission interval in seconds */
    { "ipdiameter.timeout.reqretxinterval", DIAMETER_REQ_RETX_INTERVAL },

    /* Diameter max request retransmission */
    { "ipdiameter.maxreqretx", DIAMETER_REQ_MAX_RETX },

    /* Enable TLS flag on Diameter local node */
    { "ipdiameter.tls", DIAMETER_TLS_ENABLED },

    /* Diameter local node's TLS certificate file */
    { "ipdiameter.tls.certfile", DIAMETER_TLS_CERTIFICATE },

    /* Diameter local node's TLS private key file */
    { "ipdiameter.tls.keyfile", DIAMETER_TLS_KEY_FILE },

    /* Diameter local node's TLS key file's key password */
    { "ipdiameter.tls.keypasswd", DIAMETER_TLS_KEY_PASSWORD },

    /* Diameter TLS CA root certificate file */
    { "ipdiameter.tls.cacertfile", DIAMETER_TLS_ROOT_CERTIFICATE },

    /* Diameter TLS certificate root directory */
    { "ipdiameter.tls.rootdir", DIAMETER_TLS_ROOT_DIR },

    { IP_NULL, IP_NULL }
};


/*
 ****************************************************************************
 *                     PUBLIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipdiameterp_configure
 *===========================================================================
 * Description: Diameter proxy agent configuration function 
 * Parameters: none
 * Returns: IPCOM_SUCCESS
 *
 */
IP_PUBLIC Ip_err
ipdiameterp_configure(void)
{
    /* Initialize IPDIAMETER system variables. */
    (void)ipcom_sysvar_setv(ipdiameter_conf_sysvar, 0);

#ifdef INCLUDE_IPDIAMETERP_CMD    
    /* Add the diameter proxy agent command */
    (void)ipcom_shell_add_cmd("diameterp", "diameterp [command] <modifier...>",
                              "Diameterp", ipdiameter_cmd_diameterp,
                               IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_DEFAULT);
#endif /* INCLUDE_IPDIAMETERP_CMD */

    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

