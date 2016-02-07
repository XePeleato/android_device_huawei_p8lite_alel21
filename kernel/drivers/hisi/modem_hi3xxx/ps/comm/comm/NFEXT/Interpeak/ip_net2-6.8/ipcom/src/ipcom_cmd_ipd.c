/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_cmd_ipd.c,v $ $Revision: 1.18 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_cmd_ipd.c,v $
 *   $Author: jhorteli $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <email@interpeak.se>
 ******************************************************************************
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

#include "ipcom_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */


#define IPCOM_USE_CLIB_PROTO
#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_clib.h"
#include "ipcom_err.h"
#include "ipcom_syslog.h"
#include "ipcom_getopt.h"
#include "ipcom_ipd.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipcom_ipd_message
 *===========================================================================
 */
typedef struct Command_struct
{
    char    *name;
    int      msgtype;
}
Ipcom_ipd_message;


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

#ifdef IP_PORT_OSE5
IP_PUBLIC Ip_err ipcom_ipd_isinstalled_ose5(const char *name);
#endif


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC int ipcom_cmd_ipd(int argc, char **argv);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_STATIC IP_CONST Ipcom_ipd_message ipcom_cmd_ipd_messages[] =
{
    /* name,         msgtype */
    { "start",       IPCOM_IPD_MSGTYPE_START },
    { "reconfigure", IPCOM_IPD_MSGTYPE_RECONFIGURE },
    { "kill",        IPCOM_IPD_MSGTYPE_KILL },
    { "ping",        IPCOM_IPD_MSGTYPE_PING },
    { IP_NULL,       -1 }
};


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_cmd_ipd
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_cmd_ipd(int argc, char **argv)
{
    Ipcom_getopt   opt;
    int            i, c, msgtype;
    Ip_err         err = IPCOM_SUCCESS;
#if IPCOM_VR_MAX > 1
    int            vr = ipcom_proc_vr_get();
    int            vr_new = vr;
#endif

    if (argc < 2)
    {
usage:
        ipcom_fprintf(ip_stderr,
                      "Interpeak daemon (IPD) command, version 1.2"IP_LF
                      "usage:  "IP_LF
                      "   ipd [-V <vr>] list"IP_LF
                      "   ipd [-V <vr>] start <service>"IP_LF
                      "   ipd [-V <vr>] kill <service>"IP_LF
                      "   ipd [-V <vr>] reconfigure <service>"IP_LF
                      "   ipd [-V <vr>] <#> <service>"IP_LF
                      IP_LF);
        return 0;
    }

    ipcom_getopt_clear_r(&opt);
    while ((c = ipcom_getopt_r(argc, argv, "V:", &opt)) != -1)
    {
        switch(c)
        {
        case 'V':
#if IPCOM_VR_MAX > 1
            vr_new = ipcom_atoi(opt.optarg);
#endif
            break;
        default:
            ipcom_printf("ipd: unknown option %c"IP_LF, (char)c);
            return -1;
        }
    }

    if (opt.optind >= argc)
    {
        ipcom_printf("ipd: missing <command> argument"IP_LF);
        goto usage;
    }


    if(ipcom_strcmp(argv[opt.optind], "list") == 0)
    {
#if IPCOM_VR_MAX > 1
        if (vr != vr_new)
            ipcom_proc_vr_set(vr_new);
#endif
        ipcom_printf("Services:"IP_LF);
        for (i = 0; ipcom_ipd_products[i].name != IP_NULL; i++)
        {
            if ((argc - (opt.optind + 1)) > 0)
            {
                int p;
                for (p = opt.optind + 1; p < argc; p++)
                {
                    if (ipcom_strcasecmp(ipcom_ipd_products[i].name, argv[p]) == 0)
                        goto print_service;
                }
                continue;
            }

print_service:
            if (IP_BIT_ISSET(ipcom_ipd_products[i].flags, IPCOM_IPD_FLAG_IPD_START))
            {
#ifdef IP_PORT_OSE5
                if (ipcom_ipd_products[i].start == IP_NULL
                    && ipcom_ipd_isinstalled_ose5(ipcom_ipd_products[i].name) != IPCOM_SUCCESS)
                    continue;
#endif
                err = ipcom_ipd_send(ipcom_ipd_products[i].name, IPCOM_IPD_MSGTYPE_PING);
                ipcom_printf("%-20s %-s"IP_LF,
                             ipcom_ipd_products[i].name,
                             err == IPCOM_SUCCESS ? "started" : "killed");
            }
            else if (ipcom_ipd_products[i].start)
                ipcom_printf("%-20s %-s"IP_LF,
                             ipcom_ipd_products[i].name, "started");

        }
        ipcom_printf(IP_LF);
#if IPCOM_VR_MAX > 1
        if (vr != vr_new)
            ipcom_proc_vr_set(vr);
#endif
        return 0;
    }

    if ((argc - opt.optind) < 2)
    {
        ipcom_printf("ipd: missing <service> argument"IP_LF);
        return -1;
    }

    for (i = 0; ipcom_cmd_ipd_messages[i].name; i++)
        if (ipcom_strcmp(argv[opt.optind], ipcom_cmd_ipd_messages[i].name) == 0)
        {
            msgtype = ipcom_cmd_ipd_messages[i].msgtype;
            goto sendmsg;
        }

    if (*argv[opt.optind] == '-' && ipcom_isdigit(argv[opt.optind][1]))
    {
        /* "UNIX" signal support (using negative numbers) */
        msgtype = -ipcom_atoi(argv[opt.optind] + 1);
        goto sendmsg;
    }

    if (ipcom_isdigit(argv[opt.optind][0]))
    {
        /* positive numbers */
        msgtype = ipcom_atoi(argv[1]);
        goto sendmsg;
    }

    /* unknown command. */
    ipcom_printf ("ipd: unknown command '%s'"IP_LF, argv[opt.optind]);
    return -1;

    /* send msg */
 sendmsg:
#if IPCOM_VR_MAX > 1
    if (vr != vr_new)
        ipcom_proc_vr_set(vr_new);
#endif


    if ((argc - opt.optind) < 3)
        err = ipcom_ipd_send(argv[opt.optind+1], msgtype);
    else
    {
        Ipcom_ipd_msg   *msg;
        int             sz  = ipcom_strlen(argv[opt.optind + 2]) + 1;

        msg = ipcom_calloc(1, sz + sizeof(*msg));
        if (msg != IP_NULL)
        {
            msg->msgtype = msgtype;
            ipcom_memcpy(msg + 1, argv[opt.optind + 2], sz);
            err = ipcom_ipd_sendmsg(argv[opt.optind+1], msg, sz + sizeof(*msg));
            ipcom_free(msg);
        }
    }

    if(err == IPCOM_SUCCESS)
        ipcom_printf("ipd: %s %s ok"IP_LF, argv[opt.optind], argv[opt.optind+1]);
    else
        ipcom_printf("ipd: %s %s failed: %s"IP_LF, argv[opt.optind], argv[opt.optind+1], ipcom_err_string(err));

#if IPCOM_VR_MAX > 1
    if (vr != vr_new)
        ipcom_proc_vr_set(vr);
#endif
    return 0;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

