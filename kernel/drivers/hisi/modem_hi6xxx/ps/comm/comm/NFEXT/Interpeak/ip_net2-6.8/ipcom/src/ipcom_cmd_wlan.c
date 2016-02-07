/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_cmd_wlan.c,v $ $Revision: 1.5 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_cmd_wlan.c,v $
 *     $Author: lob $ $Date: 2006-11-14 10:23:10 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Firstname Lastname <email@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1  DESCRIPTION.
 *
 * Test TCP connection.  Makes a connection on default port 5001
 * and transfers fabricated buffers or data copied from stdin.
 *
 * Machines using System V with BSD sockets should define SYSV.
 *
 * HISTORY OF DEVELOPMENT.
 *
 * Modified for operation under 4.2BSD, 18 Dec 84
 *      T.C. Slattery, USNA
 * Minor improvements, Mike Muuss and Terry Slattery, 16-Oct-85.
 * Modified in 1989 at Silicon Graphics, Inc.
 *      catch SIGPIPE to be able to print stats when receiver has died
 *      for tcp, don't look for sentinel during reads to allow small transfers
 *      increased default buffer size to 8K, nbuf to 2K to transfer 16MB
 *      moved default port to 5001, beyond IPPORT_USERRESERVED
 *      make sinkmode default because it is more popular,
 *              -s now means don't sink/source
 *      count number of read/write system calls to see effects of
 *              blocking from full socket buffers
 *      for tcp, -D option turns off buffered writes (sets TCP_NODELAY sockopt)
 *      buffer alignment options, -A and -O
 *      print stats in a format that's a bit easier to use with grep & awk
 *      for SYSV, mimic BSD routines to use most of the existing timing code
 * Modified by Steve Miller of the University of Maryland, College Park
 *      -b sets the socket buffer size (SO_SNDBUF/SO_RCVBUF)
 * Modified Sept. 1989 at Silicon Graphics, Inc.
 *      restored -s sense at request of tcs@brl
 * Modified Oct. 1991 at Silicon Graphics, Inc.
 *      use getopt(3) for option processing, add -f and -T options.
 *      SGI IRIX 3.3 and 4.0 releases don't need #define SYSV.
 * Ported to OSE march 1997 at Enea OSE Systems AB by Lennart Bang.
 *      OSE portation (compile with -DOSE_DELTA or -DOSE_CLASSIC), ANSI prototypes,
 *      removal of static initialized data and general code cleanup.
 * Rewritten (being careful not to affect test loop performance) to use Interpeak
 *      IPCOM APIs. Added IPv6 support. October 2001, Lennart Bång.
 *
 * Distribution Status -
 *      Public Domain.  Distribution Unlimited.
 */


/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"

/*
 ****************************************************************************
 * 3  INCLUDE FILES.
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include <stdarg.h>
#include "ipcom_clib.h"
#include "ipcom_cstyle.h"
#include "ipcom_getopt.h"
#include "ipcom_os.h"
#include "ipcom_sock.h"
#include "ipcom_sock2.h"
#include "ipcom_time.h"
#include "ipcom_type.h"
#include "ipcom_wlan.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define MIN(a, b) (((a)<(b))?(a):(b))

#define WLAN_MAX_TEXT_LEN 100
#define WLAN_FREQUENCY_BAND 2412000


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

enum { wlan_mode_client_infra = 0, wlan_mode_client_adhoc, wlan_mode_client_auto, wlan_mode_ap, wlan_mode_unknown };


/*
 ****************************************************************************
 * 6                    EXTERNAL FUNCTIONS
 ****************************************************************************
 */

IP_EXTERN int ipcom_vprintf(const char *format, va_list ap);
IP_EXTERN int vsnprintf(char *, int, const char *, va_list);


/*
 ****************************************************************************
 * 6                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC int ipwlan_cmd_wlan(int argc, char **argv);


/*
 ****************************************************************************
 * 7                    DATA
 ****************************************************************************
 */


/*
 *===========================================================================
 *                         STATIC DATA
 *===========================================================================
 */

IP_STATIC IP_CONST char *wlan_mode_text[] = {"client infra", "client adhoc", "client auto", "access point", "adhoc", "unknown"};
IP_CONST int ipwlan_encr_policy[] = {IP_IW_ENCR_ALG_NONE, IP_IW_ENCR_ALG_WEP, IP_IW_ENCR_ALG_TKIP, IP_IW_ENCR_ALG_CCMP};
IP_STATIC IP_CONST char *wlan_encr_text[] = {"none", "WEP", "TKIP", "AES_CCMP", "<unknown>"};

/*
 ****************************************************************************
 * 8                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                      ipwlan_printf
 *===========================================================================
 * Description: Prints to console
 * Parameters:  'enabled' == 0 => printout is suppressed
 * Returns:     0
 */
IP_STATIC int
ipwlan_printf(int enabled, const char *format,  ...)
{
    va_list ptr;
    char text[WLAN_MAX_TEXT_LEN];
    va_start(ptr, format);

    if (enabled)
    {
        vsnprintf(text, WLAN_MAX_TEXT_LEN - 1, format, ptr);
        ipcom_printf("%s", text);
    }

    va_end(ptr);

    return 0;
}

/*
 *===========================================================================
 *                      ipwlan_print_usage
 *===========================================================================
 * Description: Lists shell commands available
 * Parameters:  'en' == 0 => No printout to console
 * Returns:     <void>
 */
IP_STATIC void
ipwlan_print_usage
(
    int en
)
{
    ipwlan_printf(en, "Usage:"IP_LF);
    ipwlan_printf(en, " wifi <interface> auth [open]                       Set/Get authentication policy"IP_LF);
    ipwlan_printf(en, "                  auth shared-key|wpa|wpa-psk       Set authentication policy"IP_LF);
    ipwlan_printf(en, "                  auth wpa2|wpa2-psk                Set authentication policy"IP_LF);
    ipwlan_printf(en, "                  commit                            Commit changes"IP_LF);
    ipwlan_printf(en, "                  encr [none|tkip|ccmp]             Set/Get encryption type"IP_LF);
    ipwlan_printf(en, "                  encr wep 1|2|3|4 [<key>|<string>] Set WEP encryption"IP_LF);
    ipwlan_printf(en, "                  freq [auto|<frequency>]           Set/Get frequency"IP_LF);
    ipwlan_printf(en, "                  mode                              Get current mode"IP_LF);
    ipwlan_printf(en, "                  mode  ap                          Configure as access point"IP_LF);
    ipwlan_printf(en, "                  mode  client adhoc|infra|auto     Configure as client"IP_LF);
    ipwlan_printf(en, "                  quality                           Get link status"IP_LF);
    ipwlan_printf(en, "                  reset                             Reset device"IP_LF);
    ipwlan_printf(en, "                  ssid [<ssid>]                     Set/Get SSID"IP_LF);
}

/*
 ****************************************************************************
 * 9                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                      ipwlan_cmd_wlan_en
 *===========================================================================
 * Description: Parses and executes wlan shell commands.
 * Parameters:  'en' == 0 => Do not print to console,
 * Returns:     0 on success
 */
IP_STATIC int
ipwlan_cmd_wlan_en(int en, int argc, char **argv)
{
    int i;
    int j;
    int k;

    Ip_fd fd;
    char *endp;
    char ssid[32];
    Ipcom_iwreq iwreq;
    unsigned char oid_data[10];
    Ip_err ret = IPCOM_SUCCESS;

    if (argc < 3)
    {
        /* To few arguments */
        ipwlan_print_usage(en);

        return ret;
    }

#ifdef IPCOM_USE_INET
    fd = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, 0);
#else
    fd = ipcom_socket(IP_AF_INET6, IP_SOCK_DGRAM, 0);
#endif
    if (fd == IP_SOCKERR)
    {
        ipwlan_printf(en, "Error: unable to create command socket"IP_LF);

        return IPCOM_ERR_FAILED;
    }

    /* Verify interface existance */
    ipcom_sprintf(iwreq.if_name, "%.*s", IP_IFNAMSIZ - 1, argv[1]);
    if(ipcom_if_nametoindex(iwreq.if_name) < 1)
    {
        ipwlan_printf(en, "Error: Interface '%s' does not exist"IP_LF, iwreq.if_name);

        goto error;
    }

    /* Reset WLAN device */
    if (!ipcom_strcmp(argv[2], "reset"))
    {
        ipcom_socketioctl(fd, IP_SIOCSIWRESET, &iwreq);

        goto done;
    }

    /* Set/Get mode */
    if (!ipcom_strcmp(argv[2], "mode"))
    {
        if(argc < 4)
        {
            if(ipcom_socketioctl(fd, IP_SIOCGIWMODE, &iwreq) < 0)
            {
                goto error;
            }
            if(iwreq.u.mode == IP_IW_MODE_INFRA)
            {
                i = wlan_mode_client_infra;
            }
            else if(iwreq.u.mode == IP_IW_MODE_AUTO)
            {
                i = wlan_mode_client_auto;
            }
            else if(iwreq.u.mode == IP_IW_MODE_ADHOC)
            {
                i = wlan_mode_client_adhoc;
            }
            else if(iwreq.u.mode == IP_IW_MODE_MASTER)
            {
                i = wlan_mode_ap;
            }
            else
            {
                i = wlan_mode_unknown;
            }
            ipwlan_printf(en, "Current mode is: %s"IP_LF, wlan_mode_text[i]);
        }
        else
        {
            /* Set mode */
            if (!strcmp(argv[3], "client"))
            {
                if(argc < 5 || (strcmp(argv[4], "adhoc") && strcmp(argv[4], "auto") && strcmp(argv[4], "infra")))
                {
                    ipwlan_printf(en, "prism <interface> mode client [auto|adhoc|infra]"IP_LF);

                    goto done;
                }
                iwreq.u.mode = !strcmp(argv[4], "auto") ? IP_IW_MODE_AUTO : (!strcmp(argv[4], "infra") ? IP_IW_MODE_INFRA : IP_IW_MODE_ADHOC);

            }
            else if (!strcmp(argv[3], "ap"))
            {
                iwreq.u.mode = IP_IW_MODE_MASTER;
            }
            else
            {
                ipwlan_printf(en, "prism <interface> mode ap"IP_LF);
                ipwlan_printf(en, "prism <interface> mode client auto|adhoc|infra"IP_LF);

                goto done;
            }
            if (ipcom_socketioctl(fd, IP_SIOCSIWMODE, &iwreq) < 0)
            {
                goto error;
            }
        }
        goto done;
    }

    /* Get/Set SSID */
    if (!ipcom_strcmp(argv[2], "ssid"))
    {
        iwreq.u.essid.pointer = ssid;
        if (argc < 4)
        {
            /* Get SSID */
            iwreq.u.essid.length = sizeof(ssid);
            if (ipcom_socketioctl(fd, IP_SIOCGIWESSID, &iwreq) == 0)
            {
                if(ssid[0])
                {
                    ipwlan_printf(en, "ssid: %s"IP_LF, ssid);
                }
                else
                {
                    ipwlan_printf(en, "-- Not associated --"IP_LF);
                }
                goto done;
            }
        }
        else
        {
            /* Set SSID */
            iwreq.u.essid.length = ipcom_sprintf(ssid, "%.*s", sizeof(ssid) - 1, argv[3]);
            if (ipcom_socketioctl(fd, IP_SIOCSIWESSID, &iwreq) == 0)
            {
                goto done;
            }
        }
        goto error;
    }

    /* Get/Set frequency */
    if (!ipcom_strcmp(argv[2], "freq"))
    {
        if(argc < 4)
        {
            /* Get frequency */
            if (ipcom_socketioctl(fd, IP_SIOCGIWFREQ, &iwreq) < 0)
            {
                goto error;
            }
            ipwlan_printf(en, "RF frequency: %d kHz"IP_LF, iwreq.u.freq.m);
        }
        else
        {
            /* Set frequency */
            if (!strcmp(argv[3], "auto"))
            {
                iwreq.u.freq.flags = IP_IW_FREQ_AUTO;
                iwreq.u.freq.m = WLAN_FREQUENCY_BAND;
                iwreq.u.freq.e = 0;
            }
            else
            {
                i = ipcom_strtol(argv[3], &endp, 10);
                if (*endp != '\0')
                {
                    ipwlan_printf(en, "Error: invalid frequency value, '%s'"IP_LF, argv[3]);

                    goto done;
                }
                iwreq.u.freq.flags = IP_IW_FREQ_FIXED;
                iwreq.u.freq.m = i;
                iwreq.u.freq.e = 0;
                if (ipcom_socketioctl(fd, IP_SIOCSIWFREQ, &iwreq) < 0)
                {
                    goto error;
                }
            }
        }
        goto done;
    }

    /* Get/Set authentication */
    if (!ipcom_strcmp(argv[2], "auth"))
    {
        if (argc < 4)
        {
            /* Get authentication policy */
            ipwlan_printf(en, "Cannot get authentication policy. Try device specific shell command"IP_LF);
        }
        else
        {
            /* Set authentication policy */
            if (strcmp(argv[3], "open") && strcmp(argv[3], "shared-key") && strcmp(argv[3], "wpa-psk") && strcmp(argv[3], "wpa") && strcmp(argv[3], "wpa2-psk") && strcmp(argv[3], "wpa2"))
            {
                ipwlan_printf(en, "Error: invalid authentication policy, valid policies are 'open', 'shared-key', 'wpa-psk or 'wpa'" IP_LF);

                goto done;
            }
            if(strcmp(argv[3], "open") == 0)
                iwreq.u.auth_type = IP_IW_AUTH_ALG_OPEN_SYSTEM;
            else if(strcmp(argv[3], "shared-key") == 0)
                iwreq.u.auth_type = IP_IW_AUTH_ALG_SHARED_KEY;
            else if(strcmp(argv[3], "wpa2-psk") == 0)
                iwreq.u.auth_type = IP_IW_AUTH_ALG_WPA2_PSK;
            else if(strcmp(argv[3], "wpa2") == 0)
                iwreq.u.auth_type = IP_IW_AUTH_ALG_WPA2;
            else if(strcmp(argv[3], "wpa-psk") == 0)
                iwreq.u.auth_type = IP_IW_AUTH_ALG_WPA_PSK;
            else
                iwreq.u.auth_type = IP_IW_AUTH_ALG_WPA;

            if (ipcom_socketioctl(fd, IP_SIOCSIWAUTHTYPE, &iwreq) < 0)
            {
                goto error;
            }
        }
        goto done;
    }

    /* Get/Set encryption */
    if (!ipcom_strcmp(argv[2], "encr"))
    {
        if (argc < 4)
        {
            if (ipcom_socketioctl(fd, IP_SIOCGIWENCRTYPE, &iwreq) < 0)
            {
                goto error;
            }
            for(i = 0; i < (int) (sizeof(ipwlan_encr_policy)/sizeof(ipwlan_encr_policy[0])); i++)
            {
                if ((unsigned) ipwlan_encr_policy[i] == iwreq.u.encr_type)
                {
                    break;
                }
            }
            ipwlan_printf(en, "privacy: %s"IP_LF, wlan_encr_text[i]);
        }
        else
        {
            /* Set encryption parameters */
            if (!strcmp(argv[3], "wep"))
            {
                if (argc < 5 || argc > 6)
                {
                    ipwlan_printf(en, "Usage: wlan <interface> encr wep 1|2|3|4 [<key>]"IP_LF);

                    goto done;
                }
                i = ipcom_strtol(argv[4], &endp, 10);
                if (*endp != '\0' || i < 1 || i > 4)
                {
                    ipwlan_printf(en, "Error: Invalid index value allowed are 1 to 4" IP_LF);

                    goto done;
                }
                if(argc == 6)
                {
                    if (argv[5][0] == '0' && argv[5][1] == 'x')
                    {
                        if (ipcom_strspn(argv[5] + 2, "012345678ABCDEFabcdef") != ipcom_strlen(argv[5] + 2) || ipcom_strlen(argv[5] + 2) > 32 || (ipcom_strlen(argv[5] + 2) % 2) != 0) /* 32 nibbles => 128 bits */
                        {
                            ipwlan_printf(en, "Usage: wlan <interface> encr wep <index> 0x<hex key>|<string key>"IP_LF);

                            goto done;
                        }
                        for (j = 0; ipcom_sscanf(argv[5] + 2 + 2 * j, "%2x", &k); j++)
                        {
                            iwreq.u.key.key[j] = (unsigned char)k;
                        }
                        iwreq.u.key.len = j;
                    }
                    else
                    {
                        /* key is a text string */
                        iwreq.u.key.len = ipcom_sprintf((char *)iwreq.u.key.key, "%.*s", MIN(sizeof(iwreq.u.key.key) - 1, ipcom_strlen(argv[5])), argv[5]);
                    }
                    iwreq.u.key.flags = 0;
                }
                else
                {
                    iwreq.u.key.flags = IP_IW_KEY_FLAG_NOKEY;
                }
                iwreq.u.key.type = IP_IW_ENCR_ALG_WEP;
                iwreq.u.key.index = i - 1; /* Index 1 - 4 */
                if (ipcom_socketioctl(fd, IP_SIOCSIWENCRKEY, &iwreq) < 0)
                {
                    goto error;
                }

                iwreq.u.encr_type = IP_IW_ENCR_ALG_WEP;
                if (ipcom_socketioctl(fd, IP_SIOCSIWENCRTYPE, &iwreq) < 0)
                {
                    goto error;
                }
            }
            else if (!strcmp(argv[3], "none"))
            {
                iwreq.u.encr_type = IP_IW_ENCR_ALG_NONE;

                if (ipcom_socketioctl(fd, IP_SIOCSIWENCRTYPE, &iwreq) < 0)
                {
                    goto error;
                }
            }
            else if (!strcmp(argv[3], "tkip"))
            {
                iwreq.u.encr_type = IP_IW_ENCR_ALG_TKIP;

                if (ipcom_socketioctl(fd, IP_SIOCSIWENCRTYPE, &iwreq) < 0)
                {
                    goto error;
                }
            }
            else if (!strcmp(argv[3], "ccmp"))
            {
                iwreq.u.encr_type = IP_IW_ENCR_ALG_CCMP;

                if (ipcom_socketioctl(fd, IP_SIOCSIWENCRTYPE, &iwreq) < 0)
                {
                    goto error;
                }
            }
            else
            {
                goto error;
            }
        }

        goto done;
    }

    /* Commit changes */
    if (!ipcom_strcmp(argv[2], "commit"))
    {
        if (ipcom_socketioctl(fd, IP_SIOCSIWCOMMIT, &iwreq) < 0)
        {
            goto error;
        }

        goto done;
    }

    /* Get link quality */
    if (!ipcom_strcmp(argv[2], "quality"))
    {
        ipcom_socketioctl(fd, IP_SIOCGIWAP, &iwreq);
        ipcom_memcpy(oid_data, iwreq.u.addr.sa_data, 6);
        ipcom_memcpy(iwreq.u.quality.bssid, oid_data, 6);
        if (ipcom_socketioctl(fd, IP_SIOCGIWQUALITY, &iwreq) < 0)
        {
            goto error;
        }

        ipwlan_printf(en, "\r\nLink quality\r\n============\r\n");
        ipwlan_printf(en, "RSSI:   % 3d dBm\r\n", (int)iwreq.u.quality.level);
        ipwlan_printf(en, "Rate:   % 3d Mbps\r\n", (unsigned int)iwreq.u.quality.qual);

        goto done;
    }

error:

    ipwlan_printf(en, "Error: command %s failed"IP_LF, argv[2]);
    ret = IPCOM_ERR_FAILED;

done:
    (void)ipcom_socketclose(fd);

    return ret;
}

 /*
 *===========================================================================
 *                      ipwlan_cmd_prism
 *===========================================================================
 * Description: Executes shell commands. Prints output to console
 * Parameters:
 * Returns:     Non-zero on failure
 */
IP_PUBLIC int
ipwlan_cmd_wlan(int argc, char **argv)
{
    return ipwlan_cmd_wlan_en(1, argc, argv);
}


/*
 *===========================================================================
 *                         END OF FILE
 *===========================================================================
 */

