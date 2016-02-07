/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_access.c,v $ $Revision: 1.12 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_access.c,v $
 *   $Author: lob $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by FirstName LastName <email@interpeak.se>
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

#if IPCOM_USE_FILE != IPCOM_FILE_NONE

#if IPCOM_USE_FILE == IPCOM_FILE_NATIVE && \
 (defined(IP_PORT_OSE) || defined(IP_PORT_OSE5) || defined(IP_PORT_WIN32) || defined(IP_PORT_UNIX))
#define IPCOM_USE_FILE_ACCESS
#endif


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipcom_file.h"

#ifdef IPCOM_USE_FILE_ACCESS
#include <sys/types.h>
#include <sys/stat.h>
#ifdef IP_PORT_WIN32
#include "io.h"
#else
#include "unistd.h"
#endif
#else
#define IPCOM_USE_CLIB_PROTO
#endif

#include "ipcom_clib.h"


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

#if defined(IPCOM_USE_FILE_ACCESS) && defined(IP_PORT_UNIX)
/* Has to add per-task cwd to all paths */
IP_PUBLIC char *ipcom_pfile_cwd(const char *path);
IP_PUBLIC void ipcom_pfile_free(void *buf);
#endif


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
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_access
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_access(const char *path, int amode)
{
#ifdef IPCOM_USE_FILE_ACCESS
#if defined(IP_PORT_UNIX)
    char *p;
    int ret;

    ret = access(p = ipcom_pfile_cwd(path), amode);
    ipcom_pfile_free(p);

    return ret;

#elif defined (IP_PORT_WIN32)
    return _access(path, amode);

#else
    return access(path, amode);
#endif

#else
    (void)path;
    (void)amode;
    return 0;  /* access ok. */
#endif /* IPCOM_USE_FILE_ACCESS */
}


/*
 *===========================================================================
 *                    ipcom_umask
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_mode_t
ipcom_umask(Ip_mode_t cmask)
{
#ifdef IPCOM_USE_FILE_ACCESS
#ifdef IP_PORT_WIN32
    return (Ip_mode_t)_umask((int)cmask);
#else
    return (Ip_mode_t)umask((mode_t)cmask);
#endif
#else
    return cmask;
#endif
}


/*
 *===========================================================================
 *                    ipcom_chmod
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_chmod(const char *path, Ip_mode_t mode)
{
#ifdef IPCOM_USE_FILE_ACCESS
#if defined(IP_PORT_UNIX)
    char *p;
    int ret;

    ret = chmod(p = ipcom_pfile_cwd(path), mode);
    ipcom_pfile_free(p);

    return ret;

#elif defined(IP_PORT_WIN32)
    return _chmod(path, (int)mode);

#else
    return chmod(path, (mode_t)mode);
#endif

#else
    (void)path;
    (void)mode;
    return 0;
#endif /* IPCOM_USE_FILE_ACCESS */
}

#else
int ipcom_access_empty_file;
#endif /* IPCOM_USE_FILE == IPCOM_FILE_NATIVE */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

