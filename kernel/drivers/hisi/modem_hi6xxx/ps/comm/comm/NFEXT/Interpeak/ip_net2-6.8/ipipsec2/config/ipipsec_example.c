/*
 ******************************************************************************
 *                     INTERPEAK EXAMPLE SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipipsec_example.c,v $ $Revision: 1.17 $
 *   $Source: /home/interpeak/CVSRoot/ipipsec2/config/ipipsec_example.c,v $
 *   $Author: lob $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Example file of how to configure IPIPSEC.
 *
 * The example configuration is based on the IPNET/IPLITE target having the
 * IPv4 address 10.1.2.100 and IPv6 fec0:0:0:1::64.
 * The remote target has IPv4 10.1.2.110 and IPv6 fec0:0:0:1::1.
 * NOTE: This configuration must match the one in config/ipnet_config.c or
 *       iplite_config.c!
 *
 * You MUST _minimum_ change these two IP address to match your system if
 * you want the default configuration to work on your target. That is,
 * edit the below configuration option values for '-src', '-dst', '-srcflow'
 * and '-dstflow'.
 *
 * IMPORTANT: Also please note that the encryption and authentication keys
 *            listed below MUST be changed since they are by default the same
 *            for all Interpeak IPSec releases, i.e. VERY unsecure.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipipsec_config.h"

/* What example configuration to run. */
#if defined(IPIPSEC_USE_ESP)
#define IPIPSEC_USE_ESP_EXAMPLE

#elif defined(IPIPSEC_USE_AH)
#define IPIPSEC_USE_AH_EXAMPLE
#endif

/* Third example, both AH and ESP:
#define IPIPSEC_USE_ESP_AH_EXAMPLE
*/

/* Define for tunnel mode example:
#define IPIPSEC_USE_TUNNEL_EXAMPLE
 */

/* Define for UDP encap example:
#define IPIPSEC_USE_UDPENCAP_EXAMPLE
*/


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO

#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_clib.h>
#include <ipcom_err.h>
#include <ipcom_syslog.h>
#include <ipcom_os.h>
#include <ipcom_shell.h>
#include <ipcom_strlib.h>

#include "ipipsec.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#ifdef IPIPSEC_USE_UDPENCAP_EXAMPLE
#define EXTRAFLAGS " -udpencap 4500"
#ifndef IPIPSEC_USE_TUNNEL_EXAMPLE
#define IPIPSEC_USE_TUNNEL_EXAMPLE
#endif
#else
#define EXTRAFLAGS ""
#endif

#ifdef IPIPSEC_USE_TUNNEL_EXAMPLE
#define TUNFLAG " -forcetunnel"
#else
#define TUNFLAG ""
#endif

#define IPCOM_SYSLOG_PRIORITY    IPIPSEC_SYSTEM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPIPSEC


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

IP_PUBLIC int ipipsec_cmd_keyadm(int argc, char **argv);


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IPCOM_PROCESS(ipipsec_config);

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */


/*
 *===========================================================================
 *                         ESP + AH example
 *===========================================================================
 */

#ifdef IPIPSEC_USE_ESP_AH_EXAMPLE

/* Commands. An initial '#' means a comment and is not sent to the shell. */
IP_STATIC IP_CONST char *ipipsec_init_commands[] =
{

#ifdef IPCOM_USE_INET
    "#################### output ####################",
    "keyadm add -proto ah -new -auth md5 -spi 601 -dst 10.1.2.110 -src 10.1.2.100 -akey 8317954752bf255b5321fe19e54a7f8d",

    "keyadm add -new -enc aes -auth md5 -spi 603 -dst 10.1.2.110 -src 10.1.2.100 -ekey b0e39876daf6d4ee5aaa895a4733bd8afdead83a2a8aa480 -akey 8317954752bf255b5321fe19e54a7f8d "TUNFLAG"",

    "keyadm group -proto esp -proto2 ah -dst 10.1.2.110 -dst2 10.1.2.110 -spi 603 -spi2 601",

    "keyadm addflow -spi 603 -dst 10.1.2.110 -proto esp -srcmask 255.255.255.255 -dstmask 255.255.255.255 -srcflow 10.1.2.100 -dstflow 10.1.2.110",


    "#################### input  ####################",
    "keyadm add -proto ah -new -auth md5 -spi 601 -dst 10.1.2.100 -src 10.1.2.110 -akey 8317954752bf255b5321fe19e54a7f8d -ingress",

    "keyadm add -new -enc aes -auth md5 -spi 603 -dst 10.1.2.100 -src 10.1.2.110 -ekey b0e39876daf6d4ee5aaa895a4733bd8afdead83a2a8aa480 -akey 8317954752bf255b5321fe19e54a7f8d -ingress",

    "keyadm addflow -spi 601 -dst 10.1.2.100 -proto ah -srcmask 255.255.255.255 -dstmask 255.255.255.255 -srcflow 10.1.2.110 -dstflow 10.1.2.100 -ingress",

    "keyadm addflow -spi 603 -dst 10.1.2.100 -proto esp -srcmask 255.255.255.255 -dstmask 255.255.255.255 -srcflow 10.1.2.110 -dstflow 10.1.2.100 -ingress",
#endif /* IPCOM_USE_INET */


#ifdef IPCOM_USE_INET6
    "#################### output  ####################",
    "keyadm add -proto ah -new -auth md5 -spi 701 -dst fec0:0:0:1::1 -src fec0:0:0:1::64 -akey 8317954752bf255b5321fe19e54a7f8d",

    "keyadm add -new -enc aes -auth md5 -spi 703 -dst fec0:0:0:1::1 -src fec0:0:0:1::64 -ekey b0e39876daf6d4ee5aaa895a4733bd8afdead83a2a8aa480 -akey 8317954752bf255b5321fe19e54a7f8d"TUNFLAG"",

    "keyadm group -proto esp -proto2 ah -dst fec0:0:0:1::1 -dst2 fec0:0:0:1::1 -spi 703 -spi2 701",

    "keyadm addflow -spi 703 -dst fec0:0:0:1::1 -proto esp -srcmask ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff -dstmask ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff -srcflow fec0:0:0:1::64 -dstflow fec0:0:0:1::1",


    "#################### input  ####################",
    "keyadm add -proto ah -new -auth md5 -spi 701 -src fec0:0:0:1::1 -dst fec0:0:0:1::64 -akey 8317954752bf255b5321fe19e54a7f8d -ingress",

    "keyadm add -new -proto esp -enc aes -auth md5 -spi 703 -src fec0:0:0:1::1 -dst fec0:0:0:1::64 -ekey b0e39876daf6d4ee5aaa895a4733bd8afdead83a2a8aa480 -akey 8317954752bf255b5321fe19e54a7f8d -ingress",

    "keyadm addflow -spi 703 -dst fec0:0:0:1::64 -proto esp -srcmask ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff -dstmask ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff -srcflow fec0:0:0:1::1  -dstflow fec0:0:0:1::64 -ingress",

#endif /* #ifdef IPCOM_USE_INET6 */


  "#################### end of commands ####################",
  "\n"
};

#ifdef IPIPSEC_USE_EXAMPLE
#error only_use_one_ipsec_example_at_a_time
#endif
#define IPIPSEC_USE_EXAMPLE
#endif /* IPIPSEC_USE_ESP_AH_EXAMPLE */


/*
 *===========================================================================
 *                         ESP example
 *===========================================================================
 */

#ifdef IPIPSEC_USE_ESP_EXAMPLE

/* Commands. An initial '#' means a comment and is not sent to the shell. */
IP_STATIC IP_CONST char *ipipsec_init_commands[] =
{

#ifdef IPCOM_USE_INET
    "#################### input  ####################",
    "keyadm add -new -enc aes -auth md5 -spi 603 -dst 10.1.2.100 -src 10.1.2.110 -ekey b0e39876daf6d4ee5aaa895a4733bd8afdead83a2a8aa480 -akey 8317954752bf255b5321fe19e54a7f8d -ingress"EXTRAFLAGS"",

    "keyadm addflow -spi 603 -dst 10.1.2.100 -proto esp -srcmask 255.255.255.255 -dstmask 255.255.255.255 -srcflow 10.1.2.110 -dstflow 10.1.2.100 -ingress",


    "#################### output  ####################",
    "keyadm add -new -enc aes -auth md5 -spi 603 -dst 10.1.2.110 -src 10.1.2.100 -ekey b0e39876daf6d4ee5aaa895a4733bd8afdead83a2a8aa480 -akey 8317954752bf255b5321fe19e54a7f8d"TUNFLAG""EXTRAFLAGS"",

    "keyadm addflow -spi 603 -dst 10.1.2.110 -proto esp -srcmask 255.255.255.255 -dstmask 255.255.255.255 -srcflow 10.1.2.100 -dstflow 10.1.2.110",
#endif /* IPCOM_USE_INET */


#ifdef IPCOM_USE_INET6
    "#################### output  ####################",
    "keyadm add -new -enc aes -auth md5 -spi 703 -dst fec0:0:0:1::1 -src fec0:0:0:1::64 -ekey b0e39876daf6d4ee5aaa895a4733bd8afdead83a2a8aa480 -akey 8317954752bf255b5321fe19e54a7f8d"TUNFLAG""EXTRAFLAGS"",

    "keyadm addflow -spi 703 -dst fec0:0:0:1::1 -proto esp -srcmask ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff -dstmask ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff -srcflow fec0:0:0:1::64 -dstflow fec0:0:0:1::1",


    "#################### input  ####################",
    "keyadm add -new -enc aes -auth md5 -spi 703 -src fec0:0:0:1::1 -dst fec0:0:0:1::64 -ekey b0e39876daf6d4ee5aaa895a4733bd8afdead83a2a8aa480 -akey 8317954752bf255b5321fe19e54a7f8d -ingress"EXTRAFLAGS"",

    "keyadm addflow -spi 703 -dst fec0:0:0:1::64 -proto esp -srcmask ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff -dstmask ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff -srcflow fec0:0:0:1::1 -dstflow fec0:0:0:1::64 -ingress",

#endif /* #ifdef IPCOM_USE_INET6 */


  "#################### end of commands ####################",
  "\n"
};

#ifdef IPIPSEC_USE_EXAMPLE
#error only_use_one_ipsec_example_at_a_time
#endif
#define IPIPSEC_USE_EXAMPLE
#endif /* IPIPSEC_USE_ESP_EXAMPLE */


/*
 *===========================================================================
 *                         AH example
 *===========================================================================
 */

#ifdef IPIPSEC_USE_AH_EXAMPLE

/* Commands. An initial '#' means a comment and is not sent to the shell. */
IP_STATIC IP_CONST char *ipipsec_init_commands[] =
{

#ifdef IPCOM_USE_INET
    "#################### output ####################",
    "keyadm add -proto ah -new -auth md5 -spi 601 -dst 10.1.2.110 -src 10.1.2.100 -akey 8317954752bf255b5321fe19e54a7f8d"TUNFLAG"",

    "keyadm addflow -spi 601 -dst 10.1.2.110 -proto ah -srcmask 255.255.255.255 -dstmask 255.255.255.255 -srcflow 10.1.2.100 -dstflow 10.1.2.110",


    "#################### input  ####################",
    "keyadm add -proto ah -new -auth md5 -spi 601 -dst 10.1.2.100 -src 10.1.2.110 -akey 8317954752bf255b5321fe19e54a7f8d -ingress",

    "keyadm addflow -spi 601 -dst 10.1.2.100 -proto ah -srcmask 255.255.255.255 -dstmask 255.255.255.255 -srcflow 10.1.2.110 -dstflow 10.1.2.100 -ingress",
#endif /* IPCOM_USE_INET */


#ifdef IPCOM_USE_INET6
    "#################### output  ####################",
    "keyadm add -proto ah -new -auth md5 -spi 701 -dst fec0:0:0:1::1 -src fec0:0:0:1::64 -akey 8317954752bf255b5321fe19e54a7f8d"TUNFLAG"",
    "keyadm addflow -spi 701 -dst fec0:0:0:1::1 -proto ah -srcmask ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff -dstmask ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff -srcflow fec0:0:0:1::64 -dstflow fec0:0:0:1::1 ",


    "#################### input  ####################",
    "keyadm add -proto ah -new -auth md5 -spi 701 -dst fec0:0:0:1::64 -src fec0:0:0:1::1 -akey 8317954752bf255b5321fe19e54a7f8d -ingress",

    "keyadm addflow -spi 701 -dst fec0:0:0:1::64 -proto ah -srcmask ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff -dstmask ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff -srcflow fec0:0:0:1::1 -dstflow fec0:0:0:1::64 -ingress",
#endif /* #ifdef IPCOM_USE_INET6 */


  "#################### end of commands ####################",
  "\n"
};

#ifdef IPIPSEC_USE_EXAMPLE
#error only_use_one_ipsec_example_at_a_time
#endif
#define IPIPSEC_USE_EXAMPLE
#endif /* IPIPSEC_USE_AH_EXAMPLE */


/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                    PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipipsec_config
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPIPSEC_USE_EXAMPLE
IPCOM_PROCESS(ipipsec_config)
{
    int     i;
    int     argc;
    char  **argv;
    char    buf[256];
    Ip_err  retval;

    ipcom_proc_init();


    /***** execute static ipsec keyadm commands. *****/
    for(i = 0; *ipipsec_init_commands[i] != '\n'; i++)
        if(*ipipsec_init_commands[i] != '#')
        {
            /* Copy command to a non-const buffer and add the silent flag! */
            ipcom_snprintf(buf, sizeof(buf), "%s %s",
                           ipipsec_init_commands[i],
                           "-silent");
            argv = IP_NULL;
            retval = ipcom_parse_argstr(buf, (int *)&argc, &argv);
            if(retval != IPCOM_SUCCESS)
            {
                IPCOM_LOG2(ERR, "ipipsec_config() :: ipcom_parse_argstr(%s) failed, err = %ld",
                           ipipsec_init_commands[i], retval);
                IP_PANIC();
                goto terminate;
            }

            if(ipipsec_cmd_keyadm(argc, argv) != 0)
            {
                IPCOM_LOG3(ERR, "ipipsec_config() :: '> %s' failed: %s (%d)",
                           ipipsec_init_commands[i],
                           ipcom_strerror(ipcom_errno), ipcom_errno);
            }

            ipcom_free(argv);
        }


    /***** cleanup then exit *****/
 terminate:
    ipcom_proc_exit();
}
#endif /* IPIPSEC_USE_EXAMPLE */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

