/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_cmd_syslog.c,v $ $Revision: 1.17 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_cmd_syslog.c,v $
 *   $Author: rboden $
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
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC int ipcom_cmd_syslog(int argc, char **argv);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_cmd_syslog_facility
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC Ip_u32
ipcom_cmd_syslog_facility(const char *facility)
{
    Ip_u32 i;

    for(i = 0; i < IPCOM_LOG_FACILITY_MAX; i++)
        if (ipcom_strcmp(ipcom_syslog_facility_names[i], facility) == 0)
            return (i << 3);

    return 0xffffffff;
}


/*
 *===========================================================================
 *                    ipcom_cmd_syslog_priority
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC int
ipcom_cmd_syslog_priority(const char *priority)
{
    int i;

    for(i = 0; i <= IPCOM_LOG_DEBUG2; i++)
        if (ipcom_strcasecmp(ipcom_syslog_priority_names[i], priority) == 0)
            return i;

    return -1;
}


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_cmd_syslog
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_cmd_syslog(int argc, char **argv)
{
    Ipcom_getopt   opt;
    int            c;
    Ip_u32         i;
    int            priority;
    Ip_u32         facility;

    if (argc < 2)
    {
        ipcom_fprintf(ip_stderr,
                      "Interpeak syslog command, version 1.1"IP_LF
                      "usage:  "IP_LF
                      "   syslog echo <prio> <message>"IP_LF
                      "   syslog list"IP_LF
                      "   syslog priority <facility> <prio>"IP_LF
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
                      "   syslog log file [logfile]"IP_LF
#endif
                      IP_LF
                      );
        return 0;
    }

    ipcom_getopt_clear_r(&opt);
    while ((c = ipcom_getopt_r(argc - 1, argv + 1, "orc", &opt)) != -1)
    {
        switch(c)
        {
        case 'o':
            break;
        case 'r':
            break;
        case 'c':
            break;
        default:
            ipcom_printf("syslog: unknown option %c"IP_LF, (char)c);
            return -1;
        }
    }

    /* list command. */
    if (ipcom_strcmp(argv[1], "list") == 0)
    {
        ipcom_printf("syslog facility    priority"IP_LF);
        for(i = 0; i < IPCOM_LOG_FACILITY_MAX; i++)
        {
            priority = ipcom_getlogmask_on(i << 3);
            if (priority == -1)
                continue;

            ipcom_printf("%-15s    %-7s"IP_LF,
                         ipcom_syslog_facility_names[i],
                         ipcom_syslog_priority_names[priority]);
        }
        ipcom_printf(IP_LF);
        return 0;
    }

    /* priority command. */
    else if (ipcom_strcmp(argv[1], "priority") == 0)
    {
        if (argc < 3)
        {
            ipcom_printf("syslog: missing <facility>"IP_LF);
            ipcom_printf("        type 'syslog list' for available facilities"IP_LF);
            return -1;
        }

        if (argc < 4)
        {
            ipcom_printf("syslog: missing <prio>"IP_LF);
            goto prios;
        }

        priority = ipcom_cmd_syslog_priority(argv[3]);
        if (priority == -1)
        {
            ipcom_printf("syslog: unknown priority '%s'"IP_LF, argv[3]);
        prios:
            ipcom_printf("        priorities: ");
            for(i = 0; i <= IPCOM_LOG_DEBUG2; i++)
                ipcom_printf("%s ", ipcom_syslog_priority_names[i]);
            ipcom_printf(IP_LF);
            return -1;
        }

        if (ipcom_strcasecmp(argv[2], "all") == 0)
        {
            for(i = 0; i < IPCOM_LOG_FACILITY_MAX; i++)
            {
                if (ipcom_setlogmask_on(priority, i << 3) != -1)
                    ipcom_printf("syslog: facility %s priority set to %s"IP_LF, ipcom_syslog_facility_names[i], argv[3]);
            }
            return 0;
        }
        else
        {
            facility = ipcom_cmd_syslog_facility(argv[2]);
            if (facility == 0xffffffff)
            {
                ipcom_printf("syslog: unknown facility '%s'"IP_LF, argv[2]);
                return -1;
            }

            if (ipcom_setlogmask_on(priority, facility) != -1)
                ipcom_printf("syslog: facility %s priority set to %s"IP_LF, argv[2], argv[3]);
            else
                ipcom_printf("syslog: failed to set priority"IP_LF);

            return 0;
        }
    }

    /* log command. */
    else if (ipcom_strcmp(argv[1], "log") == 0)
    {
        if (argc ==  2)
        {
            /*!! list. */
            return 0;
        }
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
        if (ipcom_strcmp(argv[2], "file") == 0)
        {
            c = ipcom_setlogfile(argc == 3 ? IP_NULL : argv[3]);
            ipcom_printf ("syslog: %s"IP_LF,
                          (c == 0 && argc == 3) ? "disabling file log" :
                          c == 0 ? "logging to file" :
                          "failed");
            return 0;
        }
#endif
        return 0;
    }

    /* echo command. */
    else if (ipcom_strcmp(argv[1], "echo") == 0)
    {

        if (argc < 3)
        {
            ipcom_printf("syslog: missing <prio>"IP_LF);
            return -1;
        }

        priority = ipcom_cmd_syslog_priority(argv[2]);
        if (priority == -1)
        {
            ipcom_printf("syslog: unknown priority '%s'"IP_LF, argv[2]);
            goto prios;
        }

        if (argc < 4)
        {
            ipcom_printf("syslog: missing <message>"IP_LF);
            return -1;
        }

#if !defined(_lint) && defined(IPCOM_USE_SYSLOG)
        if (IPCOM_SYSLOG_ENABLE(1) && IPCOM_SYSLOG_IPCOM_PRIORITY >= priority)
            ipcom_syslog(IPCOM_LOG_IPCOM | priority, IPCOM_SYSLOG_BODY(argv[3]));
#endif

        return 0;
    }

    /* unknown command. */
    else
    {
        ipcom_printf ("syslog: unknown command '%s'"IP_LF, argv[1]);
        return -1;
    }
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

