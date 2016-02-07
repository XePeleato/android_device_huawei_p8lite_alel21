/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_cmd_user.c,v $ $Revision: 1.13 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_cmd_user.c,v $
 *   $Author: ulf $
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
#include "ipcom_auth.h"
#include "ipcom_sysvar.h"


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

IP_PUBLIC int ipcom_cmd_user(int argc, char **argv);


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
 ****************************************************************************
 * 10                    PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_cmd_user
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_cmd_user(int argc, char **argv)
{
    Ip_err    err;

    if(argc < 2)
    {
        ipcom_fprintf(ip_stderr,
                      "Interpeak system variable command, version 1.1"IP_LF
                      "usage:  "IP_LF
                      "   user list"IP_LF
                      "   user add <username> <password> <user id> <h/hash or c/cleartext>"IP_LF
                      "   user del <username>"IP_LF
                      IP_LF
                      );
        return 0;
    }

    /* add command. */
    if(ipcom_strcmp(argv[1], "add") == 0)
    {
        if(argc < 5)
        {
            ipcom_printf("user: missing argument(s)"IP_LF);
            return -1;
        }  

	if(argc < 6) /* Cleartext */
            err = ipcom_auth_useradd(argv[2], argv[3], (Ip_uid_t)ipcom_atoi(argv[4])); /*lint !e571 */
        else 
            if((ipcom_strcmp(argv[5], "c") == 0) || (ipcom_strcmp(argv[5], "cleartext") == 0)) /* Cleartext */ 
                err = ipcom_auth_useradd(argv[2], argv[3], (Ip_uid_t)ipcom_atoi(argv[4])); /*lint !e571 */
            else if((ipcom_strcmp(argv[5], "h") == 0) || (ipcom_strcmp(argv[5], "hash") == 0)) /* Hash */ 
                err = ipcom_auth_useradd_hash(argv[2], argv[3], (Ip_uid_t)ipcom_atoi(argv[4]), IP_TRUE); /*lint !e571 */
            else 
	    {    
                ipcom_printf ("user: unknown hash option '%s'"IP_LF, argv[5]);
                return -1;
            }  

        if(err == IPCOM_SUCCESS)
        {
            ipcom_printf("added user %s"IP_LF, argv[2]);
            return 0;
        }
        else
        {
            ipcom_printf("user: failed to add user '%s', %s"IP_LF,
                         argv[2], ipcom_err_string(err));
            return -1;
        }
    }

    /* delete command. */
    else if(ipcom_strcmp(argv[1], "del") == 0)
    {
        if(argc < 3)
        {
            ipcom_printf("user: missing argument(s)"IP_LF);
            return -1;
        }
        else
        {
            err = ipcom_auth_userdel(argv[2]);
            if(err == IPCOM_SUCCESS)
            {
                ipcom_printf("deleted user %s"IP_LF, argv[2]);

#ifdef IPCOM_USE_SYSVAR
                {
                    char conf[IPCOM_AUTH_USERNAME_MAX+16];
                    ipcom_sprintf(conf, "ipcom.users.%s*", argv[2]);
                    (void)ipcom_sysvar_unset(conf);
                }
#endif /* IPCOM_USE_SYSVAR */
                return 0;
            }
            else
            {
                ipcom_printf("user: failed to delete user '%s', %s"IP_LF,
                             argv[2], ipcom_err_string(err));
                return -1;
            }
        }
    }

    /* delete command. */
    else if(ipcom_strcmp(argv[1], "del") == 0)
    {
        if(argc < 3)
        {
            ipcom_printf("user: missing argument(s)"IP_LF);
            return -1;
        }
        else
        {
            err = ipcom_auth_userdel(argv[2]);
            if(err == IPCOM_SUCCESS)
            {
                ipcom_printf("deleted user %s"IP_LF, argv[2]);
                return 0;
            }
            else
            {
                ipcom_printf("user: failed to delete user '%s', %s"IP_LF,
                             argv[2], ipcom_err_string(err));
                return -1;
            }
        }
    }

    /* list command. */
    else if(ipcom_strcmp(argv[1], "list") == 0)
    {
        Ip_uid_t  userid = (Ip_uid_t)(~0);
        Ipcom_auth_userinfo userinfo;

        /* Get first user's userid. */
        ipcom_printf("Users:"IP_LF);
        err = IPCOM_SUCCESS;
        (void)ipcom_auth_userlist(&userid, IP_NULL);
        while (err == IPCOM_SUCCESS && userid != (Ip_uid_t)~0)
        {
            err = ipcom_auth_userlist(&userid, &userinfo);
            if (err == IPCOM_SUCCESS)
                ipcom_printf("%5u   %s"IP_LF, (int)userinfo.userid, userinfo.username);
        }
        return 0;
    }

    ipcom_printf ("user: unknown command '%s'"IP_LF, argv[1]);
    return -1;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

