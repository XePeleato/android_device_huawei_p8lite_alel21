/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipftps.h,v $ $Revision: 1.7 $
 *   $Source: /home/interpeak/CVSRoot/ipappl/include/ipftps.h,v $
 *   $Author: gzadoyan $ $Date: 2009-09-01 13:56:48 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Ulf Olofsson <ulf@interpeak.se>
 ******************************************************************************
 */
#ifndef IPFTPS_H
#define IPFTPS_H

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
#include "ipcom_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         release
 *===========================================================================
 */

#define IPFTPS_RELEASE 060004

/* Transport protocol for FTPS */
#ifdef IPFTPS_USE_SCTP
#define IPFTPS_PROTO IP_IPPROTO_SCTP
#else
#define IPFTPS_PROTO 0
#endif

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/* dummy, see below for real struct */
struct Ipftps_session;

/* Socket control block */
typedef struct
{
    union Ip_sockaddr_union paddr;      /* Peer address */
    union Ip_sockaddr_union laddr;      /* Local address */
    Ip_fd fd;                           /* File descriptor */
}
Ipftps_sockcb;

/* Command descriptor */
typedef struct
{
    const char *name;                   /* Command name */
    void (*func)(struct Ipftps_session *, int argc, char *argv[]);
                                        /* Command handler */
    Ip_u8 min;                          /* Min no of arguments */
    Ip_u8 max;                          /* Max no of arguments */
    Ip_u8 argtype;                      /* Command argument. 1 if string */        
    Ip_bool login;                      /* Requires login */
    const char *help;                   /* Help text */
}
Ipftps_cmd;

/* Session descriptor */
typedef struct Ipftps_session
{
    Ipcom_list head;                    /* List header */
    Ipftps_sockcb csock;                /* Control socket */
    Ipftps_sockcb dsock;                /* Data socket */
    const Ipftps_cmd *last;             /* Last command */
    char *username;                     /* Username */
    char *rnfr;                         /* Rename from this name */
    char *root;                         /* Directory root */
    char *cwd;                          /* Current working directory */
    Ip_u32 trnow;                       /* Transferred bytes*/
    Ip_u32 trtotal;                     /* Total bytes */
    Ip_u16 pportbase;                   /* Reject peer ports <= pportbase */
    Ip_u16 lportbase;                   /* Use local ports from lportbase+1 */
    Ip_bool occupied;                   /* Session has a handler */
    Ip_bool login;                      /* Login performed */
    Ip_bool cwdroot;                    /* Use root as next CWD */
    Ip_bool epsvonly;                   /* Use only extended passive mode */
    Ip_bool passive;                    /* Passive mode */
    Ip_bool binary;                     /* Binary mode */
    Ip_bool readonly;                   /* Readonly mode */
    Ip_bool proxy;                      /* Enable proxy FTP */
    int authentications;                /* Number of authentications */
    int cmdlen;                         /* Length of current command */
    int cmdbuflen;                      /* Length of all commands */
    char cmdbuf[IPFTPS_CMDBUFSIZE + 1]; /* Command buffer */
    char name[20];                      /* Session name */
}
Ipftps_session;


/*
 ****************************************************************************
 * 6                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *      ipftps_authenticate
 *===========================================================================
 * Description:
 *      Authenticate user.
 *
 * Parameters:
 *      session         Session descriptor.
 *      password        Password.
 *
 * Returns:
 *      0 if successful authentication, 1 otherwise.
 */
IP_PUBLIC int
ipftps_authenticate(Ipftps_session *session, char *password);


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
