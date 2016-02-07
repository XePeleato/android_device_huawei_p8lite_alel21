/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_cmd_sysvar.c,v $ $Revision: 1.26 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_cmd_sysvar.c,v $
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
#include "ipcom_sysvar.h"
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

IP_PUBLIC int ipcom_cmd_sysvar(int argc, char **argv);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_cmd_sysvar_for_each_cb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_cmd_sysvar_for_each_cb(const char *name, const char *value, int flags, void *cookie)
{
    (void)flags;
    (void)cookie;

    ipcom_printf("   %s=%s"IP_LF, name, value);
    return 0;
}


/*
 ****************************************************************************
 * 10                    PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_cmd_sysvar
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_cmd_sysvar(int argc, char **argv)
{
    Ipcom_getopt   opt;
    int            c;
    Ip_size_t      size = 0;
    int            flags = IPCOM_SYSVAR_FLAG_NOCREATE;
    char          *value;
    Ip_err         retval;
#if IPCOM_VR_MAX > 1
    int            vr = ipcom_proc_vr_get();
    int            vr_new = vr;
#endif

    if(argc < 2)
    {
#if IPCOM_VR_MAX > 1
        ipcom_fprintf(ip_stderr,
                      "Interpeak system variable command, version 1.2 [VR %d]"IP_LF, vr);
#else
        ipcom_fprintf(ip_stderr,
                      "Interpeak system variable command, version 1.2"IP_LF);
#endif
        ipcom_fprintf(ip_stderr,
                      "usage:  "IP_LF
                      "   sysvar list [name[*]]"IP_LF
                      "   sysvar get <name>"IP_LF
                      "   sysvar unset <name[*]>"IP_LF
                      "   sysvar set [-cor] <name> <value>"IP_LF
                      "      -c  ok to create"IP_LF
                      "      -o  ok to overwrite"IP_LF
                      "      -r  flag readonly"IP_LF
                      IP_LF
                      );
        return 0;
    }

    ipcom_getopt_clear_r(&opt);
    while ((c = ipcom_getopt_r(argc - 1, argv + 1, "orcs:V:", &opt)) != -1)
    {
        switch(c)
        {
        case 'o':
            IP_BIT_SET(flags, IPCOM_SYSVAR_FLAG_OVERWRITE);
            break;
        case 'r':
            IP_BIT_SET(flags, IPCOM_SYSVAR_FLAG_READONLY);
            break;
        case 'c':
            IP_BIT_CLR(flags, IPCOM_SYSVAR_FLAG_NOCREATE);
            break;
        case 's':
            size = (Ip_size_t)ipcom_atol(opt.optarg);
            break;
#if IPCOM_VR_MAX > 1
        case 'V':
            vr_new = ipcom_atoi(opt.optarg);
            break;
#endif
        default:
            ipcom_printf("sysvar: unknown option %c"IP_LF, (char)c);
            return -1;
        }
    }

#if IPCOM_VR_MAX > 1
    if (vr != vr_new)
        (void)ipcom_proc_vr_set(vr_new);
#endif

    /* get command. */
    if(ipcom_strcmp(argv[1], "get") == 0)
    {
        if(argc < 3)
            ipcom_printf("sysvar: missing <name> argument"IP_LF);
        else
        {
            if (size > 0)
            {
                char *buf;

                buf = ipcom_malloc(size);
                if (buf == IP_NULL)
                    ipcom_printf("sysvar: out of memory"IP_LF);
                else
                {
                    value = ipcom_sysvar_get(argv[opt.optind+1], buf, &size);
                    if (value)
                        ipcom_printf("sysvar: %s=%s, length=%d"IP_LF,
                                     argv[opt.optind+1], value, (int)size);
                    else
                        ipcom_printf("sysvar: '%s' not found (length = %d)"IP_LF,
                                     argv[opt.optind+1], (int)size);
                    ipcom_free(buf);
                    goto leave;
                }
            }
            else
            {
                value = ipcom_sysvar_get(argv[opt.optind+1], IP_NULL, 0);
                if(value)
                {
                    ipcom_printf("sysvar: %s=%s"IP_LF, argv[2], value);
                    ipcom_free(value);
                    goto leave;
                }
                else
                    ipcom_printf("sysvar: '%s' not found"IP_LF, argv[2]);
            }
        }
    }
    /* set command. */
    else if(ipcom_strcmp(argv[1], "set") == 0)
    {
        if(argc < opt.optind + 3)
            ipcom_printf("sysvar: missing <value> argument"IP_LF);
        else
        {
            int vi;
            for (vi = opt.optind + 1; (vi + 1) < argc; vi += 2)
            {
                retval = ipcom_sysvar_set(argv[vi], argv[vi + 1], flags);
                if(retval == IPCOM_SUCCESS)
                    ipcom_printf("sysvar: %s=%s ok"IP_LF, argv[vi], argv[vi + 1]);
                else
                    ipcom_printf("sysvar: set failed : %s"IP_LF, ipcom_err_string(retval));
            }
        }
    }
    /* unset command. */
    else if(ipcom_strcmp(argv[1], "unset") == 0)
    {
        if(argc < 3)
            ipcom_printf("sysvar: missing <name> argument"IP_LF);
        else
        {
            int vi;
            for (vi = opt.optind + 1; vi < argc; vi++)
            {
                retval = ipcom_sysvar_unset(argv[vi]);
                if(retval == IPCOM_SUCCESS)
                {
                    ipcom_printf("sysvar: '%s' unset ok"IP_LF, argv[vi]);
                }
                else
                    ipcom_printf("sysvar: unset failed: %s"IP_LF, ipcom_err_string(retval));
            }
        }
    }
    /* list command. */
    else if(ipcom_strcmp(argv[1], "list") == 0)
    {
        ipcom_printf("System variables:"IP_LF);
        retval = ipcom_sysvar_for_each(argc > 2 ? argv[opt.optind+1] : IP_NULL,
                                       ipcom_cmd_sysvar_for_each_cb,
                                       IP_NULL);
        if(retval == IPCOM_SUCCESS)
            goto leave;
        else
            ipcom_printf("sysvar: list failed: %s"IP_LF, ipcom_err_string(retval));

    }
    /* unknown command. */
    else
        ipcom_printf ("sysvar: unknown command '%s'"IP_LF, argv[1]);

leave:
#if IPCOM_VR_MAX > 1
    if (vr != vr_new)
        (void)ipcom_proc_vr_set(vr);
#endif

    return 0;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

