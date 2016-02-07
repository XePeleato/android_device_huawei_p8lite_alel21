/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_send_config.c,v $ $Revision: 1.2 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/ipnet_send_config.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Roger Boden
 ******************************************************************************
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

#include "ipnet_config.h"

#if defined(IPNET_USE_RFC3971) || defined(INCLUDE_IPSEND)

#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_sock.h>
#include <ipcom_sock6.h>
#include <ipcom_shell.h>
#include <ipcom_sysvar.h>

#include <openssl/x509.h>

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
IP_EXTERN int ipnet_cmd_cga(int argc, char **argv);

/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */
IP_PUBLIC Ip_err ipnet_send_configure(void);

IP_PUBLIC Ip_err
ipnet_send_cert_chain_verify_cb(X509               *ca_cert,
                                STACK_OF(X509)     *cert_chain,
                                X509               *router_cert,
                                struct Ip_in6_addr *prefix);


/*
 ****************************************************************************
 * 7                    DATA
 ****************************************************************************
 */

IP_STATIC IP_CONST Ipcom_sysvar ipnet_send_conf_sysvar[] =
{
    /* The path to the RSA key used to sign secure NDP messages */
    IP_SYSVAR("ipnet.inet6.send.rsa_key_file", "/ram/rsa.pem", IPNET_SEND_KEY_FILE),

    /* The path to X509 certificate (PEM format) file for the RSA key above */
    IP_SYSVAR("ipnet.inet6.send.cert_file", "/ram/cert.pem", IPNET_SEND_CERT_FILE),

    /* The path to the file holding the trusted CA certificate in PEM format.
     * The order of the certificates in the file is not relevant. */
    IP_SYSVAR("ipnet.inet6.send.ca_cert_file", "/ram/ca_cert.pem", IPNET_SEND_CA_FILE),

    /* The path to the file used for intermediate CAs in PEM format, used to
     * create a certificate chain. */
    IP_SYSVAR("ipnet.inet6.send.intermediate_ca_cert_file", "/ram/ca.pem", IPNET_SEND_INTERMEDIATE_CA_FILE),

    /* The allowed difference between local time and the timestamp in received
     * signed NDP messages */
    IP_SYSVAR("ipnet.inet6.send.timestamp_delta", "300", IPNET_SEND_TIMESTAMP_DELTA),

    /* on         Outgoing NDP messages are signed and incoming are required
     *            to have a valid signature
     * off        Secure NDP disabled.
     * transition Outgoing NDP messages are signed and signed incoming NDP messages
     *            are validated. Unsigned incoming NDP messages are allowed. */
    IP_SYSVAR("ipnet.inet6.send.mode", "on", IPNET_SEND_MODE),

    /*  off     routers sending router advertisments are not verified
     *  host,   a certification path solication message will be sent when
     *          a router advertisement is received
     *  router, will send certification path advertisments in response to
     *          cert path solicitations */
    IP_SYSVAR("ipnet.inet6.send.cert_path_mode", "off", IPNET_SEND_CERT_PATH_MODE),

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
 *                    ipnet_send_configure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipnet_send_configure(void)
{
    /* Set Secure NDP system variables. */
    (void)ipcom_sysvar_setv(ipnet_send_conf_sysvar, 0);

#if !defined(IP_PORT_VXWORKS) || defined(INCLUDE_CGA_CMD)
    Ip_err err;
    err = ipcom_shell_add_cmd("cga", "cga [ options ]",
                              "Cryptografically generated addresses utility", ipnet_cmd_cga,
                              IPCOM_SHELL_PRIO_SAME, IPCOM_PROC_STACK_MAX);
    if (err < 0) return err;
#endif

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipnet_send_cert_chain_verify_cb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipnet_send_cert_chain_verify_cb(X509               *ca_cert,
                                STACK_OF(X509)     *cert_chain,
                                X509               *router_cert,
                                struct Ip_in6_addr *prefix)
{
    (void)ca_cert;
    (void)cert_chain;
    (void)router_cert;
    (void)prefix;
    return IPCOM_SUCCESS;
}

#else
int ipnet_send_config_empty_file;
#endif /* IPNET_USE_RFC3971 */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

