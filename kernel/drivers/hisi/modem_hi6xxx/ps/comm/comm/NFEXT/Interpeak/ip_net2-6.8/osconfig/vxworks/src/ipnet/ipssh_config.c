/*
 ******************************************************************************
 *                     IPSSH CONFIGURATION FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipssh_config.c,v $ $Revision: 1.7.20.1 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/ipssh_config.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2006 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *   Design and implementation by Roger Boden <roger@interpeak.se>
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

#ifdef IPCOM_USE_CLIB_PROTO
#undef IPCOM_USE_CLIB_PROTO
#endif

#include "ipcom_type.h"
#include <ipcom_cstyle.h>
#include "ipcom_clib.h"
#include "ipcom_err.h"
#include "ipcom_file.h"
#include "ipcom_syslog.h"
#include <ipcom_file.h>
#include <ipcom_shell.h>
#include <ipcom_os.h>
#include <ipcom_buffer.h>
#ifdef IPCOM_USE_AUTH
#include <ipcom_auth.h>
#endif

#include "ipssh_config.h"

#include <openssl/bio.h>
#include <openssl/dsa.h>
#ifdef IPSSH_USE_RSA
#include <openssl/rsa.h>
#endif
#include <openssl/pem.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#if 0
#include <ipssh.h>
#endif

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

IP_GLOBAL int ipssh_stop_cmd(int argc, char **argv);
IP_GLOBAL int ipssh_cmd_sftp(int argc, char **argv);
IP_GLOBAL int ipssh_list_cmd(int argc, char **argv);
IP_GLOBAL int ipssh_keyfp_cmd(int argc, char **argv);

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

IP_CONST Ipcom_sysvar ipssh_conf_sysvar[] =
{
    { "ipssh.port", SSH_PORT },
    { "ipssh.idle_timeout", SSH_IDLE_TIMEOUT },
    { "ipssh.max_clients", SSH_MAX_CLIENTS },
    { "ipssh.max_channels", SSH_MAX_SHELLS },
#ifdef IPSSH_USE_AES
    { "ipssh.enc.aes", SSH_USE_AES },
#endif
#ifdef IPSSH_USE_DES
    { "ipssh.enc.3des", SSH_USE_3DES },
    { "ipssh.enc.des", SSH_USE_DES },
#endif
#ifdef IPSSH_USE_BLOWFISH
    { "ipssh.enc.blowfish", SSH_USE_BLOWFISH },
#endif
#ifdef IPSSH_USE_CAST
    { "ipssh.enc.cast", SSH_USE_CAST},
#endif
#ifdef IPSSH_USE_ARCFOUR
    { "ipssh.enc.arcfour",SSH_USE_ARCFOUR},
#endif
    { "ipssh.enc.none", SSH_USE_NO_CIPHER },
#ifdef IPSSH_USE_SHA1
    { "ipssh.mac.sha1", SSH_USE_SHA1 },
    { "ipssh.mac.sha1-96", SSH_USE_SHA1_96 },
#endif
#ifdef IPSSH_USE_MD5
    { "ipssh.mac.md5", SSH_USE_MD5},
    { "ipssh.mac.md5-96", SSH_USE_MD5_96 },
#endif
    { "ipssh.mac.none", SSH_USE_NO_MAC },

#ifdef IPSSH_USE_V1
    { "ipssh.v1", SSH_USE_V1 },
    { "ipssh.v2", SSH_USE_V2 },
#endif
    { "ipssh.service.terminal",SSH_USE_TERMINAL },
#ifdef IPSSH_USE_SFTP
    { "ipssh.service.sftp", SSH_USE_SFTP },
#endif
    { "ipssh.service.port_fwd", SSH_USE_PORT_FWD },
    { "ipssh.key_id.dsa", SSH_DSA_KEY_ID },
#ifdef IPSSH_USE_RSA
    { "ipssh.key_id.rsa", SSH_RSA_KEY_ID },
#endif
    { "ipssh.auth.pw.allowed", SSH_AUTH_PW_ALLOWED },
    { "ipssh.auth.pw.required", SSH_AUTH_PW_REQUIRED },
    { "ipssh.auth.pub_key.allowed", SSH_AUTH_PUB_KEY_ALLOWED },
    { "ipssh.auth.pub_key.required", SSH_AUTH_PUB_KEY_REQUIRED },
    { "ipssh.auth.pub_key_first", SSH_AUTH_PUB_KEY_FIRST },
    { "ipssh.auth.max_fail", SSH_AUTH_MAX_FAIL },
    { "ipssh.host_key.rsa", "1" },
    { "ipssh.host_key.dsa", "1" },
    { "ipssh.kex.dh_gr1_sha1", "1" },
    { "ipssh.kex.dh_gr14_sha1", "1" },
    { "ipssh.kex.dh_gex.sha1", "1" },
    { "ipssh.kex.dh_gex.sha256", "1" },
    { "ipssh.kex.dh_gex.clt.min", "1024" },
    { "ipssh.kex.dh_gex.clt.n", "1024" },
    { "ipssh.kex.dh_gex.clt.max", "4096" },
    { "ipssh.kex.dh_gex.moduli_file", IPCOM_FILE_ROOT"moduli" },
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
 *                    ipssh_configure
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipssh_configure(void)
{
    ipcom_sysvar_setv(ipssh_conf_sysvar, 0);

#ifdef IPSSH_USE_STOP_SPAWN
    (void)ipcom_shell_add_cmd("ipssh_stop",
                              "ipssh_stop [-c <id>] [-a] [-h]",
                              "Stop SSH spawns",
                              (Ipcom_shell_cmd_type) ipssh_stop_cmd,
                              IPSSH_SHELLCMD_PROC_PRIO,
                              IPCOM_PROC_STACK_DEFAULT);
#endif
    (void)ipcom_shell_add_cmd("ipssh_list",
                              "ipssh_list [-l] [-h]",
                              "List connected ssh clients",
                              (Ipcom_shell_cmd_type) ipssh_list_cmd,
                              IPSSH_SHELLCMD_PROC_PRIO,
                              IPCOM_PROC_STACK_DEFAULT);
#ifdef IPSSH_USE_SFTP_CLT_CMD
    (void)ipcom_shell_add_cmd("sftp",
                              "sftp",
                              "sftp file transfer",
                              (Ipcom_shell_cmd_type) ipssh_cmd_sftp,
                              IPSSH_SHELLCMD_PROC_PRIO,
                              IPCOM_PROC_STACK_MAX);
#endif

#ifdef IPSSH_USE_KEY_FINGERPRINT
    (void)ipcom_shell_add_cmd("keyfp",
                              "keyfp -f <file> <-b|-h>",
                              "Generate key finger print",
                              (Ipcom_shell_cmd_type) ipssh_keyfp_cmd,
                              IPSSH_SHELLCMD_PROC_PRIO,
                              IPCOM_PROC_STACK_DEFAULT);
#endif

#ifdef IPSSH_USE_DH_GROUP_EX_EXAMPLE_MODULI_FILE
    ipssh_create_dh_gex_moduli_example_file();
#endif

    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

