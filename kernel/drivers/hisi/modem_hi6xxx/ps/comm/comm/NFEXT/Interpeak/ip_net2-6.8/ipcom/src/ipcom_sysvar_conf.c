/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_sysvar_conf.c,v $ $Revision: 1.5 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_sysvar_conf.c,v $
 *   $Author: rime $
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
 *                    ipcom_sysvar_get_conf
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_PUBLIC char *
ipcom_sysvar_get_conf(const char *name, char *value, Ip_size_t *value_size,
                      const char *username, const char *ifname)
{
    char buf[256];
    Ip_size_t org_value_size;
    char *ret;

    if (value != IP_NULL && value_size == IP_NULL)
    {
        return IP_NULL;
    }

    org_value_size = value_size ? *value_size : 0;

    /* First try user configuration. */
    if (username && *username)
    {
        ipcom_snprintf(buf, sizeof(buf), "ipcom.users.%s.%s", username, name);
        ret = ipcom_sysvar_get(buf, value, value_size);
        if (ret)
            return ret;
        if (value_size && *value_size > org_value_size)
            return IP_NULL;
    }

    /* Second try interface configuration. */
    if (ifname && *ifname)
    {
        ipcom_snprintf(buf, sizeof(buf), "ipcom.if.%s.%s", ifname, name);
        ret = ipcom_sysvar_get(buf, value, value_size);
        if (ret)
            return ret;
        if (value_size && *value_size > org_value_size)
            return IP_NULL;
    }

    /* Third try global configuration. */
    return ipcom_sysvar_get(name, value, value_size);
}


/*
 *===========================================================================
 *                    ipcom_sysvar_set_conf
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_PUBLIC Ip_err
ipcom_sysvar_set_conf(const char *name, const char *value, int flags,
                      const char *username, const char *ifname)
{
    char buf[256];

    if (username && *username)
    {
        ipcom_snprintf(buf, sizeof(buf), "ipcom.users.%s.%s", username, name);
        return ipcom_sysvar_set(buf, value, flags);
    }

    if (ifname && *ifname)
    {
        ipcom_snprintf(buf, sizeof(buf), "ipcom.if.%s.%s", ifname, name);
        return ipcom_sysvar_set(buf, value, flags);
    }

    return ipcom_sysvar_set(name, value, flags);
}


/*
 *===========================================================================
 *                    ipcom_sysvar_get_conf_as_int
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:      Integer config value stored under 'name' or -1 if not found.
 */
IP_PUBLIC int
ipcom_sysvar_get_conf_as_int(const char *name,
                             const char *username, const char *ifname)
{
    char strval[16];
    Ip_size_t strval_len = sizeof(strval);

    if (ipcom_sysvar_get_conf(name, strval, &strval_len,
                              username, ifname) == IP_NULL)
        return -1;

    return ipcom_atoi(strval);
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

