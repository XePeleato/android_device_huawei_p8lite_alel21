/*
 ******************************************************************************
 *                     INTERPEAK EXAMPLE SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipftps_example.c,v $ $Revision: 1.2 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/ipftps_example.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *   Design and implementation by Ulf Olofsson <ulf@interpeak.se>
 ******************************************************************************
 */

/*
modification history
--------------------
01d,02oct07,uol  Remove default ftp/interpeak username/password.
01c,03jan07,kch  Updated CVS revision log entry for Jan 03 code drop.
01b,28dec06,tkf  Remove debug statements.
01a,11dec06,kch  Integrated to VxWorks build infrastructure.
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

#include "ipftps_config.h"

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef IPCOM_USE_CLIB_PROTO
#undef IPCOM_USE_CLIB_PROTO
#endif

#if 0
#include <ipcom.h>
#include <ipcom_config.h>
#include <ipcom_os.h>
#include <ipcom_auth.h>
#endif

#include "ipftps.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#if 0
/* Syslog facility code */
#ifdef IPCOM_SYSLOG_FACILITY
#undef IPCOM_SYSLOG_FACILITY
#endif
#define IPCOM_SYSLOG_FACILITY IPCOM_LOG_IPFTP
#endif

/* typedef to make compiler happy... */
typedef int (*ipftps_cb)(Ipftps_session *session, char *password);


/*****************************************************************************
 *
 * ipftps_authenticate_default_callback - default ftps callback routine
 *
 * This is an example implementation of the ftp server authentication
 * callback routine. It is called if no ftp server callback routine is
 * assigned to the macro FTPS_AUTH_CALLBACK_HOOK.
 *
 * NOMANUAL
 */
_WRS_INLINE
int ipftps_authenticate_default_callback(Ipftps_session *session,
                                                   char *password)
{
    /*
    ** This is a simple authentication skeleton,
    ** please modify to suit your purposes.
    */
    if (strcmp(session->username, "anonymous") == 0)
    {
        /* IPCOM_LOG1(INFO, "Anonymous login with password %s", password); */

        /* Access restrictions apply for anonymous users */
        session->readonly = 1;
    }
#ifdef IPFTPS_USE_TEST
    else if (strcmp(session->username, "ftp") == 0 &&
             strcmp(password, "interpeak") == 0)
    {
        /* IPCOM_LOG1(INFO, "Hard-coded 'ftp' login with password %s", 
                      password); */

    }
#endif
#ifdef INCLUDE_IPCOM_USE_AUTH
    else if (ipcom_auth_login(session->username, password,
                              IP_NULL, IP_NULL, IP_NULL) == IPCOM_SUCCESS)
    {
        /* IPCOM_LOG1(INFO, "User '%s' login", session->username); */
    }
#endif
    else
    {
        /* IPCOM_LOG2(WARNING, "Login attempt with user: %s, pw: %s",
                   session->username, password); */
        return 1;
    }

#if 0

    /*
    ** Set security parameters individually for the session if the
    ** default values defined as sysvars aren't appropriate:
    **
    ** readonly:  Write commands disallowed.
    ** proxy:     Allow that data and control connections
    **            use different IP addresses.
    ** lportbase: Use local ports above this value.
    **            Set to 0 to use port 20.
    ** pportbase: Refuse to connect to peer ports <= this value.
    **            Set to 65535 to entirely disable PORT and EPRT commands.
    ** root:      Root directory for the session.
    */

    session->readonly = 1;      /* Readonly mode */
    session->proxy = 0;         /* Disable proxy FTP */
    session->lportbase = 49151; /* Use local ports from lportbase+1 */
    session->pportbase = 1023;  /* Reject peer ports <= pportbase */

    /* Limit access to the filesystem */
    ipftps_free(session->root);
    if ((session->root = ipcom_strdup("/pub")) == IP_NULL)
    {
        /* IPCOM_LOG(ERR, "Out of memory") IPCOM_END; */
        return 1;
    }

#endif

    /* IPCOM_LOG1(INFO, "User %s logged in", session->username); */

    return 0;    
}

#if FTPS_INSTALL_CALLBACK_HOOK == TRUE

/*
 * forward declarations for the user-defined ftp server authentication
 * callback hook to satisfy the compiler.
 */
int FTPS_AUTH_CALLBACK_HOOK (Ipftps_session *, char *);

#else

/*
 * FTPS_AUTH_CALLBACK_HOOK is applicable only if FTPS_INSTALL_CALLBACK_HOOK
 * is set to TRUE. Explicitly re-define to NULL so that we don't get
 * compiler errors.
 */
#endif /* FTPS_INSTALL_CALLBACK_HOOK */

/*****************************************************************************
 * 
 * ipftps_authenticate - ftp server authentication callback routine
 *
 * The FTP sessions may be configured individually by setting session options.
 * This is done after the authentication is complete, and enables clients to
 * be given permissions according to general policies, local databases or
 * even a RADIUS request to a remote host. The options are normally set in
 * the ipftps_authenticate() routine and are accessed through the session
 * pointer. 
 *
 * .IP <session>
 * Session descriptor.
 * .IP <password>
 * Password.
 *
 * RETURNS:
 * 0 for successful authentication, 1 otherwise.
 *
 * NOMANUAL
 */
IP_PUBLIC int
ipftps_authenticate(Ipftps_session *session, char *password)
{
    ipftps_cb ftps_cb;

    ftps_cb = (FTPS_AUTH_CALLBACK_HOOK == NULL) ?
              ipftps_authenticate_default_callback : FTPS_AUTH_CALLBACK_HOOK;

    return ftps_cb(session, password);
}
