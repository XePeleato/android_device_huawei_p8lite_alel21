/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_err_string.c,v $ $Revision: 1.8 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_err_string.c,v $
 *   $Author: lob $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bång <lob@interpeak.se>
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
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_err_string
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC char *
ipcom_err_string(Ip_err err)
{
    static char errname[20];

    /* IPCOM errors */
    switch(err)
    {
        /* success/failure */
    case IPCOM_SUCCESS:
        return "success";

#ifndef IP_SIZE
    case IPCOM_ERR_FAILED:
        return "failed";

        /* general failures */
    case IPCOM_ERR_NO_MEMORY:
        return "no memory";
    case IPCOM_ERR_NOT_FOUND:
        return "not found";
    case IPCOM_ERR_NOT_SUPPORTED:
        return "not supported";
    case IPCOM_ERR_PERMISSION_DENIED:
        return "permission denied";

        /* 'already' failures */
    case IPCOM_ERR_ALREADY_OPEN:
        return "already open";
    case IPCOM_ERR_ALREADY_CLOSED:
        return "already closed";
    case IPCOM_ERR_ALREADY_INSTALLED:
        return "already installed";
    case IPCOM_ERR_ALREADY_STARTED:
        return "already started";

        /* 'not' failures */
    case IPCOM_ERR_NOT_OPENED:
        return "not opened";
    case IPCOM_ERR_NOT_CLOSED:
        return "not closed";
    case IPCOM_ERR_NOT_INSTALLED:
        return "not installed";
    case IPCOM_ERR_NOT_STARTED:
        return "not started";

        /* 'invalid' failures */
    case IPCOM_ERR_INVALID_ARG:
        return "invalid argument";
    case IPCOM_ERR_INVALID_CONFIG:
        return "invalid configuration";
    case IPCOM_ERR_INVALID_VERSION:
        return "invalid version";
    case IPCOM_ERR_INVALID_ALIGNMENT:
        return "invalid alignment";
    case IPCOM_ERR_INVALID_INTERFACE:
        return "invalid interface";

        /* route failures */
    case IPCOM_ERR_NO_ROUTE:
        return "no route";
    case IPCOM_ERR_DEAD_ROUTE:
        return "dead route";
    case IPCOM_ERR_INVALID_ROUTE:
        return "invalid route";
    case IPCOM_ERR_INVALID_MASK:
        return "invalid mask";
    case IPCOM_ERR_INVALID_RTCLIENT:
        return "invalid rtclient";

        /* entries failures */
    case IPCOM_ERR_DUPLICATE:
        return "duplicate entry";
    case IPCOM_ERR_LOCKED:
        return "locked";
    case IPCOM_ERR_END:
        return "end of operation";
    case IPCOM_ERR_READONLY:
        return "readonly entry";

        /* tag failures */
    case IPCOM_ERR_INVALID_TAG_VALUE:
        return "invalid tag value";
    case IPCOM_ERR_UNSUPPORTED_TAG:
        return "unsupported tag";

        /* packet failures */
    case IPCOM_ERR_INVALID_PACKET:
        return "invalid packet";
    case IPCOM_ERR_BAD_PACKET:
        return "bad packet";
    case IPCOM_ERR_IGNORED_PACKET:
        return "ignored packet";

        /* BSD failures */
    case IPCOM_ERR_SOCKCALLFAILED:
        return "socket call failed";

        /* 'min/max' failures */
    case IPCOM_ERR_REACHED_MIN:
        return "reached minimum";
    case IPCOM_ERR_REACHED_MAX:
        return "reached maximum";

        /* system call failures */
    case IPCOM_ERR_TIMEOUT:
        return "timeout";
    case IPCOM_ERR_WOULDBLOCK:
        return "would block";
    case IPCOM_ERR_INTERRUPT:
        return "operation interrupted";
    case IPCOM_ERR_PENDING:
        return "pending";

        /* authentication failures */
    case IPCOM_ERR_AUTH_FAILED:
        return "auth failed";
    case IPCOM_ERR_AUTH_UNKNOWN:
        return "unknown auth user";
#endif /* ifndef IP_SIZE */

    default:
        ipcom_sprintf(errname, "error %d", (int)err);
        return (char *)errname;
    }
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

