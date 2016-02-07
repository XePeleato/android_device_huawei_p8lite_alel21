/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_sysvar2.c,v $ $Revision: 1.8 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_sysvar2.c,v $
 *   $Author: joel $
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
#include "ipcom_clib.h"
#include "ipcom_err.h"
#include "ipcom_sysvar.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 * The expected syntax for the <param> member in 'Ipcom_sysvar_ext_struct'
 * is as follows: "<ifparam1>=<value1>;<ifparam2>=<value2>;...". Each pair
 * of "<ifparam>=<value>" is semicolon generated.
 */
#ifdef IPCOM_USE_SYSVAR_EXT
#define IPCOM_IFPARAM_LIST_DELIMITER   ";"
#define IPCOM_IFPARAM_DELIMITER   "="
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

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

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
 *                    ipcom_sysvar_setv
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_PUBLIC Ip_err
ipcom_sysvar_setv(const Ipcom_sysvar entries[], int flags)
{
    /* !!TODO: make atomic */
    int i;
    Ip_err err;

    /*
     * Add the sysvar such that they will be listed in the same
     * order they were defined.
     */

    for(i = 0; entries[i].name != IP_NULL; i++);

    for(--i; i >= 0; i--)
    {
        err = ipcom_sysvar_set(entries[i].name, entries[i].value, flags);
        if (err != IPCOM_SUCCESS && err != IPCOM_ERR_DUPLICATE)
            return err;
    }
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_sysvar_get_as_int
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_sysvar_get_as_int(const char *name, int default_value)
{
    char strval[16];
    Ip_size_t strval_len = sizeof(strval);

    if (ipcom_sysvar_get(name, strval, &strval_len) == IP_NULL)
        return default_value;

    return ipcom_atoi(strval);
}


/*
 *===========================================================================
 *                    ipcom_sysvar_ext_setv
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
#ifdef IPCOM_USE_SYSVAR_EXT
IP_PUBLIC Ip_err
ipcom_sysvar_ext_setv(const Ipcom_sysvar_ext entries[], int flags)
{
    /* !!TODO: make atomic */
    int i;
    int len = 0;
    int  param_len = 0;
    char *sysvar_name = IP_NULL;
    char *sysvar_param = IP_NULL;
    char *conf = IP_NULL;
    char *last = IP_NULL;
    char *value = IP_NULL;
    char *ifparam = IP_NULL;
    Ip_err err = IPCOM_SUCCESS;

    sysvar_name = ipcom_calloc(1, sizeof(char) * 256);
    if (sysvar_name == IP_NULL)
        return IPCOM_ERR_NO_MEMORY;

    for (i = 0; ; i++)
    {
        /*
         * The syntax {IP_NULL,IP_NULL,IP_NULL} terminates the configuration
         * array. Otherwise, <name1> and/or <name2> must be specified.
         */
        if ((entries[i].name1 == IP_NULL) && (entries[i].name2 == IP_NULL))
        {
            err = (entries[i].param == IP_NULL) ? IPCOM_SUCCESS :
                  IPCOM_ERR_INVALID_CONFIG;
            goto done;
        }

        /*
         * The expected string should contain <ifparam>=<value>. If the
         * separator is not found, assume the string does not contain
         * any valid configuration values.
         */
        if (entries[i].param == IP_NULL ||
            ipcom_strchr(entries[i].param, '=') == IP_NULL)
            continue;

        len = ipcom_strlen(entries[i].param);
        if ((len > param_len) || (sysvar_param == IP_NULL))
        {
            if (sysvar_param)
                ipcom_free(sysvar_param);  /* free previously allocated mem */
            param_len = len;
            sysvar_param = ipcom_calloc(1, param_len+1);
            if (sysvar_param == IP_NULL)
            {
                ipcom_free(sysvar_name);
                return IPCOM_ERR_NO_MEMORY;
            }
        }
        else
            ipcom_memset(sysvar_param, 0, param_len+1);

        ipcom_strcpy(sysvar_param, entries[i].param);

        /*
         * Expected format: <ifparam1>=<value1>;<ifparam2>=<value2>;...
         */
        conf = ipcom_strtok_r(sysvar_param,
                              IPCOM_IFPARAM_LIST_DELIMITER, &last);
        if (conf == IP_NULL)
        {
            err = IPCOM_ERR_INVALID_CONFIG;
            goto done;
        }

        do {
            /* Expected format: <ifparam>=<value> */
            ifparam = ipcom_strtok_r(conf, IPCOM_IFPARAM_DELIMITER, &value);
            if ((ifparam == IP_NULL) || (value == IP_NULL))
            {
                err = IPCOM_ERR_INVALID_CONFIG;
                goto done;
            }

            ipcom_memset(sysvar_name, 0, sizeof(sysvar_name));
            if (entries[i].name1 != IP_NULL)
            {
                ipcom_strcpy(sysvar_name, entries[i].name1);
                ipcom_strcat(sysvar_name, ".");
            }
            ipcom_strcat(sysvar_name, ifparam);
            if (entries[i].name2 != IP_NULL)
            {
                ipcom_strcat(sysvar_name, ".");
                ipcom_strcat(sysvar_name, entries[i].name2);
            }
            err = ipcom_sysvar_set(sysvar_name, value, flags);
            if (err == IPCOM_ERR_DUPLICATE)
                err = IPCOM_SUCCESS;
            else if (err != IPCOM_SUCCESS)
                goto done;
        } while (IP_NULL != (conf = ipcom_strtok_r(IP_NULL,
                                                   IPCOM_IFPARAM_LIST_DELIMITER, &last)));
    }

done:
    ipcom_free(sysvar_name);
    if (sysvar_param)
        ipcom_free(sysvar_param);
    return err;
}
#endif /* IPCOM_USE_SYSVAR_EXT */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

