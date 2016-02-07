/*
 ******************************************************************************
 *                     INTERPEAK CONFIGURATION SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipssl_config.c,v $ $Revision: 1.2 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/ipssl_config.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright Interpeak AB 2000-2003 <www.interpeak.se>. All rights reserved.
 *     Design and implementation by Roger Boden <roger@interpeak.se>
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

#if 0
#include "/wind/components/ip_net2-6.5/ipssl2/include/ipssl.h"
#include <ipssl_config.h>
#endif


#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_shell.h>
#include <ipcom_os.h>
#include <ipcom_err.h>

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
#ifdef INCLUDE_IPSSL_USE_CMDS
    IP_GLOBAL int ipssl_ciphers(int argc, char** argv);
    IP_GLOBAL int ipssl_s_client(int argc, char** argv);
    IP_GLOBAL int ipssl_s_server(int argc, char** argv);
    IP_GLOBAL int ipssl_s_time(int argc, char** argv);
    IP_GLOBAL int ipssl_cmd_ssl_clt(int argc, char** argv);
    IP_GLOBAL int ipssl_cmd_ssl_srv(int argc, char** argv);
#endif /* IPSSL_USE_CMDS */

#ifdef INCLUDE_IPSSL_USE_TEST_CMDS
    IP_PUBLIC int ssltest_main(int argc, char** argv);
#endif

    IP_PUBLIC Ip_err ipssl_create(void);
    IP_PUBLIC Ip_err ipssl_start(void);

/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipssl_configure(void);

/*
 ****************************************************************************
 * 7                    DATA
 ****************************************************************************
 */

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
 *                    ipssl_configure
 *===========================================================================
 */
IP_PUBLIC Ip_err
ipssl_configure(void)
{

#ifdef INCLUDE_IPSSL_USE_CMDS
    ipcom_shell_add_cmd("ciphers", "ciphers", "SSL Cipher Suites", ipssl_ciphers,
                        IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_MAX);
    ipcom_shell_add_cmd("s_client", "s_client", "SSL client", ipssl_s_client,
                        IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_MAX);
    ipcom_shell_add_cmd("s_server", "s_server", "SSL server", ipssl_s_server,
                        IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_MAX);
    ipcom_shell_add_cmd("s_time", "s_time", "Time SSL connection", ipssl_s_time,
                        IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_MAX);
    ipcom_shell_add_cmd("ssl_clt", "ssl_clt", "SSL client for performance measurements ", ipssl_cmd_ssl_clt,
                        IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_MAX);
    ipcom_shell_add_cmd("ssl_srv", "ssl_srv", "SSL server for performance measurements ", ipssl_cmd_ssl_srv,
                        IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_MAX);
#endif
#ifdef INCLUDE_IPSSL_USE_TEST_CMDS
    ipcom_shell_add_cmd("ssltest", "ssltest", "ssltest", ssltest_main,
                        IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_MAX);
#endif

    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

