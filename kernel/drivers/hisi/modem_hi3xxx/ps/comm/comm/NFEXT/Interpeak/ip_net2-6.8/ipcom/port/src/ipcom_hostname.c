/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_hostname.c,v $ $Revision: 1.9 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_hostname.c,v $
 *     $Author: lob $ $Date: 2004-11-02 12:35:33 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Lennart Bang <lob@interpeak.se>
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
#include "ipcom_sock2.h"
#include "ipcom_sysvar.h"


/*
 ****************************************************************************
 * 4                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 5                    DEFINES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 6                    TYPES
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
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_gethostname
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_GETHOSTNAME) && IPCOM_GETHOSTNAME == 1
IP_PUBLIC int
ipcom_gethostname(char *name, Ip_size_t len)
{
    char *sysvar;

    ip_assert2(name);

    if (len <= 1)
        return ipcom_errno_set(IP_ERRNO_EINVAL);
    if (name == IP_NULL)
        return ipcom_errno_set(IP_ERRNO_EFAULT);

    sysvar = ipcom_sysvar_get("ipcom.hostname", IP_NULL, IP_NULL);
    if (sysvar == IP_NULL)
        *name = '\0';
    else
    {
        ipcom_strncpy(name, sysvar, len - 1);
        name[len-1] = '\0';
        ipcom_free(sysvar);
    }

    return 0;
}
#endif


/*
 *===========================================================================
 *                    ipcom_sethostname
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_SETHOSTNAME) && IPCOM_SETHOSTNAME == 1
IP_PUBLIC int
ipcom_sethostname(const char *name, Ip_size_t len)
{
    ip_assert2(name);

    if (len <= 1 || len >= IP_MAXHOSTNAMELEN)
        return ipcom_errno_set(IP_ERRNO_EINVAL);
    if (name == IP_NULL)
        return ipcom_errno_set(IP_ERRNO_EFAULT);

    if (*name)
        (void)ipcom_sysvar_set("ipcom.hostname", name, IPCOM_SYSVAR_FLAG_OVERWRITE);
    else
        (void)ipcom_sysvar_unset("ipcom.hostname");

    return 0;
}
#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
